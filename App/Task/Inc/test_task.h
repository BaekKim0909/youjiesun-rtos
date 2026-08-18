//
// Created by 74222 on 2026/8/13.
//

#ifndef YOUJIESUN_TEST_TASK_H
#define YOUJIESUN_TEST_TASK_H
#include <stdbool.h>
#include "../../App/System/Inc/test_data.h"
#include "fpga_comm.h"

// 测试事件类型
typedef enum
{
    TEST_EVENT_START_TEST_REQUEST = 0,
    TEST_EVENT_FPGA_RESPONSE,
} test_event_enum;

// 队列中传递的数据
typedef struct
{
    test_event_enum event_type; /* 测试事件类型 */

    union
    {
        test_request_t test_request; /* UI任务提交开始测试事件携带的请求参数 */
        fpga_response_t fpga_response; /* 通讯任务返回的响应 */
    } event_data/* 当前事件携带的数据 */;
} test_event_t;


bool test_request_start(const test_request_t *request);

bool test_report_fpga_response(const fpga_response_t *response);
#endif //YOUJIESUN_TEST_TASK_H
