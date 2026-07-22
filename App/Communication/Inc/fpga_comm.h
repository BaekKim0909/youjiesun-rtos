//
// Created by 74222 on 2026/7/22.
//

#ifndef YOUJIESUN_FPGA_COMM_H
#define YOUJIESUN_FPGA_COMM_H

#define TEMPERATURE_REG             0x0101  /* 温度寄存器 */
#define LID_STATE_REG               0x0105  /* 盖子状态寄存器地址 */
#define OIL_POUR_STATE_REG          0x0106  /* 排油状态寄存器地址 */
#define OIL_CUP_STATE_REG           0x0107  /* 油杯状态寄存器地址 */

#define FPGA_COMM_REGISTER_SIZE                 (2U)    /* 寄存器大小为两个字节 */
#define FPGA_COMM_REGISTER_DATA_START_INDEX     (6U)    /* 指令数据内容起始index */
#define FPGA_COMM_REGISTER_DATA_END_INDEX(reg_num)\
    (FPGA_COMM_REGISTER_DATA_START_INDEX + (reg_num) * FPGA_COMM_REGISTER_SIZE)
#include <stdint.h>

// 解析指令
void fpga_comm_parse_command(const uint8_t *command_buf, uint16_t length);
#endif //YOUJIESUN_FPGA_COMM_H
