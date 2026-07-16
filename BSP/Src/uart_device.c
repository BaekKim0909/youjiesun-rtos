//
// Created by 74222 on 2026/7/16.
//

#include "../Inc/uart_device.h"
#include "main.h"

#define RECEIVE_BUFFER_SIZE_OF_FPGA 128
// 结构体声明
typedef struct
{
    UART_HandleTypeDef * uart_handle;
    osSemaphoreId_t* tx_semaphore;
    uint8_t rx_datas[RECEIVE_BUFFER_SIZE_OF_FPGA];
}UART_Data;

// 串口发送函数
static int stm32_uart_send(struct UART_Device *pUart_device,const uint8_t *datas,uint16_t length,uint32_t timeout);



UART_Data fpga_device_uart_data =
{
    .uart_handle = &huart2,
    .tx_semaphore = &uart_tx_fpga_semaphoreHandle,
    .rx_datas = {0}
};

// fpga串口通讯设备定义
UART_Device fpga_device =
{
    "fpga_device",
    stm32_uart_send,
    &fpga_device_uart_data
};

// 打印机串口通讯设备定义
UART_Device printer_device =
{
    "printer_device",
    stm32_uart_send,
    &huart4
};
static int stm32_dma_uart_init(UART_Device *pUart_device)
{
    UART_Data *uart_data = pUart_device->uart_data;
    HAL_UARTEx_ReceiveToIdle_DMA(uart_data->uart_handle,uart_data->rx_datas,RECEIVE_BUFFER_SIZE_OF_FPGA);
    return 0;
}

static int stm32_uart_send(UART_Device *pUart_device,const uint8_t *datas,uint16_t length,uint32_t timeout)
{
    UART_Data *uart_data = pUart_device->uart_data;
    if (HAL_OK != HAL_UART_Transmit_DMA(uart_data->uart_handle,datas,length))
    {
        // 串口DMA发送失败
        return -1;
    }
    // 等待发送完毕:等待信号量
    if (osOK != osSemaphoreAcquire(*(uart_data->tx_semaphore),timeout))
    {
        // 发送失败
        return -1;
    }
    return 0;
}
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart == ((UART_Data *)fpga_device.uart_data)->uart_handle)
    {
        // 释放信号量
        osSemaphoreRelease(*((UART_Data *)fpga_device.uart_data)->tx_semaphore);
    }
}
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    
}