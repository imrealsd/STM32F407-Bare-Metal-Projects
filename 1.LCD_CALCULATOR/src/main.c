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

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "gpio.h"
#include "i2c_lcd.h"
#include "keypad.h"



void SystemClock_Config(void);
void key_action (char key);
char get_keys_from_key_val(uint8_t key_val);
void show_result_to_lcd(int32_t result, int32_t remainder);
void convert_int_to_string(int32_t num,char *str, int8_t size);

int main(void)
{	
	uint8_t key_val;
	char key;

	HAL_Init();
	SystemClock_Config();
	MX_GPIO_Init();
	MX_I2C1_Init();
	start_i2c_lcd();
	setup_keypad_gpio();
	key_val = scan_keypad();


	while (1){

		while (key_val == 0){
			key_val = scan_keypad();
			HAL_Delay(170);
		}

		key = get_keys_from_key_val(key_val);
		if (key != '=')
			i2c_lcd_data(key);
		key_action(key);
		key_val = 0;
	}
}

/* 4x4 calculator Keypad Layout:

    --------------
	| 1  2  3  + |
	| 4  5  6  - |
	| 7  8  9  x |
	| C  0  =  / |
	--------------
*/

char get_keys_from_key_val(uint8_t key_val)
{
	switch(key_val){

	case 1:
	case 2:
	case 3:
		return key_val + 48;

	case 4:
		return '+';
		
	case 5:
	case 6:
	case 7:
		return key_val + 48 - 1;

	case 8:
		return '-';

	case 9 :
	case 10:
	case 11:
		return key_val + 48 - 2;

	case 12:
		return 'x';

	case 13:
		return 'C';

	case 14:
		return '0';
		
	case 15:
		return '=';

	case 16:
		return '/';

	default:
		return 0;
	}
}

void key_action (char key)
{	
	static int8_t addition_flag = 0;
	static int8_t subtraction_flag = 0;
	static int8_t multiplication_flag = 0;
	static int8_t division_flag = 0;

	static int32_t num1 = 0, num2 = 0, result = 0, remainder = 0;

	if (key == '='){

		// show calculated value to lcd
		if (addition_flag){
			result = num1 + num2;
		}
		if (subtraction_flag){
			result = num1 - num2;
		}
		if (multiplication_flag){
			result = num1 * num2;
		}
		if (division_flag){
			result = num1 / num2;
			remainder = num1 % num2;
		}

		show_result_to_lcd(result, remainder);

		/*reset flags and values*/
		addition_flag = 0;
		subtraction_flag = 0;
		multiplication_flag = 0;
		division_flag = 0;
		num1 = 0;
		num2 = 0;
		result = 0;
		remainder = 0;
	}

	if (key == 'C'){
		i2c_lcd_clear();
		i2c_lcd_set_cursor(0,0);
	}

	if (key >= '0' && key <= '9'){

		if (addition_flag || subtraction_flag || multiplication_flag || division_flag){
			num2 = (num2 * 10) + (key-48);
		} else {
			num1 =  (num1 * 10) + (key-48);
		}
	}

	if (key == '+' || key == '-' || key == 'x' || key == '/'){

		if (key == '+')
			addition_flag = 1;
		if (key == '-')
			subtraction_flag = 1;
		if (key == 'x')
			multiplication_flag = 1;
		if (key == '/')
			division_flag = 1;
	}

}

void show_result_to_lcd(int32_t result, int32_t remainder)
{
	uint8_t negative_flag = 0;
	char result_buff[8];
	char remainder_buff[8];
	i2c_lcd_set_cursor(1,0);
	i2c_lcd_string("= ");

	if (result == 0){
		i2c_lcd_data('0');
		goto REMAINDER;
	}

	(result > 0) ? (negative_flag = 0) : (negative_flag = 1);
	if (negative_flag){
		i2c_lcd_data('-');
		result = -result;
	}
	convert_int_to_string(result,result_buff,8);
	i2c_lcd_string(result_buff);

	REMAINDER:

	if (remainder != 0){
		convert_int_to_string(remainder,remainder_buff,8);
		i2c_lcd_set_cursor(1,8);
		i2c_lcd_string(remainder_buff);
	}
}


void convert_int_to_string(int32_t num,char *str, int8_t size)
{

	int8_t temp, index = 0;
	
	for (int8_t i = 0; i < size; i++){str[i] = 0;}

	while (num){
		temp = num % 10;
		str[index] = temp + 48;
		num = num / 10;
		index++;
	}

	for (int8_t i = 0; i < index / 2; i++){
		temp = str[i];
		str[i] = str[index-1-i];
		str[index-1-i] = temp;
	}
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
