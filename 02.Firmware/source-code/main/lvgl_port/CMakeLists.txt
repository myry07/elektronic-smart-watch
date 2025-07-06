idf_component_register(SRCS "ui_page_p1.c" 
                    "ui_page_p3.c"
                    "ui_page_p2.c" 
                    "lvgl_port.c" 
                    "ui_img_btn.c" 
                    "ui_page_lock.c"
                    "ui_page_home.c"
                    INCLUDE_DIRS "include"
                    PRIV_REQUIRES 
                    st7789 
                    cst816t 
                    esp_lcd 
                    cst816s 
                    decode
                    lvgl_port
                    driver
                    lvgl
                    esp_timer)
