#include <stdio.h>
#include "adapter_lcd.h"
#include "st7789.h"
#include "cst816s.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_spiffs.h"
#include <string.h>
#include "decode_jpeg.h"
#include "decode_png.h"
#include "pngle.h"

static const char *TAG = "lcd2";
TFT_t dev;

void spiffs_init(void)
{
    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",
        .partition_label = NULL,
        .max_files = 5,
        .format_if_mount_failed = true};

    esp_err_t ret = esp_vfs_spiffs_register(&conf);
    if (ret != ESP_OK)
    {
        ESP_LOGE("SPIFFS", "Failed to mount or format filesystem (%s)", esp_err_to_name(ret));
    }
    else
    {
        size_t total = 0, used = 0;
        esp_spiffs_info(NULL, &total, &used);
        ESP_LOGI("SPIFFS", "Partition size: total: %d, used: %d", total, used);
    }
}

void adapter_lcd1_init(void)
{
    spi_master_init(&dev, SPI_MOSI_GPIO, SPI_SCLK_GPIO,
                    SPI_CS_GPIO, LCD_DC_GPIO, LCD_RESET_GPIO, LCD_BL_GPIO);
    lcdInit(&dev, LCD_WIDTH, LCD_HEIGHT, 0, 20, DIRECTION0);
    lcdFillScreen(&dev, BLACK);

    ESP_LOGI(TAG, "adapter lcd1 inited");
}


void show_png(const char *file_path) {

    FILE *fp = fopen(file_path, "rb");
    if (!fp) {
        ESP_LOGE(TAG, "Failed to open PNG file: %s", file_path);
        return;
    }

    lcdFillScreen(&dev, BLACK);
    pngle_t *pngle = pngle_new(LCD_WIDTH, LCD_HEIGHT);
    if (!pngle) {
        ESP_LOGE(TAG, "Failed to create pngle");
        fclose(fp);
        return;
    }

    pngle_set_init_callback(pngle, png_init);
    pngle_set_draw_callback(pngle, png_draw);
    pngle_set_done_callback(pngle, png_finish);

    uint8_t buf[1024];
    size_t remain = 0;
    while (!feof(fp)) {
        size_t len = fread(buf + remain, 1, sizeof(buf) - remain, fp);
        if (len <= 0) break;

        int fed = pngle_feed(pngle, buf, remain + len);
        if (fed < 0) {
            ESP_LOGE(TAG, "pngle_feed failed: %s", pngle_error(pngle));
            fclose(fp);
            pngle_destroy(pngle, LCD_WIDTH, LCD_HEIGHT);
            return;
        }

        remain = remain + len - fed;
        if (remain > 0) memmove(buf, buf + fed, remain);
    }
    fclose(fp);

    // 居中
    uint16_t x_offset = 0, y_offset = 0;
    if (LCD_WIDTH > pngle->imageWidth)
        x_offset = (LCD_WIDTH - pngle->imageWidth) / 2;
    if (LCD_HEIGHT > pngle->imageHeight)
        y_offset = (LCD_HEIGHT - pngle->imageHeight) / 2;

    // 每行绘制
    for (uint16_t y = 0; y < pngle->imageHeight; y++) {
        lcdDrawMultiPixels(
            &dev,                  // 替换成你的 TFT_t * 或 dev 实例
            x_offset, y + y_offset,       // 屏幕位置
            pngle->imageWidth,            // 每行长度
            pngle->pixels[y]              // RGB565 数据指针
        );
    }

    pngle_destroy(pngle, LCD_WIDTH, LCD_HEIGHT);
    ESP_LOGI(TAG, "PNG display complete.");
}