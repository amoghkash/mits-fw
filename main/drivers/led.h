#pragma once
#ifdef __cplusplus
    extern "C" {
#endif

#include <stdio.h>
#include "sdkconfig.h"



// Definintions
#define RAINBOW_MAX 360
#define COLOR_ITERATIONS 100
#define RAINBOW_STEP (RAINBOW_MAX / COLOR_ITERATIONS)
#define ITERATION_TIME (10 / portTICK_PERIOD_MS)



// LED Configuration
void initialize_led(void);
bool led_configured(void);


// LED Control Functions

void turn_led_on(void);
void turn_led_off(void);

void gradiate_led_on(void);
void gradiate_led_off(void);

void set_color_instant(uint8_t r, uint8_t g, uint8_t b);
void set_color_gradiate(uint8_t r, uint8_t g, uint8_t b);

#ifdef CONFIG_LED_CONSOLE_COMMANDS
void register_led_commands();
#endif

#ifdef __cplusplus
    }
#endif