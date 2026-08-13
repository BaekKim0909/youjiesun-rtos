//
// Created by 74222 on 2026/8/13.
//

#ifndef YOUJIESUN_NOTICE_MESSAGE_H
#define YOUJIESUN_NOTICE_MESSAGE_H

typedef enum
{
    NOTICE_SUCCESS = 0, /* 成功 */
    NOTICE_WARNING, /* 警告 */
    NOTICE_ERROR /* 失败 */
} result_t;

typedef struct
{
    /* data */
    result_t result_type;
    const char *message;
} notice_message_t;

void show_notice_message_box(notice_message_t notice_message);
#endif //YOUJIESUN_NOTICE_MESSAGE_H
