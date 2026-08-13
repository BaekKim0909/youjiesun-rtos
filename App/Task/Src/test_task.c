//
// Created by 74222 on 2026/8/10.
//
#include "fpga_comm.h"
#include "test_task.h"
#include "main.h"
#include "FreeRTOS.h"
#include "timers.h"
#include "task.h"
#include "queue.h"


typedef enum
{
    TEST_STATE_IDLE = 0,
    TEST_STATE_START_REQUEST_RECEIVED, // 收到测试请求
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
} test_context_t;

/* ---------------------------------VARIABLE----------------------------------------*/
// 测试事件队列
static test_context_t test_context = {
    .test_state = TEST_STATE_IDLE,
    .test_request = {}
};


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
    return xQueueSend(
        test_event_queue,
        &event,
        0U) == pdPASS;
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
                if (test_context.test_state != TEST_STATE_IDLE)
                    break;
                test_context.test_request = event.event_data.test_request;
                test_context.test_state = TEST_STATE_START_REQUEST_RECEIVED;
                break;
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
    xQueueSend(fpga_tx_read_instruction_queue, &read_instruction, 0);
    xTaskNotify(communicate_taskHandle, FPGA_TX_EVENT, eSetBits);
}
