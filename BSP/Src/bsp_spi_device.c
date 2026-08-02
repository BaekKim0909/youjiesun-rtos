//
// Created by 74222 on 2026/8/2.
//

#include "../Inc/bsp_spi_device.h"
#include "main.h"

extern SPI_HandleTypeDef hspi5;
#define W25Q256_WRITE_ENABLE_COMMAND  (0x06U)

typedef struct {
    SPI_HandleTypeDef *spi_handle;
    GPIO_TypeDef *CS_GPIOx;
    uint16_t      CS_GPIO_Pin;
}SPI_Data;

// W25Q256 片选 选中
static void spi_w25q256_cs_assert(const SPI_Device *device_p);
// W25Q256 片选 取消选中
static void spi_w25q256_cs_deassert(const SPI_Device *device_p);
static void spi_w25q256_write_enable(const SPI_Device *device_p);



SPI_Data w25q256_data = {
    .spi_handle = &hspi5,
    .CS_GPIOx = W25Q256_CS_GPIO_Port,
    .CS_GPIO_Pin = W25Q256_CS_Pin
};

SPI_Device w25q256_spi_device = {
    .name = "W25Q256",
    .spi_cs_assert = spi_w25q256_cs_assert,
    .spi_cs_deassert = spi_w25q256_cs_deassert,
    .spi_data = &w25q256_data
};

static void spi_w25q256_cs_assert(const SPI_Device *device_p) {
    SPI_Data* spi_data =  device_p->spi_data;
    HAL_GPIO_WritePin(spi_data->CS_GPIOx,spi_data->CS_GPIO_Pin,GPIO_PIN_RESET);
}

static void spi_w25q256_cs_deassert(const SPI_Device *device_p) {
    SPI_Data* spi_data =  device_p->spi_data;
    HAL_GPIO_WritePin(spi_data->CS_GPIOx,spi_data->CS_GPIO_Pin,GPIO_PIN_SET);
}
static void spi_w25q256_write_enable(const SPI_Device *device_p) {
    SPI_Data* spi_data =  device_p->spi_data;
    uint8_t buff[1] = {W25Q256_WRITE_ENABLE_COMMAND};
    HAL_SPI_Transmit_DMA(spi_data->spi_handle,buff,1);
}

