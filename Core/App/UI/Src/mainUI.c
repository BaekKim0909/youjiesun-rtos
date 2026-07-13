#include "mainUI.h"
#include "lvgl.h"
#include "cmsis_os.h"
#include "lv_port_disp.h"
// lvgl初始化
void lvgl_init()
{
    lv_init();
    // 为lvgl设置时钟接口
    lv_tick_set_cb(xTaskGetTickCount);
    lv_port_disp_init();
}