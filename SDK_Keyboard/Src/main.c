/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "kb.h"
#include "sdk_uart.h"
#include "pca9538.h"
#include "oled.h"
#include "fonts.h"

#include <stdio.h>
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void KB_Test( void );
void OLED_KB( uint8_t OLED_Keys[]);
void oled_Reset( void );
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
#define BUFSIZE 64

int32_t number1 = 0;
int32_t number2 = 0;
char* signs[] = {"+", "-", "*", "/"};
int8_t sign_ptr = -1;
int8_t part = 0;
char string1[6];
char string2[6];
char string_res[50];
int8_t number_count = 0;


void show_error() {
	oled_Fill(Black);
	oled_SetCursor(0,0);
	oled_WriteString("error", Font_11x18, White);
	oled_UpdateScreen();
	number_count = 0;
	number1 = 0;
	number2 = 0;
	part = 0;
	sign_ptr = -1;
}

int8_t is_sign(char symbol) {
	if (symbol == '+' || symbol == '-' || symbol == '*' || symbol == '/') return 1;
	return 0;
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */


  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_USART6_UART_Init();
  /* USER CODE BEGIN 2 */
  oled_Init();

  /* USER CODE END 2 */



  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  int number_1;
  int number_2;
  int nc;

  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

	  while (part == 0) {
		  number_1 = number1;
		  number_2 = number2;
		  nc = number_count;
		  KB_Test();
		  if (number_count > 5) {
			  show_error();
			  break;
		  }
		  if (sign_ptr != -1) number_count = 5;
	  }

	  number_count = 0;

	  while (part == 1) {
		  KB_Test();
		  if (number_count > 5) {
			  show_error();
			  break;
		  }
	  }

	  number_count = 0;
	  number1 = 0;
	  number2 = 0;
	  sign_ptr = -1;

  }
  /* USER CODE END 3 */
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
  /** Initializes the CPU, AHB and APB busses clocks
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
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
void KB_Test( void ) {
	uint8_t Row[4] = {ROW1, ROW2, ROW3, ROW4}, Key, OldKey, OLED_Keys[12] = {0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30};
	//oled_Reset();
	//oled_WriteString("From bottom to top", Font_7x10, White);
	//OLED_KB(OLED_Keys);
	//oled_UpdateScreen();
	Key = Check_Row( Row[0] );
	for ( int i = 0; i < 4; i++ ) {
		OldKey = Key;
		Key = Check_Row( Row[i] );
		Key = Check_Row( Row[i] );
		Key = Check_Row( Row[i] );
		Key = Check_Row( Row[i] );
		Key = Check_Row( Row[i] );
		Key = Check_Row( Row[i] );
		Key = Check_Row( Row[i] );
		Key = Check_Row( Row[i] );
		Key = Check_Row( Row[i] );
		Key = Check_Row( Row[i] );
		if ( Key == 0x01 ) {
			if (i == 0) {
				if (part == 0) {
					number1 = number1*10 + 3;
					sprintf(string1, "%d", number1);
					oled_Fill(Black);
					oled_SetCursor(0,0);
					oled_WriteString(string1, Font_11x18, White);
					oled_UpdateScreen();
					number_count++;
				} else if (part == 1) {
					number2 = number2*10 + 3;
					sprintf(string2, "%d", number2);
					oled_Fill(Black);
					oled_SetCursor(0,0);
					oled_WriteString(string1, Font_11x18, White);
					oled_SetCursor(0,20);
					oled_WriteString(string2, Font_11x18, White);
					oled_UpdateScreen();
					number_count++;
				}
			} else if (i == 1) {
				if (part == 0) {
					number1 = number1*10 + 6;
					sprintf(string1, "%d", number1);
					oled_Fill(Black);
					oled_SetCursor(0,0);
					oled_WriteString(string1, Font_11x18, White);
					oled_UpdateScreen();
					number_count++;
				} else if (part == 1) {
					number2 = number2*10 + 6;
					sprintf(string2, "%d", number2);
					oled_Fill(Black);
					oled_SetCursor(0,0);
					oled_WriteString(string1, Font_11x18, White);
					oled_SetCursor(0,20);
					oled_WriteString(string2, Font_11x18, White);
					oled_UpdateScreen();
					number_count++;
				}
			} else if (i == 2) {
				if (part == 0) {
					number1 = number1*10 + 9;
					sprintf(string1, "%d", number1);
					oled_Fill(Black);
					oled_SetCursor(0,0);
					oled_WriteString(string1, Font_11x18, White);
					oled_UpdateScreen();
					number_count++;
				} else if (part == 1) {
					number2 = number2*10 + 9;
					sprintf(string2, "%d", number2);
					oled_Fill(Black);
					oled_SetCursor(0,0);
					oled_WriteString(string1, Font_11x18, White);
					oled_SetCursor(0,20);
					oled_WriteString(string2, Font_11x18, White);
					oled_UpdateScreen();
					number_count++;
				}
			} else if (i == 3) {
				if (part == 0) {
					if (sign_ptr != -1) {
						part = 1;
						oled_SetCursor(0,20);
						oled_WriteString("0", Font_11x18, White);
						oled_UpdateScreen();
					}
				} else if (part == 1) {
					int64_t res;
					int64_t n1 = number1;
					int64_t n2 = number2;
					switch (sign_ptr) {
					case 0:
						res = n1+n2;
						if (res > 2147483647 || res < -2147483648) {
							show_error();
							break;
						}
						sprintf(string_res, "%d", number1+number2);
						break;
					case 1:
						res = n1-n2;
						if (res > 2147483647 || res < -2147483648) {
							show_error();
							break;
						}
						sprintf(string_res, "%d", number1-number2);
						break;
					case 2:
						res = n1*n2;
						if (res > 2147483647 || res < -2147483648) {
							show_error();
							break;
						}
						sprintf(string_res, "%d", number1*number2);
						break;
					case 3:
						res = n1/n2;
						if (res > 2147483647 || res < -2147483648) {
							show_error();
							break;
						}
						sprintf(string_res, "%d", number1/number2);
						break;
					}
					if (part == 1) {
						oled_Fill(Black);
						oled_SetCursor(0,0);
						oled_WriteString(string1, Font_11x18, White);
						oled_SetCursor(0,20);
						sprintf(string2, "%d=", number2);
						oled_WriteString(string2, Font_11x18, White);
						oled_SetCursor(0,40);
						oled_WriteString(string_res, Font_11x18, White);
						oled_UpdateScreen();
						part = 0;
					}
				}
				UART_Transmit( (uint8_t*)"enter\r\n" );
			}
			//OLED_Keys[2+3*i] = 0x31;
			//OLED_KB(OLED_Keys);
		} else if ( Key == 0x02 ) {
			if (i == 0) {
				if (part == 0) {
					number1 = number1*10 + 2;
					sprintf(string1, "%d", number1);
					oled_Fill(Black);
					oled_SetCursor(0,0);
					oled_WriteString(string1, Font_11x18, White);
					oled_UpdateScreen();
					number_count++;
				} else if (part == 1) {
					number2 = number2*10 + 2;
					sprintf(string2, "%d", number2);
					oled_Fill(Black);
					oled_SetCursor(0,0);
					oled_WriteString(string1, Font_11x18, White);
					oled_SetCursor(0,20);
					oled_WriteString(string2, Font_11x18, White);
					oled_UpdateScreen();
					number_count++;
				}
			} else if (i == 1) {
				if (part == 0) {
					number1 = number1*10 + 5;
					sprintf(string1, "%d", number1);
					oled_Fill(Black);
					oled_SetCursor(0,0);
					oled_WriteString(string1, Font_11x18, White);
					oled_UpdateScreen();
					number_count++;
				} else if (part == 1) {
					number2 = number2*10 + 5;
					sprintf(string2, "%d", number2);
					oled_Fill(Black);
					oled_SetCursor(0,0);
					oled_WriteString(string1, Font_11x18, White);
					oled_SetCursor(0,20);
					oled_WriteString(string2, Font_11x18, White);
					oled_UpdateScreen();
					number_count++;
				}
			} else if (i == 2) {
				if (part == 0) {
					number1 = number1*10 + 8;
					sprintf(string1, "%d", number1);
					oled_Fill(Black);
					oled_SetCursor(0,0);
					oled_WriteString(string1, Font_11x18, White);
					oled_UpdateScreen();
					number_count++;
				} else if (part == 1) {
					number2 = number2*10 + 8;
					sprintf(string2, "%d", number2);
					oled_Fill(Black);
					oled_SetCursor(0,0);
					oled_WriteString(string1, Font_11x18, White);
					oled_SetCursor(0,20);
					oled_WriteString(string2, Font_11x18, White);
					oled_UpdateScreen();
					number_count++;
				}
			} else if (i == 3) {
				if (part == 0) {
					number1 = number1*10;
					sprintf(string1, "%d", number1);
					oled_Fill(Black);
					oled_SetCursor(0,0);
					oled_WriteString(string1, Font_11x18, White);
					oled_UpdateScreen();
					if (number1 != 0) number_count++;
				} else if (part == 1) {
					number2 = number2*10;
					sprintf(string2, "%d", number2);
					oled_Fill(Black);
					oled_SetCursor(0,0);
					oled_WriteString(string1, Font_11x18, White);
					oled_SetCursor(0,20);
					oled_WriteString(string2, Font_11x18, White);
					oled_UpdateScreen();
					if (number2 != 0) number_count++;
				}
			}
			//OLED_Keys[1+3*i] = 0x31;
			//OLED_KB(OLED_Keys);
		} else if ( Key == 0x04 ) {
			if (i == 0) {
				if (part == 0) {
					number1 = number1*10 + 1;
					sprintf(string1, "%d", number1);
					oled_Fill(Black);
					oled_SetCursor(0,0);
					oled_WriteString(string1, Font_11x18, White);
					oled_UpdateScreen();
					number_count++;
				} else if (part == 1) {
					number2 = number2*10 + 1;
					sprintf(string2, "%d", number2);
					oled_Fill(Black);
					oled_SetCursor(0,0);
					oled_WriteString(string1, Font_11x18, White);
					oled_SetCursor(0,20);
					oled_WriteString(string2, Font_11x18, White);
					oled_UpdateScreen();
					number_count++;
				}
			} else if (i == 1) {
				if (part == 0) {
					number1 = number1*10 + 4;
					sprintf(string1, "%d", number1);
					oled_Fill(Black);
					oled_SetCursor(0,0);
					oled_WriteString(string1, Font_11x18, White);
					oled_UpdateScreen();
					number_count++;
				} else if (part == 1) {
					number2 = number2*10 + 4;
					sprintf(string2, "%d", number2);
					oled_Fill(Black);
					oled_SetCursor(0,0);
					oled_WriteString(string1, Font_11x18, White);
					oled_SetCursor(0,20);
					oled_WriteString(string2, Font_11x18, White);
					oled_UpdateScreen();
					number_count++;
				}
			} else if (i == 2) {
				if (part == 0) {
					number1 = number1*10 + 7;
					sprintf(string1, "%d", number1);
					oled_Fill(Black);
					oled_SetCursor(0,0);
					oled_WriteString(string1, Font_11x18, White);
					oled_UpdateScreen();
					number_count++;
				} else if (part == 1) {
					number2 = number2*10 + 7;
					sprintf(string2, "%d", number2);
					oled_Fill(Black);
					oled_SetCursor(0,0);
					oled_WriteString(string1, Font_11x18, White);
					oled_SetCursor(0,20);
					oled_WriteString(string2, Font_11x18, White);
					oled_UpdateScreen();
					number_count++;
				}
			} else if (i == 3) {
				if (part == 0) {
					sign_ptr++;
					if (sign_ptr > 3) sign_ptr = 0;
					sprintf (string1, "%d%s", number1, signs[sign_ptr]);
					oled_Fill(Black);
					oled_SetCursor(0,0);
					oled_WriteString(string1, Font_11x18, White);
					oled_UpdateScreen();
				}
				UART_Transmit( (uint8_t*)"sign\r\n" );
			}
			//OLED_Keys[3*i] = 0x31;
			//OLED_KB(OLED_Keys);
		}
	}
	HAL_Delay(200);
}
void OLED_KB( uint8_t OLED_Keys[12]) {
	for (int i = 3; i >= 0; i--) {
		oled_SetCursor(56, 5+(4-i)*10);
		for (int j = 0; j < 3; j++) {
			oled_WriteChar(OLED_Keys[j+3*i], Font_7x10, White);
		}
	}
	oled_UpdateScreen();
}
void oled_Reset( void ) {
	oled_Fill(Black);
	oled_SetCursor(0, 0);
	oled_UpdateScreen();
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
