//
// Created by 74222 on 2026/7/14.
//

#include "header.h"

static lv_obj_t * header = NULL;
// 暴露外部引用
lv_obj_t *header_get()
{
	return header;
}
void header_init(void)
{
	header = lv_obj_create(lv_scr_act());
	lv_obj_set_size(header, 800, 40);
	lv_obj_set_pos(header, 0, 0);
	lv_obj_set_style_bg_color(header, lv_color_hex(0xF1F2F4), LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(header, LV_OPA_COVER, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_radius(header, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_border_width(header, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_pad_all(header, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_remove_flag(header, LV_PART_MAIN | LV_STATE_DEFAULT);
}