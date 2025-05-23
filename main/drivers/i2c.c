#include <stdio.h>

#include "esp_log.h"
#include "esp_console.h"
#include "argtable3/argtable3.h"

#include "driver/i2c_master.h"
#include "driver/i2c_types.h"

#include "i2c.h"
#include "mits_board.h"

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

esp_err_t i2c_initialize(void) {
    esp_err_t ret = i2c_new_master_bus(&i2c_config, &i2c_bus_handle);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to initialize I2C master: %s", esp_err_to_name(ret));
    }
    ESP_LOGD(TAG, "I2C master initialized successfully");
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
    ESP_LOGD(TAG, "I2C device added successfully");
    return dev_handle;
}

/// @brief Check if the I2C device is connected to the bus
/// @param device_address 
/// @return esp_err_t
esp_err_t i2c_probe(uint8_t device_address) {
    ESP_LOGI(TAG, "Probing I2C device at address 0x%02X", device_address);
    return i2c_master_probe(i2c_bus_handle, device_address, 1000);
}


// CONSOLE FUNCTIONS
#ifdef CONFIG_I2C_CONSOLE_COMMANDS
    static int i2c_status_console(int argc, char **argv) {
        ESP_LOGI(TAG, "I2C Bus Status:");
        ESP_LOGI(TAG, "SCL: %d", i2c_config.scl_io_num);
        ESP_LOGI(TAG, "SDA: %d", i2c_config.sda_io_num);
        ESP_LOGI(TAG, "Glitch Ignore Count: %d", i2c_config.glitch_ignore_cnt);
        ESP_LOGI(TAG, "Internal Pullup: %s", i2c_config.flags.enable_internal_pullup ? "Enabled" : "Disabled");
        ESP_LOGI(TAG, "Interrupt Priority: %d", i2c_config.intr_priority);
        return 0;
    }

    static struct {
        struct arg_int *device_address;
        struct arg_int *register_address;
        struct arg_int *data;
        struct arg_end *end;
    } i2c_write_args;

    static int i2c_write_console(int argc, char **argv) {
        int nerrors = arg_parse(argc, argv, (void **)&i2c_write_args);
        if (nerrors != 0) {
            arg_print_errors(stderr, i2c_write_args.end, argv[0]);
            return 1;
        }

        uint8_t device_address = i2c_write_args.device_address->ival[0];
        uint8_t register_address = i2c_write_args.register_address->ival[0];
        uint8_t data = i2c_write_args.data->ival[0];

        i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = device_address,
        .scl_speed_hz = 400000,
        };

        i2c_master_dev_handle_t dev_handle;
        ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_master_bus_add_device(i2c_bus_handle, &dev_cfg, &dev_handle));
        ESP_ERROR_CHECK(i2c_master_transmit(dev_handle, &register_address, sizeof(register_address), 1000));
        ESP_ERROR_CHECK(i2c_master_transmit(dev_handle, &data, sizeof(data), 1000));
        ESP_ERROR_CHECK(i2c_master_bus_rm_device(dev_handle));
        return 0;
    }

    static struct {
        struct arg_int *device_address;
        struct arg_int *register_address;
        struct arg_end *end;
    } i2c_read_args;

    static int i2c_read_console(int argc, char **argv) {
        int nerrors = arg_parse(argc, argv, (void **)&i2c_read_args);
        if (nerrors != 0) {
            arg_print_errors(stderr, i2c_read_args.end, argv[0]);
            return 1;
        }

        uint8_t device_address = i2c_read_args.device_address->ival[0];
        uint8_t register_address = i2c_read_args.register_address->ival[0];

        i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = device_address,
        .scl_speed_hz = 400000,
        };

        i2c_master_dev_handle_t dev_handle;
        uint8_t readbuf[2];
        ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_master_bus_add_device(i2c_bus_handle, &dev_cfg, &dev_handle));
        ESP_ERROR_CHECK(i2c_master_transmit_receive(dev_handle, &register_address, sizeof(register_address),
                                                                readbuf, sizeof(readbuf), 1000));
        printf("Read data: %02X %02X\n", readbuf[0], readbuf[1]);
        ESP_ERROR_CHECK(i2c_master_bus_rm_device(dev_handle));
        return 0;
    }
#endif

void register_i2c_commands() {
    #ifdef CONFIG_I2C_CONSOLE_COMMANDS
        i2c_write_args.device_address = arg_int0("d", "device_address", "<d>", "I2C Device Address");
        i2c_write_args.register_address = arg_int1("r", "register_address", "<r>", "Register Address");
        i2c_write_args.data = arg_intn("w", "data", "<w>", 0, 3, "Data to write");
        i2c_write_args.end = arg_end(1);

        i2c_read_args.device_address = arg_int0("d", "device_address", "<d>", "I2C Device Address");
        i2c_read_args.register_address = arg_int1("r", "register_address", "<r>", "Register Address");
        i2c_read_args.end = arg_end(1);

        const esp_console_cmd_t i2c_status_cmd = {
            .command = "i2c_status",
            .help = "Show I2C bus status",
            .hint = NULL,
            .func = &i2c_status_console,
            .argtable = NULL,
        };
        ESP_ERROR_CHECK(esp_console_cmd_register(&i2c_status_cmd));

        const esp_console_cmd_t i2c_write_cmd = {
            .command = "i2c_write",
            .help = "Write data to I2C device",
            .hint = NULL,
            .func = &i2c_write_console,
            .argtable = &i2c_write_args,
        };
        ESP_ERROR_CHECK(esp_console_cmd_register(&i2c_write_cmd));

        const esp_console_cmd_t i2c_read_cmd = {
            .command = "i2c_read",
            .help = "Read data from I2C device",
            .hint = NULL,
            .func = &i2c_read_console,
            .argtable = &i2c_read_args,
        };
        ESP_ERROR_CHECK(esp_console_cmd_register(&i2c_read_cmd));
    #endif
}