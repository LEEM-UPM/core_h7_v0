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
#define THYONE_RX_Pin GPIO_PIN_6
#define THYONE_RX_GPIO_Port GPIOF
#define THYONE_TX_Pin GPIO_PIN_7
#define THYONE_TX_GPIO_Port GPIOF
#define THYONE_RTS_Pin GPIO_PIN_8
#define THYONE_RTS_GPIO_Port GPIOF
#define THYONE_CTS_Pin GPIO_PIN_9
#define THYONE_CTS_GPIO_Port GPIOF
#define OSC_IN_Pin GPIO_PIN_0
#define OSC_IN_GPIO_Port GPIOH
#define OSC_OUT_Pin GPIO_PIN_1
#define OSC_OUT_GPIO_Port GPIOH
#define CAM_EN_Pin GPIO_PIN_2
#define CAM_EN_GPIO_Port GPIOA
#define CAM3_RX_Pin GPIO_PIN_12
#define CAM3_RX_GPIO_Port GPIOB
#define CAM3_TX_Pin GPIO_PIN_13
#define CAM3_TX_GPIO_Port GPIOB
#define LED_1_Pin GPIO_PIN_11
#define LED_1_GPIO_Port GPIOD
#define LED_PWM_Pin GPIO_PIN_12
#define LED_PWM_GPIO_Port GPIOD
#define CAM2_RX_Pin GPIO_PIN_14
#define CAM2_RX_GPIO_Port GPIOD
#define CAM2_TX_Pin GPIO_PIN_15
#define CAM2_TX_GPIO_Port GPIOD
#define BUTTON2_Pin GPIO_PIN_3
#define BUTTON2_GPIO_Port GPIOG
#define BUTTON1_Pin GPIO_PIN_4
#define BUTTON1_GPIO_Port GPIOG
#define SD1_CD_Pin GPIO_PIN_7
#define SD1_CD_GPIO_Port GPIOC
#define SD1_DATA0_Pin GPIO_PIN_8
#define SD1_DATA0_GPIO_Port GPIOC
#define SD1_DATA1_Pin GPIO_PIN_9
#define SD1_DATA1_GPIO_Port GPIOC
#define SD2_CD_Pin GPIO_PIN_8
#define SD2_CD_GPIO_Port GPIOA
#define SD1_DATA2_Pin GPIO_PIN_10
#define SD1_DATA2_GPIO_Port GPIOC
#define SD1_DATA3_Pin GPIO_PIN_11
#define SD1_DATA3_GPIO_Port GPIOC
#define SD1_CLK_Pin GPIO_PIN_12
#define SD1_CLK_GPIO_Port GPIOC
#define SD1_CMD_Pin GPIO_PIN_2
#define SD1_CMD_GPIO_Port GPIOD
#define CAN2_STB_Pin GPIO_PIN_3
#define CAN2_STB_GPIO_Port GPIOD
#define CAN1_STB_Pin GPIO_PIN_4
#define CAN1_STB_GPIO_Port GPIOD
#define FDCAN1_RX_Pin GPIO_PIN_8
#define FDCAN1_RX_GPIO_Port GPIOB
#define FDCAN1_TX_Pin GPIO_PIN_9
#define FDCAN1_TX_GPIO_Port GPIOB
#define CAM1_RX_Pin GPIO_PIN_0
#define CAM1_RX_GPIO_Port GPIOE
#define CAM1_TX_Pin GPIO_PIN_1
#define CAM1_TX_GPIO_Port GPIOE

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
