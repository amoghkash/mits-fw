#include <stdio.h>
#include <string.h>

#include "esp_system.h"
#include "esp_console.h"
#include "esp_log.h"


#include "console.h"
#include "commands/common.h"

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

static volatile bool isInitialized = false;
static volatile bool desired_state = false;
static esp_console_repl_t *repl = NULL;
static esp_console_repl_config_t repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();



void initialize_all_commands() {
    esp_console_register_help_command();
    register_common_commands();
}

esp_err_t console_initialize(void) {
    if (!isInitialized) {
        ESP_LOGD(TAG, "Console Initializing");
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
        isInitialized = true;
        return ret;
    }
    return ESP_OK;
}

esp_err_t console_deinitialize(void) {
    if (isInitialized) {
        esp_err_t ret = repl->del(repl);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to close console: %s", esp_err_to_name(ret));
            return ret;
        }
        ESP_LOGI(TAG, "Console Deinitializing");
        printf("\n");
        ret = esp_console_deinit();
        repl = NULL;
        isInitialized = false;
        return ESP_OK;
    }
    return ESP_OK;
}