#include <stdio.h>

#include "esp_console.h"
#include "esp_system.h"
#include "esp_chip_info.h"
#include "esp_log.h"
#include "common.h"

#include "system/console.h"

#define TAG "system"

static int restart(int argc, char **argv) {
    ESP_LOGI(TAG, "Restarting...");
    // TODO End all tasks and save all information
    esp_restart();
    return 0;
}

static void register_restart() {
    const esp_console_cmd_t restart_cmd = {
        .command = "restart",
        .help = "Restart the device",
        .hint = NULL,
        .func = &restart,
        .argtable = NULL
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&restart_cmd));
}

static int shutdown(int argc, char **argv) {
    ESP_LOGI(TAG, "Shutting Down...");
    // TODO End all tasks and save all information
    console_stop();
    return 0;
}

static void register_shutdown() {
    const esp_console_cmd_t shutdown_cmd = {
        .command = "shutdown",
        .help = "Shutdown the device",
        .hint = NULL,
        .func = &shutdown,
        .argtable = NULL
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&shutdown_cmd));
}

static int hi(int argc, char **argv) {
    printf("Hello, how are you?\n");
    // Printf works in USB mode but not in UART mode
    return 0;
}

static void register_hi() {
    const esp_console_cmd_t sayhi_cmd = {
        .command = "hi",
        .help = "Say Hi",
        .hint = NULL,
        .func = &hi,
        .argtable = NULL
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&sayhi_cmd));
}



static int get_chip_data(int argc, char **argv) {
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    ESP_LOGI(TAG, "This is ESP32 chip with %d CPU cores, WiFi%s%s, ",
           chip_info.cores,
           (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
           (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");
    ESP_LOGI(TAG, "silicon revision %d, ", chip_info.revision);
    return 0;
}

//TODO: Change console command to get board data later
static void register_get_board_data() {
    const esp_console_cmd_t get_chip_data_cmd = {
        .command = "hardware-info",
        .help = "Get chip data",
        .hint = NULL,
        .func = &get_chip_data,
        .argtable = NULL
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&get_chip_data_cmd));
}




esp_err_t register_common_commands(void) {
    register_restart();
    register_shutdown();
    register_hi();
    register_get_board_data();
    return ESP_OK;
}