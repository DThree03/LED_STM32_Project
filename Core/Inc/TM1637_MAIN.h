/*
 * TM1637.h
 *
 *  Created on: Dec 4, 2022
 *      Author: user
 */

#ifndef INC_TM1637_MAIN_H_
#define INC_TM1637_MAIN_H_

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* Exported macro ------------------------------------------------------------*/
#define TM1637_I2C_COMM1    0x40

#define TM1637_1_AllOff 10
#define TM1637_1_AllOn 11
#define TM1637_1_Xen_ke 12

#define PWM1 	0x80
#define PWM2 	0x81
#define PWM3 	0x82
#define PWM4 	0x83
#define PWM5 	0x84
#define PWM6 	0x85
#define PWM7 	0x86
#define PWM8 	0x87
#define Dis_ON  0x88
#define Dis_OFF 0x80
/* Exported typedef -----------------------------------------------------------*/

void Led7TurnTime_Display(int led7ch, int led7dv, int led_num_left, int led_num_right);

void Led7HitCnt_Display(int slc_ch, int slc_dv, int sct_ch, int sct_dv);

void Led7RoundTime_Display(int h_ch, int h_dv, int m_ch, int m_dv);

#endif /* INC_TM1637_MAIN_H_ */
