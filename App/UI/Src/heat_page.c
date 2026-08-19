//
// Created by 74222 on 2026/8/19.
//

#include "../Inc/heat_page.h"
#include "mainUI.h"
#include "system_state.h"
LV_FONT_DECLARE(chinese_character_20)
LV_FONT_DECLARE(chinese_character_24)
LV_FONT_DECLARE(chinese_character_72)
LV_IMAGE_DECLARE(FireIcon);

void load_heat_page(const char *standard_name, const test_standard_type standard_type, const uint16_t rho_param)
{
    current_page_index_g = HEAT_PAGE;

    lv_obj_t *container = container_get();
    lv_obj_t *label1 = lv_label_create(container);
    lv_obj_set_style_text_font(label1, &chinese_character_20, LV_STATE_DEFAULT);
    lv_label_set_translation_tag(label1, "test_standard");
    lv_obj_set_style_text_color(label1, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_set_pos(label1, 19, 20);

    lv_obj_t *label2 = lv_label_create(container);
    lv_obj_set_style_text_font(label2, &chinese_character_20, LV_STATE_DEFAULT);
    lv_label_set_text(label2, standard_name);
    lv_obj_set_style_text_color(label2, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_align_to(label2, label1, LV_ALIGN_OUT_RIGHT_MID, 5, 0);

    // 测试进度标签
    lv_obj_t *progress_label = lv_label_create(container);
    lv_obj_set_style_text_font(progress_label, &chinese_character_20, LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(progress_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);

    lv_obj_t *stop_tip_message_label = lv_label_create(container);
    lv_obj_set_style_text_font(stop_tip_message_label, &chinese_character_24, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(stop_tip_message_label, lv_color_hex(0xFF3346), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_label_set_translation_tag(stop_tip_message_label, "stop_tip_message");
    lv_obj_align(stop_tip_message_label, LV_ALIGN_TOP_RIGHT, -50, 20);

    if (standard_type == FULL_TEMPLATE && (rho_param == 1 || rho_param == 2))
    {
        /* code */
        lv_obj_t *step1_circle = lv_obj_create(container);
        lv_obj_t *step1_label = lv_label_create(container);
        lv_obj_set_size(step1_circle, 20, 20);
        lv_obj_set_pos(step1_circle, 100, 120);

        // 当前步骤进行中颜色：0xFFA200
        lv_obj_set_style_bg_color(step1_circle, lv_color_hex(0xFFA200), LV_PART_MAIN | LV_STATE_DEFAULT);
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
        // 当前步骤未开始颜色：0xFFFFFF
        lv_obj_set_style_bg_color(step2_circle, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
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

        lv_label_set_translation_tag(progress_label, "progress_1/3");
    }
    else
    {
        /* code */
        lv_obj_t *step1_circle = lv_obj_create(container);
        lv_obj_t *step1_label = lv_label_create(container);
        lv_obj_set_size(step1_circle, 20, 20);
        lv_obj_set_pos(step1_circle, 170, 120);

        // 当前步骤进行中颜色：0xFFA200
        lv_obj_set_style_bg_color(step1_circle, lv_color_hex(0xFFA200), LV_PART_MAIN | LV_STATE_DEFAULT);
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
        // 当前步骤未开始颜色：0xFFFFFF
        lv_obj_set_style_bg_color(step2_circle, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_border_width(step2_circle, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_remove_flag(step2_circle, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_style_radius(step2_circle, 10, LV_PART_MAIN | LV_STATE_DEFAULT);

        lv_obj_align_to(step2_circle, step1_circle, LV_ALIGN_OUT_RIGHT_MID, 320, 0);

        lv_obj_set_style_text_font(step2_label, &lv_font_montserrat_24, LV_STATE_DEFAULT);
        lv_label_set_text(step2_label, "STEP-2");
        lv_obj_set_style_text_color(step2_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
        lv_obj_align_to(step2_label, step2_circle, LV_ALIGN_OUT_RIGHT_MID, 10, 0);
        lv_label_set_translation_tag(progress_label, "progress_1/2");
    }

    lv_obj_t *HeatImg = lv_image_create(container);
    lv_obj_set_size(HeatImg, 80, 80);
    lv_image_set_src(HeatImg, &FireIcon);
    lv_obj_set_pos(HeatImg, 175, 225);

    lv_obj_t *temperature_label = lv_label_create(container);
    lv_obj_set_name(temperature_label, "temperature_label");
    lv_obj_set_style_text_font(temperature_label, &chinese_character_72, LV_STATE_DEFAULT);
    lv_label_set_text_fmt(temperature_label, "%.1f", device_state.oil_cup_temperature);
    lv_obj_set_style_text_color(temperature_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_align(temperature_label, LV_ALIGN_BOTTOM_MID, 0, -133);

    lv_obj_t *UnitLabel = lv_label_create(container);
    lv_obj_set_style_text_font(UnitLabel, &chinese_character_72, LV_STATE_DEFAULT);
    lv_label_set_text_fmt(UnitLabel, "℃");
    lv_obj_set_style_text_color(UnitLabel, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_align_to(UnitLabel, HeatImg, LV_ALIGN_OUT_RIGHT_MID, 300, 0);

    static lv_point_precise_t linePoints[] = {{175, 305}, {625, 305}};
    lv_obj_t *Line = lv_line_create(container);
    lv_line_set_points(Line, linePoints, 2);
    lv_obj_set_style_line_width(Line, 4, LV_PART_MAIN);
    lv_obj_set_style_line_color(Line, lv_color_hex(0xFFFFFF), LV_PART_MAIN);

    lv_obj_t *StepMessageLabel = lv_label_create(container);
    lv_obj_set_style_text_font(StepMessageLabel, &chinese_character_20, LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(StepMessageLabel, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_label_set_translation_tag(StepMessageLabel, "heat");

    lv_obj_align(progress_label, LV_ALIGN_BOTTOM_LEFT, 20, -21);
    lv_obj_align(StepMessageLabel, LV_ALIGN_BOTTOM_MID, 0, -85);
}
