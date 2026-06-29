/*
 * SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "user_event_fifo.h"
#include "user_scan_handle.h"
#include "user_sys.h"

/* GPIO pin definition for each row in the key matrix
 * 键盘矩阵每一行对应的 GPIO 引脚定义
 */
static const uint16_t row_pins[8] = {LL_GPIO_PIN_0, LL_GPIO_PIN_1, LL_GPIO_PIN_2, LL_GPIO_PIN_3,
                                     LL_GPIO_PIN_4, LL_GPIO_PIN_5, LL_GPIO_PIN_6, LL_GPIO_PIN_7};

/* Key mapping table
 * Maps [row][column] to logical key ID
 * 键位映射表
 * 将 [行][列] 映射到逻辑按键 ID
 */
static const uint8_t keymap[8][9] = {
    {10, 8, 39, 45, 36, 30, 55, KEY_NULL, 60}, {1, 9, 40, 31, 37, 19, 28, KEY_NULL, 61},
    {2, 11, 50, 17, 49, 21, 59, 0, 64},        {3, 12, 51, 32, 48, 46, 57, 14, 65},
    {4, 27, 52, 33, 23, 16, 58, 62, 66},       {5, 25, 54, 34, 24, 44, KEY_NULL, 13, 53},
    {6, 26, 29, 22, 15, 20, KEY_NULL, 63, 67}, {7, 38, 47, 35, 18, 43, 42, 41, 68}};

/* Per-key state information
 * 每个按键的状态信息
 */
static key_state_t keys[TOTAL_KEYS];

/* Last scan bitmap cache
 * 上一次扫描结果缓存
 */
static uint8_t last_bitmap[TOTAL_KEYS];

/**
 * @brief Delay for a specified number of microseconds.
 *        微秒级延时函数。
 * @note  This is a busy-wait delay implemented using NOP instructions.
 *        It is used to allow GPIO signal levels to stabilize.
 *        该函数通过空操作指令实现忙等待，
 *        用于等待 GPIO 电平稳定。
 *
 * @param us Delay time in microseconds.
 *           延时时间（微秒）。
 * @retval None
 */
static inline void delay_us(uint32_t us)
{
    volatile uint32_t cycles = us * CYCLES_PER_US;  // Convert time to CPU cycles / 转换为 CPU 周期数
    while (cycles--) {
        __NOP();
    }
}

/* ---------- Key matrix and independent key scanning ---------- */
/* ---------- 扫描矩阵键及独立按键 ---------- */

/**
 * @brief Scan key matrix and independent keys.
 *        扫描键盘矩阵及独立按键。
 * @note  This function sequentially drives each row high and reads
 *        column input states to generate a key bitmap.
 *        该函数依次将每一行拉高，并读取列输入状态，
 *        生成当前的按键位图。
 *
 * @param bitmap Pointer to key bitmap buffer.
 *               按键位图缓冲区指针。
 * @retval None
 */
static void keyboard_scan(uint8_t *bitmap)
{
    /* Clear bitmap quickly / 快速清零位图 */
    memset(bitmap, 0, TOTAL_KEYS);

    /* Scan matrix keys / 扫描矩阵键 */
    for (uint8_t row = 0; row < 8; row++) {
        /* Drive current row high / 将当前行置高 */
        LL_GPIO_SetOutputPin(ROW0_GPIO_Port, row_pins[row]);

        /* Wait for signal stabilization / 等待信号稳定 */
        delay_us(10);

        /* Multiple sampling to get stable column state / 多次采样取稳定列状态 */
        uint16_t cols_state = 0xFFFF;
        for (uint8_t s = 0; s < SAMPLE_COUNT; s++) {
            cols_state &= GPIOA->IDR;
        }

        /* Scan 9 columns / 扫描 9 列 */
        for (uint8_t col = 0; col < 9; col++) {
            if (cols_state & (1U << col)) {
                uint8_t key_id = keymap[row][col];
                if (key_id != KEY_NULL && key_id < TOTAL_KEYS) {
                    bitmap[key_id] = 1;
                }
            }
        }

        /* Restore row to low level / 将该行恢复为低电平 */
        LL_GPIO_ResetOutputPin(ROW0_GPIO_Port, row_pins[row]);
    }

    /* Scan independent keys / 扫描独立按键 */
    if (LL_GPIO_IsInputPinSet(GPIOA, COL9_Pin) == 0) {
        bitmap[56] = 1;
    }
    if (LL_GPIO_IsInputPinSet(GPIOA, COL10_Pin) == 0) {
        bitmap[69] = 1;
    }
}

/* ---------- Event encoding and enqueue ---------- */
/* ---------- 事件编码并入队 ---------- */

/**
 * @brief Encode key event and push into event FIFO.
 *        编码按键事件并压入事件 FIFO。
 * @note  Event format:
 *        [7]    : pressed/released
 *        [6:4]  : row index
 *        [3:0]  : column index
 *        事件格式：
 *        [7]    : 按下/释放
 *        [6:4]  : 行号
 *        [3:0]  : 列号
 *
 * @param key_id Logical key ID.
 *               逻辑按键 ID。
 * @param pressed Key press state (1 = pressed, 0 = released).
 *                按键状态（1 = 按下，0 = 释放）。
 * @retval None
 */
static void keyboard_enqueue(uint8_t key_id, uint8_t pressed)
{
    if (key_id >= TOTAL_KEYS) return;

    uint8_t row = key_id / 14U;  // Calculate row index / 计算行号
    uint8_t col = key_id % 14U;  // Calculate column index / 计算列号

    uint8_t data = (pressed ? 0x80U : 0x00U) | ((row & 0x07U) << 4) | (col & 0x0FU);

    event_fifo_push(data);
}

/* ---------- State update and debouncing ---------- */
/* ---------- 状态更新与消抖处理 ---------- */

/**
 * @brief Process key states and apply debouncing.
 *        处理按键状态并进行消抖。
 * @note  This function compares sampled states with previous states,
 *        applies debounce timing, and generates key events on state change.
 *        该函数比较采样状态与历史状态，
 *        应用消抖时间，并在状态变化时生成按键事件。
 *
 * @param bitmap Current key bitmap.
 *               当前按键位图。
 * @param now Current system tick in milliseconds.
 *            当前系统时间（毫秒）。
 * @retval None
 */
static void keyboard_process(const uint8_t *bitmap, uint32_t now)
{
    for (uint8_t i = 0; i < TOTAL_KEYS; i++) {
        /* Optimization: process only changed keys
         * 优化：仅处理发生变化的按键
         */
        uint8_t sample = bitmap[i];
        if (sample == last_bitmap[i] && sample == keys[i].stable) {
            continue;
        }

        key_state_t *k = &keys[i];

        /* Detected sampling change / 检测到采样变化 */
        if (sample != k->sample) {
            k->sample           = sample;
            k->last_change_time = now;
            continue;
        }

        /* Debounce time elapsed and state is stable
         * 消抖时间已过且状态稳定
         */
        if ((now - k->last_change_time) >= DEBOUNCE_MS) {
            if (sample != k->stable) {
                k->stable = sample;
                keyboard_enqueue(i, sample);
            }
        }
    }

    /* Update last bitmap cache / 更新上一次位图缓存 */
    memcpy(last_bitmap, bitmap, TOTAL_KEYS);
}

/* ---------- Initialization ---------- */
/* ---------- 初始化 ---------- */

/**
 * @brief Initialize keyboard scanning module.
 *        初始化键盘扫描模块。
 * @note  This function clears all key states and ensures
 *        all row GPIO pins are set to low level.
 *        该函数清除所有按键状态，
 *        并确保所有行 GPIO 引脚处于低电平。
 *
 * @param None
 * @retval None
 */
void keyboard_init(void)
{
    memset(keys, 0, sizeof(keys));
    memset(last_bitmap, 0, sizeof(last_bitmap));

    /* Ensure all row pins are low at startup
     * 确保所有行引脚在启动时为低电平
     */
    for (uint8_t i = 0; i < 8; i++) {
        LL_GPIO_ResetOutputPin(ROW0_GPIO_Port, row_pins[i]);
    }
}

/* ---------- Main update interface ---------- */
/* ---------- 主更新接口 ---------- */

/**
 * @brief Periodic keyboard update function.
 *        键盘周期性更新函数。
 * @note  This function should be called periodically (e.g. in main loop)
 *        to scan the keyboard and process key events.
 *        该函数应周期性调用（如在主循环中），
 *        用于扫描键盘并处理按键事件。
 *
 * @param None
 * @retval None
 */
void keyboard_update(void)
{
    static uint8_t bitmap[TOTAL_KEYS];  // Key state bitmap / 按键状态位图
    uint32_t now = HAL_GetTick();       // Current system time / 当前系统时间

    keyboard_scan(bitmap);
    keyboard_process(bitmap, now);
}