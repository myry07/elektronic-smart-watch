#include "lvgl.h"
#include "esp_log.h"
#include "adapter_lcd.h"
#include <stdio.h>
#include "ui.h"

#include "hc2.c"
#include "hc3.c"

static lv_obj_t *s_home_page = NULL;
static lv_obj_t *s_lock_page = NULL;

void ui_home_page_create(void)
{
    // 获取锁屏页面
    s_lock_page = ui_get_lock_page();

    // 如果锁屏页面已存在 则删除并置空
    if (s_lock_page && lv_obj_is_valid(s_lock_page))
    {
        lv_obj_del(s_lock_page);
        s_lock_page = NULL;
    }

    // 创建主页对象
    s_home_page = lv_obj_create(lv_scr_act());
    lv_obj_set_size(s_home_page, LCD_WIDTH, LCD_HEIGHT);
    lv_obj_set_style_bg_color(s_home_page, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(s_home_page, LV_OPA_COVER, 0);

    lv_obj_t *img = lv_img_create(s_home_page);
    // lv_img_set_src(img, "S:/hc2.png");
    lv_img_set_src(img, &hc2);
    lv_obj_align(img, LV_ALIGN_CENTER, 0, 0);
}

lv_obj_t *ui_get_home_page(void)
{
    return s_home_page;
}