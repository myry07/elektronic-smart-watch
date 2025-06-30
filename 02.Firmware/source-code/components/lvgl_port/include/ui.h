#ifndef _UI_LED_H_
#define _UI_LED_H_

#include "lvgl.h"


void ui_led_create(void);
void ui_img_create(const void * src);

void ui_lock_page_create(void);
lv_obj_t *ui_get_lock_page(void);

void ui_home_page_create(void);

#endif