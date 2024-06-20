#include <stdio.h>
#include "storage.h"
#include "system/fs.h"
#include "test.h"
#include "esp_log.h"


void test(void) {
    FileSystem fs;
    fs.format();
    fs.initialize();
    fs.readRegistryFile();
    fs.createFile("/data/test.txt", "Hello World!");
    fs.printFiles();
    fs.createFile("/data/data_07202024.txt");
    fs.createFile("/data/data_07212024.txt");
    fs.printFiles();
    fs.createFile("/data/data_07202024.txt");
    fs.printFiles();
}