//
// Created by 74222 on 2026/9/10.
//

#include "info_page.h"
#include "lvgl.h"
#include "mainUI.h"
static const char phone_number[] = "010-68940148";
LV_IMAGE_DECLARE(QR_code_image);

LV_IMAGE_DECLARE(phone_number_image);

LV_IMAGE_DECLARE(calibration_data_image);

LV_IMAGE_DECLARE(serial_number_image);

LV_IMAGE_DECLARE(software_version_image);

LV_FONT_DECLARE(chinese_character_20)

void load_info_page(void)
{
    current_page_index_g = INFO_PAGE;

    lv_obj_t *QR_code_img = NULL;
    lv_obj_t *label1 = NULL;
    lv_obj_t *phone_img = NULL;
    lv_obj_t *phone_label = NULL;
    lv_obj_t *phone_content_label = NULL;

    lv_obj_t *container = container_get();

    QR_code_img = lv_image_create(container);
    lv_image_set_src(QR_code_img, &QR_code_image);
    lv_obj_align(QR_code_img, LV_ALIGN_TOP_MID, 0, 45);

    label1 = lv_label_create(container);
    lv_obj_set_style_text_font(label1, &chinese_character_20, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(label1, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);


    lv_obj_t *serial_number_img = lv_image_create(container);
    lv_image_set_src(serial_number_img, &serial_number_image);

    lv_obj_t *serial_number_label = lv_label_create(container);
    lv_obj_set_style_text_font(serial_number_label, &chinese_character_20, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(serial_number_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *serial_number_content_label = lv_label_create(container);
    lv_obj_set_style_text_font(serial_number_content_label, &chinese_character_20, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(serial_number_content_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_label_set_text(serial_number_content_label, "--");

    lv_obj_t *software_version_img = lv_image_create(container);
    lv_image_set_src(software_version_img, &software_version_image);

    lv_obj_t *software_version_label = lv_label_create(container);
    lv_obj_set_style_text_font(software_version_label, &chinese_character_20, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(software_version_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *software_version_content_label = lv_label_create(container);
    lv_obj_set_style_text_font(software_version_content_label, &chinese_character_20, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(software_version_content_label, lv_color_hex(0xFFFFFF),
                                LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_label_set_text_fmt(software_version_content_label, "V%d.%d", major_version, minor_version);

    lv_obj_t *cal_date_img = lv_image_create(container);
    lv_image_set_src(cal_date_img, &calibration_data_image);

    lv_obj_t *cal_date_label = lv_label_create(container);
    lv_obj_set_style_text_font(cal_date_label, &chinese_character_20, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(cal_date_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *cal_date_content_label = lv_label_create(container);
    lv_obj_set_style_text_font(cal_date_content_label, &chinese_character_20, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(cal_date_content_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_label_set_text(cal_date_content_label, "--");

    phone_img = lv_image_create(container);
    lv_image_set_src(phone_img, &phone_number_image);

    phone_label = lv_label_create(container);
    lv_obj_set_style_text_font(phone_label, &chinese_character_20, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(phone_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);

    phone_content_label = lv_label_create(container);
    lv_obj_set_style_text_font(phone_content_label, &chinese_character_20, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(phone_content_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_label_set_text(phone_content_label, phone_number);


    lv_label_set_text(serial_number_label, "Serial Number");
    lv_label_set_text(software_version_label, "Firmware Version");
    lv_label_set_text(cal_date_label, "Calibration Date");
    lv_label_set_text(label1, "Contact Us for Support");
    lv_label_set_text(phone_label, "Support Hotline");


    lv_obj_set_pos(serial_number_img, 60, 290);
    lv_obj_align_to(serial_number_content_label, serial_number_img, LV_ALIGN_OUT_RIGHT_MID, 165, 0);
    lv_obj_align_to(software_version_img, serial_number_img, LV_ALIGN_OUT_RIGHT_MID, 340, 0);
    lv_obj_align_to(software_version_content_label, software_version_img, LV_ALIGN_OUT_RIGHT_MID, 185, 0);

    lv_obj_align_to(cal_date_img, serial_number_img, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);
    lv_obj_align_to(cal_date_content_label, cal_date_img, LV_ALIGN_OUT_RIGHT_MID, 175, 0);
    lv_obj_align_to(phone_img, software_version_img, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);
    lv_obj_align_to(phone_content_label, phone_img, LV_ALIGN_OUT_RIGHT_MID, 185, 0);

    lv_obj_align_to(label1, QR_code_img, LV_ALIGN_OUT_BOTTOM_MID, 0, 25);
    lv_obj_align_to(serial_number_label, serial_number_img, LV_ALIGN_OUT_RIGHT_MID, 7, 0);
    lv_obj_align_to(software_version_label, software_version_img, LV_ALIGN_OUT_RIGHT_MID, 7, 0);
    lv_obj_align_to(cal_date_label, cal_date_img, LV_ALIGN_OUT_RIGHT_MID, 7, 0);
    lv_obj_align_to(phone_label, phone_img, LV_ALIGN_OUT_RIGHT_MID, 7, 0);
}
