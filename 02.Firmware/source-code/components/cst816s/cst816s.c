#include "cst816s.h"
#include "driver/i2c.h"
#include "esp_log.h"

#define TAG "cst816s"
#define I2C_PORT I2C_NUM_0
#define CST816S_ADDR 0x15

static uint16_t limit_x = 0;
static uint16_t limit_y = 0;

static esp_err_t i2c_read_bytes(uint8_t reg, uint8_t *data, size_t len);

esp_err_t cst816s_init(cst816s_cfg_t *cfg)
{
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = cfg->sda,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = cfg->scl,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = cfg->fre,
    };
    limit_x = cfg->x_limit;
    limit_y = cfg->y_limit;

    ESP_ERROR_CHECK(i2c_param_config(I2C_PORT, &conf));
    ESP_ERROR_CHECK(i2c_driver_install(I2C_PORT, conf.mode, 0, 0, 0));

    uint8_t id;
    i2c_read_bytes(0xA7, &id, 1);
    ESP_LOGI(TAG, "Chip ID: 0x%02X", id);

    i2c_read_bytes(0xA9, &id, 1);
    ESP_LOGI(TAG, "Firmware Version: 0x%02X", id);

    i2c_read_bytes(0xAA, &id, 1);
    ESP_LOGI(TAG, "Vendor ID: 0x%02X", id);

    return ESP_OK;
}

void cst816s_read(int16_t *x, int16_t *y, int *state)
{
    uint8_t buf[7];
    static int16_t last_x = 0;
    static int16_t last_y = 0;

    if (i2c_read_bytes(0x00, buf, sizeof(buf)) != ESP_OK) {
        *x = last_x;
        *y = last_y;
        *state = 0;
        return;
    }

    uint8_t finger = buf[2] & 0x0F;
    if (finger == 0) {
        *x = last_x;
        *y = last_y;
        *state = 0;
        return;
    }

    uint16_t x_pos = ((buf[3] & 0x0F) << 8) | buf[4];
    uint16_t y_pos = ((buf[5] & 0x0F) << 8) | buf[6];

    if (x_pos >= limit_x) x_pos = limit_x - 1;
    if (y_pos >= limit_y) y_pos = limit_y - 1;

    last_x = x_pos;
    last_y = y_pos;

    *x = x_pos;
    *y = y_pos;
    *state = 1;

    ESP_LOGI("CST816S", "Touch detected: x=%d y=%d", *x, *y);
}

static esp_err_t i2c_read_bytes(uint8_t reg, uint8_t *data, size_t len)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    if (!cmd) return ESP_FAIL;

    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (CST816S_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg, I2C_MASTER_ACK);

    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (CST816S_ADDR << 1) | I2C_MASTER_READ, true);

    for (size_t i = 0; i < len; i++) {
        i2c_master_read_byte(cmd, &data[i], i == len - 1 ? I2C_MASTER_NACK : I2C_MASTER_ACK);
    }

    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_PORT, cmd, pdMS_TO_TICKS(100));
    i2c_cmd_link_delete(cmd);
    return ret;
}