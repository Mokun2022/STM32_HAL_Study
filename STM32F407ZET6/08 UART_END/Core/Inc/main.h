/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "stm32f4xx_hal.h"

#include "stm32f4xx_ll_usart.h"
#include "stm32f4xx_ll_rcc.h"
#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_cortex.h"
#include "stm32f4xx_ll_system.h"
#include "stm32f4xx_ll_utils.h"
#include "stm32f4xx_ll_pwr.h"
#include "stm32f4xx_ll_gpio.h"
#include "stm32f4xx_ll_dma.h"

#include "stm32f4xx_ll_exti.h"

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
#define IMU_EN_Pin GPIO_PIN_6
#define IMU_EN_GPIO_Port GPIOF
#define rtk_tx_Pin GPIO_PIN_2
#define rtk_tx_GPIO_Port GPIOA
#define rtk_rk_Pin GPIO_PIN_3
#define rtk_rk_GPIO_Port GPIOA
#define Beep_Pin GPIO_PIN_12
#define Beep_GPIO_Port GPIOB
#define debug_tx_Pin GPIO_PIN_8
#define debug_tx_GPIO_Port GPIOD
#define debug_rx_Pin GPIO_PIN_9
#define debug_rx_GPIO_Port GPIOD
#define bluetooth_tx_Pin GPIO_PIN_9
#define bluetooth_tx_GPIO_Port GPIOA
#define bluetooth_rx_Pin GPIO_PIN_10
#define bluetooth_rx_GPIO_Port GPIOA
#define cat1_tx_Pin GPIO_PIN_10
#define cat1_tx_GPIO_Port GPIOC
#define cat1_rx_Pin GPIO_PIN_11
#define cat1_rx_GPIO_Port GPIOC
#define Power5V_Pin GPIO_PIN_7
#define Power5V_GPIO_Port GPIOD
#define battery_rx_Pin GPIO_PIN_9
#define battery_rx_GPIO_Port GPIOG
#define battery_tx_Pin GPIO_PIN_14
#define battery_tx_GPIO_Port GPIOG

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */