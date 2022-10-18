#include <stdint.h>
#include "i2c.h"
#include "i2c_lcd.h"
#include "stm32f4xx_hal.h"

/* I2C-LCD connection details [pcf874t ic with lcd]:
 * P0 -> RS , P1 -> R/W , P2 -> EN , P3 -> LED 
 * P4 to P7 [D4 to D7 of lcd] [higher nibble]
*/

void start_i2c_lcd (void)
{   
    MX_I2C1_Init();

    /* reseting all pins of i/o expander */
    uint8_t reset_io_port_data = 0x00;
    HAL_I2C_Master_Transmit(&hi2c1, (0x27 << 1 | 0),&reset_io_port_data,1,100);

    /* lcd 4bit config code */
    i2c_lcd_cmd(0x02);
    i2c_lcd_cmd(0x28);
    i2c_lcd_cmd(0x0C);
    i2c_lcd_cmd(0x01);
    i2c_lcd_cmd(0x80);
    
}

void i2c_lcd_clear (void)
{
    i2c_lcd_cmd(0x01);
}

void i2c_lcd_set_cursor (int8_t row, int8_t col)
{
    if (row == 0){
        i2c_lcd_cmd((0x80 + col));

    } else if (row == 1){
        i2c_lcd_cmd((0xc0+col));
    }
}


void i2c_lcd_cmd (int8_t cmd)
{   
    /* cmd_en0 = 0x08 [rs = 0, En = 0, led on]
     * cmd_en1 = 0x0C [rs = 0, en = 1, led 0n]
    */
    int8_t cmd_en1 = 0x0C;
    int8_t cmd_en0 = 0x08;
    int8_t high_nibble = cmd & 0xF0;
    int8_t low_nibble  = (cmd << 4) & 0xF0;


    uint8_t buff1[2] = {high_nibble,(cmd_en0|high_nibble)};
    HAL_I2C_Master_Transmit(&hi2c1, (0x27 << 1 | 0),buff1,2,100);

    HAL_Delay(1);

    buff1[0] = cmd_en1 | high_nibble;
    HAL_I2C_Master_Transmit(&hi2c1, (0x27 << 1 | 0),buff1,1,100);

    HAL_Delay(1);

    buff1[0] = cmd_en0 | high_nibble;
    HAL_I2C_Master_Transmit(&hi2c1, (0x27 << 1 | 0),buff1,1,100);

  
    /* send lower nibble */

    uint8_t buff2[2] = {low_nibble,(cmd_en0|low_nibble)};
    HAL_I2C_Master_Transmit(&hi2c1, (0x27 << 1 | 0),buff2,2,100);

    HAL_Delay(1);

    buff2[0] = cmd_en1 | low_nibble;
    HAL_I2C_Master_Transmit(&hi2c1, (0x27 << 1 | 0),buff2,1,100);

    HAL_Delay(1);

    buff2[0] = cmd_en0 | low_nibble;
    HAL_I2C_Master_Transmit(&hi2c1, (0x27 << 1 | 0),buff2,1,100);
}   


void i2c_lcd_data (int8_t data)
{
    /* dta_en0 = 0x09 [rs = 0, En = 0, led on]
     * dta_en1 = 0x0D [rs = 0, en = 1, led 0n]
    */
    int8_t dta_en1 = 0x0D;
    int8_t dta_en0 = 0x09;
    int8_t high_nibble = data & 0xF0;
    int8_t low_nibble  = (data << 4) & 0xF0;

    uint8_t buff1[2] = {high_nibble,(dta_en0|high_nibble)};
    HAL_I2C_Master_Transmit(&hi2c1, (0x27 << 1 | 0),buff1,2,100);

    HAL_Delay(1);

    buff1[0] = dta_en1 | high_nibble;
    HAL_I2C_Master_Transmit(&hi2c1, (0x27 << 1 | 0),buff1,1,100);

    HAL_Delay(1);

    buff1[0] = dta_en0 | high_nibble;
    HAL_I2C_Master_Transmit(&hi2c1, (0x27 << 1 | 0),buff1,1,100);

  
    /* send lower nibble */

    uint8_t buff2[2] = {low_nibble,(dta_en0|low_nibble)};
    HAL_I2C_Master_Transmit(&hi2c1, (0x27 << 1 | 0),buff2,2,100);

    HAL_Delay(1);

    buff2[0] = dta_en1 | low_nibble;
    HAL_I2C_Master_Transmit(&hi2c1, (0x27 << 1 | 0),buff2,1,100);

    HAL_Delay(1);

    buff2[0] = dta_en0 | low_nibble;
    HAL_I2C_Master_Transmit(&hi2c1, (0x27 << 1 | 0),buff2,1,100);
    
}

void i2c_lcd_string (char* str)
{
    while (*str != 0){
        i2c_lcd_data(*str);
        str++;
    }
}