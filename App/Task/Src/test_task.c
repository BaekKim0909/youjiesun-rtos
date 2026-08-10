//
// Created by 74222 on 2026/8/10.
//
#include "fpga_comm.h"
#include "main.h"
#include "FreeRTOS.h"
#include "timers.h"
#include "task.h"
#include "queue.h"

void start_test_task(void *argument)
{
    xTimerStart(read_temperature_timer, pdMS_TO_TICKS(1000U));
    for (;;)
    {
        vTaskDelay(10);
    }
}

typedef enum
{
    TEST_IDLE = 0,
    TEST_HEAT
} System_Status;

void read_fpga_temperature_timer_cb(TimerHandle_t xTimer)
{
    read_instruction_t read_instruction = {
        .start_address = TEMPERATURE_REG,
        .reg_num = 0x0002
    };
    xQueueSend(fpga_tx_read_instruction_queue, &read_instruction, 0);
    xTaskNotify(communicate_taskHandle, FPGA_TX_EVENT, eSetBits);
}
