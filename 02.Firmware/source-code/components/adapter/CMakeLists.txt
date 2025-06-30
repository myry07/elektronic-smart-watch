idf_component_register(SRCS "adapter_lcd.c" "adapter_btn.c"
                    INCLUDE_DIRS "include"
                    REQUIRES 
                    st7789 
                    cst816t 
                    esp_lcd 
                    cst816s 
                    spiffs 
                    decode 
                    lvgl
                    freertos
                    esp_timer
                    application)
