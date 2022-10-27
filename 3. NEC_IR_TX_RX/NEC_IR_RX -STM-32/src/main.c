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

/* This software is for decoding various IR REMOTES [NEC PROTOCOL]*/

/* Includes ------------------------------------------------------------------*/

 /*IR Reciever Board connection: signal_pin => PA1 , VCC_pin => +VCC, GND_pin => GND */

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
volatile uint8_t Device_Address, cmp_Device_Address, cmd , cmp_cmd ;

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

			/* Re arranging 4 8bit data  into a single 32 bit varibale: [in reverse order of actual NEC frame]
			 * creating this for transmitter , we will copy this value from serial monitor and paste it in transmitter code
			 * & trasnmit this 32 bit data from LSB to MSB to get the corrcet oreded signal at reciever side.
			 * i.e [Device add]->[cmp_Device_add]->[cmd]->[cmp_cmd]
			*/

			Recieved_NEC_frame = (cmp_cmd << 24) | (cmd << 16) | (cmp_Device_Address << 8) | (Device_Address);
			/*                    MSB                                                                    LSB*/

			int_to_arr(NEC_ARR,Recieved_NEC_frame,NEC_FRAME_SIZE);
			HAL_UART_Transmit(&huart1,NEC_ARR,NEC_FRAME_SIZE,100);
			HAL_UART_Transmit(&huart1,f,1,100);

			pulse_count = -1;
			Recieved_NEC_frame = 0;
			Device_Address = 0;
			cmp_Device_Address = 0;
			cmd = 0;
			cmp_cmd = 0;
		}
	}
}



/**
 * @brief  convert int to array.
 * @retval none
 */
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


/**
 * @brief  EXTI handler routine
 * @retval none
 */
void HAL_GPIO_EXTI_Callback(uint16_t gpio_pin)
{	
	static volatile uint16_t current_time;
	current_time = Micros();
	TIM6->CNT = 0;
	
	static uint8_t START_OF_FRAME = 0;
	HAL_NVIC_ClearPendingIRQ(EXTI1_IRQn);

	/*NEC FRAME with respect to Time:
	*
	* [START OF FRAME]->[8BIT DEVICE ADDRESS]->[8BIT INVERSED DEVICE ADDRESS]->[8BIT CMD]->[8BIT INVERSE CMD]->[END OF FRAME]
	*                   LSB               MSB   LSB                       MSB   LSB   MSB   LSB          MSB
	*  0ms                                                                                                              67 ms
	*  Time -->
	*/

	/* NEC Transmitter will trasmit LSB first of each 8bit data segmennt:
	 * Accumulating each Recieved 8 bit data into corrospomding 8 bit variables
	*/

	if (current_time > 13000 && current_time < 14000){  

		START_OF_FRAME = 1;
		Recieved_NEC_frame = 0;
		pulse_count = 0;

	} else if (pulse_count >= 0 && pulse_count < 32) {   

		if ((current_time > 2000 && current_time < 3000) && START_OF_FRAME == 1){
			//Recieved_NEC_frame |= (1 << (pulse_count));

			if (pulse_count >= 0 && pulse_count <= 7)
				Device_Address |= (1 << pulse_count);

			else if (pulse_count >= 8 && pulse_count <= 15)
				cmp_Device_Address |= (1 << pulse_count);

			else if (pulse_count >= 16 && pulse_count <= 23)
				cmd |= (1 << pulse_count);

			else if (pulse_count >= 24 && pulse_count <= 31)
				cmp_cmd |= (1 << pulse_count);

			pulse_count++;

		} else if ((current_time > 1000 && current_time < 2000) && START_OF_FRAME == 1){
	
			pulse_count++;
		}
	}

	if (pulse_count > 31){
		START_OF_FRAME = 0;
	}
}


/**
 * @brief  Calculate absolute value.
 * @retval int
 */
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
