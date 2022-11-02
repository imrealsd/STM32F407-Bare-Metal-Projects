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
#include "spi.h"
#include "usart.h"
#include "gpio.h"
#include "max7219_graphics.h"
#include <stm32f407xx.h>

#define MSG_BUFF_SIZE 5

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void reset (uint8_t *buff);

uint8_t rx_buff[MSG_BUFF_SIZE], msg[MSG_BUFF_SIZE];
volatile int8_t received_complete = 0;

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{	
	/*Initialise drivers*/
	HAL_Init();
	SystemClock_Config();
	MX_GPIO_Init();
	MX_SPI1_Init();
	MX_USART1_UART_Init();
	Max7219_Init();

	/*Setup interrupt to receive MSG_BUFF_SIZE bytes*/
	HAL_UART_Receive_IT(&huart1,rx_buff,MSG_BUFF_SIZE);

	while (1){

		/* If total buffer is received update msg string */
		if (received_complete){
			
			/*copy rx_buff to msg*/
			for (volatile int8_t i = 0; i < MSG_BUFF_SIZE; i++)
				msg[i] = rx_buff[i];

			received_complete = 0;
			reset(rx_buff);

			/*Clear screen if received msg is "CLEAR" */
			if (msg[0] == 'C' && msg[1] == 'L' && msg[2] == 'E' && msg[3] == 'A' && msg[4] == 'R')
				reset(msg);
		}
		/*scroll msg*/
		Max7219_Scroll_Text((char*)msg);
	}
}


/**
 * @brief  RX complete callback
 * @retval None
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{	
	/* This function will be called ater MSG_BUFF_SIZE bytes receive is completed
	 * after receiving MSG_BUFF_SIZE bytes , setup for another MSG_BUFF_SIZE bytes reception
	*/
	HAL_UART_Receive_IT(&huart1,rx_buff,MSG_BUFF_SIZE);
	received_complete = 1;
}

/**
 * @brief  reset buffer
 * @retval None
 */
void reset (uint8_t *buff)
{
	for (volatile int8_t i = 0; i < MSG_BUFF_SIZE; i++)
		buff[i] = 0;
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
