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
#include "adc.h"
#include "i2c.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"
#include <string.h>
#define BUFF_SIZE 7
#define EEPROM_I2C_ADDRESS 0x50         /* Making A0, A1 & A2 Ground*/

/*Local Function Prototypes*/
void SystemClock_Config(void);
static float Get_Temparature(void);
void Send_To_Serial_Monitor (float);
void Write_To_I2C_EEPROM(float value);
void Write_To_SPI_FLASH(float value);
uint8_t Read_From_I2C_EEPROM(void);


/*Global Variables*/
float temparature =  0;

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{	
	/*initialize drivers*/
	HAL_Init();
	SystemClock_Config();
	MX_GPIO_Init();
	MX_ADC1_Init();
	MX_I2C1_Init();
	MX_SPI1_Init();
	MX_USART1_UART_Init();

	uint8_t *msg1 = "Written-Values:\n";
	uint8_t *msg2 = "Read-Values:\n";
	
	/**
	 * Get temparature
	 * Save to EEPROM & FLASH
	 * Send To Serial Monitor
	 * Delay & Repeat after 60s
	 */
	while (1){

		/**
		 * write 256 values to EEPROM
		 * Read those 256 values form EEPROM and send to serial monitor
		 */
		HAL_UART_Transmit(&huart1, msg1, strlen(msg1), 100);
		for (int i = 0; i < 256; i++){
			temparature = Get_Temparature();
			Write_To_I2C_EEPROM(temparature);
			Send_To_Serial_Monitor(temparature);

			/* 5ms delay is needed to complete EEPROM internal write cycle [10ms for safety]*/
			HAL_Delay(10);
		}
		
		HAL_Delay(100);

		HAL_UART_Transmit(&huart1, msg2, strlen(msg2), 100);
		for (int i = 0; i < 256; i++){
			int8_t temp = Read_From_I2C_EEPROM();
			Send_To_Serial_Monitor(temp);
			HAL_Delay(10);
		}

		/*
		Write_To_SPI_FLASH(temparature);
		*/
		HAL_Delay(5000);
	}
	return 0;
}


/**
 * @brief  Get temparature 
 * @retval float
 */
static float Get_Temparature(void)
{	
	uint16_t raw_adc = 0;

	/**
	 * Read adc steps :
	 * start adc
	 * poll until conversation is done
	 * read the value 
	 * stop the adc
	 */
	HAL_ADC_Start(&hadc1);

	if (HAL_ADC_PollForConversion(&hadc1, 5) == HAL_OK)
		raw_adc = HAL_ADC_GetValue(&hadc1);

	HAL_ADC_Stop(&hadc1);

	/** 
	 * thermistor range -24 to +80 degree C [diff 104]
     * ADC range 0 to 4096 [12 bit] [diff 4096]
     */
    float frac = (float)(104)/(float)(4096);

    /* IF [temparature increase -> voltage increase] */
    temparature = -24 + (frac * (float)raw_adc); 

    /** 
	 * IF [temparature increae -> volt decrease]
     * float temparature = 80 - (frac * (float)(4096))
	 */
    return temparature;
}


/**
 * @brief Sending temparature data to Serial Monitor 
 * @retval None
 */
void Send_To_Serial_Monitor (float value)
{   
	uint8_t tx_buff[BUFF_SIZE];
    value = value * 100;

    /* Before point [2 digits] */
    int8_t d4 = (((int)value) / 1000) % 10;
    int8_t d3 = (((int)value) / 100) % 10;

    /* After point 2 digits */
    int8_t d2 = ((int)value / 10) % 10;
    int8_t d1 = (int)value % 10;

	/*load data into tx-buff*/
	tx_buff[0] = d4 + 48;
	tx_buff[1] = d3 + 48;
	tx_buff[2] = '.';
	tx_buff[3] = d2 + 48;
	tx_buff[4] = d1 + 48;
	tx_buff[5] = 'C';
	tx_buff[6] = '\n';

	/*send tx-buff to pc*/
	HAL_UART_Transmit(&huart1, tx_buff, BUFF_SIZE, 100);
}



/**
 * MEMORY MAP OF K24C02 EEPROM
 * WORD SIZE = 8 BIT OR 1 BYTE
 * PAGE SIZE = 8 BYTE , MEANS 8 WORDS IN A SINGLE PAGE
 * TOTAL PAGES = 32
 * TOTAL EEPROM SIZE = 32 * 8 BYTE = 256 BYTE OR 2048 BITS
 * WORD ADDRESS 0 TO 255
*/

/**
 * @brief  Writing temparature data to EEPROM 
 * @retval None
 */
void Write_To_I2C_EEPROM(float value)
{	
	uint8_t data[2];
	static uint8_t word_address = 0;

	/*Word Address*/
	if (word_address == 255)
		word_address = 0;

	data[0] = word_address++;

	/*Data*/
	data[1] = (int8_t)value;

	/*write single byte data to EEPROM*/
	HAL_I2C_Master_Transmit(&hi2c1, ((EEPROM_I2C_ADDRESS) << 1 | 0), data, 2, 100 );
}

/**
 * @brief  reading temparature from EERPOM
 * @retval uint8_t
 */
uint8_t Read_From_I2C_EEPROM(void)
{
	static uint8_t word_address = 0;
	uint8_t data;

	/*Word Address*/
	if (word_address == 255)
		word_address = 0;

	/*write register address to EEPROM*/
	HAL_I2C_Master_Transmit(&hi2c1, ((EEPROM_I2C_ADDRESS << 1) | 0), &word_address ,1, 100 );

	/*Read 1 byte from EEPROM*/
	HAL_I2C_Master_Receive(&hi2c1, ((EEPROM_I2C_ADDRESS << 1) | 1), &data, 1, 100);
	word_address++;
	return data;
}


/**
 * @brief  Writing temparature data to FLASH
 * @retval None
 */
void Write_To_SPI_FLASH(float value)
{

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
