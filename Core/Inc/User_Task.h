/*
 * User_Task.h
 *
 *  Created on: Dec 2, 2022
 *      Author: user
 */

#ifndef INC_USER_TASK_H_
#define INC_USER_TASK_H_

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

#define POWER_BUT_VAL		HAL_GPIO_ReadPin(BTN_1_GPIO_Port, BTN_1_Pin)
#define MODE_BUT_VAL		HAL_GPIO_ReadPin(BTN_2_GPIO_Port, BTN_2_Pin)
#define MINUS_BUT_VAL		HAL_GPIO_ReadPin(BTN_3_GPIO_Port, BTN_3_Pin)
#define NEXT_BUT_VAL		HAL_GPIO_ReadPin(BTN_4_GPIO_Port, BTN_4_Pin)
#define PLUS_BUT_VAL		HAL_GPIO_ReadPin(BTN_5_GPIO_Port, BTN_5_Pin)

#define BUTTON_ACTIVE		GPIO_PIN_SET
#define BUTTON_DELAY		20

#define IR_LEDXL_CODE		0x908908
#define IR_NEXT_CODE		0x904904
#define IR_PUSH_CODE		0x910910
#define IR_STOP_CODE		0x920920

extern volatile uint32_t IRcode;
/* Public function prototypes ------------------------------------------------*/
void Task_Read_Cfg(void);

void Task_Save_Cfg(void);

void Task_Mode_Cfg(void);

uint8_t Task_User_1stInit(uint8_t readFlash);

uint8_t Task_Round_Init(void);

uint8_t Task_Playing(void);

void Task_TestMode_Display(uint8_t global_num, uint32_t irCODE);

uint8_t Task_Run_TestMode(void);

void Task_100ms(void);

void Task_Clear_Display(uint8_t addr);

void Task_led_xl(uint8_t addr, uint8_t state);

void Task_Blink_Line(uint8_t addr, uint8_t state, uint8_t line);

void Task_Led_StartPoint(uint8_t tram, uint8_t chuc, uint8_t donvi);

void Task_Buzzer_Enable(void);

void delay_ms(unsigned int x);

#endif /* INC_USER_TASK_H_ */
