//
// Created by 74222 on 2026/7/16.
//

#include "../Inc/uart_device.h"

#include <string.h>

#include "main.h"
#include "semphr.h"

#define RECEIVE_BUFFER_SIZE_OF_FPGA 128U
// 保留一个空槽区分满和空，物理长度多分配1字节，实际可用容量仍为256字节
#define RING_BUFFER_SIZE_OF_FPGA (RECEIVE_BUFFER_SIZE_OF_FPGA * 2U + 1U)
#define FPGA_DEVICE_ADDRESS 0x01U
#define FPGA_FUNCTION_READ 0x03U
#define FPGA_FUNCTION_WRITE 0x06U
#define FPGA_FRAME_HEADER_SIZE 6U
#define FPGA_FRAME_FIXED_SIZE 8U

/*
 * 接收数据流说明：
 *
 * FPGA发送字节
 *     ↓
 * USART2 RX DMA先写入rx_datas临时缓冲区
 *     ↓
 * UART IDLE或DMA TC事件进入HAL_UARTEx_RxEventCallback
 *     ↓
 * 中断回调把本次有效字节复制到fpga_ring_buffer
 *     ↓
 * 中断通过FreeRTOS任务通知唤醒CommunictaeTask
 *     ↓
 * 任务调用get_command，从环形缓冲区拼出一包CRC正确的数据
 *
 * 中断只负责快速搬运数据，查找包头、计算长度和CRC校验都在任务中完成。
 */

/** 串口信息数据 */
typedef struct
{
    // HAL串口句柄，fpga_device绑定USART2
    UART_HandleTypeDef *uart_handle;
    // DMA发送完成信号量：发送任务等待，中断回调释放
    SemaphoreHandle_t tx_semaphore;
    // 接收任务句柄：RX中断通过它通知CommunictaeTask
    TaskHandle_t rx_task_handle;
    // 软件环形缓冲区空间不足导致的丢块次数
    uint32_t rx_overflow_count;
    // ReceiveToIdle DMA重新启动失败次数
    uint32_t rx_restart_error_count;
    // DMA单次接收使用的临时缓冲区，不直接交给任务长期保存
    uint8_t rx_datas[RECEIVE_BUFFER_SIZE_OF_FPGA];
} UART_Data;

typedef struct Ring_Buffer Ring_Buffer;

/** 环形缓冲区数据 */
struct Ring_Buffer
{
    // 环形缓冲区数组
    uint8_t *buffer;
    // 环形缓冲区物理长度，实际可用容量为buffer_size - 1
    uint16_t buffer_size;
    // 缓冲区读索引，仅由任务更新
    volatile uint16_t read_index;
    // 缓冲区写索引，仅由中断更新
    volatile uint16_t write_index;
    // 缓冲区写入函数
    uint16_t (*buffer_write)(Ring_Buffer *ring_buffer, const uint8_t *data, uint16_t length);
    // 获取一包完整且CRC正确的指令
    uint16_t (*command_get)(Ring_Buffer *ring_buffer, uint8_t *command, uint16_t command_capacity);
};

static void ringBuffer_add_readIndex(Ring_Buffer *ring_buffer, uint16_t length);
static uint8_t ringBuffer_read(const Ring_Buffer *ring_buffer, uint16_t index);
static uint16_t ringBuffer_get_length(const Ring_Buffer *ring_buffer);
static uint16_t ringBuffer_get_remainLength(const Ring_Buffer *ring_buffer);
static uint16_t ringBuffer_write(Ring_Buffer *ring_buffer, const uint8_t *data, uint16_t length);
static uint16_t ringBuffer_get_command(Ring_Buffer *ring_buffer, uint8_t *command, uint16_t command_capacity);
static uint16_t modbus_crc16(const uint8_t *data, uint16_t length);

static int stm32_dma_uart_init(UART_Device *uart_device_p);
static int stm32_uart_send(const UART_Device *uart_device_p, const uint8_t *datas, uint16_t length, uint32_t timeout);
static void stm32_uart_bind_rx_task(UART_Device *uart_device_p, TaskHandle_t task_handle);
static uint16_t stm32_uart_get_command(const UART_Device *uart_device_p, uint8_t *command, uint16_t command_capacity);
static HAL_StatusTypeDef stm32_uart_start_receive(UART_Data *uart_data);

// FPGA串口通讯设备数据
static UART_Data fpga_device_uart_data =
{
    .uart_handle = &huart2,
    .tx_semaphore = NULL,
    .rx_task_handle = NULL,
    .rx_overflow_count = 0,
    .rx_restart_error_count = 0,
    .rx_datas = {0}
};

// 与FPGA通讯的串口设备
UART_Device fpga_device =
{
    .name = "fpga_device",
    .init = stm32_dma_uart_init,
    .send_data = stm32_uart_send,
    .bind_rx_task = stm32_uart_bind_rx_task,
    .get_command = stm32_uart_get_command,
    .uart_data = &fpga_device_uart_data
};

// 与FPGA通讯的接收环形缓冲区
static uint8_t ring_buffer_of_fpga[RING_BUFFER_SIZE_OF_FPGA];
static Ring_Buffer fpga_ring_buffer =
{
    .buffer = ring_buffer_of_fpga,
    .buffer_size = RING_BUFFER_SIZE_OF_FPGA,
    .read_index = 0,
    .write_index = 0,
    .buffer_write = ringBuffer_write,
    .command_get = ringBuffer_get_command
};

/**
 * @brief 启动一次USART2 DMA空闲接收
 * @param uart_data 串口私有数据
 * @return HAL_OK表示启动成功，其余值表示HAL拒绝或启动失败
 *
 * 当前RX DMA使用Normal模式，所以每次IDLE或TC结束后都必须重新调用本函数。
 * 这里关闭HT半传输中断，因为本方案只在IDLE或TC时把一整段有效数据写入软件环形缓冲区。
 */
static HAL_StatusTypeDef stm32_uart_start_receive(UART_Data *uart_data)
{
    HAL_StatusTypeDef status;

    if (uart_data == NULL || uart_data->uart_handle == NULL)
    {
        return HAL_ERROR;
    }

    status = HAL_UARTEx_ReceiveToIdle_DMA(
        uart_data->uart_handle,
        uart_data->rx_datas,
        RECEIVE_BUFFER_SIZE_OF_FPGA
    );

    if (status == HAL_OK && uart_data->uart_handle->hdmarx != NULL)
    {
        __HAL_DMA_DISABLE_IT(uart_data->uart_handle->hdmarx, DMA_IT_HT);
    }

    return status;
}

/**
 * @brief 初始化FPGA串口设备
 * @return 0表示成功，-1表示参数、信号量或DMA接收初始化失败
 *
 * 二值信号量只创建一次，用于等待TX DMA完成；随后启动第一轮ReceiveToIdle DMA接收。
 */
static int stm32_dma_uart_init(UART_Device *uart_device_p)
{
    UART_Data *uart_data;

    if (uart_device_p == NULL || uart_device_p->uart_data == NULL)
    {
        return -1;
    }

    uart_data = uart_device_p->uart_data;

    if (uart_data->tx_semaphore == NULL)
    {
        uart_data->tx_semaphore = xSemaphoreCreateBinary();
        if (uart_data->tx_semaphore == NULL)
        {
            return -1;
        }
    }

    if (stm32_uart_start_receive(uart_data) != HAL_OK)
    {
        return -1;
    }

    return 0;
}

/**
 * @brief 使用DMA发送一段串口数据，并等待发送完成中断
 * @param timeout 等待时间，单位为毫秒
 * @return 0表示发送完成，-1表示参数错误、DMA启动失败或等待超时
 *
 * HAL_UART_Transmit_DMA()只负责启动DMA，不代表数据已经发送完毕。
 * 真正发送完成后HAL_UART_TxCpltCallback()会从ISR释放信号量，本函数才能返回成功。
 */
static int stm32_uart_send(const UART_Device *uart_device_p, const uint8_t *datas, uint16_t length, uint32_t timeout)
{
    UART_Data *uart_data;

    if (uart_device_p == NULL || uart_device_p->uart_data == NULL || datas == NULL || length == 0U)
    {
        return -1;
    }

    uart_data = uart_device_p->uart_data;
    if (uart_data->tx_semaphore == NULL)
    {
        return -1;
    }

    // 清除上一次异常流程可能遗留的发送完成信号
    (void)xSemaphoreTake(uart_data->tx_semaphore, 0U);

    if (HAL_UART_Transmit_DMA(uart_data->uart_handle, datas, length) != HAL_OK)
    {
        return -1;
    }

    if (xSemaphoreTake(uart_data->tx_semaphore, timeout) != pdTRUE)
    {
        (void)HAL_UART_AbortTransmit(uart_data->uart_handle);
        return -1;
    }

    return 0;
}

/**
 * @brief 绑定负责消费接收数据的FreeRTOS任务
 *
 * 必须在启动DMA接收前完成绑定，否则早期RX中断没有目标任务可以通知。
 */
static void stm32_uart_bind_rx_task(UART_Device *uart_device_p, TaskHandle_t task_handle)
{
    UART_Data *uart_data;

    if (uart_device_p == NULL || uart_device_p->uart_data == NULL)
    {
        return;
    }

    uart_data = uart_device_p->uart_data;
    uart_data->rx_task_handle = task_handle;
}

/**
 * @brief 从FPGA专用环形缓冲区尝试取出一包完整数据
 * @return 成功时返回完整包长度，返回0表示当前还没有完整包
 *
 * 这里只把UART设备接口转发到fpga_ring_buffer，实际的寻帧和CRC校验由ringBuffer_get_command完成。
 */
static uint16_t stm32_uart_get_command(const UART_Device *uart_device_p, uint8_t *command, uint16_t command_capacity)
{
    if (uart_device_p != &fpga_device)
    {
        return 0;
    }

    return fpga_ring_buffer.command_get(&fpga_ring_buffer, command, command_capacity);
}

/**
 * @brief 消费指定数量的字节并向前移动读索引
 *
 * 该函数只允许在CommunictaeTask中调用。使用取模运算处理跨越数组尾部的情况。
 * __DMB()保证任务已经读取完数据后，才向ISR发布新的read_index。
 */
static void ringBuffer_add_readIndex(Ring_Buffer *ring_buffer, uint16_t length)
{
    uint32_t next_index;

    if (ring_buffer == NULL || ring_buffer->buffer_size == 0U)
    {
        return;
    }

    next_index = (uint32_t)ring_buffer->read_index + length;
    __DMB();
    ring_buffer->read_index = (uint16_t)(next_index % ring_buffer->buffer_size);
}

/**
 * @brief 按绝对逻辑索引读取一个字节
 *
 * index超过数组末尾时自动回绕，所以解析跨越环形缓冲区尾部的帧时不需要分两段读取。
 */
static uint8_t ringBuffer_read(const Ring_Buffer *ring_buffer, uint16_t index)
{
    return ring_buffer->buffer[index % ring_buffer->buffer_size];
}

/**
 * @brief 计算环形缓冲区中尚未被任务消费的字节数
 *
 * write_index在read_index后方时直接相减；发生物理回绕时，需要加上数组尾部到起点的长度。
 * 本实现保留一个空槽，因此read_index == write_index始终只表示空，不会同时表示满。
 */
static uint16_t ringBuffer_get_length(const Ring_Buffer *ring_buffer)
{
    uint16_t read_index;
    uint16_t write_index;

    if (ring_buffer == NULL || ring_buffer->buffer == NULL || ring_buffer->buffer_size < 2U)
    {
        return 0;
    }

    read_index = ring_buffer->read_index;
    write_index = ring_buffer->write_index;
    __DMB();

    if (write_index >= read_index)
    {
        return write_index - read_index;
    }

    return ring_buffer->buffer_size - read_index + write_index;
}

/**
 * @brief 计算当前还能写入多少字节
 *
 * 减去1是为了永久保留一个空槽，用它区分“空”和“满”两种状态。
 */
static uint16_t ringBuffer_get_remainLength(const Ring_Buffer *ring_buffer)
{
    if (ring_buffer == NULL || ring_buffer->buffer_size < 2U)
    {
        return 0;
    }

    return (uint16_t)(ring_buffer->buffer_size - 1U - ringBuffer_get_length(ring_buffer));
}

/**
 * @brief 将DMA本次收到的一整块数据写入软件环形缓冲区
 * @param ring_buffer 目标环形缓冲区
 * @param data DMA临时接收缓冲区中的有效数据起始地址
 * @param length 本次需要写入的字节数
 * @return 成功返回length，空间不足或参数错误返回0
 *
 * 写入策略：
 * 1. 先检查参数和剩余空间，空间不足时直接返回0，不写入任何字节；
 * 2. 使用当前write_index作为写入起点；
 * 3. 如果从write_index到数组末尾足够放下length字节，只需要复制一次；
 * 4. 如果放不下，则先复制第一段到数组末尾，再从buffer[0]开始复制第二段；
 * 5. 数据全部复制完成后，最后再更新write_index，把新数据一次性发布给读取任务。
 *
 * 这里采用“全写或全不写”策略，不能只写半个DMA块，否则可能把一包协议数据人为截断。
 * 环形缓冲区永久保留一个空槽，所以write_index追上read_index时表示空，不表示满。
 */
static uint16_t ringBuffer_write(Ring_Buffer *ring_buffer, const uint8_t *data, uint16_t length)
{
    uint16_t write_index;
    uint16_t first_length;
    uint16_t second_length;
    uint16_t new_write_index;

    // 参数无效或本次没有数据时，不修改环形缓冲区状态
    if (ring_buffer == NULL || ring_buffer->buffer == NULL || data == NULL || length == 0U)
    {
        return 0;
    }

    // 剩余空间不足时整块丢弃，避免只写入半包数据导致后续解析错位
    if (ringBuffer_get_remainLength(ring_buffer) < length)
    {
        return 0;
    }

    // 先记录当前写指针，后续复制都基于局部变量计算，避免提前暴露未写完的数据
    write_index = ring_buffer->write_index;

    // 第一段最多写到物理数组末尾；如果本次数据没跨尾部，first_length就是length
    first_length = (uint16_t)(ring_buffer->buffer_size - write_index);
    if (first_length > length)
    {
        first_length = length;
    }

    // 第二段表示跨过数组末尾后，需要从buffer[0]继续写入的字节数
    second_length = (uint16_t)(length - first_length);

    // 复制第一段：[write_index, write_index + first_length)
    memcpy(ring_buffer->buffer + write_index, data, first_length);
    if (second_length > 0U)
    {
        // 发生回绕时，复制第二段到数组头部：[0, second_length)
        memcpy(ring_buffer->buffer, data + first_length, second_length);
    }

    // 写指针前进length字节，超过数组末尾时通过取模回到数组头部
    new_write_index = (uint16_t)(((uint32_t)write_index + length) % ring_buffer->buffer_size);

    // 保证数据内容先写入RAM，再让任务看到新的write_index
    __DMB();
    ring_buffer->write_index = new_write_index;

    return length;
}

/**
 * @brief 从连续字节流中寻找一包完整且CRC正确的旧协议数据
 * @param command 输出缓冲区
 * @param command_capacity 输出缓冲区容量
 * @return 成功返回帧长度；返回0表示暂时没有完整帧
 *
 * 旧协议入站帧格式：
 * [0x01][0x03或0x06][寄存器地址2字节][寄存器数量2字节][数据2*N字节][CRC低][CRC高]
 *
 * 本函数能够处理：
 * 1. 帧前存在垃圾字节；
 * 2. 一帧被多次DMA回调拆开；
 * 3. 一次DMA回调中包含多个粘包；
 * 4. CRC错误帧后紧跟合法帧。
 */
static uint16_t ringBuffer_get_command(Ring_Buffer *ring_buffer, uint8_t *command, uint16_t command_capacity)
{
    uint16_t available_length;
    uint16_t read_index;
    uint16_t register_count;
    uint16_t frame_length;
    uint16_t crc_calculated;
    uint16_t crc_received;
    uint32_t frame_length_calculated;
    uint8_t function_code;

    if (ring_buffer == NULL || command == NULL || command_capacity < FPGA_FRAME_FIXED_SIZE)
    {
        return 0;
    }

    // 至少收到前6字节后，才能读取地址、功能码和寄存器数量
    while (ringBuffer_get_length(ring_buffer) >= FPGA_FRAME_HEADER_SIZE)
    {
        read_index = ring_buffer->read_index;

        // 当前字节不是设备地址0x01，说明它是噪声或残帧，丢弃1字节继续找包头
        if (ringBuffer_read(ring_buffer, read_index) != FPGA_DEVICE_ADDRESS)
        {
            ringBuffer_add_readIndex(ring_buffer, 1U);
            continue;
        }

        // 当前只接受旧协议中的读功能码0x03和自定义写功能码0x06
        function_code = ringBuffer_read(ring_buffer, (uint16_t)(read_index + 1U));
        if (function_code != FPGA_FUNCTION_READ && function_code != FPGA_FUNCTION_WRITE)
        {
            ringBuffer_add_readIndex(ring_buffer, 1U);
            continue;
        }

        // 寄存器数量采用大端顺序：高字节在前，低字节在后
        register_count = (uint16_t)((uint16_t)ringBuffer_read(ring_buffer, (uint16_t)(read_index + 4U)) << 8U);
        register_count |= ringBuffer_read(ring_buffer, (uint16_t)(read_index + 5U));

        // 固定字段和CRC共8字节，每个寄存器占2字节
        frame_length_calculated = FPGA_FRAME_FIXED_SIZE + (uint32_t)register_count * 2U;
        if (register_count == 0U
            || frame_length_calculated > command_capacity
            || frame_length_calculated > (uint32_t)(ring_buffer->buffer_size - 1U))
        {
            ringBuffer_add_readIndex(ring_buffer, 1U);
            continue;
        }

        frame_length = (uint16_t)frame_length_calculated;
        available_length = ringBuffer_get_length(ring_buffer);
        if (available_length < frame_length)
        {
            // 说明发生了拆包：现有字节先保留，等待下一次DMA数据到达
            return 0;
        }

        // 使用ringBuffer_read逐字节复制，可以自然处理帧跨越数组尾部的情况
        for (uint16_t i = 0; i < frame_length; i++)
        {
            command[i] = ringBuffer_read(ring_buffer, (uint16_t)(read_index + i));
        }

        // 最后两个字节不参与CRC计算；线上顺序是CRC低字节在前、高字节在后
        crc_calculated = modbus_crc16(command, (uint16_t)(frame_length - 2U));
        crc_received = (uint16_t)command[frame_length - 2U]
                     | ((uint16_t)command[frame_length - 1U] << 8U);

        if (crc_calculated != crc_received)
        {
            // CRC失败时只丢弃当前候选包头，避免把后方可能存在的合法粘包一起删除
            ringBuffer_add_readIndex(ring_buffer, 1U);
            continue;
        }

        // CRC通过后才真正消费整帧，command中的数据可以安全交给上层解析
        ringBuffer_add_readIndex(ring_buffer, frame_length);
        return frame_length;
    }

    return 0;
}

/**
 * @brief 计算CRC-16/MODBUS
 *
 * 初始值为0xFFFF，使用反射多项式0xA001。返回的16位数在线路上传输时低字节在前。
 */
static uint16_t modbus_crc16(const uint8_t *data, uint16_t length)
{
    uint16_t crc = 0xFFFFU;

    for (uint16_t position = 0; position < length; position++)
    {
        crc ^= data[position];
        for (uint8_t bit = 0; bit < 8U; bit++)
        {
            if ((crc & 0x0001U) != 0U)
            {
                crc >>= 1U;
                crc ^= 0xA001U;
            }
            else
            {
                crc >>= 1U;
            }
        }
    }

    return crc;
}

/**
 * @brief UART DMA发送完成回调
 *
 * 本函数运行在中断上下文，因此必须使用xSemaphoreGiveFromISR，不能使用普通xSemaphoreGive。
 */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    UART_Data *fpga_uart_data = fpga_device.uart_data;
    BaseType_t higher_priority_task_woken = pdFALSE;

    if (huart == fpga_uart_data->uart_handle)
    {
        xSemaphoreGiveFromISR(fpga_uart_data->tx_semaphore, &higher_priority_task_woken);
        portYIELD_FROM_ISR(higher_priority_task_woken);
    }
}

/**
 * @brief UART ReceiveToIdle DMA接收事件回调
 * @param size DMA本轮从rx_datas[0]开始写入的有效字节数
 *
 * 回调运行在中断上下文，只做三件事：
 * 1. 把DMA临时缓冲区中的有效字节复制到软件环形缓冲区；
 * 2. 重新启动下一轮Normal DMA接收；
 * 3. 通知CommunictaeTask处理数据。
 *
 * 包头查找、长度计算和CRC校验不在中断中执行，避免中断占用时间过长。
 */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t size)
{
    UART_Data *fpga_uart_data = fpga_device.uart_data;
    BaseType_t higher_priority_task_woken = pdFALSE;
    HAL_UART_RxEventTypeTypeDef event_type = HAL_UARTEx_GetRxEventType(huart);
    if (huart == fpga_uart_data->uart_handle) {
        if (event_type == HAL_UART_RXEVENT_IDLE ||  event_type == HAL_UART_RXEVENT_TC)
        {
            uint16_t written_length;
            // 环形缓冲区空间不足时整块拒绝，并通过计数器记录本次丢块
            written_length = fpga_ring_buffer.buffer_write(
                &fpga_ring_buffer,
                fpga_uart_data->rx_datas,
                size
            );

            if (written_length != size)
            {
                fpga_uart_data->rx_overflow_count++;
            }

            // Normal DMA在IDLE或TC后停止，因此必须立即重新挂接下一轮接收
            if (stm32_uart_start_receive(fpga_uart_data) != HAL_OK)
            {
                fpga_uart_data->rx_restart_error_count++;
            }

            // 通知只表示“可能有新数据”，任务醒来后需要循环取出所有完整包
            if (fpga_uart_data->rx_task_handle != NULL)
            {
                xTaskNotifyFromISR(fpga_uart_data->rx_task_handle,FPGA_RX_EVENT,eSetBits,&higher_priority_task_woken);
                portYIELD_FROM_ISR(higher_priority_task_woken);
            }
        }
    }
}
