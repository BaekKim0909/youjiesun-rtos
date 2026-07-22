# FlashDB、FAL、SFUD 在 FreeRTOS 中的移植注意事项

## 1. 分析范围

本文基于以下两个工程进行分析：

- 当前工程：`D:\Code\YouJieSun`
- 参考工程：`D:\Work\youjiesun\YouJieSun-Rel\MDK-ARM`

当前工程的主控为 **STM32F429IGT6**，运行 FreeRTOS。参考工程中的存储软件栈为：

```text
FlashDB 2.1.1
    ↓
FAL
    ↓
SFUD
    ↓
SPI5
    ↓
外部 W25Q256 SPI NOR Flash（32MB）
```

参考工程使用的是外部 SPI NOR Flash，不是 STM32F429 的片内 Flash。因此，不存在擦写片内 Flash 时同 Bank 取指停顿的问题，但仍然必须处理多任务并发、SPI 总线互斥、长时间擦除和掉电一致性。

---

## 2. 总体结论

参考工程中的 FlashDB、FAL、SFUD 代码不能不加修改地复制到当前 FreeRTOS 工程。

至少需要完成以下处理：

1. 将 SFUD 中的全局关中断锁替换为 FreeRTOS Mutex。
2. 为 FlashDB 的 KVDB 和 TSDB 注册数据库级递归 Mutex。
3. 禁止从 ISR 中调用 FlashDB、FAL、SFUD 和阻塞式 SPI API。
4. 最好建立单独的 StorageTask，由其他任务通过队列提交存储请求。
5. 修复 SFUD 和 FAL 底层错误被忽略的问题。
6. 修复 TSDB rollover 参数传入 `NULL` 的问题。
7. 同时检查 KVDB 和 TSDB 的初始化结果。
8. 确认当前硬件是否具有 W25Q256，以及 SPI5、PF6 和分区是否可以直接使用。
9. 不要在 Flash 擦写期间长时间关闭中断或暂停调度器。
10. 将多任务互斥与掉电一致性分开处理。

---

## 3. 参考工程实际使用的存储硬件

参考工程中的 FAL 只注册了一个外部 Flash 设备：

```c
struct fal_flash_dev nor_flash0 =
{
    .name = FAL_USING_NOR_FLASH_DEV_NAME,
    .addr = 0,
    .len = 32 * 1024 * 1024,
    .blk_size = 4096,
    .ops = {init, read, write, erase},
    .write_gran = 1
};
```

源码位置：

```text
D:\Work\youjiesun\YouJieSun-Rel\MDK-ARM\Middlewares\FAL\porting\fal_flash_port.c:25
```

SFUD 端口实际使用：

```text
SPI5
PF6：W25Q256片选
W25Q256：32MB
最小擦除单位：4KB
页编程大小：256字节
```

源码位置：

```text
D:\Work\youjiesun\YouJieSun-Rel\MDK-ARM\Middlewares\SFUD\port\sfud_port.c:122
D:\Work\youjiesun\YouJieSun-Rel\MDK-ARM\Middlewares\SFUD\inc\sfud_flash_def.h:131
```

当前工程已经初始化了 SPI5 的 PF7、PF8、PF9，但尚未发现参考工程使用的 PF6 片选配置。因此，移植前必须确认：

- 当前硬件是否确实安装了 W25Q256；
- W25Q256 是否连接到 SPI5；
- 片选引脚是否确实是 PF6；
- WP 和 HOLD 引脚是否保持正确电平；
- SPI 模式、时钟极性、相位和最高频率是否符合硬件；
- 外部 Flash 中是否已经存有字库、图片、升级固件或其他数据。

---

## 4. 必须替换 SFUD 的全局关中断锁

### 4.1 当前实现

参考工程当前使用以下方式保护 SFUD 操作：

```c
static void spi_lock(const sfud_spi *spi)
{
    __disable_irq();
}

static void spi_unlock(const sfud_spi *spi)
{
    __enable_irq();
}
```

源码位置：

```text
D:\Work\youjiesun\YouJieSun-Rel\MDK-ARM\Middlewares\SFUD\port\sfud_port.c:68
```

这个实现不适合 FreeRTOS。

### 4.2 问题一：关中断时间可能非常长

SFUD 的锁覆盖完整的 Flash 操作，包括：

- 读取数据；
- 写使能；
- 发送页编程命令；
- 发送扇区擦除命令；
- 读取状态寄存器；
- 等待 Flash BUSY 位清零。

参考端口配置了最长约 60 秒的重试：

```c
flash->retry.times = 60 * 10000;
```

源码位置：

```text
D:\Work\youjiesun\YouJieSun-Rel\MDK-ARM\Middlewares\SFUD\port\sfud_port.c:135
```

如果整个过程都关闭中断，会导致：

- FreeRTOS Tick 停止；
- UART、DMA 等中断无法及时响应；
- 通信接收缓冲区溢出；
- 任务通知和队列处理严重延迟；
- LVGL 时基异常；
- 看门狗处理异常；
- 系统表现为卡死。

### 4.3 问题二：无条件开中断会破坏嵌套临界区

如果调用 SFUD 前，上层本来已经处于关中断状态：

```c
taskENTER_CRITICAL();

sfud_write(...);

/* SFUD内部spi_unlock已经无条件打开中断 */
taskEXIT_CRITICAL();
```

SFUD 不知道调用之前的中断状态，`__enable_irq()` 会提前打开中断，破坏上层临界区语义。

### 4.4 推荐修改

应为 SPI5/SFUD 建立 FreeRTOS Mutex：

```c
static StaticSemaphore_t spi5_mutex_buffer;
static SemaphoreHandle_t spi5_mutex;

void storage_lock_init(void)
{
    spi5_mutex = xSemaphoreCreateMutexStatic(&spi5_mutex_buffer);
    configASSERT(spi5_mutex != NULL);
}

static void spi_lock(const sfud_spi *spi)
{
    (void)spi;
    configASSERT(xSemaphoreTake(spi5_mutex, portMAX_DELAY) == pdTRUE);
}

static void spi_unlock(const sfud_spi *spi)
{
    (void)spi;
    configASSERT(xSemaphoreGive(spi5_mutex) == pdTRUE);
}
```

当前工程已经启用普通 Mutex：

```c
#define configUSE_MUTEXES 1
```

源码位置：

```text
D:\Code\YouJieSun\Core\Inc\FreeRTOSConfig.h:80
```

这里应使用 Mutex，而不是二值信号量，因为 Mutex 提供优先级继承，可以降低优先级反转风险。

替换后，在 Flash 操作期间：

- 其他访问同一 Flash 或 SPI5 的任务会阻塞；
- UART、DMA、FreeRTOS Tick 中断仍然可以正常响应；
- 高优先级任务仍然可以抢占 StorageTask；
- SPI Flash 事务不会被其他任务破坏。

---

## 5. SPI5 上的所有设备必须共用同一把锁

如果 SPI5 还连接其他设备，例如：

- 触摸控制器；
- 传感器；
- 另一颗 Flash；
- 使用 `HAL_SPI_*()` 直接操作的设备；

这些设备的驱动必须共用同一个 `spi5_mutex`。

否则可能发生：

```text
任务A：拉低W25Q256的CS并发送命令
    ↓ 被任务B抢占
任务B：使用SPI5访问另一设备
    ↓
任务A恢复并继续读取W25Q256
    ↓
SPI事务和数据均被破坏
```

HAL 内部的 `__HAL_LOCK(&hspi5)` 不能代替总线 Mutex。HAL 锁只保护单次 HAL API，不保护完整的：

```text
CS拉低 → 发送命令 → 发送/读取数据 → CS拉高
```

因此，SPI 总线 Mutex 的保护范围必须覆盖完整事务。

---

## 6. FlashDB 还需要数据库级锁

### 6.1 SFUD 锁不能替代 FlashDB 锁

SFUD Mutex 只能保护物理 Flash 和 SPI 总线，不能完整保护 FlashDB 内部状态，例如：

- KV 当前存储位置；
- 扇区状态；
- KV 和 sector cache；
- GC 状态；
- TSDB 当前写入位置；
- 数据库迭代状态。

参考工程当前直接初始化数据库，但没有设置 FlashDB 锁：

```c
result = fdb_kvdb_init(&kvdb, "Setting_Param_DB", "setting_param", NULL, NULL);
result2 = fdb_tsdb_init(&tsdb, "Records_DB", "test_records", fdb_tsdb_get_timestamp, 256, NULL);
```

源码位置：

```text
D:\Work\youjiesun\YouJieSun-Rel\MDK-ARM\Middlewares\FLASHDB\src\fdb_port.c:7
```

因此，多任务并发调用 FlashDB API 时并不安全。

### 6.2 推荐使用递归 Mutex

建议 KVDB 和 TSDB 共用同一个递归 Mutex：

```c
static StaticSemaphore_t flashdb_mutex_buffer;
static SemaphoreHandle_t flashdb_mutex;

static void flashdb_lock(fdb_db_t db)
{
    (void)db;
    configASSERT(
        xSemaphoreTakeRecursive(flashdb_mutex, portMAX_DELAY) == pdTRUE
    );
}

static void flashdb_unlock(fdb_db_t db)
{
    (void)db;
    configASSERT(
        xSemaphoreGiveRecursive(flashdb_mutex) == pdTRUE
    );
}
```

创建 Mutex：

```c
flashdb_mutex =
    xSemaphoreCreateRecursiveMutexStatic(&flashdb_mutex_buffer);

configASSERT(flashdb_mutex != NULL);
```

当前工程已启用递归 Mutex：

```c
#define configUSE_RECURSIVE_MUTEXES 1
```

源码位置：

```text
D:\Code\YouJieSun\Core\Inc\FreeRTOSConfig.h:83
```

### 6.3 注册锁回调

必须在数据库初始化前注册锁：

```c
fdb_kvdb_control(
    &kvdb,
    FDB_KVDB_CTRL_SET_LOCK,
    (void *)flashdb_lock
);

fdb_kvdb_control(
    &kvdb,
    FDB_KVDB_CTRL_SET_UNLOCK,
    (void *)flashdb_unlock
);

fdb_tsdb_control(
    &tsdb,
    FDB_TSDB_CTRL_SET_LOCK,
    (void *)flashdb_lock
);

fdb_tsdb_control(
    &tsdb,
    FDB_TSDB_CTRL_SET_UNLOCK,
    (void *)flashdb_unlock
);
```

对应控制命令位于：

```text
D:\Work\youjiesun\YouJieSun-Rel\MDK-ARM\Middlewares\FLASHDB\inc\fdb_def.h:88
D:\Work\youjiesun\YouJieSun-Rel\MDK-ARM\Middlewares\FLASHDB\inc\fdb_def.h:96
```

锁必须在 `fdb_kvdb_init()` 和 `fdb_tsdb_init()` 前注册，因为初始化过程本身可能读取、检查、恢复或格式化 Flash。

### 6.4 为什么使用递归 Mutex

普通 Mutex 在同一任务重复获取时会死锁。以下场景可能出现嵌套调用：

- TSDB 迭代回调中再次调用 FlashDB API；
- 应用层手动持有数据库锁后调用一个内部也会加锁的 API；
- 后续升级 FlashDB 后出现新的嵌套锁路径。

递归 Mutex 会记录同一任务的嵌套获取次数，更适合数据库级锁。

---

## 7. 推荐保留两层锁

推荐使用两把不同的锁：

```text
flashdb_mutex：递归Mutex
    └── 保护FlashDB内部状态和一次完整数据库操作

spi5_mutex：普通Mutex
    └── 保护SPI5总线和一次完整SFUD物理事务
```

固定锁顺序为：

```text
先获取FlashDB锁
    ↓
再获取SFUD/SPI5锁
```

不要让其他模块在持有 SPI5 锁时再调用 FlashDB，否则可能形成：

```text
任务A：持有FlashDB锁，等待SPI5锁
任务B：持有SPI5锁，等待FlashDB锁
```

最终导致死锁。

---

## 8. 推荐使用独立 StorageTask

即使已经配置 Mutex，仍建议建立一个单独的存储任务：

```text
UI任务 ─────────┐
通信任务 ───────┼──消息队列──→ StorageTask
其他业务任务 ──┘                  ↓
                                FlashDB
                                   ↓
                                  FAL
                                   ↓
                                  SFUD
                                   ↓
                              SPI5/W25Q256
```

例如：

- UI 任务提交“保存设置”请求；
- 通信任务提交“写入运行记录”请求；
- StorageTask 统一调用 `fdb_kv_set_blob()`、`fdb_tsl_append()`。

这样做的优点：

1. 不让高优先级通信任务直接阻塞在 Flash 擦除上；
2. 数据库操作天然串行化；
3. 容易控制写入频率；
4. 容易处理失败重试和错误统计；
5. 容易选择合适时机执行格式化和 GC；
6. 后续增加看门狗和掉电处理更简单。

即使采用单 StorageTask，仍建议保留 FlashDB 和 SPI 总线锁，防止其他模块绕开 StorageTask 直接访问。

---

## 9. ISR 中禁止访问数据库和 Flash

ISR 中不要调用：

```c
fdb_kv_set()
fdb_kv_set_blob()
fdb_tsl_append()
fal_partition_write()
sfud_write()
sfud_erase()
HAL_SPI_Transmit()
xSemaphoreTake()
```

原因包括：

- Mutex 不能从 ISR 获取；
- FlashDB、FAL 和 SFUD 不是 ISR 安全接口；
- 阻塞式 SPI HAL 不适合在 ISR 中执行；
- Flash 擦除可能持续数毫秒甚至更久；
- FlashDB 会修改数据库内部状态。

ISR 应只向 StorageTask 发送消息或通知：

```c
BaseType_t higher_priority_task_woken = pdFALSE;

xQueueSendFromISR(
    storage_queue,
    &request,
    &higher_priority_task_woken
);

portYIELD_FROM_ISR(higher_priority_task_woken);
```

实际 Flash 写入由 StorageTask 完成。

---

## 10. 不要使用长临界区保护 Flash 操作

不推荐以下写法：

```c
taskENTER_CRITICAL();
sfud_erase(...);
taskEXIT_CRITICAL();
```

也不推荐：

```c
vTaskSuspendAll();
sfud_erase(...);
xTaskResumeAll();
```

更不能使用：

```c
__disable_irq();
sfud_erase(...);
__enable_irq();
```

正确做法是：

```c
xSemaphoreTake(spi5_mutex, portMAX_DELAY);
sfud_erase(...);
xSemaphoreGive(spi5_mutex);
```

Mutex 解决的是“其他任务不能并发访问设备”，而不是“CPU不能被中断”。

---

## 11. SFUD 端口必须正确返回 SPI 错误

参考工程当前实现：

```c
static sfud_err spi_write_read(...)
{
    sfud_err result = SFUD_SUCCESS;
    HAL_StatusTypeDef state = HAL_OK;

    ...

    state = HAL_SPI_Transmit(...);

    if (state != HAL_OK)
    {
        goto __exit;
    }

    ...

__exit:
    HAL_GPIO_WritePin(..., GPIO_PIN_SET);
    return result;
}
```

源码位置：

```text
D:\Work\youjiesun\YouJieSun-Rel\MDK-ARM\Middlewares\SFUD\port\sfud_port.c:79
```

即使 `HAL_SPI_Transmit()` 或 `HAL_SPI_Receive()` 失败，`result` 仍然是 `SFUD_SUCCESS`。

这会形成：

```text
SPI传输失败
    ↓
SFUD认为成功
    ↓
FAL认为成功
    ↓
FlashDB继续提交状态
    ↓
数据库可能不一致
```

移植时必须把 HAL 错误转换为相应的 SFUD 错误。具体错误值应以该版本的 `sfud_err` 枚举为准，逻辑示例如下：

```c
if (HAL_SPI_Transmit(...) != HAL_OK)
{
    result = 对应的SFUD写错误;
    goto __exit;
}

if (HAL_SPI_Receive(...) != HAL_OK)
{
    result = 对应的SFUD读错误;
    goto __exit;
}
```

同时应避免以下无超时等待：

```c
while (HAL_SPI_GetState(spi_dev->spi_handle) != HAL_SPI_STATE_READY);
```

当前使用的是阻塞式 HAL SPI API，函数返回时一般已经结束，没有必要再无限等待；如果确实需要检查状态，也必须添加明确超时。

---

## 12. FAL 读取函数同样忽略了错误

当前 FAL 读取实现：

```c
static int read(long offset, uint8_t *buf, size_t size)
{
    assert(sfud_dev);
    assert(sfud_dev->init_ok);
    sfud_read(sfud_dev, nor_flash0.addr + offset, size, buf);

    return size;
}
```

源码位置：

```text
D:\Work\youjiesun\YouJieSun-Rel\MDK-ARM\Middlewares\FAL\porting\fal_flash_port.c:51
```

它忽略了 `sfud_read()` 的返回结果，应改为类似：

```c
if (sfud_read(sfud_dev, nor_flash0.addr + offset, size, buf) != SFUD_SUCCESS)
{
    return -1;
}

return (int)size;
```

FlashDB 的掉电恢复和状态提交依赖一个重要前提：

> 底层 read、write、erase 返回成功时，操作必须真实完成且数据有效。

如果底层失败后仍返回成功，FlashDB 自身的状态机无法保证数据库一致性。

---

## 13. TSDB rollover 参数存在空指针问题

参考工程当前调用：

```c
fdb_tsdb_control(
    &tsdb,
    FDB_TSDB_CTRL_SET_ROLLOVER,
    NULL
);
```

源码位置：

```text
D:\Work\youjiesun\YouJieSun-Rel\MDK-ARM\Middlewares\FLASHDB\src\fdb_port.c:14
```

但 FlashDB 实现会直接解引用参数：

```c
db->rollover = *(bool *)arg;
```

源码位置：

```text
D:\Work\youjiesun\YouJieSun-Rel\MDK-ARM\Middlewares\FLASHDB\src\fdb_tsdb.c:877
```

因此传入 `NULL` 会造成非法内存访问。正确方式：

```c
bool rollover = true;

fdb_tsdb_control(
    &tsdb,
    FDB_TSDB_CTRL_SET_ROLLOVER,
    &rollover
);
```

该控制命令必须在 TSDB 初始化成功后调用。

---

## 14. KVDB 和 TSDB 初始化结果都必须检查

参考代码声明：

```c
fdb_err_t result;
fdb_err_t result2;
```

但是只检查了 `result`，没有检查 `result2`。

应分别检查：

```c
if (kvdb_result != FDB_NO_ERR)
{
    return -1;
}

if (tsdb_result != FDB_NO_ERR)
{
    return -1;
}
```

否则 TSDB 初始化失败时，系统仍可能继续使用一个未正确初始化的数据库对象。

---

## 15. 推荐初始化顺序

建议只允许一个启动任务执行初始化，并按以下顺序进行：

```text
1. 初始化GPIO和SPI5
2. 配置W25Q256片选引脚
3. 创建spi5_mutex
4. 创建flashdb_mutex
5. 初始化SFUD
6. 初始化FAL
7. 为KVDB和TSDB注册锁回调
8. 初始化KVDB
9. 初始化TSDB
10. 在TSDB初始化成功后设置rollover
11. 发布“存储系统就绪”状态
12. 放行业务任务访问数据库
```

不要让多个任务同时初始化数据库，也不要在数据库初始化完成之前访问数据库。

可以使用以下方式控制启动顺序：

- 在创建业务任务前完成初始化；
- 使用 EventGroup 的就绪位；
- 使用任务通知；
- 使用只读初始化完成标志，并保证内存可见性。

---

## 16. Flash 长耗时操作的处理

参考端口使用忙等待实现约 100 微秒延时：

```c
static void retry_delay_100us(void)
{
    uint32_t delay = 120;
    while (delay--);
}
```

该实现不精确，并且持续占用 CPU。

建议：

- 页编程的短等待可以采用短时间轮询；
- 扇区擦除等待过程中应定期让出 CPU；
- StorageTask 的优先级不要高于通信等实时任务；
- 如果使用 `vTaskDelay()` 替换延时，必须同步重新计算 `retry.times`；
- 不要简单地把 100 微秒延时改为 1 Tick，却保留原来的 60 万次重试，否则总超时会被大幅放大；
- 使用看门狗时，应保证长时间擦除过程中系统仍能正常执行看门狗管理任务；
- 不建议在驱动深层随意喂狗，最好由系统级看门狗管理模块判断 StorageTask 是否仍有进展。

需要特别注意：如果 SFUD 在等待 BUSY 时释放 `spi5_mutex`，其他任务可能在 Flash 仍忙时访问同一芯片。因此，更稳妥的方式是保持 Flash 设备锁，仅通过调度让出 CPU，而不是允许其他任务访问同一颗 Flash。

---

## 17. FAL 分区必须按物理擦除边界对齐

参考工程分区表：

```c
#define FAL_PART_TABLE                                                           \
{                                                                                \
    {FAL_PART_MAGIC_WORD, "setting_param", NOR_FLASH_DEV_NAME,                  \
        0, 2 * 1024 * 1024, 0},                                                  \
    {FAL_PART_MAGIC_WORD, "test_records", NOR_FLASH_DEV_NAME,                   \
        2 * 1024 * 1024, 30 * 1024 * 1024, 0},                                  \
}
```

源码位置：

```text
D:\Work\youjiesun\YouJieSun-Rel\MDK-ARM\Middlewares\FAL\porting\fal_cfg.h:28
```

对应布局：

```text
0～2MB：setting_param
2～32MB：test_records
```

W25Q256 最小擦除单位为 4KB，因此：

- 分区起始地址必须按 4KB 对齐；
- 分区长度必须按 4KB 对齐；
- 两个分区不能重叠；
- 分区总长度不能超过实际芯片容量；
- 必须确认这些区域没有存放图片、字库、OTA 固件或其他数据。

参考分区占用了整颗 32MB W25Q256。如果当前产品使用同一颗 Flash 存放其他资源，不能直接复用该分区表。

---

## 18. Mutex 原子性与掉电原子性不是一回事

### 18.1 多任务并发原子性

由 Mutex 保证：

```text
任务A修改数据库期间，任务B不能同时修改数据库或访问同一Flash事务
```

这解决的是软件并发问题。

### 18.2 掉电原子性

Mutex 无法解决掉电问题。

如果页编程或扇区擦除过程中掉电：

- 当前页可能只写入了一部分；
- 当前扇区内容可能处于不确定状态；
- 多个 KV 更新不会自动构成一个事务；
- TSDB 一条记录的状态和数据可能未全部提交。

FlashDB 已通过以下机制提高掉电恢复能力：

- KV 状态从预写入状态转换为有效状态；
- KV 数据使用 CRC32；
- 删除和 GC 使用中间状态；
- TSDB 使用预写入和最终提交状态；
- 启动时扫描并恢复数据库状态。

但应用仍需处理：

1. 配置合理的 BOR 掉电复位等级；
2. 硬件电源提供足够的编程完成余量；
3. 检查所有 FlashDB、FAL、SFUD 返回值；
4. 保证底层返回成功时操作已经真实完成；
5. 关键配置增加应用层版本号和 CRC；
6. 多键一致更新使用双副本或提交标志；
7. 不要把多个 `fdb_kv_set()` 当作一个原子事务；
8. 掉电告警发生后不要启动新的擦除操作。

---

## 19. 建议的数据更新方式

对于需要多字段保持一致的配置，不建议拆成多个独立 KV：

```c
fdb_kv_set("param_a", ...);
fdb_kv_set("param_b", ...);
fdb_kv_set("param_c", ...);
```

如果第二次写入后掉电，三个键可能属于不同版本。

更建议把相关配置组成一个结构体，并增加版本和 CRC：

```c
typedef struct
{
    uint32_t version;
    DeviceConfig config;
    uint32_t crc32;
} PersistentConfig;
```

然后通过一个 blob 整体保存：

```c
fdb_kv_set_blob(...);
```

如果数据极其关键，可使用：

```text
config_a
config_b
active_config_index
```

先写入非活动副本，校验成功后再切换活动索引。

---

## 20. 推荐的锁和任务关系

```text
应用任务
    │
    │ StorageRequest
    ▼
StorageTask
    │
    │ 获取flashdb_mutex（递归Mutex）
    ▼
FlashDB
    │
    ▼
FAL
    │
    ▼
SFUD
    │
    │ 获取spi5_mutex（普通Mutex）
    ▼
SPI5事务
    │
    ▼
W25Q256
```

中断只向队列发送请求，不直接进入上述调用链。

---

## 21. 移植检查清单

### 硬件

- [ ] 确认外部 Flash 型号和实际容量；
- [ ] 确认 SPI 外设编号；
- [ ] 确认 CS 引脚；
- [ ] 确认 WP/HOLD 电平；
- [ ] 确认 SPI 模式和最高频率；
- [ ] 确认外部 Flash 是否已有其他资源。

### SFUD

- [ ] 将全局关中断锁替换为 FreeRTOS Mutex；
- [ ] 所有 SPI5 设备共用总线 Mutex；
- [ ] 正确转换 HAL SPI 错误；
- [ ] 删除或限制无超时状态等待；
- [ ] 调整 BUSY 轮询和超时；
- [ ] 禁止 ISR 调用。

### FAL

- [ ] 检查设备容量；
- [ ] 检查擦除粒度；
- [ ] 检查写粒度；
- [ ] 分区起始地址和长度按 4KB 对齐；
- [ ] 检查所有 read/write/erase 返回值；
- [ ] 确认分区没有覆盖其他数据。

### FlashDB

- [ ] KVDB 和 TSDB 注册同一个递归 Mutex；
- [ ] 锁回调在数据库初始化前注册；
- [ ] 初始化只由一个任务执行；
- [ ] 同时检查 KVDB 和 TSDB 初始化结果；
- [ ] rollover 传入有效 `bool *`；
- [ ] 迭代器使用期间避免并发修改；
- [ ] 关键数据增加应用层版本和 CRC；
- [ ] 不把多个 KV 更新视为事务。

### FreeRTOS

- [ ] 建立 StorageTask；
- [ ] 其他任务通过队列提交请求；
- [ ] StorageTask 优先级低于实时通信任务；
- [ ] ISR 只使用 `FromISR` API 发通知；
- [ ] 不在 Flash 操作期间关闭中断；
- [ ] 不在 Flash 操作期间暂停调度器；
- [ ] 设计长时间擦除期间的看门狗策略。

---

## 22. 最终建议

针对当前 STM32F429 + FreeRTOS 项目，推荐采用以下方案：

1. 使用外部 W25Q256，不将 FlashDB 放入 STM32F429 片内程序 Flash；
2. 使用独立 StorageTask 串行处理所有数据库请求；
3. FlashDB 层使用一把共享递归 Mutex；
4. SFUD/SPI5 层使用一把普通 Mutex；
5. 所有 SPI5 设备共用同一个总线 Mutex；
6. Flash 擦除和 BUSY 等待期间保持中断及任务调度正常；
7. 修复 SFUD/FAL 的错误返回链；
8. 检查外部 Flash 分区，禁止与图片、字库、OTA 区域重叠；
9. 使用 FlashDB 自身的恢复能力，并为关键数据补充应用层版本、CRC 或双副本；
10. 集成前评估是否升级 FlashDB 版本，但不要未经兼容性验证直接替换参考工程的 2.1.1。

---

## 23. 参考资料

- [FlashDB 官方仓库](https://github.com/armink/FlashDB)
- [FlashDB 官方文档](https://armink.github.io/FlashDB/)
- [FlashDB 线程安全讨论](https://github.com/armink/FlashDB/discussions/194)
- [FlashDB 递归锁修改](https://github.com/armink/FlashDB/pull/339/files)
- [FlashDB Releases](https://github.com/armink/FlashDB/releases)
- [FreeRTOS 递归互斥量文档](https://www.freertos.org/Documentation/02-Kernel/02-Kernel-features/02-Queues-mutexes-and-semaphores/05-Recursive-mutexes)