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

    initialize_fs();
    create_file("/data/test.txt", "Hello, World!\n");
    readout_file("/data/test.txt");
    ESP_ERROR_CHECK(add_data_to_file("/data/test.txt", "This is a test.\n"));
    deinitialize_fs();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    initialize_fs();
    char buffer[100];
    ESP_ERROR_CHECK(read_file("/data/test.txt", buffer, 100));
    printf("Buffer: %s\n", buffer);
}