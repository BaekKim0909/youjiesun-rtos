//
// Created by 74222 on 2026/7/15.
//

#ifndef YOUJIESUN_SYSTEM_STRUCTS_H
#define YOUJIESUN_SYSTEM_STRUCTS_H
#include <stdint.h>

typedef struct
{
    uint16_t lid_state;         // 0: 闭合  1: 打开
    uint16_t pour_state;        // 0: 默认  1: 排油中
    uint16_t oil_cup_state;     // 0: 正常  1: 异常
}DeviceState_t;

typedef struct
{
    uint8_t language;   // 0:中文    1:英文
    uint8_t time_format; // 时间格式  0:年月日 1:月日年
    uint8_t brightness; // 屏幕亮度  1:25%; 2:50%; 3:75%; 4:100%
}Setting_t;
#endif //YOUJIESUN_SYSTEM_STRUCTS_H
