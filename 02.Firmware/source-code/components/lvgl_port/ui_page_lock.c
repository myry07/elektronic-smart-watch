#include "lvgl.h"
#include "esp_log.h"
#include "adapter_lcd.h"
#include <stdio.h>

#include "hc1.c"

static lv_obj_t *s_lock_page = NULL;
static lv_obj_t *s_time_label = NULL;

void ui_lock_page_create(void)
{
    s_lock_page = lv_obj_create(lv_scr_act()); // 创建页面容器
    lv_obj_set_size(s_lock_page, LCD_WIDTH, LCD_HEIGHT);
    // lv_obj_set_size(s_lock_page, lv_pct(100), lv_pct(100));
    lv_obj_set_style_bg_color(s_lock_page, lv_color_black(), 0); // 设置黑色背景
    lv_obj_set_style_bg_opa(s_lock_page, LV_OPA_COVER, 0);
    lv_obj_clear_flag(s_lock_page, LV_OBJ_FLAG_SCROLLABLE);      // 禁止滚动

    lv_obj_t *img = lv_img_create(s_lock_page);
    lv_img_set_src(img, &hc1); // 替换为你的 SPIFFS 或 C 图片资源
    lv_obj_align(img, LV_ALIGN_CENTER, 0, 0);

    // 创建时间显示标签
    s_time_label = lv_label_create(s_lock_page);
    lv_obj_set_style_text_color(s_time_label, lv_color_white(), 0);
    lv_obj_set_style_text_font(s_time_label, &lv_font_montserrat_40, 0); // 设置字体
    lv_obj_align(s_time_label, LV_ALIGN_TOP_LEFT, 0, 30);                // 顶部中间偏下

    // 初始时间
    lv_label_set_text(s_time_label, "09:00");

    lv_obj_t *bat_label = lv_label_create(s_lock_page);
    lv_label_set_text(bat_label, LV_SYMBOL_BATTERY_FULL);
    lv_obj_align(bat_label, LV_ALIGN_TOP_RIGHT, -5, 0);
}

lv_obj_t *ui_get_lock_page(void)
{
    return s_lock_page;
}