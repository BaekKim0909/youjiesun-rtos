//
// Created by 74222 on 2026/8/2.
//

#ifndef YOUJIESUN_W25Q256_H
#define YOUJIESUN_W25Q256_H
#include <stdint.h>

#define W25Q256_SIZE                    (32U * 1024U * 1024U)
#define W25Q256_START_ADDR              (0U)
#define W25Q256_END_ADDR                (W25Q256_START_ADDR + W25Q256_SIZE)

#define W25Q256_PAGE_SIZE              ( 256 )
#define W25Q256_SECTOR_SIZE            ( 4 * 1024 )
#define W25Q256_BLOCK_SIZE             ( 64 * 1024 )
/*
 * @brief   4B地址 sector 擦除
 * @return  1: 擦除sector成功
 *          0: 擦除失败
 */
int W25Q256_sector_erase(uint32_t address);
#endif //YOUJIESUN_W25Q256_H
