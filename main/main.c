// Base
#include <stdio.h>
#include "esp_system.h"
#include "esp_log.h"

// System
#include "firmware_system.h"


// FreeRTOS
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Storage
#include "nvs_flash.h"
#include "nvs.h"



#include "mpu_test.h"


void app_main(void)
{
    printf("Testing MPU6050...\n");
    //task_mpu6050();

    /*
    printf("Starting Application...\n");
    firmware_init();
    */
}