#include <stdio.h>

#include "esp_system.h"
#include "esp_log.h"
#include "esp_console.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "storage.h"
#include "hal/lsm9ds1.h"
#include "motion.h"

#define TAG "Motion"

static char *accel_data_str;
static bool log;

Motion::Motion() {
}

esp_err_t Motion::initialize(void) {
    ESP_LOGI(TAG, "Initializing Motion...");
    // Initialize LSM9DS1
    lsm9ds1_connect();
    lsm9ds1_activate();
    ESP_LOGI(TAG, "Motion Initialized.");
    #ifdef CONFIG_MOTION_CONSOLE_COMMANDS
        register_motion_commands();
    #endif
    return ESP_OK;
}

esp_err_t Motion::start(void) {
    ESP_LOGI(TAG, "Starting Motion...");
    if (!lsm9ds1_is_connected()) {
        ESP_LOGE(TAG, "LSM9DS1 not connected");
        return ESP_ERR_NOT_ALLOWED;
    }
    xTaskCreate(logMotion, "logMotion", 4096, NULL, 5, &tHandle);
    ESP_LOGI(TAG, "Motion Started.");
    return ESP_OK;
}

esp_err_t Motion::stop(void) {
    //TaskHandle_t xHandle = xTaskGetHandle("logMotion");
    log = false;
    return ESP_OK;
}

void logMotion(void * pvParameters) {
    ESP_LOGI(TAG, "Creating Log File...");
    FileSystem fs;
    fs.createFile("/data/motion.txt");
    int16_t accel_data[3];
    float accel_data_f[3];
    char *accel_data_str = (char *)malloc(50);
    log = true;
    
    while (log) {
        lsm9ds1_read(accel_data);
        accel_data_f[0] = (float)(accel_data[0]*0.244)/1000;
        accel_data_f[1] = (float)(accel_data[1]*0.244)/1000;
        accel_data_f[2] = (float)(accel_data[2]*0.244)/1000;
        sprintf(accel_data_str, "%.2f, %.2f, %.2f", accel_data_f[0], accel_data_f[1], accel_data_f[2]);
        fs.addToFile("/data/motion.txt", accel_data_str);
    }
    printf("Motion Stopped.\n");
    free(accel_data_str);
    fs.addToFile("/data/motion.txt", "Motion Stopped.");
    TaskHandle_t task = xTaskGetHandle("logMotion");
    vTaskDelete(task);
}


// Console Commands
#ifdef CONFIG_MOTION_CONSOLE_COMMANDS
static int console_motion_start(int argc, char **argv) {
    printf("Starting Data Logging...\n");
    Motion motion;
    motion.start();
    return 0;
}

static int console_motion_stop(int argc, char **argv) {
    printf("Stopping Data Logging...\n");
    Motion motion;
    motion.stop();
    return 0;
}

void register_motion_commands(void) {
    const esp_console_cmd_t start_motion_cmd = {
        .command = "start-motion",
        .help = "Start Data Logging",
        .hint = NULL,
        .func = &console_motion_start,
        .argtable = NULL
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&start_motion_cmd));

    const esp_console_cmd_t stop_motion_cmd = {
        .command = "stop-motion",
        .help = "Stop Data Logging",
        .hint = NULL,
        .func = &console_motion_stop,
        .argtable = NULL
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&stop_motion_cmd));
}
#endif