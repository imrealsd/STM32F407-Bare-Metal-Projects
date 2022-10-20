#include <stm32f407xx.h>
#include <stdint.h>
#include "seven_segment.h"

void setup_seven_segment_gpio(void)
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

    /*port c pin 0 to 3 [output highspeed digit control line] */
    GPIOC->MODER |= GPIO_MODER_MODE0_0 | GPIO_MODER_MODE1_0 | GPIO_MODER_MODE2_0 | GPIO_MODER_MODE3_0;
    GPIOC->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR0_1 | GPIO_OSPEEDER_OSPEEDR1_1 | GPIO_OSPEEDER_OSPEEDR2_1;
    GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR3_1;

    /* make all data line low & control line high [control disable] */
    GPIOA->ODR = 0x0000;
    GPIOC->ODR = 0x000F;
}

void wait (void)
{   
    volatile int16_t i;
    for (i = 0; i < 1000; i++);
}

void delay (int32_t val)
{   
    volatile int32_t i;
    for (i = 0; i < (val * 2000); i++);
}

void count_down_number (int16_t number)
{   
    int8_t arr[10] = {0x3f, 0x06, 0x5B, 0x4F, 0X66, 0x6D, 0x7D, 0x07, 0x7F, 0x67}; 
    int8_t d1, d2, d3, d4;

    for (int16_t i = number; i >= 0; i--){
        
        d1 = i % 10;
        d2 = (i / 10) % 10;
        d3 = (i / 100) % 10;
        d4 = (i / 1000) % 10;

        for (int16_t j = 0; j < DIGIT_DURATION; j++){

            GPIOA->ODR = 0x00FF & arr[d1];                        // data
            GPIOC->ODR &= ~(1 << GPIO_ODR_OD0_Pos);               // enable digit
            wait();
            GPIOC->ODR |= 0x000F;                                 // disable digit

            GPIOA->ODR = 0x00FF & arr[d2];                        // data
            GPIOC->ODR &= ~(1 << GPIO_ODR_OD1_Pos);               // enable digit
            wait();
            GPIOC->ODR |= 0x000F;                                 // disable digit

            GPIOA->ODR = 0x00FF & arr[d3];                        // data
            GPIOC->ODR &= ~(1 << GPIO_ODR_OD2_Pos);               // enable digit
            wait();
            GPIOC->ODR |= 0x000F;                                 // disable digit

            GPIOA->ODR = 0x00FF & arr[d4];                        // data
            GPIOC->ODR &= ~(1 << GPIO_ODR_OD3_Pos);               // enable digit
            wait();
            GPIOC->ODR |= 0x000F;                                 // disable digit
        }
    }
}

void count_upto_number (int16_t number)
{   
    int8_t arr[10] = {0x3f, 0x06, 0x5B, 0x4F, 0X66, 0x6D, 0x7D, 0x07, 0x7F, 0x67}; 
    int8_t d1, d2, d3, d4;

    for (int16_t i = 0; i <= number; i++){
        
        d1 = i % 10;
        d2 = (i / 10) % 10;
        d3 = (i / 100) % 10;
        d4 = (i / 1000) % 10;

        for (int16_t j = 0; j < DIGIT_DURATION; j++){

            GPIOA->ODR = 0x00FF & arr[d1];                        // data
            GPIOC->ODR &= ~(1 << GPIO_ODR_OD0_Pos);               // enable digit
            wait();
            GPIOC->ODR |= 0x000F;                                 // disable digit

            GPIOA->ODR = 0x00FF & arr[d2];                        // data
            GPIOC->ODR &= ~(1 << GPIO_ODR_OD1_Pos);               // enable digit
            wait();
            GPIOC->ODR |= 0x000F;                                 // disable digit

            GPIOA->ODR = 0x00FF & arr[d3];                        // data
            GPIOC->ODR &= ~(1 << GPIO_ODR_OD2_Pos);               // enable digit
            wait();
            GPIOC->ODR |= 0x000F;                                 // disable digit

            GPIOA->ODR = 0x00FF & arr[d4];                        // data
            GPIOC->ODR &= ~(1 << GPIO_ODR_OD3_Pos);               // enable digit
            wait();
            GPIOC->ODR |= 0x000F;                                 // disable digit
        }
    }
}

/* Display number one time : call this func inside a loop*/

void show_number(int16_t number)
{
    int8_t arr[10] = {0x3f, 0x06, 0x5B, 0x4F, 0X66, 0x6D, 0x7D, 0x07, 0x7F, 0x67};     /* without dot pt*/ 
    int8_t arr_dot[10] = {0xBf, 0x86, 0xDB, 0xCF, 0XE6, 0xED, 0xFD, 0x87, 0xFF, 0xE7};  /* with dot point*/
    int8_t d1, d2, d3, d4;

    d1 =  number % 10;
    d2 = (number / 10) % 10;
    d3 = (number / 100) % 10;
    d4 = (number / 1000) % 10;

    GPIOA->ODR = 0x00FF & arr[d1];                        // data
    GPIOC->ODR &= ~(1 << GPIO_ODR_OD0_Pos);               // enable digit
    wait();
    GPIOC->ODR |= 0x000F;                                 // disable digit

    GPIOA->ODR = 0x00FF & arr[d2];                        // data
    GPIOC->ODR &= ~(1 << GPIO_ODR_OD1_Pos);               // enable digit
    wait();
    GPIOC->ODR |= 0x000F;                                 // disable digit

    GPIOA->ODR = 0x00FF & arr_dot[d3];                        // data
    GPIOC->ODR &= ~(1 << GPIO_ODR_OD2_Pos);               // enable digit
    wait();
    GPIOC->ODR |= 0x000F;                                 // disable digit

    GPIOA->ODR = 0x00FF & arr[d4];                        // data
    GPIOC->ODR &= ~(1 << GPIO_ODR_OD3_Pos);               // enable digit
    wait();
    GPIOC->ODR |= 0x000F;
}

void show_ON (void)
{
   
    GPIOA->ODR = 0x00FF & 0x54;                           // data
    GPIOC->ODR &= ~(1 << GPIO_ODR_OD1_Pos);               // enable digit
    wait();
    GPIOC->ODR |= 0x000F;                                 // disable digit

    GPIOA->ODR = 0x00FF & 0x3F;                             // data
    GPIOC->ODR &= ~(1 << GPIO_ODR_OD2_Pos);               // enable digit
    wait();
    GPIOC->ODR |= 0x000F;                                 // disable digit
}

void show_OFF (void)
{   
    GPIOA->ODR = 0x00FF & 0x71;                           // data
    GPIOC->ODR &= ~(1 << GPIO_ODR_OD0_Pos);               // enable digit
    wait();
    GPIOC->ODR |= 0x000F;   

    GPIOA->ODR = 0x00FF & 0x71;                           // data
    GPIOC->ODR &= ~(1 << GPIO_ODR_OD1_Pos);               // enable digit
    wait();
    GPIOC->ODR |= 0x000F;                                 // disable digit

    GPIOA->ODR = 0x00FF & 0x3F;                             // data
    GPIOC->ODR &= ~(1 << GPIO_ODR_OD2_Pos);               // enable digit
    wait();
    GPIOC->ODR |= 0x000F;                                 // disable digit
}