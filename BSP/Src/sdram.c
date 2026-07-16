#include "sdram.h"
extern SDRAM_HandleTypeDef hsdram1;

/**
 * @brief       向SDRAM发送命令
 * @param       bankx:0,向BANK5上面的SDRAM发送指令
 * @param             1,向BANK6上面的SDRAM发送指令
 * @param       cmd:指令(0,正常模式/1,时钟配置使能/2,预充电所有存储区/3,自动刷新/4,加载模式寄存器/5,自刷新/6,掉电)
 * @param       refresh:自刷新次数
 * @param       返回值:0,正常;1,失败.
 * @retval      模式寄存器的定义
 */
uint8_t sdram_send_cmd(uint8_t bankx, uint8_t cmd, uint8_t refresh, uint16_t regval)
{
    uint32_t target_bank = 0;
    FMC_SDRAM_CommandTypeDef command;

    if (bankx == 0)
    {
        target_bank = FMC_SDRAM_CMD_TARGET_BANK1;
    }
    else if (bankx == 1)
    {
        target_bank = FMC_SDRAM_CMD_TARGET_BANK2;
    }

    command.CommandMode = cmd;                                              /* 命令 */
    command.CommandTarget = target_bank;                                    /* 目标SDRAM存储区域 */
    command.AutoRefreshNumber = refresh;                                    /* 自刷新次数 */
    command.ModeRegisterDefinition = regval;                                /* 要写入模式寄存器的值 */

    if (HAL_SDRAM_SendCommand(&hsdram1, &command, 0X1000) == HAL_OK) /* 向SDRAM发送命令 */
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

/**
 * @brief       在指定地址(addr+BANK5_SDRAM_ADDR)开始,连续写入n个字节
 * @param       pbuf  : 字节指针
 * @param       addr  : 要写入的地址
 * @param       n     : 要写入的字节数
 * @retval      无
*/
void fmc_sdram_write_buffer(uint8_t *pbuf, uint32_t addr, uint32_t n)
{
    for (; n != 0; n--)
    {
        *(volatile uint8_t*)(BANK5_SDRAM_ADDR + addr) = *pbuf;
        addr++;
        pbuf++;
    }
}

/**
 * @brief       在指定地址((addr+BANK5_SDRAM_ADDR))开始,连续读出n个字节
 * @param       pbuf  : 字节指针
 * @param       addr  : 要读出的起始地址
 * @param       n     : 要写入的字节数
 * @retval      无
*/
void fmc_sdram_read_buffer(uint8_t *pbuf, uint32_t addr, uint32_t n)
{
    for (; n != 0; n--)
    {
        *pbuf++ = *(volatile uint8_t*)(BANK5_SDRAM_ADDR + addr);
        addr++;
    }
}
/**
 * @brief       发送SDRAM初始化序列
 * @param       无
 * @retval      无
 */
void sdram_initialization_sequence(void)
{
    uint32_t temp = 0;

    /* SDRAM控制器初始化完成以后还需要按照如下顺序初始化SDRAM */
    sdram_send_cmd(0, FMC_SDRAM_CMD_CLK_ENABLE, 1, 0);                /* 时钟配置使能 */
    HAL_Delay(1);                                                    /* 至少延时500us */
    sdram_send_cmd(0, FMC_SDRAM_CMD_PALL, 1, 0);                      /* 对所有存储区预充电 */
    sdram_send_cmd(0, FMC_SDRAM_CMD_AUTOREFRESH_MODE, 8, 0);          /* 设置自刷新次数 */

    /* 配置模式寄存器,SDRAM的bit0~bit2为指定突发访问的长度，
     * bit3为指定突发访问的类型，bit4~bit6为CAS值，bit7和bit8为运行模式
     * bit9为指定的写突发模式，bit10和bit11位保留位 */
    temp = (uint32_t)SDRAM_MODEREG_BURST_LENGTH_1  |                  /* 设置突发长度:1(可以是1/2/4/8) */
              SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL  |                  /* 设置突发类型:连续(可以是连续/交错) */
              SDRAM_MODEREG_CAS_LATENCY_3          |                  /* 设置CAS值:3(可以是2/3) */
              SDRAM_MODEREG_OPERATING_MODE_STANDARD|                  /* 设置操作模式:0,标准模式 */
              SDRAM_MODEREG_WRITEBURST_MODE_SINGLE;                   /* 设置突发写模式:1,单点访问 */
    sdram_send_cmd(0, FMC_SDRAM_CMD_LOAD_MODE, 1, temp);              /* 设置SDRAM的模式寄存器 */
}
