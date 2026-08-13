//
// Created by 74222 on 2026/8/12.
//

#ifndef YOUJIESUN_MEASURE_STANDARD_H
#define YOUJIESUN_MEASURE_STANDARD_H
#include <stdbool.h>
#include <stdint.h>
#define INTERNATIONAL_TEST_STANDARD_TOTAL_CNT        (16)
#define CUSTOM_TEST_STANDARD_TOTAL_CNT               (2)
#define TEST_STANDARD_TOTAL_CNT                 (INTERNATIONAL_TEST_STANDARD_TOTAL_CNT + CUSTOM_TEST_STANDARD_TOTAL_CNT)

typedef enum
{
    FULL_TEMPLATE = 0,
    NO_DC_TEMPLATE,
    NO_AC_TEMPLATE,
    SPECIAL_TEMPLATE
} test_standard_type;

typedef struct
{
    char standard_name[24];
    test_standard_type template; // 测试标准模板
    uint16_t fill_num; // 填充次数 1，2
    int32_t temperature; // 介损设定温度  测试温度
    uint16_t ac_voltage; // 介损设定电压  交流电压
    uint16_t frequency; // 介损设定频率  测试频率
    uint16_t dc_voltage; // 体积电阻设定电压 直流电压
} MeasureStandard;

typedef struct
{
    /* data */
    uint16_t selectedStandardTemplateIndex; // 所选标准测试模板编号
    test_standard_type template; // 测试标准模板
    uint16_t fill_num; // 填充次数 1，2
    int32_t temperature; // 介损设定温度  测试温度
    uint16_t ac_voltage; // 介损设定电压  交流电压
    uint16_t frequency; // 介损设定频率  测试频率
    uint16_t dc_voltage; // 体积电阻设定电压 直流电压
} CustomStandard;

extern MeasureStandard international_test_standard_list[INTERNATIONAL_TEST_STANDARD_TOTAL_CNT];
extern MeasureStandard custom_test_standard_list[CUSTOM_TEST_STANDARD_TOTAL_CNT];
extern MeasureStandard selected_test_standard_list[TEST_STANDARD_TOTAL_CNT];
extern bool test_standard_select_state[INTERNATIONAL_TEST_STANDARD_TOTAL_CNT + CUSTOM_TEST_STANDARD_TOTAL_CNT];
extern uint16_t test_standard_select_count;

// 更新选中测试列表
void update_selected_measure_standard_list(void);
#endif //YOUJIESUN_MEASURE_STANDARD_H
