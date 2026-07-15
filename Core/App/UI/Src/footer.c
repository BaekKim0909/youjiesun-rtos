//
// Created by 74222 on 2026/7/15.
//

#include "../Inc/footer.h"
#include "style_g.h"

LV_IMAGE_DECLARE(TestIcon);
LV_IMAGE_DECLARE(TestFocusIcon);
LV_IMAGE_DECLARE(DataRecordIcon);
LV_IMAGE_DECLARE(DataRecordFocusIcon);
LV_IMAGE_DECLARE(TestStandardIcon);
LV_IMAGE_DECLARE(TestStandardFocusIcon);
LV_IMAGE_DECLARE(CalibrationIcon);
LV_IMAGE_DECLARE(CalibrationFocusIcon);
LV_IMAGE_DECLARE(InfoIcon);
LV_IMAGE_DECLARE(InfoFocusIcon);
LV_IMAGE_DECLARE(SettingIcon);
LV_IMAGE_DECLARE(SettingFocusIcon);

static lv_obj_t * footer = NULL;

lv_obj_t *footer_get()
{
    return footer;
}
// 初始化footer
void footer_init(void)
{
    footer = lv_obj_create(lv_screen_active());
    lv_obj_set_size(footer, 800, 40);
    lv_obj_set_pos(footer, 0, 440);
    lv_obj_set_style_bg_color(footer, lv_color_hex(0x123867), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(footer, LV_OPA_COVER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(footer, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(footer, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(footer, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_flag(footer, LV_OBJ_FLAG_SCROLLABLE);
}
void footer_load()
{
    lv_obj_t * home_page_btn = lv_button_create(footer);
    lv_obj_t * data_record_page_btn = lv_button_create(footer);
    lv_obj_t * test_standard_page_btn = lv_button_create(footer);
    lv_obj_t * calibration_page_btn = lv_button_create(footer);
    lv_obj_t * setting_page_btn = lv_button_create(footer);
    lv_obj_t * info_page_btn = lv_button_create(footer);

    lv_obj_t * home_btn_img = lv_image_create(home_page_btn);
    lv_obj_t * dataRecord_btn_img = lv_image_create(data_record_page_btn);
    lv_obj_t * testStandard_btn_img = lv_image_create(test_standard_page_btn);
    lv_obj_t * calibration_btn_img = lv_image_create(calibration_page_btn);
    lv_obj_t * setting_btn_img = lv_image_create(setting_page_btn);
    lv_obj_t * info_btn_img = lv_image_create(info_page_btn);

    lv_obj_set_size(home_page_btn, 133, 40);
    lv_obj_set_size(data_record_page_btn, 133, 40);
    lv_obj_set_size(test_standard_page_btn, 133, 40);
    lv_obj_set_size(calibration_page_btn, 133, 40);
    lv_obj_set_size(setting_page_btn, 133, 40);
    lv_obj_set_size(info_page_btn, 133, 40);

    lv_obj_set_size(home_btn_img, 30, 30);
    lv_obj_set_size(dataRecord_btn_img, 30, 30);
    lv_obj_set_size(testStandard_btn_img, 30, 30);
    lv_obj_set_size(calibration_btn_img, 30, 30);
    lv_obj_set_size(setting_btn_img, 30, 30);
    lv_obj_set_size(info_btn_img, 30, 30);

    lv_obj_set_pos(home_page_btn, 0, 0);
    lv_obj_align_to(data_record_page_btn, home_page_btn, LV_ALIGN_OUT_RIGHT_MID, 2, 0);
    lv_obj_align_to(test_standard_page_btn, data_record_page_btn, LV_ALIGN_OUT_RIGHT_MID, 2, 0);
    lv_obj_align_to(calibration_page_btn, test_standard_page_btn, LV_ALIGN_OUT_RIGHT_MID, 2, 0);
    lv_obj_align_to(setting_page_btn, calibration_page_btn, LV_ALIGN_OUT_RIGHT_MID, 2, 0);
    lv_obj_align_to(info_page_btn, setting_page_btn, LV_ALIGN_OUT_RIGHT_MID, 2, 0);

    lv_obj_add_style(home_page_btn, &nav_btn_style_g, LV_PART_MAIN);
    lv_obj_add_style(data_record_page_btn, &nav_btn_style_g, LV_PART_MAIN);
    lv_obj_add_style(test_standard_page_btn, &nav_btn_style_g, LV_PART_MAIN);
    lv_obj_add_style(calibration_page_btn, &nav_btn_style_g, LV_PART_MAIN);
    lv_obj_add_style(setting_page_btn, &nav_btn_style_g, LV_PART_MAIN);
    lv_obj_add_style(info_page_btn, &nav_btn_style_g, LV_PART_MAIN);

    lv_obj_set_style_bg_color(home_page_btn, lv_color_hex(0x0E2A4D), LV_PART_MAIN);
    lv_obj_set_style_bg_color(data_record_page_btn, lv_color_hex(0x0E2A4D), LV_PART_MAIN);
    lv_obj_set_style_bg_color(test_standard_page_btn, lv_color_hex(0x0E2A4D), LV_PART_MAIN);
    lv_obj_set_style_bg_color(calibration_page_btn, lv_color_hex(0x0E2A4D), LV_PART_MAIN);
    lv_obj_set_style_bg_color(setting_page_btn, lv_color_hex(0x0E2A4D), LV_PART_MAIN);
    lv_obj_set_style_bg_color(info_page_btn, lv_color_hex(0x0E2A4D), LV_PART_MAIN);

    lv_image_set_src(home_btn_img, &TestIcon);
    lv_image_set_src(dataRecord_btn_img, &DataRecordIcon);
    lv_image_set_src(testStandard_btn_img, &TestStandardIcon);
    lv_image_set_src(calibration_btn_img, &CalibrationIcon);
    lv_image_set_src(setting_btn_img, &SettingIcon);
    lv_image_set_src(info_btn_img, &InfoIcon);

    lv_obj_center(home_btn_img);
    lv_obj_center(dataRecord_btn_img);
    lv_obj_center(testStandard_btn_img);
    lv_obj_center(calibration_btn_img);
    lv_obj_center(setting_btn_img);
    lv_obj_center(info_btn_img);

    // lv_obj_add_event_cb(home_page_btn, HandleNavBtnClicked, LV_EVENT_CLICKED, NULL);
    // lv_obj_add_event_cb(data_record_page_btn, HandleNavBtnClicked, LV_EVENT_CLICKED, NULL);
    // lv_obj_add_event_cb(test_standard_page_btn, HandleNavBtnClicked, LV_EVENT_CLICKED, NULL);
    // lv_obj_add_event_cb(calibration_btn_img, HandleNavBtnClicked, LV_EVENT_CLICKED, NULL);
    // lv_obj_add_event_cb(setting_btn_img, HandleNavBtnClicked, LV_EVENT_CLICKED, NULL);
    // lv_obj_add_event_cb(info_page_btn, HandleNavBtnClicked, LV_EVENT_CLICKED, NULL);
    //
    // lv_obj_add_event_cb(home_page_btn, SetNavBtnFocus_cb, LV_EVENT_FOCUSED, NULL);
    // lv_obj_add_event_cb(data_record_page_btn, SetNavBtnFocus_cb, LV_EVENT_FOCUSED, NULL);
    // lv_obj_add_event_cb(test_standard_page_btn, SetNavBtnFocus_cb, LV_EVENT_FOCUSED, NULL);
    // lv_obj_add_event_cb(calibration_page_btn, SetNavBtnFocus_cb, LV_EVENT_FOCUSED, NULL);
    // lv_obj_add_event_cb(setting_page_btn, SetNavBtnFocus_cb, LV_EVENT_FOCUSED, NULL);
    // lv_obj_add_event_cb(info_page_btn, SetNavBtnFocus_cb, LV_EVENT_FOCUSED, NULL);
    //
    // lv_obj_add_event_cb(home_page_btn, SetNavBtnDeFocus_cb, LV_EVENT_DEFOCUSED, NULL);
    // lv_obj_add_event_cb(data_record_page_btn, SetNavBtnDeFocus_cb, LV_EVENT_DEFOCUSED, NULL);
    // lv_obj_add_event_cb(test_standard_page_btn, SetNavBtnDeFocus_cb, LV_EVENT_DEFOCUSED, NULL);
    // lv_obj_add_event_cb(calibration_page_btn, SetNavBtnDeFocus_cb, LV_EVENT_DEFOCUSED, NULL);
    // lv_obj_add_event_cb(setting_page_btn, SetNavBtnDeFocus_cb, LV_EVENT_DEFOCUSED, NULL);
    // lv_obj_add_event_cb(info_page_btn, SetNavBtnDeFocus_cb, LV_EVENT_DEFOCUSED, NULL);
}