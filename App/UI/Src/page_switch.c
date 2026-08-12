//
// Created by 74222 on 2026/8/11.
//

#include "../Inc/page_switch.h"
#include "../Inc/mainUI.h"
extern lv_indev_t *indev_keypad;
extern lv_group_t *navigation_group;
static bool navigation_bar_is_focused = false;

void handle_return_page(void)
{
    if (current_page_index_g == START_MEASURE_PAGE || current_page_index_g == DATA_HANDLE_PAGE || current_page_index_g
        == TEST_STANDARD_PAGE || current_page_index_g == ELECTRODE_PAGE || current_page_index_g == SETTING_PAGE ||
        current_page_index_g == INFO_PAGE)
    {
        // 设置组到导航栏组
        lv_indev_set_group(indev_keypad, navigation_group);
        // 更新状态 导航栏选中
        navigation_bar_is_focused = true;

        if (current_page_index_g == START_MEASURE_PAGE)
        {
            lv_group_focus_obj(lv_group_get_obj_by_index(navigation_group, 0));
        }
        else if (current_page_index_g == DATA_HANDLE_PAGE)
        {
        }
        else if (current_page_index_g == TEST_STANDARD_PAGE)
        {
            lv_group_focus_obj(lv_group_get_obj_by_index(navigation_group, 2));
        }
    }
}
