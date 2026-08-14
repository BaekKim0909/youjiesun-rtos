#include "FreeRTOS.h"
#include "task.h"
#include "bsp_uart_device.h"
#include "main.h"
#include "fpga_comm.h"
#include "test_task.h"
//
// Created by 74222 on 2026/7/17.
//

// 当前DMA单次接收缓冲区为128Byte，完整包输出缓冲区保持相同容量
#define FPGA_COMMAND_BUFFER_SIZE 128U
// 写参数后等待FPGA应答的最长时间
#define FPGA_WRITE_RESPONSE_TIMEOUT_MS            1000U

// TestTask事件队列已满时，重试投递响应的间隔
#define FPGA_RESPONSE_REPORT_RETRY_MS             10U

extern UART_Device fpga_device;


/* -------------------------发送请求相关----------------------- */
// 当前是否存在已发送但尚未收到响应的请求  表示正在等待FPGA应答
static bool pending_request_valid = false;
//当前等待FPGA响应的请求
static fpga_request_t pending_request;
// 当前pending请求的发送完成时刻
static TickType_t pending_request_start_tick = 0U;


/* -------------------------等待响应相关----------------------- */
// 等待投递给TestTask的响应
static fpga_response_t pending_response;
// 当前是否有响应尚未成功投递给TestTask   表示FPGA事务已经结束，但结果还没有成功交给TestTask
static bool pending_response_valid = false;


// 处理数据包
static void communicate_process_received_frames(uint8_t *command_buffer, uint16_t buffer_size);

// 处理FPGA 请求队列
static void communicate_process_request_queue(void);

// 保存一条等待投递给TestTask的通讯响应
static void communicate_prepare_response(const fpga_request_t *request, fpga_response_status_enum response_status);

// 尝试把通讯结果投递给TestTask
static void communicate_try_report_response(void);

// 检查当前写请求是否等待应答超时
static void communicate_check_pending_timeout(void);

// 计算CommunicateTask下一次需要被唤醒的时间
static TickType_t communicate_get_wait_ticks(void);

/**
 * @brief FPGA串口通信任务
 *
 * 该任务是接收数据的唯一消费者。DMA中断只把字节放入环形缓冲区并发送任务通知，
 * 本任务被唤醒后负责从连续字节流中取出一包或多包完整、CRC正确的数据。
 *
 * 当前阶段只完成“可靠取包”，还没有根据寄存器地址解析温度、状态等业务数据。
 */
void start_communicate_task(void *argument)
{
    // get_command成功后，完整帧会复制到该数组中
    uint8_t command_buffer[FPGA_COMMAND_BUFFER_SIZE] = {0};
    //  任务通知事件
    uint32_t event_data = 0;
    TickType_t wait_ticks;
    (void) argument;

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
        wait_ticks = communicate_get_wait_ticks();
        // 没有数据时任务在这里阻塞，不占用CPU；RX中断到达后由xTaskNotifyFromISR唤醒
        // event_data:获取通讯任务唤醒要要处理的事件 ,portMAX_DELAY表示一直等到有数据
        xTaskNotifyWait(0x00000000, 0xFFFFFFFF, &event_data, wait_ticks);
        // 通知值中有FPGA串口接收事件标志位
        if (event_data & FPGA_RX_EVENT)
        {
            communicate_process_received_frames(command_buffer, FPGA_COMMAND_BUFFER_SIZE);
        }
        communicate_check_pending_timeout();

        communicate_try_report_response();

        /*
         * 无论本轮是不是TX通知，都检查一次队列。
         * 这样旧事务结束后，不需要依赖额外通知才能启动队列中的下一项请求。
         */
        communicate_process_request_queue();
    }
}

static void communicate_process_received_frames(uint8_t *command_buffer, uint16_t buffer_size)
{
    // 当前完整帧的实际长度，返回0表示暂时没有完整帧
    uint16_t command_length;
    // 一次通知不等于一包数据：可能是半包，也可能是多个粘包 因此持续调用get_command，直到环形缓冲区暂时没有完整包为止
    while ((command_length = fpga_device.get_command(
                &fpga_device,
                command_buffer,
                buffer_size
            )) > 0U)
    {
        bool write_success = false;
        // 当有写入请求在等待ACK 且 获取到的帧为写入响应帧
        if (pending_request_valid && fpga_comm_parse_write_response(
                command_buffer, command_length, &write_success))
        {
            communicate_prepare_response(&pending_request, write_success ? FPGA_RESPONSE_SUCCESS : FPGA_RESPONSE_FAIL);
            pending_request_valid = false;
        }
        else
        {
            // 解析命令
            fpga_comm_parse_command(command_buffer, command_length);
        }
    }
}

static void communicate_process_request_queue(void)
{
    fpga_request_t request;


    // 正在等待FPGA应答，或者上一个结果还没交给TestTask时，都不能启动下一项事务。
    if (pending_request_valid == true || pending_response_valid == true)
    {
        return;
    }
    //
    while (xQueueReceive(fpga_request_queue, &request, 0U) == pdPASS)
    {
        bool send_success = false;
        switch (request.operation)
        {
            case FPGA_OPERATION_WRITE_TEST_PARAMS:
            {
                // 发送失败
                if (!fpga_comm_send_test_params(&request.request_data.test_params))
                {
                    // 响应发送失败
                    communicate_prepare_response(&request, FPGA_RESPONSE_SEND_FAILED);
                    return;
                }
                // 发送成功 更新等待响应请求
                pending_request = request;
                pending_request_start_tick = xTaskGetTickCount();
                pending_request_valid = true;
                return; // return掉,已经启动一个需要等待应答的写事务,不发送其他请求，防止混乱
            }
            case FPGA_OPERATION_READ_REGISTERS:
            {
                // 不需要响应
                fpga_comm_send_read_command(&request.request_data.read_instruction);
            }
            default:
                break;
        }
        if (send_success)
        {
            pending_request = request;
            pending_request_valid = true;
        }
        else
        {
            switch (request.operation)
            {
                case FPGA_OPERATION_WRITE_TEST_PARAMS:
                {
                    const fpga_response_t response = {
                        .request_id = request.request_id,
                        .operation = request.operation,
                        .response_status =
                        FPGA_RESPONSE_SEND_FAILED
                    };
                    test_report_fpga_response(&response);
                }
                default:
                    break;
            }
        }
    }
}

bool communicate_submit_request(const fpga_request_t *request)
{
    if (request == NULL ||
        fpga_request_queue == NULL ||
        communicate_taskHandle == NULL)
    {
        return false;
    }

    if (xQueueSend(fpga_request_queue, request, 0U) != pdPASS)
    {
        return false;
    }

    /*
     * 请求数据已经进入队列。
     * 任务通知只作为“队列中有新请求”的门铃。
     */
    return xTaskNotify(communicate_taskHandle, FPGA_TX_EVENT, eSetBits) == pdPASS;
}

static void communicate_prepare_response(const fpga_request_t *request, fpga_response_status_enum response_status)
{
    if (request == NULL)
    {
        return;
    }
    pending_response.request_id = request->request_id;
    pending_response.operation = request->operation;
    pending_response.response_status = response_status;
    pending_response_valid = true;
}

static void communicate_try_report_response(void)
{
    if (!pending_response_valid)
    {
        return;
    }

    if (test_report_fpga_response(&pending_response))
    {
        pending_response_valid = false;
    }
}

static void communicate_check_pending_timeout(void)
{
    if (!pending_request_valid)
    {
        return;
    }

    const TickType_t timeout_ticks =
            pdMS_TO_TICKS(FPGA_WRITE_RESPONSE_TIMEOUT_MS);

    const TickType_t elapsed_ticks =
            xTaskGetTickCount() - pending_request_start_tick;

    if (elapsed_ticks < timeout_ticks)
    {
        return;
    }

    communicate_prepare_response(
        &pending_request,
        FPGA_RESPONSE_TIMEOUT);

    pending_request_valid = false;
}


static TickType_t communicate_get_wait_ticks(void)
{
    if (pending_response_valid)
    {
        return pdMS_TO_TICKS(FPGA_RESPONSE_REPORT_RETRY_MS);
    }

    if (!pending_request_valid)
    {
        return portMAX_DELAY;
    }

    const TickType_t timeout_ticks =
            pdMS_TO_TICKS(FPGA_WRITE_RESPONSE_TIMEOUT_MS);

    const TickType_t elapsed_ticks =
            xTaskGetTickCount() - pending_request_start_tick;

    if (elapsed_ticks >= timeout_ticks)
    {
        return 0U;
    }

    return timeout_ticks - elapsed_ticks;
}
