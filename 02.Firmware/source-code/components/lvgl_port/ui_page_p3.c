#include "lvgl.h"
#include "esp_log.h"
#include "adapter_lcd.h"
#include <stdio.h>
#include "ui.h"

#include "bg4.c"

static lv_obj_t *s_page3 = NULL;

lv_obj_t *ui_page3_create(void)
{
    s_page3 = lv_obj_create(lv_scr_act());
    lv_obj_set_size(s_page3, LCD_WIDTH, LCD_HEIGHT);
    lv_obj_clear_flag(s_page3, LV_OBJ_FLAG_SCROLLABLE); 

    lv_obj_t *img = lv_img_create(s_page3);
    lv_img_set_src(img, &bg4);
    lv_obj_align(img, LV_ALIGN_CENTER, 0, 0);

    return s_page3;
}

lv_obj_t *ui_get_page3(void)
{
    return s_page3;
}