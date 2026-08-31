#include "FreeRTOS.h"
#include "task.h"
#include "bsp_uart_device.h"
#include "main.h"
#include "fpga_comm.h"
#include "test_task.h"
#include "communicate_task.h"
//
// Created by 74222 on 2026/7/17.
//

// DMA单次接收缓冲区为128字节，完整帧缓冲区保持相同容量，避免取帧时发生截断
#define FPGA_COMMAND_BUFFER_SIZE 128U
// 写请求发送成功后，等待FPGA应答的最长时间
#define FPGA_WRITE_RESPONSE_TIMEOUT_MS            (200U)
// 最大发送次数：首次发送1次，超时后最多重传4次
#define FPGA_WRITE_MAX_RETRY_COUNT                 5U

// TestTask事件队列已满时，再次尝试投递通讯结果的间隔
#define FPGA_RESPONSE_REPORT_RETRY_MS             10U

extern UART_Device fpga_device;


/* -------------------------发送请求相关----------------------- */
// 是否正在等待 FPGA 对写请求的应答;同一时刻只允许存在一个等待应答的请求
static bool awaiting_fpga_write_response = false;
// 当前等待FPGA应答的请求，用于匹配响应及超时重传
static fpga_request_t pending_request;
// 当前等待请求最近一次发送成功的时刻，用于计算应答超时
static TickType_t await_response_start_tick = 0U;
// 当前等待请求已发生的超时次数；首次发送前为0
static uint16_t pending_request_retry_count = 0U;

static bool awaiting_test_outcome_response = false;


/* -------------------------响应投递相关----------------------- */
// 已生成但尚未成功投递给TestTask的通讯结果
static fpga_response_t pending_response;
// 是否存在待投递结果；为true时FPGA事务已经结束，但TestTask尚未收到Response结果
static bool response_report_pending = false;


// 从UART接收流中提取并处理所有完整帧
static void communicate_process_received_frames(uint8_t *command_buffer, uint16_t buffer_size);

// 从FPGA请求队列中取出请求并启动对应的串口事务
static void communicate_process_request_queue(void);

// 发送当前等待请求，首次发送和超时重传共用
static bool communicate_send_pending_request(void);

// 根据请求生成一条待投递给TestTask的通讯结果
static void communicate_prepare_response(const fpga_request_t *request, fpga_response_status_enum response_status);

// 尝试把通讯结果投递给TestTask，队列已满时保留结果等待下次重试
static void communicate_try_report_response(void);

// 检查当前写请求是否应答超时，并执行重传或上报最终失败
static void communicate_check_pending_timeout(void);

// 根据等待应答和等待投递状态，计算CommunicateTask的最长阻塞时间
static TickType_t communicate_get_wait_ticks(void);

/**
 * @brief FPGA串口通信任务
 *
 * 该任务是FPGA串口的事务管理者，也是接收数据的唯一消费者。DMA中断只把字节
 * 放入环形缓冲区并发送任务通知；本任务负责提取完整帧、发送队列中的请求、匹配
 * 写应答、执行超时重传，并将写事务结果投递给TestTask。
 *
 * 为防止写应答与请求错配，同一时刻只允许存在一个等待FPGA应答的写事务。
 */
void start_communicate_task(void *argument)
{
    // get_command成功后会将一帧完整数据复制到该缓冲区
    uint8_t command_buffer[FPGA_COMMAND_BUFFER_SIZE] = {0};
    // 保存本轮唤醒时取得的任务通知事件位
    uint32_t event_data = 0;
    TickType_t wait_ticks;

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
        // 没有事件时在此阻塞；等待时长还受写应答超时和响应投递重试周期限制
        xTaskNotifyWait(0x00000000, 0xFFFFFFFFU, &event_data, wait_ticks);
        // RX事件表示环形缓冲区中可能出现了一个或多个完整帧
        if (event_data & FPGA_RX_EVENT)
        {
            communicate_process_received_frames(command_buffer, FPGA_COMMAND_BUFFER_SIZE);
        }
        // 即使本轮由其他事件唤醒，也必须检查正在等待的写请求是否已经超时
        communicate_check_pending_timeout();

        // FPGA事务结束后，持续尝试将其结果可靠地交给TestTask
        communicate_try_report_response();
        /*
         * TX事件只表示“请求队列中可能有新请求”，实际请求数量以队列内容为准。
         */
        if (event_data & FPGA_TX_EVENT)
        {
            communicate_process_request_queue();
        }
    }
}

static void communicate_process_received_frames(uint8_t *command_buffer, uint16_t buffer_size)
{
    // 当前完整帧的实际长度；get_command返回0表示暂时没有完整帧
    uint16_t command_length;
    // 一次RX通知可能对应半帧或多个粘连帧，因此持续取帧直到当前没有完整帧
    while ((command_length = fpga_device.get_command(
                &fpga_device,
                command_buffer,
                buffer_size
            )) > 0U)
    {
        bool write_success = false;
        // 仅在存在等待请求时尝试匹配写应答，避免普通上报帧被误判为事务响应
        if (awaiting_fpga_write_response && fpga_comm_parse_write_response(
                command_buffer, command_length, &write_success))
        {
            communicate_prepare_response(&pending_request, write_success ? FPGA_RESPONSE_SUCCESS : FPGA_RESPONSE_FAIL);
            awaiting_fpga_write_response = false;
        }
        else if (awaiting_fpga_write_response && fpga_comm_parse_outcome_response(command_buffer, command_length))
        {
            communicate_prepare_response(&pending_request, FPGA_RESPONSE_TEST_OUTCOME);
            awaiting_test_outcome_response = false;
        }
        else
        {
            // 非当前写事务的响应帧，按FPGA主动上报或读取响应进行解析
            fpga_comm_parse_command(command_buffer, command_length);
        }
    }
}

static void communicate_process_request_queue(void)
{
    fpga_request_t request;


    // 写事务尚未结束或其结果尚未交付时，不启动下一项事务，避免请求与响应错配
    if (awaiting_fpga_write_response == true || awaiting_test_outcome_response == true || response_report_pending ==
        true)
    {
        return;
    }
    // 连续处理无需等待应答的读取请求；启动一个写请求后立即退出并等待其应答
    while (xQueueReceive(fpga_request_queue, &request, 0U) == pdPASS)
    {
        switch (request.operation)
        {
            case FPGA_OPERATION_READ_REGISTERS:
            {
                /*
                 * 读取请求用于周期轮询，不占用pending_request，也不向业务层回传发送结果。
                 * UART发送失败时放弃本次轮询，后续轮询仍可继续执行。
                 */
                fpga_comm_send_read_command(&request.request_data.read_instruction);
                break;
            }
            case FPGA_OPERATION_READ_OUTCOME:
            case FPGA_OPERATION_WRITE_TEST_PARAMS:
            case FPGA_OPERATION_WRITE_REGISTER:
            {
                /*
                 * 写入完整测试参数和写入单个寄存器共用同一套可靠事务流程。
                 * 上方case不写break是有意穿透：两类请求均需等待应答、超时重传并上报结果。
                 */
                pending_request = request;
                pending_request_retry_count = 0U;

                if (!communicate_send_pending_request())
                {
                    communicate_prepare_response(&pending_request, FPGA_RESPONSE_SEND_FAILED);
                    return;
                }
                if (request.operation == FPGA_OPERATION_WRITE_TEST_PARAMS || request.operation ==
                    FPGA_OPERATION_WRITE_REGISTER)
                    awaiting_fpga_write_response = true;
                else if (request.operation == FPGA_OPERATION_READ_OUTCOME)
                    awaiting_test_outcome_response = true;
                break;
            }

            default:
                break;
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

    // 请求数据已经进入队列；任务通知只作为“队列中有新请求”的门铃
    xTaskNotify(communicate_taskHandle, FPGA_TX_EVENT, eSetBits);
    return true;
}

static bool communicate_send_pending_request(void)
{
    bool send_success = false;

    switch (pending_request.operation)
    {
        case FPGA_OPERATION_WRITE_TEST_PARAMS:
            // 按协议封装并发送完整测试参数
            send_success = fpga_comm_send_test_params(&pending_request.request_data.test_params);
            break;

        case FPGA_OPERATION_WRITE_REGISTER:
            // 按协议封装并发送单个16位寄存器值
            send_success = fpga_comm_send_write_register(&pending_request.request_data.write_register);
            break;
        case FPGA_OPERATION_READ_OUTCOME:
            // 发送读取测试结果记录
            send_success = fpga_comm_send_read_command(&pending_request.request_data.read_instruction);
            break;
        default:
            break;
    }

    if (send_success)
    {
        // 只有UART发送成功后才开始计算本轮FPGA应答等待时间
        await_response_start_tick = xTaskGetTickCount();
    }

    return send_success;
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
    response_report_pending = true;
}

static void communicate_try_report_response(void)
{
    if (!response_report_pending)
    {
        return;
    }

    if (test_report_fpga_response(&pending_response))
    {
        // TestTask已成功接收结果，可以允许后续事务继续执行
        response_report_pending = false;
    }
}

static void communicate_check_pending_timeout(void)
{
    if (!awaiting_fpga_write_response && !awaiting_test_outcome_response)
    {
        return;
    }

    const TickType_t timeout_ticks =
            pdMS_TO_TICKS(FPGA_WRITE_RESPONSE_TIMEOUT_MS);

    const TickType_t elapsed_ticks =
            xTaskGetTickCount() - await_response_start_tick;

    if (elapsed_ticks < timeout_ticks)
    {
        return;
    }
    // 先累计本轮超时次数；未达到阈值时重新发送同一请求
    if (++pending_request_retry_count < FPGA_WRITE_MAX_RETRY_COUNT)
    {
        if (communicate_send_pending_request())
        {
            // 重传成功启动后，从本次UART发送完成时刻重新等待FPGA应答
            return;
        }

        // UART未能启动或完成重传，通信链路已明确失败，不再等待应答
        communicate_prepare_response(&pending_request, FPGA_RESPONSE_SEND_FAILED);
        awaiting_fpga_write_response = false;
        awaiting_test_outcome_response = false;
        return;
    }

    // 首次发送及四次重传均未收到应答，向TestTask报告最终超时
    communicate_prepare_response(&pending_request, FPGA_RESPONSE_TIMEOUT);
    awaiting_fpga_write_response = false;
    awaiting_test_outcome_response = false;
}


static TickType_t communicate_get_wait_ticks(void)
{
    if (response_report_pending)
    {
        // TestTask队列曾经满载时，定期唤醒并重试投递，避免结果永久滞留
        return pdMS_TO_TICKS(FPGA_RESPONSE_REPORT_RETRY_MS);
    }

    if (!awaiting_fpga_write_response && !awaiting_test_outcome_response)
    {
        // 没有内部定时事务，仅等待任务通知唤醒
        return portMAX_DELAY;
    }

    const TickType_t timeout_ticks =
            pdMS_TO_TICKS(FPGA_WRITE_RESPONSE_TIMEOUT_MS);

    const TickType_t elapsed_ticks =
            xTaskGetTickCount() - await_response_start_tick;

    if (elapsed_ticks >= timeout_ticks)
    {
        return 0U;
    }

    return timeout_ticks - elapsed_ticks;
}
