#include <stdio.h>

#include "esp_system.h"

#include "system/fs.h"

#include "storage.h"

#define TAG "storage"

esp_err_t test_cpp(void) {
    printf("Test C++\n");
    return ESP_OK;
}