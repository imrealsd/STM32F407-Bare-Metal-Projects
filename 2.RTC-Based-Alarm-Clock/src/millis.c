#include <stm32f407xx.h>
#include <stdint.h>
#include "millis.h"

void start_millis (void)
{
    /* Timer 6 clock enable */
    RCC->APB1ENR |= RCC_APB1ENR_TIM7EN;       // timer clock source -> 84 Mhz : 2 x APB1 clock

    /* Timer config */
    TIM7->CR1 &= ~(1 << TIM_CR1_UDIS_Pos);   // UEV enable for counter overflow/underflow
    TIM7->PSC = 167999;                       // counter clock freq = 84 Mhz/(167999+1) = = 500 hz
                                             // [1 clk period = 2 mili sec] 
    TIM7->ARR = 0xFFFF;         
    TIM7->CNT = 0;
    TIM7->CR1 |= TIM_CR1_CEN;                        
}

void stop_millis (void)
{
    TIM7->CR1 &= ~TIM_CR1_CEN;
}

uint16_t millis (void)
{   
    volatile uint16_t val = TIM7->CNT;
    return val;
}