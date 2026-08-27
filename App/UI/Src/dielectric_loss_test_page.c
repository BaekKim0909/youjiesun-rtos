//
// Created by 74222 on 2026/8/27.
//

#include "dielectric_loss_test_page.h"
#include "mainUI.h"
#include "system_state.h"


LV_FONT_DECLARE(chinese_character_20)
LV_FONT_DECLARE(chinese_character_24)
LV_FONT_DECLARE(chinese_character_72)


lv_timer_t *dielectric_loss_test_timer = NULL;

static void update_remain_test_time_cb(lv_timer_t *timer);

void load_dielectric_loss_test_page(const char *standard_name, const test_standard_type standard_type,
                                    const uint16_t rho_param, const uint16_t ac_voltage)
{
    current_page_index_g = DIELECTRIC_LOSS_TEST_PAGE;

    lv_obj_t *container = container_get();

    lv_obj_t *test_standard_title_label = lv_label_create(container);
    lv_obj_set_style_text_font(test_standard_title_label, &chinese_character_20, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_label_set_translation_tag(test_standard_title_label, "test_standard");
    lv_obj_set_style_text_color(test_standard_title_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_pos(test_standard_title_label, 19, 20);


    lv_obj_t *test_standard_value_label = lv_label_create(container);
    lv_obj_set_style_text_font(test_standard_value_label, &chinese_character_20, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_label_set_text(test_standard_value_label, standard_name);
    lv_obj_set_style_text_color(test_standard_value_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_align_to(test_standard_value_label, test_standard_title_label, LV_ALIGN_OUT_RIGHT_MID, 5, 0);

    // 测试进度标签
    lv_obj_t *progress_label = lv_label_create(container);
    lv_obj_set_style_text_font(progress_label, &chinese_character_20, LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(progress_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);

    // 停止提示标签
    lv_obj_t *stop_tip_message_label = lv_label_create(container);
    lv_obj_set_style_text_font(stop_tip_message_label, &chinese_character_24, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(stop_tip_message_label, lv_color_hex(0xFF3346), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_label_set_translation_tag(stop_tip_message_label, "stop_tip_message");
    lv_obj_align(stop_tip_message_label, LV_ALIGN_TOP_RIGHT, -50, 20);

    if (standard_type == FULL_TEMPLATE && (rho_param == 1 || rho_param == 2))
    {
        lv_obj_t *step1_circle = lv_obj_create(container);
        lv_obj_t *step1_label = lv_label_create(container);
        lv_obj_set_size(step1_circle, 20, 20);
        lv_obj_set_pos(step1_circle, 100, 120);

        // 当前步骤已完成色：0x05D235
        lv_obj_set_style_bg_color(step1_circle, lv_color_hex(0x05D235), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_border_width(step1_circle, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_remove_flag(step1_circle, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_style_radius(step1_circle, 10, LV_PART_MAIN | LV_STATE_DEFAULT);

        lv_obj_set_style_text_font(step1_label, &lv_font_montserrat_24, LV_STATE_DEFAULT);
        lv_label_set_text(step1_label, "STEP-1");
        lv_obj_set_style_text_color(step1_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
        lv_obj_align_to(step1_label, step1_circle, LV_ALIGN_OUT_RIGHT_MID, 10, 0);

        lv_obj_t *step2_circle = lv_obj_create(container);
        lv_obj_t *step2_label = lv_label_create(container);
        lv_obj_set_size(step2_circle, 20, 20);
        // 当前步骤进行中颜色：0xFFA200
        lv_obj_set_style_bg_color(step2_circle, lv_color_hex(0xFFA200), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_border_width(step2_circle, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_remove_flag(step2_circle, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_style_radius(step2_circle, 10, LV_PART_MAIN | LV_STATE_DEFAULT);

        lv_obj_align_to(step2_circle, step1_circle, LV_ALIGN_OUT_RIGHT_MID, 220, 0);

        lv_obj_set_style_text_font(step2_label, &lv_font_montserrat_24, LV_STATE_DEFAULT);
        lv_label_set_text(step2_label, "STEP-2");
        lv_obj_set_style_text_color(step2_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
        lv_obj_align_to(step2_label, step2_circle, LV_ALIGN_OUT_RIGHT_MID, 10, 0);

        lv_obj_t *step3_circle = lv_obj_create(container);
        lv_obj_t *step3_label = lv_label_create(container);
        lv_obj_set_size(step3_circle, 20, 20);
        // 当前步骤未开始颜色：0xFFFFFF
        lv_obj_set_style_bg_color(step3_circle, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_border_width(step3_circle, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_remove_flag(step3_circle, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_style_radius(step3_circle, 10, LV_PART_MAIN | LV_STATE_DEFAULT);

        lv_obj_align_to(step3_circle, step2_circle, LV_ALIGN_OUT_RIGHT_MID, 220, 0);

        lv_obj_set_style_text_font(step3_label, &lv_font_montserrat_24, LV_STATE_DEFAULT);
        lv_label_set_text(step3_label, "STEP-3");
        lv_obj_set_style_text_color(step3_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
        lv_obj_align_to(step3_label, step3_circle, LV_ALIGN_OUT_RIGHT_MID, 10, 0);

        lv_label_set_translation_tag(progress_label, "progress_2/3");
    }
    else
    {
        lv_obj_t *step1_circle = lv_obj_create(container);
        lv_obj_t *step1_label = lv_label_create(container);
        lv_obj_set_size(step1_circle, 20, 20);
        lv_obj_set_pos(step1_circle, 170, 120);

        // 当前步骤已完成颜色：0x05D235
        lv_obj_set_style_bg_color(step1_circle, lv_color_hex(0x05D235), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_border_width(step1_circle, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_remove_flag(step1_circle, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_style_radius(step1_circle, 10, LV_PART_MAIN | LV_STATE_DEFAULT);

        lv_obj_set_style_text_font(step1_label, &lv_font_montserrat_24, LV_STATE_DEFAULT);
        lv_label_set_text(step1_label, "STEP-1");
        lv_obj_set_style_text_color(step1_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
        lv_obj_align_to(step1_label, step1_circle, LV_ALIGN_OUT_RIGHT_MID, 10, 0);

        lv_obj_t *step2_circle = lv_obj_create(container);
        lv_obj_t *step2_label = lv_label_create(container);
        lv_obj_set_size(step2_circle, 20, 20);
        // 当前步骤进行中颜色：0xFFA200
        lv_obj_set_style_bg_color(step2_circle, lv_color_hex(0xFFA200), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_border_width(step2_circle, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_remove_flag(step2_circle, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_style_radius(step2_circle, 10, LV_PART_MAIN | LV_STATE_DEFAULT);

        lv_obj_align_to(step2_circle, step1_circle, LV_ALIGN_OUT_RIGHT_MID, 320, 0);

        lv_obj_set_style_text_font(step2_label, &lv_font_montserrat_24, LV_STATE_DEFAULT);
        lv_label_set_text(step2_label, "STEP-2");
        lv_obj_set_style_text_color(step2_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
        lv_obj_align_to(step2_label, step2_circle, LV_ALIGN_OUT_RIGHT_MID, 10, 0);

        lv_label_set_translation_tag(progress_label, "progress_2/2");
    }

    lv_obj_t *time_label = lv_label_create(container);
    lv_obj_set_name(time_label, "AC_TEST_REMAIN_TIME");
    lv_obj_align(time_label, LV_ALIGN_BOTTOM_MID, 0, -230);
    lv_obj_set_style_text_font(time_label, &chinese_character_24, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(time_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_label_set_text(time_label, "30s");
    lv_obj_set_style_text_align(time_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *VoltageImg = lv_animimg_create(container);
    lv_animimg_set_src(VoltageImg, (const void **) lightning_anim_g, 2);
    lv_obj_set_size(VoltageImg, 80, 80);
    lv_obj_set_pos(VoltageImg, 175, 225);
    lv_animimg_set_duration(VoltageImg, 1000);
    lv_animimg_set_repeat_count(VoltageImg, LV_ANIM_REPEAT_INFINITE);
    lv_animimg_start(VoltageImg);

    lv_obj_t *unit_label = lv_label_create(container);
    lv_obj_set_style_text_font(unit_label, &chinese_character_72, LV_STATE_DEFAULT);
    lv_label_set_text_fmt(unit_label, "V AC");
    lv_obj_set_style_text_color(unit_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_align_to(unit_label, VoltageImg, LV_ALIGN_OUT_RIGHT_MID, 225, 0);

    lv_obj_t *ac_voltage_label = lv_label_create(container);
    lv_obj_set_style_text_font(ac_voltage_label, &chinese_character_72, LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ac_voltage_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_label_set_text_fmt(ac_voltage_label, "%d", ac_voltage);
    lv_obj_set_style_text_align(ac_voltage_label, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN);
    lv_obj_align_to(ac_voltage_label, unit_label, LV_ALIGN_OUT_LEFT_MID, -20, 0);

    static lv_point_precise_t linePoints[] = {{175, 305}, {625, 305}};
    lv_obj_t *Line = lv_line_create(container);
    lv_line_set_points(Line, linePoints, 2);
    lv_obj_set_style_line_width(Line, 4, LV_PART_MAIN);
    lv_obj_set_style_line_color(Line, lv_color_hex(0xFFFFFF), LV_PART_MAIN);

    lv_obj_t *step_message_label = lv_label_create(container);
    lv_obj_set_style_text_font(step_message_label, &chinese_character_20, LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(step_message_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);

    lv_label_set_translation_tag(step_message_label, "dielectric_loss");

    lv_obj_align(progress_label, LV_ALIGN_BOTTOM_LEFT, 20, -21);
    lv_obj_align(step_message_label, LV_ALIGN_BOTTOM_MID, 0, -85);

    dielectric_loss_test_timer = lv_timer_create(update_remain_test_time_cb, 1000U, time_label);
}

static void update_remain_test_time_cb(lv_timer_t *timer)
{
    lv_obj_t *time_label = (lv_obj_t *) lv_timer_get_user_data(timer);
    lv_label_set_text_fmt(time_label, "%lus", device_state.remain_test_time);
}
