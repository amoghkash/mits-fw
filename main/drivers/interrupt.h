#pragma once

#ifdef __cplusplus
    extern "C" {
#endif

void configure_interrupts();
void remove_interrupts();

esp_err_t configure_intr_usb();

#ifdef __cplusplus
    }
#endif


