/*
 * SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "user_i2c_callback.h"
#include "string.h"
#include "user_rgb.h"
#include "user_int.h"
#include "user_event_fifo.h"
#include "user_hid_fifo.h"
#include "user_char_fifo.h"
#include "user_flash.h"

/* ----------- 全局寄存器变量定义 ----------- */
__IO keyboard_intr_config_t keyboard_intr_config_reg;  // Interrupt config / 中断配置
__IO keyboard_intr_status_t keyboard_intr_status_reg;  // Interrupt status / 中断状态
__IO uint8_t rgb_brightness_reg                  = 0;  // RGB brightness / RGB 亮度
__IO keyboard_work_mode_t keyboard_work_mode_reg = 0;  // Keyboard mode / 键盘工作模式
__IO rgb_work_mode_t rgb_work_mode_reg           = 0;  // RGB mode / RGB 工作模式
__IO key_event_t key_event_reg;                        // Key event data / 按键事件数据
__IO hid_event_t hid_event_reg;                        // HID event data / HID 事件数据
__IO char_event_t char_event_reg;                      // Char event data / 字符事件数据
__IO uint8_t rgb_buf_reg[8]       = {0};               // RGB data buffer / RGB 数据缓冲区
__IO uint8_t firmware_version_reg = 0;                 // Firmware version / 固件版本
__IO uint8_t i2c_addr_reg         = 0;                 // I2C slave address / I2C 从机地址

static __IO uint32_t i2c_stop_timeout_delay = 0;  // Timeout delay timer / 超时延迟计时器

/**
 * @brief Helper function to parse I2C write data into a temporary buffer.
 *        辅助函数：将 I2C 写入数据解析到临时缓冲区。
 * @note  Handles non-contiguous writes logic by marking updated bytes.
 *        通过标记更新的字节来处理非连续写入逻辑。
 *
 * @param rx_buf Temporary data buffer. / 临时数据缓冲区。
 * @param rx_mark Mask buffer to indicate updated bytes. / 掩码缓冲区，指示更新的字节。
 * @param rx_data Raw I2C received data (Address + Payload). / 原始 I2C 数据。
 * @param base_addr Base address of the register block. / 寄存器块基地址。
 * @param len Total length of received data. / 接收数据的总长度。
 */
static void update_register_buffer(uint8_t *rx_buf, uint8_t *rx_mark, uint8_t *rx_data, uint8_t base_addr, uint16_t len)
{
    for (uint8_t i = 0; i < len - 1; i++) {
        uint8_t rx_index  = rx_data[0] - base_addr + i;
        rx_buf[rx_index]  = rx_data[1 + i];
        rx_mark[rx_index] = 1;  // Mark this byte as updated / 标记此字节已更新
    }
}

/**
 * @brief Callback function executed when I2C slave transfer is complete.
 *        I2C 从机传输完成时执行的回调函数。
 * @note  Manages register read/write logic based on the received register address.
 *        根据接收到的寄存器地址管理寄存器读/写逻辑。
 *
 * @param rx_data Pointer to the received data buffer.
 * @param len Length of the received data.
 */
void Slave_Complete_Callback(uint8_t *rx_data, uint16_t len)
{
    uint8_t rx_buf[64];
    uint8_t tx_buf[64];
    uint8_t rx_mark[64] = {0};

    if (len == 1) {  // Read Request Logic / 读请求逻辑
        if (rx_data[0] >= SYS_CONFIG_START_REG_ADDR && rx_data[0] <= SYS_CONFIG_END_REG_ADDR) {
            tx_buf[0] = keyboard_intr_config_reg.value;
            tx_buf[1] = keyboard_intr_status_reg.value;
            // Get FIFO count based on current mode / 根据当前模式获取 FIFO 计数
            if (keyboard_work_mode_reg == KEYBOARD_MODE_KEY) {
                tx_buf[2] = event_fifo_count();
            } else if (keyboard_work_mode_reg == KEYBOARD_MODE_HID) {
                tx_buf[2] = hid_fifo_count();
            } else if (keyboard_work_mode_reg == KEYBOARD_MODE_CHAR) {
                tx_buf[2] = char_fifo_count();
            }
            tx_buf[3] = rgb_brightness_reg;
            i2c2_set_send_data((uint8_t *)&tx_buf[rx_data[0] - SYS_CONFIG_START_REG_ADDR],
                               SYS_CONFIG_END_REG_ADDR - rx_data[0] + 1);
        } else if (rx_data[0] >= MODE_CONFIG_START_REG_ADDR && rx_data[0] <= MODE_CONFIG_END_REG_ADDR) {
            tx_buf[0] = keyboard_work_mode_reg;
            tx_buf[1] = rgb_work_mode_reg;
            i2c2_set_send_data((uint8_t *)&tx_buf[rx_data[0] - MODE_CONFIG_START_REG_ADDR],
                               MODE_CONFIG_END_REG_ADDR - rx_data[0] + 1);
        } else if (rx_data[0] == KEY_EVENT_REG_ADDR) {
            if (!event_fifo_is_empty()) {
                event_fifo_pop(&key_event_reg.value);
            } else {
                key_event_reg.value = 0xFF;  // Return 0xFF if empty / 为空则返回 0xFF
            }
            tx_buf[0] = key_event_reg.value;
            i2c2_set_send_data(tx_buf, 1);
        } else if (rx_data[0] == HID_EVENT_REG_ADDR) {
            if (!hid_fifo_is_empty()) {
                hid_fifo_pop(&hid_event_reg);
            } else {
                hid_event_reg.modifier = 0xFF;
                hid_event_reg.keycode  = 0xFF;
            }
            tx_buf[0] = hid_event_reg.modifier;
            tx_buf[1] = hid_event_reg.keycode;
            i2c2_set_send_data(tx_buf, 2);
        } else if (rx_data[0] == CHAR_EVENT_LENGTH_REG_ADDR) {
            if (!char_fifo_is_empty()) {
                char_fifo_peek(&char_event_reg);
                tx_buf[0] = char_event_reg.size + 1;  // Include modifier / 包含修饰键
            } else {
                tx_buf[0] = 0x00;
            }
            i2c2_set_send_data(tx_buf, 1);
        } else if (rx_data[0] == CHAR_EVENT_REG_ADDR) {
            if (!char_fifo_is_empty()) {
                char_fifo_pop(&char_event_reg);
            } else {
                char_event_reg.size = 0x00;
            }
            tx_buf[0] = char_event_reg.modifier;
            if (char_event_reg.size > 0) {
                memcpy(&tx_buf[1], (uint8_t *)&char_event_reg.str[0], char_event_reg.size);
            }
            i2c2_set_send_data(tx_buf, char_event_reg.size + 1);
        } else if (rx_data[0] >= RGB_VALUE_START_REG_ADDR && rx_data[0] <= RGB_VALUE_END_REG_ADDR) {
            memcpy(&tx_buf[0], (uint8_t *)rgb_buf_reg, 8);
            i2c2_set_send_data((uint8_t *)&tx_buf[rx_data[0] - RGB_VALUE_START_REG_ADDR],
                               RGB_VALUE_END_REG_ADDR - rx_data[0] + 1);
        } else if (rx_data[0] >= FIRMWARE_VERSION_REG_ADDR && rx_data[0] <= I2C_ADDRESS_REG_ADDR) {
            tx_buf[0] = firmware_version_reg;
            tx_buf[1] = i2c_addr_reg;
            i2c2_set_send_data((uint8_t *)&tx_buf[rx_data[0] - FIRMWARE_VERSION_REG_ADDR],
                               I2C_ADDRESS_REG_ADDR - rx_data[0] + 1);
        }
    } else if (len > 1) {  // Write Request Logic / 写请求逻辑
        if (rx_data[0] >= SYS_CONFIG_START_REG_ADDR && rx_data[0] <= SYS_CONFIG_END_REG_ADDR && len <= 5) {
            update_register_buffer(rx_buf, rx_mark, rx_data, SYS_CONFIG_START_REG_ADDR, len);
            if (rx_mark[INTR_CONFIG_REG_ADDR]) {
                keyboard_intr_config_t config;
                config.value                                    = rx_buf[INTR_CONFIG_REG_ADDR];
                keyboard_intr_config_reg.bits.key_event_enable  = config.bits.key_event_enable;
                keyboard_intr_config_reg.bits.hid_event_enable  = config.bits.hid_event_enable;
                keyboard_intr_config_reg.bits.char_event_enable = config.bits.char_event_enable;
            }
            if (rx_mark[INTR_STATUS_REG_ADDR]) {
                if (rx_buf[INTR_STATUS_REG_ADDR] == 0) {
                    keyboard_intr_status_reg.value = 0;
                    int_disable();  // Pull INT pin high / 将 INT 引脚拉高
                }
            }
            if (rx_mark[EVENT_NUM_REG_ADDR]) {
                if (rx_buf[EVENT_NUM_REG_ADDR] == 0) {  // Clear FIFO if 0 written / 写入 0 则清空 FIFO
                    if (keyboard_work_mode_reg == KEYBOARD_MODE_KEY) {
                        event_fifo_reset();
                    } else if (keyboard_work_mode_reg == KEYBOARD_MODE_HID) {
                        hid_fifo_reset();
                    } else if (keyboard_work_mode_reg == KEYBOARD_MODE_CHAR) {
                        char_fifo_reset();
                    }
                    int_disable();
                }
            }
            if (rx_mark[RGB_BRIGHTNESS_REG_ADDR]) {
                if (rx_buf[RGB_BRIGHTNESS_REG_ADDR] != rgb_brightness_reg &&
                    rx_buf[RGB_BRIGHTNESS_REG_ADDR] <= RGB_BRIGHTNESS_MAX) {
                    rgb_brightness_reg         = rx_buf[RGB_BRIGHTNESS_REG_ADDR];
                    rgb_brightness_update_flag = 1;  // Trigger brightness update / 触发亮度更新
                }
            }
        } else if (rx_data[0] >= MODE_CONFIG_START_REG_ADDR && rx_data[0] <= MODE_CONFIG_END_REG_ADDR && len <= 3) {
            update_register_buffer(rx_buf, rx_mark, rx_data, MODE_CONFIG_START_REG_ADDR, len);
            if (rx_mark[KEYBOARD_MODE_REG_ADDR]) {
                if (rx_buf[KEYBOARD_MODE_REG_ADDR] != keyboard_work_mode_reg &&
                    rx_buf[KEYBOARD_MODE_REG_ADDR] <= KEYBOARD_MODE_CHAR) {
                    switch (rx_buf[KEYBOARD_MODE_REG_ADDR]) {
                        case KEYBOARD_MODE_KEY:
                            event_fifo_reset();  // Clear all FIFOs / 清空所有 FIFO
                            break;
                        case KEYBOARD_MODE_HID:
                            hid_fifo_reset();
                            break;
                        case KEYBOARD_MODE_CHAR:
                            char_fifo_reset();
                            break;
                        default:
                            break;
                    }
                    keyboard_work_mode_reg = rx_buf[KEYBOARD_MODE_REG_ADDR];
                    int_disable();
                    keyboard_work_mode_update_flag = 1;
                }
            }
            if (rx_mark[RGB_MODE_REG_ADDR]) {
                if (rx_buf[RGB_MODE_REG_ADDR] != rgb_work_mode_reg && rx_buf[RGB_MODE_REG_ADDR] <= RGB_MODE_USER) {
                    rgb_work_mode_reg    = rx_buf[RGB_MODE_REG_ADDR];
                    rgb_mode_update_flag = 1;
                }
            }
        } else if (rx_data[0] >= RGB_VALUE_START_REG_ADDR && rx_data[0] <= RGB_VALUE_END_REG_ADDR && len <= 9) {
            update_register_buffer(rx_buf, rx_mark, rx_data, RGB_VALUE_START_REG_ADDR, len);
            for (uint8_t i = 0; i < 8; i++) {
                if (rx_mark[i] && (rx_buf[i] != rgb_buf_reg[i])) {
                    rgb_buf_reg[i]              = rx_buf[i];
                    rgb_user_buffer_update_flag = 1;
                }
            }
        } else if (rx_data[0] == IAP_UPDATE_REG_ADDR && len == 2) {
            if (rx_data[1]) {
                NVIC_SystemReset();  // Software reset for update / 软件重置以进行更新
            }
        } else if (rx_data[0] == I2C_ADDRESS_REG_ADDR && len == 2) {
            if (rx_data[1] >= I2C_ADDR_REG_MIN && rx_data[1] <= I2C_ADDR_REG_MAX) {
                if (i2c_addr_reg != rx_data[1]) {
                    i2c_addr_reg = rx_data[1];
                    set_i2c_addr(rx_data[1]);  // Update local address / 更新本地地址
                    user_i2c_init();           // Re-initialize I2C / 重新初始化 I2C
                    i2c2_it_enable();          // Re-enable interrupts / 重新使能中断
                }
            }
        }
    }
}

/**
 * @brief Handler for I2C timeout events to recover the bus.
 *        I2C 超时事件处理句柄，用于恢复总线。
 * @note  Monitors stop condition timeout and re-initializes I2C if necessary.
 *        监控停止条件超时并在必要时重新初始化 I2C。
 *
 * @param None
 * @retval None
 */
void i2c_timeout_handler(void)
{
    i2c_timeout_counter = 0;
    if (i2c_stop_timeout_flag) {
        if (i2c_stop_timeout_delay < HAL_GetTick()) {
            i2c_stop_timeout_counter++;
            i2c_stop_timeout_delay = HAL_GetTick() + 10;
        }
    }
    // If timeout counter exceeds limit, reset I2C peripheral / 若超时计数超过限制，重置 I2C 外设
    if (i2c_stop_timeout_counter > 50) {
        LL_I2C_DeInit(I2C2);
        LL_I2C_DisableAutoEndMode(I2C2);
        LL_I2C_Disable(I2C2);
        LL_I2C_DisableIT_ADDR(I2C2);
        user_i2c_init();   // Recovery initialization / 恢复初始化
        i2c2_it_enable();  // Re-enable interrupts / 重新使能中断
        HAL_Delay(500);
    }
}