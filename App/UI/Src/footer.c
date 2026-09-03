//
// Created by 74222 on 2026/7/15.
//

#include "../Inc/footer.h"
#include "style_g.h"
#include "core/lv_group_private.h"
#include "mainUI.h"

LV_IMAGE_DECLARE(test_nav_normal_image);
LV_IMAGE_DECLARE(test_nav_focused_image);
LV_IMAGE_DECLARE(data_record_nav_normal_image);
LV_IMAGE_DECLARE(data_record_nav_focused_image);
LV_IMAGE_DECLARE(test_standard_nav_normal_image);
LV_IMAGE_DECLARE(test_standard_nav_focused_image);
LV_IMAGE_DECLARE(electrode_nav_normal_image);
LV_IMAGE_DECLARE(electrode_nav_focused_image);
LV_IMAGE_DECLARE(setting_nav_normal_image);
LV_IMAGE_DECLARE(setting_nav_focused_image);
LV_IMAGE_DECLARE(info_nav_normal_image);
LV_IMAGE_DECLARE(info_nav_focused_image);


static lv_obj_t *footer = NULL;
lv_group_t *navigation_group = NULL;


extern lv_indev_t *indev_keypad;

extern void load_start_test_page(void);

extern void load_test_standard_page(void);

// 导航栏按钮聚焦事件
static void navigation_bar_button_focus(lv_event_t *e);

//导航栏按钮移去焦点事件
static void navigation_bar_button_defocus(lv_event_t *e);

//导航栏按钮点击事件
static void navigation_bar_button_clicked(lv_event_t *e);

// 将导航栏的所有导航项恢复默认样式
static void navigation_bar_set_normal(void);

lv_obj_t *footer_get()
{
    return footer;
}

// 初始化footer
void footer_init(void)
{
    footer = lv_obj_create(lv_screen_active());
    lv_obj_set_size(footer, 800, 40);
    lv_obj_set_pos(footer, 0, 440);
    lv_obj_set_style_bg_color(footer, lv_color_hex(0x123867), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(footer, LV_OPA_COVER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(footer, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(footer, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(footer, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_flag(footer, LV_OBJ_FLAG_SCROLLABLE);
    if (navigation_group == NULL)
        navigation_group = lv_group_create();
}

void footer_load()
{
    lv_obj_t *home_page_btn = lv_button_create(footer);
    lv_obj_set_name(home_page_btn, "home_nav");

    lv_obj_t *data_record_page_btn = lv_button_create(footer);
    lv_obj_set_name(data_record_page_btn, "data_record_nav");

    lv_obj_t *test_standard_page_btn = lv_button_create(footer);
    lv_obj_set_name(test_standard_page_btn, "test_standard_nav");

    lv_obj_t *electrode_page_btn = lv_button_create(footer);
    lv_obj_set_name(electrode_page_btn, "electrode_nav");

    lv_obj_t *setting_page_btn = lv_button_create(footer);
    lv_obj_set_name(setting_page_btn, "setting_nav");

    lv_obj_t *info_page_btn = lv_button_create(footer);
    lv_obj_set_name(info_page_btn, "info_nav");

    lv_obj_t *home_btn_img = lv_image_create(home_page_btn);
    lv_obj_t *dataRecord_btn_img = lv_image_create(data_record_page_btn);
    lv_obj_t *testStandard_btn_img = lv_image_create(test_standard_page_btn);
    lv_obj_t *electrode_btn_img = lv_image_create(electrode_page_btn);
    lv_obj_t *setting_btn_img = lv_image_create(setting_page_btn);
    lv_obj_t *info_btn_img = lv_image_create(info_page_btn);

    lv_obj_set_size(home_page_btn, 133, 40);
    lv_obj_set_size(data_record_page_btn, 133, 40);
    lv_obj_set_size(test_standard_page_btn, 133, 40);
    lv_obj_set_size(electrode_page_btn, 133, 40);
    lv_obj_set_size(setting_page_btn, 133, 40);
    lv_obj_set_size(info_page_btn, 133, 40);

    lv_obj_set_size(home_btn_img, 30, 30);
    lv_obj_set_size(dataRecord_btn_img, 30, 30);
    lv_obj_set_size(testStandard_btn_img, 30, 30);
    lv_obj_set_size(electrode_btn_img, 30, 30);
    lv_obj_set_size(setting_btn_img, 30, 30);
    lv_obj_set_size(info_btn_img, 30, 30);

    lv_obj_set_pos(home_page_btn, 0, 0);
    lv_obj_align_to(data_record_page_btn, home_page_btn, LV_ALIGN_OUT_RIGHT_MID, 2, 0);
    lv_obj_align_to(test_standard_page_btn, data_record_page_btn, LV_ALIGN_OUT_RIGHT_MID, 2, 0);
    lv_obj_align_to(electrode_page_btn, test_standard_page_btn, LV_ALIGN_OUT_RIGHT_MID, 2, 0);
    lv_obj_align_to(setting_page_btn, electrode_page_btn, LV_ALIGN_OUT_RIGHT_MID, 2, 0);
    lv_obj_align_to(info_page_btn, setting_page_btn, LV_ALIGN_OUT_RIGHT_MID, 2, 0);

    lv_obj_add_style(home_page_btn, &nav_btn_style_g, LV_PART_MAIN);
    lv_obj_add_style(data_record_page_btn, &nav_btn_style_g, LV_PART_MAIN);
    lv_obj_add_style(test_standard_page_btn, &nav_btn_style_g, LV_PART_MAIN);
    lv_obj_add_style(electrode_page_btn, &nav_btn_style_g, LV_PART_MAIN);
    lv_obj_add_style(setting_page_btn, &nav_btn_style_g, LV_PART_MAIN);
    lv_obj_add_style(info_page_btn, &nav_btn_style_g, LV_PART_MAIN);

    lv_obj_set_style_bg_color(home_page_btn, lv_color_hex(0x0E2A4D), LV_PART_MAIN);
    lv_obj_set_style_bg_color(data_record_page_btn, lv_color_hex(0x0E2A4D), LV_PART_MAIN);
    lv_obj_set_style_bg_color(test_standard_page_btn, lv_color_hex(0x0E2A4D), LV_PART_MAIN);
    lv_obj_set_style_bg_color(electrode_page_btn, lv_color_hex(0x0E2A4D), LV_PART_MAIN);
    lv_obj_set_style_bg_color(setting_page_btn, lv_color_hex(0x0E2A4D), LV_PART_MAIN);
    lv_obj_set_style_bg_color(info_page_btn, lv_color_hex(0x0E2A4D), LV_PART_MAIN);

    lv_image_set_src(home_btn_img, &test_nav_normal_image);
    lv_image_set_src(dataRecord_btn_img, &data_record_nav_normal_image);
    lv_image_set_src(testStandard_btn_img, &test_standard_nav_normal_image);
    lv_image_set_src(electrode_btn_img, &electrode_nav_normal_image);
    lv_image_set_src(setting_btn_img, &setting_nav_normal_image);
    lv_image_set_src(info_btn_img, &info_nav_normal_image);

    lv_obj_center(home_btn_img);
    lv_obj_center(dataRecord_btn_img);
    lv_obj_center(testStandard_btn_img);
    lv_obj_center(electrode_btn_img);
    lv_obj_center(setting_btn_img);
    lv_obj_center(info_btn_img);

    lv_group_remove_all_objs(navigation_group);
    lv_group_add_obj(navigation_group, home_page_btn);
    lv_group_add_obj(navigation_group, data_record_page_btn);
    lv_group_add_obj(navigation_group, test_standard_page_btn);
    lv_group_add_obj(navigation_group, electrode_page_btn);
    lv_group_add_obj(navigation_group, setting_page_btn);
    lv_group_add_obj(navigation_group, info_page_btn);

    lv_obj_add_event_cb(home_page_btn, navigation_bar_button_clicked, LV_EVENT_CLICKED, (void *) START_MEASURE_PAGE);
    lv_obj_add_event_cb(data_record_page_btn, navigation_bar_button_clicked, LV_EVENT_CLICKED,
                        (void *) DATA_HANDLE_PAGE);
    lv_obj_add_event_cb(test_standard_page_btn, navigation_bar_button_clicked, LV_EVENT_CLICKED,
                        (void *) TEST_STANDARD_PAGE);
    lv_obj_add_event_cb(electrode_page_btn, navigation_bar_button_clicked, LV_EVENT_CLICKED, (void *) ELECTRODE_PAGE);
    lv_obj_add_event_cb(setting_page_btn, navigation_bar_button_clicked, LV_EVENT_CLICKED, (void *) SETTING_PAGE);
    lv_obj_add_event_cb(info_page_btn, navigation_bar_button_clicked, LV_EVENT_CLICKED, (void *) INFO_PAGE);

    lv_obj_add_event_cb(home_page_btn, navigation_bar_button_focus, LV_EVENT_FOCUSED, (void *) START_MEASURE_PAGE);
    lv_obj_add_event_cb(data_record_page_btn, navigation_bar_button_focus, LV_EVENT_FOCUSED, (void *) DATA_HANDLE_PAGE);
    lv_obj_add_event_cb(test_standard_page_btn, navigation_bar_button_focus, LV_EVENT_FOCUSED,
                        (void *) TEST_STANDARD_PAGE);
    lv_obj_add_event_cb(electrode_page_btn, navigation_bar_button_focus, LV_EVENT_FOCUSED, (void *) ELECTRODE_PAGE);
    lv_obj_add_event_cb(setting_page_btn, navigation_bar_button_focus, LV_EVENT_FOCUSED, (void *) SETTING_PAGE);
    lv_obj_add_event_cb(info_page_btn, navigation_bar_button_focus, LV_EVENT_FOCUSED, (void *) INFO_PAGE);

    lv_obj_add_event_cb(home_page_btn, navigation_bar_button_defocus, LV_EVENT_DEFOCUSED, (void *) START_MEASURE_PAGE);
    lv_obj_add_event_cb(data_record_page_btn, navigation_bar_button_defocus, LV_EVENT_DEFOCUSED,
                        (void *) DATA_HANDLE_PAGE);
    lv_obj_add_event_cb(test_standard_page_btn, navigation_bar_button_defocus, LV_EVENT_DEFOCUSED,
                        (void *) TEST_STANDARD_PAGE);
    lv_obj_add_event_cb(electrode_page_btn, navigation_bar_button_defocus, LV_EVENT_DEFOCUSED, (void *) ELECTRODE_PAGE);
    lv_obj_add_event_cb(setting_page_btn, navigation_bar_button_defocus, LV_EVENT_DEFOCUSED, (void *) SETTING_PAGE);
    lv_obj_add_event_cb(info_page_btn, navigation_bar_button_defocus, LV_EVENT_DEFOCUSED, (void *) INFO_PAGE);
}

// 销毁footer
void footer_dispose(void)
{
    if (footer != NULL)
    {
        if (navigation_group != NULL)
        {
            lv_group_remove_all_objs(navigation_group);
        }

        lv_obj_delete(footer);
        footer = NULL;
    }
}

/*
 * @brief 导航栏按钮聚焦事件
 */
static void navigation_bar_button_focus(lv_event_t *e)
{
    navigation_bar_set_normal();

    container_dispose();
    container_init();

    lv_obj_t *btn = lv_event_get_target(e);
    const Page_Index flag = (uint32_t) lv_event_get_user_data(e);
    if (flag == START_MEASURE_PAGE)
    {
        load_start_test_page();
        lv_obj_set_style_bg_color(btn, lv_color_hex(0x2D96FF), LV_PART_MAIN);
    }
    else if (flag == DATA_HANDLE_PAGE)
    {
        lv_obj_set_style_bg_color(btn, lv_color_hex(0x2D96FF), LV_PART_MAIN);
    }
    else if (flag == TEST_STANDARD_PAGE)
    {
        load_test_standard_page();
        lv_obj_set_style_bg_color(btn, lv_color_hex(0x2D96FF), LV_PART_MAIN);
    }
    else if (flag == ELECTRODE_PAGE)
    {
        lv_obj_set_style_bg_color(btn, lv_color_hex(0x2D96FF), LV_PART_MAIN);
    }
    else if (flag == SETTING_PAGE)
    {
        lv_obj_set_style_bg_color(btn, lv_color_hex(0x2D96FF), LV_PART_MAIN);
    }
    else if (flag == INFO_PAGE)
    {
        lv_obj_set_style_bg_color(btn, lv_color_hex(0x2D96FF), LV_PART_MAIN);
    }
}

/*
 * @brief 导航栏按钮移去焦点事件
 */
static void navigation_bar_button_defocus(lv_event_t *e)
{
    navigation_bar_set_normal();

    lv_obj_t *btn = lv_event_get_target(e);
    const Page_Index flag = (uint32_t) lv_event_get_user_data(e);
    if (flag == START_MEASURE_PAGE)
    {
        lv_obj_set_style_bg_color(btn, lv_color_hex(0x0E2A4D), LV_PART_MAIN);
    }
    else if (flag == DATA_HANDLE_PAGE)
    {
        lv_obj_set_style_bg_color(btn, lv_color_hex(0x0E2A4D), LV_PART_MAIN);
    }
    else if (flag == TEST_STANDARD_PAGE)
    {
        lv_obj_set_style_bg_color(btn, lv_color_hex(0x0E2A4D), LV_PART_MAIN);
    }
    else if (flag == ELECTRODE_PAGE)
    {
        lv_obj_set_style_bg_color(btn, lv_color_hex(0x0E2A4D), LV_PART_MAIN);
    }
    else if (flag == SETTING_PAGE)
    {
        lv_obj_set_style_bg_color(btn, lv_color_hex(0x0E2A4D), LV_PART_MAIN);
    }
    else if (flag == INFO_PAGE)
    {
        lv_obj_set_style_bg_color(btn, lv_color_hex(0x0E2A4D), LV_PART_MAIN);
    }
}

static void navigation_bar_set_normal(void)
{
    lv_obj_t *home_page_btn = lv_obj_find_by_name(footer, "home_nav");
    lv_obj_t *data_record_page_btn = lv_obj_find_by_name(footer, "data_record_nav");
    lv_obj_t *test_standard_page_btn = lv_obj_find_by_name(footer, "test_standard_nav");
    lv_obj_t *electrode_page_btn = lv_obj_find_by_name(footer, "electrode_nav");
    lv_obj_t *setting_page_btn = lv_obj_find_by_name(footer, "setting_nav");
    lv_obj_t *info_page_btn = lv_obj_find_by_name(footer, "info_nav");

    lv_obj_t *home_btn_img = lv_obj_get_child_by_type(home_page_btn, 0, &lv_image_class);
    lv_obj_t *dataRecord_btn_img = lv_obj_get_child_by_type(data_record_page_btn, 0, &lv_image_class);
    lv_obj_t *testStandard_btn_img = lv_obj_get_child_by_type(test_standard_page_btn, 0, &lv_image_class);
    lv_obj_t *electrode_btn_img = lv_obj_get_child_by_type(electrode_page_btn, 0, &lv_image_class);
    lv_obj_t *setting_btn_img = lv_obj_get_child_by_type(setting_page_btn, 0, &lv_image_class);
    lv_obj_t *info_btn_img = lv_obj_get_child_by_type(info_page_btn, 0, &lv_image_class);

    lv_image_set_src(home_btn_img, &test_nav_normal_image);
    lv_obj_set_style_border_color(home_page_btn, lv_color_hex(0x7AA5DB), LV_PART_MAIN);

    lv_image_set_src(dataRecord_btn_img, &data_record_nav_normal_image);
    lv_obj_set_style_border_color(data_record_page_btn, lv_color_hex(0x7AA5DB), LV_PART_MAIN);

    lv_image_set_src(testStandard_btn_img, &test_standard_nav_normal_image);
    lv_obj_set_style_border_color(test_standard_page_btn, lv_color_hex(0x7AA5DB), LV_PART_MAIN);

    lv_image_set_src(electrode_btn_img, &electrode_nav_normal_image);
    lv_obj_set_style_border_color(electrode_page_btn, lv_color_hex(0x7AA5DB), LV_PART_MAIN);

    lv_image_set_src(setting_btn_img, &setting_nav_normal_image);
    lv_obj_set_style_border_color(setting_page_btn, lv_color_hex(0x7AA5DB), LV_PART_MAIN);

    lv_image_set_src(info_btn_img, &info_nav_normal_image);
    lv_obj_set_style_border_color(info_page_btn, lv_color_hex(0x7AA5DB), LV_PART_MAIN);
}

//导航栏按钮点击事件
static void navigation_bar_button_clicked(lv_event_t *e)
{
    lv_obj_t *btn = lv_event_get_target(e);
    lv_obj_t *img = lv_obj_get_child_by_type(btn, 0, &lv_image_class);
    const Page_Index flag = (uint32_t) lv_event_get_user_data(e);
    if (flag == START_MEASURE_PAGE)
    {
        lv_obj_set_style_border_color(btn, lv_color_hex(0xFFD83B), LV_PART_MAIN);
        lv_obj_set_style_bg_color(btn, lv_color_hex(0x0E2A4D), LV_PART_MAIN);
        lv_image_set_src(img, &test_nav_focused_image);

        lv_indev_set_group(indev_keypad, measure_page_group);
        lv_group_focus_obj(lv_group_get_obj_by_index(measure_page_group, 0));
    }
    else if (flag == DATA_HANDLE_PAGE)
    {
        lv_obj_set_style_border_color(btn, lv_color_hex(0xFFD83B), LV_PART_MAIN);
        lv_obj_set_style_bg_color(btn, lv_color_hex(0x0E2A4D), LV_PART_MAIN);
        lv_image_set_src(img, &data_record_nav_focused_image);
    }
    else if (flag == TEST_STANDARD_PAGE)
    {
        lv_obj_set_style_border_color(btn, lv_color_hex(0xFFD83B), LV_PART_MAIN);
        lv_obj_set_style_bg_color(btn, lv_color_hex(0x0E2A4D), LV_PART_MAIN);
        lv_image_set_src(img, &test_standard_nav_focused_image);

        lv_indev_set_group(indev_keypad, test_standard_page_group);
        lv_group_focus_obj(lv_group_get_obj_by_index(test_standard_page_group, 0));
    }
    else if (flag == ELECTRODE_PAGE)
    {
        lv_obj_set_style_border_color(btn, lv_color_hex(0xFFD83B), LV_PART_MAIN);
        lv_obj_set_style_bg_color(btn, lv_color_hex(0x0E2A4D), LV_PART_MAIN);
        lv_image_set_src(img, &electrode_nav_focused_image);
    }
    else if (flag == SETTING_PAGE)
    {
        lv_obj_set_style_border_color(btn, lv_color_hex(0xFFD83B), LV_PART_MAIN);
        lv_obj_set_style_bg_color(btn, lv_color_hex(0x0E2A4D), LV_PART_MAIN);
        lv_image_set_src(img, &setting_nav_focused_image);
    }
    else if (flag == INFO_PAGE)
    {
        lv_obj_set_style_border_color(btn, lv_color_hex(0xFFD83B), LV_PART_MAIN);
        lv_obj_set_style_bg_color(btn, lv_color_hex(0x0E2A4D), LV_PART_MAIN);
        lv_image_set_src(img, &info_nav_focused_image);
    }
}
