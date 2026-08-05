//
// Created by 74222 on 2026/8/2.
//

#include "../Inc/W25Q256.h"
#include "bsp_spi_device.h"
#include "FreeRTOS.h"
#include "task.h"
extern SPI_Device w25q256_spi_device;

#define 

/* 命令字 */
#define WRITE_ENABLE_INSTRUCTION        (0x06U)
#define READ_STATUS_INSTRUCTION         (0x05U)
#define ERASE_SECTOR_4B_ADD_INSTRUCTION (0x21U)

static void W25Q256_write_enable()
{
    uint8_t tx_buf[1] = {WRITE_ENABLE_INSTRUCTION};
    w25q256_spi_device.transmit_data(&w25q256_spi_device, tx_buf,1,1000);
}

/*
 * @return 1: busy  0: ready
 */
static int W25Q256_read_status(void)
{
    uint8_t tx_buf[2] = {READ_STATUS_INSTRUCTION,0xFF};
    uint8_t rx_buf[2] = {0xFF, 0xFF};
    w25q256_spi_device.transmit_receive_data(&w25q256_spi_device,tx_buf,rx_buf,2,1000);
    return rx_buf[1];
}
/*
 *  @brief 等待W25Q256空闲
 */
static void W25Q256_wait_not_busy(void)
{
    while (W25Q256_read_status() & 0x01)
    {
        vTaskDelay(5);
    }
}
/*
 * @brief  4B地址 sector 擦除
 * @return 擦除字节数量
 */
int W25Q256_sector_erase(uint32_t address, uint32_t size)
{
    // 拉低CS
    w25q256_spi_device.spi_cs_assert(&w25q256_spi_device);

    // 写使能
    W25Q256_write_enable();

    // 发送擦除指令 和 地址
    uint8_t tx_buf[5] = {ERASE_SECTOR_4B_ADD_INSTRUCTION,(address >> 24) & 0xFF, address >> 16 & 0xFF, address >> 8 & 0xFF, address & 0xFF};
    w25q256_spi_device.transmit_data(&w25q256_spi_device, tx_buf,5,1000);

    // 等待擦除完成
    W25Q256_wait_not_busy();

    // 拉高CS
    w25q256_spi_device.spi_cs_deassert(&w25q256_spi_device);

    return size;
}

