#include <stm32f407xx.h>
#include <stdint.h>
#define DIGIT_DURATION 1

#ifndef SEVEN_SEGMENT_H
#define SEVEN_SEGMENT_H
void setup_seven_segment_gpio (void);
void count_down_number (int16_t);
void count_upto_number (int16_t);
void wait (void);
void delay (int32_t);
void show_time (int8_t, int8_t);
#endif