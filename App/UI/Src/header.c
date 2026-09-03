//
// Created by 74222 on 2026/7/14.
//

#include "header.h"
#include "main.h"
#include "mainUI.h"
#include "system_state.h"
#include "system_structs.h"
#include "image.h"
LV_IMAGE_DECLARE(lid_open_image);
LV_IMAGE_DECLARE(lid_close_image);
LV_IMAGE_DECLARE(pour_oil_close_image);
LV_IMAGE_DECLARE(pour_oil_open_image);
LV_IMAGE_DECLARE(oil_cup_normal_image);
LV_IMAGE_DECLARE(oil_cup_abnormal_image);
LV_IMAGE_DECLARE(temperature_normal_image);
LV_IMAGE_DECLARE(temperature_high_image);

LV_FONT_DECLARE(chinese_character_20);

extern RTC_HandleTypeDef hrtc;
extern Setting_t user_setting_g;
extern Setting_t default_setting_g;
RTC_DateTypeDef CurrentDate; // 当前日期
RTC_TimeTypeDef CurrentTime; // 当前时间

static lv_obj_t *header = NULL;
static lv_timer_t *update_time_timer = NULL;
static lv_timer_t *update_device_state_timer = NULL;

static void update_device_state_cb(lv_timer_t *timer);

static void update_time_cb(lv_timer_t *timer);

// 暴露外部引用
lv_obj_t *header_get()
{
    return header;
}

void header_init(void)
{
    header = lv_obj_create(lv_screen_active());
    lv_obj_set_size(header, 800, 40);
    lv_obj_set_pos(header, 0, 0);
    lv_obj_set_style_bg_color(header, lv_color_hex(0x123867), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(header, LV_OPA_COVER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(header, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(header, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(header, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    // 边框
    lv_obj_set_style_border_width(header, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(header, LV_BORDER_SIDE_BOTTOM, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(header, lv_color_hex(0x9AC3EF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_flag(header, LV_OBJ_FLAG_SCROLLABLE);
}

void header_load(void)
{
    lv_obj_t *lid_state_img = lv_image_create(header);
    lv_obj_set_name(lid_state_img, "lid_state_img");
    lv_obj_set_size(lid_state_img, 30, 30);
    if (device_state.lid_state == 0)
    {
        /* code */
        lv_image_set_src(lid_state_img, &lid_close_image);
    }
    else if (device_state.lid_state == 1)
    {
        lv_image_set_src(lid_state_img, &lid_open_image);
    }
    lv_obj_align(lid_state_img, LV_ALIGN_LEFT_MID, 25, 0);

    lv_obj_t *pour_oil_img = lv_image_create(header);
    lv_obj_set_name(pour_oil_img, "pour_oil_img");
    lv_obj_set_size(pour_oil_img, 30, 30);
    if (device_state.pour_state == 0)
    {
        lv_image_set_src(pour_oil_img, &pour_oil_close_image);
    }
    else if (device_state.pour_state == 1)
    {
        /* code */
        lv_image_set_src(pour_oil_img, &pour_oil_open_image);
    }

    lv_obj_align_to(pour_oil_img, lid_state_img, LV_ALIGN_OUT_RIGHT_MID, 30, 0);

    lv_obj_t *oil_cup_img = lv_image_create(header);
    lv_obj_set_name(oil_cup_img, "oil_cup_img");
    lv_obj_set_size(oil_cup_img, 30, 30);
    if (device_state.oil_cup_state == 0)
    {
        /* code */
        lv_image_set_src(oil_cup_img, &oil_cup_normal_image);
    }
    else if (device_state.oil_cup_state == 1)
    {
        /* code */
        lv_image_set_src(oil_cup_img, &oil_cup_abnormal_image);
    }

    lv_obj_align_to(oil_cup_img, pour_oil_img, LV_ALIGN_OUT_RIGHT_MID, 30, 0);

    lv_obj_t *temperatureImg = lv_image_create(header);
    lv_obj_set_size(temperatureImg, 30, 30);
    lv_obj_set_name(temperatureImg, "TemperatureImg");
    if (device_state.oil_cup_temperature > 40.0f)
    {
        lv_image_set_src(temperatureImg, &temperature_high_image);
    }
    else
    {
        lv_image_set_src(temperatureImg, &temperature_normal_image);
    }

    lv_obj_align(temperatureImg, LV_ALIGN_CENTER, -20, 0);

    lv_obj_t *temperatureLabel = lv_label_create(header);
    lv_obj_set_name(temperatureLabel, "TemperatureLabel");
    lv_obj_set_style_text_font(temperatureLabel, &chinese_character_20, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(temperatureLabel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_label_set_text_fmt(temperatureLabel, "%.1f℃", device_state.oil_cup_temperature);
    lv_obj_align_to(temperatureLabel, temperatureImg, LV_ALIGN_OUT_RIGHT_MID, 10, 0);

    lv_obj_t *headerTimLabel = lv_label_create(header);
    lv_obj_set_name(headerTimLabel, "HeaderTimeLabel");
    lv_obj_set_style_text_font(headerTimLabel, &chinese_character_20, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(headerTimLabel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);

    HAL_RTC_GetTime(&hrtc, &CurrentTime, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &CurrentDate, RTC_FORMAT_BIN);

    if (user_setting_g.time_format == 0)
    {
        /* code */
        lv_label_set_text_fmt(headerTimLabel, "20%2d/%02d/%02d   %02d:%02d", CurrentDate.Year, CurrentDate.Month,
                              CurrentDate.Date, CurrentTime.Hours, CurrentTime.Minutes);
    }
    else if (user_setting_g.time_format == 1)
    {
        /* code */
        lv_label_set_text_fmt(headerTimLabel, "%02d/%02d/20%02d   %02d:%02d", CurrentDate.Month, CurrentDate.Date,
                              CurrentDate.Year, CurrentTime.Hours, CurrentTime.Minutes);
    }
    lv_obj_align(headerTimLabel, LV_ALIGN_RIGHT_MID, -20, 0);

    if (update_time_timer == NULL)
    {
        /* code */
        update_time_timer = lv_timer_create(update_time_cb, 60100, NULL);
    }
    if (update_device_state_timer == NULL)
    {
        /* code */
        update_device_state_timer = lv_timer_create(update_device_state_cb, 100, NULL);
    }
}

static void update_time_cb(lv_timer_t *timer)
{
    lv_obj_t *headerTimLabel = lv_obj_get_child_by_name(header, "HeaderTimeLabel");
    if (headerTimLabel != NULL && lv_obj_is_valid(headerTimLabel) && lv_obj_check_type(headerTimLabel, &lv_label_class))
    {
        /* code */

        /* code */
        HAL_RTC_GetTime(&hrtc, &CurrentTime, RTC_FORMAT_BIN);
        HAL_RTC_GetDate(&hrtc, &CurrentDate, RTC_FORMAT_BIN);
        if (user_setting_g.time_format == 0)
        {
            /* code */
            lv_label_set_text_fmt(headerTimLabel, "20%2d/%02d/%02d   %02d:%02d", CurrentDate.Year, CurrentDate.Month,
                                  CurrentDate.Date, CurrentTime.Hours, CurrentTime.Minutes);
        }
        else if (user_setting_g.time_format == 1)
        {
            /* code */
            lv_label_set_text_fmt(headerTimLabel, "%02d/%02d/20%02d   %02d:%02d", CurrentDate.Month, CurrentDate.Date,
                                  CurrentDate.Year, CurrentTime.Hours, CurrentTime.Minutes);
        }
    }
}

static void update_device_state_cb(lv_timer_t *timer)
{
    lv_obj_t *temperature_label = lv_obj_get_child_by_name(header, "TemperatureLabel");
    lv_obj_t *temperature_img = lv_obj_get_child_by_name(header, "TemperatureImg");
    lv_obj_t *lid_state_img = lv_obj_get_child_by_name(header, "lid_state_img");
    lv_obj_t *pour_oil_img = lv_obj_get_child_by_name(header, "pour_oil_img");
    lv_obj_t *oil_cup_img = lv_obj_get_child_by_name(header, "oil_cup_img");
    if (lid_state_img != NULL)
    {
        if (device_state.lid_state == 0)
        {
            /* code */
            lv_image_set_src(lid_state_img, &lid_close_image);
        }
        else if (device_state.lid_state == 1)
        {
            lv_image_set_src(lid_state_img, &lid_open_image);
        }
    }
    if (pour_oil_img != NULL)
    {
        if (device_state.pour_state == 0)
        {
            lv_image_set_src(pour_oil_img, &pour_oil_close_image);
        }
        else if (device_state.pour_state == 1)
        {
            /* code */
            lv_image_set_src(pour_oil_img, &pour_oil_open_image);
        }
    }
    if (oil_cup_img != NULL)
    {
        if (device_state.oil_cup_state == 0)
        {
            /* code */
            lv_image_set_src(oil_cup_img, &oil_cup_normal_image);
        }
        else if (device_state.oil_cup_state == 1)
        {
            /* code */
            lv_image_set_src(oil_cup_img, &oil_cup_abnormal_image);
        }
    }
    if (temperature_label != NULL)
    {
        if (device_state.oil_cup_temperature > 200.0f)
        {
            lv_label_set_text(temperature_label, "INVALID");
        }
        else
        {
            lv_label_set_text_fmt(temperature_label, "%.1f℃", device_state.oil_cup_temperature);
        }
    }
    if (temperature_img != NULL)
    {
        if (device_state.oil_cup_temperature >= 40.0f)
        {
            lv_image_set_src(temperature_img, &temperature_high_image);
        }
        else
        {
            lv_image_set_src(temperature_img, &temperature_normal_image);
        }
    }
    if (current_page_index_g == HEAT_PAGE)
    {
        lv_obj_t *container = container_get();
        lv_obj_t *temperature_label_of_container = lv_obj_get_child_by_name(container, "temperature_label");
        if (temperature_label_of_container != NULL &&
            lv_obj_check_type(temperature_label_of_container, &lv_label_class))
            lv_label_set_text_fmt(temperature_label_of_container, "%.1f", device_state.oil_cup_temperature);
    }
}
