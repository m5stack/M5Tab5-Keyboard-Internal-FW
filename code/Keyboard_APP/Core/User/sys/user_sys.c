/*
 * SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "user_sys.h"
#include "user_i2c_callback.h"
#include "user_char_fifo.h"
#include "user_hid_fifo.h"
#include "user_int.h"

/**
 * @brief Timer period elapsed callback.
 *        定时器周期到达回调函数。
 * @note  Used to check keyboard event FIFOs and
 *        trigger interrupt flags when events are pending.
 *        用于检查键盘事件 FIFO，
 *        在有事件待处理时触发中断标志。
 *
 * @param htim Pointer to timer handle.
 *             定时器句柄指针。
 *
 * @retval None
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim == (&htim6)) {
        switch (keyboard_work_mode_reg) {
            case KEYBOARD_MODE_KEY:
                if (keyboard_intr_config_reg.bits.key_event_enable) {
                    if (event_fifo_count()) {
                        keyboard_intr_status_reg.bits.key_event_flag = 1;
                        int_enable();
                    }
                }
                break;

            case KEYBOARD_MODE_HID:
                if (keyboard_intr_config_reg.bits.hid_event_enable) {
                    if (hid_fifo_count()) {
                        keyboard_intr_status_reg.bits.hid_event_flag = 1;
                        int_enable();
                    }
                }
                break;

            case KEYBOARD_MODE_CHAR:
                if (keyboard_intr_config_reg.bits.char_event_enable) {
                    if (char_fifo_count()) {
                        keyboard_intr_status_reg.bits.char_event_flag = 1;
                        int_enable();
                    }
                }
                break;

            default:
                break;
        }
    }
}

/**
 * @brief Update keyboard processing logic.
 *        更新键盘处理逻辑。
 * @note  Dispatches keyboard processing according to
 *        the current keyboard work mode.
 *        根据当前键盘工作模式，
 *        分发对应的键盘处理逻辑。
 *
 * @retval None
 */
void user_keyboard_update(void)
{
    switch (keyboard_work_mode_reg) {
        case KEYBOARD_MODE_KEY:
            /* Key scan handled elsewhere */
            break;

        case KEYBOARD_MODE_HID:
            key_hid_update();
            break;

        case KEYBOARD_MODE_CHAR:
            key_char_update();
            break;

        default:
            break;
    }
}