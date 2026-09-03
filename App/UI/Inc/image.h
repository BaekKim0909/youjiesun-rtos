//
// Created by 74222 on 2026/9/2.
//

#ifndef YOUJIESUN_IMAGE_H
#define YOUJIESUN_IMAGE_H


#include "lvgl.h"

typedef struct
{
    char img_file_name[64];
    uint32_t img_size;
    uint8_t *img_map;
} image_t;

#define IMAGE_LIST_COUNT 27U

extern image_t image_list[IMAGE_LIST_COUNT];


extern const lv_image_dsc_t measure_standard_image;
extern const lv_image_dsc_t electrode_image;
extern const lv_image_dsc_t sample_image;
extern const lv_image_dsc_t resistance_image;
extern const lv_image_dsc_t temperature_high_image;
extern const lv_image_dsc_t temperature_normal_image;
extern const lv_image_dsc_t lid_close_image;
extern const lv_image_dsc_t lid_open_image;
extern const lv_image_dsc_t pour_oil_close_image;
extern const lv_image_dsc_t pour_oil_open_image;
extern const lv_image_dsc_t oil_cup_normal_image;
extern const lv_image_dsc_t oil_cup_abnormal_image;
#endif //YOUJIESUN_IMAGE_H
