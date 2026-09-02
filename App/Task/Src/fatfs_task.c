//
// Created by 74222 on 2026/9/1.
//
#include <stdio.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"
#include "ff.h"
#include "fatfs.h"
#include "image.h"
#include "ui_task.h"

// 从外部SD卡中加载图片资源
bool load_all_img_from_SD_card();

// 从外部SD卡中加载图片资源
bool load_img_from_SD_card(const char *file_name, uint32_t img_size, uint8_t *img_map);

bool load_electrode_icon_from_SD_card();

static char image_file_directory[] = "0:/image";

void start_fatfs_task(void *argument)
{
    if (FR_OK != f_mount(&SDFatFS, "0:", 1))
    {
        ui_notice_post(UI_NOTICE_SD_CARD_MOUNTED_FAILED);
    }
    load_all_img_from_SD_card();
    for (;;)
    {
        vTaskDelay(1000);
    }
}


bool load_all_img_from_SD_card()
{
    for (uint32_t i = 0; i < sizeof(image_list) / sizeof(image_list[0]); i++)
    {
        load_img_from_SD_card(image_list[i].img_file_name, image_list[i].img_size, image_list[i].img_map);
    }
    return true;
}

bool load_img_from_SD_card(const char *file_name, const uint32_t img_size, uint8_t *img_map)
{
    FIL file;
    UINT bytes_read = 0U;
    FRESULT result;

    lv_image_header_t file_header;

    char path[128];
    sprintf(path, "%s/%s", image_file_directory, file_name);

    result = f_open(&file, path, FA_READ);

    if (FR_OK != result)
    {
        ui_notice_post(UI_NOTICE_IMAGE_LACK);
        return false;
    }
    const FSIZE_t expected_file_size = (FSIZE_t) sizeof(file_header) + img_size;

    if (f_size(&file) != expected_file_size)
    {
        f_close(&file);
        return false;
    }

    /* 先读取lvgl 二进制 lv_image_header_t */
    result = f_read(&file, &file_header, sizeof(file_header), &bytes_read);
    if (FR_OK != result || file_header.w * file_header.h * 3 != img_size)
    {
        f_close(&file);
        return false;
    }

    result = f_read(&file, img_map, img_size, &bytes_read);
    f_close(&file);

    if (result != FR_OK ||
        bytes_read != img_size)
    {
        /* 防止LVGL显示未完整加载的图片数据 */
        memset(img_map, 0, img_size);
        return false;
    }
    return true;
}
