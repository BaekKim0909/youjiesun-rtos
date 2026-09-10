//
// Created by 74222 on 2026/9/9.
//
#include <stddef.h>

#include "FreeRTOS.h"
#include "task.h"
#include "W25Q256.h"

// 版本信息放在FLASH 30MB 地址上
#define FLASH_ADDRESS_OF_SOFT_VERSION  (0x01E00000U)

#define SOFTWARE_VERSION       "V1.0"
uint16_t major_version = 1;
uint16_t minor_version = 0;


void start_init_task(void *argument)
{
    W25Q256_init();

    uint8_t software_version[4];
    W25Q256_read(software_version, FLASH_ADDRESS_OF_SOFT_VERSION, 4);

    uint16_t major_version_of_flash = software_version[0] << 8 | software_version[1];
    uint16_t minor_version_of_flash = software_version[2] << 8 | software_version[3];
    // 如果flash存储的版本号与代码不同，则覆盖重写
    if (major_version_of_flash != major_version || minor_version_of_flash != minor_version)
    {
        uint8_t transmit_buffer[4] =
        {
            (major_version >> 8) & 0xFF,
            major_version & 0xFF,
            (minor_version >> 8) & 0xFF,
            minor_version & 0xFF
        };
        W25Q256_erase_sector(FLASH_ADDRESS_OF_SOFT_VERSION);
        W25Q256_write_page(transmit_buffer, FLASH_ADDRESS_OF_SOFT_VERSION, 4);
    }
    vTaskDelete(NULL);
}
