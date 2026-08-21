//
// Created by 74222 on 2026/7/22.
//

#ifndef YOUJIESUN_SYSTEM_STATE_H
#define YOUJIESUN_SYSTEM_STATE_H
#include <stdint.h>

typedef struct
{
    uint16_t lid_state; // 0: 闭合  1: 打开
    uint16_t pour_state; // 0: 默认  1: 排油中
    uint16_t oil_cup_state; // 0: 正常  1: 异常
    float oil_cup_temperature; // 油杯温度
} DeviceState_t;

extern DeviceState_t device_state;
extern uint16_t current_test_state_g;
#endif //YOUJIESUN_SYSTEM_STATE_H
