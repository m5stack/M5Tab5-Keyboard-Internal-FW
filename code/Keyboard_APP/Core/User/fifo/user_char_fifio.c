/*
 * SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "user_char_fifo.h"
#include "user_int.h"
#include "user_i2c_callback.h"

/* ----------- 全局变量定义 ----------- */
static __IO char_fifo_t char_fifo;  // Internal FIFO instance / 内部 FIFO 实例

/**
 * @brief Initialize the global character FIFO.
 *        初始化全局字符 FIFO。
 * @note  Clears the entire FIFO structure memory.
 *        清除整个 FIFO 结构体内存。
 *
 * @param None
 * @retval None
 */
void char_fifo_init(void)
{
    uint32_t primask = __get_PRIMASK();
    __disable_irq();  // Enter critical section / 进入临界区

    memset((void *)&char_fifo, 0, sizeof(char_fifo));  // Zero out the structure / 结构体清零

    __set_PRIMASK(primask);  // Exit critical section / 退出临界区
}

/**
 * @brief Reset the FIFO pointers and count to clear all data.
 *        清空 FIFO，重置指针和计数。
 *
 * @param None
 * @retval None
 */
void char_fifo_reset(void)
{
    uint32_t primask = __get_PRIMASK();
    __disable_irq();  // Atomic reset / 原子重置

    char_fifo.head  = 0;  // Reset write pointer / 重置写指针
    char_fifo.tail  = 0;  // Reset read pointer / 重置读指针
    char_fifo.count = 0;  // Reset counter / 重置计数器

    __set_PRIMASK(primask);
}

/**
 * @brief Check if the FIFO is empty.
 *        检查 FIFO 是否为空。
 *
 * @param None
 * @retval 1 Empty / 为空, 0 Not empty / 非空
 */
int char_fifo_is_empty(void)
{
    return (char_fifo.count == 0);  // Check if count is zero / 检查计数是否为零
}

/**
 * @brief Check if the FIFO is full.
 *        检查 FIFO 是否已满。
 *
 * @param None
 * @retval 1 Full / 已满, 0 Not full / 未满
 */
int char_fifo_is_full(void)
{
    return (char_fifo.count >= FIFO_DEPTH);  // Check if count reached depth / 检查计数是否达到深度
}

/**
 * @brief Get the current number of elements in the FIFO.
 *        获取当前 FIFO 中的元素数量。
 *
 * @param None
 * @retval Number of elements / 元素个数
 */
uint8_t char_fifo_count(void)
{
    return char_fifo.count;  // Return current count / 返回当前计数
}

/**
 * @brief Push a character event into the FIFO (Enqueue).
 *        向 FIFO 写入一个事件（入队）。
 * @note  If the FIFO is full, the data will not be written.
 *        如果 FIFO 已满，数据将不会被写入。
 *
 * @param event The character event to be written.
 * @retval 0 Success / 成功, -1 FIFO full / 队列满
 */
int char_fifo_push(char_event_t event)
{
    if (char_fifo.count >= FIFO_DEPTH) {
        return -1;  // Return error if full / 满载返回错误
    }

    // Prepare data outside critical section / 在临界区外准备数据，减少关中断时间
    uint8_t h      = char_fifo.head;
    uint8_t next_h = (h + 1U) & FIFO_MASK;

    char_fifo.buf[h].modifier = event.modifier;

    // Boundary check for size / 长度边界检查
    uint8_t max_size  = sizeof(char_fifo.buf[0].str);
    uint8_t copy_size = (event.size > max_size) ? max_size : event.size;

    memcpy(char_fifo.buf[h].str, event.str, copy_size);
    char_fifo.buf[h].size = copy_size;

    // Update pointers atomically / 原子更新指针和计数
    uint32_t primask = __get_PRIMASK();
    __disable_irq();

    char_fifo.head = next_h;
    char_fifo.count++;

    __set_PRIMASK(primask);

    return 0;  // Return success / 返回成功
}

/**
 * @brief Pop a character event from the FIFO (Dequeue).
 *        从 FIFO 读取一个事件（出队）。
 * @note  If the FIFO is empty, the operation fails.
 *        如果 FIFO 为空，操作失败。
 *
 * @param event Pointer to return the retrieved event.
 * @retval 0 Success / 成功, -1 FIFO empty / 队列空
 */
int char_fifo_pop(char_event_t *event)
{
    if (char_fifo.count == 0) return -1;  // Return error if empty / 空载返回错误

    uint8_t t      = char_fifo.tail;
    uint8_t next_t = (t + 1U) & FIFO_MASK;

    if (event) {
        // Copy event data using stored size / 使用存储的实际长度拷贝数据
        event->modifier = char_fifo.buf[t].modifier;
        event->size     = char_fifo.buf[t].size;
        memcpy(event->str, char_fifo.buf[t].str, event->size);
    }

    // Update pointers atomically / 原子更新指针和计数
    uint32_t primask = __get_PRIMASK();
    __disable_irq();

    char_fifo.tail = next_t;
    char_fifo.count--;

    __set_PRIMASK(primask);

    return 0;  // Return success / 返回成功
}

/**
 * @brief Peek at the next element in the FIFO without dequeuing.
 *        查看下一个元素但不出队。
 *
 * @param event Pointer to return the next event.
 * @retval 0 Success / 成功, -1 FIFO empty / 队列空
 */
int char_fifo_peek(char_event_t *event)
{
    if (char_fifo.count == 0) return -1;  // Return error if empty / 空载返回错误

    if (event) {
        uint8_t t = char_fifo.tail;
        // Copy event data without moving tail / 拷贝数据但不移动尾指针
        event->modifier = char_fifo.buf[t].modifier;
        event->size     = char_fifo.buf[t].size;
        memcpy(event->str, char_fifo.buf[t].str, event->size);
    }

    return 0;  // Return success / 返回成功
}