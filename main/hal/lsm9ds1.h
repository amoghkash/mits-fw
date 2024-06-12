#pragma once

struct device_transaction {
    uint8_t     address;
    uint8_t     data;
};



void lsm9ds1_init(void);
esp_err_t lsm9ds1_connect(void);
esp_err_t lsm9ds1_write(uint8_t data);

