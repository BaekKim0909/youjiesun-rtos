//
// Created by 74222 on 2026/7/15.
//

#include "../Inc/test_page.h"
#include "test_standard.h"
#include <string.h>
#include "mainUI.h"
#include "style_g.h"
#include "notice_message.h"
#include "system_state.h"
#include "test_data.h"
#include "test_task.h"
#include "image.h"
LV_IMAGE_DECLARE(measure_standard_image);
LV_IMAGE_DECLARE(electrode_image);
LV_IMAGE_DECLARE(sample_image);
LV_IMAGE_DECLARE(resistance_image);

LV_FONT_DECLARE(lv_font_montserrat_16);
LV_FONT_DECLARE(chinese_character_20);

extern lv_indev_t *indev_keypad;

static void widgets_focus_cb(lv_event_t *e);

static void widgets_defocus_cb(lv_event_t *e);

// 开始测试回调函数
static void start_test_cb(lv_event_t *e);

void load_start_test_page(void)
{
    current_page_index_g = START_MEASURE_PAGE;

    lv_obj_t *container = container_get();

    lv_obj_t *measure_standard_img = lv_image_create(container);
    lv_obj_set_size(measure_standard_img, 72, 72);
    lv_obj_set_pos(measure_standard_img, 99, 50);
    lv_image_set_src(measure_standard_img, &measure_standard_image);

    lv_obj_t *electrode_img = lv_image_create(container);
    lv_obj_set_size(electrode_img, 72, 72);
    lv_obj_align_to(electrode_img, measure_standard_img, LV_ALIGN_OUT_RIGHT_MID, 131, 0);
    lv_image_set_src(electrode_img, &electrode_image);

    lv_obj_t *SampleImg = lv_image_create(container);
    lv_obj_set_size(SampleImg, 72, 72);
    lv_obj_align_to(SampleImg, electrode_img, LV_ALIGN_OUT_RIGHT_MID, 102, 0);
    lv_image_set_src(SampleImg, &sample_image);

    lv_obj_t *ResistanceImg = lv_image_create(container);
    lv_obj_set_size(ResistanceImg, 72, 72);
    lv_obj_align_to(ResistanceImg, SampleImg, LV_ALIGN_OUT_RIGHT_MID, 111, 0);
    lv_image_set_src(ResistanceImg, &resistance_image);

    lv_obj_t *label1 = lv_label_create(container);
    lv_obj_t *label2 = lv_label_create(container);
    lv_obj_t *label3 = lv_label_create(container);
    lv_obj_t *label4 = lv_label_create(container);

    lv_obj_set_style_text_font(label1, &chinese_character_20, LV_STATE_DEFAULT);
    lv_label_set_translation_tag(label1, "test_standard");
    lv_obj_set_style_text_font(label2, &chinese_character_20, LV_STATE_DEFAULT);
    lv_label_set_translation_tag(label2, "test_electrode");
    lv_obj_set_style_text_font(label3, &chinese_character_20, LV_STATE_DEFAULT);
    lv_label_set_translation_tag(label3, "sample");
    lv_obj_set_style_text_font(label4, &chinese_character_20, LV_STATE_DEFAULT);
    lv_label_set_translation_tag(label4, "rho");

    lv_obj_set_style_text_color(label1, lv_color_hex(0xF8F9FF), LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(label1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_align_to(label1, measure_standard_img, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

    lv_obj_set_style_text_color(label2, lv_color_hex(0xF8F9FF), LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(label2, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_align_to(label2, electrode_img, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

    lv_obj_set_style_text_color(label3, lv_color_hex(0xF8F9FF), LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(label3, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_align_to(label3, SampleImg, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

    lv_obj_set_style_text_color(label4, lv_color_hex(0xF8F9FF), LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(label4, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_align_to(label4, ResistanceImg, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);


    /* 测试标准下拉框 */
    lv_obj_t *measure_standard_dd_list = lv_dropdown_create(container);
    lv_obj_set_name(measure_standard_dd_list, "test_standard_dd");
    lv_obj_set_style_text_font(measure_standard_dd_list, &chinese_character_20, LV_PART_MAIN);
    lv_obj_set_style_text_font(measure_standard_dd_list, &lv_font_montserrat_16, LV_PART_INDICATOR);
    lv_obj_set_style_radius(measure_standard_dd_list, 0, LV_PART_MAIN);
    lv_obj_set_style_outline_width(measure_standard_dd_list, 0, LV_PART_MAIN | LV_STATE_FOCUS_KEY);

    lv_obj_t *standardList = lv_dropdown_get_list(measure_standard_dd_list);
    lv_obj_set_style_radius(standardList, 0, LV_PART_MAIN);
    lv_obj_set_style_text_font(standardList, &chinese_character_20, LV_PART_MAIN);
    lv_obj_set_style_max_height(standardList, 170, LV_PART_MAIN);
    lv_obj_set_style_max_width(standardList, 240, LV_PART_MAIN);
    lv_obj_set_style_pad_top(standardList, 8, LV_PART_MAIN);
    lv_obj_set_style_pad_left(standardList, 5, LV_PART_MAIN);

    lv_obj_set_style_pad_all(measure_standard_dd_list, 1, LV_PART_MAIN);
    lv_obj_set_size(measure_standard_dd_list, 240, 40);
    lv_obj_align_to(measure_standard_dd_list, label1, LV_ALIGN_OUT_BOTTOM_MID, 0, 21);

    lv_obj_add_event_cb(measure_standard_dd_list, widgets_focus_cb, LV_EVENT_FOCUSED, NULL);
    lv_obj_add_event_cb(measure_standard_dd_list, widgets_defocus_cb, LV_EVENT_DEFOCUSED, NULL);

    char dd_options[512]; // 存下拉框选项字符串
    dd_options[0] = '\0'; // 清空初始内容

    for (uint16_t i = 0; i < test_standard_select_count; i++)
    {
        // 如果不是第一个选中项，先加一个换行符
        if (strlen(dd_options) > 0)
        {
            strcat(dd_options, "\n");
        }
        // 追加选项名称
        strcat(dd_options, selected_test_standard_list[i].standard_name);
    }
    lv_dropdown_set_options(measure_standard_dd_list, dd_options);

    /* 测试电极下拉框 */
    lv_obj_t *electrode_dd_list = lv_dropdown_create(container);
    lv_obj_set_name(electrode_dd_list, "electrode_dd");
    lv_obj_set_size(electrode_dd_list, 140, 40);
    lv_obj_align_to(electrode_dd_list, label2, LV_ALIGN_OUT_BOTTOM_MID, 0, 21);
    lv_obj_set_style_radius(electrode_dd_list, 0, LV_PART_MAIN);
    lv_obj_set_style_text_font(electrode_dd_list, &chinese_character_20, LV_PART_MAIN);
    lv_obj_set_style_text_font(electrode_dd_list, &lv_font_montserrat_16, LV_PART_INDICATOR);
    lv_obj_set_style_outline_width(electrode_dd_list, 0, LV_PART_MAIN | LV_STATE_FOCUS_KEY);

    lv_dropdown_set_options(electrode_dd_list, "A\nB\nC\nD\nE\nF\nG\nH");

    lv_obj_add_event_cb(electrode_dd_list, widgets_focus_cb, LV_EVENT_FOCUSED, NULL);
    lv_obj_add_event_cb(electrode_dd_list, widgets_defocus_cb, LV_EVENT_DEFOCUSED, NULL);

    lv_obj_t *electrodeList = lv_dropdown_get_list(electrode_dd_list);
    lv_obj_set_style_radius(electrodeList, 0, LV_PART_MAIN);
    lv_obj_set_style_text_font(electrodeList, &chinese_character_20, LV_PART_MAIN);
    lv_obj_set_style_max_height(electrodeList, 170, LV_PART_MAIN);
    lv_obj_set_style_max_width(electrodeList, 100, LV_PART_MAIN);
    lv_obj_set_style_pad_top(electrodeList, 8, LV_PART_MAIN);
    lv_obj_set_style_pad_left(electrodeList, 5, LV_PART_MAIN);


    /* 样品ID输入框 相关控件 */
    lv_obj_t *sample_id_textArea = lv_textarea_create(container);
    lv_obj_set_name(sample_id_textArea, "id_textArea");
    lv_textarea_set_one_line(sample_id_textArea, true);
    lv_obj_set_style_radius(sample_id_textArea, 0, LV_PART_MAIN);
    lv_obj_set_style_text_font(sample_id_textArea, &chinese_character_20, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_set_style_text_align(sample_id_textArea, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_ANY);
    lv_obj_set_style_text_align(sample_id_textArea, LV_TEXT_ALIGN_CENTER, LV_PART_TEXTAREA_PLACEHOLDER | LV_STATE_ANY);

    lv_obj_set_style_text_font(sample_id_textArea, &chinese_character_20,
                               LV_PART_TEXTAREA_PLACEHOLDER | LV_STATE_DEFAULT);
    lv_textarea_set_placeholder_text(sample_id_textArea, lv_translation_get("please_enter_id"));

    lv_obj_set_style_pad_all(sample_id_textArea, 6, LV_PART_MAIN);
    lv_obj_set_size(sample_id_textArea, 170, 40);
    lv_obj_align_to(sample_id_textArea, label3, LV_ALIGN_OUT_BOTTOM_MID, 0, 21);
    lv_obj_set_style_text_align(sample_id_textArea, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_set_style_outline_width(sample_id_textArea, 0, LV_PART_MAIN | LV_STATE_FOCUS_KEY);
    lv_textarea_set_max_length(sample_id_textArea, 51);

    lv_obj_add_event_cb(sample_id_textArea, widgets_focus_cb, LV_EVENT_FOCUSED, NULL);
    lv_obj_add_event_cb(sample_id_textArea, widgets_defocus_cb, LV_EVENT_DEFOCUSED, NULL);

    /* 体积电阻率下拉框 */
    lv_obj_t *resistance_dd_list = lv_dropdown_create(container);
    lv_obj_set_name(resistance_dd_list, "rho_dd");
    lv_obj_set_size(resistance_dd_list, 170, 40);
    lv_obj_align_to(resistance_dd_list, label4, LV_ALIGN_OUT_BOTTOM_MID, 0, 21);
    lv_obj_set_style_radius(resistance_dd_list, 0, LV_PART_MAIN);
    lv_obj_set_style_text_font(resistance_dd_list, &chinese_character_20, LV_PART_MAIN);
    lv_obj_set_style_text_font(resistance_dd_list, &lv_font_montserrat_16, LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(resistance_dd_list, lv_color_hex(0x808080), LV_STATE_DISABLED | LV_PART_MAIN);
    lv_dropdown_set_options(resistance_dd_list, "NULL\nRho+\nRho+ & Rho-");
    lv_obj_set_style_outline_width(resistance_dd_list, 0, LV_PART_MAIN | LV_STATE_FOCUS_KEY);

    lv_obj_add_event_cb(resistance_dd_list, widgets_focus_cb, LV_EVENT_FOCUSED, NULL);
    lv_obj_add_event_cb(resistance_dd_list, widgets_defocus_cb, LV_EVENT_DEFOCUSED, NULL);

    lv_obj_t *resistance_list = lv_dropdown_get_list(resistance_dd_list);
    lv_obj_set_style_radius(resistance_list, 0, LV_PART_MAIN);
    lv_obj_set_style_text_font(resistance_list, &chinese_character_20, LV_PART_MAIN);
    lv_obj_set_style_max_height(resistance_list, 170, LV_PART_MAIN);
    lv_obj_set_style_max_width(resistance_list, 170, LV_PART_MAIN);
    lv_obj_set_style_pad_top(resistance_list, 8, LV_PART_MAIN);
    lv_obj_set_style_pad_left(resistance_list, 5, LV_PART_MAIN);

    /* 开始测试按钮 */
    lv_obj_t *start_measure_btn = lv_button_create(container);
    lv_obj_set_size(start_measure_btn, 300, 40);
    lv_obj_align(start_measure_btn, LV_ALIGN_BOTTOM_MID, 0, -60);
    lv_obj_set_style_border_width(start_measure_btn, 2, LV_PART_MAIN);
    lv_obj_set_style_border_color(start_measure_btn, lv_color_hex(0xF8F9FF), LV_PART_MAIN);
    lv_obj_set_style_radius(start_measure_btn, 20, LV_PART_MAIN);
    lv_obj_set_style_bg_color(start_measure_btn, lv_color_hex(0x123867), LV_PART_MAIN);
    lv_obj_set_style_outline_width(start_measure_btn, 0, LV_PART_MAIN | LV_STATE_FOCUS_KEY);
    lv_obj_add_style(start_measure_btn, &btn_focus_style_g, LV_PART_MAIN | LV_STATE_FOCUS_KEY);

    lv_obj_add_event_cb(start_measure_btn, start_test_cb, LV_EVENT_CLICKED,NULL);

    lv_obj_t *label5 = lv_label_create(start_measure_btn);
    lv_obj_set_style_text_font(label5, &chinese_character_20, LV_STATE_DEFAULT);
    lv_label_set_translation_tag(label5, "start_test");
    lv_obj_set_style_text_color(label5, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_center(label5);

    // 添加组对象
    lv_group_remove_all_objs(measure_page_group);
    lv_group_add_obj(measure_page_group, measure_standard_dd_list);
    lv_group_add_obj(measure_page_group, electrode_dd_list);
    lv_group_add_obj(measure_page_group, sample_id_textArea);
    lv_group_add_obj(measure_page_group, resistance_dd_list);
    lv_group_add_obj(measure_page_group, start_measure_btn);

    lv_obj_send_event(measure_standard_dd_list, LV_EVENT_DEFOCUSED,NULL);
}

static void start_test_cb(lv_event_t *e)
{
    if (test_standard_select_count <= 0)
    {
        notice_message_t notice_message =
        {
            .message = lv_translation_get("select_standard_tip"),
            .result_type = NOTICE_ERROR
        };
        show_notice_message_box(notice_message);
        return;
    }
    if (!check_device_state())
        return;
    if (device_state.oil_cup_temperature > 300.0f)
    {
        notice_message_t notice_message =
        {
            .message = lv_translation_get("check_temperature_prob"),
            .result_type = NOTICE_ERROR
        };
        show_notice_message_box(notice_message);
        return;
    }
    lv_obj_t *container = container_get();
    lv_obj_t *test_standard_dd = lv_obj_get_child_by_name(container, "test_standard_dd");
    lv_obj_t *electrode_dd = lv_obj_get_child_by_name(container, "electrode_dd");
    lv_obj_t *rho_dd = lv_obj_get_child_by_name(container, "rho_dd");

    uint32_t selected_standard_id = lv_dropdown_get_selected(test_standard_dd);
    uint32_t selected_electrode_id = lv_dropdown_get_selected(electrode_dd);
    uint16_t rho_param = lv_dropdown_get_selected(rho_dd);

    test_request_t test_request = {
        .params =
        {
            .empty_cell_capacitance = electrode_list[selected_electrode_id].capacitance,
            .fill_num = selected_test_standard_list[selected_standard_id].fill_num,
            .temperature = selected_test_standard_list[selected_standard_id].temperature,
            .ac_voltage = selected_test_standard_list[selected_standard_id].ac_voltage,
            .frequency = selected_test_standard_list[selected_standard_id].frequency,
            .dc_voltage = selected_test_standard_list[selected_standard_id].dc_voltage,
            .rho_param = rho_param
        },
        .standard_type = selected_test_standard_list[selected_standard_id].template
    };

    // 数组成员不能直接使用另一个数组赋值，需复制字符串内容。
    strncpy(test_request.standard_name,
            selected_test_standard_list[selected_standard_id].standard_name,
            sizeof(test_request.standard_name) - 1U);
    test_request.standard_name[sizeof(test_request.standard_name) - 1U] = '\0';

    if (!test_request_start(&test_request))
    {
        // 开始测试失败
        const notice_message_t notice_message =
        {
            .result_type = NOTICE_ERROR,
            .message = lv_translation_get("start_test_error")
        };
        show_notice_message_box(notice_message);
    }
}

static void widgets_focus_cb(lv_event_t *e)
{
    lv_obj_t *obj = lv_event_get_target_obj(e);
    if (lv_obj_check_type(obj, &lv_dropdown_class))
    {
        /* code */
        lv_obj_set_style_bg_color(obj, lv_color_hex(0x2D96FF), LV_PART_MAIN | LV_STATE_FOCUS_KEY);
        lv_obj_set_style_text_color(obj, lv_color_hex(0xF8F9FF), LV_PART_MAIN);
    }
    else if (lv_obj_check_type(obj, &lv_textarea_class))
    {
        /* code */
        lv_obj_set_style_bg_color(obj, lv_color_hex(0x2D96FF), LV_PART_MAIN | LV_STATE_FOCUS_KEY);
        lv_obj_set_style_text_color(obj, lv_color_hex(0xF8F9FF), LV_PART_MAIN);
    }
}

static void widgets_defocus_cb(lv_event_t *e)
{
    lv_obj_t *obj = lv_event_get_target_obj(e);
    if (lv_obj_check_type(obj, &lv_dropdown_class))
    {
        /* code */
        lv_obj_set_style_bg_color(obj, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_FOCUS_KEY);
        lv_obj_set_style_text_color(obj, lv_color_hex(0x000000), LV_PART_MAIN);
    }
    else if (lv_obj_check_type(obj, &lv_textarea_class))
    {
        /* code */
        lv_obj_set_style_bg_color(obj, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_FOCUS_KEY);
        lv_obj_set_style_text_color(obj, lv_color_hex(0x000000), LV_PART_MAIN);
    }
}

bool check_device_state(void)
{
    if (device_state.lid_state == 1)
    {
        const notice_message_t notice_message = {
            .message = lv_translation_get("close_lid_tip"),
            .result_type = NOTICE_ERROR
        };
        show_notice_message_box(notice_message);
        return false;
    }
    if (device_state.pour_state == 1)
    {
        const notice_message_t notice_message = {
            .message = lv_translation_get("check_oil_cup_tip"),
            .result_type = NOTICE_ERROR
        };
        show_notice_message_box(notice_message);
        return false;
    }
    if (device_state.oil_cup_state == 1)
    {
        const notice_message_t notice_message = {
            .message = lv_translation_get("pouring_tip"),
            .result_type = NOTICE_ERROR
        };
        show_notice_message_box(notice_message);
        return false;
    }
    return true;
}
