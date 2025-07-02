#include "lvgl.h"
#include "esp_log.h"
#include "adapter_lcd.h"
#include <stdio.h>
#include "ui.h"

#include "bg3.c"

static lv_obj_t *s_page2 = NULL;

lv_obj_t *ui_page2_create(void)
{
    s_page2 = lv_obj_create(lv_scr_act());
    lv_obj_set_size(s_page2, LCD_WIDTH, LCD_HEIGHT);
    lv_obj_clear_flag(s_page2, LV_OBJ_FLAG_SCROLLABLE); 

    lv_obj_t *img = lv_img_create(s_page2);
    lv_img_set_src(img, &bg3);
    lv_obj_align(img, LV_ALIGN_CENTER, 0, 0);

    return s_page2;
}

lv_obj_t *ui_get_page2(void)
{
    return s_page2;
}