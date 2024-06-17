// Base
#include <stdio.h>

#include "drivers/led.h"
#include "drivers/i2c.h"
#include "drivers/usb.h"

#include "hal/lsm9ds1.h"

void app_main(void)
{
    i2c_init();
    initialize_usb();
    ESP_ERROR_CHECK(lsm9ds1_connect());
    lsm9ds1_write(0x40);
    while (1) {
        printf("hi");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}