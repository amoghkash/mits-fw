
#include "tinyusb.h"
#include "tusb_cdc_acm.h"


void initialize_usb();
void tinyusb_cdc_rx_callback(int itf, cdcacm_event_t *event);
