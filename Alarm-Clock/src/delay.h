#include <stm32f407xx.h>
#include <stdint.h>
#ifndef DELAY_H
#define DELAY_H
void setup_ms_delay_timer (void);
void setup_us_delay_timer (void);
void delay_us (int32_t);
void delay_ms (int32_t);
#endif