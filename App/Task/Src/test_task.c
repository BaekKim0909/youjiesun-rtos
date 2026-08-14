//
// Created by 74222 on 2026/8/10.
//
#include "fpga_comm.h"
#include "test_task.h"
#include "communicate_task.h"
#include "main.h"
#include "FreeRTOS.h"
#include "timers.h"
#include "task.h"
#include "queue.h"


typedef enum
{
    TEST_STATE_IDLE = 0,
    TEST_STATE_START_REQUEST_RECEIVED, // 收到测试请求
    TEST_STATE_WAIT_PARAM_RESPONSE, // 等待FPGA确认收到请求参数
    TEST_STATE_PARAM_CONFIRM, // FPGA确认收到参数
    TEST_STATE_START_FAIL, // 测试启动失败
    TEST_STATE_HEATING,
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
    uint32_t pending_fpga_request_id; // 当前正在等待响应的FPGA请求编号
} test_context_t;

/* ---------------------------------VARIABLE----------------------------------------*/
// 测试事件队列
static test_context_t test_context = {
    .test_state = TEST_STATE_IDLE,
    .test_request = {}
};
static uint32_t next_fpga_request_id = 1U;

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
    xTimerStart(read_temperature_timer, pdMS_TO_TICKS(1000U));
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
                test_context.test_request = event.event_data.test_request;
                test_context.test_state = TEST_STATE_START_REQUEST_RECEIVED;
                fpga_request_t fpga_request = {
                    .request_id = next_fpga_request_id++,
                    .operation = FPGA_OPERATION_WRITE_TEST_PARAMS,
                    .request_data.test_params = event.event_data.test_request.params
                };
                // 将请求发送到通讯任务队列中
                if (!communicate_submit_request(&fpga_request))
                {
                    test_context.test_state = TEST_STATE_IDLE;
                    break;
                }
                test_context.pending_fpga_request_id = fpga_request.request_id;
                test_context.test_state = TEST_STATE_WAIT_PARAM_RESPONSE;
                break;
            }
            case TEST_EVENT_FPGA_RESPONSE:
            {
                const fpga_response_t *fpga_response = &event.event_data.fpga_response;
                /*
                * 只处理当前参数写入事务对应的响应。
                * 其他迟到或无关响应不能推进测试状态。
                */
                if (test_context.test_state != TEST_STATE_WAIT_PARAM_RESPONSE ||
                    fpga_response->request_id != test_context.pending_fpga_request_id ||
                    fpga_response->operation != FPGA_OPERATION_WRITE_TEST_PARAMS)
                {
                    break;
                }
                if (fpga_response->response_status == FPGA_RESPONSE_SUCCESS)
                {
                    test_context.test_state = TEST_STATE_PARAM_CONFIRM;
                }
                else
                {
                    test_context.test_state = TEST_STATE_START_FAIL;
                }
            }
            default:
                break;
        }
    }
}


void read_fpga_temperature_timer_cb(TimerHandle_t xTimer)
{
    read_instruction_t read_instruction = {
        .start_address = TEMPERATURE_REG,
        .reg_num = 0x0002
    };
    fpga_request_t request = {
        .request_id = 0,
        .operation = FPGA_OPERATION_READ_REGISTERS,
        .request_data.read_instruction = read_instruction
    };

    communicate_submit_request(&request);
}
