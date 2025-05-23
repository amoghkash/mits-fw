#pragma once

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t ble_server_init(void);
esp_err_t ble_server_set_serial_number(const char *serial);
esp_err_t ble_server_set_sensor_data(const char *data);

#ifdef __cplusplus
}
#endif