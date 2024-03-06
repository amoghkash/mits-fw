#include <stdio.h>
#include "driver/gpio.h"
#include "esp_log.h"
#include "sdkconfig.h"

// FreeRTOS
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Peripherals
#include "led.h"
#include "i2c.h"

// Storage
#include "nvs_flash.h"
#include "nvs.h"

#include "firmware_system.h"

static const char *TAG = "initalizer";


static bool s_firmware_healthy = false;

/**
 * @brief       Indicate if the firmware is healthy
 */
void firmware_safe(void) {
    s_firmware_healthy = true;
    set_color_instant(0, 75, 0);
    return;
}


/**
 * @brief    Initialize the firmware
 */
void firmware_init(void) {
    // TODO Check chip status

    
    // Configure Peripherals
    configure_led();

    configure_I2C();


    if (FIRMWARE_TEST_ENABLED) {
        firmware_test();
    }

    //firmware_safe();

    return;
}

/**
 * @brief       Test the firmware & peripherals
 */
void firmware_test(void) {
    // Test LED

    ESP_LOGI(TAG, "Testing Peripherals");

    ESP_LOGD(TAG, "Testing LED");
    //led_test();
    ESP_LOGI(TAG, "LED Test Complete");

    ESP_LOGD(TAG, "Testing I2C");
    i2c_test();
    ESP_LOGI(TAG, "I2C Test Complete");

    return;
}



