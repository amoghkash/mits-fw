#ifndef BLE_H
#define BLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include "esp_err.h"
#include "esp_gatt_defs.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt.h"
#include "esp_bt_main.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PROFILE_NUM                 1
#define IMPACT_APP_IDX              0
#define ESP_APP_ID                  0x55
#define SAMPLE_DEVICE_NAME          "SirahBand"
#define SVC_INST_ID                 0
#define MAX_GATTS_NUM_HANDLE        20
/* The max length of characteristic value. When the GATT client performs a write or prepare write operation,
*  the data length must be less than GATTS_DEMO_CHAR_VAL_LEN_MAX.
*/
#define GATTS_DEMO_CHAR_VAL_LEN_MAX 500
#define PREPARE_BUF_MAX_SIZE        1024
#define CHAR_DECLARATION_SIZE       (sizeof(uint8_t))

#define ADV_CONFIG_FLAG             (1 << 0)
#define SCAN_RSP_CONFIG_FLAG        (1 << 1)

// BLE Types
struct gatts_profile_inst {
    esp_gatts_cb_t gatts_cb;
    uint16_t gatts_if;
    uint16_t app_id;
    uint16_t conn_id;
    uint16_t service_handle;
    esp_gatt_srvc_id_t service_id;
    uint16_t char_handle;
    esp_bt_uuid_t char_uuid;
    esp_gatt_perm_t perm;
    esp_gatt_char_prop_t property;
    uint16_t descr_handle;
    esp_bt_uuid_t descr_uuid;
};

typedef struct {
    uint8_t                 *prepare_buf;
    int                     prepare_len;
} prepare_type_env_t;

enum
{
    IDX_SVC,
    IDX_CHAR_A,
    IDX_CHAR_VAL_A,

    IMPACT_IDX_NB,
};



// esp_attr_desc_t is type for attribute descriptor

// BLE GAP Characteristics
static uint8_t raw_adv_data[] = {
    /* Flags */
    0x02, ESP_BLE_AD_TYPE_FLAG, 0x06,
    /* TX Power Level */
    0x02, ESP_BLE_AD_TYPE_TX_PWR, 0xEB,
    /* Complete 16-bit Service UUIDs */
    0x03, ESP_BLE_AD_TYPE_16SRV_CMPL, 0xFF, 0x00,
    /* Complete Local Name */
    0x0A, ESP_BLE_AD_TYPE_NAME_CMPL,
    'S', 'I', 'R', 'A', 'H', 'B', 'A', 'N', 'D',
    /* Manufacturer Specific Data */
    0x0A, ESP_BLE_AD_MANUFACTURER_SPECIFIC_TYPE,
    'S', 'i', 'r', 'a', 'h', 'T', 'e', 'c', 'h'
};

static uint8_t raw_scan_rsp_data[] = {
    /* Flags */
    0x02, ESP_BLE_AD_TYPE_FLAG, 0x06,
    /* TX Power Level */
    0x02, ESP_BLE_AD_TYPE_TX_PWR, 0xEB,
    /* Complete 16-bit Service UUIDs */
    0x03, ESP_BLE_AD_TYPE_16SRV_CMPL, 0xFF, 0x00,
    /* Manufacturer Specific Data */
    0x0A, ESP_BLE_AD_MANUFACTURER_SPECIFIC_TYPE,
    'S', 'i', 'r', 'a', 'h', 'T', 'e', 'c', 'h'
};

static esp_ble_adv_params_t adv_params = {
    .adv_int_min         = 0x20,
    .adv_int_max         = 0x40,
    .adv_type            = ADV_TYPE_IND,
    .own_addr_type       = BLE_ADDR_TYPE_PUBLIC,
    .channel_map         = ADV_CHNL_ALL,
    .adv_filter_policy   = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};

void gap_event_handler(esp_gap_ble_cb_event_t event, 
                              esp_ble_gap_cb_param_t *param)


// BLE GATT Characteristics
void gatts_profile_event_handler(esp_gatts_cb_event_t event,
					esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);




// BLE Server API

/**
 * @brief Initialize the BLE server and stack.
 * @param device_name The device name to advertise.
 * @return ESP_OK on success, error code otherwise.
 */
void ble_initialize();
#ifdef __cplusplus
}
#endif

#endif // BLE_SERVER_H


