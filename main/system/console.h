#pragma once

#ifdef __cplusplus
    extern "C" {
#endif

#define PROMPT_STR "esp32"
#define CONSOLE_MAX_COMMAND_LINE_LENGTH 256


esp_err_t initialize_console(void);
esp_err_t console_run(void);
esp_err_t console_stop(void);

#ifdef __cplusplus
    }
#endif