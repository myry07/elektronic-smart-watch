#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "adapter_btn.h"

#define DEBOUNCE_TIME_MS 30
#define LONG_PRESS_TIME_MS 2000
#define DOUBLE_CLICK_TIME_MS 350

TaskHandle_t btn_handle = NULL;

static const char *TAG = "adapter_btn";

// 支持单个按钮（可扩展为数组）
typedef struct {
    gpio_num_t gpio;
    button_state_t state;
    int64_t press_time;
    int64_t last_release_time;
    int click_count;
    bool long_press_handled;
    button_callback_t callback;
} button_instance_t;

static button_instance_t btn = {0};

void button_task(void *arg)
{
    while (1) {
        int level = gpio_get_level(btn.gpio);
        int64_t now = esp_timer_get_time() / 1000; // ms

        switch (btn.state)
        {
        case BTN_IDLE:
            if (level == 0) {
                btn.state = BTN_DEBOUNCE_PRESS;
                btn.press_time = now;
            } else if (btn.click_count == 1 && (now - btn.last_release_time) > DOUBLE_CLICK_TIME_MS) {
                btn.click_count = 0;
                ESP_LOGI(TAG, "Single Click (timeout) detected");
                // if (btn.callback) btn.callback(BUTTON_EVENT_SINGLE_CLICK);
            }
            break;

        case BTN_DEBOUNCE_PRESS:
            if ((now - btn.press_time) >= DEBOUNCE_TIME_MS) {
                if (gpio_get_level(btn.gpio) == 0) {
                    btn.state = BTN_PRESSED;
                    btn.press_time = now;
                } else {
                    btn.state = BTN_IDLE;
                }
            }
            break;

        case BTN_PRESSED:
            if (level == 1) {
                btn.state = BTN_DEBOUNCE_RELEASE;
                btn.press_time = now;
            } else if ((now - btn.press_time) >= LONG_PRESS_TIME_MS) {
                btn.state = BTN_LONG_PRESSED;
                btn.long_press_handled = true;
                ESP_LOGI(TAG, "Long Press detected");
                // if (btn.callback) btn.callback(BUTTON_EVENT_LONG_PRESS);
            }
            break;

        case BTN_LONG_PRESSED:
            if (level == 1) {
                btn.state = BTN_DEBOUNCE_RELEASE;
                btn.press_time = now;
            }
            break;

        case BTN_DEBOUNCE_RELEASE:
            if ((now - btn.press_time) >= DEBOUNCE_TIME_MS) {
                if (gpio_get_level(btn.gpio) == 1) {
                    btn.state = BTN_IDLE;
                    if (!btn.long_press_handled) {
                        if (btn.click_count == 0) {
                            btn.click_count = 1;
                            btn.last_release_time = now;
                        } else if ((now - btn.last_release_time) <= DOUBLE_CLICK_TIME_MS) {
                            btn.click_count = 0;
                            ESP_LOGI(TAG, "Double Click detected");
                            // if (btn.callback) btn.callback(BUTTON_EVENT_DOUBLE_CLICK);
                        } else {
                            btn.click_count = 1;
                            ESP_LOGI(TAG, "Single Click detected");
                            btn.last_release_time = now;
                        }
                    }
                    btn.long_press_handled = false;
                } else {
                    btn.state = BTN_PRESSED;
                }
            }
            break;
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void button_register(button_callback_t cb)
{
    btn.gpio = BUTTON_NUM;
    btn.callback = cb;
    btn.state = BTN_IDLE;
    btn.click_count = 0;
    btn.long_press_handled = false;

    gpio_config_t cfg = {
        .pin_bit_mask = (1ULL << BUTTON_NUM),
        .pull_up_en = GPIO_PULLUP_ONLY,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .mode = GPIO_MODE_INPUT,
        .intr_type = GPIO_INTR_DISABLE};

    gpio_config(&cfg);
}

void button_callback(button_event_t event)
{
    //发送给led任务
    // TaskHandle_t h = led_get_task_handle();
    // if (h)
    // {
    //     xTaskNotify(h, event, eSetValueWithOverwrite);
    // }
}

void adpater_btn_init(void)
{
    button_register(button_callback); // 注册事件
    xTaskCreatePinnedToCore(button_task, "btn", 2048, NULL, 5, &btn_handle, 1);
}