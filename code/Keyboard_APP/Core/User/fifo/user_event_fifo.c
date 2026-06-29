/*
 * SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "user_event_fifo.h"
#include "user_int.h"
#include "user_i2c_callback.h"

/* ----------- 全局变量定义 ----------- */
static __IO event_fifo_t event_fifo;  // Internal event FIFO instance / 内部事件 FIFO 实例

/**
 * @brief Initialize the global event FIFO.
 *        初始化全局事件 FIFO。
 */
void event_fifo_init(void)
{
    uint32_t primask = __get_PRIMASK();
    __disable_irq();

    memset((void *)&event_fifo, 0, sizeof(event_fifo));  // Zero out the structure / 结构体清零

    __set_PRIMASK(primask);
}

/**
 * @brief Reset the FIFO pointers and count to clear all data.
 *        清空 FIFO，重置指针和计数。
 */
void event_fifo_reset(void)
{
    uint32_t primask = __get_PRIMASK();
    __disable_irq();

    event_fifo.head  = 0;  // Reset write pointer / 重置写指针
    event_fifo.tail  = 0;  // Reset read pointer / 重置读指针
    event_fifo.count = 0;  // Reset counter / 重置计数器

    __set_PRIMASK(primask);
}

/**
 * @brief Check if the FIFO is empty.
 *        检查 FIFO 是否为空。
 */
int event_fifo_is_empty(void)
{
    return (event_fifo.count == 0);
}

/**
 * @brief Check if the FIFO is full.
 *        检查 FIFO 是否已满。
 */
int event_fifo_is_full(void)
{
    return (event_fifo.count >= FIFO_DEPTH);
}

/**
 * @brief Get the current number of elements in the FIFO.
 *        获取当前 FIFO 中的元素数量。
 */
uint8_t event_fifo_count(void)
{
    return event_fifo.count;
}

/**
 * @brief Push a byte into the FIFO (Enqueue).
 *        向 FIFO 写入一个字节（入队）。
 *
 * @param val The byte data to be written.
 * @retval 0 Success / 成功, -1 FIFO full / 队列满
 */
int event_fifo_push(uint8_t val)
{
    // Check if full with mode dependency / 检查是否已满
    if (event_fifo.count >= FIFO_DEPTH) {
        return -1;
    }

    // Prepare index outside critical section / 在临界区外计算索引
    uint8_t h      = event_fifo.head;
    uint8_t next_h = (h + 1U) & FIFO_MASK;

    event_fifo.buf[h] = val;  // Store value / 存入值

    // Atomic update / 原子更新
    uint32_t primask = __get_PRIMASK();
    __disable_irq();

    event_fifo.head = next_h;
    event_fifo.count++;

    __set_PRIMASK(primask);

    return 0;
}

/**
 * @brief Pop a byte from the FIFO (Dequeue).
 *        从 FIFO 读取一个字节（出队）。
 *
 * @param val Pointer to return the retrieved byte.
 * @retval 0 Success / 成功, -1 FIFO empty / 队列空
 */
int event_fifo_pop(uint8_t *val)
{
    if (event_fifo.count == 0) return -1;

    uint8_t t      = event_fifo.tail;
    uint8_t next_t = (t + 1U) & FIFO_MASK;

    if (val) {
        *val = event_fifo.buf[t];  // Get value / 获取值
    }
    event_fifo.buf[t] = 0;  // Clear slot after reading / 读取后清零

    // Atomic update / 原子更新
    uint32_t primask = __get_PRIMASK();
    __disable_irq();

    event_fifo.tail = next_t;
    event_fifo.count--;

    __set_PRIMASK(primask);

    return 0;
}

/**
 * @brief Peek at the next byte in the FIFO without dequeuing.
 *        查看下一个元素但不出队。
 *
 * @param val Pointer to return the next value.
 * @retval 0 Success / 成功, -1 FIFO empty / 队列空
 */
int event_fifo_peek(uint8_t *val)
{
    if (event_fifo.count == 0) return -1;

    if (val) {
        *val = event_fifo.buf[event_fifo.tail];  // Copy value without moving tail / 拷贝值
    }

    return 0;
}