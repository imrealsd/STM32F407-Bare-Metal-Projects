#include <stdint.h>
#include "i2c.h"
#ifndef I2C_LCD
#define I2C_LCD
void start_i2c_lcd (void);
void i2c_lcd_set_cursor (int8_t, int8_t);
void i2c_lcd_cmd (int8_t);
void i2c_lcd_data (int8_t);
void i2c_lcd_string (char*);
void i2c_lcd_clear (void);
#endif