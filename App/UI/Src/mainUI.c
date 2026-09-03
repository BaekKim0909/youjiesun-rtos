#include "mainUI.h"
#include "cmsis_os.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"
#include "lcd_screen.h"
#include "../../System/Inc/system_structs.h"
#include "header.h"
#include "style_g.h"
#include "translations.h"

extern lv_indev_t *indev_keypad;
extern lv_group_t *navigation_group;

extern void load_start_test_page(void);

extern void update_selected_measure_standard_list(void);

LV_IMAGE_DECLARE(test_nav_focused_image);
LV_IMAGE_DECLARE(lightning_light_image);
LV_IMAGE_DECLARE(lightning_dark_image);

static lv_obj_t *Container = NULL;
Setting_t user_setting_g = {0};
Setting_t default_setting_g = {0, 0, 4};

Page_Index current_page_index_g = START_MEASURE_PAGE;

// 测试界面组
lv_group_t *measure_page_group = NULL;
// 测试标准界面组
lv_group_t *test_standard_page_group = NULL;
// 闪电动画
const lv_image_dsc_t *lightning_anim_g[2] = {
    &lightning_light_image,
    &lightning_dark_image
};

// 初始化组
static void group_init(void);

// lvgl初始化
void lvgl_init()
{
    lv_init();
    // 为lvgl设置时钟接口
    lv_tick_set_cb(xTaskGetTickCount);
    // 屏幕接口初始化
    lv_port_disp_init();
    // 按键输入接口初始化
    lv_port_indev_init();
}

// 首次加载
void ui_first_load()
{
    style_init();
    group_init();
    // 添加翻译
    lv_translation_add_static(languages, tags, translations);
    lv_translation_set_language("CN");

    header_init();
    header_load();


    update_selected_measure_standard_list();

    container_init();
    load_start_test_page();
    lv_indev_set_group(indev_keypad, measure_page_group);
    lv_group_focus_obj(lv_group_get_obj_by_index(measure_page_group, 0));

    footer_init();
    footer_load();
    lv_obj_t *home_nav_button = lv_group_get_obj_by_index(navigation_group, 0);
    lv_obj_t *img = lv_obj_get_child_by_type(home_nav_button, 0, &lv_image_class);
    lv_obj_set_style_border_color(home_nav_button, lv_color_hex(0xFFD83B), LV_PART_MAIN);
    lv_obj_set_style_bg_color(home_nav_button, lv_color_hex(0x0E2A4D), LV_PART_MAIN);
    lv_image_set_src(img, &test_nav_focused_image);


    // 设置屏幕亮度
    lcd_screen_set_bg_brightness(4);
}

lv_obj_t *container_get(void)
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

void container_mid_init(void)
{
    Container = lv_obj_create(lv_screen_active());
    lv_obj_set_size(Container, 800, 440);
    lv_obj_set_pos(Container, 0, 40);
    lv_obj_set_style_bg_color(Container, lv_color_hex(0x123867), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(Container, LV_OPA_COVER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(Container, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(Container, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(Container, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_flag(Container, LV_OBJ_FLAG_SCROLLABLE);
}

// Container 销毁
void container_dispose(void)
{
    if (Container != NULL)
    {
        lv_obj_delete(Container);
        Container = NULL;
    }
}

void group_init(void)
{
    measure_page_group = lv_group_create();
    test_standard_page_group = lv_group_create();
}
