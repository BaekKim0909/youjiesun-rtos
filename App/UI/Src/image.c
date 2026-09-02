//
// Created by 74222 on 2026/9/2.
//
#include "image.h"

#define MEASURE_STANDARD_IMG_DATA_SIZE (72U * 72U * 3U)
#define ELECTRODE_IMG_DATA_SIZE (72U * 72U * 3U)
#define SAMPLE_IMG_DATA_SIZE (72U * 72U * 3U)
#define RESISTANCE_IMG_DATA_SIZE (72U * 72U * 3U)

/* 测试标准icon图片数组 */
__attribute__((section(".sdram"),aligned(4)))
uint8_t measure_standard_img_map[MEASURE_STANDARD_IMG_DATA_SIZE] = {};

/* 测试电极icon图片数组 */
__attribute__((section(".sdram"),aligned(4)))
uint8_t electrode_img_map[ELECTRODE_IMG_DATA_SIZE] = {};

/* 样品ID图片数组 */
__attribute__((section(".sdram"),aligned(4)))
uint8_t sample_imge_map[SAMPLE_IMG_DATA_SIZE] = {};

/* 体积电阻率图片数组 */
__attribute__((section(".sdram"),aligned(4)))
uint8_t resistance_imge_map[SAMPLE_IMG_DATA_SIZE] = {};


image_t image_list[4] = {
    {
        .img_file_name = "measure_standard_icon.bin",
        .img_size = MEASURE_STANDARD_IMG_DATA_SIZE,
        .img_map = measure_standard_img_map
    },
    {
        .img_file_name = "electrode_icon.bin",
        .img_size = ELECTRODE_IMG_DATA_SIZE,
        .img_map = electrode_img_map
    },
    {
        .img_file_name = "sample_icon.bin",
        .img_size = SAMPLE_IMG_DATA_SIZE,
        .img_map = sample_imge_map
    },
    {
        .img_file_name = "resistance_icon.bin",
        .img_size = RESISTANCE_IMG_DATA_SIZE,
        .img_map = resistance_imge_map
    },
};

/* 测试标准图片对象 */
const lv_image_dsc_t measure_standard_image = {
    .header.cf = LV_COLOR_FORMAT_RGB565A8,
    .header.magic = LV_IMAGE_HEADER_MAGIC,
    .header.w = 72,
    .header.h = 72,
    .data_size = 5184 * 3,
    .data = measure_standard_img_map,
};

/* 电极图片对象 */
const lv_image_dsc_t electrode_image = {
    .header.cf = LV_COLOR_FORMAT_RGB565A8,
    .header.magic = LV_IMAGE_HEADER_MAGIC,
    .header.w = 72,
    .header.h = 72,
    .data_size = 5184 * 3,
    .data = electrode_img_map,
};

/* 样品ID对象 */
const lv_image_dsc_t sample_image = {
    .header.cf = LV_COLOR_FORMAT_RGB565A8,
    .header.magic = LV_IMAGE_HEADER_MAGIC,
    .header.w = 72,
    .header.h = 72,
    .data_size = 5184 * 3,
    .data = sample_imge_map,
};

/* 体积电阻率图片对象 */
const lv_image_dsc_t resistance_image = {
    .header.cf = LV_COLOR_FORMAT_RGB565A8,
    .header.magic = LV_IMAGE_HEADER_MAGIC,
    .header.w = 72,
    .header.h = 72,
    .data_size = 5184 * 3,
    .data = resistance_imge_map,
};
