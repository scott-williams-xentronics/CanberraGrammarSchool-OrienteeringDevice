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
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
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
#include "stm32l0xx_hal.h"

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
#define SWITCH_Pin GPIO_PIN_14
#define SWITCH_GPIO_Port GPIOC
#define HEARTBEAT_Pin GPIO_PIN_15
#define HEARTBEAT_GPIO_Port GPIOC
#define HEARTBEAT_EXTI_IRQn EXTI4_15_IRQn
#define RED_RX_Pin GPIO_PIN_0
#define RED_RX_GPIO_Port GPIOA
#define RED_TX_Pin GPIO_PIN_1
#define RED_TX_GPIO_Port GPIOA
#define BLUE_RX_Pin GPIO_PIN_2
#define BLUE_RX_GPIO_Port GPIOA
#define BLUE_TX_Pin GPIO_PIN_3
#define BLUE_TX_GPIO_Port GPIOA
#define RSSI_3_ED_Pin GPIO_PIN_4
#define RSSI_3_ED_GPIO_Port GPIOA
#define RSSI_2_LED_Pin GPIO_PIN_5
#define RSSI_2_LED_GPIO_Port GPIOA
#define RSSI_1_LED_Pin GPIO_PIN_6
#define RSSI_1_LED_GPIO_Port GPIOA
#define TX_LED_Pin GPIO_PIN_7
#define TX_LED_GPIO_Port GPIOA
#define MASTER_LED_Pin GPIO_PIN_0
#define MASTER_LED_GPIO_Port GPIOB
#define STATUS_LED_Pin GPIO_PIN_1
#define STATUS_LED_GPIO_Port GPIOB
#define XBEE_RESETn_Pin GPIO_PIN_8
#define XBEE_RESETn_GPIO_Port GPIOA
#define XBEE_RX_Pin GPIO_PIN_9
#define XBEE_RX_GPIO_Port GPIOA
#define XBEE_TX_Pin GPIO_PIN_10
#define XBEE_TX_GPIO_Port GPIOA
#define BAT_LOW_Pin GPIO_PIN_15
#define BAT_LOW_GPIO_Port GPIOA
#define PC_RX_Pin GPIO_PIN_3
#define PC_RX_GPIO_Port GPIOB
#define PC_TX_Pin GPIO_PIN_4
#define PC_TX_GPIO_Port GPIOB
#define BUZZER_Pin GPIO_PIN_5
#define BUZZER_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
