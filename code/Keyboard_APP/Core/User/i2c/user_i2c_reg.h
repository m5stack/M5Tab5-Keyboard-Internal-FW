/*
 * SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#ifndef __USER_I2C_REG_H__
#define __USER_I2C_REG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

/**
 * @brief System configuration register start address.
 *        系统配置寄存器起始地址。
 */
#define SYS_CONFIG_START_REG_ADDR (0x00)

/**
 * @brief System configuration register end address.
 *        系统配置寄存器结束地址。
 */
#define SYS_CONFIG_END_REG_ADDR (0x03)

/**
 * @brief Interrupt configuration register address.
 *        中断配置寄存器地址。
 */
#define INTR_CONFIG_REG_ADDR (0x00)

/**
 * @brief Interrupt status register address.
 *        中断状态寄存器地址。
 */
#define INTR_STATUS_REG_ADDR (0x01)

/**
 * @brief Event number register address.
 *        事件数量寄存器地址。
 */
#define EVENT_NUM_REG_ADDR (0x02)

/**
 * @brief RGB brightness register address.
 *        RGB 亮度寄存器地址。
 * @note  Used to control global RGB brightness.
 *        用于控制 RGB 全局亮度。
 */
#define RGB_BRIGHTNESS_REG_ADDR (0x03)

/**
 * @brief Mode configuration register start address.
 *        模式配置寄存器起始地址。
 */
#define MODE_CONFIG_START_REG_ADDR (0x10)

/**
 * @brief Mode configuration register end address.
 *        模式配置寄存器结束地址。
 */
#define MODE_CONFIG_END_REG_ADDR (0x11)

/**
 * @brief Keyboard work mode register address.
 *        键盘工作模式寄存器地址。
 */
#define KEYBOARD_MODE_REG_ADDR (0x00)

/**
 * @brief RGB work mode register address.
 *        RGB 工作模式寄存器地址。
 */
#define RGB_MODE_REG_ADDR (0x01)

/**
 * @brief Key event register address.
 *        按键事件寄存器地址。
 * @note  Used to report key events to the I2C master.
 *        用于向 I2C 主机上报按键事件。
 */
#define KEY_EVENT_REG_ADDR (0x20)

/**
 * @brief HID event register address.
 *        HID 事件寄存器地址。
 * @note  Used to report HID events.
 *        用于上报 HID 事件。
 */
#define HID_EVENT_REG_ADDR (0x30)

/**
 * @brief Character event length register address.
 *        字符事件长度寄存器地址。
 * @note  Used to report the length of character input events.
 *        用于上报字符输入事件的长度。
 */
#define CHAR_EVENT_LENGTH_REG_ADDR (0x40)

/**
 * @brief Character event register address.
 *        字符事件寄存器地址。
 * @note  Used to report character input events.
 *        用于上报字符输入事件。
 */
#define CHAR_EVENT_REG_ADDR (0x50)

/**
 * @brief RGB value register start address.
 *        RGB 数值寄存器起始地址。
 * @note  RGB values are stored in a continuous register range.
 *        RGB 数值存储在连续的寄存器地址范围内。
 */
#define RGB_VALUE_START_REG_ADDR (0x60)

/**
 * @brief RGB value register end address.
 *        RGB 数值寄存器结束地址。
 */
#define RGB_VALUE_END_REG_ADDR (0x67)

/**
 * @brief IAP update control register address.
 *        IAP 升级控制寄存器地址。
 * @note  Used to enter firmware update (IAP) mode.
 *        用于进入固件升级（IAP）模式。
 */
#define IAP_UPDATE_REG_ADDR (0xFD)

/**
 * @brief Firmware version register address.
 *        固件版本寄存器地址。
 * @note  This register is read-only.
 *        该寄存器为只读。
 */
#define FIRMWARE_VERSION_REG_ADDR (0xFE)

/**
 * @brief I2C slave address register address.
 *        I2C 从机地址寄存器地址。
 * @note  Used to configure the I2C slave address.
 *        用于配置 I2C 从机地址。
 */
#define I2C_ADDRESS_REG_ADDR (0xFF)

#ifdef __cplusplus
}
#endif

#endif /* __USER_I2C_REG_H__ */