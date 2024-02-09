#include <stdio.h>
#include "driver/gpio.h"
#include "esp_log.h"
#include "led_strip.h"
#include "sdkconfig.h"

// FreeRTOS
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "led.h"

static const char *TAG = "led";

/* Use project configuration menu (idf.py menuconfig) to choose the GPIO to blink,
   or you can edit the following line and set a number here.
*/
#define BLINK_GPIO 48

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




// EXTERNAL FUNCTIONS

/**
 * @brief Configure the LED on the Board
 */
void configure_led(void) {
    ESP_LOGD(TAG, "Configuring LED Strip");
    /* LED strip initialization with the GPIO and pixels number*/
    led_strip_config_t strip_config = {
        .strip_gpio_num = BLINK_GPIO,
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


void set_color_gradiate(uint8_t r, uint8_t g, uint8_t b) {
    // Check if LED is configured
    if (!s_led_configured) {
        return;
    }

    // Set LED Color
    ESP_LOGD(TAG, "Gradiating LED color to %d %d %d", r, g, b);

    int r_diff = r - s_led_colors[0];
    int g_diff = g - s_led_colors[1];
    int b_diff = b - s_led_colors[2];

    ESP_LOGD(TAG, "Differences: %d %d %d", r_diff, g_diff, b_diff);

    int r_step = r_diff / COLOR_ITERATIONS;
    int g_step = g_diff / COLOR_ITERATIONS;
    int b_step = b_diff / COLOR_ITERATIONS;

    ESP_LOGD(TAG, "Steps: %d %d %d", r_step, g_step, b_step);

    for (int i = 0; i < COLOR_ITERATIONS; i++) {
        led_strip_set_pixel(led_strip, 0, s_led_colors[0] + (r_step * i), s_led_colors[1] + (g_step * i), s_led_colors[2] + (b_step * i));
        led_strip_refresh(led_strip);
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
    esp_err_t ret = led_strip_refresh(led_strip);

    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "led_strip_refresh failed, %d", ret);
        return;
    }

    s_led_state = false;
    update_color_data(255, 255, 255);
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





/**
 * @brief Run a test on the LED
 */
void led_test(void) {
    if (!s_led_configured) {
        ESP_LOGE(TAG, "LED not configured");
        return;
    }

    turn_led_on();
    vTaskDelay(500 / portTICK_PERIOD_MS);
    gradiate_led_off();
    vTaskDelay(500 / portTICK_PERIOD_MS);


    gradiate_led_on();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    set_color_instant(255, 0, 0);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    set_color_gradiate(0,0,255);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    turn_led_off();
}