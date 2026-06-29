/*
 * SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef __USER_HID_FIFO_H__
#define __USER_HID_FIFO_H__

#include "user_sys.h"

#ifdef __cplusplus
extern "C" {
#endif

/* -------- API 声明 -------- */

/**
 * @brief Initialize the global HID event FIFO.
 *        初始化全局 HID 事件 FIFO。
 *
 * @param None
 * @retval None
 */
void hid_fifo_init(void);

/**
 * @brief Reset the FIFO pointers and count to clear all data.
 *        清空 FIFO，重置指针和计数。
 *
 * @param None
 * @retval None
 */
void hid_fifo_reset(void);

/**
 * @brief Check if the FIFO is empty.
 *        检查 FIFO 是否为空。
 *
 * @param None
 * @retval 1 Empty / 为空, 0 Not empty / 非空
 */
int hid_fifo_is_empty(void);

/**
 * @brief Check if the FIFO is full.
 *        检查 FIFO 是否已满。
 *
 * @param None
 * @retval 1 Full / 已满, 0 Not full / 未满
 */
int hid_fifo_is_full(void);

/**
 * @brief Get the current number of elements in the FIFO.
 *        获取当前 FIFO 中的元素数量。
 *
 * @param None
 * @retval Number of elements / 元素个数
 */
uint8_t hid_fifo_count(void);

/**
 * @brief Push an HID event into the FIFO (Enqueue).
 *        向 FIFO 写入一个 HID 事件（入队）。
 *
 * @param val The HID event data to be written.
 *            待写入的 HID 事件数据。
 * @retval 0 Success / 成功, -1 FIFO full / 队列满
 */
int hid_fifo_push(hid_event_t val);

/**
 * @brief Pop an HID event from the FIFO (Dequeue).
 *        从 FIFO 读取一个 HID 事件（出队）。
 *
 * @param val Pointer to return the retrieved HID event.
 *            指向用于返回读取值的指针。
 * @retval 0 Success / 成功, -1 FIFO empty / 队列空
 */
int hid_fifo_pop(hid_event_t *val);

/**
 * @brief Peek at the next HID event in the FIFO without dequeuing.
 *        查看下一个元素但不出队。
 *
 * @param val Pointer to return the next HID event.
 *            指向用于返回下一个值的指针。
 * @retval 0 Success / 成功, -1 FIFO empty / 队列空
 */
int hid_fifo_peek(hid_event_t *val);

#ifdef __cplusplus
}
#endif

#endif /* __USER_HID_FIFO_H__ */