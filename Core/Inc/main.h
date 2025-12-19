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
#include "stm32l4xx_hal.h"

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
#define B1_Pin GPIO_PIN_13
#define B1_GPIO_Port GPIOC
#define TEMP_IN_Pin GPIO_PIN_0
#define TEMP_IN_GPIO_Port GPIOC
#define EPD_CS_Pin GPIO_PIN_1
#define EPD_CS_GPIO_Port GPIOC
#define LUMI_IN_Pin GPIO_PIN_2
#define LUMI_IN_GPIO_Port GPIOC
#define EPD_DATA_Pin GPIO_PIN_3
#define EPD_DATA_GPIO_Port GPIOC
#define GPS_TX_Pin GPIO_PIN_0
#define GPS_TX_GPIO_Port GPIOA
#define GPS_RX_Pin GPIO_PIN_1
#define GPS_RX_GPIO_Port GPIOA
#define USART_TX_Pin GPIO_PIN_2
#define USART_TX_GPIO_Port GPIOA
#define USART_RX_Pin GPIO_PIN_3
#define USART_RX_GPIO_Port GPIOA
#define EPD_RST_Pin GPIO_PIN_4
#define EPD_RST_GPIO_Port GPIOA
#define LD2_Pin GPIO_PIN_5
#define LD2_GPIO_Port GPIOA
#define EPD_DC_Pin GPIO_PIN_0
#define EPD_DC_GPIO_Port GPIOB
#define SRCLK_Pin GPIO_PIN_1
#define SRCLK_GPIO_Port GPIOB
#define NSRCLR_Pin GPIO_PIN_2
#define NSRCLR_GPIO_Port GPIOB
#define EPDA_SCK_Pin GPIO_PIN_10
#define EPDA_SCK_GPIO_Port GPIOB
#define SER_Pin GPIO_PIN_13
#define SER_GPIO_Port GPIOB
#define NOE_Pin GPIO_PIN_14
#define NOE_GPIO_Port GPIOB
#define RCLK_Pin GPIO_PIN_15
#define RCLK_GPIO_Port GPIOB
#define DHT_INT_Pin GPIO_PIN_6
#define DHT_INT_GPIO_Port GPIOC
#define DHT_INT_EXTI_IRQn EXTI9_5_IRQn
#define DHT_DATA_Pin GPIO_PIN_8
#define DHT_DATA_GPIO_Port GPIOC
#define DEBUG_Pin GPIO_PIN_9
#define DEBUG_GPIO_Port GPIOC
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define WIFI_ENABLE_Pin GPIO_PIN_10
#define WIFI_ENABLE_GPIO_Port GPIOC
#define WIFI_RESET_Pin GPIO_PIN_11
#define WIFI_RESET_GPIO_Port GPIOC
#define WIFI_TX_Pin GPIO_PIN_12
#define WIFI_TX_GPIO_Port GPIOC
#define WIFI_RX_Pin GPIO_PIN_2
#define WIFI_RX_GPIO_Port GPIOD
#define SWO_Pin GPIO_PIN_3
#define SWO_GPIO_Port GPIOB
#define EPD_BUSY_Pin GPIO_PIN_4
#define EPD_BUSY_GPIO_Port GPIOB
#define PAGESELECT_Pin GPIO_PIN_5
#define PAGESELECT_GPIO_Port GPIOB
#define PAGESELECT_EXTI_IRQn EXTI9_5_IRQn

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
