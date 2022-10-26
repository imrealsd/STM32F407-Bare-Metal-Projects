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

#include "main.h"
#include "tim.h"
#include "gpio.h"

void SystemClock_Config(void);
void send_IR_signal(uint32_t button_code);

uint32_t VOL_DOWN_DATA = 0b10001101011100101001101110001000;
uint32_t VOL_UP_DATA   = 0b10001110011100011001101110001000;
uint32_t CH_UP_DATA    = 0b10001100011100111001101110001000;
uint32_t CH_DOWN_DATA  = 0b10001011011101001001101110001000;
uint32_t POWER_DATA    = 0b11110100000010111001101110001000;
uint32_t MUTE_DATA     = 0b11101000000101111001101110001000;

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{

	HAL_Init();
	SystemClock_Config();
	MX_GPIO_Init();
	MX_TIM1_Init();
	MX_TIM6_Init();

	//send_IR_signal(VOL_DOWN_DATA);

	while (1){

		if (!HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)){
			HAL_Delay(200);
			send_IR_signal(VOL_UP_DATA);

		} else if (!HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_1)){
			HAL_Delay(200);
			send_IR_signal(VOL_DOWN_DATA);

		} else if (!HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_2)){
			HAL_Delay(200);
			send_IR_signal(CH_UP_DATA);

		} else if (!HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_3)){
			HAL_Delay(200);
			send_IR_signal(CH_DOWN_DATA);

		} else if (!HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_4)){
			HAL_Delay(200);
			send_IR_signal(MUTE_DATA);

		} else if (!HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_5)){
			HAL_Delay(200);
			send_IR_signal(POWER_DATA);
		}
	}
}



void send_IR_signal(uint32_t button_code)
{
	volatile int len = 0;

	/* SOF */
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	Delay_Micros(9000);
	HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
	Delay_Micros(4500);

	/* 32 bit data frame [adddress + cmp_address + command + cmp_cmooamd] */
	while (len <=  31){

		if (button_code & (1 << len)){
		
			HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
			Delay_Micros(563);
			HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
			Delay_Micros(1690);
		
		} else {

			HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
			Delay_Micros(563);
			HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
			Delay_Micros(563);
		}
		len++;
	}
	
	/*EOF*/
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	Delay_Micros(563);
	HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
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
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
	{
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
	while (1)
	{
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
