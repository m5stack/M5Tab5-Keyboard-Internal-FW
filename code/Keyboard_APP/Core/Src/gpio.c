/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    gpio.c
 * @brief   This file provides code for the configuration
 *          of all used GPIO pins.
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

/* Includes ------------------------------------------------------------------*/
#include "gpio.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins
 */
void MX_GPIO_Init(void)
{
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* GPIO Ports Clock Enable */
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);

    /**/
    LL_GPIO_ResetOutputPin(ROW0_GPIO_Port, ROW0_Pin);

    /**/
    LL_GPIO_ResetOutputPin(ROW1_GPIO_Port, ROW1_Pin);

    /**/
    LL_GPIO_ResetOutputPin(ROW2_GPIO_Port, ROW2_Pin);

    /**/
    LL_GPIO_ResetOutputPin(ROW3_GPIO_Port, ROW3_Pin);

    /**/
    LL_GPIO_ResetOutputPin(ROW4_GPIO_Port, ROW4_Pin);

    /**/
    LL_GPIO_ResetOutputPin(ROW5_GPIO_Port, ROW5_Pin);

    /**/
    LL_GPIO_ResetOutputPin(ROW6_GPIO_Port, ROW6_Pin);

    /**/
    LL_GPIO_ResetOutputPin(ROW7_GPIO_Port, ROW7_Pin);

    /**/
    LL_GPIO_SetOutputPin(INT_GPIO_Port, INT_Pin);

    /**/
    GPIO_InitStruct.Pin  = COL0_Pin;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_DOWN;
    LL_GPIO_Init(COL0_GPIO_Port, &GPIO_InitStruct);

    /**/
    GPIO_InitStruct.Pin  = COL1_Pin;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_DOWN;
    LL_GPIO_Init(COL1_GPIO_Port, &GPIO_InitStruct);

    /**/
    GPIO_InitStruct.Pin  = COL2_Pin;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_DOWN;
    LL_GPIO_Init(COL2_GPIO_Port, &GPIO_InitStruct);

    /**/
    GPIO_InitStruct.Pin  = COL3_Pin;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_DOWN;
    LL_GPIO_Init(COL3_GPIO_Port, &GPIO_InitStruct);

    /**/
    GPIO_InitStruct.Pin  = COL4_Pin;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_DOWN;
    LL_GPIO_Init(COL4_GPIO_Port, &GPIO_InitStruct);

    /**/
    GPIO_InitStruct.Pin  = COL5_Pin;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_DOWN;
    LL_GPIO_Init(COL5_GPIO_Port, &GPIO_InitStruct);

    /**/
    GPIO_InitStruct.Pin  = COL6_Pin;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_DOWN;
    LL_GPIO_Init(COL6_GPIO_Port, &GPIO_InitStruct);

    /**/
    GPIO_InitStruct.Pin  = COL7_Pin;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_DOWN;
    LL_GPIO_Init(COL7_GPIO_Port, &GPIO_InitStruct);

    /**/
    GPIO_InitStruct.Pin        = ROW0_Pin;
    GPIO_InitStruct.Mode       = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed      = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull       = LL_GPIO_PULL_NO;
    LL_GPIO_Init(ROW0_GPIO_Port, &GPIO_InitStruct);

    /**/
    GPIO_InitStruct.Pin        = ROW1_Pin;
    GPIO_InitStruct.Mode       = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed      = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull       = LL_GPIO_PULL_NO;
    LL_GPIO_Init(ROW1_GPIO_Port, &GPIO_InitStruct);

    /**/
    GPIO_InitStruct.Pin        = ROW2_Pin;
    GPIO_InitStruct.Mode       = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed      = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull       = LL_GPIO_PULL_NO;
    LL_GPIO_Init(ROW2_GPIO_Port, &GPIO_InitStruct);

    /**/
    GPIO_InitStruct.Pin  = COL8_Pin;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_DOWN;
    LL_GPIO_Init(COL8_GPIO_Port, &GPIO_InitStruct);

    /**/
    GPIO_InitStruct.Pin  = COL9_Pin;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
    LL_GPIO_Init(COL9_GPIO_Port, &GPIO_InitStruct);

    /**/
    GPIO_InitStruct.Pin  = COL10_Pin;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
    LL_GPIO_Init(COL10_GPIO_Port, &GPIO_InitStruct);

    /**/
    GPIO_InitStruct.Pin        = INT_Pin;
    GPIO_InitStruct.Mode       = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed      = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull       = LL_GPIO_PULL_NO;
    LL_GPIO_Init(INT_GPIO_Port, &GPIO_InitStruct);

    /**/
    GPIO_InitStruct.Pin        = ROW3_Pin;
    GPIO_InitStruct.Mode       = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed      = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull       = LL_GPIO_PULL_NO;
    LL_GPIO_Init(ROW3_GPIO_Port, &GPIO_InitStruct);

    /**/
    GPIO_InitStruct.Pin        = ROW4_Pin;
    GPIO_InitStruct.Mode       = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed      = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull       = LL_GPIO_PULL_NO;
    LL_GPIO_Init(ROW4_GPIO_Port, &GPIO_InitStruct);

    /**/
    GPIO_InitStruct.Pin        = ROW5_Pin;
    GPIO_InitStruct.Mode       = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed      = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull       = LL_GPIO_PULL_NO;
    LL_GPIO_Init(ROW5_GPIO_Port, &GPIO_InitStruct);

    /**/
    GPIO_InitStruct.Pin        = ROW6_Pin;
    GPIO_InitStruct.Mode       = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed      = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull       = LL_GPIO_PULL_NO;
    LL_GPIO_Init(ROW6_GPIO_Port, &GPIO_InitStruct);

    /**/
    GPIO_InitStruct.Pin        = ROW7_Pin;
    GPIO_InitStruct.Mode       = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed      = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull       = LL_GPIO_PULL_NO;
    LL_GPIO_Init(ROW7_GPIO_Port, &GPIO_InitStruct);
}

/* USER CODE BEGIN 2 */

/* USER CODE END 2 */
