//
// Created by 74222 on 2026/8/12.
//

#ifndef YOUJIESUN_TEST_STRUCTS_H
#define YOUJIESUN_TEST_STRUCTS_H
#include <stdint.h>
#include "test_standard.h"

typedef struct
{
    uint16_t start_address;
    uint16_t reg_num;
} read_instruction_t;

typedef struct
{
    float empty_cell_capacitance; // 空杯电容值
    uint16_t fill_num; // 填充次数
    int32_t temperature; // 目标温度
    uint16_t ac_voltage; //交流电压
    uint16_t frequency; //频率
    uint16_t dc_voltage; // 直流电压
    uint16_t rho_param; // 体积电阻率选择 0:不测 1:rho+ 2:rho+ & rho-
} test_params_t;

typedef struct
{
    test_params_t params; //  指令参数
    test_standard_type standard_type; // 测试类型
    char standard_name[24]; // 测试标准
} test_request_t;


#define ELECTRODE_COUNT     (8)

typedef struct
{
    char electrode_name[2]; // 电极名称
    uint16_t electrode_id; // 编号
    float capacitance; //   空杯电容值
} electrode_t;

extern electrode_t electrode_list[ELECTRODE_COUNT];
#endif //YOUJIESUN_TEST_STRUCTS_H
