//
// Created by 74222 on 2026/7/16.
//

#include "../Inc/uart_device.h"
#include "main.h"
static int stm32_uart_send(struct UART_Device *pUart_device,const uint8_t *datas,uint16_t length)
{
    HAL_UART_Transmit_DMA((UART_HandleTypeDef *)pUart_device->target_uart,datas,length);

}
struct UART_Device fpga_device =
{
    "fpga_device",
    stm32_uart_send,
    100,
    &huart2
};
struct UART_Device printer_device =
{
    "printer_device",
    stm32_uart_send,
    100,
    &huart4
};