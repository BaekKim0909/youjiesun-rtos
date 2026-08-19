//
// Created by 74222 on 2026/8/17.
//

#ifndef YOUJIESUN_UI_EVENT_H
#define YOUJIESUN_UI_EVENT_H

#include <stdbool.h>

#include "test_standard.h"

/**
 * @brief UI通知原因
 *
 * 事件只传递原因，不携带字符串和LVGL对象。
 * 具体文案和显示样式由UITask决定。
 */
typedef enum
{
    UI_NOTICE_COMM_ERROR, /* 通用通信异常 */
    UI_NOTICE_STOP_FAILED /* 停止指令失败，设备状态未知 */
} ui_notice_reason_enum;

/**
 * @brief UI事件类型
 */
typedef enum
{
    UI_EVENT_SHOW_NOTICE = 0, // 弹窗提示
    UI_EVENT_LOAD_HEAT_PAGE, // 加载加热界面
} ui_event_enum;

typedef struct
{
    char standard_name[24];
    test_standard_type template;
    uint16_t fill_num;
    uint16_t rho_param;
} ui_page_params_t;

/**
 * @brief 发送给UITask的事件
 */
typedef struct
{
    ui_event_enum event_type;

    union
    {
        ui_notice_reason_enum notice_reason;
        ui_page_params_t page_params;
    } event_data;
} ui_event_t;

/**
 * @brief 初始化UI事件队列
 *
 * 必须在创建UITask和其他事件生产任务之前调用。
 *
 * @return true 初始化成功或已经初始化
 * @return false 队列创建失败
 */
bool ui_event_init(void);

/**
 * @brief 发布一条UI通知
 *
 * 该接口不会调用任何LVGL函数，也不会阻塞调用任务。
 * 当前队列保存最新一条通知，新通知会覆盖尚未消费的旧通知。
 *
 * @return true 通知已保存
 * @return false 队列尚未初始化或通知原因无效
 */
bool ui_notice_post(ui_notice_reason_enum reason);

/*
 * @brief 提交 UI 更新请求
 */
bool ui_submit_request(const ui_event_t *event);
#endif //YOUJIESUN_UI_EVENT_H
