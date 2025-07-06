#include "lvgl.h"
#include "esp_log.h"
#include "adapter_lcd.h"
#include <stdio.h>
#include "ui.h"

#include "bg2.c"

static lv_obj_t *s_page1 = NULL;

lv_obj_t *ui_page1_create(void)
{
    s_page1 = lv_obj_create(NULL);
    lv_obj_set_size(s_page1, LCD_WIDTH, LCD_HEIGHT);
    lv_obj_clear_flag(s_page1, LV_OBJ_FLAG_SCROLLABLE); 

    lv_obj_t *img = lv_img_create(s_page1);
    lv_img_set_src(img, &bg2);
    lv_obj_align(img, LV_ALIGN_CENTER, 0, 0);

    return s_page1;
}

lv_obj_t *ui_get_page1(void)
{
    return s_page1;
}