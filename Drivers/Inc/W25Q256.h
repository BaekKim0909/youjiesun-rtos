//
// Created by 74222 on 2026/8/2.
//

#ifndef YOUJIESUN_W25Q256_H
#define YOUJIESUN_W25Q256_H
#include <stdint.h>

/*
 * @brief  4B地址 sector 擦除
 * @return 擦除字节数量
 */
int W25Q256_sector_erase(uint32_t address, uint32_t size);

#endif //YOUJIESUN_W25Q256_H
