//
// Created by 74222 on 2026/8/12.
//

#include "../Inc/test_standard.h"

#include <stdbool.h>
#include <string.h>

// 测试标准列表
MeasureStandard international_test_standard_list[INTERNATIONAL_TEST_STANDARD_TOTAL_CNT] = {
    {"IEC60247:2004S", FULL_TEMPLATE, 2, 90, 2000, 55, 500}, //
    {"IEC60247:2004R", FULL_TEMPLATE, 1, 90, 2000, 55, 500},
    {"VDE 0380-2:2015_1S", FULL_TEMPLATE, 2, 90, 2000, 55, 500},
    {"VDE 0380-2:2015_1R", FULL_TEMPLATE, 1, 90, 2000, 55, 500},
    {"BS 5737:1979S", FULL_TEMPLATE, 2, 90, 2000, 55, 500},
    {"BS 5737:1979R", FULL_TEMPLATE, 1, 90, 2000, 55, 500},
    {"ASTM D924-08S", NO_DC_TEMPLATE, 2, 100, 2000, 60, 0},
    {"ASTM D924-08R", NO_DC_TEMPLATE, 1, 100, 2000, 60, 0},
    {"ASTM D1169:2002S", NO_AC_TEMPLATE, 2, 100, 0, 0, 500},
    {"ASTM D1169:2002R", NO_AC_TEMPLATE, 1, 100, 0, 0, 500},
    {"IEC 61620:1998-11", FULL_TEMPLATE, 1, 90, 2000, 55, 500},
    {"JIS C2101:1999", FULL_TEMPLATE, 2, 80, 2000, 55, 500},
    {"IS 6262:1971S", NO_DC_TEMPLATE, 2, 90, 2000, 55, 0},
    {"IS 6262:1971R", NO_DC_TEMPLATE, 1, 90, 2000, 55, 0},
    {"IS 6103:1971S", NO_AC_TEMPLATE, 2, 90, 0, 0, 500},
    {"IS 6103:1971R", NO_AC_TEMPLATE, 1, 90, 0, 0, 500},
};
// 自定义测试列表
MeasureStandard custom_test_standard_list[CUSTOM_TEST_STANDARD_TOTAL_CNT] = {0};
// 已选择的测试列表
MeasureStandard selected_test_standard_list[TEST_STANDARD_TOTAL_CNT] = {0};
// 选中标准数量
uint16_t test_standard_select_count = 0;
// 记录选中状态
bool test_standard_select_state[TEST_STANDARD_TOTAL_CNT] = {0};
// 更新选中测试列表
void update_selected_measure_standard_list(void)
{
    // 初始化选中
    memset(selected_test_standard_list, 0, sizeof(MeasureStandard) * TEST_STANDARD_TOTAL_CNT);
    test_standard_select_count = 0;
    // 遍历所有标准
    for (uint8_t i = 0; i < TEST_STANDARD_TOTAL_CNT; i++)
    {
        if (test_standard_select_state[i]) // 如果第 i 项被选中
        {
            // 复制到选中列表
            if (i < INTERNATIONAL_TEST_STANDARD_TOTAL_CNT)
            {
                memcpy(&selected_test_standard_list[test_standard_select_count],
                       &international_test_standard_list[i],
                       sizeof(MeasureStandard));
            }
            else
            {
                memcpy(&selected_test_standard_list[test_standard_select_count],
                       &custom_test_standard_list[i - INTERNATIONAL_TEST_STANDARD_TOTAL_CNT],
                       sizeof(MeasureStandard));
            }


            test_standard_select_count++;
        }
    }
}
