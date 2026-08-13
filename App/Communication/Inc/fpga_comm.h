//
// Created by 74222 on 2026/7/22.
//

#ifndef YOUJIESUN_FPGA_COMM_H
#define YOUJIESUN_FPGA_COMM_H


#define EMPTY_ELECTRODE_CAPACITANCE_REG         (0x0000U) /* 空杯电容值 参数寄存器 */

#define WRITE_RESULT_REG                        (0x000CU)   /* 写入状态判断寄存器 */
#define TEMPERATURE_REG                         (0x0101U)   /* 温度寄存器 */
#define LID_STATE_REG                           (0x0105U)   /* 盖子状态寄存器地址 */
#define OIL_POUR_STATE_REG                      (0x0106U)   /* 排油状态寄存器地址 */
#define OIL_CUP_STATE_REG                       (0x0107U)   /* 油杯状态寄存器地址 */

#define FPGA_COMM_REGISTER_SIZE                 (2U)    /* 寄存器大小为两个字节 */
#define FPGA_COMM_REGISTER_DATA_START_INDEX     (6U)    /* 指令数据内容起始index */
#define FPGA_COMM_REGISTER_DATA_END_INDEX(reg_num)\
    (FPGA_COMM_REGISTER_DATA_START_INDEX + (reg_num) * FPGA_COMM_REGISTER_SIZE)
#include <stdint.h>
#include "bsp_uart_device.h"
#include "test_data.h"

typedef struct
{
    uint16_t start_address;
    uint16_t reg_num;
} read_instruction_t;

typedef enum
{
    FPGA_OPERATION_READ_REGISTERS = 0, /* 读取寄存器 */
    FPGA_OPERATION_WRITE_TEST_PARAMS /* 写入测试参数 */
} fpga_operation_enum;

/**
 * @brief 提交给CommunicateTask的FPGA通讯请求
 */
typedef struct
{
    uint32_t request_id; /* 上位机内部请求编号，不发送给FPGA */

    fpga_operation_enum operation; /* 本次通讯要执行的操作 */

    union
    {
        read_instruction_t read_instruction; /* 读取寄存器参数 */
        test_params_t test_params; /* 写入FPGA的测试参数 */
    } request_data; /* 当前通讯请求携带的数据 */
} fpga_request_t;

/**
 * @brief FPGA通讯请求的处理结果
 */
typedef enum
{
    FPGA_RESPONSE_SUCCESS = 0, /* FPGA确认请求执行成功 */
    FPGA_RESPONSE_REJECTED, /* FPGA返回写入失败 */
    FPGA_RESPONSE_SEND_FAILED, /* UART发送失败 */
    FPGA_RESPONSE_TIMEOUT, /* 等待应答超时 */
    FPGA_RESPONSE_FORMAT_ERROR /* 应答格式错误 */
} fpga_response_status_enum;

/**
 * @brief CommunicateTask返回给TestTask的通讯响应
 */
typedef struct
{
    uint32_t request_id; /* 对应原通讯请求的内部编号 */

    fpga_operation_enum operation; /* 对应原通讯请求的操作 */

    fpga_response_status_enum response_status; /* 通讯处理结果 */
} fpga_response_t;

// 解析指令
void fpga_comm_parse_command(const uint8_t *command_buf, uint16_t length);

// 读取寄存器指令
void fpga_comm_send_read_command(uint16_t start_address, uint16_t reg_num);
#endif //YOUJIESUN_FPGA_COMM_H
