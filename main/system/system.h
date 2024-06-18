#pragma once

#ifdef __cplusplus
    extern "C" {
#endif

esp_err_t initialize_system(void);
esp_err_t initialize_storage(void);
esp_err_t enable_console(void);

esp_err_t start_system(void);

#ifdef __cplusplus
    }
#endif