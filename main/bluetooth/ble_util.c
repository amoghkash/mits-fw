#include "esp_gap_ble_api.h"
#include "esp_log.h"

#include "ble_util.h"

#define TAG "BLE_UTIL"

/**
 * @brief Convert esp_ble_key_type_t to string representation.
 */
char *esp_key_type_to_str(esp_ble_key_type_t key_type) {
   char *key_str = NULL;
   switch(key_type) {
    case ESP_LE_KEY_NONE:
        key_str = "ESP_LE_KEY_NONE";
        break;
    case ESP_LE_KEY_PENC:
        key_str = "ESP_LE_KEY_PENC";
        break;
    case ESP_LE_KEY_PID:
        key_str = "ESP_LE_KEY_PID";
        break;
    case ESP_LE_KEY_PCSRK:
        key_str = "ESP_LE_KEY_PCSRK";
        break;
    case ESP_LE_KEY_PLK:
        key_str = "ESP_LE_KEY_PLK";
        break;
    case ESP_LE_KEY_LLK:
        key_str = "ESP_LE_KEY_LLK";
        break;
    case ESP_LE_KEY_LENC:
        key_str = "ESP_LE_KEY_LENC";
        break;
    case ESP_LE_KEY_LID:
        key_str = "ESP_LE_KEY_LID";
        break;
    case ESP_LE_KEY_LCSRK:
        key_str = "ESP_LE_KEY_LCSRK";
        break;
    default:
        key_str = "INVALID BLE KEY TYPE";
        break;

   }

   return key_str;
}

/**
 * @brief Convert esp_ble_auth_req_t to string representation.
 */
char *esp_auth_req_to_str(esp_ble_auth_req_t auth_req) {
   char *auth_str = NULL;
   switch(auth_req) {
    case ESP_LE_AUTH_NO_BOND:
        auth_str = "ESP_LE_AUTH_NO_BOND";
        break;
    case ESP_LE_AUTH_BOND:
        auth_str = "ESP_LE_AUTH_BOND";
        break;
    case ESP_LE_AUTH_REQ_MITM:
        auth_str = "ESP_LE_AUTH_REQ_MITM";
        break;
    case ESP_LE_AUTH_REQ_BOND_MITM:
        auth_str = "ESP_LE_AUTH_REQ_BOND_MITM";
        break;
    case ESP_LE_AUTH_REQ_SC_ONLY:
        auth_str = "ESP_LE_AUTH_REQ_SC_ONLY";
        break;
    case ESP_LE_AUTH_REQ_SC_BOND:
        auth_str = "ESP_LE_AUTH_REQ_SC_BOND";
        break;
    case ESP_LE_AUTH_REQ_SC_MITM:
        auth_str = "ESP_LE_AUTH_REQ_SC_MITM";
        break;
    case ESP_LE_AUTH_REQ_SC_MITM_BOND:
        auth_str = "ESP_LE_AUTH_REQ_SC_MITM_BOND";
        break;
    default:
        auth_str = "INVALID BLE AUTH REQ";
        break;
   }
   return auth_str;
}

/**
 * @brief Show bonded devices.
 * This function retrieves and displays the list of bonded devices.
 */
void show_bonded_devices(void) {
    int dev_num = esp_ble_get_bond_device_num();
    if (dev_num == 0) {
        ESP_LOGI(TAG, "Bonded devices number zero\n");
        return;
    }

    esp_ble_bond_dev_t *dev_list = (esp_ble_bond_dev_t *)malloc(sizeof(esp_ble_bond_dev_t) * dev_num);
    if (!dev_list) {
        ESP_LOGE(TAG, "malloc failed\n");
        return;
    }
    esp_ble_get_bond_device_list(&dev_num, dev_list);
    ESP_LOGI(TAG, "Bonded devices number %d", dev_num);
    for (int i = 0; i < dev_num; i++) {
        ESP_LOGI(TAG, "[%u] addr_type %u, addr "ESP_BD_ADDR_STR"",
                 i, dev_list[i].bd_addr_type, ESP_BD_ADDR_HEX(dev_list[i].bd_addr));
    }

    free(dev_list);
}

/**
 * @brief Remove all bonded devices.
 * This function removes all bonded devices from the device.
 */
void remove_all_bonded_devices(void)
{
    int dev_num = esp_ble_get_bond_device_num();
    if (dev_num == 0) {
        ESP_LOGI(TAG, "Bonded devices number zero\n");
        return;
    }

    esp_ble_bond_dev_t *dev_list = (esp_ble_bond_dev_t *)malloc(sizeof(esp_ble_bond_dev_t) * dev_num);
    if (!dev_list) {
        ESP_LOGE(TAG, "malloc failed\n");
        return;
    }
    esp_ble_get_bond_device_list(&dev_num, dev_list);
    for (int i = 0; i < dev_num; i++) {
        esp_ble_remove_bond_device(dev_list[i].bd_addr);
    }

    free(dev_list);
    ESP_LOGI(TAG, "All bonded devices removed\n");
}


uint8_t *load_string_as_uint8(const char *input_string) {
    // Allocate memory for the uint8_t array
    size_t length = strlen(input_string);
    uint8_t *output = (uint8_t *)malloc(length * sizeof(uint8_t));
    if (output == NULL) {
        printf("Memory allocation failed\n");
        return NULL;
    }

    // Copy the string into the uint8_t array
    memcpy(output, input_string, length);

    return output;
}