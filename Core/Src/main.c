/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>

#include "User_Task.h"
#include "TM1637_MAIN.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef enum{
	E_STATE_STARTUP,
	E_STATE_POWER_ON,
	E_STATE_PLAYING,
	E_STATE_CFG_MODE,
	E_STATE_RUN_MODE
}eUSER_TASK_STATE;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;

UART_HandleTypeDef huart3;

/* USER CODE BEGIN PV */

static eUSER_TASK_STATE eUserTask_State = E_STATE_STARTUP;
static uint8_t Playing_Stt;
static uint16_t button_press_cnt = 0;

volatile uint32_t tempCode;
volatile uint8_t bitIndex;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM1_Init(void);

extern volatile uint32_t IRcode;
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif

PUTCHAR_PROTOTYPE
{
  HAL_UART_Transmit(&huart3, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
  return ch;
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART3_UART_Init();
  MX_TIM2_Init();
  MX_TIM1_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start(&htim1);
  __HAL_TIM_SET_COUNTER(&htim1, 0);

  HAL_TIM_Base_Start_IT(&htim2);
  //HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_SET);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  Task_User_1stInit(1);
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	 switch(eUserTask_State)
	 {
		 case E_STATE_STARTUP:
		 {
			 HAL_GPIO_TogglePin(LED_Y_GPIO_Port, LED_Y_Pin);

			 Led7TurnTime_Display(10, 10, 0, 0);
			 Led7HitCnt_Display(10, 10, 10, 10);
			 Led7RoundTime_Display(10, 10, 10, 10, 0);

			 Task_Blink_Line(0, 0, 1);
			 Task_Led_StartPoint(12, 12, 12);
			 Task_led_xl(0, 0x00);
			 Task_Clear_Display(0);
			 Task_Buzzer_Enable();

			 while(1)
			 {
				 //Task_100ms();
				 if(POWER_BUT_VAL == BUTTON_ACTIVE){
					 delay_ms(BUTTON_DELAY);
					 if(POWER_BUT_VAL == BUTTON_ACTIVE){
						 while(POWER_BUT_VAL == BUTTON_ACTIVE)
						 {
							 button_press_cnt++;
							 if(button_press_cnt > 20){
								 Led7TurnTime_Display(10, 10, 8*(button_press_cnt%2), 8*(button_press_cnt%2));
							 }
							 delay_ms(100);
						 }
						 if(button_press_cnt > 20){
							 Task_Buzzer_Enable();
							 eUserTask_State = E_STATE_POWER_ON;
						 }
						 button_press_cnt = 0;
					 }
				 }
				 else if(PLUS_BUT_VAL == BUTTON_ACTIVE){
					 delay_ms((int)BUTTON_DELAY);
					 if(PLUS_BUT_VAL ==BUTTON_ACTIVE){
						 while(PLUS_BUT_VAL == BUTTON_ACTIVE)
						 {
							 button_press_cnt++;
							 if(button_press_cnt > 20){
								 Led7TurnTime_Display(10, 10, 8*(button_press_cnt%2), 8*(button_press_cnt%2));
							 }
							 delay_ms(100);
						 }
						 if(button_press_cnt > 20){
							 eUserTask_State = E_STATE_RUN_MODE;
						 }
						 button_press_cnt = 0;
					 }
				 }
//				 else if(MINUS_BUT_VAL == BUTTON_ACTIVE){
//					 delay_ms(20);
//					 if(MINUS_BUT_VAL == BUTTON_ACTIVE){
//						 while(MINUS_BUT_VAL == BUTTON_ACTIVE)
//						 {
//							 button_press_cnt++;
//							 if(button_press_cnt > 30){
//								 Led7TurnTime_Display(10, 10, 8*(button_press_cnt%2), 8*(button_press_cnt%2));
//							 }
//							 delay_ms(100);
//						 }
//						 button_press_cnt = 0;
//					 }
//				 }
//				 else if(NEXT_BUT_VAL == BUTTON_ACTIVE){
//					 delay_ms(20);
//					 if(NEXT_BUT_VAL == BUTTON_ACTIVE){
//						 while(NEXT_BUT_VAL == BUTTON_ACTIVE)
//						 {
//							 button_press_cnt++;
//							 if(button_press_cnt > 30){
//								 Led7TurnTime_Display(10, 10, 8*(button_press_cnt%2), 8*(button_press_cnt%2));
//							 }
//							 delay_ms(100);
//						 }
//						 button_press_cnt = 0;
//					 }
//				 }
//				 else if(MODE_BUT_VAL == BUTTON_ACTIVE){
//					 delay_ms(20);
//					 if(MODE_BUT_VAL == BUTTON_ACTIVE){x
//						 while(MODE_BUT_VAL == BUTTON_ACTIVE)
//						 {
//							 button_press_cnt++;
//							 if(button_press_cnt > 30){
//								 Led7TurnTime_Display(10, 10, 8*(button_press_cnt%2), 8*(button_press_cnt%2));
//							 }
//							 delay_ms(100);
//						 }
//						 button_press_cnt = 0;
//					 }
//				 }

				 if(eUserTask_State!=E_STATE_STARTUP){
					 break;
				 }
			 }
			 break;
		 }

		 case E_STATE_POWER_ON:
		 {
			 //Send data ON LXL
			 Task_Round_Init();
			 HAL_GPIO_TogglePin(LED_Y_GPIO_Port, LED_Y_Pin);
			 while(1)
			 {
				 Task_100ms();
				 if(POWER_BUT_VAL == BUTTON_ACTIVE){
					 delay_ms((int)BUTTON_DELAY);
					 if (POWER_BUT_VAL == BUTTON_ACTIVE){
						 while (POWER_BUT_VAL == BUTTON_ACTIVE){
							 button_press_cnt++;
							 if(button_press_cnt > 20){
								 Led7TurnTime_Display(10, 10, 8*(button_press_cnt%2), 8*(button_press_cnt%2));
							 }
							 delay_ms(100);
						 }
						 if (button_press_cnt > 20){
							 eUserTask_State = E_STATE_STARTUP;
						 }
						 else {
							 Task_Buzzer_Enable();
							 eUserTask_State = E_STATE_PLAYING;
						 }
						 button_press_cnt = 0;
					 }
				 }
				 else if(MODE_BUT_VAL == BUTTON_ACTIVE){
					 delay_ms((int)BUTTON_DELAY);
					 if (MODE_BUT_VAL == BUTTON_ACTIVE){
						 while(MODE_BUT_VAL == BUTTON_ACTIVE)
						 {
							 button_press_cnt++;
							 if(button_press_cnt > 20){
								 Led7TurnTime_Display(10, 10, 8*(button_press_cnt%2), 8*(button_press_cnt%2));
							 }
							 delay_ms(100);
						 }
						 if(button_press_cnt > 20){
							 Task_Buzzer_Enable();
							 eUserTask_State = E_STATE_CFG_MODE;
						 }
						 button_press_cnt = 0;
					 }
				 }

				 if(eUserTask_State!=E_STATE_POWER_ON)
					 break;
			 }
			 break;
		 }

		 case E_STATE_PLAYING:
		 {
			 HAL_GPIO_TogglePin(LED_R_GPIO_Port, LED_R_Pin);

			 while(1)
			 {
				Playing_Stt = Task_Playing();
				if(Playing_Stt == 0x01)			//Next Rout
					eUserTask_State = E_STATE_POWER_ON;
				else if(Playing_Stt == 0xFF)	//Winner -> Reset
					Task_led_xl(0, 0x0F);

				Task_100ms();
				if(POWER_BUT_VAL == BUTTON_ACTIVE){
					delay_ms((int)BUTTON_DELAY);
					if (POWER_BUT_VAL == BUTTON_ACTIVE){
						while (POWER_BUT_VAL == BUTTON_ACTIVE){
							button_press_cnt++;
							if(button_press_cnt > 20){
								Led7TurnTime_Display(10, 10, 8*(button_press_cnt%2), 8*(button_press_cnt%2));
							}
							delay_ms(100);
						}
						if(button_press_cnt > 20){
							Task_User_1stInit(0);
							eUserTask_State = E_STATE_STARTUP;
						}
						button_press_cnt = 0;
					}
				}

				if(eUserTask_State!=E_STATE_PLAYING)
					break;
			 }
			 break;
		 }
		 case E_STATE_RUN_MODE:
		 {
			 Task_led_xl(0, 0x0F);
			 Task_TestMode_Display(0, IRcode);
			 while(1){
				if(Task_Run_TestMode()){
					eUserTask_State = E_STATE_STARTUP;
				}
				if(eUserTask_State!=E_STATE_RUN_MODE)
					break;
			 }
			 break;
		 }
		 case E_STATE_CFG_MODE:
		 {
			 Task_Read_Cfg();
			 Led7TurnTime_Display(10, 10, 0, 0);
			 Led7HitCnt_Display(10, 10, 10, 10);
			 Led7RoundTime_Display(10, 10, 10, 10, 0);

			 Task_Led_StartPoint(12, 12, 12);
			 Task_led_xl(0, 0x00);
			 Task_Clear_Display(0);
			 while(1)
			 {
				 Task_Mode_Cfg();
				 Task_100ms();
				 if(MODE_BUT_VAL == BUTTON_ACTIVE){
					 delay_ms((int)BUTTON_DELAY);
					 if (MODE_BUT_VAL == BUTTON_ACTIVE){
						 while(MODE_BUT_VAL == BUTTON_ACTIVE)
						 {
							 button_press_cnt++;
							 if(button_press_cnt > 20){
								 Led7TurnTime_Display(10, 10, 8*(button_press_cnt%2), 8*(button_press_cnt%2));
							 }
							 delay_ms(100);
						 }
						 if(button_press_cnt > 20){
							 Task_Buzzer_Enable();
							 Task_User_1stInit(0);
							Task_Save_Cfg();
							eUserTask_State = E_STATE_POWER_ON;
						 }
						 button_press_cnt = 0;
					 }
				 }
				 if(eUserTask_State!=E_STATE_CFG_MODE)
					break;
			 }
			 break;
		 }
	 }
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 7;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 65535;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 79;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 9999;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 9600;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, IO1_Pin|IO2_Pin|LED_Y_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, CLK_1_Pin|DIO_1_Pin|CLK_2_Pin|DIO_2_Pin
                          |LED_R_Pin|LED_G_Pin|BUZZER_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : IO1_Pin IO2_Pin LED_Y_Pin */
  GPIO_InitStruct.Pin = IO1_Pin|IO2_Pin|LED_Y_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : BTN_1_Pin BTN_2_Pin BTN_3_Pin BTN_4_Pin
                           BTN_5_Pin */
  GPIO_InitStruct.Pin = BTN_1_Pin|BTN_2_Pin|BTN_3_Pin|BTN_4_Pin
                          |BTN_5_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : CLK_1_Pin DIO_1_Pin CLK_2_Pin DIO_2_Pin
                           LED_R_Pin LED_G_Pin BUZZER_Pin */
  GPIO_InitStruct.Pin = CLK_1_Pin|DIO_1_Pin|CLK_2_Pin|DIO_2_Pin
                          |LED_R_Pin|LED_G_Pin|BUZZER_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : IR_SIGNAL_Pin */
  GPIO_InitStruct.Pin = IR_SIGNAL_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(IR_SIGNAL_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

/* USER CODE BEGIN 4 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if(GPIO_Pin == IR_SIGNAL_Pin) {
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
			//if((int)IRcode != (int)IR_LEDXL_CODE)
			 //Do your main work HERE
			//printf("IR Code 0x%x\n", (int)IRcode);

			tempCode = 0;
			bitIndex = 0;
		}
	}
  } else {
      __NOP();
  }
}
//void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
//{
//	vGetIOButtonValue(eButton1, HAL_GPIO_ReadPin(BTN_1_GPIO_Port, BTN_1_Pin), &strOld_Button_Value, &strIO_Button_Value);
//	vGetIOButtonValue(eButton2, HAL_GPIO_ReadPin(BTN_2_GPIO_Port, BTN_2_Pin), &strOld_Button_Value, &strIO_Button_Value);
//	vGetIOButtonValue(eButton3, HAL_GPIO_ReadPin(BTN_3_GPIO_Port, BTN_3_Pin), &strOld_Button_Value, &strIO_Button_Value);
//	vGetIOButtonValue(eButton4, HAL_GPIO_ReadPin(BTN_4_GPIO_Port, BTN_4_Pin), &strOld_Button_Value, &strIO_Button_Value);
//	vGetIOButtonValue(eButton5, HAL_GPIO_ReadPin(BTN_5_GPIO_Port, BTN_5_Pin), &strOld_Button_Value, &strIO_Button_Value);
//	if(memcmp(strOld_Button_Value.bButtonState, strIO_Button_Value.bButtonState, NUMBER_IO_BUTTON_USE))
//	{
//		memcpy(strOld_Button_Value.bButtonState, strIO_Button_Value.bButtonState, NUMBER_IO_BUTTON_USE);
//		strIO_Button_Value.bFlagNewButton = 1;
//	}
//	else{
//		strIO_Button_Value.bFlagNewButton = 0;
//	}
//}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
