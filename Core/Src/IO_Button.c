/*
 * IO_Button.c
 *
 *  Created on: Dec 1, 2022
 *      Author: user
 */
/* Includes ------------------------------------------------------------------*/
#include "IO_Button.h"

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* CYCLE 10 ms */
#define ANTI_NOISE_PRESS		5	/* 2 cycle of Button Process 20ms */
#define ANTI_NOISE_RELEASE		5	/* 2 cycle of Button Process 20ms */
#define TIME_SINGLE_PRESS		20	/* 10 cycle of Button Process 100ms */
#define TIME_LONG_PRESS_T1		90	/* 90 cycle of Button Process 900s */
#define TIME_LONG_PRESS_T2 		250	/* 250 cycle of Button Process ~ 2500ms = 2.5s */
#define TIME_HOLD_ON 			600	/* 600 cycle of Button Process ~ 6000ms = 6s */
#define TIME_HOLD_OFF 			10	/* 10 cycle of Button Process ~ 100ms */
#define TIME_HOLD_OFF_LONG_T1	100	/* 100 cycle of Button Process ~ 1s */
#define TIME_HOLD_OFF_LONG_T2	200	/* 200 cycle of Button Process ~ 2s */
#define TIME_HOLD_OFF_LONG		400	/* 400 cycle of Button Process ~ 4s */

#define COUNT_TIME_SAMPLE		40
/* USER CODE END PM */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
structIO_Button strIO_Button_Value;
structIO_Button strOld_Button_Value;

static uint32_t	uTimeCheck[NUMBER_IO_BUTTON_USE];
static uint32_t	uTimePress[NUMBER_IO_BUTTON_USE];
static uint8_t	bClickCount[NUMBER_IO_BUTTON_USE];
static uint32_t	bHoldOff[NUMBER_IO_BUTTON_USE];
static uint8_t 	flag_start_sample[NUMBER_IO_BUTTON_USE];
/* USER CODE END PFP */


/* Public function -----------------------------------------------*/
void vButtonIO_Init(void)
{
	strIO_Button_Value.bFlagNewButton = 0;

	for(int i=0;i<(int)NUMBER_IO_BUTTON_USE;i++){
		strOld_Button_Value.bButtonState[i] = eButtonRelease;
		strIO_Button_Value.bButtonState[i] = eButtonRelease;
	}
}

void vGetIOButtonValue(eIndexButton bIndex, GPIO_PinState InputState, structIO_Button *OldValue, structIO_Button *NewValue)
{
/* Start get new state */
	/* GPIO_PIN_SET button press, GPIO_PIN_RESET button release*/
	if(InputState == GPIO_PIN_RESET)
	{
		/* Base on the last state, caculate to change state */
		switch(OldValue->bButtonState[bIndex])
		{
			case eButtonPress:
				if(bHoldOff[bIndex]>=(int)ANTI_NOISE_RELEASE)
					NewValue->bButtonState[bIndex] = eButtonRelease;
				break;
			case eButtonSingleClick:
				if(bHoldOff[bIndex]>=(int)ANTI_NOISE_RELEASE)
					NewValue->bButtonState[bIndex] = eButtonRelease;
				break;
			case eButtonDoubleClick:
				if(bHoldOff[bIndex]>=(int)ANTI_NOISE_RELEASE)
					NewValue->bButtonState[bIndex] = eButtonRelease;
				break;
			case eButtonTripleClick:
				if(bHoldOff[bIndex]>=(int)ANTI_NOISE_RELEASE)
					NewValue->bButtonState[bIndex] = eButtonRelease;
				break;
			case eButtonLongPressT1:
				if(bHoldOff[bIndex]>=(int)ANTI_NOISE_RELEASE)
					NewValue->bButtonState[bIndex] = eButtonRelease;
				break;
			case eButtonLongPressT2:
				if(bHoldOff[bIndex]>=(int)ANTI_NOISE_RELEASE)
					NewValue->bButtonState[bIndex] = eButtonRelease;
				break;
			case eButtonHoldOn:
				if(bHoldOff[bIndex]>=(int)ANTI_NOISE_RELEASE)
					NewValue->bButtonState[bIndex] = eButtonRelease;
				break;
			case eButtonRelease:
				if(bHoldOff[bIndex]>=(int)TIME_HOLD_OFF)
					NewValue->bButtonState[bIndex] = eButtonHoldOff;
				break;
			case eButtonHoldOff:
				if(bHoldOff[bIndex]>=(int)TIME_HOLD_OFF_LONG_T1)
					NewValue->bButtonState[bIndex] = eButtonHoldOffLongT1;
				break;
			case eButtonHoldOffLongT1:
				if(bHoldOff[bIndex]>=(int)TIME_HOLD_OFF_LONG_T2)
					NewValue->bButtonState[bIndex] = eButtonHoldOffLongT2;
				break;
			case eButtonHoldOffLongT2:
				if(bHoldOff[bIndex]>=(int)TIME_HOLD_OFF_LONG)
					NewValue->bButtonState[bIndex] = eButtonHoldOffLong;
				break;
			case eButtonHoldOffLong:
				break;
		}
		/* Increase time Hold Off */
		if(bHoldOff[bIndex]<(int)TIME_HOLD_OFF_LONG){bHoldOff[bIndex]++;}
		/* Reset Time counter */
		uTimePress[bIndex] = 0;
		NewValue->bButtonTime[bIndex] = 0;
	}
	/* Button press */
	else if(InputState == GPIO_PIN_SET)
	{
		/* Base on the last state, caculate to change state */
		switch(OldValue->bButtonState[bIndex])
		{
			case eButtonRelease:
				bHoldOff[bIndex]=0;
				if(uTimeCheck[bIndex]>=(int)ANTI_NOISE_PRESS)
				{
					NewValue->bButtonState[bIndex] = eButtonPress;
					bClickCount[bIndex]++;
				}
				flag_start_sample[bIndex] = 1;
				break;
			case eButtonHoldOff:
	//			bHoldOff[bIndex]=0;
	//			if(uTimeCheck[bIndex]>=ANTI_NOISE_PRESS)
	//			{
	//				NewValue->bButtonState[bIndex] = eButtonPress;
	//				bClickCount[bIndex]++;
	//			}
	//			flag_start_sample[bIndex] = 1;
				NewValue->bButtonState[bIndex] = eButtonRelease;
				break;
			case eButtonHoldOffLongT1:
				NewValue->bButtonState[bIndex] = eButtonRelease;
				break;
			case eButtonHoldOffLongT2:
				NewValue->bButtonState[bIndex] = eButtonRelease;
				break;
			case eButtonHoldOffLong:
				NewValue->bButtonState[bIndex] = eButtonRelease;
				break;
			}
		/* Increase counter */
		uTimePress[bIndex]++;
	}

	/* Time End period */
	if(flag_start_sample[bIndex]==1)
		uTimeCheck[bIndex]++;
	if(uTimeCheck[bIndex]>=(int)COUNT_TIME_SAMPLE)/* 10*20=200ms */
	{
		if((bClickCount[bIndex]==1)&&(uTimePress[bIndex]<=(int)TIME_SINGLE_PRESS))
			NewValue->bButtonState[bIndex] = eButtonSingleClick;
		if(bClickCount[bIndex]==2)
			NewValue->bButtonState[bIndex] = eButtonDoubleClick;
		if(bClickCount[bIndex]==3)
			NewValue->bButtonState[bIndex] = eButtonTripleClick;
		/* Reset counter */
		bClickCount[bIndex] = 0;
		/* Reset */
		uTimeCheck[bIndex] = 0;
		/* Set flag sample */
		flag_start_sample[bIndex] = 0;
	}
	/* Change mode */
	if(uTimePress[bIndex]>=(int)TIME_HOLD_ON)
		NewValue->bButtonState[bIndex] = eButtonHoldOn;
	else if(uTimePress[bIndex]>=TIME_LONG_PRESS_T2)
		NewValue->bButtonState[bIndex] = eButtonLongPressT2;
	else if(uTimePress[bIndex]>=TIME_LONG_PRESS_T1)
		NewValue->bButtonState[bIndex] = eButtonLongPressT1;
	/* Update button press time */
	NewValue->bButtonTime[bIndex] = uTimePress[bIndex];
}
