#pragma once

#ifdef __cplusplus
    extern "C" {
#endif

void nvs_initialize(void);
void nvs_deinitialize(void);

char *nvs_get_string(const char *key);
void nvs_iterate_strings(void);
#ifdef __cplusplus
    }
#endif