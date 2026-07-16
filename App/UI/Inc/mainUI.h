#ifndef MAINUI_H
#define MAINUI_H
#include "lvgl.h"
// lvgl初始化
void lvgl_init(void);
// 首次加载
void ui_first_load(void);
// 返回Container
lv_obj_t * container_get(void);
// 容器初始化
void container_init(void);
#endif
