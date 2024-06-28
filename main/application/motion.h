#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

class Motion {
    private:
        TaskHandle_t tHandle;
        EventGroupHandle_t loggingHandle;
        
    public:
        Motion();
        esp_err_t initialize(void);
        esp_err_t start(void);
        esp_err_t stop(void);

};

void logMotion(void * pvParameters);
void register_motion_commands(void);
