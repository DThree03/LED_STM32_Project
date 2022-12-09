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
void Task_Mode_Cfg(void);

uint8_t Task_User_Init(uint8_t player_num);

void Task_Blink_Line(uint8_t addr, uint8_t state, uint8_t line);

void Task_4_Player(void);

void Task_Clear_Display(uint8_t addr);

void Task_500ms(void);

void Task_led_xl(uint8_t addr, uint8_t state);

#endif /* INC_USER_TASK_H_ */
