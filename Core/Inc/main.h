/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "cmsis_os2.h"
#include "FreeRTOS.h"
#include "semphr.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
extern osSemaphoreId_t ScreenFlushSemaphoreHandle;
extern osSemaphoreId_t uart_tx_fpga_semaphoreHandle;
extern TIM_HandleTypeDef htim2;
extern UART_HandleTypeDef huart4;
extern UART_HandleTypeDef huart2;
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define KB_R1_Pin GPIO_PIN_7
#define KB_R1_GPIO_Port GPIOD
#define KB_R2_Pin GPIO_PIN_9
#define KB_R2_GPIO_Port GPIOG
#define KB_R3_Pin GPIO_PIN_12
#define KB_R3_GPIO_Port GPIOG
#define KB_R4_Pin GPIO_PIN_13
#define KB_R4_GPIO_Port GPIOG
#define KB_R5_Pin GPIO_PIN_14
#define KB_R5_GPIO_Port GPIOG
#define KB_C5_Pin GPIO_PIN_3
#define KB_C5_GPIO_Port GPIOB
#define KB_C4_Pin GPIO_PIN_4
#define KB_C4_GPIO_Port GPIOB
#define KB_C3_Pin GPIO_PIN_5
#define KB_C3_GPIO_Port GPIOB
#define KB_C2_Pin GPIO_PIN_6
#define KB_C2_GPIO_Port GPIOB
#define KB_C1_Pin GPIO_PIN_8
#define KB_C1_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
