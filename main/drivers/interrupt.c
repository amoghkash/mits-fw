#include "driver/gpio.h"
#include "esp_intr_alloc.h"
#include "board.h"
#include "esp_log.h"

#include "system/console.h"
#include "interrupt.h"

#define TAG "interrupt"

void configure_interrupts() {
    // Enable Per Pin Interrupts
    gpio_install_isr_service(0);
    esp_err_t stat = configure_intr_usb();
    ESP_ERROR_CHECK(stat);
}

void remove_interrupts() {
    gpio_isr_handler_remove(USB_SENSING_PIN);
    gpio_uninstall_isr_service();
}

static void usb_isr_handler(void* arg) {
    if (gpio_get_level(USB_SENSING_PIN)) {
        setConsoleState(true);
    } else {
        setConsoleState(false);
    }
    return;
}

esp_err_t configure_intr_usb() {
    gpio_set_direction(USB_SENSING_PIN, GPIO_MODE_INPUT);
    gpio_set_intr_type(USB_SENSING_PIN, GPIO_INTR_ANYEDGE);
    esp_err_t ret = gpio_isr_handler_add(USB_SENSING_PIN,usb_isr_handler,(void*)USB_SENSING_PIN);
    return ret;
}

