//
// Created by 74222 on 2026/8/17.
//

#ifndef YOUJIESUN_UI_EVENT_H
#define YOUJIESUN_UI_EVENT_H

#include <stdbool.h>

/**
 * @brief UI通知原因
 *
 * 事件只传递原因，不携带字符串和LVGL对象。
 * 具体文案和显示样式由UITask决定。
 */
typedef enum
{
    UI_NOTICE_COMM_ERROR, /* UART发送失败 */
} ui_notice_reason_enum;

/**
 * @brief UI事件类型
 */
typedef enum
{
    UI_EVENT_SHOW_NOTICE = 0 // 弹窗提示
} ui_event_enum;

/**
 * @brief 发送给UITask的事件
 */
typedef struct
{
    ui_event_enum event_type;

    union
    {
        ui_notice_reason_enum notice_reason;
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

/**
 * @brief 尝试读取一条UI事件
 *
 * 仅供UITask调用，接口为非阻塞读取。
 *
 * @return true 成功读取事件
 * @return false 当前没有事件、队列未初始化或参数无效
 */
bool ui_event_try_receive(ui_event_t *event);

#endif //YOUJIESUN_UI_EVENT_H
