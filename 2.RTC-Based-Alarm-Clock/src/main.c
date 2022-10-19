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


/* DESIGN FLOW*/

/* SET TIME :
 * press hour button to set hour -> press minute button to set minute -> Done
 *
 * CONFIG ALARM:
 * press config alarm button -> press hour button -> press minute button -> press config alarm button -> Done
 * 
 * 
 * ALARM ON / OFF:
 * press alarm on/off button to toggle alarm status [on / off]
*/


/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "rtc.h"
#include "gpio.h"
#include "seven_segment.h"
#include <stm32f407xx.h>

void SystemClock_Config(void);
void get_RTC_time (void);

volatile uint8_t rtc_hour, rtc_min, alrm_hr, alrm_min;
uint8_t config_alarm_flag = 0, alarm_status = 0;

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{	
	uint16_t combined_time = 0;

	HAL_Init();
	SystemClock_Config();
	MX_GPIO_Init();
	MX_RTC_Init();
	setup_seven_segment_gpio();

	while (1){

		get_RTC_time();
		combined_time = (rtc_hour * 100) + rtc_min;
		show_number(combined_time);

		if (config_alarm_flag){
			//setup_alarm_time ();  // yet to implement
		}
	}
}


void get_RTC_time (void)
{	
	RTC_TimeTypeDef t_Current = {0};
	RTC_DateTypeDef d_Current = {0};
	volatile int8_t bcd_min, bcd_hr;

	if((HAL_RTC_GetTime(&hrtc,&t_Current,RTC_FORMAT_BCD) == HAL_OK) && 
		(HAL_RTC_GetDate(&hrtc,&d_Current,RTC_FORMAT_BCD) == HAL_OK)){

		/* BCD to DEC conversion */
		bcd_hr = t_Current.Hours;
		bcd_min = t_Current.Minutes;
		rtc_hour = (((bcd_hr & 0xF0) >> 4) * 10) + (bcd_hr & 0x0F);
		rtc_min = (((bcd_min & 0xF0) >> 4) * 10) + (bcd_min & 0x0F);
	}
}



void HAL_GPIO_EXTI_Callback(uint16_t gpio_pin)	
{	
	/* GPIO_PIN_0 => set alarm button
	 * GPIO_PIN_1 => Alarm on/off Button
 	* GPIO_PIN_2 => Hour Button [both for alarm and time]
 	* GPIO_PIN_3 => Minute Button [both for alarm and time]
	*/

	HAL_NVIC_DisableIRQ(EXTI0_IRQn);
	HAL_NVIC_DisableIRQ(EXTI1_IRQn);
	HAL_NVIC_DisableIRQ(EXTI2_IRQn);
	HAL_NVIC_DisableIRQ(EXTI3_IRQn);


	RTC_TimeTypeDef user_time = {0};
	uint8_t temp_tim_hr = rtc_hour;
	uint8_t temp_tim_mn = rtc_min;

	switch (gpio_pin){
	
	case GPIO_PIN_0:
		config_alarm_flag = 1;
		break;

	case GPIO_PIN_1:
		alarm_status = (~(alarm_status)) & 0x01;
		break;

	case GPIO_PIN_2:
		if (config_alarm_flag == 1){

			alrm_hr++;

		} else {
			if (temp_tim_hr <= 22)
				temp_tim_hr++;
			else
				temp_tim_hr = 0;

			temp_tim_hr = ((temp_tim_hr / 10) << 4) | (temp_tim_hr % 10);
			temp_tim_mn = ((temp_tim_mn / 10) << 4) | (temp_tim_mn % 10);

			user_time.Hours = temp_tim_hr;
			user_time.Minutes = temp_tim_mn;

			HAL_RTC_SetTime(&hrtc,&user_time,RTC_FORMAT_BCD);
		}
		break;

	case GPIO_PIN_3:
		if (config_alarm_flag == 1){

			alrm_min++;

		} else {
			if (temp_tim_mn <= 58)
				temp_tim_mn++;
			else 
				temp_tim_mn = 0;

			temp_tim_hr = ((temp_tim_hr / 10) << 4) | (temp_tim_hr % 10);
			temp_tim_mn = ((temp_tim_mn / 10) << 4) | (temp_tim_mn % 10);

			user_time.Hours = temp_tim_hr;
			user_time.Minutes = temp_tim_mn;

			HAL_RTC_SetTime(&hrtc,&user_time,RTC_FORMAT_BCD);
		}
		break;
	}
			
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);
	HAL_NVIC_EnableIRQ(EXTI1_IRQn);
	HAL_NVIC_EnableIRQ(EXTI2_IRQn);
	HAL_NVIC_EnableIRQ(EXTI3_IRQn);
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
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_LSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.LSIState = RCC_LSI_ON;
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
