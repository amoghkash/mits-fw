#include <stdio.h>
#include <stdbool.h>

#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_console.h"
#include "argtable3/argtable3.h"
#include "led_strip.h"
#include "sdkconfig.h"
#include "mits_board.h"

// FreeRTOS
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "led.h"

static const char *TAG = "RGBLed";

static bool s_led_state = false; // True is on, false is off
static bool s_led_configured = false;
static uint8_t s_led_colors[] = {0,0,0};

static led_strip_handle_t led_strip;


// INTERNAL FUNCTIONS

/**
 * @brief       Update the color data
 * 
 * @param r     Red
 * @param g     Green
 * @param b     Blue
 * @return uint8_t* Array containing current color data
 */
static uint8_t* update_color_data(uint8_t r, uint8_t g, uint8_t b) {
    s_led_colors[0] = r;
    s_led_colors[1] = g;
    s_led_colors[2] = b;
    return s_led_colors;
}

/**
 * @brief       Update LED Color by 1 step in the target direction.
 * 
 * @param r     Target Red Value
 * @param g     Target Green Value
 * @param b     Target Blue Value
 * 
 * @returns     True if the LED color was updated, False if the LED color is already at the target
 */
static bool step_led_color(uint8_t r, uint8_t g, uint8_t b) {
    int r_diff = 0;
    int g_diff = 0;
    int b_diff = 0;

    if (s_led_colors[0] < r){
        r_diff = 1;
    } else if (s_led_colors[0] == r){
        r_diff = 0;
    } else {
        r_diff = -1;
    }

    if (s_led_colors[1] < g){
        g_diff = 1;
    } else if (s_led_colors[1] == g){
        g_diff = 0;
    } else {
        g_diff = -1;
    }

    if (s_led_colors[2] < b){
        b_diff = 1;
    } else if (s_led_colors[2] == b){
        b_diff = 0;
    } else {
        b_diff = -1;
    }

    if (r_diff == 0 && g_diff == 0 && b_diff == 0) {
        return false;
    }

    //  DEBUG LOG
    //ESP_LOGD(TAG, "Diff: %d %d %d", r_diff, g_diff, b_diff);
    //ESP_LOGD(TAG, "Updating LED Color to %d %d %d", s_led_colors[0] + r_diff, s_led_colors[1] + g_diff, s_led_colors[2] + b_diff);

    led_strip_set_pixel(led_strip, 0, s_led_colors[0] + r_diff, s_led_colors[1] + g_diff, s_led_colors[2] + b_diff);
    led_strip_refresh(led_strip);

    update_color_data(s_led_colors[0] + r_diff, s_led_colors[1] + g_diff, s_led_colors[2] + b_diff);

    return true;
}

// EXTERNAL FUNCTIONS

/**
 * @brief Configure the LED on the Board
 */
void initialize_led(void) {
    ESP_LOGD(TAG, "Configuring LED Strip");
    /* LED strip initialization with the GPIO and pixels number*/
    led_strip_config_t strip_config = {
        .strip_gpio_num = RGB_LED_PIN,
        .max_leds = 1, // at least one LED on board
    };
    led_strip_rmt_config_t rmt_config = {
        .resolution_hz = 10 * 1000 * 1000, // 10MHz
    };
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip));
    /* Set all LED off to clear all pixels */
    led_strip_clear(led_strip);

    s_led_configured = true;
}

/**
 * @brief Check if the LED is configured
 * 
 * @return true if the LED is configured, false otherwise
 */
bool led_configured(void) {
    return s_led_configured;
}

/**
 * @brief Turn on the LED
 */
void turn_led_on(void) {
    // Check if the LED is not configured or LED is already on
    if (!s_led_configured || s_led_state) {
        return;
    }

    // Turn on the LED
    ESP_LOGD(TAG, "Turning LED on");
    led_strip_set_pixel(led_strip, 0, 255, 255, 255);
    esp_err_t ret = led_strip_refresh(led_strip);

    // Ensure LED turns on properly
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "led_strip_refresh failed, %d", ret);
        return;
    }

    s_led_state = true;
    update_color_data(255, 255, 255);
}

/**
 * @brief Turn off the LED instantly
 */
void turn_led_off(void) {
    if (!s_led_configured || !s_led_state) { // If the LED is not configured or LED is already off, return
        return;
    }

    ESP_LOGD(TAG, "Turning LED off");
    
    esp_err_t ret = led_strip_clear(led_strip);

    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "led_strip_clear failed, %d", ret);
        return;
    }

    s_led_state = false;
}

/**
 * @brief       Set the LED color. 
 * 
 * @param r     Red Value
 * @param g     Green Value
 * @param b     Blue Value
 */
void set_color_instant(uint8_t r, uint8_t g, uint8_t b) {
    // Check if LED is configured
    if (!s_led_configured) {
        return;
    }

    // Set LED Color
    ESP_LOGD(TAG, "Setting LED color to %d %d %d", r, g, b);
    led_strip_set_pixel(led_strip, 0, r, g, b);
    esp_err_t ret = led_strip_refresh(led_strip);

    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "led_strip_refresh failed, %d", ret);
        return;
    }

    s_led_state = true;
    update_color_data(r, g, b);
}

/**
 * @brief       Gradiate the LED color to the target color
 * 
 * @param r     Target Red Value
 * @param g     Target Green Value
 * @param b     Target Blue Value
 */
void set_color_gradiate(uint8_t r, uint8_t g, uint8_t b) {
    // Check if LED is configured
    if (!s_led_configured) {
        return;
    }

    // Set LED Color
    ESP_LOGD(TAG, "Gradiating LED color to %d %d %d", r, g, b);

    while (step_led_color(r, g, b)) {
        vTaskDelay(ITERATION_TIME);
    }
   
    s_led_state = true;
    update_color_data(r, g, b);
}

/**
 * @brief Gradiate the LED on
 */
void gradiate_led_on(void) {
    if (!s_led_configured || s_led_state) { // If the LED is not configured or LED is already off, return
        return;
    }

    ESP_LOGD(TAG, "Gradiating LED on");

    set_color_gradiate(255, 255, 255);

    s_led_state = true;
}

/**
 * @brief Gradiate the LED off
 */
void gradiate_led_off(void) {
    if (!s_led_configured || !s_led_state) { // If the LED is not configured or LED is already off, return
        return;
    }

    ESP_LOGD(TAG, "Gradiating LED off");

    set_color_gradiate(0, 0, 0);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    esp_err_t ret = led_strip_clear(led_strip);

    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "led_strip_clear failed, %d", ret);
        return;
    }

    s_led_state = false;
    update_color_data(0, 0, 0);
}


// LED Control Over Console
#ifdef CONFIG_LED_CONSOLE_COMMANDS
    // LED On Wrapper
    static int led_on(int argc, char **argv) {
        turn_led_on();
        return 0;
    }

    // LED Off Wrapper
    static int led_off(int argc, char **argv) {
        turn_led_off();
        return 0;
    }

    static struct {
        struct arg_int *red_color;
        struct arg_int *green_color;
        struct arg_int *blue_color;
        struct arg_end *end;
    } led_color_args;

    static int led_color(int argc, char **argv) {
        int nerrors = arg_parse(argc, argv, (void **)&led_color_args);

        if (nerrors != 0) {
            arg_print_errors(stderr, led_color_args.end, argv[0]);
            return 0;
        }

        uint8_t r = (uint8_t)led_color_args.red_color->ival[0];
        uint8_t g = (uint8_t)led_color_args.green_color->ival[0];
        uint8_t b = (uint8_t)led_color_args.blue_color->ival[0];

        set_color_gradiate(r, g, b);
        printf("Setting LED color to %d %d %d\n", r, g, b);
        return 0;
    }

    void register_led_commands() {
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

        led_color_args.red_color = arg_int0("r", "red", "<int>", "LED Red Value");
        led_color_args.green_color = arg_int0("g", "green", "<int>", "LED Green Value");
        led_color_args.blue_color = arg_int0("b", "blue", "<int>", "LED Blue Value");
        led_color_args.end = arg_end(1);
        const esp_console_cmd_t led_color_cmd = {
            .command = "led-color",
            .help = "Change LED Color",
            .hint = NULL,
            .func = &led_color,
            .argtable = &led_color_args
        };
        ESP_ERROR_CHECK(esp_console_cmd_register(&led_color_cmd));
    }
#endif