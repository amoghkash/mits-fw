#include "stdio.h"

#include "esp_system.h"
#include "esp_log.h"
#include "esp_err.h"

#include "fs.h"
#include "nvs_system.h"
#include "console.h"

#include "system.h"

#define TAG "system"


esp_err_t initialize_system(void) {
    ESP_LOGI(TAG, "Initializing system...");

    ESP_ERROR_CHECK(initialize_storage());
    ESP_ERROR_CHECK(enable_console());

    ESP_LOGI(TAG, "System initialized.");

    return ESP_OK;
}

esp_err_t initialize_storage(void) {
    ESP_LOGI(TAG, "Initializing storage...");

    ESP_ERROR_CHECK(initialize_fs());
    ESP_ERROR_CHECK(initialize_nvs());

    ESP_LOGI(TAG, "Storage initialized.");

    return ESP_OK;
}

esp_err_t enable_console(void) {
    ESP_LOGI(TAG, "Enabling console...");

    ESP_ERROR_CHECK(initialize_console());

    ESP_LOGI(TAG, "Console enabled.");

    return ESP_OK;
}

esp_err_t start_system(void)
{
    return ESP_OK;
}
