//
// Created by 74222 on 2026/7/22.
//

#include "../Inc/fpga_comm.h"

#include <string.h>
#include <sys/types.h>

#include "system_state.h"

extern UART_Device fpga_device;

void fpga_comm_parse_command(const uint8_t *command_buf, uint16_t length)
{
    const uint16_t start_reg_address = (uint16_t) command_buf[2] << 8 | command_buf[3];
    const uint16_t reg_num = (uint16_t) command_buf[4] << 8 | command_buf[5];
    const uint16_t excepted_length = reg_num * FPGA_COMM_REGISTER_SIZE + 8;
    if (excepted_length != length)
        return;
    if (command_buf[0] == 0x01 && command_buf[1] == 0x03)
    {
        if (reg_num == 1)
        {
            uint16_t temp_value = 0;
            for (uint32_t i = FPGA_COMM_REGISTER_DATA_START_INDEX; i < FPGA_COMM_REGISTER_DATA_END_INDEX(reg_num); i++)
            {
                temp_value <<= 8;
                temp_value |= (uint16_t) command_buf[i];
            }
            // 判断寄存器
            switch (start_reg_address)
            {
                case LID_STATE_REG:
                    device_state.lid_state = temp_value;
                    break;
                case OIL_POUR_STATE_REG:
                    device_state.pour_state = temp_value;
                    break;
                case OIL_CUP_STATE_REG:
                    device_state.oil_cup_state = temp_value;
                    break;
                case TEST_STATE_REG:
                    device_state.current_step_state = temp_value;
                    break;
                case TEST_REMAIN_TIME_REG:
                    device_state.remain_test_time = temp_value;
                default:
                    break;
            }
        }
        else if (start_reg_address == TEMPERATURE_REG && reg_num == 2)
        {
            uint32_t temp_value = 0;
            float temp_temperature = 0.0f;
            for (uint32_t i = FPGA_COMM_REGISTER_DATA_START_INDEX; i < FPGA_COMM_REGISTER_DATA_END_INDEX(reg_num); i++)
            {
                temp_value <<= 8;
                temp_value |= (uint32_t) command_buf[i];
            }
            memcpy(&temp_temperature, &temp_value, sizeof(float));
            device_state.oil_cup_temperature = temp_temperature;
        }
    }
}

bool fpga_comm_parse_write_response(const uint8_t *command_buffer, uint16_t command_length,bool *write_accepted)
{
    if (command_buffer == NULL || write_accepted == NULL || command_length != 10U)
    {
        return false;
    }
    const uint16_t start_address =
            ((uint16_t) command_buffer[2] << 8) |
            command_buffer[3];

    const uint16_t register_count =
            ((uint16_t) command_buffer[4] << 8) |
            command_buffer[5];

    const uint16_t write_result =
            ((uint16_t) command_buffer[6] << 8) |
            command_buffer[7];
    if (command_buffer[0] != 0x01 ||
        command_buffer[1] != 0x06 ||
        start_address != WRITE_RESULT_REG ||
        register_count != 1)
    {
        return false;
    }
    if (write_result == 0x00U)
        return false;
    *write_accepted = write_result == 1U;
    return true;
}

bool fpga_comm_parse_outcome_response(const uint8_t *command_buffer, uint16_t command_length)
{
    if (command_buffer == NULL || command_length != 36)
    {
        return false;
    }
    const uint16_t start_address =
            ((uint16_t) command_buffer[2] << 8) |
            command_buffer[3];

    const uint16_t register_count =
            ((uint16_t) command_buffer[4] << 8) |
            command_buffer[5];

    if (command_buffer[0] != 0x01 ||
        command_buffer[1] != 0x03 ||
        start_address != PERMITTIVITY_OUTCOME_REG ||
        register_count != 0x1C)
    {
        return false;
    }
    return true;
}

// 读取寄存器指令
bool fpga_comm_send_read_command(const read_instruction_t *read_instruction)
{
    uint8_t reg_high_addr = (read_instruction->start_address >> 8) & 0xFF;
    uint8_t reg_low_addr = read_instruction->start_address & 0xFF;
    uint8_t reg_num_high = (read_instruction->reg_num >> 8) & 0xFF;
    uint8_t reg_num_low = read_instruction->reg_num & 0xFF;

    uint8_t commandBuffer[8] = {0x01, 0x03, reg_high_addr, reg_low_addr, reg_num_high, reg_num_low};
    uint16_t crc = modbus_crc16(commandBuffer, 6);
    commandBuffer[6] = crc & 0xFF;
    commandBuffer[7] = (crc >> 8) & 0xFF;
    return fpga_device.send_data(&fpga_device, commandBuffer, 8, 1000) == 0;
}

// 发送测试参数
bool fpga_comm_send_test_params(const test_params_t *test_params)
{
    uint32_t capacitance_temp = 0;
    memcpy(&capacitance_temp, &test_params->empty_cell_capacitance, sizeof(float));
    uint8_t command_buffer[26] = {
        0x01, 0x06,
        0x00, 0x00, 0x00, 0x09,
        (capacitance_temp >> 24) & 0xFF,
        (capacitance_temp >> 16) & 0xFF,
        (capacitance_temp >> 8) & 0xFF,
        capacitance_temp & 0xFF,
        (test_params->fill_num >> 8) & 0xFF,
        test_params->fill_num & 0xFF,
        (test_params->temperature >> 24) & 0xFF,
        (test_params->temperature >> 16) & 0xFF,
        (test_params->temperature >> 8) & 0xFF,
        test_params->temperature & 0xFF,
        (test_params->ac_voltage >> 8) & 0xFF,
        test_params->ac_voltage & 0xFF,
        (test_params->frequency >> 8) & 0xFF,
        (test_params->frequency) & 0xFF,
        (test_params->dc_voltage >> 8) & 0xFF,
        test_params->dc_voltage & 0xFF,
        (test_params->rho_param >> 8) & 0xFF,
        test_params->rho_param & 0xFF
    };
    const uint16_t crc = modbus_crc16(command_buffer, 24);
    command_buffer[24] = crc & 0xFF; // CRC低字节
    command_buffer[25] = (crc >> 8) & 0xFF; // CRC高字节
    return fpga_device.send_data(&fpga_device, command_buffer, 26, 1000U) == 0;
}

bool fpga_comm_send_write_register(const write_register_instruction_t *instruction)
{
    if (instruction == NULL)
    {
        return false;
    }

    uint8_t command_buffer[10] = {
        0x01, 0x06,
        (instruction->register_address >> 8) & 0xFF,
        instruction->register_address & 0xFF,
        0x00, 0x01,
        (instruction->register_value >> 8) & 0xFF,
        instruction->register_value & 0xFF
    };
    const uint16_t crc = modbus_crc16(command_buffer, 8);
    command_buffer[8] = crc & 0xFF;
    command_buffer[9] = (crc >> 8) & 0xFF;
    return fpga_device.send_data(&fpga_device, command_buffer, 10, 1000U) == 0;
}
