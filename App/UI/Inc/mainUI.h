#ifndef MAINUI_H
#define MAINUI_H
#include "lvgl.h"
#include "notice_message.h"
#include "footer.h"

typedef enum
{
    START_MEASURE_PAGE = 0,
    DATA_HANDLE_PAGE,
    TEST_STANDARD_PAGE,
    ELECTRODE_PAGE,
    SETTING_PAGE,
    INFO_PAGE,
} Page_Index;

extern Page_Index current_page_index_g;

extern lv_group_t *measure_page_group;
extern lv_group_t *test_standard_page_group;

// lvgl初始化
void lvgl_init(void);

// 首次加载
void ui_first_load(void);

// 返回Container
lv_obj_t *container_get(void);

// 容器初始化
void container_init(void);

// Container 销毁
void container_dispose(void);
#endif
