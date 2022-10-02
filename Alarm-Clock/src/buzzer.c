#include "buzzer.h"

void setup_buzzer (void)
{   
    /* PORT B PIN 4 BUZZER OUTPUT [medium speed] */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
    GPIOD->MODER |= GPIO_MODER_MODE4_0;
    GPIOD->OSPEEDR |= GPIO_OSPEEDR_OSPEED4_0;
}
void buzzer_on (void)
{   
    setup_buzzer();
    GPIOD->ODR |= (1 << GPIO_ODR_OD4_Pos);
}

void buzzer_off (void)
{   
    setup_buzzer();
    GPIOD->ODR &= ~(1 << GPIO_ODR_OD4_Pos);
}

