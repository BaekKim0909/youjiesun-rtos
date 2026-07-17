//
// Created by 74222 on 2026/7/16.
//

#include "../Inc/uart_device.h"

#include <string.h>

#include "main.h"


#define RECEIVE_BUFFER_SIZE_OF_FPGA 128
#define RING_BUFFER_SIZE_OF_FPGA (RECEIVE_BUFFER_SIZE_OF_FPGA * 2)
// 结构体声明

/**  串口信息数据   **/
typedef struct
{
    UART_HandleTypeDef * uart_handle;
    SemaphoreHandle_t tx_semaphore;
    uint8_t rx_datas[RECEIVE_BUFFER_SIZE_OF_FPGA];
}UART_Data;

typedef struct Ring_Buffer Ring_Buffer;
/**      环形缓冲区数据       **/
struct Ring_Buffer
{
    // 环形缓冲区数组
    uint8_t *buffer;
    // 环形缓冲区长度
    uint16_t buffer_size;
    // 缓冲区读锁引
    uint16_t read_index;
    // 缓冲区写索引
    uint16_t write_index;
    // 指令最小长度
    uint8_t command_min_len;
    // 缓冲区写入函数
    uint16_t (* buffer_write)(Ring_Buffer *ring_buffer,const uint8_t *data,uint16_t length);
    // 获取缓冲区中命令
    // uint8_t (* command_get)(uint8_t *command);
};

/***
 * @brief 增加读锁引
 * @param ring_buffer 目标缓冲区
 * @param length 要增加的长度
 */
static void ringBuffer_add_readIndex(Ring_Buffer *ring_buffer,uint16_t length);

/***
 *  @brief 读取第i位数据 超过缓冲区长度自动循环
 *  @return 环形缓冲区[i] 的内容
 */
static uint8_t ringBuffer_read(Ring_Buffer *ring_buffer,uint16_t i);

/**
 * @brief   计算未处理的数据长度
 * @return  返回未处理的数据长度
 * @retval  0 :缓冲区为空
 * @retval  1 ~ buffer_size-1 未处理的数据长度
 * @retval  buffer_size 缓冲区已满
 */
static uint16_t ringBuffer_get_length(Ring_Buffer *ring_buffer);

/**
 * @brief   计算缓冲区剩余控件
 * @return  返回剩余空间
 * @retval  0 :缓冲区已满
 * @retval  1 ~ buffer_size-1 剩余空间
 * @retval  buffer_size 缓冲区为空
 */
static uint16_t ringBuffer_get_remainLength(Ring_Buffer *ring_buffer);

/**
 * @brief           向缓冲区写入数据
 * @param ring_buffer
 * @param data      要写入的数据指针
 * @param length    要写入的数据长度
 * @return          写入的数据长度
 */
uint16_t ringBuffer_write(Ring_Buffer *ring_buffer, const uint8_t *data,uint16_t length);


/**
 * @brief               尝试获取一条指令
 * @param ring_buffer   目标缓冲区
 * @param command       要写入的指令数组
 * @return              获取的指令长度  0 没有获取到指令
 */
uint16_t ringBuffer_get_command(Ring_Buffer *ring_buffer,uint8_t *command);

/*********************************************************************************************************************/
// 串口初始化函数
static int stm32_dma_uart_init(UART_Device *uart_device_p);
// 串口发送函数
static int stm32_uart_send(const UART_Device *pUart_device,const uint8_t *datas,uint16_t length,uint32_t timeout);


// fpga串口通讯设备 串口数据
UART_Data fpga_device_uart_data =
{
    .uart_handle = &huart2,
    .rx_datas = {0}
};

// 与fpga串口通讯 设备定义
UART_Device fpga_device =
{
    .name ="fpga_device",
    .init = stm32_dma_uart_init,
    .send_data = stm32_uart_send,
    .uart_data = &fpga_device_uart_data
};
// 与fpga串口通讯 接收缓冲区定义
uint8_t ring_buffer_of_fpga[RING_BUFFER_SIZE_OF_FPGA];
Ring_Buffer fpga_ring_buffer =
{
    .buffer = ring_buffer_of_fpga,
    .buffer_size = RING_BUFFER_SIZE_OF_FPGA,
    .read_index = 0,
    .write_index = 0,
    .command_min_len = 10,
    .buffer_write = ringBuffer_write
};

// 打印机串口通讯设备定义
// UART_Device printer_device =
// {
//     "printer_device",
//
//     stm32_uart_send,
//     &huart4
// };
static int stm32_dma_uart_init(UART_Device *uart_device_p)
{
    UART_Data *uart_data = uart_device_p->uart_data;
    uart_data->tx_semaphore = xSemaphoreCreateBinary();
    HAL_UARTEx_ReceiveToIdle_DMA(uart_data->uart_handle,uart_data->rx_datas,RECEIVE_BUFFER_SIZE_OF_FPGA);
    return 0;
}

static int stm32_uart_send(const UART_Device *pUart_device,const uint8_t *datas,uint16_t length,uint32_t timeout)
{
    UART_Data *uart_data = pUart_device->uart_data;
    if (HAL_OK != HAL_UART_Transmit_DMA(uart_data->uart_handle,datas,length))
    {
        // 串口DMA发送失败
        return -1;
    }
    // 等待发送完毕:等待信号量
    if (pdTRUE != xSemaphoreTake(uart_data->tx_semaphore,timeout))
    {
        // 发送失败
        return -1;
    }
    return 0;
}
static void ringBuffer_add_readIndex(Ring_Buffer *ring_buffer,uint16_t length) {
    ring_buffer->read_index += length;
    ring_buffer->read_index %= ring_buffer->buffer_size;
}
static uint8_t ringBuffer_read(Ring_Buffer *ring_buffer,uint16_t i)
{
    uint16_t index = i % ring_buffer->buffer_size;
    return ring_buffer->buffer[index];
}
static uint16_t ringBuffer_get_length(Ring_Buffer *ring_buffer)
{
    // 读索引等于写索引时，缓冲区为空
    if (ring_buffer->read_index == ring_buffer->write_index) {
      return 0;
    }
    // 如果缓冲区已满,返回BUFFER_SIZE
    if (ring_buffer->write_index + 1 == ring_buffer->read_index || (ring_buffer->write_index == ring_buffer->buffer_size - 1 && ring_buffer->read_index == 0)) {
      return ring_buffer->buffer_size;
    }
    // 如果缓冲区未满,返回未处理的数据长度
    if (ring_buffer->read_index <= ring_buffer->write_index) {
      return ring_buffer->write_index - ring_buffer->read_index;
    }
    return ring_buffer->buffer_size - ring_buffer->read_index + ring_buffer->write_index;
}
static uint16_t ringBuffer_get_remainLength(Ring_Buffer *ring_buffer)
{
    return ring_buffer->buffer_size - ringBuffer_get_length(ring_buffer);
}
uint16_t ringBuffer_write(Ring_Buffer *ring_buffer,const uint8_t *data,uint16_t length)
{
    // 如果缓冲区不足 则不写入数据 返回0
    if (ringBuffer_get_remainLength(ring_buffer) < length) {
        return 0;
    }
    // 使用memcpy函数将数据写入缓冲区
    if (ring_buffer->write_index + length < ring_buffer->buffer_size) {
        memcpy(ring_buffer->buffer + ring_buffer->write_index, data, length);
        ring_buffer->write_index += length;
    } else {
        uint8_t firstLength = ring_buffer->buffer_size - ring_buffer->write_index;
        memcpy(ring_buffer->buffer + ring_buffer->write_index, data, firstLength);
        memcpy(ring_buffer->buffer, data + firstLength, length - firstLength);
        ring_buffer->write_index = length - firstLength;
    }
    return length;
}
uint16_t ringBuffer_get_command(Ring_Buffer *ring_buffer,uint8_t *command)
{
    while (1)
    {
        // 缓冲区长度小于最小指令长度 则不可能有完整的指令
        if (ringBuffer_get_length(ring_buffer) < ring_buffer->command_min_len)
        {
            return 0;
        }
        // 如果不是包头,则跳过重新开始找
        if (ringBuffer_read(ring_buffer,ring_buffer->read_index) != 0x01)
        {
            ringBuffer_add_readIndex(ring_buffer,1);
            continue;
        }

    }
}




void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    UART_Data *fpga_uart_data = fpga_device.uart_data;
    if (huart == fpga_uart_data->uart_handle)
    {
        // 释放信号量
        xSemaphoreGive(fpga_uart_data->tx_semaphore);
    }
}
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    UART_Data *fpga_uart_data = fpga_device.uart_data;
    if (huart == fpga_uart_data->uart_handle)
    {
        HAL_UART_RxEventTypeTypeDef event_type = HAL_UARTEx_GetRxEventType(huart);
        if (HAL_UART_RXEVENT_IDLE == event_type || HAL_UART_RXEVENT_TC == event_type)
        {
            fpga_ring_buffer.buffer_write(&fpga_ring_buffer,fpga_uart_data->rx_datas,Size);
            HAL_UARTEx_ReceiveToIdle_DMA(fpga_uart_data->uart_handle,fpga_uart_data->rx_datas,RECEIVE_BUFFER_SIZE_OF_FPGA);
        }
    }
}