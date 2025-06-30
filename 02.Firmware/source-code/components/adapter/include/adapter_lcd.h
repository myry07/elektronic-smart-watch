#ifndef ADAPTER_LCD_H_
#define ADAPTER_LCD_H_

#include "st7789.h"


#include "config.h"

// #define LCD_WIDTH 240
// #define LCD_HEIGHT 280
// #define SPI_MOSI_GPIO 47
// #define SPI_SCLK_GPIO 21
// #define SPI_CS_GPIO 14
// #define LCD_DC_GPIO 13
// #define LCD_RESET_GPIO 12
// #define LCD_BL_GPIO 1

extern TFT_t dev;

void spiffs_init(void);
void adapter_lcd1_init(void);
void show_png(const char *file_path);

#endif