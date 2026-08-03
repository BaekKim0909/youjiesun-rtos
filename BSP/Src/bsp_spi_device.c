//
// Created by 74222 on 2026/8/2.
//

#include "../Inc/bsp_spi_device.h"
#include "main.h"

extern SPI_HandleTypeDef hspi5;
#define W25Q256_WRITE_ENABLE_COMMAND  (0x06U)

typedef struct {
    SPI_HandleTypeDef *spi_handle;
    // 发送完成信号量：发送任务等待，中断回调释放
    SemaphoreHandle_t tx_semaphore;
    GPIO_TypeDef *CS_GPIOx;
    uint16_t      CS_GPIO_Pin;
}SPI_Data;

// W25Q256 SPI 初始化
static int spi_w25q256_init(SPI_Device *device_p);
// W25Q256 片选 选中
static void spi_w25q256_cs_assert(const SPI_Device *device_p);
// W25Q256 片选 取消选中
static void spi_w25q256_cs_deassert(const SPI_Device *device_p);
// SPI 发送命令
static int spi_w25q256_send_data(const SPI_Device *device_p, const uint8_t *datas, uint16_t length, uint32_t timeout);

SPI_Data w25q256_data = {
    .spi_handle = &hspi5,
    .CS_GPIOx = W25Q256_CS_GPIO_Port,
    .CS_GPIO_Pin = W25Q256_CS_Pin
};

SPI_Device w25q256_spi_device = {
    .name = "W25Q256",
    .spi_init = spi_w25q256_init,
    .spi_cs_assert = spi_w25q256_cs_assert,
    .spi_cs_deassert = spi_w25q256_cs_deassert,
    .send_data = spi_w25q256_send_data,
    .spi_data = &w25q256_data
};


static int spi_w25q256_init(SPI_Device *device_p)
{
    SPI_Data* spi_data =  device_p->spi_data;
    if (device_p == NULL || spi_data == NULL)
    {
        return -1;
    }
    spi_data->tx_semaphore = spi5_tx_semaphore;
    return 1;
}
static void spi_w25q256_cs_assert(const SPI_Device *device_p) {
    SPI_Data* spi_data =  device_p->spi_data;
    HAL_GPIO_WritePin(spi_data->CS_GPIOx,spi_data->CS_GPIO_Pin,GPIO_PIN_RESET);
}

static void spi_w25q256_cs_deassert(const SPI_Device *device_p) {
    SPI_Data* spi_data =  device_p->spi_data;
    HAL_GPIO_WritePin(spi_data->CS_GPIOx,spi_data->CS_GPIO_Pin,GPIO_PIN_SET);
}
static int spi_w25q256_send_data(const SPI_Device *device_p, const uint8_t *datas, uint16_t length, uint32_t timeout)
{
    SPI_Data* spi_data =  device_p->spi_data;
    // 清除上一次异常流程可能遗留的发送完成信号
    xSemaphoreTake(spi_data->tx_semaphore,0);

    if (HAL_OK != HAL_SPI_Transmit_DMA(spi_data->spi_handle,datas,length))
    {
        return 0;   // 传输启动失败
    }

    if (pdTRUE != xSemaphoreTake(spi_data->tx_semaphore,timeout))
    {
        return -1;  // 超时
    }
    return 1;       // 成功
}

// SPI 发送完成回调
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
    SPI_Data *w25q256_spi_data = w25q256_spi_device.spi_data;

    BaseType_t higher_priority_task_woken = pdFALSE;
    if (hspi == w25q256_spi_data->spi_handle)
    {
        xSemaphoreGiveFromISR(w25q256_spi_data->tx_semaphore,&higher_priority_task_woken);
        portYIELD_FROM_ISR(higher_priority_task_woken);
    }
}