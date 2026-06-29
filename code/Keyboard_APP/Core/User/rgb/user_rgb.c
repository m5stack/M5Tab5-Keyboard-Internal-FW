/*
 * SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "user_rgb.h"
#include "tim.h"
#include "user_i2c_callback.h"
#include "stdio.h"

/**
 * @brief RGB DMA send buffer.
 *        RGB DMA 发送缓冲区。
 * @note  Buffer layout:
 *        [3 leading low slots] +
 *        [24 PWM slots per RGB LED] * RGB_NUM +
 *        [reset slots]
 *        缓冲区布局：
 *        [前导低电平 3 个] +
 *        [每个 RGB 灯珠 24 个 PWM 时隙] * RGB_NUM +
 *        [复位时隙]
 */
static uint16_t rgb_send_buf[100 + 24 * RGB_NUM + RESET_SLOTS] = {0};

/**
 * @brief RGB DMA busy flag.
 *        RGB DMA 忙状态标志。
 * @note  This flag is set when a PWM + DMA transmission is in progress
 *        and cleared in the PWM pulse finished callback.
 *        当 PWM + DMA 传输进行中时该标志被置位，
 *        在 PWM 脉冲发送完成回调中清除。
 */
static volatile uint8_t rgb_dma_busy = 0;

/**
 * @brief RGB brightness update request flag.
 *        RGB 亮度更新请求标志。
 */
__IO uint8_t rgb_brightness_update_flag = 0;

/**
 * @brief RGB user buffer update request flag.
 *        RGB 用户缓冲区更新请求标志。
 */
__IO uint8_t rgb_user_buffer_update_flag = 0;

/**
 * @brief RGB mode update request flag.
 *        RGB 模式更新请求标志。
 */
__IO uint8_t rgb_mode_update_flag = 1;

/**
 * @brief Keyboard work mode update request flag.
 *        键盘工作模式更新请求标志。
 */
__IO uint8_t keyboard_work_mode_update_flag = 0;

/* ============================================================
 * Utility functions
 * ============================================================*/

/**
 * @brief Convert RGB888 value to separate R, G, B components.
 *        将 RGB888 格式颜色值拆分为 R、G、B 分量。
 *
 * @param rgb888 RGB888 color value.
 *               RGB888 格式颜色值。
 * @param r      Pointer to red component.
 *               红色分量指针。
 * @param g      Pointer to green component.
 *               绿色分量指针。
 * @param b      Pointer to blue component.
 *               蓝色分量指针。
 *
 * @retval None
 */
static inline void rgb888_to_rgb(uint32_t rgb888, uint8_t *r, uint8_t *g, uint8_t *b)
{
    *r = (uint8_t)(rgb888 >> 16);
    *g = (uint8_t)(rgb888 >> 8);
    *b = (uint8_t)(rgb888);
}

/**
 * @brief Apply global brightness scaling to a color component.
 *        对单个颜色分量应用全局亮度缩放。
 * @note  Integer arithmetic is used to avoid floating-point operations.
 *        使用整数运算以避免浮点运算。
 *
 * @param c          Original color component value.
 *                   原始颜色分量值。
 * @param brightness Global brightness value (0–100).
 *                   全局亮度值（0–100）。
 *
 * @retval Scaled color component value.
 *         缩放后的颜色分量值。
 */
static inline uint8_t apply_brightness(uint8_t c, uint8_t brightness)
{
    return (uint8_t)((c * brightness) / 100);
}

/**
 * @brief Encode one RGB LED data in GRB order for WS2812.
 *        按 WS2812 要求以 GRB 顺序编码一个 RGB 灯珠的数据。
 * @note  Data is encoded MSB first.
 *        数据按高位优先（MSB first）方式编码。
 *
 * @param buf Pointer to the start of the 24-slot PWM buffer.
 *            指向 24 个 PWM 时隙缓冲区的指针。
 * @param g   Green component value.
 *            绿色分量值。
 * @param r   Red component value.
 *            红色分量值。
 * @param b   Blue component value.
 *            蓝色分量值。
 *
 * @retval None
 */
static inline void encode_grb(uint16_t *buf, uint8_t g, uint8_t r, uint8_t b)
{
    for (uint8_t j = 0; j < 8; j++) {
        buf[j]      = (g & (0x80 >> j)) ? BIT_1 : BIT_0;
        buf[j + 8]  = (r & (0x80 >> j)) ? BIT_1 : BIT_0;
        buf[j + 16] = (b & (0x80 >> j)) ? BIT_1 : BIT_0;
    }
}

/* ============================================================
 * RGB buffer update functions
 * ============================================================*/

/**
 * @brief Update RGB DMA buffer using user-defined RGB data.
 *        使用用户自定义 RGB 数据更新 DMA 发送缓冲区。
 * @note  This function reads RGB data from rgb_buf_reg,
 *        applies global brightness scaling,
 *        and encodes the data into WS2812 PWM format.
 *        该函数从 rgb_buf_reg 中读取 RGB 数据，
 *        应用全局亮度缩放后，
 *        编码为 WS2812 所需的 PWM 格式。
 *
 * @retval None
 */
static void update_user_rgb_buf(void)
{
    uint8_t r, g, b;

    for (uint8_t i = 0; i < RGB_NUM; i++) {
        uint16_t *p = &rgb_send_buf[3 + i * 24];

        b = apply_brightness(rgb_buf_reg[i * 4 + 0], rgb_brightness_reg);
        g = apply_brightness(rgb_buf_reg[i * 4 + 1], rgb_brightness_reg);
        r = apply_brightness(rgb_buf_reg[i * 4 + 2], rgb_brightness_reg);

        encode_grb(p, g, r, b);
    }
}

/**
 * @brief Update RGB DMA buffer for indicator LEDs.
 *        更新指示灯模式下的 RGB DMA 发送缓冲区。
 * @note  Indicator LEDs reflect AA status and keyboard work mode.
 *        指示灯用于显示 AA 状态及键盘工作模式。
 *
 * @retval None
 */
static void update_indicator_buffer(void)
{
    uint8_t r, g, b;
    uint32_t rgb_temp;

    // 获取当前毫秒数，用于计算闪烁状态
    // 每 500ms 翻转一次状态 (200ms 亮, 200ms 灭)
    uint8_t blink_on = (HAL_GetTick() / 200) % 2;

    for (uint8_t i = 0; i < RGB_NUM; i++) {
        uint16_t *p = &rgb_send_buf[3 + i * 24];
        rgb_temp    = 0;

        if (i == 0) {
            // 逻辑：如果是非 KEY 模式
            if (keyboard_work_mode_reg != KEYBOARD_MODE_KEY) {
                if (aa_blink_flag) {
                    // 闪烁模式：根据时间决定亮还是灭
                    rgb_temp = blink_on ? AA_INDICATOR_LIGHT : 0;
                } else if (aa_flag) {
                    // 常亮模式
                    rgb_temp = AA_INDICATOR_LIGHT;
                }
            }
        } else if (i == 1) {
            switch (keyboard_work_mode_reg) {
                case KEYBOARD_MODE_KEY:
                    rgb_temp = KEY_MODE_INDICATOR_LIGHT;
                    break;
                case KEYBOARD_MODE_HID:
                    rgb_temp = HID_MODE_INDICATOR_LIGHT;
                    break;
                case KEYBOARD_MODE_CHAR:
                    rgb_temp = CHAR_MODE_INDICATOR_LIGHT;
                    break;
                default:
                    break;
            }
        }

        rgb888_to_rgb(rgb_temp, &r, &g, &b);

        r = apply_brightness(r, rgb_brightness_reg);
        g = apply_brightness(g, rgb_brightness_reg);
        b = apply_brightness(b, rgb_brightness_reg);

        encode_grb(p, g, r, b);
    }
}

/* ============================================================
 * PWM + DMA control
 * ============================================================*/

/**
 * @brief Start RGB data transmission via PWM + DMA.
 *        通过 PWM + DMA 启动 RGB 数据发送。
 * @note  Any ongoing PWM/DMA transfer will be stopped before
 *        starting a new transmission.
 *        在启动新的传输前会停止可能正在进行的 PWM/DMA 传输。
 *
 * @retval None
 */
void rgb_send(void)
{
    HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_3);
    HAL_TIMEx_PWMN_Stop_DMA(&htim1, TIM_CHANNEL_3);

    HAL_TIMEx_PWMN_Start_DMA(&htim1, TIM_CHANNEL_3, (uint32_t *)rgb_send_buf, 100 + 24 * RGB_NUM + RESET_SLOTS);
}

/**
 * @brief PWM pulse finished callback.
 *        PWM 脉冲发送完成回调函数。
 * @note  This callback is invoked after all PWM pulses
 *        for the RGB data frame have been transmitted.
 *        当 RGB 数据帧的所有 PWM 脉冲发送完成后，
 *        该回调函数被调用。
 *
 * @param htim Pointer to TIM handle.
 *             定时器句柄指针。
 *
 * @retval None
 */
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == htim1.Instance) {
        HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_3);
        HAL_TIMEx_PWMN_Stop_DMA(&htim1, TIM_CHANNEL_3);
        rgb_dma_busy = 0;
    }
}

/* ============================================================
 * RGB update state machine
 * ============================================================*/

/**
 * @brief Handle RGB LED update process.
 *        处理 RGB 灯珠更新流程。
 * @note  This function checks update request flags and
 *        starts a new PWM + DMA transmission when needed.
 *        该函数检查各类更新请求标志，
 *        并在需要时启动新的 PWM + DMA 传输。
 *
 * @retval None
 */
void rgb_update(void)
{
    if (rgb_dma_busy) {
        return;
    }

    static uint32_t last_blink_tick = 0;

    switch (rgb_work_mode_reg) {
        case RGB_MODE_BIND:
            // 触发条件：原有标志位更新 OR (开启了闪烁且距离上次更新超过 50ms)
            if (rgb_brightness_update_flag || keyboard_work_mode_update_flag || rgb_mode_update_flag ||
                aa_update_flag || (aa_blink_flag && (HAL_GetTick() - last_blink_tick > 50))) {
                update_indicator_buffer();
                rgb_send();
                rgb_dma_busy                   = 1;
                rgb_brightness_update_flag     = 0;
                keyboard_work_mode_update_flag = 0;
                rgb_mode_update_flag           = 0;
                aa_update_flag                 = 0;
                last_blink_tick                = HAL_GetTick();  // 记录更新时间
            }
            break;
        case RGB_MODE_USER:
            if (rgb_user_buffer_update_flag || rgb_brightness_update_flag || rgb_mode_update_flag) {
                update_user_rgb_buf();
                rgb_send();
                rgb_dma_busy                = 1;
                rgb_user_buffer_update_flag = 0;
                rgb_brightness_update_flag  = 0;
                rgb_mode_update_flag        = 0;
            }
            break;
        default:
            break;
    }
}