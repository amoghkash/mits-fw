#include <stdio.h>
#include "string"
#include "cstring"

#include "esp_system.h"
#include "esp_log.h"
#include "esp_console.h"
#include "freertos/FreeRTOS.h"

#include "system/fs.h"
#include "system/system.h"
#include "argtable3/argtable3.h"

#include "storage.h"

#define TAG "FileSystem"


FileSystem::FileSystem()
{
    this->initialize();
}

// Basic File System Functions
esp_err_t FileSystem::initialize(void) {
    ESP_LOGI(TAG, "Initializing FSManager...");
    if(!is_storage_initialized()) {
        return initialize_storage();
    }
    ESP_LOGI(TAG, "FSManager initialized.");

    #ifdef CONFIG_FS_CONSOLE_COMMANDS
        register_fs_commands();
    #endif

    // Read from Registry File
    return readRegistryFile();
}

esp_err_t FileSystem::format(void) {
    ESP_LOGI(TAG, "Formatting File System...");
    return format_fs();
}

// Registry File Functions
esp_err_t FileSystem::checkRegistryFile(void) {
    ESP_LOGD(TAG, "Checking Registry File...");
    // Check if file exists
    FILE *file = fopen(registryFilePath, "r");
    if (file == NULL) {
        ESP_LOGI(TAG, "Registry File does not exist.");
        return createRegistryFile();
    }

    ESP_LOGD(TAG, "Registry File exists.");
    return ESP_OK;
}

esp_err_t FileSystem::createRegistryFile(void) {
    ESP_LOGI(TAG, "Creating Registry File...");
    // Create file
    return create_file(registryFilePath);
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
        ESP_LOGD(TAG, "File %d: %s", numFiles, line);
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
    readout_file(registryFilePath);
    return;
}

// File Functions
esp_err_t FileSystem::createFile(const char *path, const char *data) {
    esp_err_t ret;
    ESP_LOGI(TAG, "Creating file %s", path);
    
    // Create the File
    ret = create_file_add_data(path, data);
    if (ret == ESP_ERR_NOT_ALLOWED) {
        //ESP_LOGE(TAG, "File %s Exists.", path);
        return ret;
    } else if (ret != ESP_OK){
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

esp_err_t FileSystem::createFile(const char *path) {
    esp_err_t ret;
    ESP_LOGI(TAG, "Creating file %s", path);
    
    // Create the File
    ret = create_file(path);
    if (ret == ESP_ERR_NOT_ALLOWED) {
        //ESP_LOGE(TAG, "File %s Exists.", path);
        return ret;
    } else if (ret != ESP_OK){
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

esp_err_t FileSystem::readFile(const char* filename, char* buffer, size_t buffer_size) {
    ESP_LOGI(TAG, "Reading file %s", filename);
    return read_file(filename, buffer, buffer_size);
}

esp_err_t FileSystem::addToFile(const char* filename, const char* data) {
    ESP_LOGI(TAG, "Adding %s file %s", data, filename);
    return add_data_to_file(filename, data);
}

void FileSystem::testFS() {
    #ifndef CONFIG_MITS_PRODUCTION
        format();
        initialize();
        readRegistryFile();
        createFile("/data/test.txt", "Hello World!");
        printFiles();
        createFile("/data/data_07202024.txt");
        createFile("/data/data_07212024.txt");
        printFiles();
        createFile("/data/data_07202024.txt");
        printFiles();
        format();
    #endif
}


#ifdef CONFIG_FS_CONSOLE_COMMANDS
// Create a file from console
static struct {
    struct arg_file *fileinfo;
    struct arg_end *end;
} file_args;

// Create a file
static int console_createfile(int argc, char **argv) {
    int nerrors = arg_parse(argc, argv, (void **)&file_args);

    if (nerrors != 0) {
        arg_print_errors(stderr, file_args.end, argv[0]);
        return 0;
    }

    const char* filename = file_args.fileinfo->filename[0];
    FileSystem fs;
    fs.createFile(filename);
    return 0;
}

// Readout a file
static int console_readfile(int argc, char **argv) {
    int nerrors = arg_parse(argc, argv, (void **)&file_args);

    if (nerrors != 0) {
        arg_print_errors(stderr, file_args.end, argv[0]);
        return 0;
    }

    const char* filename = file_args.fileinfo->filename[0];
    readout_file(filename);
    return 0;
}

// Create a file from console
static struct {
    struct arg_file *fileinfo;
    struct arg_str *data;
    struct arg_end *end;
} write_file_args;

// Create a file
static int console_writefile(int argc, char **argv) {
    int nerrors = arg_parse(argc, argv, (void **)&write_file_args);

    if (nerrors != 0) {
        arg_print_errors(stderr, write_file_args.end, argv[0]);
        return 0;
    }

    const char* filename = write_file_args.fileinfo->filename[0];
    const char* data = write_file_args.data->sval[0];

    FileSystem fs;
    fs.addToFile(filename, data);
    return 0;
}

// List all files
static int console_listfiles(int argc, char **argv) {
    readout_file("/data/registry.txt");
    return 0;
}

static int console_format(int argc, char **argv) {
    printf("Formatting FS...\n");
    format_fs();
    FileSystem fs;
    return 0;
}

void register_fs_commands() {
    if (!is_storage_initialized()) {
        return;
    }

    ESP_LOGI(TAG, "Registering FS Commands...");

    file_args.fileinfo = arg_file0("f", NULL, "<filename>", "File to create");
    file_args.end = arg_end(1);
    const esp_console_cmd_t create_file_cmd = {
        .command = "create-file",
        .help = "Create a file",
        .hint = NULL,
        .func = &console_createfile,
        .argtable = &file_args
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&create_file_cmd));
    
    const esp_console_cmd_t read_file_cmd = {
        .command = "read-file",
        .help = "Read a file",
        .hint = NULL,
        .func = &console_readfile,
        .argtable = &file_args
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&read_file_cmd));

    write_file_args.fileinfo = arg_file0("f", NULL, "<filename>", "File to create");
    write_file_args.data = arg_str0("d", NULL, "<data>", "Data to write");
    write_file_args.end = arg_end(1);
    const esp_console_cmd_t write_file_cmd = {
        .command = "write-file",
        .help = "Write to a file",
        .hint = NULL,
        .func = &console_writefile,
        .argtable = &write_file_args
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&write_file_cmd));

    const esp_console_cmd_t list_file_cmd = {
        .command = "ls",
        .help = "List all Files",
        .hint = NULL,
        .func = &console_listfiles,
        .argtable = NULL
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&list_file_cmd));

    const esp_console_cmd_t format_cmd = {
        .command = "format",
        .help = "Format File System",
        .hint = NULL,
        .func = &console_format,
        .argtable = NULL
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&format_cmd));
    
}
#endif