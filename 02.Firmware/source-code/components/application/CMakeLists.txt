idf_component_register(SRCS "bread_board.c" "application.c"
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
                    lvgl_port
                    freertos
                    application
                    adapter
                    )
