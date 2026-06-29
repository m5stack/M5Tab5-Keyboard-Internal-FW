/*
 * SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#ifndef __USER_I2C_CALLBACK_H__
#define __USER_I2C_CALLBACK_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "user_i2c_reg.h"
#include "user_i2c.h"
#include "user_sys.h"
#include "user_event_fifo.h"
#include "user_hid_fifo.h"
#include "user_keyboard_handle.h"

/* -------- External Variables / 外部变量声明 -------- */
extern __IO keyboard_intr_config_t keyboard_intr_config_reg;
extern __IO keyboard_intr_status_t keyboard_intr_status_reg;
extern __IO uint8_t rgb_brightness_reg;
extern __IO keyboard_work_mode_t keyboard_work_mode_reg;
extern __IO rgb_work_mode_t rgb_work_mode_reg;
extern __IO key_event_t key_event_reg;
extern __IO hid_event_t hid_event_reg;
extern __IO char_event_t char_event_reg;
extern __IO uint8_t rgb_buf_reg[8];
extern __IO uint8_t firmware_version_reg;
extern __IO uint8_t i2c_addr_reg;

/**
 * @brief Callback function executed when I2C slave transfer is complete.
 *        I2C 从机传输完成时执行的回调函数。
 *
 * @param rx_data Pointer to the received data buffer.
 *                指向接收数据缓冲区的指针。
 * @param len Length of the received data.
 *            接收数据的长度。
 * @retval None
 */
void Slave_Complete_Callback(uint8_t *rx_data, uint16_t len);

/**
 * @brief Handler for I2C timeout events to recover the bus.
 *        I2C 超时事件处理句柄，用于恢复总线。
 *
 * @param None
 * @retval None
 */
void i2c_timeout_handler(void);

#ifdef __cplusplus
}
#endif

#endif /*__USER_I2C_CALLBACK_H__ */