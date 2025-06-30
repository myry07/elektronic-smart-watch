#include "lvgl.h"
#include "esp_log.h"
#include "driver/gpio.h"

static lv_obj_t *s_led_button;

static lv_obj_t *s_led_lable;

static lv_obj_t *img1;

void ui_led_create(void)
{
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_black(), 0); // 背景颜色

    s_led_button = lv_btn_create(lv_scr_act());                                     // btn父类是屏幕
    lv_obj_set_size(s_led_button, 50, 20);                                          // 设置按钮大小
    lv_obj_align(s_led_button, LV_ALIGN_CENTER, 0, 0);                              // 居中
    lv_obj_set_style_bg_color(s_led_button, lv_palette_main(LV_PALETTE_YELLOW), 0); // btn颜色

    s_led_lable = lv_label_create(s_led_button); // lable父类是btn
    lv_label_set_text(s_led_lable, "button");    // 设置标签内容
    lv_obj_center(s_led_lable);                  // 居中标签
}

void ui_img_create(const void * src)
{
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_black(), 0); // 背景颜色

    if (img1 == NULL) {
        img1 = lv_img_create(lv_scr_act());
        lv_obj_align(img1, LV_ALIGN_CENTER, 0, 0);
    }

    lv_img_set_src(img1, src);                  // 设置图片源
}