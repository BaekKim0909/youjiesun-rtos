#include "FreeRTOS.h"
#include "task.h"
#include "uart_device.h"

//
// Created by 74222 on 2026/7/17.
//

// 当前DMA单次接收缓冲区为128字节，完整包输出缓冲区保持相同容量
#define FPGA_COMMAND_BUFFER_SIZE 128U

extern UART_Device fpga_device;

/**
 * @brief FPGA串口通信任务
 *
 * 该任务是接收数据的唯一消费者。DMA中断只把字节放入环形缓冲区并发送任务通知，
 * 本任务被唤醒后负责从连续字节流中取出一包或多包完整、CRC正确的数据。
 *
 * 当前阶段只完成“可靠取包”，还没有根据寄存器地址解析温度、状态等业务数据。
 */
void StartCommunictaeTask(void *argument)
{
    // get_command成功后，完整帧会复制到该数组中
    uint8_t command_buffer[FPGA_COMMAND_BUFFER_SIZE] = {0};
    // 当前完整帧的实际长度，返回0表示暂时没有完整帧
    uint16_t command_length;
    //  任务通知事件
    uint32_t event_data;
    (void)argument;

    // 必须先绑定再启动DMA，否则DMA很快收到数据时还不知道应该唤醒哪个任务
    fpga_device.bind_rx_task(&fpga_device, xTaskGetCurrentTaskHandle());

    // 初始化包括创建TX完成信号量和启动第一轮ReceiveToIdle DMA
    // 初始化失败时每100ms重试一次，避免高优先级任务无休止空转
    while (fpga_device.init(&fpga_device) != 0)
    {
        vTaskDelay(pdMS_TO_TICKS(100U));
    }

    for (;;)
    {
        // 没有数据时任务在这里阻塞，不占用CPU；RX中断到达后由xTaskNotifyFromISR唤醒
        // event_data:获取通讯任务唤醒要要处理的事件 ,portMAX_DELAY表示一直等到有数据
        xTaskNotifyWait(0x00000000,0xFFFFFFFF, &event_data,portMAX_DELAY);
        // 通知值中有FPGA串口接收事件标志位
        if (event_data & FPGA_RX_EVENT)
        {
            while ((command_length = fpga_device.get_command(
                   &fpga_device,
                   command_buffer,
                   sizeof(command_buffer)
               )) > 0U)
            {
                // 到达这里说明：包头、功能码、长度和CRC已经全部通过检查
                // command_buffer[0 ... command_length-1]就是一包完整数据
                // 后续可在这里根据command_buffer[1]功能码、[2..3]寄存器地址解析业务内容
                (void)command_length;
            }
        }
        // 通知值中有FPGA串口发送事件标志位
        if (event_data & FPGA_TX_EVENT)
        {

        }
        // 一次通知不等于一包数据：可能是半包，也可能是多个粘包
        // 因此持续调用get_command，直到环形缓冲区暂时没有完整包为止

    }
}
