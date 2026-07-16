//
// Created by 74222 on 2026/7/14.
//

#include "header.h"
#include "system_structs.h"
#include "main.h"

LV_IMAGE_DECLARE(LidOpen);
LV_IMAGE_DECLARE(LidClose);
LV_IMAGE_DECLARE(PourOilClose);
LV_IMAGE_DECLARE(PourOilOpen);
LV_IMAGE_DECLARE(OilCupNormal);
LV_IMAGE_DECLARE(OilCupAbnormal);
LV_IMAGE_DECLARE(TemperatureNormal);
LV_IMAGE_DECLARE(TemperatureHigh);

LV_FONT_DECLARE(chinese_character_20);

extern RTC_HandleTypeDef hrtc;
extern Setting_t user_setting_g;
extern Setting_t default_setting_g;

RTC_DateTypeDef CurrentDate; // 当前日期
RTC_TimeTypeDef CurrentTime; // 当前时间

static lv_obj_t * header = NULL;
static lv_timer_t *update_time_timer_g = NULL;
static lv_timer_t *update_temperature_timer_g = NULL;
volatile DeviceState_t device_state = {0};               // 设备状态
volatile float current_temperature_g = 40.5;             // 当前油温
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
    lv_obj_t *lidStateImg = lv_image_create(header);
    lv_obj_set_size(lidStateImg, 30, 30);
    if (device_state.lid_state == 0)
    {
        /* code */
        lv_image_set_src(lidStateImg, &LidClose);
    }
    else if (device_state.lid_state == 1)
    {
        lv_image_set_src(lidStateImg, &LidOpen);
    }
    lv_obj_align(lidStateImg, LV_ALIGN_LEFT_MID, 25, 0);

    lv_obj_t *pourOilImg = lv_image_create(header);
    lv_obj_set_size(pourOilImg, 30, 30);
    if (device_state.pour_state == 0)
    {
        lv_image_set_src(pourOilImg, &PourOilClose);
    }
    else if (device_state.pour_state == 1)
    {
        /* code */
        lv_image_set_src(pourOilImg, &PourOilOpen);
    }

    lv_obj_align_to(pourOilImg, lidStateImg, LV_ALIGN_OUT_RIGHT_MID, 30, 0);

    lv_obj_t *oilCupImg = lv_image_create(header);
    lv_obj_set_size(oilCupImg, 30, 30);
    if (device_state.oil_cup_state == 0)
    {
        /* code */
        lv_image_set_src(oilCupImg, &OilCupNormal);
    }
    else if (device_state.oil_cup_state == 1)
    {
        /* code */
        lv_image_set_src(oilCupImg, &OilCupAbnormal);
    }

    lv_obj_align_to(oilCupImg, pourOilImg, LV_ALIGN_OUT_RIGHT_MID, 30, 0);

    lv_obj_t *temperatureImg = lv_image_create(header);
    lv_obj_set_size(temperatureImg, 30, 30);
    lv_obj_set_name(temperatureImg, "TemperatureImg");
    if (current_temperature_g > 40.0f)
    {
        lv_image_set_src(temperatureImg, &TemperatureHigh);
    }
    else
    {
        lv_image_set_src(temperatureImg, &TemperatureNormal);
    }

    lv_obj_align(temperatureImg, LV_ALIGN_CENTER, -20, 0);

    lv_obj_t *temperatureLabel = lv_label_create(header);
    lv_obj_set_name(temperatureLabel, "TemperatureLabel");
    lv_obj_set_style_text_font(temperatureLabel, &chinese_character_20, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(temperatureLabel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_label_set_text_fmt(temperatureLabel, "%.1f\xE2\x84\x83", current_temperature_g);
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
        lv_label_set_text_fmt(headerTimLabel, "20%2d/%02d/%02d   %02d:%02d", CurrentDate.Year, CurrentDate.Month, CurrentDate.Date, CurrentTime.Hours, CurrentTime.Minutes);
    }
    else if (user_setting_g.time_format == 1)
    {
        /* code */
        lv_label_set_text_fmt(headerTimLabel, "%02d/%02d/20%02d   %02d:%02d", CurrentDate.Month, CurrentDate.Date, CurrentDate.Year, CurrentTime.Hours, CurrentTime.Minutes);
    }
    lv_obj_align(headerTimLabel, LV_ALIGN_RIGHT_MID, -20, 0);

    if (update_time_timer_g == NULL)
    {
        /* code */
        update_time_timer_g = lv_timer_create(update_time_cb, 60100, NULL);
    }
    if (update_temperature_timer_g == NULL)
    {
        /* code */
        update_temperature_timer_g = lv_timer_create(update_temperature_cb, 1000, NULL);
    }
}

void update_time_cb(lv_timer_t *timer)
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
            lv_label_set_text_fmt(headerTimLabel, "20%2d/%02d/%02d   %02d:%02d", CurrentDate.Year, CurrentDate.Month, CurrentDate.Date, CurrentTime.Hours, CurrentTime.Minutes);
        }
        else if (user_setting_g.time_format == 1)
        {
            /* code */
            lv_label_set_text_fmt(headerTimLabel, "%02d/%02d/20%02d   %02d:%02d", CurrentDate.Month, CurrentDate.Date, CurrentDate.Year, CurrentTime.Hours, CurrentTime.Minutes);
        }
    }
}
void update_temperature_cb(lv_timer_t *timer)
{

}