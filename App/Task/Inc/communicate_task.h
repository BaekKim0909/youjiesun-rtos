//
// Created by 74222 on 2026/8/14.
//

#ifndef YOUJIESUN_COMMUNICATE_TASK_H
#define YOUJIESUN_COMMUNICATE_TASK_H
#include <stdbool.h>
#include "fpga_comm.h"

/**
 * @brief 向CommunicateTask提交一条FPGA通讯请求
 *
 * 请求通过FreeRTOS队列按值复制，调用结束后调用者可以释放局部变量。
 *
 * @param request 待提交的通讯请求
 * @return true 请求已经成功进入通讯队列
 * @return false 参数无效、队列未初始化或队列已满
 */
bool communicate_submit_request(const fpga_request_t *request);
#endif //YOUJIESUN_COMMUNICATE_TASK_H
