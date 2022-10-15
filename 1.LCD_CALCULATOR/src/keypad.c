#include <stm32f407xx.h>
#include <stdint.h>
#include "keypad.h"


void setup_keypad_gpio (void)
{   
    /* Turn on Port D clock */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;

    /* port D pin 0 to 3 highspeed outout row */
    GPIOD->MODER |= GPIO_MODER_MODE0_0 | GPIO_MODER_MODE1_0;
    GPIOD->MODER |= GPIO_MODER_MODE2_0 | GPIO_MODER_MODE3_0;
    GPIOD->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR0_1 | GPIO_OSPEEDR_OSPEED1_1;
    GPIOD->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR2_1 | GPIO_OSPEEDR_OSPEED3_1;

    /* port D pin 4 to 7 input pulldown col*/
    GPIOD->MODER &= ~ (GPIO_MODER_MODE4_Msk | GPIO_MODER_MODE5_Msk | GPIO_MODER_MODE6_Msk | GPIO_MODER_MODE7_Msk);
    GPIOD->PUPDR |= GPIO_PUPDR_PUPD4_1 | GPIO_PUPDR_PUPD5_1;
    GPIOD->PUPDR |= GPIO_PUPDR_PUPD6_1 | GPIO_PUPDR_PUPD7_1;
}


int8_t scan_keypad (void)
{   
    int8_t row, col;
    volatile int16_t col_input = 0;
    GPIOD->ODR &= ~ (0x000F);                                             // make all rows zero

    for (row = 0; row < 4; row++){

        GPIOD->ODR = (1 << row);    

        for (col = 0; col < 4; col++){        
                                                                         
            col_input = GPIOD->IDR & 0x00F0;  
            col_input = (col_input >> 4);  

            if (col_input != 0){
                
                for (col = 0; col < 4; col++){                                // check which col is high
                    if ((col_input == (1 << col))){
                        return ((row * 4) + col + 1);
                    }
                }
            }
        }
    }
    return 0; 
}