/*
 * SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#ifndef __USER_SYS_H__
#define __USER_SYS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "tim.h"
#include "stdbool.h"

/* ============================================================
 * System configuration macros
 * ============================================================*/

/**
 * @brief Application start address.
 *        应用程序起始地址。
 */
#define APPLICATION_ADDRESS (0x08001000)

/**
 * @brief Maximum RGB brightness value.
 *        RGB 最大亮度值。
 */
#define RGB_BRIGHTNESS_MAX (100)

/**
 * @brief Default RGB brightness value.
 *        RGB 默认亮度值。
 */
#define RGB_BRIGHTNESS_DEFAULT (20)

/**
 * @brief AA indicator light color (RGB888).
 *        AA 指示灯颜色（RGB888）。
 */
#define AA_INDICATOR_LIGHT (0x0000FF)

/**
 * @brief Keyboard mode indicator light color.
 *        键盘模式指示灯颜色。
 */
#define KEY_MODE_INDICATOR_LIGHT (0x0000FF)

/**
 * @brief HID mode indicator light color.
 *        HID 模式指示灯颜色。
 */
#define HID_MODE_INDICATOR_LIGHT (0x00FF00)

/**
 * @brief Character mode indicator light color.
 *        字符模式指示灯颜色。
 */
#define CHAR_MODE_INDICATOR_LIGHT (0xFF00FF)

/**
 * @brief Firmware version value.
 *        固件版本号。
 */
#define FIRMWARE_VERSION (0x01)

/**
 * @brief Minimum valid I2C address.
 *        I2C 地址最小值。
 */
#define I2C_ADDR_REG_MIN (0x08)

/**
 * @brief Maximum valid I2C address.
 *        I2C 地址最大值。
 */
#define I2C_ADDR_REG_MAX (0x77)

/**
 * @brief Default I2C slave address.
 *        I2C 默认从机地址。
 */
#define I2C_ADDR_REG_DEFAULT (0x6D)

/**
 * @brief Flash default erased value.
 *        Flash 默认擦除值。
 */
#define FLASH_DEFAULT_DATA (0xFF)

/**
 * @brief Invalid key value.
 *        无效按键值。
 */
#define KEY_NULL (0xFF)

/**
 * @brief Key debounce time in milliseconds.
 *        按键去抖时间（毫秒）。
 */
#define DEBOUNCE_MS (10)

/**
 * @brief Total number of keys.
 *        键盘按键总数。
 */
#define TOTAL_KEYS (70)

/**
 * @brief CPU cycles per microsecond.
 *        每微秒 CPU 周期数。
 */
#define CYCLES_PER_US (12U)

/**
 * @brief Sample count for debounce.
 *        去抖采样次数。
 */
#define SAMPLE_COUNT (3U)

/**
 * @brief Number of RGB LEDs.
 *        RGB 灯珠数量。
 */
#define RGB_NUM (2)

/**
 * @brief WS2812 logic '1' duty value.
 *        WS2812 逻辑 1 占空比值。
 */
#define BIT_1 (19)

/**
 * @brief WS2812 logic '0' duty value.
 *        WS2812 逻辑 0 占空比值。
 */
#define BIT_0 (10)

/**
 * @brief WS2812 reset slot count.
 *        WS2812 复位时隙数量。
 */
#define RESET_SLOTS (200)

/**
 * @brief Maximum character string length.
 *        最大字符长度。
 */
#define CHRE_MAX_SIZE (16)

/**
 * @brief Default FIFO depth.
 *        默认 FIFO 深度（可修改）。
 */
#define FIFO_DEPTH (32U)

/**
 * @brief FIFO mask for circular buffer operation.
 *        循环缓冲区操作的 FIFO 掩码。
 */
#define FIFO_MASK (FIFO_DEPTH - 1U)

/**
 * @brief Long press time for AA mode in milliseconds.
 *        AA 模式长按时间（毫秒）。
 */
#define AA_LONG_PRESS_MS (1000)

/**
 * @brief Double click time for AA mode in milliseconds.
 *        AA 模式双击时间（毫秒）。
 */
#define AA_DOUBLE_CLICK_MS (500U)

/* ============================================================
 * Enumerations
 * ============================================================*/

/**
 * @brief Keyboard work mode enumeration.
 *        键盘工作模式枚举。
 */
typedef enum {
    KEYBOARD_MODE_KEY  = 0, /**< Key scan mode / 按键模式 */
    KEYBOARD_MODE_HID  = 1, /**< HID mode / HID 模式 */
    KEYBOARD_MODE_CHAR = 2, /**< Character mode / 字符模式 */
} keyboard_work_mode_t;

/**
 * @brief RGB work mode enumeration.
 *        RGB 工作模式枚举。
 */
typedef enum {
    RGB_MODE_BIND = 0, /**< Bind mode / 绑定模式 */
    RGB_MODE_USER = 1, /**< User mode / 用户模式 */
} rgb_work_mode_t;

/**
 * @brief AA work mode enumeration.
 *        AA 工作模式枚举。
 */
typedef enum {
    AA_MODE_BIND      = 0, /**< Bind mode */
    AA_MODE_BIND_NONO = 1, /**< Bind-nono mode */
} aa_work_mode_t;

/* ============================================================
 * Keyboard event structures
 * ============================================================*/

/**
 * @brief Key event register format.
 *        按键事件寄存器格式。
 *
 * Bit7   : Key state (1 = pressed, 0 = released)
 * Bit6-4 : Row number
 * Bit3-0 : Column number
 */
typedef union {
    struct {
        uint8_t col : 4;     /*!< Column number / 列号 */
        uint8_t row : 3;     /*!< Row number / 行号 */
        uint8_t pressed : 1; /*!< Key state / 按键状态 */
    } bits;
    uint8_t value; /*!< Raw register value / 原始寄存器值 */
} key_event_t;

/**
 * @brief FIFO structure for key events.
 *        按键事件 FIFO 环形队列结构体。
 */
typedef struct {
    __IO uint8_t buf[FIFO_DEPTH]; /*!< Buffer / 缓冲区 */
    __IO uint8_t head;            /*!< Write index / 写索引 */
    __IO uint8_t tail;            /*!< Read index / 读索引 */
    __IO uint8_t count;           /*!< Element count / 当前元素数量 */
} event_fifo_t;

/**
 * @brief HID event structure.
 *        HID 事件结构体。
 */
typedef struct {
    uint8_t modifier; /*!< Modifier key mask / 修饰键 */
    uint8_t keycode;  /*!< HID key code / 键值 */
} hid_event_t;

/**
 * @brief FIFO structure for HID events.
 *        HID 事件 FIFO 环形队列结构体。
 */
typedef struct {
    __IO hid_event_t buf[FIFO_DEPTH];
    __IO uint8_t head;
    __IO uint8_t tail;
    __IO uint8_t count;
} hid_fifo_t;

/**
 * @brief Character event structure.
 *        字符事件结构体。
 */
typedef struct {
    uint8_t modifier;           /*!< Modifier key mask */
    uint8_t str[CHRE_MAX_SIZE]; /*!< Character string */
    uint8_t size;               /*!< String length */
} char_event_t;

/**
 * @brief FIFO structure for character events.
 *        字符事件 FIFO 环形队列结构体。
 */
typedef struct {
    __IO char_event_t buf[FIFO_DEPTH];
    __IO uint8_t head;
    __IO uint8_t tail;
    __IO uint8_t count;
} char_fifo_t;

/**
 * @brief Key debounce state structure.
 *        按键去抖状态结构体。
 */
typedef struct {
    uint8_t stable;            /*!< Stable state / 稳定状态 */
    uint8_t sample;            /*!< Last sample / 最近采样值 */
    uint32_t last_change_time; /*!< Last change timestamp / 上次变化时间 */
} key_state_t;

/**
 * @brief Raw key event structure.
 *        原始按键事件结构体。
 */
typedef struct {
    bool state; /*!< Key state */
    uint8_t row;
    uint8_t col;
} key_event_raw_t;

/**
 * @brief Processed key handling event structure.
 *        按键处理事件结构体。
 */
typedef struct {
    bool special_key;
    bool state;
    uint8_t modifier;
    uint8_t key_code;
    const char *key_name;
} key_handle_event_t;

/**
 * @brief Key mapping value structure.
 *        按键映射值结构体。
 */
typedef struct {
    const char *firstName;
    const uint8_t firstModifierMask;
    const uint8_t firstKeyCode;
    const char *secondName;
    const uint8_t secondModifierMask;
    const uint8_t secondKeyCode;
} key_value_t;

/* ============================================================
 * Keyboard interrupt registers
 * ============================================================*/

/**
 * @brief Keyboard interrupt configuration register.
 *        键盘中断配置寄存器。
 */
typedef union {
    struct {
        uint8_t key_event_enable : 1;  /*!< Key event interrupt enable */
        uint8_t hid_event_enable : 1;  /*!< HID event interrupt enable */
        uint8_t char_event_enable : 1; /*!< CHAR event interrupt enable */
        uint8_t reserved : 5;          /*!< Reserved */
    } bits;
    uint8_t value;
} keyboard_intr_config_t;

/**
 * @brief Keyboard interrupt status register.
 *        键盘中断状态寄存器。
 */
typedef union {
    struct {
        uint8_t key_event_flag : 1;  /*!< Key event flag */
        uint8_t hid_event_flag : 1;  /*!< HID event flag */
        uint8_t char_event_flag : 1; /*!< CHAR event flag */
        uint8_t reserved : 5;        /*!< Reserved */
    } bits;
    uint8_t value;
} keyboard_intr_status_t;

/**
 * @brief Update keyboard state.
 *        更新键盘状态。
 *
 * @retval None
 */
void user_keyboard_update(void);

#ifdef __cplusplus
}
#endif

#endif /* __USER_SYS_H__ */
