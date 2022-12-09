/*
 * IO_Button.h
 *
 *  Created on: Dec 1, 2022
 *      Author: user
 */

#ifndef INC_IO_BUTTON_H_
#define INC_IO_BUTTON_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
#define NUMBER_IO_BUTTON_USE	5

#define BUT_NEW_STA_FLAG	strIO_Button_Value.bFlagNewButton

#define POWER_BUT_STATE		strIO_Button_Value.bButtonState[eButton1]
#define MODE_BUT_STATE		strIO_Button_Value.bButtonState[eButton2]
#define MINUS_BUT_STATE		strIO_Button_Value.bButtonState[eButton3]
#define NEXT_BUT_STATE		strIO_Button_Value.bButtonState[eButton4]
#define PLUS_BUT_STATE		strIO_Button_Value.bButtonState[eButton5]
/* USER CODE END EM */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
/* Enum button state */
typedef enum
{
    eButtonPress			= 1,
    eButtonRelease 			= 2,
	eButtonLongPressT1		= 3,
	eButtonLongPressT2		= 4,
	eButtonHoldOn			= 5,
	eButtonDoubleClick		= 6,
	eButtonSingleClick		= 7,
	eButtonTripleClick		= 8,
	eButtonHoldOff			= 9,
    eButtonHoldOffLongT1	= 10,
    eButtonHoldOffLongT2	= 11,
    eButtonHoldOffLong  	= 12,
	eButtonStateUN			= 0xff,
}eButtonState;

/* Enum button index */
typedef enum
{
    eButton1			= 0,
    eButton2 			= 1,
    eButton3			= 2,
	eButton4			= 3,
	eButton5			= 4,
	eButtonUN			= 0xff,
}eIndexButton;
/* Struct manage button */
typedef struct{
    eButtonState	bButtonState[NUMBER_IO_BUTTON_USE];/* Button state */
	uint16_t		bButtonTime[NUMBER_IO_BUTTON_USE];/* Time to hole */
	uint8_t 		bFlagNewButton;
}structIO_Button;
/* USER CODE END ET */

/* Public function prototypes -----------------------------------------------*/
void vButtonIO_Init(void);

void vGetIOButtonValue(eIndexButton bIndex, GPIO_PinState InputState, structIO_Button *OldValue, structIO_Button *NewValue);

#endif /* INC_IO_BUTTON_H_ */
