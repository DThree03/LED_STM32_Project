/*
 * User_Task.c
 *
 *  Created on: Dec 2, 2022
 *      Author: user
 */
/* Includes ------------------------------------------------------------------*/
#include <User_Task.h>
#include "main.h"
#include "IO_Button.h"
#include "TM1637_MAIN.h"
#include "FLASH_PAGE.h"

#include <stdio.h>
#include <string.h>
/* Private typedef -----------------------------------------------------------*/
typedef enum{
	E_CFG_START_POINT,
	E_CFG_ROUNDTIME_M,
	E_CFG_TURNTIME_S,
	E_CFG_START_MODE
}eUSER_CFG_STATE;

typedef union Struct_Flash_Cfg
{
	struct{
		uint8_t	 playing_mode;
		uint8_t  startup_time_m;
		uint16_t start_point;
		uint16_t turn_time_s;
		uint16_t rount_time_s;
		uint8_t	 mode_signed;
		uint8_t	 Reversed[3];
	}Parameter;
	uint32_t paraBuffer[3];
}Str_Cfg_Para;

typedef struct{
	uint8_t		ledxl_mask;
	uint8_t 	addr;
	uint8_t		sum_signed;
	float		average;
	uint16_t	point;
	uint16_t	max_hit_get_point;
	uint16_t	sum_point;
}player_data_t;

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define LAST_MODE		2
#define MODE_SIGN		1
#define BEGIN_POINT		30
#define TURN_TIME_S		45
#define ROUND_TIME_M	30
#define STARTUP_TIME_M	60

#define MAX_PLAYER_NUM	4

#define DATA_LED7_TYPE      1
#define UPDATE_LED7_TYPE    2
#define DATA_LEDXL_TYPE     3
#define UPDATE_BLINK_STATE  4

#define IR_PUSH_CODE	0x908908
#define IR_MINUS_CODE	0x904904
#define IR_NEXT_CODE	0x910910
#define IR_STOP_CODE	0x920920
/* USER CODE END PM */

/* Public variables ---------------------------------------------------------*/
extern structIO_Button strIO_Button_Value;

volatile uint16_t sys_millis = 0;
volatile uint8_t bFlagDelay = 0;
volatile uint32_t uCountDelay = 0;

volatile uint8_t Task1_Flag = 0;
volatile uint8_t Task2_Flag = 0;

volatile uint32_t IRcode = 0;
/* Private variables ---------------------------------------------------------*/
static uint8_t pTask_1st_Flag;
static uint8_t pCfg_1st_state_Flag = 1;
/*	Read from FLASH	 */
static Str_Cfg_Para PlayCfg;

static uint16_t turn_time_s;
static uint16_t rount_time_s;
/**********************************************/
static uint32_t all_turn_cnt;
static float	last_average;
static uint16_t hit_get_point_cnt;

static player_data_t Player[MAX_PLAYER_NUM];

static uint8_t current_player;
static uint8_t point_plus;

static uint16_t temp_s = 0;
static uint8_t stop_time;
static uint8_t buzzer_stt = 0xFF;
/* Private function prototypes -----------------------------------------------*/
static void Task_Upload_Display(void);
static void update_led7_data(uint8_t player_num);

static uint8_t get_player_available(void);
static uint8_t get_next_user(uint8_t current_play);
static void update_rand_addr(void);

static void delay_ms(unsigned int x);

/* Public function -----------------------------------------------------------*/
void Task_Read_Cfg(void)
{
	Flash_Read_Data(USER_INFO_FLASH_ADDR, PlayCfg.paraBuffer, 3);
	if(PlayCfg.Parameter.playing_mode == 0xFF)
	{
		PlayCfg.Parameter.start_point = BEGIN_POINT;
		PlayCfg.Parameter.turn_time_s = TURN_TIME_S;
		PlayCfg.Parameter.rount_time_s = ROUND_TIME_M;
		PlayCfg.Parameter.playing_mode = LAST_MODE;
		PlayCfg.Parameter.startup_time_m = STARTUP_TIME_M;
		PlayCfg.Parameter.mode_signed = MODE_SIGN;
		Flash_Write_Data(USER_INFO_FLASH_ADDR, PlayCfg.paraBuffer, 3);
	}
	pCfg_1st_state_Flag = 1;
}

void Task_Save_Cfg(void)
{
	Flash_Write_Data(USER_INFO_FLASH_ADDR, PlayCfg.paraBuffer, 3);
}

void Task_Mode_Cfg(void)
{
	static eUSER_CFG_STATE cfg_state_t = E_CFG_START_POINT;
	//static uint8_t pCfg_1st_state_Flag = 1;
	switch(cfg_state_t)
	{
		case E_CFG_START_POINT:
		{
			if(pCfg_1st_state_Flag){
				pCfg_1st_state_Flag = 0;
				//Send Display and Blink Start point
				Task_Led_StartPoint(PlayCfg.Parameter.start_point/100, (PlayCfg.Parameter.start_point%100)/10, PlayCfg.Parameter.start_point%10);
			}

			if((PLUS_BUT_STATE == eButtonSingleClick) || (IRcode == IR_PUSH_CODE))
			{
				PLUS_BUT_STATE = eButtonHoldOff;
				IRcode = 0;
				if(PlayCfg.Parameter.start_point<99)
					PlayCfg.Parameter.start_point++;
				//Update Display
				Task_Led_StartPoint(PlayCfg.Parameter.start_point/100, (PlayCfg.Parameter.start_point%100)/10, PlayCfg.Parameter.start_point%10);
				buzzer_stt = 1;
			}
			else if((MINUS_BUT_STATE == eButtonSingleClick) || (IRcode == IR_MINUS_CODE)){
				MINUS_BUT_STATE = eButtonHoldOff;
				IRcode = 0;
				if(PlayCfg.Parameter.start_point > 0)
					PlayCfg.Parameter.start_point--;
				//Update Display
				Task_Led_StartPoint(PlayCfg.Parameter.start_point/100, (PlayCfg.Parameter.start_point%100)/10, PlayCfg.Parameter.start_point%10);
				buzzer_stt = 1;
			}
			else if((NEXT_BUT_STATE == eButtonSingleClick) || (IRcode == IR_NEXT_CODE)){
				NEXT_BUT_STATE = eButtonHoldOff;
				IRcode = 0;
				cfg_state_t++;
				pCfg_1st_state_Flag = 1;
				//Clear old display and blink
				Task_Led_StartPoint(12, 12, 12);
				buzzer_stt = 1;
			}
			break;
		}
		case E_CFG_ROUNDTIME_M:
		{
			if(pCfg_1st_state_Flag){
				pCfg_1st_state_Flag = 0;

				//Send Display and Blink Start point
				Led7RoundTime_Display((PlayCfg.Parameter.rount_time_s/60)/10,
									  (PlayCfg.Parameter.rount_time_s/60)%10,
									  (PlayCfg.Parameter.rount_time_s%60)/10,
									  (PlayCfg.Parameter.rount_time_s%60)%10, 1);
			}
			//Check button
			if((PLUS_BUT_STATE == eButtonSingleClick) || (IRcode == IR_PUSH_CODE))
			{
				PLUS_BUT_STATE = eButtonHoldOff;
				IRcode = 0;
				if((PlayCfg.Parameter.rount_time_s/60) < 99)
					PlayCfg.Parameter.rount_time_s++;
				//Update Display
				Led7RoundTime_Display((PlayCfg.Parameter.rount_time_s/60)/10,
									  (PlayCfg.Parameter.rount_time_s/60)%10,
									  (PlayCfg.Parameter.rount_time_s%60)/10,
									  (PlayCfg.Parameter.rount_time_s%60)%10, 1);
				buzzer_stt = 1;
			}else if((MINUS_BUT_STATE == eButtonSingleClick) || (IRcode == IR_MINUS_CODE)){
				MINUS_BUT_STATE = eButtonHoldOff;
				IRcode = 0;
				if(PlayCfg.Parameter.rount_time_s > 0)
					PlayCfg.Parameter.rount_time_s--;
				//Update Display
				Led7RoundTime_Display((PlayCfg.Parameter.rount_time_s/60)/10,
									  (PlayCfg.Parameter.rount_time_s/60)%10,
									  (PlayCfg.Parameter.rount_time_s%60)/10,
									  (PlayCfg.Parameter.rount_time_s%60)%10, 1);
				buzzer_stt = 1;
			}else if((NEXT_BUT_STATE == eButtonSingleClick) || (IRcode == IR_NEXT_CODE)){
				NEXT_BUT_STATE = eButtonHoldOff;
				IRcode = 0;
				cfg_state_t++;
				pCfg_1st_state_Flag = 1;
				Led7RoundTime_Display(10, 10, 10, 10, 0);
				buzzer_stt = 1;
			}
			break;
		}
		case E_CFG_TURNTIME_S:
		{
			if(pCfg_1st_state_Flag){
				pCfg_1st_state_Flag = 0;
				//Send Display and Blink Start point
				Led7TurnTime_Display(PlayCfg.Parameter.turn_time_s/10, PlayCfg.Parameter.turn_time_s%10, 0, 0);
				buzzer_stt = 1;
			}
			//Check button
			if((PLUS_BUT_STATE == eButtonSingleClick) || (IRcode == IR_PUSH_CODE))
			{
				PLUS_BUT_STATE = eButtonHoldOff;
				IRcode = 0;
				if(PlayCfg.Parameter.turn_time_s < 99)
					PlayCfg.Parameter.turn_time_s++;
				else
					PlayCfg.Parameter.turn_time_s = 0;
				//Update Display
				Led7TurnTime_Display(PlayCfg.Parameter.turn_time_s/10, PlayCfg.Parameter.turn_time_s%10, 0, 0);
				buzzer_stt = 1;
			}else if((MINUS_BUT_STATE == eButtonSingleClick) || (IRcode == IR_MINUS_CODE)){
				MINUS_BUT_STATE = eButtonHoldOff;
				IRcode = 0;
				if(PlayCfg.Parameter.turn_time_s > 0)
					PlayCfg.Parameter.turn_time_s--;
				else
					PlayCfg.Parameter.turn_time_s = 99;
				//Update Display
				Led7TurnTime_Display(PlayCfg.Parameter.turn_time_s/10, PlayCfg.Parameter.turn_time_s%10, 0, 0);
				buzzer_stt = 1;
			}else if((NEXT_BUT_STATE == eButtonSingleClick) || (IRcode == IR_NEXT_CODE)){
				NEXT_BUT_STATE = eButtonHoldOff;
				IRcode = 0;
				cfg_state_t++;
				pCfg_1st_state_Flag = 1;
				Led7TurnTime_Display(10, 10, 0, 0);
				buzzer_stt = 1;
			}
			break;
		}
		case E_CFG_START_MODE:
		{
			if(pCfg_1st_state_Flag){
				pCfg_1st_state_Flag = 0;
				if(PlayCfg.Parameter.mode_signed == 0xFF)
					PlayCfg.Parameter.mode_signed = 2;

				Task_Led_StartPoint(12, PlayCfg.Parameter.playing_mode, 10 + PlayCfg.Parameter.mode_signed);
				//Send Display and Blink Start point
				for(int i=0;i<(PlayCfg.Parameter.playing_mode);i++){
					Task_led_xl(i+1, 0x0F);
				}
			}
			//Check button
			if((PLUS_BUT_STATE == eButtonSingleClick) || (IRcode == IR_PUSH_CODE))
			{
				PLUS_BUT_STATE = eButtonHoldOff;
				IRcode = 0;

				PlayCfg.Parameter.playing_mode++;
				if(PlayCfg.Parameter.playing_mode > (int)MAX_PLAYER_NUM)
				{
					for(int i=0;i<(int)MAX_PLAYER_NUM;i++){
						Task_led_xl(i+1, 0x00);
					}
					PlayCfg.Parameter.playing_mode = 2;
					PlayCfg.Parameter.mode_signed = 1 - PlayCfg.Parameter.mode_signed;
				}
				//Update Display
				Task_Led_StartPoint(12, PlayCfg.Parameter.playing_mode, 10 + PlayCfg.Parameter.mode_signed);
				for(int i=0;i<PlayCfg.Parameter.playing_mode;i++){
					Task_led_xl(i+1, 0x0F);
				}
				buzzer_stt = 1;
			}else if((MINUS_BUT_STATE == eButtonSingleClick) || (IRcode == IR_MINUS_CODE)){
				MINUS_BUT_STATE = eButtonHoldOff;
				IRcode = 0;

				if(PlayCfg.Parameter.playing_mode > 2){
					for(int i=0;i<PlayCfg.Parameter.playing_mode;i++){
						Task_led_xl(i+1, 0x00);
					}
					PlayCfg.Parameter.playing_mode--;
				}
				else{
					PlayCfg.Parameter.playing_mode = (int)MAX_PLAYER_NUM;
					PlayCfg.Parameter.mode_signed = 1 - PlayCfg.Parameter.mode_signed;
				}
				//Update Display
				Task_Led_StartPoint(12, PlayCfg.Parameter.playing_mode, 10 + PlayCfg.Parameter.mode_signed);
				for(int i=0;i<PlayCfg.Parameter.playing_mode;i++){
					Task_led_xl(i+1, 0x0F);
				}
				buzzer_stt = 1;
			}else if((NEXT_BUT_STATE == eButtonSingleClick) || (IRcode == IR_NEXT_CODE)){
				NEXT_BUT_STATE = eButtonHoldOff;
				IRcode = 0;

				Task_Led_StartPoint(12, 12, 12);
				for(int i=0;i<PlayCfg.Parameter.playing_mode;i++){
					Task_led_xl(i+1, 0x00);
				}
				cfg_state_t = E_CFG_START_POINT;
				pCfg_1st_state_Flag = 1;
				buzzer_stt = 1;
			}
			break;
		}
//		case E_CFG_STARTUP_TIME:
//		{
//			if(pCfg_1st_state_Flag){
//				pCfg_1st_state_Flag = 0;
//				//Clear old display and blink
//
//				//Send Display and Blink Start point
//
//			}
//			//Check button
//			if(PLUS_BUT_STATE == eButtonSingleClick)
//			{
//				PLUS_BUT_STATE = eButtonHoldOff;
//
//				if(PlayCfg.Parameter.startup_time_m < 99)
//					PlayCfg.Parameter.startup_time_m++;
//				else
//					PlayCfg.Parameter.startup_time_m = 1;
//				//Update Display
//
//			}else if(MINUS_BUT_STATE == eButtonSingleClick){
//				MINUS_BUT_STATE = eButtonHoldOff;
//				if(PlayCfg.Parameter.startup_time_m > 0)
//					PlayCfg.Parameter.startup_time_m--;
//				else
//					PlayCfg.Parameter.startup_time_m = 99;
//				//Update Display
//
//			}else if(NEXT_BUT_STATE == eButtonSingleClick){
//				NEXT_BUT_STATE = eButtonHoldOff;
//				cfg_state_t = E_CFG_START_POINT;
//				pCfg_1st_state_Flag = 1;
//			}
//			break;
//		}
	}
}

uint8_t Task_User_1stInit(uint8_t readFlash)
{
	//Read from FLASH
	if(readFlash)
		Task_Read_Cfg();

	if(PlayCfg.Parameter.playing_mode < 2){
		PlayCfg.Parameter.playing_mode = 4;
		//return 0;
	}

	all_turn_cnt = 1;
	hit_get_point_cnt = 0;

	for(int i=0;i<PlayCfg.Parameter.playing_mode;i++)
	{
		Player[i].ledxl_mask = 0x0F;
		Player[i].addr = i+1;
		Player[i].average = 0;
		Player[i].point = PlayCfg.Parameter.mode_signed*PlayCfg.Parameter.start_point*((i>=PlayCfg.Parameter.playing_mode?0:1));
		Player[i].max_hit_get_point = 0;
		Player[i].sum_point = 0;
		Player[i].sum_signed = 0;
	}
	return 1;
}

uint8_t Task_Round_Init(void)
{
	turn_time_s = PlayCfg.Parameter.turn_time_s;
	rount_time_s = PlayCfg.Parameter.rount_time_s;

	//all_turn_cnt = 1;
	//hit_get_point_cnt = 0;

	//Disable Lose Player
	for(int i=0;i<PlayCfg.Parameter.playing_mode;i++)
	{
		Player[i].sum_point = 0;
		Player[i].sum_signed = 0;
		if(Player[i].addr == 1)
			current_player = i;
	}

	//Send data to Display
	Led7TurnTime_Display(turn_time_s/10, turn_time_s%10, 8, 8);
	Led7HitCnt_Display(all_turn_cnt/10, all_turn_cnt%10, hit_get_point_cnt/10, hit_get_point_cnt%10);
	Led7RoundTime_Display((rount_time_s/60)/10, (rount_time_s/60)%10, (rount_time_s%60)/10, (rount_time_s%60)%10, 1);

	if(PlayCfg.Parameter.mode_signed)
	{
		Task_Led_StartPoint(PlayCfg.Parameter.start_point/100, (PlayCfg.Parameter.start_point%100)/10, PlayCfg.Parameter.start_point%10);
	}else{
		Task_Led_StartPoint(0, 0, 0);
	}

	for(int i=0;i<PlayCfg.Parameter.playing_mode;i++)
	{
		Task_led_xl(Player[i].addr, Player[i].ledxl_mask);
		Task_Blink_Line(i+1, 0, 1);
	}
	Task_Upload_Display();

	buzzer_stt = 1;
	pTask_1st_Flag = 1;
	return 1;
}

uint8_t Task_Playing(void)
{
	if(pTask_1st_Flag)
	{
		//point_plus = get_player_available() - 1;
		turn_time_s = PlayCfg.Parameter.turn_time_s;

		last_average = Player[current_player].average;
		Player[current_player].average = (last_average*(all_turn_cnt-1) + hit_get_point_cnt)/(float)all_turn_cnt;
		if(Player[current_player].average>99.99)
			Player[current_player].average = 99.99;

		Led7TurnTime_Display(turn_time_s/10, turn_time_s%10, 8, 8);
		for(int i=0;i<PlayCfg.Parameter.playing_mode;i++)
		{
			if(i==current_player)
				Task_Blink_Line(Player[current_player].addr, 1, 1);
			else{
				Task_Blink_Line(Player[i].addr, 0, 1);
			}
		}
		stop_time = 0;
		pTask_1st_Flag = 0;
		return 0;
	}

	//BUT_NEW_STA_FLAG = 0;
	if((MODE_BUT_STATE == eButtonLongPressT1) && (PLUS_BUT_STATE == eButtonSingleClick))
	{
		PLUS_BUT_STATE = eButtonHoldOff;
		MODE_BUT_STATE = eButtonHoldOff;

		Player[current_player].ledxl_mask = (Player[current_player].ledxl_mask<<1)|0x01;
		Task_led_xl(Player[current_player].addr, Player[current_player].ledxl_mask);
		buzzer_stt = 1;
	}
	else if((PLUS_BUT_STATE == eButtonSingleClick) || (IRcode == IR_PUSH_CODE))
	{
		PLUS_BUT_STATE = eButtonHoldOff;
		IRcode = 0;

		turn_time_s = PlayCfg.Parameter.turn_time_s;

		hit_get_point_cnt++;
		//Update CTCN
		if(hit_get_point_cnt > Player[current_player].max_hit_get_point)
			Player[current_player].max_hit_get_point = hit_get_point_cnt;

		if((hit_get_point_cnt < 100) && (all_turn_cnt < 100)){
			Led7HitCnt_Display(all_turn_cnt/10, all_turn_cnt%10, hit_get_point_cnt/10, hit_get_point_cnt%10);
		}

		Player[current_player].average = (last_average*(all_turn_cnt-1) + hit_get_point_cnt)/(float)all_turn_cnt;
		if(Player[current_player].average>99.99)
			Player[current_player].average = 99.99;

		//Update  DHT & SDTL
		if(PlayCfg.Parameter.mode_signed)
		{
			point_plus = get_player_available() - 1;
			for(int i=0;i<PlayCfg.Parameter.playing_mode;i++)
			{
				if(Player[i].point == 0)
					continue;

				if(i==current_player){
					Player[i].point += point_plus;
					if(Player[i].sum_signed)
					{
						if(point_plus >= Player[i].sum_point){
							Player[i].sum_point = point_plus - Player[i].sum_point;
							Player[i].sum_signed = 0;
						} else{
							Player[i].sum_point -= point_plus;
						}
					}else{
						Player[i].sum_point += point_plus;
					}

				}else{
					if(Player[i].point > 0)
						Player[i].point -= 1;

					if(Player[i].sum_signed){
						Player[i].sum_point += 1;
					}else{
						if(Player[i].sum_point == 0){
							Player[i].sum_point = 1;
							Player[i].sum_signed = 1;
						}else{
							Player[i].sum_point -= 1;
						}
					}
				}
			}
		}else{
			Player[current_player].point += 1;
			if(Player[current_player].sum_signed){
				if(Player[current_player].sum_point == 1){
					Player[current_player].sum_point = 0;
					Player[current_player].sum_signed = 0;
				}else{
					Player[current_player].sum_point -= 1;
				}
			} else{
				Player[current_player].sum_point += 1;
			}
		}

		//Send Display
		Task_Upload_Display();
		buzzer_stt = 1;
	}

	if((MODE_BUT_STATE == eButtonLongPressT1) && (MINUS_BUT_STATE == eButtonSingleClick))
	{
		MINUS_BUT_STATE = eButtonHoldOff;
		MODE_BUT_STATE = eButtonHoldOff;

		if(Player[current_player].ledxl_mask != 0)
		{
			Player[current_player].ledxl_mask = (Player[current_player].ledxl_mask>>1)&0x0F;
			Task_led_xl(Player[current_player].addr, Player[current_player].ledxl_mask);
			turn_time_s = PlayCfg.Parameter.turn_time_s;
		}
		buzzer_stt = 1;
	}
	else if((MINUS_BUT_STATE == eButtonSingleClick) || (IRcode == IR_MINUS_CODE))
	{
		MINUS_BUT_STATE = eButtonHoldOff;
		IRcode = 0;
		//HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET);
		turn_time_s = PlayCfg.Parameter.turn_time_s;

		if(PlayCfg.Parameter.mode_signed)
		{
			point_plus = get_player_available() - 1;
			for(int i=0;i<PlayCfg.Parameter.playing_mode;i++)
			{
				if(Player[i].point == 0)
					continue;

				if(i==current_player){
					if(Player[i].point >= point_plus)
						Player[current_player].point -= point_plus;
					else
						Player[current_player].point = 0;

					if(Player[i].sum_signed){
						Player[i].sum_point += point_plus;
					}else{
						if(point_plus > Player[i].sum_point){
							Player[i].sum_point = point_plus - Player[i].sum_point;
							Player[i].sum_signed = 1;
						}else{
							Player[i].sum_point -= point_plus;
						}
					}
				} else{
					Player[i].point += 1;
					if(Player[i].sum_signed){
						if(Player[i].sum_point == 1){
							Player[i].sum_point = 0;
							Player[i].sum_signed = 0;
						}else{
							Player[i].sum_point -= 1;
						}
					} else{
						Player[i].sum_point += 1;
					}
				}
			}
		} else{
			if(Player[current_player].point > 0)
				Player[current_player].point -= 1;

			if(Player[current_player].sum_signed){
				Player[current_player].sum_point += 1;
			}else{
				if(Player[current_player].sum_point == 0){
					Player[current_player].sum_point = 1;
					Player[current_player].sum_signed = 1;
				}else{
					Player[current_player].sum_point -= 1;
				}
			}
		}
		//Send Display
		Task_Upload_Display();
		buzzer_stt = 1;
	}

	if((MODE_BUT_STATE == eButtonLongPressT1) && (NEXT_BUT_STATE == eButtonSingleClick) && (PlayCfg.Parameter.playing_mode == 2))
	{
		NEXT_BUT_STATE = eButtonHoldOff;
		MODE_BUT_STATE = eButtonHoldOff;

		current_player = 0;
		hit_get_point_cnt = 0;
		rount_time_s = PlayCfg.Parameter.rount_time_s;

		for(int i=0;i<PlayCfg.Parameter.playing_mode;i++)
		{
			Task_Blink_Line(i+1, 0, 1);
		}

		if((PlayCfg.Parameter.mode_signed) && (PlayCfg.Parameter.playing_mode > 2))
		{
			if(get_player_available() == 1)
				return 0xFF;
			//Random address, close player
			update_rand_addr();
		}
		return 1;
	}
	else if((NEXT_BUT_STATE == eButtonSingleClick) || (IRcode == IR_NEXT_CODE))
	{
		NEXT_BUT_STATE = eButtonHoldOff;
		IRcode = 0;

		uint8_t temp_play = get_next_user(current_player);
		if(temp_play == 0xFF)
			return temp_play;
		else if(Player[temp_play].addr < Player[current_player].addr)
			all_turn_cnt++;

		current_player = temp_play;

		hit_get_point_cnt = 0;
		for(int i=0;i<PlayCfg.Parameter.playing_mode;i++)
		{
			Player[i].sum_point = 0;
			Player[i].sum_signed = 0;
		}
		if(all_turn_cnt < 100){
			Led7HitCnt_Display(all_turn_cnt/10, all_turn_cnt%10, hit_get_point_cnt/10, hit_get_point_cnt%10);
		}
		Task_Upload_Display();
		buzzer_stt = 1;
		pTask_1st_Flag = 1;
	}
	return 0;
}

void Task_TestMode_Display(uint8_t global_num)
{
	buzzer_stt = 1;
	Task_Led_StartPoint(global_num, global_num, global_num);

	Led7TurnTime_Display(global_num, global_num, 8, 8);
	Led7HitCnt_Display(global_num, global_num, global_num, global_num);
	Led7RoundTime_Display(global_num, global_num, global_num, global_num, 1);

	printf("ADDR0%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d\n", (int)DATA_LED7_TYPE,
			global_num, global_num, global_num, global_num, global_num,
			global_num, global_num, global_num, global_num, global_num,
			global_num, global_num, global_num, global_num, global_num, global_num);
	printf("ADDR0%d\n", (int)UPDATE_LED7_TYPE);
	delay_ms(200);
}

void Task_Run_TestMode(void)
{
	static uint8_t num = 0;
	static uint8_t lxl_mask = 0x0F;

	if((PLUS_BUT_STATE == eButtonSingleClick)
	   || (MINUS_BUT_STATE == eButtonSingleClick)
	   || (NEXT_BUT_STATE == eButtonSingleClick)
	   || (MODE_BUT_STATE == eButtonSingleClick)
	   || (POWER_BUT_STATE == eButtonSingleClick)
	   || (IRcode != 0))
	{
		PLUS_BUT_STATE = eButtonHoldOff;
		MINUS_BUT_STATE = eButtonHoldOff;
		NEXT_BUT_STATE = eButtonHoldOff;
		MODE_BUT_STATE = eButtonHoldOff;
		POWER_BUT_STATE = eButtonHoldOff;
		IRcode = 0;

		lxl_mask = (lxl_mask>>1)&0x0F;
		Task_led_xl(0, lxl_mask);
		if(lxl_mask == 0)
			lxl_mask = 0x1F;

		num++;
		if(num > 9)
			num = 0;
		Task_TestMode_Display(num);
	}
}

void Task_100ms(void)
{
	if(Task1_Flag == 0)
			return;
	Task1_Flag = 0;
	if(buzzer_stt == 1){
		HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_SET);
		buzzer_stt = 0;
	}
	else if(buzzer_stt == 0){
		HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET);
		buzzer_stt = 0xFF;
	}
}

void Task_Playing_Time(void)
{
	if(Task2_Flag == 0)
		return;
	Task2_Flag = 0;

	if(IRcode == IR_STOP_CODE)
	{
		IRcode = 0;
		buzzer_stt = 1;
		if(stop_time)
			stop_time = 0;
		else
			stop_time = 1;
	}
	if(stop_time == 0)
		temp_s++;

	if(temp_s>=600){
		temp_s = 0;

	}

	if((temp_s%10) == 0)
	{
		if(rount_time_s>0){
			rount_time_s--;
			Led7RoundTime_Display((rount_time_s/60)/10, (rount_time_s/60)%10, (rount_time_s%60)/10, (rount_time_s%60)%10, 1);
		}

		if(turn_time_s > 0)
		{
			turn_time_s--;

			if(turn_time_s>=16){
				Led7TurnTime_Display(turn_time_s/10, turn_time_s%10, 8, 8);
			}
			else if(turn_time_s>7){
				Led7TurnTime_Display(turn_time_s/10, turn_time_s%10, 8, (turn_time_s%8));
				buzzer_stt = 1;
			}
			else{
				Led7TurnTime_Display(turn_time_s/10, turn_time_s%10, turn_time_s%8, 0);
				buzzer_stt = 1;
			}
		}
	}

}

void Task_Clear_Display(uint8_t addr)
{
	printf("ADDR%d%dEEEEEEEEEEEEEEEE\n", (int)addr, (int)DATA_LED7_TYPE);
	printf("ADDR%d%d\n", (int)addr, (int)UPDATE_LED7_TYPE);
	delay_ms(200);
}

void Task_led_xl(uint8_t addr, uint8_t state)
{
	printf("ADDR%d%d%c\n", addr, DATA_LEDXL_TYPE, (int)state+48);
}

void Task_Led_StartPoint(uint8_t tram, uint8_t chuc, uint8_t donvi)
{
	if(donvi == 12){
		printf("ADDR5%dEEE\n", DATA_LED7_TYPE);
	}
	else if(donvi == 11){
		printf("ADDR5%dE%dD\n", DATA_LED7_TYPE, chuc);
	}
	else if(donvi == 10){
		printf("ADDR5%dE%dE\n", DATA_LED7_TYPE, chuc);
	}
	else{
		printf("ADDR5%d%d%d%d\n", DATA_LED7_TYPE, (int)tram, (int)chuc, (int)donvi);
	}
	printf("ADDR52\n");
}

void Task_Blink_Line(uint8_t addr, uint8_t state, uint8_t line)
{
	printf("ADDR%d%d%d%d\n", (int)addr, (int)UPDATE_BLINK_STATE, (int)state, (int)line);
}

void Task_Buzzer_Enable(void)
{
	buzzer_stt = 1;
}
/* Private function -----------------------------------------------*/
static void Task_Upload_Display(void)
{
	for(int i=0;i<PlayCfg.Parameter.playing_mode;i++)
	{
		update_led7_data(i);
	}
	printf("ADDR0%d\n", (int)UPDATE_LED7_TYPE);
	delay_ms(200);
}

static void update_led7_data(uint8_t player_num)
{
	printf("ADDR%d%dEE%dE%03d%04d%02d%c%02d\n",
			(int)Player[player_num].addr,
			(int)DATA_LED7_TYPE,
			(int)(player_num+1),
			(int)Player[player_num].point,
			(int)(Player[player_num].average*100),
			(int)Player[player_num].max_hit_get_point,
			Player[player_num].sum_signed==0?'E':'D',
			(int)Player[player_num].sum_point);
}

static uint8_t get_player_available(void)
{
	uint8_t player = 0;
	for(int i=0;i<PlayCfg.Parameter.playing_mode;i++)
	{
		if(Player[i].point > 0){
			player++;
		}
	}
	return player;
}

static uint8_t get_next_user(uint8_t current_play)
{
	uint8_t temp_user_buff[4];
	int i, j;

	if(PlayCfg.Parameter.mode_signed == 0)
	{
		return (current_play+1)%PlayCfg.Parameter.playing_mode;
	}

	for(i=0;i<PlayCfg.Parameter.playing_mode;i++)
	{
		for(j=0;j<PlayCfg.Parameter.playing_mode;j++)
		{
			if(Player[j].addr == (i+1)){
				temp_user_buff[i] = j;
				break;
			}
		}
	}
	for(i=0;i<PlayCfg.Parameter.playing_mode;i++){
		if(temp_user_buff[i] == current_play)
			break;
	}

	for(j=1;j<PlayCfg.Parameter.playing_mode;j++){
		if(Player[temp_user_buff[(i+j)%PlayCfg.Parameter.playing_mode]].point > 0)
		 return temp_user_buff[(i+j)%PlayCfg.Parameter.playing_mode];
	}

	return 0xFF;
}

static void update_rand_addr(void)
{
	uint16_t rand_input = sys_millis;
	uint8_t temp_user_buff[4];
	uint8_t temp_addr_buff[4];
	int i, j;
	for(i=0;i<(int)MAX_PLAYER_NUM;i++)
	{
		for(j=0;j<(int)MAX_PLAYER_NUM;j++)
		{
			if(Player[j].addr == (i+1)){
				temp_user_buff[i] = j;
				temp_addr_buff[i] = Player[j].addr;
				break;
			}
		}
	}

	Player[temp_user_buff[0]].addr = temp_addr_buff[1];
	switch(rand_input%3)
	{
		case 0:
		{
			Player[temp_user_buff[1]].addr = temp_addr_buff[0];

			Player[temp_user_buff[2]].addr = temp_addr_buff[3];
			Player[temp_user_buff[3]].addr = temp_addr_buff[2];
			break;
		}
		case 1:
		{
			Player[temp_user_buff[1]].addr = temp_addr_buff[2];
			if(rand_input%2 == 1){
				Player[temp_user_buff[2]].addr = temp_addr_buff[0];
				Player[temp_user_buff[3]].addr = temp_addr_buff[3];
			}
			else{
				Player[temp_user_buff[2]].addr = temp_addr_buff[3];
				Player[temp_user_buff[3]].addr = temp_addr_buff[0];
			}
			break;
		}
		case 2:
		{
			Player[temp_user_buff[1]].addr = temp_addr_buff[3];
			if(rand_input%2 == 0){
				Player[temp_user_buff[2]].addr = temp_addr_buff[0];
				Player[temp_user_buff[3]].addr = temp_addr_buff[2];
			}
			else{
				Player[temp_user_buff[2]].addr = temp_addr_buff[2];
				Player[temp_user_buff[3]].addr = temp_addr_buff[0];
			}
			break;
		}
	}
}

static void delay_ms(unsigned int x)
{
	static unsigned long uDelay = 0;
	uDelay = x;
	bFlagDelay = 1;
	uCountDelay = 0;
	while(uCountDelay<=uDelay)
	{

	}
	bFlagDelay = 0;
}
