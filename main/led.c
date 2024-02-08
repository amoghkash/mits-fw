#include <stdio.h>
#include "driver/gpio.h"
#include "esp_log.h"
#include "led_strip.h"
#include "sdkconfig.h"

#include "led.h"

static const char *TAG = "led";

/* Use project configuration menu (idf.py menuconfig) to choose the GPIO to blink,
   or you can edit the following line and set a number here.
*/
#define BLINK_GPIO 48

static bool s_led_state = false; // True is on, false is off

static bool s_led_configured = false;

static led_strip_handle_t led_strip;


void configure_led(void) {
    ESP_LOGI(TAG, "Configuring LED Strip");
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


// Turn on LED
void turn_led_on(void) {
    if (!s_led_configured || s_led_state) { // If the LED is not configured or LED is already on, return
        return;
    }


    ESP_LOGI(TAG, "Turning LED on");
    led_strip_set_pixel(led_strip, 0, 255, 255, 255);
    esp_err_t ret = led_strip_refresh(led_strip);

    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "led_strip_refresh failed, %d", ret);
        return;
    }

    s_led_state = true;
}

void set_color(uint8_t r, uint8_t g, uint8_t b) {
    if (!s_led_configured) { // If the LED is not configured, return
        return;
    }

    ESP_LOGI(TAG, "Setting LED color to %d %d %d", r, g, b);
    led_strip_set_pixel(led_strip, 0, r, g, b);
    esp_err_t ret = led_strip_refresh(led_strip);

    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "led_strip_refresh failed, %d", ret);
        return;
    }
}



// Turn off LED
void turn_led_off(void) {
    if (!s_led_configured || !s_led_state) { // If the LED is not configured or LED is already off, return
        return;
    }

    ESP_LOGI(TAG, "Turning LED off");
    
    esp_err_t ret = led_strip_clear(led_strip);

    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "led_strip_clear failed, %d", ret);
        return;
    }

    s_led_state = false;
}