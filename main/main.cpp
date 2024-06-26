// Base

#include <stdio.h>
#include "esp_log.h"

#include "drivers/led.h"
#include "drivers/i2c.h"
#include "drivers/usb.h"

#include "hal/lsm9ds1.h"

#include "system/console.h"

#include "drivers/peripherals.h"
#include "system/system.h"

#include "application/app.h"

#define TAG "main"



extern "C" void app_main(void)
{
    ESP_ERROR_CHECK(initialize_peripherals());
    ESP_ERROR_CHECK(initialize_system());
    ESP_ERROR_CHECK(initialize_storage());
    ESP_ERROR_CHECK(enable_console());

    application_main();
}