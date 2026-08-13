//
// Created by 74222 on 2026/8/13.
//
#include "notice_message.h"
#include "lvgl.h"
LV_FONT_DECLARE(chinese_character_20)

static lv_obj_t *notify_box = NULL;
static lv_timer_t *notify_box_timer = NULL;

static void dispose_notify_box_timer_cb(lv_timer_t *timer);

void show_notice_message_box(const notice_message_t notice_message)
{
    if (notify_box != NULL && lv_obj_is_valid(notify_box))
    {
        /* code */
        lv_obj_delete(notify_box);
        notify_box = NULL;
    }
    if (notify_box_timer != NULL)
    {
        /* code */
        lv_timer_delete(notify_box_timer);
        notify_box_timer = NULL;
    }
    // 在顶层创建
    notify_box = lv_obj_create(lv_layer_top());
    lv_obj_set_size(notify_box, 400, 90);
    lv_obj_center(notify_box);

    // 错误编码 背景色红色
    if (NOTICE_ERROR == notice_message.result_type)
    {
        /* code */
        lv_obj_set_style_bg_color(notify_box, lv_color_hex(0xE31F1B), LV_PART_MAIN | LV_STATE_DEFAULT);
    }
    // 成功编码 背景色绿色
    else if (NOTICE_SUCCESS == notice_message.result_type)
    {
        /* code */
        lv_obj_set_style_bg_color(notify_box, lv_color_hex(0x00EB20), LV_PART_MAIN | LV_STATE_DEFAULT);
    }
    // 提示编码 背景色黄色
    else if (NOTICE_WARNING == notice_message.result_type)
    {
        /* code */
        lv_obj_set_style_bg_color(notify_box, lv_color_hex(0xE6A23C), LV_STATE_DEFAULT | LV_PART_MAIN);
    }
    lv_obj_set_style_radius(notify_box, 20, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(notify_box, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(notify_box, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(notify_box, LV_OPA_90, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *label = lv_label_create(notify_box);
    lv_obj_set_style_text_font(label, &chinese_character_20, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);

    /* code */
    lv_label_set_text(label, notice_message.message);

    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_center(label);

    notify_box_timer = lv_timer_create(dispose_notify_box_timer_cb, 3000, NULL);
    lv_timer_set_repeat_count(notify_box_timer, 1);
}

static void dispose_notify_box_timer_cb(lv_timer_t *timer)
{
    if (notify_box != NULL && lv_obj_is_valid(notify_box))
    {
        /* code */
        lv_obj_delete(notify_box);
        notify_box = NULL;
    }
    if (notify_box_timer != NULL)
    {
        lv_timer_delete(notify_box_timer);
        notify_box_timer = NULL;
    }
}
