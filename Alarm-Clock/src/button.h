#include <stm32f407xx.h>
#ifndef BUTTON_H
#define BUTTON_H
void setup_buttons(void);
int is_hour_pressed (void);
int is_min_pressed (void);
int is_mode_pressed (void);
int is_alarm_pressed (void);
#endif