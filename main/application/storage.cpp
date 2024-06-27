#include <stdio.h>
#include <string>
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
    ESP_LOGD(TAG, "Initializing FSManager...");
    if(!is_storage_initialized()) {
        return initialize_storage();
    }
    ESP_LOGD(TAG, "FSManager initialized.");

    #ifdef CONFIG_FS_CONSOLE_COMMANDS
        register_fs_commands();
    #endif

    esp_err_t ret;
    ESP_LOGD(TAG, "Reading Registry File...");

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
    numFiles = 0;
    while(fgets(line, FILENAME_MAX_SIZE, file)){
        line[strcspn(line, "\n")] = 0;
        files[numFiles] = line;
        numFiles++;
    }

    fclose(file);
    return ESP_OK;
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
        ESP_LOGE(TAG, "Registry File does not exist.");
        return createRegistryFile();
    }

    ESP_LOGD(TAG, "Registry File exists.");
    fclose(file);
    return ESP_OK;
}

esp_err_t FileSystem::createRegistryFile(void) {
    ESP_LOGI(TAG, "Creating Registry File...");
    // Create file
    return create_file(registryFilePath);
}

esp_err_t FileSystem::readRegistryFile(void) {
    /* for (int i = 0; i < numFiles; i++) {
        ESP_LOGI(TAG, "File %d: %s", i, files[i].c_str());
    }
    return ESP_OK; */
    return readout_file(registryFilePath);
}

esp_err_t FileSystem::addToRegistryFile(const char *path) {
    ESP_LOGI(TAG, "Adding %s to Registry File.", path);
    // Create file
    return add_data_to_file(registryFilePath, path);
}

esp_err_t FileSystem::removeFromRegistryFile(const char *path) {
    esp_err_t ret;
    bool found = false;
    for (int i = 0; i < numFiles; i++) {
        if (files[i] == path) {
            found = true;
            files[i] = "";
            numFiles--;
            for (int j = i; j < numFiles; j++) {
                files[j] = files[j+1];
            }
            // If not last file record, set last file record to empty
            if (i != numFiles) {
                files[numFiles] = "";
            }
        }
    }

    if (!found) {
        ESP_LOGE(TAG, "File %s not found in Registry File.", path);
        return ESP_ERR_NOT_FOUND;
    }

    // Empty File
    ret = delete_file("/data/registry.txt");
    ret |= create_file("/data/registry.txt");
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error removing %s from Registry File.", path);
        return ret;
    }
    
    for (int i = 0; i < numFiles; i++) {
        ret = add_data_to_file("/data/registry.txt", files[i].c_str());
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Error adding %s to Registry File.", files[i].c_str());
            return ret;
        }
    }

    return ESP_OK;
}

// File Functions
esp_err_t FileSystem::createFile(const char *path, const char *data) {
    esp_err_t ret;

    if ((sizeof(path)) > FILENAME_MAX_SIZE) {
        ESP_LOGE(TAG, "Filename too long.");
        return ESP_ERR_INVALID_SIZE;
    }

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

    if ((sizeof(path)) > FILENAME_MAX_SIZE) {
        ESP_LOGE(TAG, "Filename too long.");
        return ESP_ERR_INVALID_SIZE;
    }

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
    ESP_LOGD(TAG, "Adding %s file %s", data, filename);
    return add_data_to_file(filename, data);
}

esp_err_t FileSystem::deleteFile(const char* filename) {
    ESP_LOGI(TAG, "Deleting file %s", filename);
    esp_err_t ret = removeFromRegistryFile(filename);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error removing %s from Registry File. Unable to delete.", filename);
        return ret;
    }
    return delete_file(filename);
}

void FileSystem::testFS() {
    #ifndef CONFIG_MITS_PRODUCTION
        format();
        initialize();
        readRegistryFile();
        createFile("/data/test.txt", "Hello World!");
        createFile("/data/data_07202024.txt");
        createFile("/data/data_07212024.txt");
        createFile("/data/data_07202024.txt");
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

// Readout a file
static int console_deletefile(int argc, char **argv) {
    int nerrors = arg_parse(argc, argv, (void **)&file_args);

    if (nerrors != 0) {
        arg_print_errors(stderr, file_args.end, argv[0]);
        return 0;
    }

    const char* filename = file_args.fileinfo->filename[0];
    FileSystem fs;
    fs.deleteFile(filename);
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
    //readout_file("/data/registry.txt");
    FileSystem fs;
    fs.readRegistryFile();
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

    ESP_LOGD(TAG, "Registering FS Commands...");

    file_args.fileinfo = arg_file0("f", NULL, "<filename>", "File to create");
    file_args.end = arg_end(1);
    const esp_console_cmd_t create_file_cmd = {
        .command = "fscreate",
        .help = "Create a file",
        .hint = NULL,
        .func = &console_createfile,
        .argtable = &file_args
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&create_file_cmd));
    
    const esp_console_cmd_t read_file_cmd = {
        .command = "fsread",
        .help = "Read a file",
        .hint = NULL,
        .func = &console_readfile,
        .argtable = &file_args
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&read_file_cmd));

    const esp_console_cmd_t delete_file_cmd = {
        .command = "fsdelete",
        .help = "Delete a file",
        .hint = NULL,
        .func = &console_deletefile,
        .argtable = &file_args
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&delete_file_cmd));

    write_file_args.fileinfo = arg_file0("f", NULL, "<filename>", "File to create");
    write_file_args.data = arg_str0("d", NULL, "<data>", "Data to write");
    write_file_args.end = arg_end(1);
    const esp_console_cmd_t write_file_cmd = {
        .command = "fswrite",
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
    
    ESP_LOGD(TAG, "FS Commands Registered.");
}
#endif