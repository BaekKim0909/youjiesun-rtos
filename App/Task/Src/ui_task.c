#include "cmsis_os2.h"
#include "mainUI.h"
#include "lvgl.h"
#include "notice_message.h"
#include "ui_task.h"

#include "dielectric_loss_test_page.h"
#include "heat_page.h"
#include "main.h"
//
// Created by 74222 on 2026/7/14.
//
extern lv_indev_t *indev_keypad;

// 将UI通知原因转换为提示信息
static notice_message_t ui_notice_reason_to_notice_message(ui_notice_reason_enum reason);

// 处理其他任务发送给UITask的事件
static void ui_process_events(void);

void StartUITask(void *argument)
{
    lvgl_init();
    ui_first_load();
    for (;;)
    {
        ui_process_events();
        lv_timer_handler();
        osDelay(16);
    }
}

static void ui_process_events(void)
{
    ui_event_t event;

    while (xQueueReceive(ui_event_queue, &event, 0U) == pdPASS)
    {
        switch (event.event_type)
        {
            case UI_EVENT_SHOW_NOTICE:
            {
                const notice_message_t notice_message = ui_notice_reason_to_notice_message(
                    event.event_data.notice_reason);
                show_notice_message_box(notice_message);
                break;
            }
            case UI_EVENT_LOAD_HEAT_PAGE:
                container_dispose();
                container_mid_init();
                lv_indev_set_group(indev_keypad, NULL);
                footer_dispose();
                load_heat_page(event.event_data.page_params.standard_name,
                               event.event_data.page_params.template,
                               event.event_data.page_params.rho_param);
                break;
            case UI_EVENT_LOAD_DIELECTRIC_LOSS_TEST_PAGE:
                container_dispose();
                container_mid_init();
                lv_indev_set_group(indev_keypad, NULL);
                load_dielectric_loss_test_page(event.event_data.page_params.standard_name,
                                               event.event_data.page_params.template,
                                               event.event_data.page_params.rho_param,
                                               event.event_data.page_params.ac_voltage);
                break;
            default:
                break;
        }
    }
}

static notice_message_t ui_notice_reason_to_notice_message(const ui_notice_reason_enum reason)
{
    notice_message_t notice_message = {};
    switch (reason)
    {
        case UI_NOTICE_COMM_ERROR:
        {
            notice_message.message = lv_translation_get("communicate_error");
            notice_message.result_type = NOTICE_ERROR;
            return notice_message;
        }
        case UI_NOTICE_STOP_FAILED:
        {
            notice_message.message = lv_translation_get("stop_test_failed");
            notice_message.result_type = NOTICE_ERROR;
            return notice_message;
        }
        default:
        {
            notice_message.message = "UNKNOWN_ERROR";
            notice_message.result_type = NOTICE_ERROR;
            return notice_message;
        }
    }
}

bool ui_notice_post(const ui_notice_reason_enum reason)
{
    if (ui_event_queue == NULL)
    {
        return false;
    }

    const ui_event_t event = {
        .event_type = UI_EVENT_SHOW_NOTICE,
        .event_data.notice_reason = reason
    };

    return xQueueOverwrite(ui_event_queue, &event) == pdPASS;
}

bool ui_submit_request(const ui_event_t *event)
{
    if (ui_event_queue == NULL)
    {
        return false;
    }
    return xQueueOverwrite(ui_event_queue, event) == pdPASS;
}
