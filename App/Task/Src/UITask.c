#include "cmsis_os2.h"
#include "mainUI.h"
#include "lvgl.h"
//
// Created by 74222 on 2026/7/14.
//
void StartUITask(void *argument)
{
	lvgl_init();
	ui_first_load();
	for (;;)
	{
		lv_timer_handler();
		osDelay(5);
	}
}