/*
 * SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#ifndef __USER_INT_H__
#define __USER_INT_H__

#include "main.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Enable interrupt signal output.
 *        使能中断信号输出。
 * @note  This function enables the interrupt signal by driving the INT GPIO pin
 *        to an active state (typically low level), allowing the external device
 *        or system to detect interrupt events.
 *        该函数通过将 INT GPIO 引脚驱动到有效状态（通常为低电平）
 *        来使能中断信号，使外部器件或系统能够检测到中断事件。
 *
 * @param None
 * @retval None
 */
void int_enable(void);

/**
 * @brief Disable interrupt signal output.
 *        禁止中断信号输出。
 * @note  This function disables the interrupt signal by driving the INT GPIO pin
 *        to an inactive state (typically high level), preventing the external device
 *        or system from detecting interrupt events.
 *        该函数通过将 INT GPIO 引脚驱动到无效状态（通常为高电平）
 *        来禁止中断信号，防止外部器件或系统检测到中断事件。
 *
 * @param None
 * @retval None
 */
void int_disable(void);

#ifdef __cplusplus
}
#endif

#endif /* __USER_INT_H__ */