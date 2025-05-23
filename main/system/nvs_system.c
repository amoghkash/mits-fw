// Library Imports
#include "esp_log.h"
#include "nvs.h"
#include "nvs_flash.h"

#include "nvs_system.h"

#define TAG "NVS"

static nvs_handle_t handle;

static const char* namespace = "info";

void nvs_initialize(void) {
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK( nvs_flash_erase() );
        ret = nvs_flash_init();
        ESP_ERROR_CHECK(ret);
    }

    ret = nvs_open(namespace, NVS_READWRITE, &handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to open NVS: %s", esp_err_to_name(ret));
        ESP_ERROR_CHECK(ret);
    }
    ESP_LOGD(TAG, "NVS initialized successfully");
}

void nvs_deinitialize(void) {
    esp_err_t ret = nvs_flash_deinit();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to deinitialize NVS: %s", esp_err_to_name(ret));
        ESP_ERROR_CHECK(ret);
    }
    ESP_LOGD(TAG, "NVS deinitialized successfully");
}

/**
 * @brief Retrieves a string from NVS.
 *
 * @param key The key to retrieve the string from.
 * @return A pointer to the retrieved string, or NULL if not found.
 * @note The caller is responsible for freeing the returned string.
 *       Use free() to deallocate the memory when done.
 */
char* nvs_get_string(const char *key) {
    size_t required_size;
    esp_err_t ret = nvs_get_str(handle, key, NULL, &required_size);
    if (ret == ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGW(TAG, "Key '%s' not found in NVS", key);
        return NULL;
    } else if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get string from NVS: %s", esp_err_to_name(ret));
        return NULL;
    }

    char* buffer = malloc(required_size);
    if (buffer == NULL) {
        ESP_LOGE(TAG, "Failed to allocate memory for response");
        return NULL;
    }

    nvs_get_str(handle, key, buffer, &required_size);
    return buffer;
}

/**
 * @brief Iterates over all NVS string entries in the current namespace and calls a callback for each.
 *
 * @param callback Function to call for each string entry. Receives key and value.
 * @param user_ctx User context pointer passed to the callback.
 */
void nvs_iterate_strings(void) {
    // Example of listing all the key-value pairs of any type under specified partition and namespace
    nvs_iterator_t it = NULL;
    esp_err_t res = nvs_entry_find("nvs", namespace, NVS_TYPE_ANY, &it);
    while(res == ESP_OK) {
        nvs_entry_info_t info;
        nvs_entry_info(it, &info); // Can omit error check if parameters are guaranteed to be non-NULL
        printf("key '%s', type '%d' \n", info.key, info.type);
        res = nvs_entry_next(&it);
    }
    nvs_release_iterator(it);
}

