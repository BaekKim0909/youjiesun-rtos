//
// Created by 74222 on 2026/8/6.
//
#include <fal_def.h>
#include <W25Q256.h>


// 获取 sector号
static int get_sector(uint32_t address)
{
    uint32_t sector = 0;
    if ( address < W25Q256_END_ADDR && address >= W25Q256_START_ADDR ) {
        address -= W25Q256_START_ADDR;
        sector = address / W25Q256_SECTOR_SIZE;
        return sector;
    }
    return -1;
}

const struct fal_flash_dev w25q256_nor_flash =
{

};