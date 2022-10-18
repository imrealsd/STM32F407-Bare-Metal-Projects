#include <stm32f407xx.h>
#include <stdint.h>
#ifndef MILLIS_H
#define MILLIS_H
void start_millis (void);
int16_t millis (void);
void stop_millis (void);
#endif