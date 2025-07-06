#include "driver/i2s_std.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <string.h>

#include "adapter_i2s.h"

static const char *TAG = "adapter_i2s";

static i2s_chan_handle_t tx_handle = NULL;
static i2s_chan_handle_t rx_handle = NULL;

static TaskHandle_t s_i2s_player_task_handle = NULL;
static TaskHandle_t s_i2s_mic_task_handle = NULL;

static float g_volume_scale = 0.1f; // 默认音量

void adapter_i2s_set_volume(float scale)
{
    if (scale < 0.0f)
        scale = 0.0f;
    if (scale > 1.0f)
        scale = 1.0f;
    g_volume_scale = scale;
    ESP_LOGI(TAG, "Volume set to %.2f", g_volume_scale);
}

static void i2s_player_task(void *param)
{
    const char *path = (const char *)param;
    FILE *f = fopen(path, "rb");
    if (!f)
    {
        ESP_LOGE(TAG, "Failed to open file: %s", path);
        vTaskDelete(NULL);
    }

    uint8_t buffer[1024];
    int16_t *pcm = (int16_t *)buffer;

    size_t bytes_read, bytes_written;

    while ((bytes_read = fread(buffer, 1, sizeof(buffer), f)) > 0)
    {
        size_t samples = bytes_read / sizeof(int16_t);
        for (size_t i = 0; i < samples; i++)
        {
            float volume = g_volume_scale;
            int32_t sample = pcm[i];
            sample = (int32_t)(sample * volume);
            if (sample > INT16_MAX)
                sample = INT16_MAX;
            if (sample < INT16_MIN)
                sample = INT16_MIN;
            pcm[i] = (int16_t)sample;
        }

        i2s_channel_write(tx_handle, buffer, bytes_read, &bytes_written, pdMS_TO_TICKS(1000));
    }

    fclose(f);
    vTaskDelete(NULL);
}

static void i2s_mic_task(void *arg)
{
    int32_t buffer[256];
    size_t bytes_read;

    while (1)
    {
        esp_err_t ret = i2s_channel_read(rx_handle, buffer, sizeof(buffer), &bytes_read, portMAX_DELAY);
        if (ret == ESP_OK && bytes_read > 0)
        {
            int samples = bytes_read / sizeof(int32_t);
            for (int i = 0; i < samples; ++i)
            {
                // 将数值范围控制在短整型范围
                int16_t sample = buffer[i] >> 13; // 简单右移压缩振幅
                printf("PCM: %d\n", sample);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

esp_err_t adapter_i2s_start_player(const char *path)
{
    if (s_i2s_player_task_handle)
    {
        ESP_LOGW(TAG, "Playback already running");
        return ESP_FAIL;
    }

    char *task_path = strdup(path);
    if (!task_path)
        return ESP_ERR_NO_MEM;

    return xTaskCreate(i2s_player_task, "i2s_player", 4096, task_path, 4, &s_i2s_player_task_handle) == pdPASS
               ? ESP_OK
               : ESP_FAIL;
}

esp_err_t adapter_i2s_start_speaker(void *arg)
{
    if (s_i2s_mic_task_handle)
    {
        ESP_LOGW(TAG, "Speaker task already running");
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Starting I2S speaker task");

    BaseType_t ret = xTaskCreate(i2s_mic_task, "i2s_speaker", 4096, arg, 4, &s_i2s_mic_task_handle);
    if (ret != pdPASS)
    {
        ESP_LOGE(TAG, "Failed to create speaker task");
        return ESP_FAIL;
    }

    return ESP_OK;
}

void adapter_i2s_spk_init(void)
{
    esp_err_t ret;

    // adapter_i2s_spk_init 用 I2S_NUM_0
    i2s_chan_config_t chan_cfg_spk = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_0, I2S_ROLE_MASTER);
    ret = i2s_new_channel(&chan_cfg_spk, &tx_handle, NULL);

    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to create I2S channel: %s", esp_err_to_name(ret));
        return;
    }

    i2s_std_config_t std_cfg = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(44100),
        .slot_cfg = I2S_STD_MSB_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_MONO),
        .gpio_cfg = {
            .mclk = I2S_GPIO_UNUSED,
            .bclk = I2S_SPK_BCLK,
            .ws = I2S_SPK_LRCK,
            .dout = I2S_SPK_DOUT,
            .din = -1,
            .invert_flags = {
                .mclk_inv = false,
                .bclk_inv = false,
                .ws_inv = false,
            },
        },
    };

    ret = i2s_channel_init_std_mode(tx_handle, &std_cfg);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to initialize I2S standard mode: %s", esp_err_to_name(ret));
        return;
    }

    ret = i2s_channel_enable(tx_handle);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to enable I2S channel: %s", esp_err_to_name(ret));
        return;
    }

    ESP_LOGI(TAG, "I2S TX channel initialized successfully");
}

void adapter_i2s_mic_init(void)
{
    esp_err_t ret;

    // adapter_i2s_mic_init 用 I2S_NUM_1
    i2s_chan_config_t chan_cfg_mic = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_1, I2S_ROLE_MASTER);
    ret = i2s_new_channel(&chan_cfg_mic, NULL, &rx_handle);

    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to create I2S RX channel: %s", esp_err_to_name(ret));
        return;
    }

    i2s_std_config_t std_cfg = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(16000),
        .slot_cfg = I2S_STD_MSB_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_32BIT, I2S_SLOT_MODE_MONO),
        .gpio_cfg = {
            .mclk = I2S_GPIO_UNUSED,
            .bclk = I2S_MIC_SCLK,
            .ws = I2S_MIC_WS,
            .dout = -1,
            .din = I2S_MIC_DIN,
            .invert_flags = {
                .mclk_inv = false,
                .bclk_inv = false,
                .ws_inv = false,
            },
        },
    };

    ret = i2s_channel_init_std_mode(rx_handle, &std_cfg);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to initialize I2S RX mode: %s", esp_err_to_name(ret));
        return;
    }

    ret = i2s_channel_enable(rx_handle);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to enable I2S RX channel: %s", esp_err_to_name(ret));
        return;
    }

    ESP_LOGI(TAG, "I2S RX channel initialized successfully");
}