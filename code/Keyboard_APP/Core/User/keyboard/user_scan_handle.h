/*
 * SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#ifndef __USER_SCAN_HANDLE_H__
#define __USER_SCAN_HANDLE_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/**
 * @brief Initialize keyboard scanning module.
 *        初始化键盘扫描模块。
 * @note  This function initializes internal key state buffers and
 *        sets all row GPIO pins to a known inactive state.
 *        该函数初始化按键状态缓存，
 *        并将所有行 GPIO 引脚设置为已知的非激活状态。
 *
 * @param None
 * @retval None
 */
void keyboard_init(void);

/**
 * @brief Update keyboard state and generate key events.
 *        更新键盘状态并生成按键事件。
 * @note  This function performs key matrix scanning, debouncing,
 *        and event generation. It should be called periodically.
 *        该函数执行键盘矩阵扫描、消抖处理以及事件生成，
 *        应周期性地被调用。
 *
 * @param None
 * @retval None
 */
void keyboard_update(void);

#ifdef __cplusplus
}
#endif

#endif /* __USER_SCAN_HANDLE_H__ */