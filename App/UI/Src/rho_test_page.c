//
// Created by 74222 on 2026/9/17.
//

#include "../Inc/rho_test_page.h"
#include "lvgl.h"
#include "mainUI.h"
#include "test_data.h"

LV_FONT_DECLARE(chinese_character_20)
LV_FONT_DECLARE(chinese_character_24)
LV_FONT_DECLARE(chinese_character_72)

void load_rho_test_page(const test_standard_type standard_type, const uint16_t rho_param,
                        const uint16_t current_rho_step, const uint16_t dc_voltage)
{
    current_page_index_g = RHO_TEST_PAGE;
    lv_obj_t *container = container_get();

    lv_obj_t *label1 = lv_label_create(container);
    lv_obj_set_style_text_font(label1, &chinese_character_20, LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(label1, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_label_set_text(label1, lv_translation_get("test_standard"));
    lv_obj_set_pos(label1, 19, 20);

    lv_obj_t *label2 = lv_label_create(container);
    lv_obj_set_style_text_font(label2, &chinese_character_20, LV_STATE_DEFAULT);

    lv_label_set_text(label2, latest_test_record.standard_name);
    lv_obj_set_style_text_color(label2, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_align_to(label2, label1, LV_ALIGN_OUT_RIGHT_MID, 5, 0);

    // 测试进度标签
    lv_obj_t *ProgressLabel = lv_label_create(container);
    lv_obj_set_style_text_font(ProgressLabel, &chinese_character_20, LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ProgressLabel, lv_color_hex(0xFFFFFF), LV_PART_MAIN);

    lv_obj_t *stopTipMessageLabel = lv_label_create(container);
    lv_obj_set_style_text_font(stopTipMessageLabel, &chinese_character_24, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(stopTipMessageLabel, lv_color_hex(0xFF3346), LV_PART_MAIN | LV_STATE_DEFAULT);

    /* code */
    lv_label_set_text(stopTipMessageLabel, lv_translation_get("stop_tip_message"));

    lv_obj_align(stopTipMessageLabel, LV_ALIGN_TOP_RIGHT, -50, 20);

    if (standard_type == FULL_TEMPLATE && (rho_param == 1 || rho_param == 2))
    {
        /* code */
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
        // 当前步骤已完成色：0x05D235
        lv_obj_set_style_bg_color(step2_circle, lv_color_hex(0x05D235), LV_PART_MAIN | LV_STATE_DEFAULT);
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
        // 当前步骤进行中颜色：0xFFA200
        lv_obj_set_style_bg_color(step3_circle, lv_color_hex(0xFFA200), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_border_width(step3_circle, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_remove_flag(step3_circle, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_style_radius(step3_circle, 10, LV_PART_MAIN | LV_STATE_DEFAULT);

        lv_obj_align_to(step3_circle, step2_circle, LV_ALIGN_OUT_RIGHT_MID, 220, 0);

        lv_obj_set_style_text_font(step3_label, &lv_font_montserrat_24, LV_STATE_DEFAULT);
        lv_label_set_text(step3_label, "STEP-3");
        lv_obj_set_style_text_color(step3_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
        lv_obj_align_to(step3_label, step3_circle, LV_ALIGN_OUT_RIGHT_MID, 10, 0);

        lv_label_set_text(ProgressLabel, lv_translation_get("progress_3/3"));
    }
    else
    {
        /* code */
        lv_obj_t *step1_circle = lv_obj_create(container);
        lv_obj_t *step1_label = lv_label_create(container);
        lv_obj_set_size(step1_circle, 20, 20);
        lv_obj_set_pos(step1_circle, 170, 120);

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

        lv_obj_align_to(step2_circle, step1_circle, LV_ALIGN_OUT_RIGHT_MID, 320, 0);

        lv_obj_set_style_text_font(step2_label, &lv_font_montserrat_24, LV_STATE_DEFAULT);
        lv_label_set_text(step2_label, "STEP-2");
        lv_obj_set_style_text_color(step2_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
        lv_obj_align_to(step2_label, step2_circle, LV_ALIGN_OUT_RIGHT_MID, 10, 0);

        lv_label_set_text(ProgressLabel, lv_translation_get("progress_2/2"));
    }

    lv_obj_t *timeLabel = lv_label_create(container);
    lv_obj_set_name(timeLabel, "DC_TEST_REMAIN_TIME");
    lv_obj_align(timeLabel, LV_ALIGN_BOTTOM_MID, 0, -230);
    lv_obj_set_style_text_font(timeLabel, &chinese_character_24, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(timeLabel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_label_set_text(timeLabel, "60s");
    lv_obj_set_style_text_align(timeLabel, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *VoltageImg = lv_animimg_create(container);
    lv_animimg_set_src(VoltageImg, (const void **) lightning_anim_g, 2);
    lv_obj_set_size(VoltageImg, 80, 80);
    lv_obj_set_pos(VoltageImg, 175, 225);
    lv_animimg_set_duration(VoltageImg, 1000);
    lv_animimg_set_repeat_count(VoltageImg, LV_ANIM_REPEAT_INFINITE);
    lv_animimg_start(VoltageImg);

    lv_obj_t *unit_label = lv_label_create(container);
    lv_obj_set_style_text_font(unit_label, &chinese_character_72, LV_STATE_DEFAULT);
    lv_label_set_text_fmt(unit_label, "V DC");
    lv_obj_set_style_text_color(unit_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_align_to(unit_label, VoltageImg, LV_ALIGN_OUT_RIGHT_MID, 225, 0);

    lv_obj_t *dc_voltage_label = lv_label_create(container);
    lv_obj_set_style_text_font(dc_voltage_label, &chinese_character_72, LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(dc_voltage_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    if (rho_param == 1)
    {
        /* code */
        lv_label_set_text_fmt(dc_voltage_label, "+ %d", dc_voltage);
    }
    else if (rho_param == 2)
    {
        /* code */
        if (current_rho_step == 1)
        {
            /* code */
            lv_label_set_text_fmt(dc_voltage_label, "+ %d", dc_voltage);
        }
        else if (current_rho_step == 2)
        {
            /* code */
            lv_label_set_text_fmt(dc_voltage_label, "- %d", dc_voltage);
        }
    }

    lv_obj_set_style_text_align(dc_voltage_label, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN);
    lv_obj_align_to(dc_voltage_label, unit_label, LV_ALIGN_OUT_LEFT_MID, -20, 0);

    static lv_point_precise_t linePoints[] = {{175, 305}, {625, 305}};
    lv_obj_t *Line = lv_line_create(container);
    lv_line_set_points(Line, linePoints, 2);
    lv_obj_set_style_line_width(Line, 4, LV_PART_MAIN);
    lv_obj_set_style_line_color(Line, lv_color_hex(0xFFFFFF), LV_PART_MAIN);

    lv_obj_t *StepMessageLabel = lv_label_create(container);
    lv_obj_set_style_text_font(StepMessageLabel, &chinese_character_20, LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(StepMessageLabel, lv_color_hex(0xFFFFFF), LV_PART_MAIN);


    lv_label_set_text(StepMessageLabel,lv_translation_get("rho_test"));


    lv_obj_align(StepMessageLabel, LV_ALIGN_BOTTOM_MID, 0, -85);
    lv_obj_align(ProgressLabel, LV_ALIGN_BOTTOM_LEFT, 20, -21);
}
