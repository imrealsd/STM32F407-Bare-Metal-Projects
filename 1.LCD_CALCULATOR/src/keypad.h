#include <stm32f407xx.h>
#include <stdint.h>
#ifndef KEYPAD_H
#define KEYPAD_H
void setup_keypad_gpio (void);
int8_t scan_keypad (void);
#endif