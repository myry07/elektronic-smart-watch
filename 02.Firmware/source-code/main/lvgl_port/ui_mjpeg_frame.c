#include "lvgl.h"
#include "esp_log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mjpeg_frame.h"
#include "img_converters.h"
#include "ui.h"

static const char *TAG = "ui_mjpeg";

// 播放定时器、暂停标志
static lv_timer_t *s_mjpeg_timer = NULL;
static bool s_mjpeg_pause = false;

// LVGL 图片控件对象（挂载在 video page 上）
lv_obj_t *s_img_obj = NULL;

// 解码后图像描述符
static lv_img_dsc_t s_img_dsc = {0};
static uint8_t *s_rgb565_buf = NULL;

extern void mjpeg_timer_cb(lv_timer_t *t);

// 获取 video page 图像控件对象（供外部设置）
lv_obj_t *ui_get_video_page_img_obj(void)
{
    return s_img_obj;
}

// 设置 video page 图像控件对象（ui_video_page.c 创建好后传入）
void ui_set_video_page_img_obj(lv_obj_t *obj)
{
    s_img_obj = obj;
}

// 播放暂停控制（供按钮调用）
void ui_toggle_mjpeg_pause(void)
{
    s_mjpeg_pause = !s_mjpeg_pause;
}

// 停止播放（页面退出时或播放结束）
void ui_stop_mjpeg_playback(void)
{
    if (s_mjpeg_timer)
    {
        lv_timer_del(s_mjpeg_timer);
        s_mjpeg_timer = NULL;
    }

    jpeg_frame_stop();

    if (s_rgb565_buf)
    {
        free(s_rgb565_buf);
        s_rgb565_buf = NULL;
    }

    if (s_img_dsc.data)
    {
        s_img_dsc.data = NULL;
        s_img_dsc.data_size = 0;
    }

    ESP_LOGI(TAG, "MJPEG playback stopped.");
}

bool ui_start_mjpeg_playback(const char *filename)
{
    if (!filename)
    {
        ESP_LOGE(TAG, "Can't start MJPEG: invalid state or already started");
        return false;
    }

    // 让 mjpeg_frame_start 做实际工作
    jpeg_frame_start(filename);

    s_mjpeg_pause = false;

    // 创建 LVGL 播放定时器
    if (s_mjpeg_timer)
    {
        lv_timer_del(s_mjpeg_timer);
    }
    s_mjpeg_timer = lv_timer_create(mjpeg_timer_cb, 80, NULL);

    ESP_LOGI(TAG, "MJPEG playback started: %s", filename);
    return true;
}

// MJPEG 播放定时器回调
void mjpeg_timer_cb(lv_timer_t *t)
{
    static jpeg_frame_data_t frame_data = {0};

    if (s_mjpeg_pause || s_img_obj == NULL)
        return;

    // 获取新的一帧（注意不要 free 旧的 frame_data.frame）
    jpeg_frame_get_one(&frame_data);

    if (frame_data.frame == NULL || frame_data.len < 4) {
        ESP_LOGW(TAG, "Received invalid frame");
        return;
    }

    ESP_LOGI(TAG, "frame ptr: %p, len: %d", frame_data.frame, frame_data.len);
    ESP_LOGI(TAG, "JPEG start: 0x%02X 0x%02X", frame_data.frame[0], frame_data.frame[1]);
    ESP_LOGI(TAG, "JPEG end:   0x%02X 0x%02X",
             frame_data.frame[frame_data.len - 2],
             frame_data.frame[frame_data.len - 1]);

    uint16_t width = 0, height = 0;

    if (s_rgb565_buf) {
        free(s_rgb565_buf);
        s_rgb565_buf = NULL;
    }

    if (!jpg2rgb565(frame_data.frame, frame_data.len, &s_rgb565_buf, &width, &height, JPG_SCALE_NONE)) {
        ESP_LOGE(TAG, "Failed to decode JPEG frame, len=%d", frame_data.len);
        free(frame_data.frame); 
        frame_data.frame = NULL;
        return;
    }

    free(frame_data.frame);
    frame_data.frame = NULL;

    s_img_dsc.header.cf = LV_IMG_CF_TRUE_COLOR;
    s_img_dsc.data = s_rgb565_buf;
    s_img_dsc.data_size = width * height * 2;
    s_img_dsc.header.w = width;
    s_img_dsc.header.h = height;

    lv_img_set_src(s_img_obj, &s_img_dsc);
}