/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2019 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdlib.h>
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define BUFSIZE 64
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
int16_t buffer[BUFSIZE];
int8_t ptr_in = 0;
int8_t ptr_out = 0;
int8_t is_interrupt = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void buffer_push(int16_t value) {
    buffer[ptr_in++] = value;
    if (ptr_in >= BUFSIZE) ptr_in = 0;
}

int16_t buffer_pop() {
    int16_t ret = buffer[ptr_out++];
    if (ptr_out >= BUFSIZE) ptr_out = 0;
    return ret;
}

int16_t buffer_get() {
    return buffer[ptr_out];
}

void buffer_clear() {
	ptr_in = 0;
	ptr_out = 0;
}

int8_t buffer_elements() {
    if (ptr_in >= ptr_out)
        return (ptr_in - ptr_out);
    else
        return ((BUFSIZE - ptr_out) + ptr_in);
}

int8_t is_sign(char symbol) {
	if (symbol == '+' || symbol == '-' || symbol == '*' || symbol == '/') return 1;
	return 0;
}

int32_t make_result(int32_t before, int32_t after, char sign) {
	if (sign == '+') return (int32_t) (before + after);
	if (sign == '-') return (int32_t) (before - after);
	if (sign == '*') return (int32_t) (before * after);
	if (sign == '/') {
		if (after == 0) return 32768;
		return (int32_t) (before / after);
	}
	return -1;
}

void signalyze_error() {
	HAL_UART_Transmit(&huart6, "error\r\n", 7, 15);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_SET);
	HAL_Delay(500);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET);
}

void toggle_interrupt() {
	if (is_interrupt == 0) {
		HAL_NVIC_EnableIRQ(USART6_IRQn);
		HAL_UART_Transmit_IT(&huart6, "Interrupts enabled\r\n", 20);
		while( HAL_UART_GetState (&huart6) == HAL_UART_STATE_BUSY_TX ) ;
		is_interrupt = 1;
	} else {
		HAL_NVIC_DisableIRQ(USART6_IRQn);
		HAL_UART_Transmit(&huart6, "Interrupts disabled\r\n", 21, 30);
		is_interrupt = 0;
	}
}

void transmit(int16_t* symbol, int8_t count) {
	if (is_interrupt == 0) {
		HAL_UART_Transmit(&huart6, symbol, count, 5);
	} else {
		HAL_UART_Transmit_IT(&huart6, symbol, count);
		while( HAL_UART_GetState (&huart6) == HAL_UART_STATE_BUSY_TX ) ;
	}
}

HAL_StatusTypeDef receive(int16_t* symbol) {
	if (is_interrupt == 0) {
		return HAL_UART_Receive(&huart6, symbol, 1, 5);
	} else {
		return HAL_UART_Receive_IT(&huart6, symbol, 1);
	}
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	char old_symb = '`';
	char symbol = '`';
	int32_t before = 0;
	int32_t after = 0;
	int32_t result;
	int8_t counter = 0;
	int8_t error_flag = 0;
	char sign;
	int8_t number = 0;
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
  MX_USART6_UART_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */


	  while (1) {
		counter++;
		old_symb = '`';
		symbol = '`';
	    while (symbol == old_symb) {
			receive(&symbol);
	    	if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_15) == GPIO_PIN_RESET) {
	    		toggle_interrupt();
	    		HAL_Delay(500);
	    		continue;
	    	}
	    }
	    if (symbol >= '0' && symbol <= '9') {
	    	if (counter > 5) {
	    		error_flag = 1;
	    		break;
	    	}
	    	buffer_push(symbol);
	    	number = atoi(&symbol);
	    	before = 10*before + number;
	    	transmit(&symbol, 1);
    	} else if (is_sign(symbol) == 1 && counter > 1) {
	   		buffer_push(symbol);
	   		sign = symbol;
	   		transmit(&symbol, 1);
	  		break;
	    } else {
	    	error_flag = 1;
	    	break;
	    }
	  }

	  if (before > 32767 || before < -32768) error_flag = 1;

	  counter = 0;
	  if (error_flag == 1) {
		  signalyze_error();
		  error_flag = 0;
		  buffer_clear();
		  before = 0;
		  after = 0;
		  continue;
	  }

	  while (1) {
	  	counter++;
		old_symb = '`';
	  	symbol = '`';
	  	while (symbol == old_symb) {
			receive(&symbol);
		}
	  	if (symbol >= '0' && symbol <= '9') {
	  	    if (counter > 5) {
	  	    	error_flag = 1;
	  	    	break;
	  	    }
	  	    buffer_push(symbol);
	  	    number = atoi(&symbol);
	  	    after = 10*after + number;
	  	    transmit(&symbol, 1);
	     } else if (symbol == '=') {
	  	   	buffer_push(symbol);
	  	   	transmit(&symbol, 1);
	  	  	break;
	  	 } else {
	  	   	error_flag = 1;
	      	break;
	 	 }
	  }

	  if (after > 32767 || after < -32768) error_flag = 1;

	  counter = 0;
	  if (error_flag == 1) {
	  	signalyze_error();
	  	error_flag = 0;
	  	buffer_clear();
	  	before = 0;
	  	after = 0;
	  	continue;
	  }

	  result = make_result(before, after, sign);
	  if (result > 32767 || result < -32768) error_flag = 1;
	  if (error_flag == 1) {
	  	  signalyze_error();
	  	  error_flag = 0;
	  	  buffer_clear();
	  	  before = 0;
	  	  after = 0;
	  	  continue;
	  }
	  int16_t* res[50];
	  sprintf((char*) res, "%d\r\n", result);
	  buffer_push(result);
	  transmit(res, strlen((char*) res));

	  buffer_clear();
	  before = 0;
	  after = 0;
	  HAL_Delay(100);
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

  /**Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /**Initializes the CPU, AHB and APB busses clocks
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /**Initializes the CPU, AHB and APB busses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
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
