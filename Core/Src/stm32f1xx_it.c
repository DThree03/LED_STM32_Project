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
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define TASK1 100
#define TASK2 100
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
static volatile uint32_t tempCode;
static volatile uint8_t bitIndex;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern TIM_HandleTypeDef htim2;
extern UART_HandleTypeDef huart3;
/* USER CODE BEGIN EV */
extern TIM_HandleTypeDef htim1;
extern structIO_Button strIO_Button_Value;
extern structIO_Button strOld_Button_Value;

extern volatile uint16_t sys_millis;
extern volatile uint8_t bFlagDelay;
extern volatile uint32_t uCountDelay;

extern volatile uint8_t Task1_Flag;
extern volatile uint8_t Task2_Flag;

extern volatile uint32_t IRcode;
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

/**
  * @brief This function handles EXTI line[15:10] interrupts.
  */
void EXTI15_10_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI15_10_IRQn 0 */
	if(HAL_GPIO_ReadPin(IR_SIGNAL_GPIO_Port, IR_SIGNAL_Pin) == GPIO_PIN_RESET)
	{
		__HAL_TIM_SET_COUNTER(&htim1, 0);
	}
	else
	{
		if (__HAL_TIM_GET_COUNTER(&htim1) > 1000)
		{
			tempCode |= (1UL << (31-bitIndex));   // write 1
		}
		else
		{
			tempCode &= ~(1UL << (31-bitIndex));  // write 0
		}

		bitIndex++;
		if(bitIndex == 24)
		{
			IRcode = tempCode >> 8; // Second last 8 bits

			 //Do your main work HERE
			printf("IR Code 0x%x\n", (int)IRcode);

			tempCode = 0;
			bitIndex = 0;
		}
	}
//	if (__HAL_TIM_GET_COUNTER(&htim1) > 8000)
//	{
//		tempCode = 0;
//		bitIndex = 0;
//	}
//	else if (__HAL_TIM_GET_COUNTER(&htim1) > 1700)
//	{
//		tempCode |= (1UL << (31-bitIndex));   // write 1
//		bitIndex++;
//	}
//	else if (__HAL_TIM_GET_COUNTER(&htim1) > 1000)
//	{
//		tempCode &= ~(1UL << (31-bitIndex));  // write 0
//		bitIndex++;
//	}
//
//	if(bitIndex == 32)
//	{
//		cmdli = ~tempCode; // Logical inverted last 8 bits
//		cmd = tempCode >> 8; // Second last 8 bits
//		if(cmdli == cmd) // Check for errors
//		{
//			IRcode = tempCode; // If no bit errors
			// Do your main work HERE

//			switch (code)
//			{
//				case (16753245):
//				SSD1306_Puts ("CH-", &Font_16x26, 1);
//				break;
//
//				case (16736925):
//				SSD1306_Puts ("CH", &Font_16x26, 1);
//				break;
//
//				case (16769565):
//				SSD1306_Puts ("CH+", &Font_16x26, 1);
//				break;
//
//				default :
//				break;
//			}
//		}
//		bitIndex = 0;
//	}
//	bitIndex++;
//	__HAL_TIM_SET_COUNTER(&htim1, 0);

  /* USER CODE END EXTI15_10_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(IR_SIGNAL_Pin);
  /* USER CODE BEGIN EXTI15_10_IRQn 1 */

  /* USER CODE END EXTI15_10_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
