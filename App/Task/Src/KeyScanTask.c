//
// Created by 74222 on 2026/8/7.
//

#include "cmsis_os2.h"
#include "bsp_matrix_key.h"

/* FreeRTOS 时钟为 1 kHz，因此 5 个 tick 对应 5 ms。 */
#define KEY_SCAN_PERIOD_TICKS  5U

/*
 * 矩阵键盘扫描任务。
 * 该强符号实现会覆盖 freertos.c 中由 CubeMX 生成的同名弱实现。
 */
void StartKeyScanTask(void *argument)
{
    uint32_t next_wake_time;

    (void)argument;

    /* GPIO 已在调度器启动前初始化，这里复位矩阵扫描状态机。 */
    bsp_matrix_key_init();
    next_wake_time = osKernelGetTickCount();

    for (;;)
    {
        /* 每个任务周期完成一次完整的 4×5 扫描。 */
        bsp_matrix_key_scan();

        /*
         * osDelayUntil() 使用绝对 tick，可避免任务执行时间逐周期累积造成扫描漂移。
         */
        next_wake_time += KEY_SCAN_PERIOD_TICKS;
        osDelayUntil(next_wake_time);
    }
}
