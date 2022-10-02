#include <stm32f407xx.h>
#include <stdint.h>
#include "delay.h"

void setup_ms_delay_timer (void)
{
    /* Timer 6 clock enable */
    RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;       // timer clock source -> 16Mhz APB1 clock

    /* Timer config */
    TIM6->CR1 &= ~(1 << TIM_CR1_UDIS_Pos);   // UEV enable for counter overflow/underflow
    TIM6->PSC = 15;                          // counter clock freq = 16Mhz/(15+1) = 1Mhz  [1 clk period = 1 micro sec]
    TIM6->ARR = 1000;                        // 1000 micro sec = 1 mili sec             
}

void delay_ms (int32_t val)
{   
    setup_ms_delay_timer();
    
    TIM6->CR1 |= TIM_CR1_CEN;                   // enable counter
    while (val--){
        while (!(TIM6->SR & TIM_SR_UIF));        // wait for overflow flag
        TIM6->SR &= ~(1 << TIM_SR_UIF_Pos);      // reset flag
        TIM6->CNT = 0x0000;                      // reset counter
    }
    TIM6->CR1 &= ~(1 << TIM_CR1_CEN_Pos);        // diable counter
}

void setup_us_delay_timer (void)
{
    /* Timer 6 clock enable */
    RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;       // timer clock source -> 16Mhz APB1 clock

    /* Timer config */
    TIM6->CR1 &= ~(1 << TIM_CR1_UDIS_Pos);   // UEV enable for counter overflow/underflow
    TIM6->PSC = 15;                          // counter clock freq = 16Mhz/(15+1) = 1Mhz  [1 clk period = 1 micro sec]
    TIM6->ARR = 1;                           // 1 micro sec             
}

void delay_us (int32_t val)
{   
    setup_us_delay_timer();
    
    TIM6->CR1 |= TIM_CR1_CEN;                   // enable counter
    while (val--){
        while (!(TIM6->SR & TIM_SR_UIF));        // wait for overflow flag
        TIM6->SR &= ~(1 << TIM_SR_UIF_Pos);      // reset flag
        TIM6->CNT = 0x0000;                      // reset counter
    }
    TIM6->CR1 &= ~(1 << TIM_CR1_CEN_Pos);        // diable counter
}