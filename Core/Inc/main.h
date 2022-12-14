/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
extern volatile uint32_t tempCode;
extern volatile uint8_t bitIndex;
/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define IO1_Pin GPIO_PIN_6
#define IO1_GPIO_Port GPIOA
#define IO2_Pin GPIO_PIN_7
#define IO2_GPIO_Port GPIOA
#define BTN_1_Pin GPIO_PIN_0
#define BTN_1_GPIO_Port GPIOB
#define BTN_2_Pin GPIO_PIN_1
#define BTN_2_GPIO_Port GPIOB
#define CLK_1_Pin GPIO_PIN_12
#define CLK_1_GPIO_Port GPIOB
#define DIO_1_Pin GPIO_PIN_13
#define DIO_1_GPIO_Port GPIOB
#define CLK_2_Pin GPIO_PIN_14
#define CLK_2_GPIO_Port GPIOB
#define DIO_2_Pin GPIO_PIN_15
#define DIO_2_GPIO_Port GPIOB
#define IR_SIGNAL_Pin GPIO_PIN_11
#define IR_SIGNAL_GPIO_Port GPIOA
#define IR_SIGNAL_EXTI_IRQn EXTI15_10_IRQn
#define LED_Y_Pin GPIO_PIN_15
#define LED_Y_GPIO_Port GPIOA
#define LED_R_Pin GPIO_PIN_3
#define LED_R_GPIO_Port GPIOB
#define LED_G_Pin GPIO_PIN_4
#define LED_G_GPIO_Port GPIOB
#define BTN_3_Pin GPIO_PIN_5
#define BTN_3_GPIO_Port GPIOB
#define BTN_4_Pin GPIO_PIN_6
#define BTN_4_GPIO_Port GPIOB
#define BTN_5_Pin GPIO_PIN_7
#define BTN_5_GPIO_Port GPIOB
#define BUZZER_Pin GPIO_PIN_8
#define BUZZER_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
