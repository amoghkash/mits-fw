#include "nvs.h"
#include "nvs_flash.h"

#include "nvs_system.h"



esp_err_t initialize_nvs(void) {
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK( nvs_flash_erase() );
        ret = nvs_flash_init();
        return ret;
    }
    return ret;
}