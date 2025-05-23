#pragma once

#ifdef __cplusplus
    extern "C" {
#endif

#include "esp_err.h"

esp_err_t fs_initialize(void);
esp_err_t fs_deinitialize(void);
esp_err_t fs_format(void);

esp_err_t create_file(const char* filename);
esp_err_t create_file_add_data(const char *filename, const char *data);

esp_err_t readout_file(const char *filename);
esp_err_t read_file(const char *filename, char *buffer, size_t buffer_size);
esp_err_t add_data_to_file(const char *filename, const char *data);

esp_err_t rename_file(const char *old_filename, const char *new_filename);
esp_err_t delete_file(const char *filename);

#ifdef __cplusplus
    }
#endif