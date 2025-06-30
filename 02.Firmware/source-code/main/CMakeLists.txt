idf_component_register(SRCS "camera-code.c"
                    INCLUDE_DIRS "."
                    PRIV_REQUIRES 
                    st7789 
                    cst816t 
                    esp_lcd 
                    cst816s 
                    adapter
                    decode
                    lvgl_port
                    lvgl
                    application)

