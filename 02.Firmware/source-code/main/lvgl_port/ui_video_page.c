#include "lvgl.h"

#include "mjpeg_frame.h"
#include "img_converters.h"
#include "esp_log.h"
#include "adapter_lcd.h"

#define TAG "mini_jpeg"

static lv_obj_t *player_img = NULL;
static lv_timer_t *player_timer = NULL;
static bool pause_flag = false;
static lv_obj_t *s_video_page = NULL;

#include "lvgl.h"
#include "img_converters.h"
#include "esp_log.h"

#define TAG "mini_jpeg"

void ui_mjpeg_minimal_create(void)
{
    lv_fs_stdio_init(); // 注册 stdio 文件系统，必须调用

    s_video_page = lv_obj_create(NULL);
    lv_obj_set_size(s_video_page, LCD_WIDTH, LCD_HEIGHT);
    lv_obj_set_style_bg_opa(s_video_page, LV_OPA_COVER, 0);
    lv_obj_clear_flag(s_video_page, LV_OBJ_FLAG_SCROLLABLE);
    lv_scr_load(s_video_page);  

    player_img = lv_img_create(s_video_page);

    const char *filepath = "S:/sdcard/test.jpg";      // 注意这里带 S:
    FILE *f = fopen("/sdcard/test.jpg", "rb"); // 原始 fopen 检查文件
    if (!f)
    {
        ESP_LOGE(TAG, "Failed to open file: /sdcard/frames/frame_0001.jpg");
        return;
    }
    else
    {
        ESP_LOGI(TAG, "打开成功: %s", filepath);
        fclose(f); // 关闭文件
    }

    lv_img_header_t header;
    lv_res_t res = lv_img_decoder_get_info(filepath, &header);
    if (res != LV_RES_OK)
    {
        ESP_LOGE(TAG, "LVGL cannot decode this JPG!");
    }
    else
    {
        ESP_LOGI(TAG, "Image width: %d, height: %d", header.w, header.h);
    }

    lv_img_set_src(player_img, "S:/sdcard/test.jpg");
    lv_obj_align(player_img, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t *bat_label = lv_label_create(s_video_page);
    lv_label_set_text(bat_label, LV_SYMBOL_BATTERY_FULL);
    lv_obj_align(bat_label, LV_ALIGN_TOP_RIGHT, -100, 0);

    // fseek(f, 0, SEEK_END);
    // size_t len = ftell(f);
    // rewind(f);

    // uint8_t *buf = malloc(len);
    // if (!buf)
    // {
    //     fclose(f);
    //     ESP_LOGE(TAG, "malloc failed");
    //     return;
    // }

    // fread(buf, 1, len, f);
    // fclose(f);

    // if (len < 2 || buf[0] != 0xFF || buf[1] != 0xD8)
    // {
    //     ESP_LOGE(TAG, "Invalid JPEG header");
    //     free(buf);
    //     return;
    // }

    // uint8_t *rgb565 = NULL;
    // uint16_t w = 0, h = 0;
    // if (jpg2rgb565(buf, len, &rgb565, &w, &h, JPG_SCALE_NONE))
    // {
    //     static lv_img_dsc_t img_dsc;
    //     memset(&img_dsc, 0, sizeof(img_dsc));
    //     img_dsc.header.cf = LV_IMG_CF_TRUE_COLOR;
    //     img_dsc.header.w = w;
    //     img_dsc.header.h = h;
    //     img_dsc.data = rgb565;
    //     img_dsc.data_size = w * h * 2;

    //     lv_img_set_src(player_img, &img_dsc);
    //     ESP_LOGI(TAG, "JPEG displayed: %dx%d", w, h);
    // }
    // else
    // {
    //     ESP_LOGE(TAG, "JPEG decode failed");
    //     free(rgb565);
    // }

    // free(buf);
}

// void mini_player_timer_cb(struct _lv_timer_t *t)
// {
//     static jpeg_frame_data_t frame_data = {0};

//     if (frame_data.frame) {
//         free(frame_data.frame);
//         frame_data.frame = NULL;
//         frame_data.len = 0;
//     }

//     if (pause_flag) return;

//     jpeg_frame_get_one(&frame_data);

//     if (frame_data.len) {
//         static lv_img_dsc_t img_dsc;
//         static uint8_t *rgb565_data = NULL;
//         uint16_t width = 0, height = 0;

//         if (rgb565_data) {
//             free(rgb565_data);
//             rgb565_data = NULL;
//         }

//         if (jpg2rgb565(frame_data.frame, frame_data.len, &rgb565_data, &width, &height, JPG_SCALE_NONE)) {
//             memset(&img_dsc, 0, sizeof(img_dsc));
//             img_dsc.header.cf = LV_IMG_CF_TRUE_COLOR;
//             img_dsc.header.w = width;
//             img_dsc.header.h = height;
//             img_dsc.data_size = width * height * 2;
//             img_dsc.data = rgb565_data;

//             lv_img_set_src(player_img, &img_dsc);
//         }
//     } else {
//         ESP_LOGI(TAG, "Playback finished");
//         lv_timer_del(player_timer);
//         player_timer = NULL;
//     }
// }
