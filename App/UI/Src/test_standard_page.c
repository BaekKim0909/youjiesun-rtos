//
// Created by 74222 on 2026/8/12.
//

#include "../Inc/test_standard_page.h"
#include "lvgl.h"
#include "mainUI.h"
#include "test_standard.h"

LV_FONT_DECLARE(chinese_character_20);

// 为表格绘制复选框
static void draw_check_box_cb(lv_event_t *e);

// 处理选中回调
static void checked_table_item_cb(lv_event_t *e);

// 处理表格聚焦回调
static void table_focus_cb(lv_event_t *e);

void load_test_standard_page(void)
{
    current_page_index_g = TEST_STANDARD_PAGE;

    lv_obj_t *container = container_get();

    lv_obj_t *international_measure_standard_table = lv_table_create(container);
    // lv_obj_t *custom_measure_standard_table = lv_table_create(container);


    lv_obj_set_size(international_measure_standard_table, 312, 360);
    lv_obj_set_pos(international_measure_standard_table, 20, 20);
    lv_table_set_row_count(international_measure_standard_table, INTERNATIONAL_TEST_STANDARD_TOTAL_CNT);
    lv_table_set_column_count(international_measure_standard_table, 1);
    lv_table_set_column_width(international_measure_standard_table, 0, 312);
    lv_obj_set_style_pad_top(international_measure_standard_table, 8, LV_PART_ITEMS);
    lv_obj_set_style_pad_bottom(international_measure_standard_table, 8, LV_PART_ITEMS);
    lv_obj_set_style_text_font(international_measure_standard_table, &chinese_character_20, LV_PART_ITEMS);
    lv_obj_set_style_text_align(international_measure_standard_table, LV_TEXT_ALIGN_CENTER, LV_PART_ITEMS);
    lv_obj_set_style_outline_width(international_measure_standard_table, 0, LV_PART_MAIN | LV_STATE_FOCUS_KEY);

    lv_obj_set_style_border_color(international_measure_standard_table, lv_color_hex(0xFFFFFF), LV_PART_ITEMS);
    lv_obj_set_style_border_side(international_measure_standard_table,
                                 LV_BORDER_SIDE_RIGHT | LV_BORDER_SIDE_BOTTOM | LV_BORDER_SIDE_LEFT |
                                 LV_BORDER_SIDE_TOP, LV_PART_ITEMS);
    lv_obj_set_style_border_width(international_measure_standard_table, 0, LV_PART_MAIN);
    lv_obj_set_style_bg_color(international_measure_standard_table, lv_color_hex(0x123867),
                              LV_STATE_DEFAULT | LV_PART_ITEMS);
    lv_obj_set_style_bg_color(international_measure_standard_table, lv_color_hex(0x123867),
                              LV_STATE_DEFAULT | LV_PART_MAIN);
    lv_obj_set_style_text_align(international_measure_standard_table, LV_TEXT_ALIGN_CENTER,
                                LV_STATE_DEFAULT | LV_PART_ITEMS);
    lv_obj_set_style_text_color(international_measure_standard_table, lv_color_hex(0xFFFFFF), LV_PART_ITEMS);
    lv_obj_set_style_bg_color(international_measure_standard_table, lv_color_hex(0xFFFFFF),
                              LV_PART_SCROLLBAR | LV_STATE_SCROLLED);
    lv_obj_set_scroll_snap_y(international_measure_standard_table, LV_SCROLL_SNAP_END);

    for (uint8_t i = 0; i < INTERNATIONAL_TEST_STANDARD_TOTAL_CNT; i++)
    {
        /* code */
        lv_table_set_cell_value(international_measure_standard_table, i, 0,
                                international_test_standard_list[i].StandardName);
    }

    lv_obj_set_scrollbar_mode(international_measure_standard_table, LV_SCROLLBAR_MODE_AUTO);
    lv_obj_add_event_cb(international_measure_standard_table, draw_check_box_cb, LV_EVENT_DRAW_TASK_ADDED,
                        (void *) 1);
    lv_obj_add_event_cb(international_measure_standard_table, checked_table_item_cb, LV_EVENT_KEY, NULL);
    lv_obj_add_event_cb(international_measure_standard_table, table_focus_cb, LV_EVENT_FOCUSED, NULL);
    lv_obj_add_flag(international_measure_standard_table, LV_OBJ_FLAG_SEND_DRAW_TASK_EVENTS);
    lv_obj_add_flag(international_measure_standard_table, LV_OBJ_FLAG_SCROLL_ON_FOCUS);


    // lv_obj_add_event_cb(custom_measure_standard_table, draw_check_box_cb, LV_EVENT_DRAW_TASK_ADDED, (void *) 2);

    lv_group_remove_all_objs(test_standard_page_group);
    lv_group_add_obj(test_standard_page_group, international_measure_standard_table);

    lv_obj_send_event(international_measure_standard_table, LV_EVENT_DEFOCUSED, NULL);
}

static void draw_check_box_cb(lv_event_t *e)
{
    uint32_t flag = (uint32_t) lv_event_get_user_data(e);
    lv_draw_task_t *draw_task = lv_event_get_draw_task(e);
    lv_draw_dsc_base_t *base_dsc = lv_draw_task_get_draw_dsc(draw_task);

    /* 只在单元格绘制阶段画复选框 */
    if (base_dsc->part == LV_PART_ITEMS &&
        lv_draw_task_get_type(draw_task) == LV_DRAW_TASK_TYPE_FILL &&
        base_dsc->id2 == 0)
    {
        uint32_t row = base_dsc->id1;

        /* 判断是左表还是右表，计算状态索引 */
        uint32_t index_offset = (flag == 2) ? INTERNATIONAL_TEST_STANDARD_TOTAL_CNT : 0;
        uint32_t idx = row + index_offset;

        /* 复选框区域 */
        lv_area_t cb_area;
        cb_area.x1 = 0;
        cb_area.x2 = 20;
        cb_area.y1 = 0;
        cb_area.y2 = 20;

        lv_area_t cell_area;
        lv_draw_task_get_area(draw_task, &cell_area);
        lv_area_align(&cell_area, &cb_area, LV_ALIGN_LEFT_MID, 15, 0);
        /* 画复选框边框 */
        lv_draw_rect_dsc_t rect_dsc;
        lv_draw_rect_dsc_init(&rect_dsc);
        rect_dsc.border_width = 1;
        rect_dsc.border_color = lv_color_hex(0x2196f3);
        rect_dsc.border_opa = LV_OPA_COVER;
        rect_dsc.bg_color = lv_color_white();
        rect_dsc.bg_opa = LV_OPA_COVER;
        rect_dsc.radius = 2;
        lv_draw_rect(base_dsc->layer, &rect_dsc, &cb_area);

        /* 如果选中，画勾 */
        if (test_standard_select_state[idx])
        {
            lv_draw_line_dsc_t line_dsc;
            lv_draw_line_dsc_init(&line_dsc);
            line_dsc.color = lv_color_black();
            line_dsc.width = 2;
            line_dsc.opa = LV_OPA_COVER;

            /* 第一条线：左下到中点 */
            line_dsc.p1.x = cb_area.x1 + 4;
            line_dsc.p1.y = cb_area.y2 - 6;
            line_dsc.p2.x = cb_area.x1 + 8;
            line_dsc.p2.y = cb_area.y2 - 3;
            lv_draw_line(base_dsc->layer, &line_dsc);

            /* 第二条线：中点到右上 */
            line_dsc.p1.x = cb_area.x1 + 8;
            line_dsc.p1.y = cb_area.y2 - 3;
            line_dsc.p2.x = cb_area.x2 - 4;
            line_dsc.p2.y = cb_area.y1 + 4;
            lv_draw_line(base_dsc->layer, &line_dsc);
        }
    }
}

static void checked_table_item_cb(lv_event_t *e)
{
    lv_obj_t *table = lv_event_get_target_obj(e);
    uint32_t flag = (uint32_t) lv_event_get_user_data(e);
    if (lv_event_get_code(e) == LV_EVENT_KEY)
    {
        uint32_t key = lv_event_get_key(e);
        if (key == LV_KEY_ENTER)
        {
            uint32_t row, col;
            lv_table_get_selected_cell(table, &row, &col);
            uint32_t index_offset = (flag == 2) ? INTERNATIONAL_TEST_STANDARD_TOTAL_CNT : 0;
            uint32_t idx = row + index_offset;
            test_standard_select_state[idx] = !test_standard_select_state[idx];

            // 更新选中标准数组
            update_selected_measure_standard_list();
            // SaveSelectedStandard(); 保存选中状态到flash
            lv_obj_invalidate(table);
        }
    }
}

// 处理表格聚焦回调
static void table_focus_cb(lv_event_t *e)
{
    lv_obj_t *table = lv_event_get_target_obj(e);

    lv_table_set_selected_cell(table, 0, 0);
}
