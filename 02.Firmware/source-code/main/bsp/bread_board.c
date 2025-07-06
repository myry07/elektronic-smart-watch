#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include <errno.h>
#include <dirent.h>
#include <sys/unistd.h>

#include "ui.h"
#include "lvgl.h"

#include "adapter_btn.h"
#include "adapter_lcd.h"
#include "adapter_storage.h"
#include "adapter_i2s.h"

#include "lvgl_port.h"

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
    PAGE_ID_VIDEO,

    PAGE_COUNT
} page_id_t;

uint32_t page_num = 0;

const char *file_path1 = "S:/spiffs/hc1.png";
const char *file_path2 = "S:/spiffs/hc2.png";

const char *sd_path = "/sdcard/test.TXT";

const static char *TAG = "bread_board";

float scale_volum = 0.2f;

static void my_button_cb(button_event_t event)
{
    switch (event)
    {
    case BUTTON_EVENT_SINGLE_CLICK:
        ESP_LOGI(TAG, "Single Click");
        break;
    case BUTTON_EVENT_LONG_PRESS:
        ESP_LOGI(TAG, "Long Press");
        break;
    case BUTTON_EVENT_DOUBLE_CLICK:
        ESP_LOGI(TAG, "Double Click");
        break;
    }
}

static lv_obj_t *s_current_page = NULL;
static page_id_t s_current_page_id = PAGE_ID_LOCK; // 默认首页

void page_manager_switch_to(page_id_t id, lv_scr_load_anim_t anim_type)
{
    if (id == s_current_page_id)
        return;

    lv_obj_t *new_page = NULL;

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
    // case PAGE_ID_VIDEO:
    //     new_page = ui_mjpeg_minimal_create();
    //     break;
    default:
        return;
    }

    lv_scr_load_anim(new_page, anim_type, 50, 0, true);

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
    lv_scr_load_anim(ui_lock_page_create(), LV_SCR_LOAD_ANIM_NONE, 10, 0, true);

    uint32_t event;
    while (1)
    {
        if (xTaskNotifyWait(0, 0, &event, pdMS_TO_TICKS(10)) == pdTRUE)
        {
            lv_scr_load_anim_t anim = LV_SCR_LOAD_ANIM_MOVE_LEFT; // 默认

            switch (event)
            {
            case BUTTON_EVENT_SINGLE_CLICK:
                if (page_num < PAGE_COUNT - 1)
                {
                    page_num++;
                    anim = LV_SCR_LOAD_ANIM_MOVE_TOP;
                    // adapter_i2s_set_volume(scale_volum -= 0.1);
                }
                break;

            case BUTTON_EVENT_DOUBLE_CLICK:
                if (page_num > 0)
                {
                    page_num--;
                    anim = LV_SCR_LOAD_ANIM_MOVE_BOTTOM;
                    // adapter_i2s_set_volume(scale_volum += 0.1);
                }
                break;

            case BUTTON_EVENT_LONG_PRESS:
                page_num = 0;
                anim = LV_SCR_LOAD_ANIM_FADE_IN;
                break;
            }

            page_manager_switch_to((page_id_t)page_num, anim);
        }

        lv_task_handler();
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void sd_task(void *param)
{
    write_file(sd_path, "hello");
    adapter_i2s_start_player("/sdcard/test.pcm");

    adapter_i2s_start_speaker(NULL);

    vTaskDelete(NULL);
}


void bread_board_app_start(void)
{
    lv_port_init();

    adapter_spiffs_init();
    adapter_sd_init();
    adapter_i2s_spk_init();
    adapter_i2s_mic_init();
    adapter_btn_init(button_callback);

    xTaskCreate(lvgl_task, "lvgl", 2048 * 2, NULL, 4, &lvgltask_handle);
    xTaskCreate(sd_task, "sd", 2048 * 3, NULL, 2, NULL);

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