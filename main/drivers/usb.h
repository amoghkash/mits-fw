#pragma once

#ifdef __cplusplus
    extern "C" {
#endif

#include "tinyusb.h"
#include "tusb_cdc_acm.h"


void initialize_usb();
void tinyusb_cdc_rx_callback(int itf, cdcacm_event_t *event);


#ifdef __cplusplus
    }
#endif