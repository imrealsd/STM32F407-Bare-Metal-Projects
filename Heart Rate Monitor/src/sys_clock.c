#include <stm32f407xx.h>
#include "sys_clock.h"

void setup_system_clk (void)
{
    RCC->CR |= RCC_CR_HSION;                       // Turn on 16 Mhz HSI clock
    while (!(RCC->CR & RCC_CR_HSIRDY));            // wait until HSI is ready 
}
