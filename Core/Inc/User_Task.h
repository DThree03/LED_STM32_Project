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

/* Public function prototypes ------------------------------------------------*/
void Task_Read_Cfg(void);

void Task_Save_Cfg(void);

void Task_Mode_Cfg(void);

uint8_t Task_User_1stInit(uint8_t readFlash);

uint8_t Task_Round_Init(void);

uint8_t Task_Playing(void);

void Task_TestMode_Display(uint8_t global_num);

void Task_Run_TestMode(void);

void Task_Playing_Time(void);

void Task_100ms(void);

void Task_Clear_Display(uint8_t addr);

void Task_led_xl(uint8_t addr, uint8_t state);

void Task_Blink_Line(uint8_t addr, uint8_t state, uint8_t line);

void Task_Led_StartPoint(uint8_t tram, uint8_t chuc, uint8_t donvi);

void Task_Buzzer_Enable(void);

#endif /* INC_USER_TASK_H_ */
