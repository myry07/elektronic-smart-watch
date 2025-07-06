#include "lvgl.h"
#include "esp_log.h"
#include "adapter_lcd.h"
#include <stdio.h>
#include "ui.h"

#include "hc2.c"
// #include "hc3.c"
#include "cute1.c"

static lv_obj_t *s_home_page = NULL;

lv_obj_t *ui_home_page_create(void)
{
    // 创建主页对象
    s_home_page = lv_obj_create(NULL);
    lv_obj_set_size(s_home_page, LCD_WIDTH, LCD_HEIGHT);
    lv_obj_clear_flag(s_home_page, LV_OBJ_FLAG_SCROLLABLE); 

    lv_obj_t *img = lv_img_create(s_home_page);
    // lv_img_set_src(img, &hc2);
    lv_img_set_src(img, &cute1);
    lv_obj_align(img, LV_ALIGN_CENTER, 0, 0);

    return s_home_page;
}

lv_obj_t *ui_get_home_page(void)
{
    return s_home_page;
}