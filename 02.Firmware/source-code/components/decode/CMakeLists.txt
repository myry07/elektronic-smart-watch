idf_component_register(SRCS "jpeg_decode.c" "pngle.c" "decode_jpeg.c" "decode_jpeg.c" "decode_png.c" 
                            "mjpeg_frame.c" "esp_jpg_decode.c" "to_bmp.c"

                    INCLUDE_DIRS "include"
                    PRIV_REQUIRES esp_driver_jpeg esp_pm hal esp_mm)
