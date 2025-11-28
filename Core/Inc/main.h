/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
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
#include "stm32h7xx_hal.h"

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
#define ALARM_Pin GPIO_PIN_4
#define ALARM_GPIO_Port GPIOE
#define CAM_EN_Pin GPIO_PIN_2
#define CAM_EN_GPIO_Port GPIOA
#define LED_1_Pin GPIO_PIN_11
#define LED_1_GPIO_Port GPIOD
#define LED_PWM_Pin GPIO_PIN_12
#define LED_PWM_GPIO_Port GPIOD
#define BUTTON2_Pin GPIO_PIN_3
#define BUTTON2_GPIO_Port GPIOG
#define BUTTON1_Pin GPIO_PIN_4
#define BUTTON1_GPIO_Port GPIOG
#define SD2_CD_Pin GPIO_PIN_7
#define SD2_CD_GPIO_Port GPIOC
#define SD1_CD_Pin GPIO_PIN_8
#define SD1_CD_GPIO_Port GPIOA
#define CAN2_STB_Pin GPIO_PIN_3
#define CAN2_STB_GPIO_Port GPIOD
#define CAN1_STB_Pin GPIO_PIN_4
#define CAN1_STB_GPIO_Port GPIOD

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
