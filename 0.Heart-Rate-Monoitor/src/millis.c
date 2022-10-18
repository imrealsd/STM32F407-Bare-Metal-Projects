#include <stm32f407xx.h>
#include <stdint.h>
#include "millis.h"

void start_millis (void)
{
    /* Timer 6 clock enable */
    RCC->APB1ENR |= RCC_APB1ENR_TIM7EN;       // timer clock source -> 16Mhz APB1 clock

    /* Timer config */
    TIM7->CR1 &= ~(1 << TIM_CR1_UDIS_Pos);   // UEV enable for counter overflow/underflow
    TIM7->PSC = 15999;                       // counter clock freq = 16Mhz/(15999+1) = 16000000/16000 = 1k hz
                                             // [1 clk period = 1 mili sec] 
    TIM7->ARR = 0xFFFF;         
    TIM7->CNT = 0;
    TIM7->CR1 |= TIM_CR1_CEN;                        
}

void stop_millis (void)
{
    TIM7->CR1 &= ~TIM_CR1_CEN;
}

int16_t millis (void)
{
    return TIM7->CNT;
}