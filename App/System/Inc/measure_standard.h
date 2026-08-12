//
// Created by 74222 on 2026/8/12.
//

#ifndef YOUJIESUN_MEASURE_STANDARD_H
#define YOUJIESUN_MEASURE_STANDARD_H
#include <stdint.h>
#define MEASURE_STANDARD_TOTAL_CNT 18

typedef enum
{
    FULL_TEMPLATE = 0,
    NO_DC_TEMPLATE,
    NO_AC_TEMPLATE,
    SPECIAL_TEMPLATE
} MeasureStandardClass;

typedef struct
{
    char StandardName[24];
    MeasureStandardClass template;  // 测试标准模板
    uint16_t fillNum;               // 填充次数 1，2
    uint32_t temperature;           // 介损设定温度  测试温度
    uint16_t ACVoltage;             // 介损设定电压  交流电压
    uint16_t frequency;             // 介损设定频率  测试频率
    uint16_t DCVoltage;             // 体积电阻设定电压 直流电压
}MeasureStandard;

typedef struct
{
    /* data */
    uint16_t selectedStandardTemplateIndex; // 所选标准测试模板编号
    MeasureStandardClass template;          // 测试标准模板
    uint16_t fillNum;                       // 填充次数 1，2
    uint16_t temperature;                   // 介损设定温度  测试温度
    uint16_t ACVoltage;                     // 介损设定电压  交流电压
    uint16_t frequency;                     // 介损设定频率  测试频率
    uint16_t DCVoltage;                     // 体积电阻设定电压 直流电压
} CustomStandard;

extern MeasureStandard MeasureStandardList[MEASURE_STANDARD_TOTAL_CNT];

#endif //YOUJIESUN_MEASURE_STANDARD_H
