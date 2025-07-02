#ifndef CONFIG_H_
#define CONFIG_H_

#define LCD_WIDTH 240
#define LCD_HEIGHT 198
#define SPI_MOSI_GPIO 10
#define SPI_SCLK_GPIO 9
#define SPI_CS_GPIO 13
#define LCD_DC_GPIO 12
#define LCD_RESET_GPIO 11 
#define LCD_BL_GPIO 14

#define BUTTON_NUM 0

void bread_board_app_start(void);


#endif