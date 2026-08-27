//
// Created by 74222 on 2026/7/22.
//

#ifndef YOUJIESUN_SYSTEM_STATE_H
#define YOUJIESUN_SYSTEM_STATE_H
#include <stdint.h>

typedef struct
{
    uint32_t lid_state; // 0: 闭合  1: 打开
    uint32_t pour_state; // 0: 默认  1: 排油中
    uint32_t oil_cup_state; // 0: 正常  1: 异常
    float oil_cup_temperature; // 油杯温度
    uint32_t current_step_state; //当前测试步骤的状态
    uint32_t remain_test_time; // 剩余测试时间
} DeviceState_t;

extern DeviceState_t device_state;
#endif //YOUJIESUN_SYSTEM_STATE_H
