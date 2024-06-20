#include <stdio.h>
#include "storage.h"
#include "system/fs.h"
#include "test.h"
#include "esp_log.h"


void test(void) {
    FileSystem fs;
    //fs.format();
    fs.initialize();
    fs.readRegistryFile();
    fs.createFile("/data/test.txt", "Hello World!");
    //fs.printFiles();
    ESP_LOGI("main", "Reading Data File...");
    readout_file("/data/test.txt");
    ESP_LOGI("main", "Reading Registry File...");
    readout_file("/data/registry.txt");

    fs.createFile("/data/test1.txt", "Hello World!");
    readout_file("/data/registry.txt");
    fs.createFile("/data/test.txt", "Hello Worldee!");
}