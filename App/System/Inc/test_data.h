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


// 测试记录结构体
typedef struct
{
    uint8_t year;
    uint8_t month;
    uint8_t date;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;

    char sample_name[52]; // 样品名称
    char standard_name[24]; // 测试标准名称
    uint8_t fill_num; // 填充次数
    uint8_t rho_param; // 体积电阻率：0:不测试 1:Rho+ 2:Rho+和Rho-
    uint8_t frequency; // 介损设定频率  测试频率
    char electrode_name[2]; // 电极名称
    float capacitance; // 空杯电容值
    test_standard_type standard_template; // 测试标准模板

    float permittivity_1; // 介电常数1
    float dielectric_loss_1; // 介损值1
    float rho_pos_1; // rho+
    float rho_neg_1; // rho-
    float temperature_1; // 第一次填充温度
    float ac_voltage_1; // 第一次填充交流电压
    float dc_voltage_1; // 第一次填充直流电压

    float permittivity_2; // 介电常数2
    float dielectric_loss_2; // 介损值2
    float rho_pos_2; // rho+
    float rho_neg_2; // rho-
    float temperature_2; // 第二次填充温度
    float ac_voltage_2; // 第二次填充交流电压
    float dc_voltage_2; // 第二次填充直流电压
} test_record_t;

// 最新测试结果
extern test_record_t latest_test_record;

void init_test_record();
#endif //YOUJIESUN_TEST_STRUCTS_H
