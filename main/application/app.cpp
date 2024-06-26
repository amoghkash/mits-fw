
#include "esp_err.h"

#include "system/console.h"

#include "storage.h"

#include "app.h"

int application_main() {
    FileSystem fs;
    fs.initialize();

    ESP_ERROR_CHECK_WITHOUT_ABORT(console_run());
    return 0;
}