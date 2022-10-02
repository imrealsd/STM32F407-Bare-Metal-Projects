#include <stm32f407xx.h>
#include <stdint.h>
#ifndef LCD_H
#define LCD_H
void setup_lcd_gpio (void);
void lcd_command (int8_t);
void lcd_data (int8_t);
void lcd_string (char*);
void wait (void);
void start_lcd (void);
void clear_lcd (void);
void set_cursor (int8_t, int8_t);
void scroll_text (char *);
void delay (int32_t);
#endif