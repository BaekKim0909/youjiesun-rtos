#include "mainUI.h"
#include "cmsis_os.h"
#include "lv_port_disp.h"
#include "lcd_screen.h"
#include "system_structs.h"
#include "header.h"
#include "footer.h"
#include "style_g.h"
static lv_obj_t * Container = NULL;
Setting_t user_setting_g = { 0 };
Setting_t default_setting_g = {0,0,4};

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
    style_init();
	header_init();
    header_load();
	container_init();
    footer_init();
    footer_load();
	lcd_screen_set_bg_brightness(4);
}
lv_obj_t * container_get(void)
{
    return Container;
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