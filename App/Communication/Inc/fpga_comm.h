//
// Created by 74222 on 2026/7/22.
//

#ifndef YOUJIESUN_FPGA_COMM_H
#define YOUJIESUN_FPGA_COMM_H
#include "bsp_uart_device.h"
#include "test_data.h"
#define EMPTY_ELECTRODE_CAPACITANCE_REG         (0x0000U) /* 空杯电容值 参数寄存器 */

#define WRITE_RESULT_REG                        (0x000CU)   /* 写入状态判断寄存器 */
#define TEST_CONTROL_REG                        (0x0100U)   /* 测试控制寄存器 */
#define TEMPERATURE_REG                         (0x0101U)   /* 温度寄存器 */
#define LID_STATE_REG                           (0x0105U)   /* 盖子状态寄存器地址 */
#define OIL_POUR_STATE_REG                      (0x0106U)   /* 排油状态寄存器地址 */
#define OIL_CUP_STATE_REG                       (0x0107U)   /* 油杯状态寄存器地址 */

#define FPGA_COMM_REGISTER_SIZE                 (2U)    /* 寄存器大小为两个字节 */
#define FPGA_COMM_REGISTER_DATA_START_INDEX     (6U)    /* 指令数据内容起始index */
#define FPGA_COMM_REGISTER_DATA_END_INDEX(reg_num)\
    (FPGA_COMM_REGISTER_DATA_START_INDEX + (reg_num) * FPGA_COMM_REGISTER_SIZE)
#include <stdint.h>

typedef enum
{
    FPGA_OPERATION_READ_REGISTERS = 0, /* 读取寄存器 */
    FPGA_OPERATION_WRITE_TEST_PARAMS, /* 写入测试参数 */
    FPGA_OPERATION_WRITE_REGISTER /* 写入单个16位寄存器 */
} fpga_operation_enum;

/**
 * @brief 单个16位寄存器写入指令
 */
typedef struct
{
    uint16_t register_address; /* 寄存器地址 */
    uint16_t register_value; /* 寄存器写入值 */
} write_register_instruction_t;


#define FPGA_REQUEST_ID_NONE 0U
/**
 * @brief 提交给CommunicateTask的FPGA通讯请求
 */
typedef struct
{
    /* MCU内部请求编号，不发送给FPGA
     * request_id == 0：不需要业务响应的请求;
     * request_id != 0：需要响应关联的请求*/
    uint32_t request_id;
    fpga_operation_enum operation; /* 本次通讯要执行的操作 */

    union
    {
        read_instruction_t read_instruction; /* 读取寄存器参数 */
        test_params_t test_params; /* 写入FPGA的测试参数 */
        write_register_instruction_t write_register; /* 写入单个寄存器 */
    } request_data; /* 当前通讯请求携带的数据 */
} fpga_request_t;

/**
 * @brief FPGA通讯请求的处理结果
 */
typedef enum
{
    FPGA_RESPONSE_SUCCESS = 0, /* FPGA确认请求执行成功 */
    FPGA_RESPONSE_FAIL, /* FPGA返回写入失败 */
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

/**
 * @brief 判断接收帧是否为写入应答
 *
 * @param command_buffer 已通过底层CRC校验的完整帧
 * @param command_length 完整帧长度
 * @param write_accepted 输出FPGA是否接受本次写入
 * @return true 该帧是写操作结果应答
 * @return false 该帧不是写操作结果应答
 */
bool fpga_comm_parse_write_response(const uint8_t *command_buffer, uint16_t command_length,bool *write_accepted);

// 读取寄存器指令
bool fpga_comm_send_read_command(const read_instruction_t *read_instruction);

// 发送测试参数
bool fpga_comm_send_test_params(const test_params_t *test_params);

/**
 * @brief 发送单个16位寄存器写入指令
 */
bool fpga_comm_send_write_register(const write_register_instruction_t *instruction);

#endif //YOUJIESUN_FPGA_COMM_H
