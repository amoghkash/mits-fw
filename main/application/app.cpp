
#include "esp_err.h"

#include "system/console.h"

#include "storage.hpp"
#include "motion.h"

#include "app.h"

int application_main() {
    FileSystem fs;
    fs.initialize();

    Motion motion;
    motion.initialize();

    return 0;
}

