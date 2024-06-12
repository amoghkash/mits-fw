#include "drivers/i2c.h"
#include "esp_log.h"


#include "hal/lsm9ds1.h"
#include "lsm9ds1.h"

#define TAG "LSM9DS1"

static struct i2c_device lsm9ds1 = {
    .name = "LSM9DS1",
    .device_address = 0x6B,
    .device_handle = NULL,
    .connected = false,
    .configured = false,
};


void lsm9ds1_init(void) {
    i2c_device_config_t lsm9ds1_config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = lsm9ds1.device_address,
        .scl_speed_hz = 400000,
    };
    lsm9ds1.device_handle = i2c_add_device(&lsm9ds1_config);
    lsm9ds1.connected = i2c_probe(lsm9ds1.device_address) == ESP_OK;
    lsm9ds1.configured = true;
}

esp_err_t lsm9ds1_connect(void) {
    lsm9ds1_init();

    if (!lsm9ds1.connected) {
        ESP_LOGE(TAG, "LSM9DS1 not connected\n");
        return ESP_ERR_NOT_FOUND;
    }

    ESP_LOGI(TAG, "LSM9DS1 connected\n");
    return ESP_OK;
}

esp_err_t lsm9ds1_write(uint8_t data) {
    esp_err_t ret;

    if (!lsm9ds1.configured) {
        ESP_LOGE(TAG, "LSM9DS1 not configured\n");
        return ESP_ERR_NOT_ALLOWED;
    }

    ret = i2c_master_transmit(lsm9ds1.device_handle, &data, sizeof(data), 1000);
    return ret;
    
}
