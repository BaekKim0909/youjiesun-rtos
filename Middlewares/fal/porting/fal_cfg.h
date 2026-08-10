//
// Created by 74222 on 2026/8/6.
//

#ifndef YOUJIESUN_FAL_CFG_H
#define YOUJIESUN_FAL_CFG_H

#define NOR_FLASH_DEV_NAME "W25Q256"

extern struct fal_flash_dev w25q256_nor_flash;
#define FAL_PART_HAS_TABLE_CFG
/* ===================== Flash device Configuration ========================= */
/* flash device table */
#define FAL_FLASH_DEV_TABLE \
    {                       \
        &w25q256_nor_flash,        \
    }
/* ====================== Partition Configuration ========================== */

#ifdef FAL_PART_HAS_TABLE_CFG
/* partition table */
#define FAL_PART_TABLE                                                                                   \
    {                                                                                                    \
        {FAL_PART_MAGIC_WORD, "setting_param", NOR_FLASH_DEV_NAME, 0, 2 * 1024 * 1024, 0},               \
        {FAL_PART_MAGIC_WORD, "test_records", NOR_FLASH_DEV_NAME, 2 * 1024 * 1024, 30 * 1024 * 1024, 0}, \
    }
#endif /* FAL_PART_HAS_TABLE_CFG */
#endif //YOUJIESUN_FAL_CFG_H
