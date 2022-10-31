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
#include "gpio.h"
#include <string.h>
#define SCROLL_DELAY 60      // in ms

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void Max7219_Send(uint8_t addr, uint8_t data);
void Max7219_Clear(void);
void Max7219_Init(void);
void Max7219_Scroll_Letter(char ch);
void Max7219_Scroll_Number(char num);
void Max7219_Scroll_Text(char *str);


uint8_t letters[26][8] = {{0x7E, 0xFF, 0xE7, 0xE7, 0xFF, 0xFF, 0xE7, 0xE7},    // A
						  {0xFE, 0XFF, 0XE7, 0XFE, 0XFE, 0XE7, 0XFF, 0XFE},    // B
						  {0x7E, 0xFF, 0xE7, 0xE0, 0xE0, 0xE7, 0xFF, 0x7E},    // C
						  {0xFE, 0xFF, 0xE7, 0xE7, 0xE7, 0xE7, 0xFF, 0XFE},    // D
						  {0xFF, 0xFF, 0xE0, 0xFC, 0xFC, 0xE0, 0xFF, 0xFF},    // E
						  {0xFF, 0xFF, 0xE0, 0xFC, 0xFC, 0xE0, 0xE0, 0xE0},    // F
						  {0x7E, 0xFF, 0xE7, 0xE0, 0xEF, 0xE7, 0xFF, 0x7E},    // G
						  {0xE7, 0xE7, 0xE7, 0xFF, 0xFF, 0xE7, 0xE7, 0xE7},    // H
						  {0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C},    // I
						  {0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x3F, 0x3E},    // J
						  {0xE3, 0xE7, 0xEE, 0xEC, 0xEC, 0xEE, 0xE7, 0xE3},    // K
						  {0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xFF, 0xFF},    // L
						  {0xC3, 0xE7, 0xFF, 0xFF, 0xE7, 0xE7, 0xE7, 0xE7},    // M
						  {0xE7, 0xF7, 0xF7, 0xFF, 0xFF, 0xFF, 0xEF, 0xE7},    // N
						  {0x7E, 0xFF, 0xE7, 0xE7, 0xE7, 0xE7, 0xFF, 0x7E},    // O
						  {0xFE, 0xFF, 0xE7, 0xFF, 0xFE, 0xE0, 0xE0, 0xE0},    // P
						  {0x7E, 0xFF, 0xE7, 0xE7, 0xE3, 0xED, 0xF6, 0x7B},    // Q
						  {0xFE, 0xFF, 0xF7, 0xFF, 0xFE, 0xE7, 0xE7, 0xE7},    // R
						  {0x7E, 0xFF, 0xE0, 0xFE, 0x7F, 0x07, 0xFF, 0x7E},    // S
						  {0xFE, 0xFE, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38},    // T
						  {0xE7, 0xE7, 0xE7, 0xE7, 0xE7, 0xE7, 0xFF, 0x7E},    // U
						  {0xE7, 0xE7, 0xE7, 0xE7, 0xE7, 0xE7, 0x7E, 0x3C},    // V
						  {0xE7, 0xE7, 0xE7, 0xE7, 0xFF, 0xFF, 0xE7, 0xC3},    // W
						  {0xE7, 0xE7, 0x7E, 0x3C, 0x3C, 0x7E, 0xE7, 0xE7},    // X
						  {0xE7, 0xE7, 0x7E, 0x3C, 0x3C, 0x18, 0x18, 0x18},    // Y
						  {0xFF, 0xFF, 0x0F, 0x1E, 0x78, 0xF0, 0xFF, 0xFF}};   // Z

uint8_t numbers[10][8] = {{0x7E, 0xFF, 0xE7, 0xE7, 0xE7, 0xE7, 0xFF, 0x7E},    // 0
						  {0x07, 0x0F, 0x1F, 0x07, 0x07, 0x07, 0x07, 0x07},    // 1
						  {0x7E, 0xFF, 0xE7, 0x0E, 0x38, 0xE0, 0xFF, 0xFF},    // 2
						  {0xFE, 0xFF, 0x07, 0xFE, 0xFE, 0x07, 0xFF, 0xFE},    // 3
						  {0xEE, 0xEE, 0xEE, 0xFF, 0xFF, 0x0E, 0x0E, 0x0E},    // 4
						  {0xFF, 0xFF, 0xE0, 0xFE, 0xFF, 0x07, 0xFF, 0x7E},    // 5
						  {0x7E, 0xFF, 0xE0, 0xFE, 0xFF, 0xE7, 0xFF, 0x7E},    // 6
						  {0xFF, 0xFF, 0x07, 0x0E, 0x1C, 0x38, 0x38, 0x38},    // 7
						  {0x7E, 0xFF, 0xE7, 0x7E, 0x7E, 0xE7, 0xFF, 0x7E},    // 8
						  {0x7E, 0xFF, 0xE7, 0xFF, 0x7F, 0x07, 0xFF, 0x7E}};   // 9
		
		

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
	HAL_Init();
	SystemClock_Config();
	MX_GPIO_Init();
	MX_SPI1_Init();
	Max7219_Init();

	while (1){

		HAL_Delay(2000);
		Max7219_Scroll_Text("HELLO ROHAN");
	}
}


/**
 * @brief  Scroll entire text/string.
 * @retval none
 */
void Max7219_Scroll_Text(char *str)
{	
	int8_t size = strlen(str);

	for (int8_t i = 0 ; i < size; i++){

		if (str[i] == ' ')
			i++;

		if (str[i] >= 'A' && str[i] <= 'Z')
			Max7219_Scroll_Letter(str[i]);

		else if (str[i] >= '0' && str[i] <= '9')
			Max7219_Scroll_Number(str[i]);
	}
}


/**
 * @brief  Scroll one letter.
 * @retval none
 */
void Max7219_Scroll_Letter(char ch)
{	
	int8_t letter_index = ch - 65, matrix_row, shift_val;
	volatile uint16_t frame = 0;
	//uint8_t matrix_row, shift_val;


	for (shift_val = 0; shift_val <= 16; shift_val++){          /* For left to right shift*/
		
		for (matrix_row = 0; matrix_row < 8; matrix_row++){     /*for writing 8 marix rows*/

			frame = letters[letter_index][matrix_row] << shift_val;
			frame = frame >> 8;
			Max7219_Send(0x1+matrix_row,(uint8_t)frame);
		}
		HAL_Delay(SCROLL_DELAY);
	}
}


/**
 * @brief  Scroll one number.
 * @retval none
 */
void Max7219_Scroll_Number(char num)
{
	int8_t number_index = num - 48, matrix_row, shift_val;
	volatile uint16_t frame = 0;
	//uint8_t matrix_row, shift_val;


	for (shift_val = 0; shift_val <= 16; shift_val++){          /* For left to right shift*/
		
		for (matrix_row = 0; matrix_row < 8; matrix_row++){     /*for writing 8 marix rows*/

			frame = numbers[number_index][matrix_row] << shift_val;
			frame = frame >> 8;
			Max7219_Send(0x1+matrix_row,(uint8_t)frame);
		}
		HAL_Delay(SCROLL_DELAY);
	}
}



/**
 * @brief  Send data fro one row.
 * @retval none
 */
void Max7219_Send(uint8_t addr, uint8_t data)
{	
	/*send 2x8 bit = 16 bit data at a time, acc. to max7219 datasheet*/
	uint8_t tx_buff[2] = {addr,data};
	uint8_t rx_buff[2];

	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_RESET);
	HAL_SPI_TransmitReceive(&hspi1,(uint8_t*)tx_buff,(uint8_t*)rx_buff,2,100);
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_SET);
}

/**
 * @brief  Clear led screen.
 * @retval none
 */
void Max7219_Clear(void)
{
	for (uint8_t i = 0; i <= 7; i++)
		Max7219_Send((1+i),0x00);
}


/**
 * @brief  Initialize led screen.
 * @retval none
 */
void Max7219_Init(void)
{
	uint8_t init_data[8] = {0x09,0x00,0x0B,0x07,0x0C,0x01,0x0A,0x0F};

	Max7219_Send(init_data[0], init_data[1]);
	Max7219_Send(init_data[2], init_data[3]);
	Max7219_Send(init_data[4], init_data[5]);
	Max7219_Send(init_data[6], init_data[7]);

	Max7219_Clear();
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

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

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
