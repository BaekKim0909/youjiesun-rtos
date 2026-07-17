//
// Created by 74222 on 2026/7/16.
//

#ifndef YOUJIESUN_UART_DEVICE_H
#define YOUJIESUN_UART_DEVICE_H
#include <stdint.h>
typedef struct UART_Device UART_Device;
struct UART_Device
{
    char* name;
    int (*init)(UART_Device* uart_device_p);
    int (*send_data)(const UART_Device *pUart_device,const uint8_t *datas,uint16_t length,uint32_t timeout); // 发送函数
    void *uart_data;
};
#endif //YOUJIESUN_UART_DEVICE_H
