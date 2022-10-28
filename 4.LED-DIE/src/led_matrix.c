#include <stm32f407xx.h>
#include <stdint.h>
#include "led_matrix.h"
#include "main.h"


/* 8X8 LED MATRIX: 
 * ROW 1 TO 8 [GND] -> CONNECTED TO PD0 TO PD7
 * COL 1 TO 8 [VCC] -> CONNECTED TO PA7 TO PA0   [ reverse order]
*/

void setup_led_matrix_gpio (void)
{
    /* setup PA and PD clock */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIODEN;

    /* PA0 to PA7 & PD0 to PD7 highspeed output */
    GPIOA->MODER |= GPIO_MODER_MODE0_0 | GPIO_MODER_MODE1_0 | GPIO_MODER_MODE2_0;
    GPIOA->MODER |= GPIO_MODER_MODE3_0 | GPIO_MODER_MODE4_0 | GPIO_MODER_MODE5_0;
    GPIOA->MODER |= GPIO_MODER_MODE6_0 | GPIO_MODER_MODE7_0;
    GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR0_1 | GPIO_OSPEEDER_OSPEEDR1_1 | GPIO_OSPEEDER_OSPEEDR2_1;
    GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR3_1 | GPIO_OSPEEDER_OSPEEDR4_1 | GPIO_OSPEEDER_OSPEEDR5_1;
    GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR6_1 | GPIO_OSPEEDER_OSPEEDR7_1;

    GPIOD->MODER |= GPIO_MODER_MODE0_0 | GPIO_MODER_MODE1_0 | GPIO_MODER_MODE2_0;
    GPIOD->MODER |= GPIO_MODER_MODE3_0 | GPIO_MODER_MODE4_0 | GPIO_MODER_MODE5_0;
    GPIOD->MODER |= GPIO_MODER_MODE6_0 | GPIO_MODER_MODE7_0;
    GPIOD->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR0_1 | GPIO_OSPEEDER_OSPEEDR1_1 | GPIO_OSPEEDER_OSPEEDR2_1;
    GPIOD->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR3_1 | GPIO_OSPEEDER_OSPEEDR4_1 | GPIO_OSPEEDER_OSPEEDR5_1;
    GPIOD->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR6_1 | GPIO_OSPEEDER_OSPEEDR7_1;

    /* turn off all led [rows->high] & [cols->low]  */
    GPIOA->ODR &= 0xFF00;
    GPIOD->ODR |= 0x00FF;
}


void show_number_dots (char num)                                  // function must be in a loop in main code
{   
    if (num == 0){return;}

    char num_dots[6][8] = {{0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00},    // 1         
                           {0x00, 0x00, 0x00, 0x66, 0x66, 0x00, 0x00, 0x00},    // 2
                           {0x00, 0x00, 0x00, 0xDB, 0xDB, 0x00, 0x00, 0x00},    // 3
                           {0xC3, 0xC3, 0x00, 0x00, 0x00, 0x00, 0xC3, 0xC3},    // 4
                           {0xC3, 0xC3, 0x00, 0x18, 0x18, 0x00, 0xC3, 0xC3},    // 5
                           {0xDB, 0xDB, 0x00, 0x00, 0x00, 0x00, 0xDB, 0xDB}};   // 6

    int8_t index =  num - 1;                                       // index = num - 1

    for (int8_t row = 0; row < 8; row++){
        GPIOD->ODR &= ~(1 << row);                                // turn on a single row
        GPIOA->ODR = (0xFFFF & num_dots[index][row]);             // write col value
        HAL_Delay(1);
        GPIOD->ODR |= 0x00FF;                                     // turn off all rows
    }
}


