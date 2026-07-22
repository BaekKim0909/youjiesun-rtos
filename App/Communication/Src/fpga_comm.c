//
// Created by 74222 on 2026/7/22.
//

#include "../Inc/fpga_comm.h"

#include <string.h>

#include "../../System/Inc/system_structs.h"
extern volatile DeviceState_t device_state;
extern float current_temperature_g;
void fpga_comm_parse_command(const uint8_t *command_buf,uint16_t length)
{
    const uint16_t start_reg_address = (uint16_t)command_buf[2] <<8 | command_buf[3];
    const uint16_t reg_num = (uint16_t)command_buf[4] << 8 | command_buf[5];
    const uint16_t excepted_length = reg_num * FPGA_COMM_REGISTER_SIZE + 8;
    if (excepted_length != length)
        return;
    if (command_buf[0] == 0x01 && command_buf[1] == 0x03)
    {
        if (reg_num == 1)
        {
            uint16_t temp_value = 0;
            for (uint32_t i = FPGA_COMM_REGISTER_DATA_START_INDEX; i < FPGA_COMM_REGISTER_DATA_END_INDEX(reg_num);i++)
            {
                temp_value <<= 8;
                temp_value |= (uint16_t)command_buf[i];
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
                temp_value |= (uint32_t)command_buf[i];
            }
            memcpy(&temp_temperature, &temp_value,sizeof(float));
            current_temperature_g = temp_temperature;
        }
    }
}