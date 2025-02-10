#include "i2c.h"
//#include "usb.h"
#include "led.h"

#include "peripherals.h"
#include "interrupt.h"


esp_err_t initialize_peripherals(void) {
    esp_err_t ret;
    initialize_led();
    ret = initialize_i2c();
    configure_interrupts();
    return ret;
}


void initialize_peripheral_commands() {
    #ifdef CONFIG_I2C_CONSOLE_COMMANDS
        register_i2c_commands();
    #endif

    #ifdef CONFIG_LED_CONSOLE_COMMANDS
        register_led_commands();
    #endif
}