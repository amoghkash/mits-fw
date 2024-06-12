// Base
#include <stdio.h>

#include "drivers/led.h"
#include "drivers/i2c.h"


#include "hal/lsm9ds1.h"

void app_main(void)
{
    i2c_init();
    ESP_ERROR_CHECK(lsm9ds1_connect());
    lsm9ds1_write(0x40);
}