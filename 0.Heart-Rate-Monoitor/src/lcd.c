#include <stm32f407xx.h>
#include <stdint.h>
#include "lcd.h"
#include <string.h>

void setup_lcd_gpio (void)
{
    /* enable clk for port a and port c */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;

    /* port a pin 0 to 7 [output highspeed data line] */
    GPIOA->MODER |= GPIO_MODER_MODE0_0 | GPIO_MODER_MODE1_0 | GPIO_MODER_MODE2_0;
    GPIOA->MODER |= GPIO_MODER_MODE3_0 | GPIO_MODER_MODE4_0 | GPIO_MODER_MODE5_0;
    GPIOA->MODER |= GPIO_MODER_MODE6_0 | GPIO_MODER_MODE7_0;

    GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR0_1 | GPIO_OSPEEDER_OSPEEDR1_1 | GPIO_OSPEEDER_OSPEEDR2_1;
    GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR3_1 | GPIO_OSPEEDER_OSPEEDR4_1 | GPIO_OSPEEDER_OSPEEDR5_1;
    GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR6_1 | GPIO_OSPEEDER_OSPEEDR7_1;

    /*port c pin 4 [RS] & 5 [ENABLE] [output highspeed  control line] */
    GPIOC->MODER |= GPIO_MODER_MODE4_0 | GPIO_MODER_MODE5_0;
    GPIOC->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR4_1 | GPIO_OSPEEDER_OSPEEDR5_1 ;
    
    /* make all pins low */
    GPIOA->ODR &= ~(0x000F);                  
    GPIOC->ODR &= ~(0b11 << 4);
}

void lcd_command (int8_t cmd)
{   
    GPIOA->ODR = (0x00FF & cmd);                           // cmd
    GPIOC->ODR &= ~(1 << GPIO_ODR_OD4_Pos);                // RS = 0
    wait();                                                // RS setting time 
    GPIOC->ODR |= (1 << GPIO_ODR_OD5_Pos);                 // EN = 1
    wait();                                                // EN pulse
    GPIOC->ODR &= ~(1 << GPIO_ODR_OD5_Pos);                // EN = 0
}

void lcd_data (int8_t data)
{   
    GPIOA->ODR = (0x00FF & data);                           // data
    GPIOC->ODR |= (1 << GPIO_ODR_OD4_Pos);                  // RS = 1
    wait();                                                 // RS setting time 
    GPIOC->ODR |= (1 << GPIO_ODR_OD5_Pos);                  // EN = 1
    wait();                                                 // EN pulse
    GPIOC->ODR &= ~(1 << GPIO_ODR_OD5_Pos);                 // EN = 0
}

void lcd_string (char *str)
{
    while (*str != 0){
        lcd_data(*str);
        str++;
    }
}

void clear_lcd (void)
{
    lcd_command(0x01);
}

void set_cursor (int8_t row, int8_t col)
{
    if (row == 0){
        lcd_command((0x80 + col));

    } else if (row == 1){
        lcd_command((0xc0+col));
    }
}

void start_lcd (void)
{   
    setup_lcd_gpio();
    lcd_command(0x01);               // clear display
    lcd_command(0x38);               // 8 bit , 2 line
    lcd_command(0x0C);               // display on , cursor off
    lcd_command(0x06);               // auto increment
}

void scroll_text (char *str)
{   
   
    lcd_string(str);    
    int16_t size = strlen(str) + 16;

    while (size--){
        lcd_command(0x18);
        delay(120);
    }
}


void wait (void)
{
    volatile int16_t i;
    for (i = 0; i < 1000; i++);
}

void delay (int32_t val)
{
    volatile int32_t i;
    for (i = 0; i < val * 2000; i++);
}