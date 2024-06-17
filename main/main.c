// Base
#include <stdio.h>

#include "drivers/led.h"
#include "drivers/i2c.h"
#include "drivers/usb.h"

#include "hal/lsm9ds1.h"

#include "system/console.h"

void app_main(void)
{

    // Peripherals Initialization
    configure_led();
    i2c_init();


    //initialize_usb();
    ESP_ERROR_CHECK(lsm9ds1_connect());
    lsm9ds1_write(0x40);
    
    ESP_ERROR_CHECK(console_init());
    ESP_ERROR_CHECK(console_run());
}