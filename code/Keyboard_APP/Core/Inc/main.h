/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.h
 * @brief          : Header for main.c file.
 *                   This file contains the common defines of the application.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 STMicroelectronics.
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
#include "stm32f0xx_hal.h"

#include "stm32f0xx_ll_i2c.h"
#include "stm32f0xx_ll_iwdg.h"
#include "stm32f0xx_ll_system.h"
#include "stm32f0xx_ll_gpio.h"
#include "stm32f0xx_ll_exti.h"
#include "stm32f0xx_ll_bus.h"
#include "stm32f0xx_ll_cortex.h"
#include "stm32f0xx_ll_rcc.h"
#include "stm32f0xx_ll_utils.h"
#include "stm32f0xx_ll_pwr.h"
#include "stm32f0xx_ll_dma.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

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
#define COL0_Pin        LL_GPIO_PIN_0
#define COL0_GPIO_Port  GPIOA
#define COL1_Pin        LL_GPIO_PIN_1
#define COL1_GPIO_Port  GPIOA
#define COL2_Pin        LL_GPIO_PIN_2
#define COL2_GPIO_Port  GPIOA
#define COL3_Pin        LL_GPIO_PIN_3
#define COL3_GPIO_Port  GPIOA
#define COL4_Pin        LL_GPIO_PIN_4
#define COL4_GPIO_Port  GPIOA
#define COL5_Pin        LL_GPIO_PIN_5
#define COL5_GPIO_Port  GPIOA
#define COL6_Pin        LL_GPIO_PIN_6
#define COL6_GPIO_Port  GPIOA
#define COL7_Pin        LL_GPIO_PIN_7
#define COL7_GPIO_Port  GPIOA
#define ROW0_Pin        LL_GPIO_PIN_0
#define ROW0_GPIO_Port  GPIOB
#define ROW1_Pin        LL_GPIO_PIN_1
#define ROW1_GPIO_Port  GPIOB
#define ROW2_Pin        LL_GPIO_PIN_2
#define ROW2_GPIO_Port  GPIOB
#define COL8_Pin        LL_GPIO_PIN_8
#define COL8_GPIO_Port  GPIOA
#define COL9_Pin        LL_GPIO_PIN_9
#define COL9_GPIO_Port  GPIOA
#define COL10_Pin       LL_GPIO_PIN_10
#define COL10_GPIO_Port GPIOA
#define INT_Pin         LL_GPIO_PIN_15
#define INT_GPIO_Port   GPIOA
#define ROW3_Pin        LL_GPIO_PIN_3
#define ROW3_GPIO_Port  GPIOB
#define ROW4_Pin        LL_GPIO_PIN_4
#define ROW4_GPIO_Port  GPIOB
#define ROW5_Pin        LL_GPIO_PIN_5
#define ROW5_GPIO_Port  GPIOB
#define ROW6_Pin        LL_GPIO_PIN_6
#define ROW6_GPIO_Port  GPIOB
#define ROW7_Pin        LL_GPIO_PIN_7
#define ROW7_GPIO_Port  GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
