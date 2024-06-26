#include <stdio.h>

#include "esp_system.h"
#include "esp_log.h"




#include "storage.h"
#include "hal/lsm9ds1.h"
#include "motion.h"

#define TAG "Motion"

Motion::Motion() {
}

esp_err_t Motion::initialize(void) {
    ESP_LOGI(TAG, "Initializing Motion...");
    // Initialize LSM9DS1
    lsm9ds1_init();
    ESP_LOGI(TAG, "Motion Initialized.");
    return ESP_OK;
}