/*
 * SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "user_event_fifo.h"
#include "user_keyboard_handle.h"
#include "user_hid_fifo.h"
#include "user_hid_map.h"
#include "user_i2c_callback.h"

/* ----------- Key Mapping Table / 按键映射表 ----------- */
/* Stores HID keycodes and string names for each key in different states
   存储每个按键在不同状态下的 HID 键码和字符串名称 */
const static key_value_t key_value_map[5][14] = {
    // ================= Row 1 =================
    // esc  1   2   3   4   5   6   7   8   9   0   -   +   del
    {{"esc", KEY_MOD_RESERVED, KEY_ESC, "ESC", KEY_MOD_RESERVED, KEY_ESC},
     {"1", KEY_MOD_RESERVED, KEY_1, "1", KEY_MOD_RESERVED, KEY_1},
     {"2", KEY_MOD_RESERVED, KEY_2, "2", KEY_MOD_RESERVED, KEY_2},
     {"3", KEY_MOD_RESERVED, KEY_3, "3", KEY_MOD_RESERVED, KEY_3},
     {"4", KEY_MOD_RESERVED, KEY_4, "4", KEY_MOD_RESERVED, KEY_4},
     {"5", KEY_MOD_RESERVED, KEY_5, "5", KEY_MOD_RESERVED, KEY_5},
     {"6", KEY_MOD_RESERVED, KEY_6, "6", KEY_MOD_RESERVED, KEY_6},
     {"7", KEY_MOD_RESERVED, KEY_7, "7", KEY_MOD_RESERVED, KEY_7},
     {"8", KEY_MOD_RESERVED, KEY_8, "8", KEY_MOD_RESERVED, KEY_8},
     {"9", KEY_MOD_RESERVED, KEY_9, "9", KEY_MOD_RESERVED, KEY_9},
     {"0", KEY_MOD_RESERVED, KEY_0, "0", KEY_MOD_RESERVED, KEY_0},
     {"-", KEY_MOD_RESERVED, KEY_MINUS, "-", KEY_MOD_RESERVED, KEY_MINUS},
     {"+", KEY_MOD_LSHIFT, KEY_EQUAL, "+", KEY_MOD_LSHIFT, KEY_EQUAL},
     {"del", KEY_MOD_RESERVED, KEY_DELETE, "DEL", KEY_MOD_RESERVED, KEY_DELETE}},
    // ================= Row 2 =================
    // `~  !?  @  #  $  %  ^  &  */  (<  )>  [{  ]}  \|
    {{"`", KEY_MOD_RESERVED, KEY_GRAVE, "~", KEY_MOD_LSHIFT, KEY_GRAVE},
     {"!", KEY_MOD_LSHIFT, KEY_1, "?", KEY_MOD_LSHIFT, KEY_SLASH},
     {"@", KEY_MOD_LSHIFT, KEY_2, "@", KEY_MOD_LSHIFT, KEY_2},
     {"#", KEY_MOD_LSHIFT, KEY_3, "#", KEY_MOD_LSHIFT, KEY_3},
     {"$", KEY_MOD_LSHIFT, KEY_4, "$", KEY_MOD_LSHIFT, KEY_4},
     {"%", KEY_MOD_LSHIFT, KEY_5, "%", KEY_MOD_LSHIFT, KEY_5},
     {"^", KEY_MOD_LSHIFT, KEY_6, "^", KEY_MOD_LSHIFT, KEY_6},
     {"&", KEY_MOD_LSHIFT, KEY_7, "&", KEY_MOD_LSHIFT, KEY_7},
     {"*", KEY_MOD_LSHIFT, KEY_8, "/", KEY_MOD_RESERVED, KEY_SLASH},
     {"(", KEY_MOD_LSHIFT, KEY_9, "<", KEY_MOD_LSHIFT, KEY_COMMA},
     {")", KEY_MOD_LSHIFT, KEY_0, ">", KEY_MOD_LSHIFT, KEY_DOT},
     {"[", KEY_MOD_RESERVED, KEY_LEFTBRACE, "{", KEY_MOD_LSHIFT, KEY_LEFTBRACE},
     {"]", KEY_MOD_RESERVED, KEY_RIGHTBRACE, "}", KEY_MOD_LSHIFT, KEY_RIGHTBRACE},
     {"\\", KEY_MOD_RESERVED, KEY_BACKSLASH, "|", KEY_MOD_LSHIFT, KEY_BACKSLASH}},
    // ================= Row 3 =================
    // tab Q W E R T Y U I O P ;: '"
    {{"tab", KEY_MOD_RESERVED, KEY_TAB, "tab", KEY_MOD_RESERVED, KEY_TAB},
     {"q", KEY_MOD_RESERVED, KEY_Q, "Q", KEY_MOD_LSHIFT, KEY_Q},
     {"w", KEY_MOD_RESERVED, KEY_W, "W", KEY_MOD_LSHIFT, KEY_W},
     {"e", KEY_MOD_RESERVED, KEY_E, "E", KEY_MOD_LSHIFT, KEY_E},
     {"r", KEY_MOD_RESERVED, KEY_R, "R", KEY_MOD_LSHIFT, KEY_R},
     {"t", KEY_MOD_RESERVED, KEY_T, "T", KEY_MOD_LSHIFT, KEY_T},
     {"y", KEY_MOD_RESERVED, KEY_Y, "Y", KEY_MOD_LSHIFT, KEY_Y},
     {"u", KEY_MOD_RESERVED, KEY_U, "U", KEY_MOD_LSHIFT, KEY_U},
     {"i", KEY_MOD_RESERVED, KEY_I, "I", KEY_MOD_LSHIFT, KEY_I},
     {"o", KEY_MOD_RESERVED, KEY_O, "O", KEY_MOD_LSHIFT, KEY_O},
     {"p", KEY_MOD_RESERVED, KEY_P, "P", KEY_MOD_LSHIFT, KEY_P},
     {";", KEY_MOD_RESERVED, KEY_SEMICOLON, ":", KEY_MOD_LSHIFT, KEY_SEMICOLON},
     {"'", KEY_MOD_RESERVED, KEY_APOSTROPHE, "\"", KEY_MOD_LSHIFT, KEY_APOSTROPHE},
     {"backspace", KEY_MOD_RESERVED, KEY_BACKSPACE, "BACKSPACE", KEY_MOD_RESERVED, KEY_BACKSPACE}},
    // ================= Row 4 =================
    // sym Aa A S D F G H J K L ▲ _=
    {{"sym", KEY_MOD_RESERVED, KEY_LEFTSHIFT, "sym", KEY_MOD_RESERVED, KEY_LEFTSHIFT},
     {"Aa", KEY_MOD_RESERVED, KEY_LEFTSHIFT, "Aa", KEY_MOD_RESERVED, KEY_LEFTSHIFT},
     {"a", KEY_MOD_RESERVED, KEY_A, "A", KEY_MOD_LSHIFT, KEY_A},
     {"s", KEY_MOD_RESERVED, KEY_S, "S", KEY_MOD_LSHIFT, KEY_S},
     {"d", KEY_MOD_RESERVED, KEY_D, "D", KEY_MOD_LSHIFT, KEY_D},
     {"f", KEY_MOD_RESERVED, KEY_F, "F", KEY_MOD_LSHIFT, KEY_F},
     {"g", KEY_MOD_RESERVED, KEY_G, "G", KEY_MOD_LSHIFT, KEY_G},
     {"h", KEY_MOD_RESERVED, KEY_H, "H", KEY_MOD_LSHIFT, KEY_H},
     {"j", KEY_MOD_RESERVED, KEY_J, "J", KEY_MOD_LSHIFT, KEY_J},
     {"k", KEY_MOD_RESERVED, KEY_K, "K", KEY_MOD_LSHIFT, KEY_K},
     {"l", KEY_MOD_RESERVED, KEY_L, "L", KEY_MOD_LSHIFT, KEY_L},
     {"up", KEY_MOD_RESERVED, KEY_UP, "UP", KEY_MOD_RESERVED, KEY_UP},
     {"_", KEY_MOD_LSHIFT, KEY_MINUS, "=", KEY_MOD_RESERVED, KEY_EQUAL},
     {"enter", KEY_MOD_RESERVED, KEY_ENTER, "ENTER", KEY_MOD_RESERVED, KEY_ENTER}},
    // ================= Row 5 =================
    // ctrl alt Z X C V B N M . '  ◀ ▼ ▶ ␣
    {{"ctrl", KEY_MOD_RESERVED, KEY_LEFTCTRL, "ctrl", KEY_MOD_RESERVED, KEY_LEFTCTRL},
     {"alt", KEY_MOD_RESERVED, KEY_LEFTALT, "alt", KEY_MOD_RESERVED, KEY_LEFTALT},
     {"z", KEY_MOD_LSHIFT, KEY_Z, "Z", KEY_MOD_LSHIFT, KEY_Z},
     {"x", KEY_MOD_LSHIFT, KEY_X, "X", KEY_MOD_LSHIFT, KEY_X},
     {"c", KEY_MOD_LSHIFT, KEY_C, "C", KEY_MOD_LSHIFT, KEY_C},
     {"v", KEY_MOD_LSHIFT, KEY_V, "V", KEY_MOD_LSHIFT, KEY_V},
     {"b", KEY_MOD_LSHIFT, KEY_B, "B", KEY_MOD_LSHIFT, KEY_B},
     {"n", KEY_MOD_LSHIFT, KEY_N, "N", KEY_MOD_LSHIFT, KEY_N},
     {"m", KEY_MOD_LSHIFT, KEY_M, "M", KEY_MOD_LSHIFT, KEY_M},
     {".", KEY_MOD_RESERVED, KEY_DOT, ",", KEY_MOD_RESERVED, KEY_COMMA},
     {"left", KEY_MOD_RESERVED, KEY_LEFT, "LEFT", KEY_MOD_RESERVED, KEY_LEFT},
     {"down", KEY_MOD_RESERVED, KEY_DOWN, "DOWN", KEY_MOD_RESERVED, KEY_DOWN},
     {"right", KEY_MOD_RESERVED, KEY_RIGHT, "RIGHT", KEY_MOD_RESERVED, KEY_RIGHT},
     {" ", KEY_MOD_RESERVED, KEY_SPACE, " ", KEY_MOD_RESERVED, KEY_SPACE}}};

/* Key functional flags / 按键功能标记位 */
const uint8_t key_modifier_flag[5][14] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
};

/* Flags for keys affected by Aa mode / 受大小写模式影响的按键标志位 */
const uint8_t char_aa_flag[5][14] = {
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0},
};
/* ----------- Static Variables / 内部静态变量 ----------- */
static key_event_raw_t key_event_raw_buffer;   // Raw key event buffer / 原始按键事件缓冲区
static uint8_t modifier_mask = 0;              // HID modifier mask / HID 修饰键掩码
static key_handle_event_t key_handle_event;    // Processed event / 处理后的事件结构体
static bool sym_state                = false;  // State of SYM key / SYM 键按下状态
static bool Aa_state                 = false;  // State of Aa key / Aa 键按下状态
static bool ctrl_state               = false;  // State of Ctrl key / Ctrl 键按下状态
static bool alt_state                = false;  // State of Alt key / Alt 键按下状态
static bool special_key              = false;  // Flag for functional keys / 特殊功能按键标记
static uint32_t aa_press_start_tick  = 0;      // 记录按下时刻
static uint32_t aa_last_release_tick = 0;      // 记录上次松开时刻
static uint8_t aa_click_count        = 0;      // 连击计数
static bool aa_long_pressed_handled  = false;  // 防止一次长按重复触发
/* ----------- Global State Variables / 全局状态变量 ----------- */
__IO uint8_t aa_flag          = 0;  // Current capitalization mode / 当前大小写锁定状态
__IO uint8_t aa_update_flag   = 0;  // Flag for updating Aa state / 大小写状态更新标记
__IO uint8_t press_count      = 0;
__IO uint8_t last_press_count = 0;
__IO uint8_t aa_blink_flag    = 0;

/**
 * @brief Handle single click event for Aa key.
 *        处理 Aa 键单击事件。
 */
static void on_aa_single_click(void)
{
    if (last_press_count != 2) {
        aa_blink_flag  = 1;
        press_count    = 1;
        aa_flag        = 1;
        aa_update_flag = 1;
    } else {
        aa_blink_flag  = 0;
        press_count    = 1;
        aa_flag        = 0;
        aa_update_flag = 1;
    }

    last_press_count = 1;
}
/**
 * @brief Handle double click event for Aa key.
 *        处理 Aa 键双击事件。
 */
static void on_aa_double_click(void)
{
    aa_blink_flag    = 0;
    press_count      = 2;
    aa_flag          = 1;
    aa_update_flag   = 1;
    last_press_count = 2;
}
/**
 * @brief Update internal states based on special functional keys.
 *        根据特殊功能键更新内部状态机。
 *
 * @param key Raw key event information / 原始按键事件信息
 * @retval None
 */
static void updatemodifier_mask(const key_event_raw_t key)
{
    // Four special keys handled separately / 四个特殊功能键特殊处理

    // sysm key (Row 3, Col 0) / 处理 SYM 键
    if (key.row == 3 && key.col == 0) {
        if (key.state) {
            sym_state = true;  // Set SYM active / SYM 激活
        } else {
            sym_state = false;  // Set SYM inactive / SYM 取消
        }
        special_key = true;  // Mark as functional key / 标记为功能键
        return;
    }
    // Aa key (Row 3, Col 1) / 处理大小写切换键
    if (key.row == 3 && key.col == 1) {
        uint32_t now = HAL_GetTick();
        special_key  = true;  // 它是功能键，不直接发送键码
        if (key.state) {      // 按下
            Aa_state            = true;
            aa_press_start_tick = now;
            aa_flag             = 1;
            aa_update_flag      = 1;
            aa_blink_flag       = 1;
            // 判定是否是双击：距离上次松开小于阈值
            if (now - aa_last_release_tick < AA_DOUBLE_CLICK_MS) {
                aa_click_count++;
            } else {
                aa_click_count = 1;
            }
        } else {  // 松开
            Aa_state             = false;
            aa_last_release_tick = now;
            uint32_t duration    = now - aa_press_start_tick;
            if (duration < AA_DOUBLE_CLICK_MS) {
                if (aa_click_count >= 2) {
                    on_aa_double_click();
                    aa_click_count = 0;
                } else {
                    on_aa_single_click();
                }
            } else {
                aa_flag        = 0;
                aa_update_flag = 1;
                aa_blink_flag  = 0;
            }
        }
        return;
    }
    // ctrl key (Row 4, Col 0) / 处理 Ctrl 键
    if (key.row == 4 && key.col == 0) {
        if (key.state) {
            ctrl_state = true;
            modifier_mask |= KEY_MOD_LCTRL;  // Set Ctrl bit / 设置 Ctrl 掩码
        } else {
            ctrl_state = false;
            modifier_mask &= ~KEY_MOD_LCTRL;  // Clear Ctrl bit / 清除 Ctrl 掩码
        }
        special_key = true;
        return;
    }
    // alt key (Row 4, Col 1) / 处理 Alt 键
    if (key.row == 4 && key.col == 1) {
        if (key.state) {
            alt_state = true;
            modifier_mask |= KEY_MOD_LALT;  // Set Alt bit / 设置 Alt 掩码
        } else {
            alt_state = false;
            modifier_mask &= ~KEY_MOD_LALT;  // Clear Alt bit / 清除 Alt 掩码
        }
        special_key = true;
        return;
    }
}

/**
 * @brief Map physical key event to HID keyboard codes.
 *        将物理按键事件转换为 HID 键盘键码。
 *
 * @param key Raw key event / 原始按键事件
 * @retval Handled HID event / 处理后的 HID 事件
 */
static key_handle_event_t convert_to_hid(const key_event_raw_t key)
{
    key_handle_event_t ret;  // Return structure / 返回变量
    ret.state = key.state;   // Copy pressed state / 拷贝按键状态

    // Check if this is a letter key (a-z) / 检查是否为字母键
    KeScanCode_t baseKeyCode = key_value_map[key.row][key.col].firstKeyCode;
    bool isLetter            = (baseKeyCode >= KEY_A && baseKeyCode <= KEY_Z);

    if (isLetter) {
        if (aa_flag && ctrl_state == false && alt_state == false) {
            // Use upper case mappings / 使用大写映射
            ret.modifier = modifier_mask | key_value_map[key.row][key.col].secondModifierMask;
            ret.key_code = key_value_map[key.row][key.col].secondKeyCode;
            if (aa_flag && press_count == 1) {
                aa_flag        = 0;
                aa_update_flag = 1;
                press_count    = 0;
                aa_blink_flag  = 0;
            }
        } else {
            // Use lower case mappings / 使用小写映射
            ret.modifier = modifier_mask;
            ret.key_code = key_value_map[key.row][key.col].firstKeyCode;
        }
    } else if (key_modifier_flag[key.row][key.col]) {
        // Handle symbol modifier logic / 处理符号键切换逻辑
        if (sym_state) {
            ret.modifier = modifier_mask | key_value_map[key.row][key.col].secondModifierMask;
            ret.key_code = key_value_map[key.row][key.col].secondKeyCode;
        } else {
            ret.modifier = key_value_map[key.row][key.col].firstModifierMask;
            ret.key_code = key_value_map[key.row][key.col].firstKeyCode;
        }
    } else {
        // Standard non-letter keys / 标准非字母键
        ret.modifier = modifier_mask | key_value_map[key.row][key.col].firstModifierMask;
        ret.key_code = key_value_map[key.row][key.col].firstKeyCode;
    }
    return ret;
}

/**
 * @brief Map physical key event to character names/strings.
 *        将物理按键事件转换为字符名称或字符串。
 *
 * @param key Raw key event / 原始按键事件
 * @retval Handled character event / 处理后的字符事件
 */
static key_handle_event_t convert_to_char(const key_event_raw_t key)
{
    key_handle_event_t ret;  // Return structure / 返回变量
    ret.state = key.state;   // Copy pressed state / 拷贝按键状态

    KeScanCode_t baseKeyCode = key_value_map[key.row][key.col].firstKeyCode;
    bool isLetter            = (baseKeyCode >= KEY_A && baseKeyCode <= KEY_Z);

    if (isLetter || char_aa_flag[key.row][key.col]) {
        if (aa_flag) {
            ret.key_name = key_value_map[key.row][key.col].secondName;  // Get uppercase name / 获取大写名称
        } else {
            ret.key_name = key_value_map[key.row][key.col].firstName;  // Get lowercase name / 获取小写名称
        }
        if (aa_flag && press_count == 1) {
            aa_flag        = 0;
            aa_update_flag = 1;
            press_count    = 0;
            aa_blink_flag  = 0;
        }
    } else if (key_modifier_flag[key.row][key.col]) {
        if (sym_state) {
            ret.key_name = key_value_map[key.row][key.col].secondName;  // Get symbol name / 获取符号名称
        } else {
            ret.key_name = key_value_map[key.row][key.col].firstName;  // Get primary name / 获取原始名称
        }
    } else {
        ret.key_name = key_value_map[key.row][key.col].firstName;  // Default name / 默认名称
    }
    return ret;
}

/**
 * @brief Main logic for processing HID keyboard updates.
 *        HID 键盘更新主逻辑。
 *
 * @param key_event Original key event from hardware / 硬件原始按键事件
 * @retval Processed HID event / 处理后的 HID 事件
 */
static key_handle_event_t keyboard_update_hid(key_event_t key_event)
{
    special_key                = false;
    key_event_raw_buffer.state = key_event.bits.pressed;
    key_event_raw_buffer.row   = key_event.bits.row;
    key_event_raw_buffer.col   = key_event.bits.col;
    updatemodifier_mask(key_event_raw_buffer);  // Update functional states / 更新功能键状态
    key_handle_event             = convert_to_hid(key_event_raw_buffer);  // Convert to HID / 转换为 HID
    key_handle_event.special_key = special_key;
    return key_handle_event;
}

/**
 * @brief Main logic for processing Character keyboard updates.
 *        字符键盘更新主逻辑。
 *
 * @param key_event Original key event from hardware / 硬件原始按键事件
 * @retval Processed Character event / 处理后的字符事件
 */
static key_handle_event_t keyboard_update_char(key_event_t key_event)
{
    special_key                = false;
    key_event_raw_buffer.state = key_event.bits.pressed;
    key_event_raw_buffer.row   = key_event.bits.row;
    key_event_raw_buffer.col   = key_event.bits.col;
    updatemodifier_mask(key_event_raw_buffer);  // Update functional states / 更新功能键状态
    key_handle_event             = convert_to_char(key_event_raw_buffer);  // Convert to Char / 转换为字符
    key_handle_event.special_key = special_key;
    return key_handle_event;
}

/**
 * @brief Poll and process HID keyboard events.
 *        轮询并处理 HID 键盘事件。
 *
 * @param None
 * @retval None
 */
void key_hid_update(void)
{
    static key_handle_event_t key_hid_event;
    static key_event_t key_event;
    static hid_event_t hid_event;

    if (event_fifo_count()) {
        event_fifo_pop(&key_event.value);  // Dequeue raw event / 从队列取出原始事件

        key_hid_event      = keyboard_update_hid(key_event);  // Handle event / 处理事件
        hid_event.modifier = key_hid_event.modifier;
        hid_event.keycode  = key_hid_event.key_code;

        // Push to HID FIFO if not a special functional key / 非功能键则存入 HID 队列
        if (key_event.bits.pressed && !key_hid_event.special_key) {
            hid_fifo_push(hid_event);  // Pressed / 按下
        } else if (key_event.bits.pressed == 0 && !key_hid_event.special_key) {
            hid_event.keycode = 0;
            hid_fifo_push(hid_event);  // Released / 松开
        }
    }
}

/**
 * @brief Poll and process Character keyboard events.
 *        轮询并处理字符键盘事件。
 *
 * @param None
 * @retval None
 */
void key_char_update(void)
{
    static key_handle_event_t key_hid_event;
    static key_event_t key_event;
    static char_event_t char_event;

    if (event_fifo_count()) {
        event_fifo_pop(&key_event.value);  // Dequeue raw event / 从队列取出原始事件

        key_hid_event       = keyboard_update_char(key_event);  // Handle event / 处理事件
        char_event.modifier = 0;

        // Sync modifier states for character output / 为字符输出同步修饰键状态
        if (ctrl_state) {
            char_event.modifier |= KEY_MOD_LCTRL;
        } else {
            char_event.modifier &= ~KEY_MOD_LCTRL;
        }
        if (alt_state) {
            char_event.modifier |= KEY_MOD_LALT;
        } else {
            char_event.modifier &= ~KEY_MOD_LALT;
        }

        // Copy key name and calculate length / 拷贝键名并计算长度
        strcpy(char_event.str, key_hid_event.key_name);
        char_event.size = strlen(key_hid_event.key_name);

        // Push pressed character to FIFO / 将按下的字符存入队列
        if (key_event.bits.pressed && !key_hid_event.special_key) {
            char_fifo_push(char_event);
        }
    }
}