#pragma once
#ifdef __cplusplus
    extern "C" {
#endif

#include "board.h"
#include "driver/i2c_master.h"
#include "driver/i2c_types.h"


#define I2C_PORT_NUM         0



struct i2c_device {
    char* name;
    uint8_t device_address;
    i2c_master_dev_handle_t device_handle;
    bool connected;
    bool configured;
};


esp_err_t initialize_i2c(void);
i2c_master_bus_handle_t* i2c_get_bus_handle(void);
i2c_master_dev_handle_t i2c_add_device(i2c_device_config_t* device_config);
esp_err_t i2c_probe(uint8_t device_address);

#ifdef CONFIG_I2C_CONSOLE_COMMANDS
    void register_i2c_commands();
#endif

#ifdef __cplusplus
    }
#endif