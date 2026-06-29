/*
 * SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "user_int.h"

/**
 * @brief Enable interrupt signal by pulling INT pin low.
 *        通过拉低 INT 引脚使能中断信号。
 * @note  Writing to the BRR register resets the specified GPIO pin output.
 *        This operation is atomic and does not affect other GPIO pins.
 *        向 BRR 寄存器写入会将指定 GPIO 引脚输出为低电平。
 *        该操作是原子操作，不会影响其他 GPIO 引脚。
 *
 * @param None
 * @retval None
 */
void int_enable(void)
{
    INT_GPIO_Port->BRR = INT_Pin;  // Drive INT pin low / 将 INT 引脚拉低
}

/**
 * @brief Disable interrupt signal by pulling INT pin high.
 *        通过拉高 INT 引脚禁止中断信号。
 * @note  Writing to the BSRR register sets the specified GPIO pin output.
 *        This operation is atomic and does not affect other GPIO pins.
 *        向 BSRR 寄存器写入会将指定 GPIO 引脚输出为高电平。
 *        该操作是原子操作，不会影响其他 GPIO 引脚。
 *
 * @param None
 * @retval None
 */
void int_disable(void)
{
    INT_GPIO_Port->BSRR = INT_Pin;  // Drive INT pin high / 将 INT 引脚拉高
}