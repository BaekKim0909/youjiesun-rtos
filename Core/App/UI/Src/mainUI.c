#include "mainUI.h"
#include "lvgl.h"
#include "cmsis_os.h"
#include "lv_port_disp.h"
#include "header.h"
#include "lcd_screen.h"

lv_obj_t * Container = NULL;
// lvgl初始化
void lvgl_init()
{
    lv_init();
    // 为lvgl设置时钟接口
    lv_tick_set_cb(xTaskGetTickCount);
	// 屏幕接口初始化
    lv_port_disp_init();
}
// 首次加载
void ui_first_load()
{
	header_init();
	container_init();
	lcd_screen_set_bg_brightness(4);
}
void container_init()
{
	Container = lv_obj_create(lv_screen_active());
	lv_obj_set_size(Container, 800, 400);
	lv_obj_set_pos(Container, 0, 40);
	lv_obj_set_style_bg_color(Container, lv_color_hex(0x123867), LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(Container, LV_OPA_COVER, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_radius(Container, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_border_width(Container, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_pad_all(Container, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_remove_flag(Container, LV_OBJ_FLAG_SCROLLABLE);
}