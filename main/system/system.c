#include "stdio.h"

#include "esp_system.h"
#include "esp_log.h"
#include "esp_err.h"

#include "fs.h"
#include "nvs_system.h"
#include "console.h"

#include "system.h"

#define TAG "system"

static bool storage_initialized = false;
static bool console_initialized = false;

esp_err_t initialize_system(void) {
    ESP_LOGD(TAG, "Initializing system...");
    ESP_ERROR_CHECK(initialize_storage());
    ESP_ERROR_CHECK(enable_console());
    return ESP_OK;
}

esp_err_t initialize_storage(void) {
    if (storage_initialized) {
        ESP_LOGD(TAG, "Storage already initialized.");
        return ESP_OK;
    }
    ESP_ERROR_CHECK(initialize_fs());
    ESP_ERROR_CHECK(initialize_nvs());

    storage_initialized = true;

    return ESP_OK;
}

esp_err_t enable_console(void) {
    if (console_initialized) {
        ESP_LOGD(TAG, "Console already initialized.");
        return ESP_OK;
    }
    ESP_LOGD(TAG, "Console enabled.");
    console_run();
    console_initialized = true;
    return ESP_OK;
}

bool is_storage_initialized(void) {
    return storage_initialized;
}

bool is_console_initialized(void) {
    return console_initialized;
}

