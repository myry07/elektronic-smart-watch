#ifndef ADAPTER_BTN_H_
#define ADAPTER_BTN_H_

#define BUTTON_NUM 0

#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

extern TaskHandle_t btn_handle;

// 按键状态机内部状态（组件内部用，可隐藏）
typedef enum {
    BTN_IDLE,
    BTN_DEBOUNCE_PRESS,
    BTN_PRESSED,
    BTN_LONG_PRESSED,
    BTN_DEBOUNCE_RELEASE
} button_state_t;

// 按键事件（对外暴露）
typedef enum {
    BUTTON_EVENT_SINGLE_CLICK,
    BUTTON_EVENT_DOUBLE_CLICK,
    BUTTON_EVENT_LONG_PRESS
} button_event_t;

// 回调函数类型
typedef void (*button_callback_t)(button_event_t event);

// 注册用户回调（主程序调用）
void button_register(button_callback_t cb);

// 主任务
void button_task(void *arg);

void adpater_btn_init(void);
void button_callback(button_event_t event);

#endif