#pragma once

#include <string>
#include "config.h"
#include "esp_err.h"


class FileSystem {
    private:
        const char *registryFilePath = "/data/registry.txt";
        uint8_t numFiles;
        std::string files[MAX_FILE_COUNT];

    public:
        FileSystem();
        esp_err_t initialize(void);
        esp_err_t format(void);

        esp_err_t checkRegistryFile(void);
        esp_err_t createRegistryFile(void);
        esp_err_t readRegistryFile(void);
        esp_err_t addToRegistryFile(const char *path);

        esp_err_t createFile(const char *path, const char *data);
        esp_err_t createFile(const char *path);
        //esp_err_t readFile(const char *path, char *buffer, size_t buffer_size);
        void printFiles(void);
};
