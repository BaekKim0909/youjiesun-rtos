/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "timers.h"
#include "fpga_comm.h"
#include "test_task.h"
#include "ui_task.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define TEST_TASK_STACK_SIZE (256)
#define TEST_TASK_PRIORITY (osPriorityNormal4)

#define COMMUNICATE_TASK_STACK_SIZE (256)
#define COMMUNICATE_TASK_PRIORITY (osPriorityAboveNormal)

#define TEST_EVENT_QUEUE_LENGTH     (10U)
#define FPGA_REQUEST_QUEUE_LENGTH   (16U)
#define UI_EVENT_QUEUE_LENGTH       (1U)
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
/*
 * 任务句柄
 */
TaskHandle_t communicate_taskHandle = NULL;
TaskHandle_t test_taskHandle = NULL;

TimerHandle_t read_temperature_timer = NULL;


/*
 * 信号量句柄句柄
 */
/* SPI5发送完成信号量 */
SemaphoreHandle_t spi5_tx_semaphore = NULL;
/* SPI5接收完成信号量 */
SemaphoreHandle_t spi5_rx_semaphore = NULL;
/* SPI5发送并接收信号量 */
SemaphoreHandle_t spi5_tx_rx_semaphore = NULL;

/* ----------------------------队列句柄------------------------------------ */
/* FPGA 串口读取指令 发送队列 */
QueueHandle_t test_event_queue = NULL;
QueueHandle_t fpga_request_queue = NULL;
QueueHandle_t ui_event_queue = NULL;
/* USER CODE END Variables */
/* Definitions for UITask */
osThreadId_t UITaskHandle;
const osThreadAttr_t UITask_attributes = {
    .name = "UITask",
    .stack_size = 2048 * 4,
    .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for KeyScanTask */
osThreadId_t KeyScanTaskHandle;
const osThreadAttr_t KeyScanTask_attributes = {
    .name = "KeyScanTask",
    .stack_size = 128 * 4,
    .priority = (osPriority_t) osPriorityNormal1,
};
/* Definitions for ScreenFlushSemaphore */
osSemaphoreId_t ScreenFlushSemaphoreHandle;
const osSemaphoreAttr_t ScreenFlushSemaphore_attributes = {
    .name = "ScreenFlushSemaphore"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void start_test_task(void *argument);

void read_fpga_temperature_timer_cb(TimerHandle_t xTimer);

void start_communicate_task(void *argument);

/* USER CODE END FunctionPrototypes */

void StartUITask(void *argument);

void StartKeyScanTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* Hook prototypes */
void configureTimerForRunTimeStats(void);

unsigned long getRunTimeCounterValue(void);

void vApplicationTickHook(void);

void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName);

void vApplicationMallocFailedHook(void);

/* USER CODE BEGIN 1 */
/* Functions needed when configGENERATE_RUN_TIME_STATS is on */
__weak void configureTimerForRunTimeStats(void)
{
}

__weak unsigned long getRunTimeCounterValue(void)
{
    return 0;
}

/* USER CODE END 1 */

/* USER CODE BEGIN 3 */
void vApplicationTickHook(void)
{
    /* This function will be called by each tick interrupt if
    configUSE_TICK_HOOK is set to 1 in FreeRTOSConfig.h. User code can be
    added here, but the tick hook is called from an interrupt context, so
    code must not attempt to block, and only the interrupt safe FreeRTOS API
    functions can be used (those that end in FromISR()). */
}

/* USER CODE END 3 */

/* USER CODE BEGIN 4 */
void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName)
{
    /* Run time stack overflow checking is performed if
    configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2. This hook function is
    called if a stack overflow is detected. */
}

/* USER CODE END 4 */

/* USER CODE BEGIN 5 */
void vApplicationMallocFailedHook(void)
{
    /* vApplicationMallocFailedHook() will only be called if
    configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h. It is a hook
    function that will get called if a call to pvPortMalloc() fails.
    pvPortMalloc() is called internally by the kernel whenever a task, queue,
    timer or semaphore is created. It is also called by various parts of the
    demo application. If heap_1.c or heap_2.c are used, then the size of the
    heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
    FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
    to query the size of free heap space that remains (although it does not
    provide information on how the remaining heap might be fragmented). */
}

/* USER CODE END 5 */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void)
{
    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* USER CODE BEGIN RTOS_MUTEX */
    /* add mutexes, ... */
    /* USER CODE END RTOS_MUTEX */

    /* Create the semaphores(s) */
    /* creation of ScreenFlushSemaphore */
    ScreenFlushSemaphoreHandle = osSemaphoreNew(1, 0, &ScreenFlushSemaphore_attributes);

    /* USER CODE BEGIN RTOS_SEMAPHORES */
    /* add semaphores, ... */
    spi5_tx_semaphore = xSemaphoreCreateBinary();
    if (spi5_tx_semaphore != NULL)
    {
        vQueueAddToRegistry(spi5_tx_semaphore, "SPI5_TX_Semaphore");
    }
    spi5_rx_semaphore = xSemaphoreCreateBinary();
    spi5_tx_rx_semaphore = xSemaphoreCreateBinary();
    /* USER CODE END RTOS_SEMAPHORES */

    /* USER CODE BEGIN RTOS_TIMERS */
    /* start timers, add new ones, ... */
    read_temperature_timer = xTimerCreate("read_fpga_temperature_timer", pdMS_TO_TICKS(1000U),pdTRUE, NULL,
                                          read_fpga_temperature_timer_cb);
    /* USER CODE END RTOS_TIMERS */

    /* USER CODE BEGIN RTOS_QUEUES */
    /* add queues, ... */
    test_event_queue = xQueueCreate(TEST_EVENT_QUEUE_LENGTH, sizeof(test_event_t));
    if (test_event_queue != NULL)
    {
        vQueueAddToRegistry(test_event_queue, "TEST_EVENT_QUEUE");
    }
    fpga_request_queue = xQueueCreate(FPGA_REQUEST_QUEUE_LENGTH, sizeof(fpga_request_t));
    if (fpga_request_queue != NULL)
    {
        vQueueAddToRegistry(fpga_request_queue, "FPGA_REQUEST_QUEUE");
    }
    ui_event_queue = xQueueCreate(UI_EVENT_QUEUE_LENGTH, sizeof(ui_event_t));
    if (ui_event_queue != NULL)
    {
        vQueueAddToRegistry(ui_event_queue, "UI_EVENT_QUEUE");
    }
    /* USER CODE END RTOS_QUEUES */

    /* Create the thread(s) */
    /* creation of UITask */
    UITaskHandle = osThreadNew(StartUITask, NULL, &UITask_attributes);

    /* creation of KeyScanTask */
    KeyScanTaskHandle = osThreadNew(StartKeyScanTask, NULL, &KeyScanTask_attributes);

    /* USER CODE BEGIN RTOS_THREADS */
    /* add threads, ... */
    xTaskCreate(start_communicate_task, "CommunicateTask",COMMUNICATE_TASK_STACK_SIZE, NULL, COMMUNICATE_TASK_PRIORITY,
                &communicate_taskHandle);
    xTaskCreate(start_test_task, "TestTask", TEST_TASK_STACK_SIZE, NULL, TEST_TASK_PRIORITY, &test_taskHandle);
    /* USER CODE END RTOS_THREADS */

    /* USER CODE BEGIN RTOS_EVENTS */
    /* add events, ... */
    /* USER CODE END RTOS_EVENTS */
}

/* USER CODE BEGIN Header_StartUITask */
/**
  * @brief  Function implementing the UITask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartUITask */
__weak void StartUITask(void *argument)
{
    /* USER CODE BEGIN StartUITask */
    /* Infinite loop */
    for (;;)
    {
        osDelay(1);
    }
    /* USER CODE END StartUITask */
}

/* USER CODE BEGIN Header_StartKeyScanTask */
/**
* @brief Function implementing the KeyScanTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartKeyScanTask */
__weak void StartKeyScanTask(void *argument)
{
    /* USER CODE BEGIN StartKeyScanTask */
    /* Infinite loop */
    for (;;)
    {
        osDelay(1);
    }
    /* USER CODE END StartKeyScanTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
__weak void start_test_task(void *argument)
{
    for (;;)
    {
        vTaskDelay(1);
    }
}

__weak void read_fpga_temperature_timer_cb(TimerHandle_t xTimer)
{
}

__weak void start_communicate_task(void *argument)
{
    for (;;)
    {
        vTaskDelay(1);
    }
}

/* USER CODE END Application */
