//
// Created by 74222 on 2026/8/10.
//

#include "../Inc/translations.h"

#include <stddef.h>
const char *const languages[] = {"CN", "EN",NULL};
const char *const tags[] =
{
    "test_standard",
    "test_electrode",
    "sample",
    "rho",
    "please_enter_id",
    "start_test",
    "select_standard_tip",
    "close_lid_tip",
    "check_oil_cup_tip",
    "pouring_tip",
    "check_temperature_prob",
    "communicate_error",
    "heating_start_failed",
    "stop_test_failed",
    "start_test_error",
    "stop_tip_message",
    "progress_1/3",
    "progress_1/2",
    "heat",
    NULL
};
const char *const translations[] = {
    "测试标准", "Test Standard",
    "测试电极", "Electrode",
    "样品ID", "Sample ID",
    "体积电阻率", "Rho",
    "请输入样品ID", "Enter Sample ID",
    "开始测试", "Start Test",
    "请选择测试标准!", "Please select the test standard!",
    "请关闭盖子后重试", "Please close the lid and try again.",
    "请检查油杯后重试", "Please check the oil cup and try again later",
    "排油中......", "draining oil......",
    "请检查温度探针", "Please check the temperature probe.",
    "通信异常", "Communication error",
    "升温启动失败，测试已停止，请重试", "Heating failed to start. The test has stopped. Please try again.",
    "停止测试失败，请检查设备", "Failed to stop the test. Please check the device.",
    "开始测试失败!", "Start error!",
    "⚠测试中,按下任意键停止", "⚠Testing... Press any key to stop",
    "测试进度: 1/3", "STEP: 1/3",
    "测试进度: 1/2", "STEP: 1/2",
    "加热", "Heating",
};
