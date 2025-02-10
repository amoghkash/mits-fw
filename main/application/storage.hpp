#pragma once


#include "config.h"
#include "esp_err.h"
#include <string>
#include "cstring"

class FileSystem {
    private:
        uint8_t numFiles;
        std::string files[MAX_FILE_COUNT];
        bool initialized = false;

    public:
        const char *registryFilePath = "/data/registry.txt";

        FileSystem();
        esp_err_t initialize(void);
        esp_err_t format(void);

        esp_err_t checkRegistryFile(void);
        esp_err_t createRegistryFile(void);
        esp_err_t readRegistryFile(void);
        esp_err_t addToRegistryFile(const char *path);
        esp_err_t removeFromRegistryFile(const char *path);

        esp_err_t createFile(const char *path, const char *data);
        esp_err_t createFile(const char *path);
        esp_err_t readFile(const char *filename, char *buffer, size_t buffer_size);
        esp_err_t addToFile(const char* filename, const char* data);
        esp_err_t deleteFile(const char *path);

        // Testing Files
        void testFS();
};

#ifdef CONFIG_FS_CONSOLE_COMMANDS
    void register_fs_commands();
#endif