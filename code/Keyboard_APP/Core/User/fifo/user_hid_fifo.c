/*
 * SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "user_hid_fifo.h"
#include "user_int.h"
#include "user_i2c_callback.h"

/* ----------- 全局变量定义 ----------- */
static __IO hid_fifo_t hid_fifo;  // Internal HID FIFO instance / 内部 HID FIFO 实例

/**
 * @brief Initialize the global HID event FIFO.
 *        初始化全局 HID 事件 FIFO。
 */
void hid_fifo_init(void)
{
    uint32_t primask = __get_PRIMASK();
    __disable_irq();  // Enter critical section / 进入临界区

    memset((void *)&hid_fifo, 0, sizeof(hid_fifo));  // Zero out the structure / 结构体清零

    __set_PRIMASK(primask);  // Exit critical section / 退出临界区
}

/**
 * @brief Reset the FIFO pointers and count to clear all data.
 *        清空 FIFO，重置指针 and 计数。
 */
void hid_fifo_reset(void)
{
    uint32_t primask = __get_PRIMASK();
    __disable_irq();  // Atomic reset / 原子重置

    hid_fifo.head  = 0;
    hid_fifo.tail  = 0;
    hid_fifo.count = 0;

    __set_PRIMASK(primask);
}

/**
 * @brief Check if the FIFO is empty.
 *        检查 FIFO 是否为空。
 */
int hid_fifo_is_empty(void)
{
    return (hid_fifo.count == 0);
}

/**
 * @brief Check if the FIFO is full.
 *        检查 FIFO 是否已满。
 */
int hid_fifo_is_full(void)
{
    return (hid_fifo.count >= FIFO_DEPTH);
}

/**
 * @brief Get the current number of elements in the FIFO.
 *        获取当前 FIFO 中的元素数量。
 */
uint8_t hid_fifo_count(void)
{
    return hid_fifo.count;
}

/**
 * @brief Push an HID event into the FIFO (Enqueue).
 *        向 FIFO 写入一个 HID 事件（入队）。
 * @note  If the FIFO is full and keyboard_work_mode_reg is 1, returns error.
 *
 * @param val The HID event data to be written.
 * @retval 0 Success, -1 FIFO full
 */
int hid_fifo_push(hid_event_t val)
{
    // Mode-specific full check / 特定模式下检查是否已满
    if (hid_fifo.count >= FIFO_DEPTH) {
        return -1;
    }

    // Prepare data outside critical section / 在临界区外准备数据
    uint8_t h      = hid_fifo.head;
    uint8_t next_h = (h + 1U) & FIFO_MASK;  // Optimized index wrapping / 优化索引回环

    // Copy event data using struct assignment / 使用结构体直接赋值
    hid_fifo.buf[h] = val;

    // Atomic update of state / 原子更新共享状态
    uint32_t primask = __get_PRIMASK();
    __disable_irq();

    hid_fifo.head = next_h;
    hid_fifo.count++;

    __set_PRIMASK(primask);

    return 0;
}

/**
 * @brief Pop an HID event from the FIFO (Dequeue).
 *        从 FIFO 读取一个 HID 事件（出队）。
 * @note  The buffer slot's modifier and keycode are cleared to 0 after reading.
 *
 * @param val Pointer to return the retrieved HID event.
 * @retval 0 Success, -1 FIFO empty
 */
int hid_fifo_pop(hid_event_t *val)
{
    if (hid_fifo.count == 0) return -1;

    // Prepare data outside critical section / 在临界区外处理数据
    uint8_t t      = hid_fifo.tail;
    uint8_t next_t = (t + 1U) & FIFO_MASK;

    if (val) {
        *val = hid_fifo.buf[t];  // Structure assignment / 结构体赋值
    }

    // Clear buffer slot as required / 按要求清除该槽位数据
    hid_fifo.buf[t].modifier = 0;
    hid_fifo.buf[t].keycode  = 0;

    // Atomic update of state / 原子更新共享状态
    uint32_t primask = __get_PRIMASK();
    __disable_irq();

    hid_fifo.tail = next_t;
    hid_fifo.count--;

    __set_PRIMASK(primask);

    return 0;
}

/**
 * @brief Peek at the next HID event in the FIFO without dequeuing.
 *        查看下一个元素但不出队。
 *
 * @param val Pointer to return the next HID event.
 * @retval 0 Success, -1 FIFO empty
 */
int hid_fifo_peek(hid_event_t *val)
{
    if (hid_fifo.count == 0) return -1;

    if (val) {
        *val = hid_fifo.buf[hid_fifo.tail];  // Copy without moving pointers / 拷贝数据但不移动指针
    }

    return 0;
}