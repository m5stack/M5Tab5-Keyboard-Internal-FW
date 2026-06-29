/*
 * SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#ifndef __USER_KEYBOARD_HANDLE_H__
#define __USER_KEYBOARD_HANDLE_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* ----------- 全局变量声明 ----------- */
extern __IO uint8_t aa_flag;         // Case state flag / 大小写状态标志
extern __IO uint8_t aa_update_flag;  // Case update flag / 大小写更新标志
extern __IO uint8_t aa_blink_flag;   // Case bink flag / 大小写闪烁标志

/**
 * @brief Update and process HID keyboard events.
 *        更新并处理 HID 键盘事件。
 *
 * @param None
 * @retval None
 */
void key_hid_update(void);

/**
 * @brief Update and process Character keyboard events.
 *        更新并处理字符键盘事件。
 *
 * @param None
 * @retval None
 */
void key_char_update(void);

#ifdef __cplusplus
}
#endif

#endif /* __USER_KEYBOARD_HANDLE_H__ */
