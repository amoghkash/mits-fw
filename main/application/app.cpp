
#include "esp_err.h"

#include "system/console.h"

#include "storage.h"
#include "motion.h"

#include "app.h"

int application_main() {
    FileSystem fs;
    fs.initialize();

    Motion motion;
    motion.initialize();

    ESP_ERROR_CHECK(start_console_interface());
    return 0;
}

esp_err_t start_console_interface() {
    esp_err_t err;
    err = console_run();
    if (err != ESP_OK) {
        return err;
    }
    return ESP_OK;
}
