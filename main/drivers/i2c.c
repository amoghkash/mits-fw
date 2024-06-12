#include <stdio.h>
#include "driver/i2c_master.h"
#include "driver/i2c_types.h"
#include "esp_log.h"

#include "i2c.h"
#include "board.h"

#define TAG "I2C"

// I2C Configuration
static i2c_master_bus_handle_t i2c_bus_handle;

i2c_master_bus_config_t i2c_config = {
    .clk_source = I2C_CLK_SRC_DEFAULT,
    .i2c_port = I2C_PORT_NUM,
    .scl_io_num = I2C_SCL,
    .sda_io_num = I2C_SDA,
    .glitch_ignore_cnt = 7,
    .flags.enable_internal_pullup = true,
    .intr_priority = 2
};

esp_err_t i2c_init(void) {
    esp_err_t ret = i2c_new_master_bus(&i2c_config, &i2c_bus_handle);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to initialize I2C master: %s", esp_err_to_name(ret));
    }
    ESP_LOGI(TAG, "I2C master initialized successfully");
    return ret;
}

i2c_master_bus_handle_t* i2c_get_bus_handle(void) {
    return &i2c_bus_handle;
}

/// @brief Add an I2C device to the bus
/// @param device_config 
/// @return The device handle to control the device with
/// TODO: ADD CH
i2c_master_dev_handle_t i2c_add_device(i2c_device_config_t* device_config) {
    i2c_master_dev_handle_t dev_handle;;
    esp_err_t ret = i2c_master_bus_add_device(i2c_bus_handle, device_config, &dev_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to add I2C device: %s", esp_err_to_name(ret));
    }
    ESP_LOGI(TAG, "I2C device added successfully");
    return dev_handle;
}

/// @brief Check if the I2C device is connected to the bus
/// @param device_address 
/// @return esp_err_t
esp_err_t i2c_probe(uint8_t device_address) {
    ESP_LOGI(TAG, "Probing I2C device at address 0x%02X", device_address);
    return i2c_master_probe(i2c_bus_handle, device_address, 1000);
}