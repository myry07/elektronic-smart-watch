#include <stdio.h>
#include "adapter_lcd.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lvgl_port.h"
#include "ui.h"
#include "lvgl.h"
#include "adapter_btn.h"
#include "esp_log.h"

// #define I2C_SDA 5
// #define I2C_SCL 4
// #define TOUCH_RST 15

TaskHandle_t lvgltask_handle = NULL;

typedef enum
{
    PAGE_ID_LOCK = 0,
    PAGE_ID_HOME,
    PAGE_ID_PAGE1,
    PAGE_ID_PAGE2,
    PAGE_ID_PAGE3,

    PAGE_COUNT
} page_id_t;

uint32_t page_num = 0;

const char *file_path1 = "S:/spiffs/hc1.png";
const char *file_path2 = "S:/spiffs/hc2.png";

static void my_button_cb(button_event_t event)
{
    switch (event)
    {
    case BUTTON_EVENT_SINGLE_CLICK:
        ESP_LOGI("BTN", "Single Click");
        break;
    case BUTTON_EVENT_LONG_PRESS:
        ESP_LOGI("BTN", "Long Press");
        break;
    case BUTTON_EVENT_DOUBLE_CLICK:
        ESP_LOGI("BTN", "Double Click");
        break;
    }
}

static lv_obj_t *s_current_page = NULL;
static page_id_t s_current_page_id = PAGE_ID_LOCK; // 默认首页

void page_manager_switch_to(page_id_t id)
{
    if (id == s_current_page_id)
    {
        // 如果切换的是当前页面，就什么也不做（可选）
        return;
    }

    lv_obj_t *new_page = NULL;

    // 创建新页面（未挂载）
    switch (id)
    {
    case PAGE_ID_LOCK:
        new_page = ui_lock_page_create();
        break;
    case PAGE_ID_HOME:
        new_page = ui_home_page_create();
        break;
    case PAGE_ID_PAGE1:
        new_page = ui_page1_create();
        break;
    case PAGE_ID_PAGE2:
        new_page = ui_page2_create();
        break;
    case PAGE_ID_PAGE3:
        new_page = ui_page3_create();
        break;
    default:
        return;
    }

    if (s_current_page && lv_obj_is_valid(s_current_page))
    {
        lv_obj_del(s_current_page);
        s_current_page = NULL;
    }

    // lv_scr_load_anim(new_page, LV_SCR_LOAD_ANIM_MOVE_LEFT, 300, 0, false);

    s_current_page = new_page;
    s_current_page_id = id;
}


void button_callback(button_event_t event)
{
    if (lvgltask_handle)
    {
        xTaskNotify(lvgltask_handle, event, eSetValueWithOverwrite);
    }
}

void lvgl_task(void *param)
{
    ui_lock_page_create();
    uint32_t event;
    while (1)
    {
        if (xTaskNotifyWait(0, 0, &event, pdMS_TO_TICKS(10)) == pdTRUE)
        {
            switch (event)
            {
            case BUTTON_EVENT_SINGLE_CLICK:
                if (page_num < PAGE_COUNT - 1)
                    page_num++;
                break;

            case BUTTON_EVENT_DOUBLE_CLICK:
                if (page_num > 0)
                    page_num--;
                break;

            case BUTTON_EVENT_LONG_PRESS:
                page_num = 0;
                break;
            }

            page_manager_switch_to((page_id_t)page_num);
        }
        lv_task_handler();
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void bread_board_app_start(void)
{
    spiffs_init();
    lv_port_init();
    adapter_btn_init(button_callback);

    xTaskCreate(lvgl_task, "lvgl", 2048 * 2, NULL, 4, &lvgltask_handle);

    // while (1)
    // {
    //    show_png(file_path1);
    //    vTaskDelay(pdMS_TO_TICKS(5000));
    //    show_png(file_path2);
    //    vTaskDelay(pdMS_TO_TICKS(5000));
    // }

    // cst816s_cfg_t cfg = {
    //     .sda = I2C_SDA,
    //     .scl = I2C_SCL,
    //     .fre = 400000,
    //     .x_limit = LCD_WIDTH,
    //     .y_limit = LCD_HEIGHT
    // };
    // cst816s_init(&cfg);

    // while (1) {
    //     int16_t x, y;
    //     int state;
    //     cst816s_read(&x, &y, &state);
    //     if (state) {
    //         ESP_LOGI("TOUCH", "Touch: (%d, %d)", x, y);
    //         lcdDrawFillCircle(&dev, x, y, 5, RED);
    //     }
    //     vTaskDelay(pdMS_TO_TICKS(50));
    // }
}