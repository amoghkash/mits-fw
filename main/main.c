#include <stdio.h>
#include "esp_system.h"
#include "esp_log.h"


// FreeRTOS
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Storage
#include "nvs_flash.h"
#include "nvs.h"

// Networking

// Peripherals
#include "led.h"



void firmware_init(void) {
    // Check chip status

    // Configure Peripherals
    configure_led();

    // LED TEST
    turn_led_on();
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    set_color(30, 30, 30);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    turn_led_off();
    

    return;
}


void app_main(void)
{
    printf("Starting Application...\n");
    firmware_init();
}