/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32f1xx_it.c
  * @brief   Interrupt Service Routines.
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "IO_Button.h"
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define TASK1 10
#define TASK2 1000
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern TIM_HandleTypeDef htim2;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart3;
/* USER CODE BEGIN EV */

extern structIO_Button strIO_Button_Value;
extern structIO_Button strOld_Button_Value;

extern volatile uint16_t sys_millis;
extern volatile uint8_t bFlagDelay;
extern volatile uint32_t uCountDelay;

extern volatile uint8_t Task1_Flag;
extern volatile uint8_t Task2_Flag;
/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M3 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */
	sys_millis++;
	if(bFlagDelay==1)
		uCountDelay++;
	else
		uCountDelay=0;

	if(sys_millis==10000)	sys_millis = 0;
	if(sys_millis%TASK1 == 0)	Task1_Flag = 1;
	if(sys_millis%TASK2 == 0)	Task2_Flag = 1;
  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32F1xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f1xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles TIM2 global interrupt.
  */
void TIM2_IRQHandler(void)
{
  /* USER CODE BEGIN TIM2_IRQn 0 */
	vGetIOButtonValue(eButton1, HAL_GPIO_ReadPin(BTN_1_GPIO_Port, BTN_1_Pin), &strOld_Button_Value, &strIO_Button_Value);
	vGetIOButtonValue(eButton2, HAL_GPIO_ReadPin(BTN_2_GPIO_Port, BTN_2_Pin), &strOld_Button_Value, &strIO_Button_Value);
	vGetIOButtonValue(eButton3, HAL_GPIO_ReadPin(BTN_3_GPIO_Port, BTN_3_Pin), &strOld_Button_Value, &strIO_Button_Value);
	vGetIOButtonValue(eButton4, HAL_GPIO_ReadPin(BTN_4_GPIO_Port, BTN_4_Pin), &strOld_Button_Value, &strIO_Button_Value);
	vGetIOButtonValue(eButton5, HAL_GPIO_ReadPin(BTN_5_GPIO_Port, BTN_5_Pin), &strOld_Button_Value, &strIO_Button_Value);
	if(memcmp(strOld_Button_Value.bButtonState, strIO_Button_Value.bButtonState, NUMBER_IO_BUTTON_USE))
	{
		memcpy(strOld_Button_Value.bButtonState, strIO_Button_Value.bButtonState, NUMBER_IO_BUTTON_USE);
		strIO_Button_Value.bFlagNewButton = 1;
	}
	else{
		strIO_Button_Value.bFlagNewButton = 0;
	}

  /* USER CODE END TIM2_IRQn 0 */
  HAL_TIM_IRQHandler(&htim2);
  /* USER CODE BEGIN TIM2_IRQn 1 */

  /* USER CODE END TIM2_IRQn 1 */
}

/**
  * @brief This function handles USART1 global interrupt.
  */
void USART1_IRQHandler(void)
{
  /* USER CODE BEGIN USART1_IRQn 0 */

  /* USER CODE END USART1_IRQn 0 */
  HAL_UART_IRQHandler(&huart1);
  /* USER CODE BEGIN USART1_IRQn 1 */

  /* USER CODE END USART1_IRQn 1 */
}

/**
  * @brief This function handles USART3 global interrupt.
  */
void USART3_IRQHandler(void)
{
  /* USER CODE BEGIN USART3_IRQn 0 */

  /* USER CODE END USART3_IRQn 0 */
  HAL_UART_IRQHandler(&huart3);
  /* USER CODE BEGIN USART3_IRQn 1 */

  /* USER CODE END USART3_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
