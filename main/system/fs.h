#pragma once

#ifdef __cplusplus
    extern "C" {
#endif

esp_err_t initialize_fs(void);
esp_err_t deinitialize_fs(void);
esp_err_t format_fs(void);

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