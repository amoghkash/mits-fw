#include <stdio.h>
#include "string"
#include "cstring"

#include "esp_system.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"

#include "system/fs.h"
#include "system/system.h"

#include "storage.h"

#define TAG "FileSystem"

FileSystem::FileSystem()
{
/*     std::string base = "created";
    char* array = new char[base.length() + 1];
    strcpy(array, base.c_str());
    printf("FSManager %s\n", array); */
}

// Basic File System Functions
esp_err_t FileSystem::initialize(void) {
    ESP_LOGI(TAG, "Initializing FSManager...");
    if(!is_storage_initialized()) {
        return initialize_storage();
    }
    ESP_LOGI(TAG, "FSManager initialized.");

    // Read from Registry File
    return ESP_OK;
}

esp_err_t FileSystem::format(void) {
    ESP_LOGI(TAG, "Formatting File System...");
    return format_fs();
}

// Registry File Functions
esp_err_t FileSystem::checkRegistryFile(void) {
    ESP_LOGI(TAG, "Checking Registry File...");
    // Check if file exists
    FILE *file = fopen(registryFilePath, "r");
    if (file == NULL) {
        ESP_LOGI(TAG, "Registry File does not exist.");
        return createRegistryFile();
    }

    ESP_LOGI(TAG, "Registry File exists.");
    return ESP_OK;
}

esp_err_t FileSystem::createRegistryFile(void) {
    ESP_LOGI(TAG, "Creating Registry File...");
    // Create file
    return create_file(registryFilePath, "");
}

esp_err_t FileSystem::readRegistryFile(void) {
    esp_err_t ret;
    ESP_LOGI(TAG, "Reading Registry File...");

    ret = checkRegistryFile();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error reading Registry File.");
        return ret;
    }
    // Read file contents
    FILE *file = fopen(registryFilePath, "r");
    if (file == NULL) {
        ESP_LOGE(TAG, "Error opening Registry File.");
        return ESP_FAIL;
    }

    char line[FILENAME_MAX_SIZE];
    uint8_t numFiles = 0;
    while (fgets(line, sizeof(line), file)) {
        if (strcmp(line, "\0") == 0) {
            break;
        }
        // Remove newline character
        //line[strcspn(line, "\n")] = 0;
        // Add file to list
        files[numFiles] = line;
        printf("File %d: %s\n", numFiles, line);
        numFiles++;
    }
    return ESP_OK;
}

esp_err_t FileSystem::addToRegistryFile(const char *path) {
    ESP_LOGI(TAG, "Adding %s to Registry File.", path);
    // Create file
    return add_data_to_file(registryFilePath, path);
}

void FileSystem::printFiles(void) {
    for (uint8_t i = 0; i < sizeof(files)/sizeof(files[0]); i++) {
        printf("File %d: %s\n", i, files[i].c_str());
    }
}

// File Functions
esp_err_t FileSystem::createFile(const char *path, const char *data) {
    esp_err_t ret;
    ESP_LOGI(TAG, "Creating file %s", path);
    
    // Create the File
    ret = create_file(path, data);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error creating %s.", path);
        return ret;
    }

    // Register File
    ret = addToRegistryFile(path);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error adding %s to Registry File.", path);
        return ret;
    }
    
    return ret;
}

