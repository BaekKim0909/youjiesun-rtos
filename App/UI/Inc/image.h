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

extern image_t image_list[4];


extern const lv_image_dsc_t measure_standard_image;
extern const lv_image_dsc_t electrode_image;
extern const lv_image_dsc_t sample_image;
extern const lv_image_dsc_t resistance_image;
#endif //YOUJIESUN_IMAGE_H
