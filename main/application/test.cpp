#include <stdio.h>
#include "storage.hpp"
#include "system/fs.h"
#include "test.h"
#include "esp_log.h"


void test(void) {
    FileSystem fs;
    fs.format();
    fs.initialize();
    fs.createFile("/data/test.txt", "Hello Prajwal's World HELLO!");
    char *buffer = new char[256];
    fs.readFile("/data/test.txt", buffer, 150);
    ESP_LOGI("test", "File Contents: %s", buffer);
    fs.addToFile("/data/test.txt", "Added Data");
    fs.readFile("/data/test.txt", buffer, 150);
    ESP_LOGI("test", "File Contents: %s", buffer);
}