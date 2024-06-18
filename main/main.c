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

void app_main(void)
{
    ESP_ERROR_CHECK(initialize_peripherals());

    ESP_ERROR_CHECK(lsm9ds1_connect());
    ESP_ERROR_CHECK(lsm9ds1_activate());
    int16_t accel_data[3];
    float accel_data_f[3];
    while (true) {
        lsm9ds1_read(accel_data);
        accel_data_f[0] = (float)(accel_data[0]*0.244)/1000;
        accel_data_f[1] = (float)(accel_data[1]*0.244)/1000;
        accel_data_f[2] = (float)(accel_data[2]*0.244)/1000;
        printf("Accel: %.2f, %.2f, %.2f\n", accel_data_f[0], accel_data_f[1], accel_data_f[2]);
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
    
}