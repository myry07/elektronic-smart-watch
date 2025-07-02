#include "lvgl.h"
#include "esp_log.h"
#include "adapter_lcd.h"
#include <stdio.h>

#include "hc1.c"
#include "bg1.c"

static lv_obj_t *s_lock_page = NULL;
static lv_obj_t *s_time_label = NULL;

lv_obj_t *ui_lock_page_create(void)
{
    s_lock_page = lv_obj_create(lv_scr_act());
    lv_obj_set_size(s_lock_page, LCD_WIDTH, LCD_HEIGHT);
    lv_obj_set_style_bg_opa(s_lock_page, LV_OPA_COVER, 0);
    lv_obj_clear_flag(s_lock_page, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *img = lv_img_create(s_lock_page);
    lv_img_set_src(img, &bg1);
    lv_obj_align(img, LV_ALIGN_CENTER, 0, 0);

    s_time_label = lv_label_create(s_lock_page);
    lv_obj_set_style_text_color(s_time_label, lv_color_black(), 0);
    lv_obj_set_style_text_font(s_time_label, &lv_font_montserrat_40, 0);
    lv_obj_align(s_time_label, LV_ALIGN_TOP_LEFT, 0, 30);
    lv_label_set_text(s_time_label, "09:00");

    lv_obj_t *bat_label = lv_label_create(s_lock_page);
    lv_label_set_text(bat_label, LV_SYMBOL_BATTERY_FULL);
    lv_obj_align(bat_label, LV_ALIGN_TOP_RIGHT, -5, 0);

    return s_lock_page;
}

lv_obj_t *ui_get_lock_page(void)
{
    return s_lock_page;
}