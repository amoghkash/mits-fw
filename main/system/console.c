#include <stdio.h>
#include <string.h>

#include "esp_system.h"
#include "esp_console.h"
#include "esp_log.h"

#include "console.h"
#include "nvs_system.h"
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

static bool initialized = false;
static esp_console_repl_t *repl = NULL;
static esp_console_repl_config_t repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();


esp_err_t console_init(void) {
    esp_err_t ret;

    if (initialized) {
        ESP_LOGI(TAG, "Console already initialized");
        return ESP_OK;
    }

    initialize_nvs();

    repl_config.prompt = PROMPT_STR ">";
    repl_config.max_cmdline_length = CONSOLE_MAX_COMMAND_LINE_LENGTH;

    ESP_ERROR_CHECK(esp_console_register_help_command());
    ESP_ERROR_CHECK(register_common_commands());

    #ifdef CONFIG_ESP_CONSOLE_UART_DEFAULT
        esp_console_dev_uart_config_t hw_config = ESP_CONSOLE_DEV_UART_CONFIG_DEFAULT();
        ESP_ERROR_CHECK(esp_console_new_repl_uart(&hw_config, &repl_config, &repl));
    
    #elif defined CONFIG_ESP_CONSOLE_USB_CDC
        esp_console_dev_usb_cdc_config_t hw_config = ESP_CONSOLE_DEV_CDC_CONFIG_DEFAULT();
        ESP_ERROR_CHECK(esp_console_new_repl_usb_cdc(&hw_config, &repl_config, &repl));
    #endif

    initialized = true;
    return ret;
}

esp_err_t console_run(void) {
    if (!initialized) {
       ESP_ERROR_CHECK(console_init());
    }

    return esp_console_start_repl(repl);
}

esp_err_t console_stop(void) {

    if (!initialized) {
        ESP_LOGI(TAG, "Console not initialized");
        return ESP_OK;
    }

    esp_err_t ret = repl->del(repl);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to close console: %s", esp_err_to_name(ret));
        return ret;
    }
    esp_console_deinit();
    initialized = false;
    return ESP_OK;
}