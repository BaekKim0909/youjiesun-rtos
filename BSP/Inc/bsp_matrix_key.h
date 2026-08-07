//
// Created by 74222 on 2026/8/7.
//

#ifndef YOUJIESUN_BSP_MATRIX_KEY_H
#define YOUJIESUN_BSP_MATRIX_KEY_H

#include <stdint.h>

/*
 * 业务按键编码。
 * 该枚举只描述按键含义，不包含具体的矩阵行列位置和 LVGL 键值。
 */
typedef enum
{
    KEY_TEST = 0,   /* Test按钮 */
    KEY_LEFT,       /* 左 */
    KEY_RIGHT,      /* 右 */
    KEY_RETURN,     /* 返回 */
    KEY_ENTER,      /* 确认 */
    KEY_1,
    KEY_2,
    KEY_3,
    KEY_DELETE,     /* 删除 */
    KEY_UP,         /* 上 */
    KEY_4,
    KEY_5,
    KEY_6,
    KEY_0,
    KEY_DOWN,       /* 下 */
    KEY_7,
    KEY_8,
    KEY_9,
    KEY_DOT,        /* 小数点 */
    KEY_NONE
} KEY_CODE;

/* 稳定按键状态，供 LVGL 输入回调读取。 */
typedef enum
{
    KEY_STATE_RELEASED = 0,
    KEY_STATE_PRESSED
} KEY_STATE;

/*
 * 对外发布的按键快照。
 * 松开时 code 仍保留最后一次按下的键值，便于 LVGL 配对按下和释放状态。
 */
typedef struct
{
    KEY_CODE code;
    KEY_STATE state;
} KEY_DATA;

/*
 * 初始化矩阵键盘扫描状态。
 * 调用前必须先由 MX_GPIO_Init() 完成行列 GPIO 配置。
 */
void bsp_matrix_key_init(void);

/*
 * 执行一次完整的 4×5 矩阵扫描，并更新消抖状态机。
 * 该函数由 KeyScanTask 每 5 ms 调用一次，其他任务不应调用。
 */
void bsp_matrix_key_scan(void);

/*
 * 获取当前稳定按键快照。
 * 此函数不访问 GPIO、不阻塞，可由 LVGL 所在任务直接调用。
 */
KEY_DATA bsp_matrix_key_get_state(void);

#endif //YOUJIESUN_BSP_MATRIX_KEY_H
