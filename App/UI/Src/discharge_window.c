//
// Created by 74222 on 2026/9/17.
//

#include "discharge_window.h"

#include "mainUI.h"
#include "system_state.h"

LV_FONT_DECLARE(chinese_character_20)

static void update_discharge_remain_time_cb(lv_timer_t *timer);

void load_discharge_window()
{
    lv_obj_t *container = container_get();
    lv_obj_t *discharge_window = lv_obj_create(container);

    lv_obj_set_size(discharge_window, 270, 150);
    lv_obj_set_style_pad_all(discharge_window, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_center(discharge_window);

    // 添加 "放电中" label
    lv_obj_t *waiting_label = lv_label_create(discharge_window);
    lv_obj_set_style_text_font(waiting_label, &chinese_character_20, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_align(waiting_label, LV_ALIGN_TOP_MID, 0, 10);

    lv_label_set_text(waiting_label, lv_translation_get("discharging"));

    lv_obj_t *waiting_spinner = lv_spinner_create(discharge_window);
    lv_obj_set_size(waiting_spinner, 80, 80);
    lv_spinner_set_anim_params(waiting_spinner, 10000, 200);
    lv_obj_align(waiting_spinner, LV_ALIGN_CENTER, 0, 10);

    lv_obj_t *timeLabel = lv_label_create(discharge_window);
    lv_obj_set_style_text_font(timeLabel, &chinese_character_20, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(timeLabel, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_label_set_text_fmt(timeLabel, "%lus", device_state.remain_test_time);
    lv_obj_align(timeLabel, LV_ALIGN_CENTER, 0, 10);

    lv_timer_t *update_time_label = lv_timer_create(update_discharge_remain_time_cb, 1000U, timeLabel);
}

static void update_discharge_remain_time_cb(lv_timer_t *timer)
{
    lv_obj_t *time_label = lv_timer_get_user_data(timer);
    if (lv_obj_is_valid(time_label))
    {
        lv_label_set_text_fmt(time_label, "%lus", device_state.remain_test_time);
    }
    else
    {
        lv_timer_delete(timer);
    }
}
