/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include "tim.h"
#include "usb_device.h"
#include "gpio.h"

#include "usbd_cdc_if.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

// Function Definitions
void Digest_Bytes(uint8_t* inputBuffer, uint32_t *bufferLength);
void Execute_Function(void);
void LED_Binary(uint8_t);
void Reset_State(void);
void Func_Await_Command(void);
void Func_Debug(void);
void Func_CRC_Echo(void);
void Func_String_Echo(void);

// Define Function Codes
const uint8_t FUNC_AWAIT_COMMAND = 0;
const uint8_t FUNC_DEBUG = 3;
const uint8_t FUNC_CRC_ECHO = 5;
const uint8_t FUNC_STRING_ECHO = 10;

// Constants
const uint8_t BUFFER_SIZE = 64;

// Function and Buffer States
uint8_t BUFFER[BUFFER_SIZE];
uint8_t DIGEST_LIMIT = 1;
uint8_t CURRENT_FUNCTION = FUNC_AWAIT_COMMAND;
uint8_t FUNCTION_STATE = 0;
uint8_t INDEX = 0;

void Digest_Bytes(uint8_t* inputBuffer, uint32_t *bufferLength)
{
	//HAL_GPIO_WritePin(GPIOG, LED_PG1_Pin, GPIO_PIN_RESET);
	for (uint8_t i = 0; i < *bufferLength; i++) {
		BUFFER[INDEX] = inputBuffer[i];
		INDEX++;
		if (INDEX == DIGEST_LIMIT)
		{
			Execute_Function();
			INDEX = 0;
		}
	}
}

void Execute_Function(void)
{
	switch (CURRENT_FUNCTION)
	{
		case FUNC_AWAIT_COMMAND:
			Func_Await_Command();
			break;
		case FUNC_DEBUG:
			Func_Debug();
			break;
		case FUNC_CRC_ECHO:
			Func_CRC_Echo();
			break;
		case FUNC_STRING_ECHO:
			Func_String_Echo();
			break;
	}
}

void Reset_State(void)
{
	DIGEST_LIMIT = 1;
	CURRENT_FUNCTION = FUNC_AWAIT_COMMAND;
	FUNCTION_STATE = 0;
}

void Func_Await_Command()
{
	CURRENT_FUNCTION = BUFFER[0];
	Execute_Function();
}

void Func_Debug()
{
	switch (FUNCTION_STATE)
	{
		case 0:
		{
			HAL_GPIO_WritePin(GPIOE, LED_PE11_Pin, GPIO_PIN_RESET);
			
			uint8_t* buf;
			buf = (uint8_t*)malloc(7);
			buf[4] = 'A';
			buf[5] = 'B';
			buf[6] = 'C';
			CDC_Transmit_FS(buf, 7);
			free(buf);
			Reset_State();
			break;
		}
	}
}

void Func_CRC_Echo()
{
	switch (FUNCTION_STATE)
	{
		case 0:
			DIGEST_LIMIT = 16; // Accept 16 bytes of random data
			FUNCTION_STATE = 1; // Increase the function state
			break;
		case 1:
		{
			// crc32 random data with "TAMU RoboMasters"
			uint8_t crc_buffer[32];
			uint8_t text[16] = "TAMU RoboMasters";
			for (uint8_t i = 0; i < 16; i++)
			{
				crc_buffer[i] = BUFFER[i];
			}
			
			// Add the characters
			for (uint8_t i = 16; i < 32; i++)
			{
				crc_buffer[i] = text[i-16];
			}
			
			// CRC32
			uint32_t polynomial = 0xEDB88320;
			uint32_t crc = 0xFFFFFFFF;
			for (uint8_t byteIndex = 0; byteIndex < 32; byteIndex++)
			{
				for (uint8_t i = 0; i < 8; i++)
				{
					uint8_t bit = (crc_buffer[byteIndex] & (1 << i)) >> i;
					if (((crc & 1) ^ bit) == 1)
					{
						crc = (crc >> 1) ^ polynomial;
					} else
					{
						crc = (crc >> 1);
					}
				}
			}
			crc = crc ^ 0xFFFFFFFF;
			
			// Split crc into 4 bytes
			uint8_t byte0 = (crc >> 24) & 0xFF;
			uint8_t byte1 = (crc >> 16) & 0xFF;
			uint8_t byte2 = (crc >> 8) & 0xFF;
			uint8_t byte3 = crc & 0xFFFF;
			
			uint8_t outBytes[] = {byte0, byte1, byte2, byte3};
			CDC_Transmit_FS(outBytes, 4);
			//CDC_Transmit_FS(crc_buffer, 32);
			
			Reset_State();
			break;
		}
	}
}


uint32_t stringLength;
uint32_t byteIndex;
uint8_t* stringBuffer;
void Func_String_Echo()
{
	switch (FUNCTION_STATE)
	{
		case 0: // Receive command
			DIGEST_LIMIT = 4;
			FUNCTION_STATE = 1;
			break;
		case 1: // Receive string length
		{
			stringLength = (BUFFER[0] << 24) | (BUFFER[1] << 16) | (BUFFER[2] << 8) | BUFFER[3];
			stringLength += 4; // WORKAROUND FOR SENDING DYNAMIC ARRAY
			byteIndex = 4; // WORKAROUND FOR SENDING DYNAMIC ARRAY
			
			if (stringLength <= BUFFER_SIZE)
			{
				DIGEST_LIMIT = stringLength;
			} else
			{
				DIGEST_LIMIT = BUFFER_SIZE;
			}
			
			stringBuffer = (uint8_t*)malloc(stringLength * sizeof(uint8_t));
			
			FUNCTION_STATE = 2;
			break;
		}
		case 2: // Fill the buffer
		{
			//HAL_GPIO_WritePin(GPIOG, LED_PG2_Pin, GPIO_PIN_RESET);
			
			for (uint8_t i=0; i<DIGEST_LIMIT; i++)
			{
				stringBuffer[byteIndex] = BUFFER[i];
				byteIndex++;
			}
			
			if (byteIndex == stringLength)
			{
				// stringBuffer is filled
				CDC_Transmit_FS(stringBuffer, stringLength);
				free(stringBuffer);
				Reset_State();
			} else
			{
				if (stringLength - byteIndex <= BUFFER_SIZE)
				{
					DIGEST_LIMIT = stringLength - byteIndex;
				} else
				{
					DIGEST_LIMIT = BUFFER_SIZE;
				}
			}
			
			break;
		}
	}
}

/*
	This function sets LEDs 1-8 to represent the parameter in binary.
	LED 1 is the MSB (the one next to the H on the board)
*/
void LED_Binary(uint8_t value)
{
	HAL_GPIO_WritePin(GPIOG, LED_PG1_Pin, (value & 128) ? GPIO_PIN_RESET : GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOG, LED_PG2_Pin, (value & 64) ? GPIO_PIN_RESET : GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOG, LED_PG3_Pin, (value & 32) ? GPIO_PIN_RESET : GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOG, LED_PG4_Pin, (value & 16) ? GPIO_PIN_RESET : GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOG, LED_PG5_Pin, (value & 8) ? GPIO_PIN_RESET : GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOG, LED_PG6_Pin, (value & 4) ? GPIO_PIN_RESET : GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOG, LED_PG7_Pin, (value & 2) ? GPIO_PIN_RESET : GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOG, LED_PG8_Pin, (value & 1) ? GPIO_PIN_RESET : GPIO_PIN_SET);
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
  MX_TIM12_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();  /* Call init function for freertos objects (in freertos.c) */
  MX_FREERTOS_Init();
  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 6;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
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
