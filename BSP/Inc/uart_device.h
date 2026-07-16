//
// Created by 74222 on 2026/7/16.
//

#ifndef YOUJIESUN_UART_DEVICE_H
#define YOUJIESUN_UART_DEVICE_H
#include <stdint.h>

struct UART_Device
{
    char* name;
    int (*send_data)(struct UART_Device *pUart_device,const uint8_t *datas,uint16_t length); // 发送
    uint32_t timeout;
    void *target_uart;
};
#endif //YOUJIESUN_UART_DEVICE_H
