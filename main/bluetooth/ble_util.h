#pragma once

char *esp_key_type_to_str(esp_ble_key_type_t key_type);

char *esp_auth_req_to_str(esp_ble_auth_req_t auth_req);

void show_bonded_devices(void);

void remove_all_bonded_devices(void);

uint8_t *load_string_as_uint8(const char *input_string);
