//
// Created by 74222 on 2026/7/22.
//

#ifndef YOUJIESUN_FPGA_COMM_H
#define YOUJIESUN_FPGA_COMM_H


#define EMPTY_ELECTRODE_CAPACITANCE_REG         (0x0000U) /* 空杯电容值 参数寄存器 */

#define TEMPERATURE_REG                         (0x0101U)  /* 温度寄存器 */
#define LID_STATE_REG                           (0x0105U)  /* 盖子状态寄存器地址 */
#define OIL_POUR_STATE_REG                      (0x0106U)  /* 排油状态寄存器地址 */
#define OIL_CUP_STATE_REG                       (0x0107U)  /* 油杯状态寄存器地址 */

#define FPGA_COMM_REGISTER_SIZE                 (2U)    /* 寄存器大小为两个字节 */
#define FPGA_COMM_REGISTER_DATA_START_INDEX     (6U)    /* 指令数据内容起始index */
#define FPGA_COMM_REGISTER_DATA_END_INDEX(reg_num)\
    (FPGA_COMM_REGISTER_DATA_START_INDEX + (reg_num) * FPGA_COMM_REGISTER_SIZE)
#include <stdint.h>
#include "bsp_uart_device.h"

typedef struct
{
    uint16_t start_address;
    uint16_t reg_num;
} read_instruction_t;

// 解析指令
void fpga_comm_parse_command(const uint8_t *command_buf, uint16_t length);

// 读取寄存器指令
void fpga_comm_send_read_command(uint16_t start_address, uint16_t reg_num);
#endif //YOUJIESUN_FPGA_COMM_H
