#ifndef ADAPTER_I2S_H_
#define ADAPTER_I2S_H_

#include "config.h"

typedef enum {
    PLAYER_STATE_IDLE,
    PLAYER_STATE_PLAYING,
    PLAYER_STATE_STOPPED
} player_state_t;

void adapter_i2s_spk_init(void);
esp_err_t adapter_i2s_start_player(const char *path);
void adapter_i2s_set_volume(float scale);


esp_err_t adapter_i2s_start_speaker(void *arg);
void adapter_i2s_mic_init(void);



#endif