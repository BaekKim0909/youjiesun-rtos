//
// Created by 74222 on 2026/8/10.
//
#include "fpga_comm.h"
#include "test_task.h"

#include <string.h>

#include "communicate_task.h"
#include "main.h"
#include "FreeRTOS.h"
#include "timers.h"
#include "task.h"
#include "queue.h"
#include "system_state.h"
#include "ui_task.h"


typedef enum
{
    TEST_STATE_IDLE = 0,
    TEST_STATE_START_REQUEST_RECEIVED, // 收到测试请求
    TEST_STATE_WAIT_PARAM_RESPONSE, // 等待FPGA确认收到请求参数
    TEST_STATE_PARAM_CONFIRM, // FPGA确认收到参数
    TEST_STATE_WAIT_HEATING_RESPONSE, // 等待FPGA确认升温指令
    TEST_STATE_HEATING, // 加热阶段
    TEST_STATE_HEATING_TEMPERATURE_ACHIEVED, // 温度达到预设值
    TEST_STATE_WAIT_FIRST_DIELECTRIC_LOSS_TEST_RESPONSE, // 等待FPGA确认第一次介损测试指令
    TEST_STATE_FIRST_DIELECTRIC_LOSS_TEST, // 第一次介损测试
    TEST_STATE_WAIT_HALF_OUTCOME, // 等待两次填充记录中第一次填充结果
    TEST_STATE_WAIT_ONE_FILL_OUTCOME, // 等待一次填充测试结果
    TEST_STATE_WAIT_TWO_FILL_OUTCOME, // 等待两次填充测试结果
    TEST_STATE_WAIT_STOP_RESPONSE, // 等待FPGA确认停止指令
    TEST_STATE_COMM_FAULT, // 通信故障，无法确认设备已停止
    TEST_STATE_START_FAIL, // 测试启动失败

    TEST_STATE_FIRST_AC,
    TEST_STATE_FIRST_DC,
    TEST_STATE_SECOND_AC,
    TEST_STATE_SECOND_DC
} test_state_enum;


// 保存测试请求上下文
typedef struct
{
    test_state_enum test_state; // 测试状态
    test_request_t test_request; // 测试请求的数据
    uint16_t current_fill_round; // 当前填充轮次
    uint32_t awaiting_fpga_response_id; // 当前正在等待响应的FPGA请求编号
} test_context_t;

/* ---------------------------------VARIABLE----------------------------------------*/
// 测试事件队列
static test_context_t test_context = {
    .test_state = TEST_STATE_IDLE,
    .test_request = {},
    .current_fill_round = 1,
    .awaiting_fpga_response_id = FPGA_REQUEST_ID_NONE,
};

static uint32_t next_fpga_request_id = 1U;

// 分配一个非零的MCU内部请求编号
static uint32_t test_allocate_fpga_request_id(void);

// 提交升温请求
static bool test_submit_heating_request(void);

// 提交第一次介损测试请求
static bool test_submit_first_dielectric_loss_test(void);

// 提交读取单次填充测试结果指令
static bool test_submit_read_test_outcome(void);

// 提交停止请求并进入相应等待或故障状态
static void test_submit_stop_request(void);

// 判断响应是否属于当前等待的写事务
static bool test_is_expected_fpga_response(const fpga_response_t *response);

// 读取油杯温度
static void read_oil_cup_temperature(void);

// 读取fpga状态
static void read_fpga_state(void);

// 读取fpga剩余测试事件
static void read_remain_test_time(void);

bool test_request_start(const test_request_t *request)
{
    if (request == NULL ||
        test_event_queue == NULL)
    {
        return false;
    }
    const test_event_t event = {
        .event_type = TEST_EVENT_START_TEST_REQUEST,
        .event_data.test_request = *request
    };
    return xQueueSend(test_event_queue, &event, 0U) == pdPASS;
}

bool test_report_fpga_response(const fpga_response_t *response)
{
    if (response == NULL ||
        test_event_queue == NULL)
    {
        return false;
    }

    const test_event_t event = {
        .event_type = TEST_EVENT_FPGA_RESPONSE,
        .event_data.fpga_response = *response
    };

    return xQueueSend(test_event_queue, &event, 0U) == pdPASS;
}

void start_test_task(void *argument)
{
    test_event_t event;
    xTimerStart(fpga_communication_timer, pdMS_TO_TICKS(1000U));
    for (;;)
    {
        if (xQueueReceive(test_event_queue, &event,portMAX_DELAY) != pdPASS)
            continue;
        switch (event.event_type)
        {
            case TEST_EVENT_START_TEST_REQUEST:
            {
                if (test_context.test_state != TEST_STATE_IDLE)
                    break;
                // 更新测试状态
                test_context.test_request = event.event_data.test_request;
                test_context.test_state = TEST_STATE_START_REQUEST_RECEIVED;
                test_context.current_fill_round = 1;
                // 包装FPGA通讯请求
                fpga_request_t fpga_request = {
                    .request_id = test_allocate_fpga_request_id(),
                    .operation = FPGA_OPERATION_WRITE_TEST_PARAMS,
                    .request_data.test_params = event.event_data.test_request.params
                };
                // 将请求发送到通讯任务队列中
                if (!communicate_submit_request(&fpga_request))
                {
                    // 发送失败
                    test_context.test_state = TEST_STATE_IDLE;
                    break;
                }
                test_context.awaiting_fpga_response_id = fpga_request.request_id;
                test_context.test_state = TEST_STATE_WAIT_PARAM_RESPONSE;
                break;
            }
            case TEST_EVENT_FPGA_RESPONSE:
            {
                const fpga_response_t *fpga_response = &event.event_data.fpga_response;

                if (!test_is_expected_fpga_response(fpga_response))
                {
                    break;
                }

                // 当前等待的FPGA事务已经结束
                test_context.awaiting_fpga_response_id = FPGA_REQUEST_ID_NONE;

                switch (test_context.test_state)
                {
                    case TEST_STATE_WAIT_PARAM_RESPONSE:
                    {
                        // 写入测试参数成功
                        if (fpga_response->response_status == FPGA_RESPONSE_SUCCESS)
                        {
                            test_context.test_state = TEST_STATE_PARAM_CONFIRM;
                            // 下发升温指令
                            bool result = test_submit_heating_request();
                            device_state.current_step_state = 0;
                            if (!result)
                            {
                                test_context.test_state = TEST_STATE_COMM_FAULT;
                                test_submit_stop_request();
                            }
                        }
                        else
                        {
                            test_context.test_state = TEST_STATE_START_FAIL;
                            (void) ui_notice_post(UI_NOTICE_COMM_ERROR);
                            test_context.test_state = TEST_STATE_IDLE;
                        }
                        break;
                    }

                    case TEST_STATE_WAIT_HEATING_RESPONSE:
                        if (fpga_response->response_status == FPGA_RESPONSE_SUCCESS)
                        {
                            // 加热指令写入成功
                            device_state.current_step_state = 0;
                            test_context.test_state = TEST_STATE_HEATING;
                            ui_event_t ui_event =
                            {
                                .event_type = UI_EVENT_LOAD_HEAT_PAGE,
                                .event_data.page_params = {
                                    .fill_num = test_context.test_request.params.fill_num,
                                    .rho_param = test_context.test_request.params.rho_param,
                                    .template = test_context.test_request.standard_type
                                }
                            };
                            strncpy(ui_event.event_data.page_params.standard_name,
                                    test_context.test_request.standard_name,
                                    sizeof(test_context.test_request.standard_name) - 1U);
                            ui_event.event_data.page_params.standard_name[
                                sizeof(test_context.test_request.standard_name) - 1U] = '\0';

                            ui_submit_request(&ui_event);
                        }
                        else
                        {
                            test_context.test_state = TEST_STATE_COMM_FAULT;
                            test_submit_stop_request();
                        }
                        break;
                    case TEST_STATE_WAIT_FIRST_DIELECTRIC_LOSS_TEST_RESPONSE:
                        if (fpga_response->response_status == FPGA_RESPONSE_SUCCESS)
                        {
                            // 第一次介损测试指令写入成功
                            test_context.test_state = TEST_STATE_FIRST_DIELECTRIC_LOSS_TEST;
                            ui_event_t ui_event =
                            {
                                .event_type = UI_EVENT_LOAD_DIELECTRIC_LOSS_TEST_PAGE,
                                .event_data.page_params = {
                                    .fill_num = test_context.test_request.params.fill_num,
                                    .rho_param = test_context.test_request.params.rho_param,
                                    .template = test_context.test_request.standard_type,
                                    .ac_voltage = test_context.test_request.params.ac_voltage
                                }
                            };
                            strncpy(ui_event.event_data.page_params.standard_name,
                                    test_context.test_request.standard_name,
                                    sizeof(test_context.test_request.standard_name) - 1U);
                            ui_event.event_data.page_params.standard_name[
                                sizeof(test_context.test_request.standard_name) - 1U] = '\0';


                            ui_submit_request(&ui_event);
                        }
                        else
                        {
                        }
                        break;
                    case TEST_STATE_WAIT_STOP_RESPONSE:
                    {
                        if (fpga_response->response_status == FPGA_RESPONSE_SUCCESS)
                        {
                            (void) ui_notice_post(UI_NOTICE_COMM_ERROR);
                            test_context.test_state = TEST_STATE_IDLE;
                        }
                        else
                        {
                            (void) ui_notice_post(UI_NOTICE_STOP_FAILED);
                            test_context.test_state = TEST_STATE_COMM_FAULT;
                        }
                        break;
                    }

                    default:
                        break;
                }
                break;
            }

            default:
                break;
        }
    }
}


static uint32_t test_allocate_fpga_request_id(void)
{
    const uint32_t request_id = next_fpga_request_id++;

    if (next_fpga_request_id == FPGA_REQUEST_ID_NONE)
    {
        next_fpga_request_id = 1U;
    }

    return request_id;
}

static bool test_submit_heating_request(void)
{
    const fpga_request_t request = {
        .request_id = test_allocate_fpga_request_id(),
        .operation = FPGA_OPERATION_WRITE_REGISTER,
        .request_data.write_register = {
            .register_address = TEST_CONTROL_REG,
            .register_value = 0x0001U
        }
    };

    if (!communicate_submit_request(&request))
    {
        return false;
    }

    test_context.awaiting_fpga_response_id = request.request_id;
    test_context.test_state = TEST_STATE_WAIT_HEATING_RESPONSE;
    return true;
}

static bool test_submit_first_dielectric_loss_test(void)
{
    const fpga_request_t request =
    {
        .request_id = test_allocate_fpga_request_id(),
        .operation = FPGA_OPERATION_WRITE_REGISTER,
        .request_data.write_register = {
            .register_address = TEST_CONTROL_REG,
            .register_value = 0x0002U
        }
    };
    if (!communicate_submit_request(&request))
    {
        return false;
    }
    test_context.awaiting_fpga_response_id = request.request_id;
    test_context.test_state = TEST_STATE_WAIT_FIRST_DIELECTRIC_LOSS_TEST_RESPONSE;
    return true;
}

// 提交读取测试结果指令
static bool test_submit_read_test_outcome(void)
{
    const fpga_request_t request =
    {
        .request_id = test_allocate_fpga_request_id(),
        .operation = FPGA_OPERATION_READ_OUTCOME,
        .request_data.read_instruction = {
            .reg_num = 0x001C,
            .start_address = PERMITTIVITY_OUTCOME_REG
        }
    };
    if (!communicate_submit_request(&request))
    {
        return false;
    }
    test_context.awaiting_fpga_response_id = request.request_id;
    // 只填充一次，结果
    if (test_context.test_request.params.fill_num == 1)
    {
        // 等待单次填充测试结果
        test_context.test_state = TEST_STATE_WAIT_ONE_FILL_OUTCOME;
    }
    else if (test_context.test_request.params.fill_num == 2)
    {
        if (test_context.current_fill_round == 1)
        {
            test_context.test_state = TEST_STATE_WAIT_HALF_OUTCOME;
        }
        else if (test_context.current_fill_round == 2)
        {
            test_context.test_state = TEST_STATE_WAIT_TWO_FILL_OUTCOME;
        }
    }
    return true;
}

static void test_submit_stop_request(void)
{
    const fpga_request_t request = {
        .request_id = test_allocate_fpga_request_id(),
        .operation = FPGA_OPERATION_WRITE_REGISTER,
        .request_data.write_register = {
            .register_address = TEST_CONTROL_REG,
            .register_value = 0x00F0U
        }
    };

    if (!communicate_submit_request(&request))
    {
        (void) ui_notice_post(UI_NOTICE_STOP_FAILED);
        test_context.test_state = TEST_STATE_COMM_FAULT;
        return;
    }

    test_context.awaiting_fpga_response_id = request.request_id;
    test_context.test_state = TEST_STATE_WAIT_STOP_RESPONSE;
}

static bool test_is_expected_fpga_response(const fpga_response_t *response)
{
    if (response == NULL ||
        response->request_id != test_context.awaiting_fpga_response_id)
    {
        return false;
    }

    switch (test_context.test_state)
    {
        case TEST_STATE_WAIT_PARAM_RESPONSE:
            return response->operation == FPGA_OPERATION_WRITE_TEST_PARAMS;

        case TEST_STATE_WAIT_HEATING_RESPONSE:
        case TEST_STATE_WAIT_STOP_RESPONSE:
        case TEST_STATE_WAIT_FIRST_DIELECTRIC_LOSS_TEST_RESPONSE:
            return response->operation == FPGA_OPERATION_WRITE_REGISTER;

        default:
            return false;
    }
}

void fpga_communication_timer_cb(TimerHandle_t xTimer)
{
    (void) xTimer;

    switch (test_context.test_state)
    {
        case TEST_STATE_IDLE:
        case TEST_STATE_HEATING:
            read_oil_cup_temperature();
            break;
        case TEST_STATE_HEATING_TEMPERATURE_ACHIEVED:
            read_fpga_state();
            break;
        case TEST_STATE_FIRST_DIELECTRIC_LOSS_TEST:
            read_remain_test_time();
            break;
        default:
            break;
    }
}

static void read_oil_cup_temperature(void)
{
    if (test_context.test_state == TEST_STATE_HEATING && device_state.oil_cup_temperature >= (float) test_context.
        test_request.params.temperature)
    {
        test_context.test_state = TEST_STATE_HEATING_TEMPERATURE_ACHIEVED;
        return;
    }
    read_instruction_t read_instruction = {
        .start_address = TEMPERATURE_REG,
        .reg_num = 0x0002
    };
    const fpga_request_t request = {
        .request_id = FPGA_REQUEST_ID_NONE,
        .operation = FPGA_OPERATION_READ_REGISTERS,
        .request_data.read_instruction = read_instruction
    };

    communicate_submit_request(&request);
}

static void read_fpga_state(void)
{
    if (device_state.current_step_state != 0x0002)
    {
        const read_instruction_t read_instruction = {
            .start_address = TEST_STATE_REG,
            .reg_num = 0x0001
        };
        const fpga_request_t request = {
            .request_id = FPGA_REQUEST_ID_NONE,
            .operation = FPGA_OPERATION_READ_REGISTERS,
            .request_data.read_instruction = read_instruction
        };
        communicate_submit_request(&request);
    }
    else
    {
        // 当前测试步骤测试完成
        switch (test_context.test_state)
        {
            case TEST_STATE_HEATING_TEMPERATURE_ACHIEVED:
                if (test_context.test_request.standard_type == FULL_TEMPLATE || test_context.test_request.standard_type
                    == NO_DC_TEMPLATE)
                {
                    if (test_context.current_fill_round == 1)
                    {
                        // 发送开启第一次介损测量请求
                        bool result = test_submit_first_dielectric_loss_test();
                        device_state.current_step_state = 0;
                        device_state.remain_test_time = 30; // 测试时间初始化为30
                    }
                    else if (test_context.current_fill_round == 2)
                    {
                    }
                }
                break;
            case TEST_STATE_FIRST_DIELECTRIC_LOSS_TEST:
                // 不测体积电阻率
                if (test_context.test_request.params.rho_param == 0)
                {
                    // 读取测试结果
                    test_submit_read_test_outcome();
                }
            default:
                break;
        }
    }
}

// 读取fpga剩余测试事件
static void read_remain_test_time(void)
{
    if (device_state.remain_test_time != 0)
    {
        const read_instruction_t read_instruction = {
            .start_address = TEST_REMAIN_TIME_REG,
            .reg_num = 0x0001
        };
        const fpga_request_t request = {
            .request_id = FPGA_REQUEST_ID_NONE,
            .operation = FPGA_OPERATION_READ_REGISTERS,
            .request_data.read_instruction = read_instruction
        };
        communicate_submit_request(&request);
    }
    else
    {
        read_fpga_state();
    }
}
