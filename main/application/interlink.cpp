#include "storage.hpp"
#include "motion.h"
extern "C" void register_app_commands() {
    #ifdef CONFIG_FS_CONSOLE_COMMANDS
        register_fs_commands();
    #endif

    #ifdef CONFIG_MOTION_CONSOLE_COMMANDS
        register_motion_commands();
    #endif
}