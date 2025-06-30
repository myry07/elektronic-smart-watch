#include <stdio.h>
#include "adapter_lcd.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lvgl_port.h"
#include "ui.h"
#include "lvgl.h"
#include "adapter_btn.h"

#define I2C_SDA 5
#define I2C_SCL 4
#define TOUCH_RST 15

const char *file_path1 = "S:/spiffs/hc1.png";
const char *file_path2 = "S:/spiffs/hc2.png";

void png_task(void *param)
{
    ui_lock_page_create();
    vTaskDelay(pdMS_TO_TICKS(5000));
    ui_home_page_create();
    while (1)
    {
        
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void app_main(void)
{
    spiffs_init();
    lv_port_init();
    // adpater_btn_init();

    xTaskCreate(png_task, "png", 2048, NULL, 3, NULL);

    while (1)
    {
        lv_task_handler(); // LVGL循环处理
        vTaskDelay(pdMS_TO_TICKS(10));
    }

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