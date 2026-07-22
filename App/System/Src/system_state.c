//
// Created by 74222 on 2026/7/22.
//
#include "system_state.h"

// 设备状态
DeviceState_t device_state = {
    .lid_state = 0,
    .oil_cup_state = 0,
    .pour_state = 0
};
// 当前油温
float current_temperature_g = 0.0f;
