#include <stm32f407xx.h>

void setup_buttons(void)
{
    /* PORT D PIN 0-3 BUTTON INPUT [medium speed] */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;

    GPIOD->MODER &= ~ (GPIO_MODER_MODE3_Msk | GPIO_MODER_MODE2_Msk | GPIO_MODER_MODE1_Msk | GPIO_MODER_MODE0_Msk);
    GPIOD->PUPDR |= GPIO_PUPDR_PUPD3_0 | GPIO_PUPDR_PUPD1_0;
    GPIOD->PUPDR |= GPIO_PUPDR_PUPD2_0 | GPIO_PUPDR_PUPD0_0;
}

int is_hour_pressed (void)
{   

    if (!(GPIOD->IDR & GPIO_IDR_IDR_2)){return 1;}
    return 0;
}

int is_min_pressed (void)
{
    if (GPIOD->IDR & GPIO_IDR_IDR_3){return 0;}
    return 1;
}

int is_mode_pressed (void)
{
    if (GPIOD->IDR & GPIO_IDR_IDR_0){return 0;}
    return 1;
}

int is_alarm_pressed (void)
{
    if (GPIOD->IDR & GPIO_IDR_IDR_1){return 0;}
    return 1;
}