#pragma once

#ifdef __cplusplus
    extern "C" {
#endif

#define PROMPT_STR "sirah"
#define CONSOLE_MAX_COMMAND_LINE_LENGTH 256

esp_err_t console_initialize(void);
esp_err_t console_deinitialize(void);

#ifdef __cplusplus
    }
#endif