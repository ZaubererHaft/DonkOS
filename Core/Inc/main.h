/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "stm32l4xx_hal.h"

void Error_Handler();

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define DISPLAY_DOUT_Pin GPIO_PIN_13
#define DISPLAY_DOUT_GPIO_Port GPIOC
#define DISPLAY_CS_Pin GPIO_PIN_14
#define DISPLAY_CS_GPIO_Port GPIOC
#define DISPLAY_CLK_Pin GPIO_PIN_15
#define DISPLAY_CLK_GPIO_Port GPIOC
#define KEYBOARD_COM_Pin GPIO_PIN_1
#define KEYBOARD_COM_GPIO_Port GPIOA
#define KEYBOARD_S0_Pin GPIO_PIN_5
#define KEYBOARD_S0_GPIO_Port GPIOA
#define KEYBOARD_S1_Pin GPIO_PIN_6
#define KEYBOARD_S1_GPIO_Port GPIOA
#define KEYBOARD_S2_Pin GPIO_PIN_7
#define KEYBOARD_S2_GPIO_Port GPIOA
#define LED_1_Pin GPIO_PIN_10
#define LED_1_GPIO_Port GPIOB
#define LED_2_Pin GPIO_PIN_15
#define LED_2_GPIO_Port GPIOB


#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
