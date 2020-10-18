/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED_PG8_Pin GPIO_PIN_8
#define LED_PG8_GPIO_Port GPIOG
#define LED_PG7_Pin GPIO_PIN_7
#define LED_PG7_GPIO_Port GPIOG
#define LED_PG6_Pin GPIO_PIN_6
#define LED_PG6_GPIO_Port GPIOG
#define LED_PG5_Pin GPIO_PIN_5
#define LED_PG5_GPIO_Port GPIOG
#define LED_PG4_Pin GPIO_PIN_4
#define LED_PG4_GPIO_Port GPIOG
#define LED_PG3_Pin GPIO_PIN_3
#define LED_PG3_GPIO_Port GPIOG
#define LED_PG2_Pin GPIO_PIN_2
#define LED_PG2_GPIO_Port GPIOG
#define Button_PB2_Pin GPIO_PIN_2
#define Button_PB2_GPIO_Port GPIOB
#define LED_PG1_Pin GPIO_PIN_1
#define LED_PG1_GPIO_Port GPIOG
#define LED_PE11_Pin GPIO_PIN_11
#define LED_PE11_GPIO_Port GPIOE
#define LED_PF14_Pin GPIO_PIN_14
#define LED_PF14_GPIO_Port GPIOF
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
