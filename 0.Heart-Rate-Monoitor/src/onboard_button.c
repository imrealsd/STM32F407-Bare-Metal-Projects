#include "onboard_button.h"
#include <stm32f407xx.h>
#include <stdint.h>

void setup_onboard_button (void)
{   
    /* Enable clock for gpio A & E  */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;

    /*configure PE4 as input + pullup [button k0] */
    GPIOE->MODER &= ~(0b11 << GPIO_MODER_MODE4_Pos);
    GPIOE->OSPEEDR |= GPIO_OSPEEDR_OSPEED4_1;
    GPIOE->PUPDR |= GPIO_PUPDR_PUPD4_0;

} 

int8_t button_is_pressed (void)
{
    int8_t status;

    if (!(GPIOE->IDR & GPIO_IDR_IDR_4))
        status = 1;
    else
        status = 0;
    return status;
}

