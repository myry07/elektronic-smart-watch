#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "driver/gpio.h"
#include "lvgl_port.h"
#include "lvgl.h"
// #include "st7789.h"
// #include "cst816t.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "adapter_lcd.h"
#include <stdio.h>
#include <inttypes.h>

static lv_disp_drv_t disp_drv;
static const char *TAG = "lv_port";

static void lv_tick_inc_cb(void *data)
{
    uint32_t tick_inc_period_ms = *((uint32_t *)data);
    lv_tick_inc(tick_inc_period_ms);
}

static void lv_port_flush_ready(void *param)
{
    lv_disp_flush_ready(&disp_drv);
    // lv_disp_flush_ready((lv_disp_drv_t *)param);
    /* portYIELD_FROM_ISR (true) or not (false). */
}

static void disp_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p)
{
    lcdDrawBitmap(&dev,
                  area->x1, area->y1,
                  area->x2, area->y2,
                  (const uint16_t *)color_p);

    lv_disp_flush_ready(disp_drv);
}

static void lv_port_disp_init(void)
{
    static lv_disp_draw_buf_t draw_buf_dsc;
    size_t disp_buf_height = 80;

    /* 必须从内部RAM分配显存，这样刷新速度快 */
    lv_color_t *p_disp_buf1 = heap_caps_malloc(LCD_WIDTH * disp_buf_height * sizeof(lv_color_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA);
    lv_color_t *p_disp_buf2 = heap_caps_malloc(LCD_WIDTH * disp_buf_height * sizeof(lv_color_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA);
    ESP_LOGI(TAG, "Try allocate two %u * %u display buffer, size:%u Byte", LCD_WIDTH, disp_buf_height, LCD_WIDTH * disp_buf_height * sizeof(lv_color_t) * 2);
    if (NULL == p_disp_buf1 || NULL == p_disp_buf2)
    {
        ESP_LOGE(TAG, "No memory for LVGL display buffer");
        esp_system_abort("Memory allocation failed");
    }

    /* 初始化显示缓存 */
    lv_disp_draw_buf_init(&draw_buf_dsc, p_disp_buf1, p_disp_buf2, LCD_WIDTH * disp_buf_height);

    /* 初始化显示驱动 */
    lv_disp_drv_init(&disp_drv);

    /*设置水平和垂直宽度*/
    disp_drv.hor_res = LCD_WIDTH;  // 水平宽度
    disp_drv.ver_res = LCD_HEIGHT; // 垂直宽度

    /* 设置刷新数据函数 */
    disp_drv.flush_cb = disp_flush;

    /*设置显示缓存*/
    disp_drv.draw_buf = &draw_buf_dsc;

    /*注册显示驱动*/
    lv_disp_drv_register(&disp_drv);
}

void IRAM_ATTR indev_read(struct _lv_indev_drv_t *indev_drv, lv_indev_data_t *data)
{
    // int16_t x, y;
    // int state;
    // cst816t_read(&x, &y, &state);
    // data->point.x = y;
    // if (x == 0)
    //     x = 1;
    // data->point.y = LCD_HEIGHT - x;
    // data->state = state;
}

static esp_err_t lv_port_indev_init(void)
{
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = indev_read;
    lv_indev_drv_register(&indev_drv);

    return ESP_OK;
}

static esp_err_t lv_port_tick_init(void)
{
    static uint32_t tick_inc_period_ms = 5;
    const esp_timer_create_args_t periodic_timer_args = {
        .callback = lv_tick_inc_cb,
        .name = "",
        .arg = &tick_inc_period_ms,
        .dispatch_method = ESP_TIMER_TASK,
        .skip_unhandled_events = true,
    };

    esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, tick_inc_period_ms * 1000));

    return ESP_OK;
}

static void *spiffs_open_cb(lv_fs_drv_t *drv, const char *path, lv_fs_mode_t mode)
{
    char full_path[128];
    snprintf(full_path, sizeof(full_path), "/spiffs/%s", path);

    const char *flags = (mode == LV_FS_MODE_WR) ? "wb" : "rb";

    ESP_LOGI(TAG, "Try to open file: %s (mode: %s)", full_path, flags);

    FILE *f = fopen(full_path, flags);
    if (!f)
    {
        ESP_LOGE(TAG, "Failed to open file: %s", full_path);
        return NULL;
    }

    ESP_LOGI(TAG, "File opened: %s", full_path);
    return f;
}

static lv_fs_res_t spiffs_read_cb(lv_fs_drv_t *drv, void *file_p, void *buf, uint32_t btr, uint32_t *br)
{
    FILE *f = (FILE *)file_p;
    if (!f)
    {
        ESP_LOGE(TAG, "Read failed: invalid file pointer");
        return LV_FS_RES_UNKNOWN;
    }

    *br = fread(buf, 1, btr, f);
    ESP_LOGD(TAG, "Read %" PRIu32 " bytes", *br);
    return LV_FS_RES_OK;
}

static lv_fs_res_t spiffs_close_cb(lv_fs_drv_t *drv, void *file_p)
{
    FILE *f = (FILE *)file_p;
    fclose(f);
    return LV_FS_RES_OK;
}

static lv_fs_res_t spiffs_seek_cb(lv_fs_drv_t *drv, void *file_p, uint32_t pos, lv_fs_whence_t whence)
{
    FILE *f = (FILE *)file_p;
    fseek(f, pos, SEEK_SET);
    return LV_FS_RES_OK;
}

static lv_fs_res_t spiffs_tell_cb(lv_fs_drv_t *drv, void *file_p, uint32_t *pos_p)
{
    FILE *f = (FILE *)file_p;
    *pos_p = ftell(f);
    return LV_FS_RES_OK;
}

void lvgl_spiffs_fs_init(void)
{
    lv_fs_drv_t drv;
    lv_fs_drv_init(&drv);
    drv.letter = 'S';
    drv.open_cb = spiffs_open_cb;
    drv.read_cb = spiffs_read_cb;
    drv.close_cb = spiffs_close_cb;
    drv.seek_cb = spiffs_seek_cb;
    drv.tell_cb = spiffs_tell_cb;

    lv_fs_drv_register(&drv);
}

esp_err_t lv_port_init(void)
{
    /* 初始化LVGL库 */
    lv_init();

    /*lcd接口初始化*/
    adapter_lcd1_init();

    /* 注册显示驱动 */
    lv_port_disp_init();

    /*触摸芯片初始化*/
    // tp_init();

    // lvgl_spiffs_fs_init();
    lv_png_init();

    /* 注册输入驱动*/
    lv_port_indev_init();

    /* 初始化LVGL定时器 */
    lv_port_tick_init();

    return ESP_OK;
}