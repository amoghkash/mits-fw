// Base

#include <stdio.h>


// Architecture Imports
#include "esp_log.h"

// Storage Imports
#include "system/nvs_system.h"
#include "system/fs.h"
#include "system/console.h"

// Peripheral Imoprts
#include "drivers/i2c.h"

// Bluetooth Imports
#include "bluetooth/ble.h"

#define TAG "main"

extern "C" void app_main(void)
{
    // Initialize NVS
    nvs_initialize();
    nvs_iterate_strings();
    char *device_id = nvs_get_string("sn");
    if (device_id != NULL) {
        ESP_LOGI(TAG, "Device ID: %s", device_id);
        free(device_id);
    }

    // Initialize Peripherals
    i2c_initialize();

    // Initialize Filesystem
    fs_initialize();

    // Initialize Console
    //console_initialize();

    // Initialize Bluetooth
    ble_initialize();
}