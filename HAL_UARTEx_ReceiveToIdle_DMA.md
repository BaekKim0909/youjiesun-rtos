`HAL_UARTEx_ReceiveToIdle_DMA()` 是 STM32 HAL 中用于“DMA 接收不定长 UART 数据”的常用 API，特别适合：

- FreeRTOS
- STM32H7
- AT 指令
- GPS
- Modbus
- RS485
- 上位机不定长数据
- 连续串口数据流

它的核心思想是：

```text
DMA负责搬运数据
UART的IDLE事件用于判断一段数据暂时结束
HAL通过回调通知应用程序
```

---

# 一、函数原型

```c
HAL_StatusTypeDef HAL_UARTEx_ReceiveToIdle_DMA(
    UART_HandleTypeDef *huart,
    uint8_t *pData,
    uint16_t Size
);
```

例如：

```c
#define UART_RX_BUF_SIZE 256

uint8_t uart_rx_buf[UART_RX_BUF_SIZE];

HAL_UARTEx_ReceiveToIdle_DMA(
    &huart1,
    uart_rx_buf,
    sizeof(uart_rx_buf)
);
```

---

# 二、三个参数的含义

## 1. `huart`

UART 句柄：

```c
&huart1
```

例如 CubeMX 生成：

```c
UART_HandleTypeDef huart1;
```

这个句柄内部包含：

- UART 外设地址
- 当前接收状态
- 接收缓冲区指针
- 目标接收长度
- DMA 句柄
- 错误状态
- 接收模式

---

## 2. `pData`

DMA 接收缓冲区首地址：

```c
uint8_t uart_rx_buf[256];

HAL_UARTEx_ReceiveToIdle_DMA(
    &huart1,
    uart_rx_buf,
    sizeof(uart_rx_buf)
);
```

DMA 会把 UART 接收寄存器中的数据搬运到：

```text
uart_rx_buf[0]
uart_rx_buf[1]
uart_rx_buf[2]
...
```

`pData` 只是告诉 HAL：

> 接收到的数据应该放在哪里。

它不是输入数据，也不会被这个函数“初始化”为 0。

---

## 3. `Size`

接收缓冲区容量：

```c
sizeof(uart_rx_buf)
```

如果：

```c
uint8_t uart_rx_buf[256];
```

那么：

```c
Size = 256
```

它表示：

> DMA 最多允许接收多少个元素。

对于常见的 8 位 UART 数据，通常一个元素就是一个字节。

---

# 三、这个函数内部大概做了什么

简化理解，调用：

```c
HAL_UARTEx_ReceiveToIdle_DMA(
    &huart1,
    uart_rx_buf,
    256
);
```

HAL 大致会执行以下操作：

```text
1. 检查 UART 当前是否处于可接收状态
2. 设置 huart->pRxBuffPtr = uart_rx_buf
3. 设置 huart->RxXferSize = 256
4. 设置接收模式为 Receive To Idle
5. 配置 DMA：
       外设地址 = USART_RDR
       内存地址 = uart_rx_buf
       传输数量 = 256
6. 注册 DMA 接收完成、半传输、错误回调
7. 清除可能存在的 UART IDLE 标志
8. 开启 UART IDLE 中断
9. 开启 UART DMA 接收请求
10. 函数立即返回
```

函数返回时，数据通常还没有接收完成。

```c
HAL_UARTEx_ReceiveToIdle_DMA(...);
```

是非阻塞函数。

它只是：

> 启动一次 DMA + UART IDLE 接收任务。

---

# 四、什么时候触发回调？

这个函数最终主要触发：

```c
HAL_UARTEx_RxEventCallback()
```

回调原型：

```c
void HAL_UARTEx_RxEventCallback(
    UART_HandleTypeDef *huart,
    uint16_t Size
);
```

例如：

```c
void HAL_UARTEx_RxEventCallback(
    UART_HandleTypeDef *huart,
    uint16_t Size
)
{
    if (huart->Instance == USART1)
    {
        // Size表示本次接收到的数据位置或长度
    }
}
```

注意：

对于 `HAL_UARTEx_ReceiveToIdle_DMA()`，主要使用的是：

```c
HAL_UARTEx_RxEventCallback()
```

而不是普通 DMA 接收时经常使用的：

```c
HAL_UART_RxCpltCallback()
```

---

# 五、触发回调的三种主要情况

## 情况一：检测到 UART IDLE

这是这个 API 最重要的功能。

假设：

```c
uint8_t uart_rx_buf[256];
```

上位机发送：

```text
ABCDEF\r\n
```

一共 8 个字节。

DMA 依次写入：

```text
uart_rx_buf[0] = 'A'
uart_rx_buf[1] = 'B'
uart_rx_buf[2] = 'C'
uart_rx_buf[3] = 'D'
uart_rx_buf[4] = 'E'
uart_rx_buf[5] = 'F'
uart_rx_buf[6] = '\r'
uart_rx_buf[7] = '\n'
```

然后线路空闲。

UART 检测到 IDLE，触发：

```c
HAL_UARTEx_RxEventCallback(&huart1, 8);
```

此时有效数据通常是：

```c
uart_rx_buf[0] ~ uart_rx_buf[7]
```

也就是：

```c
uart_rx_buf[0 ... Size - 1]
```

注意：

> IDLE 表示“接收过数据后，线路暂时空闲”，不是检测到了 `\r\n`。

所以如果数据中间停顿时间过长，一帧数据可能被拆成多次回调。

---

## 情况二：DMA 接收完成，触发 Transfer Complete

如果缓冲区大小为：

```c
Size = 256
```

当 DMA 收满 256 字节时，会产生 DMA Transfer Complete 事件。

在某些配置和 HAL 版本下，会调用：

```c
HAL_UARTEx_RxEventCallback(
    &huart1,
    256
);
```

这表示：

```text
uart_rx_buf[0] ~ uart_rx_buf[255]
```

都有新数据。

对于 DMA Normal 模式，接收完成后 DMA 通常会停止，后续需要重新启动接收。

---

## 情况三：DMA 半传输，Half Transfer

如果 DMA 的半传输中断开启，当 DMA 接收到一半数据时，也可能触发回调。

例如：

```c
Size = 256
```

接收到 128 字节时：

```c
HAL_UARTEx_RxEventCallback(&huart1, 128);
```

此时：

```c
uart_rx_buf[0] ~ uart_rx_buf[127]
```

可认为已经收到。

半传输事件主要适合：

- 连续数据流
- 高速数据接收
- DMA Circular 模式
- 希望尽早处理前半缓冲区的场景

不过 STM32 HAL 默认通常会开启 DMA 半传输中断，因此你可能看到：

```text
回调触发次数比预期多
```

如果你只关心 IDLE 事件，可以关闭 DMA 半传输中断：

```c
__HAL_DMA_DISABLE_IT(
    huart1.hdmarx,
    DMA_IT_HT
);
```

不同 HAL 和 DMA 控制器的宏可能略有差异，具体以工程头文件为准。

---

# 六、如何判断当前回调是 IDLE、HT 还是 TC？

在较新的 HAL 版本中，可以调用：

```c
HAL_UARTEx_GetRxEventType(huart);
```

示例：

```c
void HAL_UARTEx_RxEventCallback(
    UART_HandleTypeDef *huart,
    uint16_t Size
)
{
    HAL_UART_RxEventTypeTypeDef event;

    event = HAL_UARTEx_GetRxEventType(huart);

    if (event == HAL_UART_RXEVENT_IDLE)
    {
        // UART IDLE事件
    }
    else if (event == HAL_UART_RXEVENT_HT)
    {
        // DMA半传输事件
    }
    else if (event == HAL_UART_RXEVENT_TC)
    {
        // DMA传输完成事件
    }
}
```

不同 HAL 版本的类型名称可能存在差异，建议查看当前工程中的：

```text
stm32h7xx_hal_uart_ex.h
```

---

# 七、`Size` 到底表示什么？

这是最容易误解的地方。

很多人会认为：

```c
Size = 本次回调新增的数据长度
```

这并不总是正确。

更准确地说：

> `Size` 表示当前接收数据在缓冲区中的位置，或者说 DMA 已经写到缓冲区的哪个位置。

---

## DMA Normal 模式下

通常可以近似理解为本次有效数据长度。

例如：

```c
HAL_UARTEx_ReceiveToIdle_DMA(
    &huart1,
    uart_rx_buf,
    256
);
```

收到 20 字节后产生 IDLE：

```c
Size = 20;
```

有效数据：

```c
uart_rx_buf[0] ~ uart_rx_buf[19]
```

这时：

```c
有效长度 = Size
```

---

## DMA Circular 模式下

`Size` 通常表示 DMA 当前写入位置，不一定是本次新增长度。

例如缓冲区大小为 256：

```text
第一次回调：Size = 20
第二次回调：Size = 80
第三次回调：Size = 10
```

可能表示：

```text
第一次新增：buffer[0 ... 19]
第二次新增：buffer[20 ... 79]
第三次发生回绕：
           buffer[80 ... 255]
           buffer[0 ... 9]
```

所以 Circular 模式需要保存上一次位置：

```c
old_pos
```

然后根据：

```c
current_pos = Size
```

计算新增数据。

---

# 八、Normal 模式和 Circular 模式

DMA 的模式通常在 CubeMX 中配置。

## 1. DMA Normal 模式

流程：

```text
启动DMA
    ↓
接收数据
    ↓
IDLE或DMA满
    ↓
触发回调
    ↓
DMA通常停止
```

代码通常是：

```c
void HAL_UARTEx_RxEventCallback(
    UART_HandleTypeDef *huart,
    uint16_t Size
)
{
    if (huart->Instance == USART1)
    {
        // 处理或通知任务

        // Normal模式下通常需要重新启动
        HAL_UARTEx_ReceiveToIdle_DMA(
            huart,
            uart_rx_buf,
            sizeof(uart_rx_buf)
        );
    }
}
```

但是在 FreeRTOS 中，不建议直接在回调中重新启动并处理复杂业务。可以在任务中重新启动。

特点：

- 简单
- 适合一帧一帧接收
- 需要考虑重新启动接收
- 处理期间可能丢失下一帧

---

## 2. DMA Circular 模式

流程：

```text
启动DMA
    ↓
持续接收
    ↓
DMA到半缓冲区
    ↓
可能触发HT回调
    ↓
DMA到末尾
    ↓
可能触发TC回调
    ↓
DMA自动回到缓冲区开头
```

特点：

- DMA 不会因为到达末尾而停止
- 适合连续数据流
- 通常不需要每次回调重新启动 DMA
- 必须处理缓冲区回绕
- 必须防止 CPU 处理速度跟不上 DMA 写入速度

---

# 九、每次调用函数会初始化 `pData` 吗？

答案是：

> 不会初始化 `pData` 指向的内存内容。

例如：

```c
uint8_t rx_buf[128];

HAL_UARTEx_ReceiveToIdle_DMA(
    &huart1,
    rx_buf,
    sizeof(rx_buf)
);
```

这个函数不会执行：

```c
memset(rx_buf, 0, sizeof(rx_buf));
```

也不会保证：

```c
rx_buf[0 ... 127] = 0;
```

它主要做的是：

```text
把 pData 的地址配置给 DMA
把 Size 配置给 DMA传输计数器
启动UART DMA接收
```

因此，假设之前缓冲区内容是：

```text
AA BB CC DD EE FF ...
```

这次只收到 3 个字节：

```text
11 22 33
```

接收后缓冲区可能是：

```text
11 22 33 DD EE FF ...
```

只有：

```c
rx_buf[0] ~ rx_buf[2]
```

是本次新数据。

后面的内容仍然可能是上一次残留数据。

所以处理数据时必须使用回调提供的有效长度：

```c
for (uint16_t i = 0; i < Size; i++)
{
    ProcessByte(rx_buf[i]);
}
```

不能直接使用：

```c
strlen((char *)rx_buf);
```

因为 DMA 接收到的数据不一定带 `'\0'`。

---

# 十、重新调用时会不会从 `pData[0]` 开始写？

这取决于 DMA 模式和 HAL 重新配置方式。

## Normal 模式

重新调用：

```c
HAL_UARTEx_ReceiveToIdle_DMA(
    &huart1,
    uart_rx_buf,
    sizeof(uart_rx_buf)
);
```

通常会重新配置：

```text
内存地址 = uart_rx_buf
传输计数 = sizeof(uart_rx_buf)
```

因此会从：

```c
uart_rx_buf[0]
```

开始写入。

但它不会清空旧内容。

---

## Circular 模式

DMA 会持续循环写入：

```text
0 → 1 → 2 → ... → N-1 → 0
```

重新调用时具体行为要谨慎，不能简单认为一定安全。

如果 DMA 仍在运行时再次调用接收函数，可能返回：

```c
HAL_BUSY
```

或者造成状态异常。

因此 Circular 模式通常是：

```text
只启动一次
```

后续通过：

- `Size`
- DMA 当前计数器
- 上次读位置

来处理新增数据。

---

# 十一、FreeRTOS 推荐使用方式

在 FreeRTOS 中，推荐不要在回调中直接解析协议，而是：

```text
DMA接收
    ↓
HAL_UARTEx_RxEventCallback()
    ↓
保存Size/写指针
    ↓
通知UART任务
    ↓
UART任务读取数据
    ↓
协议解析
```

---

## 示例：DMA Normal 模式

定义：

```c
#define UART_RX_BUF_SIZE 256

static uint8_t uart_rx_buf[UART_RX_BUF_SIZE];

static volatile uint16_t uart_rx_size;
static TaskHandle_t uart_rx_task_handle;
```

任务：

```c
void UartRxTask(void *argument)
{
    uart_rx_task_handle = xTaskGetCurrentTaskHandle();

    HAL_UARTEx_ReceiveToIdle_DMA(
        &huart1,
        uart_rx_buf,
        UART_RX_BUF_SIZE
    );

    for (;;)
    {
        ulTaskNotifyTake(
            pdTRUE,
            portMAX_DELAY
        );

        uint16_t size = uart_rx_size;

        if (size > 0)
        {
            Protocol_Parse(uart_rx_buf, size);
        }

        /*
         * Normal模式下，接收结束后重新启动
         */
        HAL_UARTEx_ReceiveToIdle_DMA(
            &huart1,
            uart_rx_buf,
            UART_RX_BUF_SIZE
        );
    }
}
```

回调：

```c
void HAL_UARTEx_RxEventCallback(
    UART_HandleTypeDef *huart,
    uint16_t Size
)
{
    if (huart->Instance != USART1)
    {
        return;
    }

    uart_rx_size = Size;

    BaseType_t higher_priority_task_woken = pdFALSE;

    vTaskNotifyGiveFromISR(
        uart_rx_task_handle,
        &higher_priority_task_woken
    );

    portYIELD_FROM_ISR(higher_priority_task_woken);
}
```

---

# 十二、上面示例有一个并发风险

如果下一帧数据在任务处理上一帧之前到来：

```text
回调第一次：uart_rx_size = 10
任务还没有处理
回调第二次：uart_rx_size = 20
```

那么第一次的长度可能被覆盖。

而且如果复用了同一个：

```c
uart_rx_buf
```

DMA 可能覆盖任务还没有处理的数据。

因此这个简单示例只适合：

- 数据帧间隔较大
- 任务处理很快
- 不会连续来多帧

更稳妥的设计是：

```text
DMA Circular + 软件环形缓冲区
```

或者：

```text
DMA双缓冲 + FreeRTOS队列
```

---

# 十三、推荐的 FreeRTOS 数据传递方式

## 方式一：任务通知 + DMA缓冲区

适用于：

```text
DMA接收后，任务马上处理
```

回调中只保存：

```c
Size
```

任务收到通知后解析。

优点：

- 快
- 内存占用少
- 实现简单

缺点：

- 任务处理期间可能覆盖数据

---

## 方式二：队列传递数据副本

定义：

```c
typedef struct
{
    uint16_t length;
    uint8_t data[128];
} UartFrame_t;
```

回调中复制到空闲帧缓冲区，然后：

```c
xQueueSendFromISR(
    uart_queue,
    &frame,
    &higher_priority_task_woken
);
```

优点：

- 数据生命周期清晰
- 任务处理时不会被 DMA 覆盖

缺点：

- 中断中复制数据有开销
- 数据量大时不适合

---

## 方式三：Stream Buffer

适合原始字节流：

```text
DMA → 软件环形缓冲区 → Stream Buffer → 解析任务
```

---

## 方式四：Message Buffer

适合已经按 IDLE 分成一帧一帧的数据：

```text
一帧数据 → 一个消息
```

---

# 十四、回调是在什么上下文中执行？

对于 DMA/IT 接收回调，通常是在中断上下文中执行。

因此回调中不能调用普通 FreeRTOS API：

错误：

```c
xSemaphoreGive(mySemaphore);
xQueueSend(queue, &data, 0);
vTaskDelay(1);
```

应该使用 ISR 版本：

```c
xSemaphoreGiveFromISR();
xQueueSendFromISR();
vTaskNotifyGiveFromISR();
```

回调中也不要调用：

```c
HAL_Delay()
```

不要进行：

- 复杂协议解析
- 大量内存拷贝
- `printf`
- LVGL API
- 阻塞发送
- 动态内存申请

推荐只做：

```text
记录数据位置
设置标志
通知任务
```

---

# 十五、STM32H743 特别需要注意 Cache 和内存区域

H743 使用 Cortex-M7，DMA 接收还要考虑两个问题。

## 1. DMA 缓冲区必须放在 DMA 可访问内存

不要随意放在：

```text
ITCM
DTCM
```

很多 DMA 控制器不能访问 DTCM。

可以考虑：

```text
AXI SRAM
SRAM1
SRAM2
```

具体要根据 H743 的 DMA 和内存映射配置。

---

## 2. D-Cache 一致性

如果 D-Cache 已开启，可能出现：

```text
DMA已经写入RAM
CPU仍然读取Cache中的旧数据
```

建议方案：

### 方案 A：将 DMA 缓冲区配置为 Non-cacheable

这是比较推荐的系统级方案。

### 方案 B：手动失效 Cache

缓冲区需要：

```c
ALIGN_32BYTES(static uint8_t uart_rx_buf[256]);
```

读取 DMA 数据前执行：

```c
SCB_InvalidateDCache_by_Addr(
    (uint32_t *)uart_rx_buf,
    sizeof(uart_rx_buf)
);
```

但是地址和长度要按照 Cortex-M7 的 Cache line 对齐处理。

---

# 十六、`HAL_UARTEx_ReceiveToIdle_DMA()` 的一个重要细节：半传输回调

很多人以为：

```text
只要检测到IDLE才回调
```

实际上，DMA 半传输中断开启时，可能还会因为：

```text
DMA Half Transfer
```

触发：

```c
HAL_UARTEx_RxEventCallback()
```

例如：

```c
uint8_t rx_buf[128];
```

收到 64 字节时，可能触发半传输事件。

如果你只想在 IDLE 时处理，可以关闭 DMA HT 中断：

```c
__HAL_DMA_DISABLE_IT(huart1.hdmarx, DMA_IT_HT);
```

但在连续高速数据流中，HT 事件反而有价值，因为可以提前处理前半段数据，降低溢出风险。

---

# 十七、回调触发示例

假设：

```c
uint8_t rx_buf[256];
```

## 示例一：收到 10 字节后空闲

```text
DMA接收10字节
UART产生IDLE
回调：Size = 10
事件：HAL_UART_RXEVENT_IDLE
```

有效数据：

```c
rx_buf[0] ~ rx_buf[9]
```

---

## 示例二：连续收到 128 字节

```text
DMA达到半缓冲区
回调：Size = 128
事件：HAL_UART_RXEVENT_HT
```

---

## 示例三：收到 256 字节

```text
DMA达到缓冲区末尾
回调：Size = 256
事件：HAL_UART_RXEVENT_TC
```

Normal 模式下，DMA 通常停止，需要重新启动。

Circular 模式下，DMA 通常继续从缓冲区开头写入。

---

## 示例四：Circular 模式发生回绕

```text
第一次回调：Size = 200
第二次回调：Size = 30
```

第二次不是收到 30 字节这么简单，而是：

```text
DMA从200写到255
然后从0写到29
```

本次新增数据长度是：

```text
256 - 200 + 30 = 86
```

---

# 十八、推荐的实际选择

## 简单命令协议

例如：

```text
AT+RST\r\n
```

可以使用：

```c
HAL_UARTEx_ReceiveToIdle_DMA()
```

DMA Normal 模式即可。

---

## 高速、不定长、持续数据

推荐：

```text
DMA Circular
+
HAL_UARTEx_ReceiveToIdle_DMA()
+
软件环形缓冲区
+
FreeRTOS任务通知
```

---

## 非常低速、数据量很小

可以使用：

```c
HAL_UARTEx_ReceiveToIdle_IT()
```

---

## 学习和测试

可以使用：

```c
HAL_UARTEx_ReceiveToIdle()
```

但不要在 FreeRTOS 中使用：

```c
HAL_MAX_DELAY
```

让一个任务长期占用 CPU 轮询 UART。

---

# 十九、最终回答你的两个核心问题

## 问题一：什么时候触发回调？

`HAL_UARTEx_ReceiveToIdle_DMA()` 可能在以下事件发生时触发：

1. UART 检测到 IDLE
2. DMA 半传输 HT
3. DMA 传输完成 TC
4. 某些 HAL 版本或配置下发生错误事件

统一回调通常是：

```c
HAL_UARTEx_RxEventCallback()
```

可以使用：

```c
HAL_UARTEx_GetRxEventType()
```

判断具体事件类型。

---

## 问题二：每次调用会初始化 `pData` 吗？

不会。

它不会清零：

```c
pData
```

也不会自动执行：

```c
memset(pData, 0, Size);
```

它只是：

```text
配置 DMA 的内存地址
配置接收长度
启动 UART DMA 接收
```

如果重新调用：

```c
HAL_UARTEx_ReceiveToIdle_DMA(
    &huart1,
    uart_rx_buf,
    256
);
```

Normal 模式下，DMA 通常会从：

```c
uart_rx_buf[0]
```

重新写入，但旧数据不会被清除。

因此一定要根据回调参数 `Size` 使用有效数据：

```c
Protocol_Parse(uart_rx_buf, Size);
```

不能直接假设整个缓冲区都是本次数据，也不要依赖字符串结束符 `'\0'`。