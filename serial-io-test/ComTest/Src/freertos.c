/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include <stdbool.h>
#include "tim.h"
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
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 128 * 4
};
/* Definitions for buttonReport */
osThreadId_t buttonReportHandle;
const osThreadAttr_t buttonReport_attributes = {
  .name = "buttonReport",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 128 * 4
};
/* Definitions for userControl */
osThreadId_t userControlHandle;
const osThreadAttr_t userControl_attributes = {
  .name = "userControl",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 128 * 4
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void buttonReport_entry(void *argument);
void userControl_entry(void *argument);

extern void MX_USB_DEVICE_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of buttonReport */
  buttonReportHandle = osThreadNew(buttonReport_entry, NULL, &buttonReport_attributes);

  /* creation of userControl */
  userControlHandle = osThreadNew(userControl_entry, NULL, &userControl_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* init code for USB_DEVICE */
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_buttonReport_entry */
/**
* @brief Function implementing the buttonReport thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_buttonReport_entry */
__weak void buttonReport_entry(void *argument)
{
  /* USER CODE BEGIN buttonReport_entry */
  /* Infinite loop */
	
	
	bool previousPressed = false;
	bool currentPressed = false;
	//bool buzzing = false;
  for(;;)
  {
		currentPressed = HAL_GPIO_ReadPin(GPIOB, Button_PB2_Pin) == GPIO_PIN_SET;
		uint8_t outputBuffer[1];
		if (currentPressed && !(previousPressed))
		{
			//HAL_GPIO_WritePin(GPIOE, LED_PE11_Pin, GPIO_PIN_RESET);
			//HAL_GPIO_TogglePin(GPIOE, LED_PE11_Pin);
			
			/*
			if (!buzzing)
			{
				buzzer_on(49, 5000);
				buzzing = true;
			} else
			{
				buzzer_off();
				buzzing = false;
			}
			*/
			
			outputBuffer[0] = 1;
			CDC_Transmit_FS(outputBuffer, 1);
			
			previousPressed = true;
		} else if ((!currentPressed) && previousPressed)
		{
			outputBuffer[0] = 0;
			CDC_Transmit_FS(outputBuffer, 1);
			previousPressed = false;
		}
    osDelay(10);
  }
  /* USER CODE END buttonReport_entry */
}

/* USER CODE BEGIN Header_userControl_entry */
/**
* @brief Function implementing the userControl thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_userControl_entry */
__weak void userControl_entry(void *argument)
{
	
	for(;;){osDelay(1);} // This disables the following code.
	
  /* USER CODE BEGIN userControl_entry */
  /* Infinite loop */
	GPIO_TypeDef* ports[] = {GPIOG, GPIOG, GPIOG, GPIOG, GPIOG, GPIOG, GPIOG, GPIOG, GPIOF, GPIOE};
	uint16_t pins[] = {LED_PG1_Pin, LED_PG2_Pin, LED_PG3_Pin, LED_PG4_Pin, LED_PG5_Pin, LED_PG6_Pin, LED_PG7_Pin,
	                   LED_PG8_Pin, LED_PF14_Pin, LED_PE11_Pin};
  
	
	//HAL_GPIO_WritePin(GPIOF, LED_PF14_Pin, GPIO_PIN_SET);
	//HAL_GPIO_WritePin(GPIOE, LED_PE11_Pin, GPIO_PIN_SET);
	osDelay(1000);
  for(;;)
  {
		for (char i=0; i<10; i++)
		{
			HAL_GPIO_TogglePin(ports[i], pins[i]);
			//HAL_GPIO_WritePin(ports[i], pins[i], GPIO_PIN_SET);
			osDelay(1000);
		}
    //osDelay(1);
  }
  /* USER CODE END userControl_entry */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
