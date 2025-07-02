#ifndef _UI_LED_H_
#define _UI_LED_H_

#include "lvgl.h"


void ui_led_create(void);
void ui_img_create(const void * src);

lv_obj_t *ui_lock_page_create(void);
lv_obj_t *ui_get_lock_page(void);

lv_obj_t *ui_home_page_create(void);
lv_obj_t *ui_get_home_page(void);

lv_obj_t *ui_page1_create(void);
lv_obj_t *ui_get_page1(void);

lv_obj_t *ui_page2_create(void);
lv_obj_t *ui_get_page2(void);

lv_obj_t *ui_page3_create(void);
lv_obj_t *ui_get_page3(void);
#endif