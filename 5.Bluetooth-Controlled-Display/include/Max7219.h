#include <stdint.h>
#include <stm32f4xx_hal.h>
#include <string.h>
#include "spi.h"

#ifndef __MAX7219_H__
#define __MAX7219_H__
#define SCROLL_DELAY 50      // in ms

void Max7219_Send(uint8_t addr, uint8_t data);
void Max7219_Clear(void);
void Max7219_Init(void);
void Max7219_Scroll_Letter(char ch);
void Max7219_Scroll_Number(char num);
void Max7219_Scroll_Text(char *str);

#endif

