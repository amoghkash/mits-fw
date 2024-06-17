#include <stdio.h>

#include "esp_console.h"
#include "esp_system.h"
#include "esp_log.h"
#include "peripherals.h"

#include "drivers/led.h"

#define TAG "peripheral_commands"


static int led_on(int argc, char **argv) {
    // TODO End all tasks and save all information
    turn_led_on();
    return 0;
}

static int led_off(int argc, char **argv) {
    // TODO End all tasks and save all information
    turn_led_off();
    return 0;
}

static void register_led() {

    const esp_console_cmd_t led_on_cmd = {
        .command = "led-on",
        .help = "Turn on LED",
        .hint = NULL,
        .func = &led_on,
        .argtable = NULL
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&led_on_cmd));

    const esp_console_cmd_t led_off_cmd = {
        .command = "led-off",
        .help = "Turn off LED",
        .hint = NULL,
        .func = &led_off,
        .argtable = NULL
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&led_off_cmd));
}



esp_err_t register_peripheral_commands(void) {
    register_led();
    return ESP_OK;
}