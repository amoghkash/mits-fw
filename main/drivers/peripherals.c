

#include "i2c.h"
#include "usb.h"
#include "led.h"

#include "peripherals.h"


esp_err_t initialize_peripherals(void) {
    esp_err_t ret;
    configure_led();
    ret = i2c_init();
    
    if (ret != ESP_OK) {
        return ret;
    }
    /* TO BE INCLUDED
    initialize_usb();
    */
   return ret;
}