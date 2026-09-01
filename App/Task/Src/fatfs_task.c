//
// Created by 74222 on 2026/9/1.
//
#include "FreeRTOS.h"
#include "task.h"
#include "ff.h"
#include "fatfs.h"
#include "ui_task.h"

void start_fatfs_task(void *argument)
{
    if (FR_OK != f_mount(&SDFatFS, "0:", 1))
    {
        ui_notice_post(UI_NOTICE_SD_CARD_MOUNTED_FAILED);
    }
    for (;;)
    {
        vTaskDelay(1);
    }
}
