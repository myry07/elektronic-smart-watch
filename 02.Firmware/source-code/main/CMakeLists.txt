idf_component_register(SRCS "camera-code.c" 

                    "bsp/bread_board.c" 

                    "hal/adapter_btn.c" "hal/adapter_lcd.c" "hal/adapter_storage.c"
                    "hal/adapter_i2s.c"


                    "lvgl_port/lvgl_port.c" "lvgl_port/ui_img_btn.c"
                    "lvgl_port/ui_page_home.c" "lvgl_port/ui_page_lock.c"
                    "lvgl_port/ui_page_p1.c" "lvgl_port/ui_page_p2.c" 
                    "lvgl_port/ui_page_p3.c" "lvgl_port/ui_video_page.c"
                    "lvgl_port/ui_mjpeg_frame.c"

                    INCLUDE_DIRS "." "bsp/include" "hal/include" "lvgl_port/include"

                    
                    PRIV_REQUIRES 
                    st7789 
                    cst816t 
                    esp_lcd 
                    cst816s 
                    spiffs
                    esp_timer
                    driver
                    decode
                    lvgl
                    fatfs)

