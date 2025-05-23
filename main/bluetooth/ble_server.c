#include "ble_server.h"
#include "esp_log.h"
#include "esp_bt.h"
#include "esp_gatts_api.h"
#include "esp_gap_ble_api.h"
#include "esp_bt_main.h"
#include "esp_gatt_common_api.h"

#include <string.h>

#define TAG "BLE_SERVER"
// ===== BLE CONSTANTS & CONFIGS =====

static const uint16_t SERVICE_UUID        = 0x00FF;
static const uint16_t CHAR_SERIAL_UUID    = 0xFF01;
static const uint16_t CHAR_SENSOR_UUID    = 0xFF02;

static const uint8_t char_prop_read = ESP_GATT_CHAR_PROP_BIT_READ;
static const uint16_t primary_service_uuid = ESP_GATT_UUID_PRI_SERVICE;
static const uint16_t character_declaration_uuid = ESP_GATT_UUID_CHAR_DECLARE;

// ===== ADV DATA CONFIG =====

static esp_ble_adv_params_t adv_params = {
    .adv_int_min        = 0x20,
    .adv_int_max        = 0x40,
    .adv_type           = ADV_TYPE_IND, // Connectable undirected advertising
    .own_addr_type      = BLE_ADDR_TYPE_PUBLIC,
    .channel_map        = ADV_CHNL_ALL,
    .adv_filter_policy  = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};

static uint8_t manufacturer_data[] = { 'S', 'I', 'R', 'A', 'H' };

static esp_ble_adv_data_t adv_data = {
    .set_scan_rsp        = false, // Advertising packet: only manufacturer data
    .include_name        = false, // Device name will be in scan response
    .include_txpower     = true,
    .min_interval        = 0x0006,
    .max_interval        = 0x0010,
    .appearance          = 0x00,
    .manufacturer_len    = sizeof(manufacturer_data),
    .p_manufacturer_data = manufacturer_data,
    .service_data_len    = 0,
    .p_service_data      = NULL,
    .service_uuid_len    = 0,
    .p_service_uuid      = NULL,
    .flag                = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT),
};

// Add scan response data with device name
static esp_ble_adv_data_t scan_rsp_data = {
    .set_scan_rsp        = true,
    .include_name        = true, // Device name in scan response
    .include_txpower     = false,
    .manufacturer_len    = 0,
    .p_manufacturer_data = NULL,
    .service_data_len    = 0,
    .p_service_data      = NULL,
    .service_uuid_len    = 0,
    .p_service_uuid      = NULL,
    .flag                = 0,
};

#define GATTS_NUM_HANDLE    6

static uint16_t service_handle;
static esp_gatt_if_t gatt_if;
static uint16_t conn_id = 0xFFFF;

static uint16_t serial_char_handle;
static uint16_t sensor_char_handle;

static uint8_t serial_val[32] = "SIRAH-001";
static uint8_t sensor_val[64] = "X:0,Y:0,Z:0";

static esp_gatts_attr_db_t gatt_db[GATTS_NUM_HANDLE] = {
    // Service Declaration
    [0] = {
        {ESP_GATT_AUTO_RSP},
        {ESP_UUID_LEN_16, (uint8_t *)&primary_service_uuid, ESP_GATT_PERM_READ,
         sizeof(uint16_t), sizeof(SERVICE_UUID), (uint8_t *)&SERVICE_UUID}
    },
    // Serial Number Characteristic Declaration
    [1] = {
        {ESP_GATT_AUTO_RSP},
        {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid, ESP_GATT_PERM_READ,
         sizeof(uint8_t), sizeof(uint8_t), (uint8_t *)&char_prop_read}
    },
    // Serial Number Characteristic Value
    [2] = {
        {ESP_GATT_AUTO_RSP},
        {ESP_UUID_LEN_16, (uint8_t *)&CHAR_SERIAL_UUID, ESP_GATT_PERM_READ,
         sizeof(serial_val), sizeof(serial_val), serial_val}
    },
    // Sensor Data Characteristic Declaration
    [3] = {
        {ESP_GATT_AUTO_RSP},
        {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid, ESP_GATT_PERM_READ,
         sizeof(uint8_t), sizeof(uint8_t), (uint8_t *)&char_prop_read}
    },
    // Sensor Data Characteristic Value
    [4] = {
        {ESP_GATT_AUTO_RSP},
        {ESP_UUID_LEN_16, (uint8_t *)&CHAR_SENSOR_UUID, ESP_GATT_PERM_READ,
         sizeof(sensor_val), sizeof(sensor_val), sensor_val}
    }
};

static void gatts_cb(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if,
                     esp_ble_gatts_cb_param_t *param) {
    ESP_LOGI(TAG, "GATT Callback Event: %d", event);
    switch (event) {
        case ESP_GATTS_REG_EVT:
            ESP_LOGI(TAG, "GATT Registered");
            esp_ble_gap_set_device_name("Sirah-Device");
            esp_ble_gatts_create_attr_tab(gatt_db, gatts_if, GATTS_NUM_HANDLE, 0);
            break;
        case ESP_GATTS_CREAT_ATTR_TAB_EVT:
            if (param->add_attr_tab.status == ESP_GATT_OK) {
                service_handle = param->add_attr_tab.handles[0];
                serial_char_handle = param->add_attr_tab.handles[2];
                sensor_char_handle = param->add_attr_tab.handles[4];
                esp_ble_gatts_start_service(service_handle);
            }
            break;
        case ESP_GATTS_CONNECT_EVT:
            conn_id = param->connect.conn_id;
            break;
        case ESP_GATTS_DISCONNECT_EVT:
            conn_id = 0xFFFF;
            esp_ble_gap_start_advertising(&adv_params);
            break;
        default:
            break;
    }
}

static void gap_cb(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param) {
    if (event == ESP_GAP_BLE_ADV_START_COMPLETE_EVT) {
        if (param->adv_start_cmpl.status != ESP_BT_STATUS_SUCCESS) {
            ESP_LOGE(TAG, "Failed to start advertising");
        }
    }
}

// Public API
esp_err_t ble_server_init(void) {
    ESP_LOGI(TAG, "Initializing BLE Server...");
    esp_err_t ret = esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT);
    if (ret) return ret;

    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ret = esp_bt_controller_init(&bt_cfg);
    if (ret) return ret;

    ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
    if (ret) return ret;

    ret = esp_bluedroid_init();
    if (ret) return ret;

    ret = esp_bluedroid_enable();
    if (ret) return ret;

    ret = esp_ble_gatts_register_callback(gatts_cb);
    if (ret) return ret;

    ret = esp_ble_gap_register_callback(gap_cb);
    if (ret) return ret;

    // Configure advertisement data
    ret = esp_ble_gap_config_adv_data(&adv_data);
    if (ret) return ret;
    ret = esp_ble_gap_config_adv_data(&scan_rsp_data);
    if (ret) return ret;


    return esp_ble_gatts_app_register(0);
}

esp_err_t ble_server_set_serial_number(const char *serial) {
    snprintf((char *)serial_val, sizeof(serial_val), "%s", serial);
    return ESP_OK;
}

esp_err_t ble_server_set_sensor_data(const char *data) {
    snprintf((char *)sensor_val, sizeof(sensor_val), "%s", data);
    if (conn_id != 0xFFFF) {
        esp_ble_gatts_set_attr_value(sensor_char_handle, strlen((char *)sensor_val), sensor_val);
    }
    return ESP_OK;
}
