#pragma once

#include "esp_err.h"
#include <stdint.h>

typedef struct {
    int sda;
    int scl;
    uint32_t fre;
    uint16_t x_limit;
    uint16_t y_limit;
} cst816s_cfg_t;

esp_err_t cst816s_init(cst816s_cfg_t *cfg);
void cst816s_read(int16_t *x, int16_t *y, int *state);