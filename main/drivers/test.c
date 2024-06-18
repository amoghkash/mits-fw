#include "esp_log.h"
#include "freertos/FreeRTOS.h"


#include "led.h"

#include "test.h"

#define TAG "peripheral_test"

/**
 * @brief Run a test on the LED
 */
void led_test(void) {
    if (!led_configured()) {
        ESP_LOGE(TAG, "LED not configured");
        return;
    }

    turn_led_on();
    vTaskDelay(500 / portTICK_PERIOD_MS);
    set_color_gradiate(255,0,0);
    vTaskDelay(500 / portTICK_PERIOD_MS);
    gradiate_led_off();
    vTaskDelay(500 / portTICK_PERIOD_MS);
    gradiate_led_on();
    vTaskDelay(500 / portTICK_PERIOD_MS);
    set_color_gradiate(47,243,123);
    vTaskDelay(500 / portTICK_PERIOD_MS);
    turn_led_off();
    vTaskDelay(500 / portTICK_PERIOD_MS);
    
    return;
}