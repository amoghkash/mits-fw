#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_err.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_littlefs.h"

#include "fs.h"

#define TAG "fs"
#define FILE_BUFFER_SIZE 256


const char* base_path = "/data";
const char* partition_label = "storage";

/**
 * @brief Initialize FileSystem
 * 
 * @return esp_err_t ESP_OK if successful, respective fail code otherwise
 */
esp_err_t initialize_fs(void) {
    esp_vfs_littlefs_conf_t conf = {
    .base_path = base_path,
    .partition_label = partition_label,
    .format_if_mount_failed = true,
    .dont_mount = false,
    };

    ESP_LOGD(TAG, "Initializing LittleFS");

    // Register FS
    esp_err_t ret = esp_vfs_littlefs_register(&conf);
    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            ESP_LOGE(TAG, "Failed to find LittleFS partition");
        } else {
            ESP_LOGE(TAG, "Failed to initialize LittleFS (%s)", esp_err_to_name(ret));
        }
        return ret;
    }

    // Get partition info
    size_t total = 0, used = 0;
    ret = esp_littlefs_info(conf.partition_label, &total, &used);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get LittleFS partition information (%s)", esp_err_to_name(ret));
        ret = esp_littlefs_format(conf.partition_label);
        return ret;
    } else {
        ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
    }

    return ESP_OK;
}


/**
 * @brief Deinitialize FileSystem
 * 
 * @return esp_err_t ESP_OK if successful
 */
esp_err_t deinitialize_fs(void) {
    ESP_LOGD(TAG, "LittleFS unmounting...");
    esp_err_t ret = esp_vfs_littlefs_unregister(partition_label);
    return ret;
}

/**
 * @brief Create a file
 * 
 * @param filename      Path of the file to create
 * @param data          Data to write to the file 
 * 
 * @return esp_err_t    ESP_OK if successful, ESP_FAIL otherwise
 */
esp_err_t create_file(const char* filename, const char* data) {
    FILE* f = fopen(filename, "w");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file for writing");
        return ESP_FAIL;
    }
    fprintf(f, data);
    fclose(f);
    return ESP_OK;
}

/**
 * @brief Read a file
 * 
 * @param filename      Path of the file to read
 * 
 * @return esp_err_t    ESP_OK if successful, ESP_FAIL otherwise
 */
esp_err_t readout_file(const char* filename) {
    FILE* f = fopen(filename, "r");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file for reading");
        return ESP_FAIL;
    }
    char line[FILE_BUFFER_SIZE];
    while (fgets(line, sizeof(line), f)) {
        printf("%s", line);
    }
    fclose(f);
    return ESP_OK;
}

/**
 * @brief Read a file
 * 
 * @param filename      Path of the file to read
 * @param buffer        Buffer to store the file contents
 * @param buffer_size   Size of the buffer
 * 
 * @return esp_err_t    ESP_OK if successful, ESP_FAIL otherwise
 */
esp_err_t read_file(const char* filename, char* buffer, size_t buffer_size) {
    if (buffer == NULL) {
        ESP_LOGE(TAG, "Buffer is NULL");
        return ESP_ERR_INVALID_ARG;
    }
    FILE* f = fopen(filename, "r");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file for reading");
        return ESP_FAIL;
    }
    fread(buffer, sizeof(char), buffer_size, f);
    fclose(f);
    return ESP_OK;
}

/**
 * @brief Add data to a file
 * 
 * @param filename      Path of the file to append data to
 * @param data          Data to append to the file
 * 
 * @return esp_err_t    ESP_OK if successful, ESP_FAIL otherwise
 */
esp_err_t add_data_to_file(const char* filename, const char* data) {
    FILE* f = fopen(filename, "a");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file for appending");
        return ESP_FAIL;
    }
    fprintf(f, data);
    fclose(f);
    return ESP_OK;
}

/**
 * @brief Rename a file
 * 
 * @param old_filename      Path of the file to rename
 * @param new_filename      New path of the file
 * 
 * @return esp_err_t        ESP_OK if successful, ESP_ERR_NOT_ALLOWED if the new file already exists, 
 *                          ESP_FAIL otherwise
 */
esp_err_t rename_file(const char* old_filename, const char* new_filename) {
    struct stat st;
    if (stat(new_filename, &st) == 0) {
        ESP_LOGE(TAG, "File %s already exists", new_filename);
        return ESP_ERR_NOT_ALLOWED;
    }

    if (rename(old_filename, new_filename) != 0) {
        ESP_LOGE(TAG, "Failed to rename file");
        return ESP_FAIL;
    }
    ESP_LOGD(TAG, "File renamed from %s to %s", old_filename, new_filename);
    return ESP_OK;
}

/**
 * @brief Delete a file
 * 
 * @param filename      Path of the file to delete
 * 
 * @return esp_err_t    ESP_OK if successful, ESP_FAIL otherwise
 */
esp_err_t delete_file(const char* filename) {
    if (unlink(filename) != 0) {
        ESP_LOGE(TAG, "Failed to delete file");
        return ESP_FAIL;
    }
    ESP_LOGD(TAG, "File deleted: %s", filename);
    return ESP_OK;
}