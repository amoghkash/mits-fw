#include <stdio.h>
#include "esp_system.h"
#include "esp_log.h"


// Storage
#include "nvs_flash.h"
#include "nvs.h"

// Networking
#include "wifi.h"



void firmware_init(void) {
    // Check chip status

    // Check if ESP Log 
    static const char *TAG = "firmware-init";
    ESP_LOGD(TAG, "Firmware Initialization Process started.");


    // Check if NVS Flash is mounted properly
    esp_err_t ret = nvs_flash_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Issue with Flash: %s", esp_err_to_name(ret));
        esp_system_abort("Flash was not able to be initialized");
    } else {
        ESP_LOGI(TAG, "Flash Initialized");
    }


    // Check if WiFi is Available
    wifi_init_sta();


    return;
}


void app_main(void)
{
    printf("Starting Application...\n");
    firmware_init();
}