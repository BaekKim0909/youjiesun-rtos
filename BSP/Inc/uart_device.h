//
// Created by 74222 on 2026/7/16.
//

#ifndef YOUJIESUN_UART_DEVICE_H
#define YOUJIESUN_UART_DEVICE_H

#include <stdint.h>
#include "FreeRTOS.h"
#include "task.h"

#define FPGA_TX_EVENT  (1UL << 8)
#define FPGA_RX_EVENT  (1UL << 9)



typedef struct UART_Device UART_Device;

/**
 * @brief UART设备抽象接口
 *
 * 上层任务只通过这些函数操作串口，不直接访问HAL句柄、DMA缓冲区和环形缓冲区。
 * 当前fpga_device使用USART2，后续如果增加打印机串口，也可以复用同一套接口。
 */
struct UART_Device
{
    // 设备名称，主要用于调试和日志
    char *name;
    // 初始化发送同步对象，并启动DMA空闲接收
    int (*init)(UART_Device *uart_device_p);
    // 使用DMA发送数据，timeout单位为毫秒
    int (*send_data)(const UART_Device *uart_device_p, const uint8_t *datas, uint16_t length, uint32_t timeout);
    // 注册接收任务句柄，DMA中断收到数据后会通知这个任务
    void (*bind_rx_task)(UART_Device *uart_device_p, TaskHandle_t task_handle);
    // 从软件环形缓冲区中取出一包完整且CRC正确的数据
    uint16_t (*get_command)(const UART_Device *uart_device_p, uint8_t *command, uint16_t command_capacity);
    // 指向具体串口设备的私有数据，上层不应直接访问
    void *uart_data;
};

#endif //YOUJIESUN_UART_DEVICE_H
