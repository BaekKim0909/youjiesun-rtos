//
// Created by 74222 on 2026/8/2.
//

#ifndef YOUJIESUN_BSP_SPI_DEVICE_H
#define YOUJIESUN_BSP_SPI_DEVICE_H
#include <stdint.h>

typedef struct SPI_Device SPI_Device;


struct SPI_Device {
    const char * name;
    int  (*spi_init)(SPI_Device *device_p);
    void (*spi_cs_assert)(const SPI_Device *device_p);      // 片选信号 有效
    void (*spi_cs_deassert)(const SPI_Device *device_p);    // 片选信号 无效
    int  (*transmit_data)(const SPI_Device *device_p, const uint8_t *data, uint16_t length, uint32_t timeout);    // 发送数据
    int  (*receive_data)(const SPI_Device *device_p, uint8_t *data, uint16_t length, uint32_t timeout);       // 接收数据
    int  (*transmit_receive_data)(const SPI_Device *device_p, uint8_t *t_data, uint8_t *r_data ,uint16_t length, uint32_t timeout); // 发送并接收数
    void * spi_data;                                        // SPI 私有数据
};

#endif //YOUJIESUN_BSP_SPI_DEVICE_H
