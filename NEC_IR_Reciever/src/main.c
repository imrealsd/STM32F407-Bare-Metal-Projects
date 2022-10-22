/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2022 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#define NEC_FRAME_SIZE (32)

void SystemClock_Config(void);
uint16_t abs_val(int16_t);
void int_to_arr(uint8_t arr[], uint32_t val, uint8_t size);

volatile uint8_t pulse_count = -1;
volatile uint32_t Recieved_NEC_frame = 0;

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
	HAL_Init();
	SystemClock_Config();
	MX_GPIO_Init();
	MX_USART1_UART_Init();
	MX_TIM6_Init();
	Micros_Init();

	uint8_t NEC_ARR[NEC_FRAME_SIZE];

	uint8_t f[1] = {'\n'};
	HAL_UART_Transmit(&huart1,f,9,100);

	while (1){

		if (pulse_count == 32){

			int_to_arr(NEC_ARR,Recieved_NEC_frame,NEC_FRAME_SIZE);
			HAL_UART_Transmit(&huart1,NEC_ARR,NEC_FRAME_SIZE,100);
			HAL_UART_Transmit(&huart1,f,1,100);

			pulse_count = -1;
			Recieved_NEC_frame = 0;
		}
	}
}


void int_to_arr(uint8_t arr[], uint32_t val, uint8_t size)
{
	int8_t index = size - 1;
	int8_t i = 0;

	while (index >= 0 ){
		if (val & (1 << index)){
			arr[i++] = '1';
			index--;
		} else {
			arr[i++] = '0';
			index--;
		}
	}
}


void HAL_GPIO_EXTI_Callback(uint16_t gpio_pin)
{	
	static volatile uint16_t current_time;
	current_time = Micros();
	TIM6->CNT = 0;
	
	static uint8_t START_OF_FRAME = 0;
	HAL_NVIC_ClearPendingIRQ(EXTI1_IRQn);

	

	/* NEC Transmitter will trasmit LSB first*/

	if (current_time > 13000 && current_time < 14000){  

		START_OF_FRAME = 1;
		Recieved_NEC_frame = 0;
		pulse_count = 0;

	} else if (pulse_count >= 0 && pulse_count < 32) {   

		if ((current_time > 2000 && current_time < 3000) && START_OF_FRAME == 1){
			Recieved_NEC_frame |= (1 << (pulse_count));
			pulse_count++;

		} else if ((current_time > 1000 && current_time < 2000) && START_OF_FRAME == 1){
	
			pulse_count++;
		}
	}

	if (pulse_count > 31){
		START_OF_FRAME = 0;
	}
}


uint16_t abs_val(int16_t val)
{
	return (val > 0 ? val : -val);
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

	/** Configure the main internal regulator output voltage
	 */
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLM = 8;
	RCC_OscInitStruct.PLL.PLLN = 168;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 4;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK){
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK){
		Error_Handler();
	}
}

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1){
	}
	/* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
	/* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
	   ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	/* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
