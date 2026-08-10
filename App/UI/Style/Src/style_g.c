//
// Created by 74222 on 2026/7/15.
//

#include "../Inc/style_g.h"

// 导航栏按钮样式
lv_style_t nav_btn_style_g;
lv_style_t btn_focus_style_g;


// 导航栏按钮样式初始化
static void nav_button_style_init();

// 按钮样式初始化
static void button_style_init();

// 样式初始化 全局调用
void style_init()
{
    nav_button_style_init();
    button_style_init();
}

// 导航栏按钮初始化
void nav_button_style_init()
{
    lv_style_init(&nav_btn_style_g);
    lv_style_set_pad_all(&nav_btn_style_g, 0); // 内边距 0
    lv_style_set_radius(&nav_btn_style_g, 0); // 圆角 0
    lv_style_set_border_width(&nav_btn_style_g, 3);
    lv_style_set_border_color(&nav_btn_style_g, lv_color_hex(0x7AA5DB));
    lv_style_set_border_side(&nav_btn_style_g, LV_BORDER_SIDE_TOP);
    lv_style_set_border_opa(&nav_btn_style_g, LV_OPA_COVER);
    lv_style_set_outline_width(&nav_btn_style_g, 0);
}

void button_style_init()
{
    lv_style_init(&btn_focus_style_g);
    lv_style_set_bg_color(&btn_focus_style_g, lv_color_hex(0x0B54CD));
    lv_style_set_bg_grad_color(&btn_focus_style_g, lv_color_hex(0x00E8FD));
    lv_style_set_bg_grad_dir(&btn_focus_style_g, LV_GRAD_DIR_HOR);
    lv_style_set_outline_width(&btn_focus_style_g, 0);
}
