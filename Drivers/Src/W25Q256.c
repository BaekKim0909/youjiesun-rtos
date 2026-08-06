//
// Created by 74222 on 2026/8/2.
//

#include "../Inc/W25Q256.h"
#include "bsp_spi_device.h"
#include "FreeRTOS.h"
#include "task.h"
extern SPI_Device w25q256_spi_device;


/* 命令字 */
#define WRITE_ENABLE_INSTRUCTION            (0x06U)
#define READ_STATUS_INSTRUCTION             (0x05U)
#define ERASE_SECTOR_4B_ADD_INSTRUCTION     (0x21U)
#define ENTER_4B_ADDRESS_MODE_INSTRUCTION   (0xB7U)
#define EXIT_4B_ADDRESS_MODE_INSTRUCTION    (0xE9U)

/*
 * @brief 写使能
 * @return 1: 写使能成功  0：写使能失败
 */
static int W25Q256_write_enable(void);
// 读取FLASH Status Register
static int W25Q256_read_status_reg(void);
// 等待W25Q256空闲
static int W25Q256_wait_not_busy(uint32_t timeout);
// 进入4B 地址模式
static void W25Q256_enter_4B_address_mode(void);
// 退出4B 地址模式
static void W25Q256_exit_4B_address_mode(void);



static int W25Q256_write_enable(void)
{
    // 拉低CS
    w25q256_spi_device.spi_cs_assert(&w25q256_spi_device);

    uint8_t tx_buf[1] = {WRITE_ENABLE_INSTRUCTION};
    int result =  w25q256_spi_device.transmit_data(&w25q256_spi_device, tx_buf,1,1000);

    // 拉高CS
    w25q256_spi_device.spi_cs_deassert(&w25q256_spi_device);

    if (result != 1)
        return -1; // 发送失败

    int status = W25Q256_read_status_reg();

    if (status < 0)
    {
        return -1;
    }

    if ((status & 0x02U) != 0U)
    {
        return 1;   // 写使能成功
    }

    return 0;   // 写使能失败

}

/*
 * @return 8bit reg_value
 *              bit0 BUSY: 1: busy, 0: idle;
 *              bit1 WEL:  1：write_enable  0: write_disable
 */
static int W25Q256_read_status_reg(void)
{
    uint8_t tx_buf[2] = {READ_STATUS_INSTRUCTION,0xFF};
    uint8_t rx_buf[2] = {0xFF, 0xFF};

    w25q256_spi_device.spi_cs_assert(&w25q256_spi_device);

    int result = w25q256_spi_device.transmit_receive_data(
        &w25q256_spi_device,
        tx_buf,
        rx_buf,
        2,
        1000);

    w25q256_spi_device.spi_cs_deassert(&w25q256_spi_device);

    if (result != 2)
    {
        return -1; // 发送接收失败
    }

    return rx_buf[1];
}
/*
 *  @brief 等待W25Q256空闲
 */
static int W25Q256_wait_not_busy(uint32_t timeout)
{
    TickType_t start_tick = xTaskGetTickCount();
    TickType_t timeout_tick = pdMS_TO_TICKS(timeout);

    while ((xTaskGetTickCount() - start_tick) < timeout_tick)
    {

        int status = W25Q256_read_status_reg();

        if (status < 0)
        {
            return -2; // 读取状态失败
        }

        if ((status & 0x01U) == 0U)
        {
            return 1; // Flash 空闲
        }

        vTaskDelay(pdMS_TO_TICKS(5));
    }
    return -1; // 超时
}
// 进入4B 地址模式
static void W25Q256_enter_4B_address_mode(void)
{
    w25q256_spi_device.spi_cs_assert(&w25q256_spi_device);

    uint8_t t_buf[1] = { ENTER_4B_ADDRESS_MODE_INSTRUCTION };
    w25q256_spi_device.transmit_data(&w25q256_spi_device, t_buf,1,1000);

    w25q256_spi_device.spi_cs_deassert(&w25q256_spi_device);
}
// 退出4B 地址模式
static void W25Q256_exit_4B_address_mode(void)
{
    w25q256_spi_device.spi_cs_assert(&w25q256_spi_device);

    uint8_t t_buf[1] = { EXIT_4B_ADDRESS_MODE_INSTRUCTION };
    w25q256_spi_device.transmit_data(&w25q256_spi_device, t_buf,1,1000);

    w25q256_spi_device.spi_cs_deassert(&w25q256_spi_device);
}
/*
 * @brief  4B地址 sector 擦除
 * @return 1: 擦除sector成功 0: 擦除失败
 */
int W25Q256_sector_erase(uint32_t address)
{
    if (address >= W25Q256_SIZE)
        return 0;
    if (W25Q256_wait_not_busy(1000) != 1)
    {
        return 0;
    }

    // 向下对齐到 4 KB 扇区首地址。
    address &= ~(W25Q256_SECTOR_SIZE - 1);

    // 写使能
    if (W25Q256_write_enable() != 1)
    {
        return 0;
    }

    w25q256_spi_device.spi_cs_assert(&w25q256_spi_device);

    // 发送擦除指令 和 地址
    uint8_t tx_buf_1[5] = {ERASE_SECTOR_4B_ADD_INSTRUCTION,(address >> 24) & 0xFF, (address >> 16) & 0xFF, (address >> 8) & 0xFF, address & 0xFF};
    int res = w25q256_spi_device.transmit_data(&w25q256_spi_device, tx_buf_1,5,1000);

    w25q256_spi_device.spi_cs_deassert(&w25q256_spi_device);

    if (res != 5)
        return 0;
    // 等待擦除完成
    if (1 != W25Q256_wait_not_busy(1000))
        return 0; // 超时

    return 1; // 擦除成功
}

