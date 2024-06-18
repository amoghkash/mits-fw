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
        ESP_LOGE(TAG, "LSM9DS1 not connected");
        return ESP_ERR_NOT_FOUND;
    }

    ESP_LOGI(TAG, "LSM9DS1 connected");
    return ESP_OK;
}

esp_err_t lsm9ds1_write(uint8_t data) {
    esp_err_t ret;

    if (!lsm9ds1.configured) {
        ESP_LOGE(TAG, "LSM9DS1 not configured");
        return ESP_ERR_NOT_ALLOWED;
    }

    ret = i2c_master_transmit(lsm9ds1.device_handle, &data, sizeof(data), 1000);
    return ret;
    
}

esp_err_t lsm9ds1_activate(void) {
    uint8_t data[2] = { LSM9DS1_ACCEL_CTRL_REG1, LSM9DS1_ACCEL_CTRL_REG1_VAL };
    ESP_ERROR_CHECK(i2c_master_transmit(lsm9ds1.device_handle, data, sizeof(data), 100));

    return ESP_OK;
}

esp_err_t lsm9ds1_read(int16_t *buf) {
    uint8_t tx[1] = { LSM9DS1_ACCEL_X_L };
    uint8_t rx[6];

    i2c_master_transmit_receive(lsm9ds1.device_handle, tx, sizeof(tx), rx, sizeof(rx), 100);

    buf[0] = (int16_t)(rx[1] << 8) | rx[0];
    buf[1] = (int16_t)(rx[3] << 8) | rx[2];
    buf[2] = (int16_t)(rx[5] << 8) | rx[4];

    return ESP_OK;
}


void example(void) {

    ESP_ERROR_CHECK(lsm9ds1_connect());
    ESP_ERROR_CHECK(lsm9ds1_activate());
    int16_t accel_data[3];
    float accel_data_f[3];
    while (true) {
        lsm9ds1_read(accel_data);
        accel_data_f[0] = (float)(accel_data[0]*0.244)/1000;
        accel_data_f[1] = (float)(accel_data[1]*0.244)/1000;
        accel_data_f[2] = (float)(accel_data[2]*0.244)/1000;
        printf("Accel: %.2f, %.2f, %.2f\n", accel_data_f[0], accel_data_f[1], accel_data_f[2]);
    }
}