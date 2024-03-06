#include <stdio.h>
#include "driver/gpio.h"
#include "esp_log.h"
#include "led_strip.h"
#include "sdkconfig.h"
#include "board.h"

// FreeRTOS
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// System Files
#include "led.h"

// I2C
#include "driver/i2c.h"
#include "i2c.h"

#include "support/mpu6050.h"


static const char *TAG = "i2c";

static bool s_i2c_configured = false;


/**
 * @brief i2c master initialization
 */
esp_err_t configure_I2C(void)
{
    int i2c_master_port = I2C_MASTER_NUM;

    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };

    i2c_param_config(i2c_master_port, &conf);

    s_i2c_configured = true;

    return i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
}


void i2c_read_byte(uint8_t deviceAddress, uint8_t registerAddress, uint8_t *data) {
    i2c_master_write_read_device(I2C_MASTER_NUM, deviceAddress, &registerAddress, 1, data, 1, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
    return;
}

void i2c_write_byte(uint8_t deviceAddress, uint8_t registerAddress, uint8_t data) {
    uint8_t buffer[2];
    buffer[0] = registerAddress;
    buffer[1] = data;
    i2c_master_write_to_device(I2C_MASTER_NUM, deviceAddress, buffer, 2, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
    return;
}




void i2c_test(void) {
    if (!s_i2c_configured) {
        ESP_LOGE(TAG, "I2C Not Configured");
        return;
    }


    uint8_t data;
    i2c_read_byte(MPU6050_ADDR, MPU6050_WHO_AM_I, &data);
    ESP_LOGI(TAG, "WHO_AM_I: %x", data);
    if (data == 0x68) {
        ESP_LOGI(TAG, "MPU6050 Detected");
        mpu_startup_seq();
    } else {
        ESP_LOGE(TAG, "MPU6050 Not Detected");
        set_color_instant(255, 0, 0);
    }
    return;
}

void mpu_startup_seq(void) {
    uint8_t data;
    i2c_write_byte(MPU6050_ADDR, MPU6050_SMPLRT_DIV, MPU6050_SMPLRT_DIV_VAL);
    i2c_write_byte(MPU6050_ADDR, MPU6050_CONFIG, 0x00);
    i2c_read_byte(MPU6050_ADDR, 0x19, &data);
    ESP_LOGI(TAG, "0x44: %d", data);
    return;
}
