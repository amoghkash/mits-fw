#include <stdio.h>
#include <string.h>

#include "esp_system.h"
#include "esp_console.h"
#include "esp_log.h"


#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "console.h"

#include "commands/common.h"
#include "drivers/peripherals.h"
#include "application/interlink.h"

#define TAG "console"


/*
 * We warn if a secondary serial console is enabled. A secondary serial console is always output-only and
 * hence not very useful for interactive console applications. If you encounter this warning, consider disabling
 * the secondary serial console in menuconfig unless you know what you are doing.
 */
#if SOC_USB_SERIAL_JTAG_SUPPORTED
    #if !CONFIG_ESP_CONSOLE_SECONDARY_NONE
        #warning "A secondary serial console is not useful when using the console component. Please disable it in menuconfig."
    #endif
#endif

static volatile bool curr_state = false;
static volatile bool desired_state = false;
static SemaphoreHandle_t xSemaphore = NULL;
static StaticSemaphore_t xSemaphoreBuffer;

static esp_console_repl_t *repl = NULL;
static esp_console_repl_config_t repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();



void initialize_all_commands() {
    esp_console_register_help_command();
    initialize_peripheral_commands();
    register_common_commands();
    register_app_commands();
}

esp_err_t initialize_console(void) {
    if (!curr_state) {
        ESP_LOGI(TAG, "Console Initializing");
        esp_err_t ret;
        repl_config.prompt = PROMPT_STR ">";
        repl_config.max_cmdline_length = CONSOLE_MAX_COMMAND_LINE_LENGTH;
    
        #ifdef CONFIG_ESP_CONSOLE_UART_DEFAULT
            esp_console_dev_uart_config_t hw_config = ESP_CONSOLE_DEV_UART_CONFIG_DEFAULT();
            ret = esp_console_new_repl_uart(&hw_config, &repl_config, &repl);
        
        #elif defined CONFIG_ESP_CONSOLE_USB_CDC
            esp_console_dev_usb_cdc_config_t hw_config = ESP_CONSOLE_DEV_CDC_CONFIG_DEFAULT();
            ret = esp_console_new_repl_usb_cdc(&hw_config, &repl_config, &repl);
        #endif
        initialize_all_commands();
        esp_console_start_repl(repl);
        curr_state = true;
        return ret;
    }
    return ESP_OK;
}

esp_err_t deinitialize_console(void) {
    if (curr_state) {
        curr_state = false;
        esp_err_t ret = repl->del(repl);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to close console: %s", esp_err_to_name(ret));
            return ret;
        }
        ESP_LOGI(TAG, "Console Deinitializing");
        printf("\n");
        ret = esp_console_deinit();
        repl = NULL;
        curr_state = false;
        return ESP_OK;
    }
    return ESP_OK;
}

void vTaskConsole(void *pvParameters) {
    while (1) {
        //ESP_LOGI(TAG, "Console State: %d", getConsoleState());
        if (getConsoleState()) {
            initialize_console();
        } else {
            deinitialize_console();
        }
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}

void console_run(void) {
    xSemaphore = xSemaphoreCreateBinaryStatic(&xSemaphoreBuffer);
    xTaskCreate(vTaskConsole, "Console", 4096, NULL, 2, NULL);
}

void setConsoleState(bool state) {
    desired_state = state;
}

bool getConsoleState() {
    return desired_state;
}