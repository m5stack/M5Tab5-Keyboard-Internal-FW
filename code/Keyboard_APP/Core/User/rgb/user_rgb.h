/*
 * SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#ifndef __USER_RGB_H__
#define __USER_RGB_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

/**
 * @brief RGB brightness update request flag.
 *        RGB 亮度更新请求标志。
 * @note  Set this flag to request an RGB brightness update.
 *        The actual update will be performed in rgb_update().
 *        设置该标志用于请求 RGB 亮度更新，
 *        实际更新将在 rgb_update() 中执行。
 */
extern __IO uint8_t rgb_brightness_update_flag;

/**
 * @brief RGB user buffer update request flag.
 *        RGB 用户缓冲区更新请求标志。
 * @note  Set this flag to request an RGB user buffer update.
 *        The actual update will be performed in rgb_update().
 *        设置该标志用于请求 RGB 用户缓冲区更新，
 *        实际更新将在 rgb_update() 中执行。
 */
extern __IO uint8_t rgb_user_buffer_update_flag;

/**
 * @brief RGB mode update request flag.
 *        RGB 模式更新请求标志。
 * @note  Set this flag to request an RGB mode update.
 *        The actual update will be performed in rgb_update().
 *        设置该标志用于请求 RGB 模式更新，
 *        实际更新将在 rgb_update() 中执行。
 */
extern __IO uint8_t rgb_mode_update_flag;

/**
 * @brief Keyboard work mode update request flag.
 *        键盘工作模式更新请求标志。
 * @note  Set this flag to request a keyboard work mode update.
 *        The actual update will be performed in rgb_update().
 *        设置该标志用于请求键盘工作模式更新，
 *        实际更新将在 rgb_update() 中执行。
 */
extern __IO uint8_t keyboard_work_mode_update_flag;

/**
 * @brief Handle RGB LED update process.
 *        处理 RGB 灯珠更新流程。
 * @note  This function should be called periodically in the main loop
 *        to process RGB update requests.
 *        该函数应在主循环中周期性调用，
 *        用于处理 RGB 更新请求。
 *
 * @param None
 * @retval None
 */
void rgb_update(void);

#ifdef __cplusplus
}
#endif

#endif /* __USER_RGB_H__ */
