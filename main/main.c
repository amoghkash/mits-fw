// Base
#include <stdio.h>
#include "esp_log.h"

#include "drivers/led.h"
#include "drivers/i2c.h"
#include "drivers/usb.h"

#include "hal/lsm9ds1.h"

#include "drivers/peripherals.h"
#include "system/console.h"

#define TAG "main"

#include "system/fs.h"

void app_main(void)
{
    ESP_ERROR_CHECK(initialize_peripherals());

    initialize_spiffs();
    create_file("/spiffs/test.txt", "Hello, World!");
    r_file("/spiffs/test.txt");
    add_data_to_file("/spiffs/test.txt", "This is a test.");
    deinitialize_spiffs();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    initialize_spiffs();
    r_file("/spiffs/test.txt");
}