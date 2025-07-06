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

#define SD_CMD 38
#define SD_CLK 39
#define SD_D0 40
#define SD_D1 -1
#define SD_D2 -1
#define SD_D3 -1

#define I2S_SPK_BCLK 2
#define I2S_SPK_LRCK 1
#define I2S_SPK_DOUT 42

#define I2S_MIC_WS   21
#define I2S_MIC_SCLK 20
#define I2S_MIC_DIN  19

void bread_board_app_start(void);


#endif