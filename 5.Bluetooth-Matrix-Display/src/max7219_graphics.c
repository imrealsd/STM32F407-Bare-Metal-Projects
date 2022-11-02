/* Max7219-8x8 Dot Matrix Graphics Driver*/
/**
 ******************************************************************************
* @file           : Max7219.c
* @brief          : Graphics Driver
******************************************************************************
* @owner: Subhadeep Dhang 
* @date : 31 OCT 2022
*
******************************************************************************
*/

#include "max7219_graphics.h"

/*8x8 pixel map for letters*/
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

/*8x8 pixel map for numbers*/
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

/*letter-number shifting logic:

 * Taking a 16 bit/ 2 byte frame & assigning row data to lower byte
 * Let row_data for 0th row = 1 0 1 0 0 0 1 1
 * 
 *          --16bit Frame--
 * 
 *  -higher byte-      -lower byte-
 * 
 * [0 0 0 0 0 0 0 0   1 0 1 0 0 0 1 1 ]
 * 
 * <-  <-  <-  <-  <-  <-  <-   <- <- <- 
 * 
 * After left shifting  whole 16bit frame by 1 bit , sending higher byte as row data 
 * For every 1 bit shift sending 8 row values [row 0 to 7]
 * Doing this until 0th bit shifted to 16th bit [15th bit is MSB of the frame , so 16th bit is out of frame]
*/


/**
 * @brief  Scroll one letter.
 * @retval none
 */
void Max7219_Scroll_Letter(char ch)
{	
	int8_t letter_index = ch - 65, matrix_row, shift_val;
	volatile uint16_t frame = 0;

	/* For left to right [0 to 16] shift*/
	for (shift_val = 0; shift_val <= 16; shift_val++){          
		
		/*for writing 8 marix rows*/
		for (matrix_row = 0; matrix_row < 8; matrix_row++){     

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

	/* For left to right [0 to 16] shift*/
	for (shift_val = 0; shift_val <= 16; shift_val++){         
		
		/*for writing 8 matrix rows*/
		for (matrix_row = 0; matrix_row < 8; matrix_row++){     

			frame = numbers[number_index][matrix_row] << shift_val;
			frame = frame >> 8;
			Max7219_Send(0x1+matrix_row,(uint8_t)frame);
		}
		HAL_Delay(SCROLL_DELAY);
	}
}



/**
 * @brief  Send data for one row.
 * @retval none
 */
void Max7219_Send(uint8_t addr, uint8_t data)
{	
	/*send 2x8 bit = 16 bit data at a time, acc. to max7219 datasheet*/
	uint8_t tx_buff[2] = {addr,data};
	uint8_t rx_buff[2];

	/*cs_on -> 16bit data -> cs_off*/
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
	/*writing 0x00 to every row*/
	for (uint8_t i = 0; i <= 7; i++)
		Max7219_Send((1+i),0x00);
}


/**
 * @brief  Initialize matrix led screen.
 * @retval none
 */
void Max7219_Init(void)
{
	uint8_t init_data[8] = {0x09,0x00,0x0B,0x07,0x0C,0x01,0x0A,0x0F};

	Max7219_Send(init_data[0], init_data[1]);   // disable decoding [ not required in matrix]
	Max7219_Send(init_data[2], init_data[3]);   // scan digit 0 to 7
	Max7219_Send(init_data[4], init_data[5]);   // turn on chip
	Max7219_Send(init_data[6], init_data[7]);   // max intensity

	Max7219_Clear();
}