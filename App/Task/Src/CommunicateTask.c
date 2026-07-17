#include "cmsis_os2.h"
#include "uart_device.h"
//
// Created by 74222 on 2026/7/17.
//
extern UART_Device fpga_device;
void StartCommunictaeTask(void *argument)
{
    /* USER CODE BEGIN StartCommunictaeTask */
    fpga_device.init(&fpga_device);
    /* Infinite loop */
    for(;;)
    {
        osDelay(1);
    }
    /* USER CODE END StartCommunictaeTask */
}