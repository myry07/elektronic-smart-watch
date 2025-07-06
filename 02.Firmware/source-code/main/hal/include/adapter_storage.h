#ifndef ADAPTER_STORAGE_H_
#define ADAPTER_STORAGE_H_

#include "config.h"

#define MOUNT_POINT "/sdcard"

void adapter_spiffs_init(void);
void adapter_sd_init(void);

esp_err_t write_file(const char *path, char *data);
esp_err_t read_file(const char *path);

#endif