#pragma once

#define PROMPT_STR "esp32"
#define CONSOLE_MAX_COMMAND_LINE_LENGTH 256


esp_err_t console_init(void);
esp_err_t console_run(void);
esp_err_t console_stop(void);