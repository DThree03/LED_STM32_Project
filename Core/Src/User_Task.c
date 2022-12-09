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

#include <stdio.h>
#include <string.h>
/* Private typedef -----------------------------------------------------------*/
typedef enum{
	E_CFG_START_POINT,
	E_CFG_ROUNDTIME_M,
	E_CFG_TURNTIME_S,
	E_CFG_START_MODE,
	E_CFG_STARTUP_TIME
}eUSER_CFG_STATE;

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
#define BEGIN_POINT		30
#define TURN_TIME_S		45
#define ROUND_TIME_M	30

#define MAX_PLAYER_NUM	4

#define DATA_LED7_TYPE      1
#define UPDATE_LED7_TYPE    2
#define DATA_LEDXL_TYPE     3
#define UPDATE_BLINK_STATE  4
/* USER CODE END PM */

/* Public variables ---------------------------------------------------------*/
extern structIO_Button strIO_Button_Value;

volatile uint16_t sys_millis = 0;
volatile uint8_t bFlagDelay = 0;
volatile uint32_t uCountDelay = 0;

volatile uint8_t Task1_Flag = 0;
volatile uint8_t Task2_Flag = 0;

/* Private variables ---------------------------------------------------------*/
static uint8_t pTask_1st_Flag;
/*	Read from FLASH	 */
static uint16_t start_point;
static uint16_t turn_time_s;
static uint32_t rount_time_m;

static uint8_t last_playing_mode;
static uint8_t startup_time_s;
/**********************************************/
static uint32_t all_turn_cnt = 1;
static float	last_average;
static uint16_t hit_get_point_cnt;

static player_data_t Player[MAX_PLAYER_NUM];
//static uint8_t random_addr_buff[MAX_PLAYER_NUM];
static uint8_t current_player;
static uint8_t point_plus;
static uint16_t temp_s = 0;
/* Private function prototypes -----------------------------------------------*/
static uint8_t get_player_available(void);

static void Task_Upload_Display(void);
static void update_led7_data(uint8_t player_num);

//static uint8_t update_rand_addr(void);
static void delay_ms(unsigned int x);

/* Public function -----------------------------------------------------------*/
void Task_Mode_Cfg(void)
{
	static eUSER_CFG_STATE cfg_state_t = E_CFG_START_POINT;
	static uint8_t pCfg_1st_state_Flag = 1;
	switch(cfg_state_t)
	{
		case E_CFG_START_POINT:
		{
			if(pCfg_1st_state_Flag){
				pCfg_1st_state_Flag = 0;
				//Read from FLASH
				start_point = (int)BEGIN_POINT;
				//Clear old display and blink

				//Send Display and Blink Start point

			}
			//Check button
			if(PLUS_BUT_STATE == eButtonSingleClick)
			{
				PLUS_BUT_STATE = eButtonHoldOff;
				if(start_point<99)
					start_point++;
				//Update Display

			}else if(MINUS_BUT_STATE == eButtonSingleClick){
				MINUS_BUT_STATE = eButtonHoldOff;
				if(start_point > 0)
					start_point--;
				//Update Display

			}else if(NEXT_BUT_STATE == eButtonSingleClick){
				NEXT_BUT_STATE = eButtonHoldOff;
				//Save flash

				cfg_state_t++;
				pCfg_1st_state_Flag = 1;
			}
			break;
		}
		case E_CFG_ROUNDTIME_M:
		{
			if(pCfg_1st_state_Flag){
				pCfg_1st_state_Flag = 0;
				//Read from FLASH
				rount_time_m = (int)ROUND_TIME_M;
				//Clear old display and blink

				//Send Display and Blink Start point
				Led7RoundTime_Display((rount_time_m/60)/10, (rount_time_m/60)%10, (rount_time_m%60)/10, (rount_time_m%60)%10);
			}
			//Check button
			if(PLUS_BUT_STATE == eButtonSingleClick)
			{
				PLUS_BUT_STATE = eButtonHoldOff;
				if((rount_time_m/60) < 99)
					rount_time_m++;
				//Update Display
				Led7RoundTime_Display((rount_time_m/60)/10, (rount_time_m/60)%10, (rount_time_m%60)/10, (rount_time_m%60)%10);
			}else if(MINUS_BUT_STATE == eButtonSingleClick){
				MINUS_BUT_STATE = eButtonHoldOff;
				if(rount_time_m > 0)
					rount_time_m--;
				//Update Display
				Led7RoundTime_Display((rount_time_m/60)/10, (rount_time_m/60)%10, (rount_time_m%60)/10, (rount_time_m%60)%10);
			}else if(NEXT_BUT_STATE == eButtonSingleClick){
				NEXT_BUT_STATE = eButtonHoldOff;
				//Save flash

				cfg_state_t++;
				pCfg_1st_state_Flag = 1;
				Led7RoundTime_Display(10, 10, 10, 10);
			}
			break;
		}
		case E_CFG_TURNTIME_S:
		{
			if(pCfg_1st_state_Flag){
				pCfg_1st_state_Flag = 0;
				//Read from FLASH
				turn_time_s = (int)TURN_TIME_S;
				//Send Display and Blink Start point
				Led7TurnTime_Display(turn_time_s/10, turn_time_s%10, 0, 0);
			}
			//Check button
			if(PLUS_BUT_STATE == eButtonSingleClick)
			{
				PLUS_BUT_STATE = eButtonHoldOff;
				if(turn_time_s < 99)
					turn_time_s++;
				else
					turn_time_s = 0;
				//Update Display
				Led7TurnTime_Display(turn_time_s/10, turn_time_s%10, 0, 0);
			}else if(MINUS_BUT_STATE == eButtonSingleClick){
				MINUS_BUT_STATE = eButtonHoldOff;
				if(turn_time_s > 0)
					turn_time_s--;
				else
					turn_time_s = 99;
				//Update Display
				Led7TurnTime_Display(turn_time_s/10, turn_time_s%10, 0, 0);
			}else if(NEXT_BUT_STATE == eButtonSingleClick){
				NEXT_BUT_STATE = eButtonHoldOff;
				//Save flash

				cfg_state_t++;
				pCfg_1st_state_Flag = 1;
				Led7TurnTime_Display(10, 10, 0, 0);
			}
			break;
		}
		case E_CFG_START_MODE:
		{
			if(pCfg_1st_state_Flag){
				pCfg_1st_state_Flag = 0;
				//Read from FLASH
				last_playing_mode = (int)LAST_MODE;
				//Send Display and Blink Start point

			}
			//Check button
			if(PLUS_BUT_STATE == eButtonSingleClick)
			{
				PLUS_BUT_STATE = eButtonHoldOff;
				last_playing_mode++;
				if(last_playing_mode > 2)
					last_playing_mode = 0;
				//Update Display

			}else if(MINUS_BUT_STATE == eButtonSingleClick){
				MINUS_BUT_STATE = eButtonHoldOff;
				if(last_playing_mode > 0)
					last_playing_mode--;
				else
					last_playing_mode = 2;
				//Update Display

			}else if(NEXT_BUT_STATE == eButtonSingleClick){
				NEXT_BUT_STATE = eButtonHoldOff;
				//Save flash

				cfg_state_t++;
				pCfg_1st_state_Flag = 1;
			}
			break;
		}
		case E_CFG_START_MODE:
		{
			if(pCfg_1st_state_Flag){
				pCfg_1st_state_Flag = 0;
				//Read from FLASH
				last_playing_mode = (int)LAST_MODE;
				//Clear old display and blink

				//Send Display and Blink Start point

			}
			//Check button
			if(PLUS_BUT_STATE == eButtonSingleClick)
			{
				PLUS_BUT_STATE = eButtonHoldOff;

				if(startup_time_s < 99)
					startup_time_s++;
				else
					startup_time_s = 1;
				//Update Display

			}else if(MINUS_BUT_STATE == eButtonSingleClick){
				MINUS_BUT_STATE = eButtonHoldOff;
				if(startup_time_s > 0)
					startup_time_s--;
				else
					startup_time_s = 99;
				//Update Display

			}else if(NEXT_BUT_STATE == eButtonSingleClick){
				NEXT_BUT_STATE = eButtonHoldOff;
				//Save flash

				cfg_state_t = E_CFG_START_POINT;
				pCfg_1st_state_Flag = 1;
			}
			break;
		}
	}
}

uint8_t Task_User_Init(uint8_t player_num)
{
	if((player_num!=2) && (player_num!=4))
		return 0;
	//Read from FLASH
	start_point = (int)BEGIN_POINT;
	turn_time_s = (int)TURN_TIME_S;
	rount_time_m = (int)ROUND_TIME_M;

	current_player = 0;
	hit_get_point_cnt = 0;

	for(int i=0;i<MAX_PLAYER_NUM;i++)
	{
		Player[i].ledxl_mask = 0x0F;
		Player[i].addr = i+1;
		Player[i].average = 0;
		Player[i].point = start_point*((i>=player_num?0:1));
		Player[i].max_hit_get_point = 0;
		Player[i].sum_point = 0;
		Player[i].sum_signed = 0;
	}

	//Send data to Display
	Led7TurnTime_Display(turn_time_s/10, turn_time_s%10, 8, 8);
	Led7HitCnt_Display(all_turn_cnt/10, all_turn_cnt%10, hit_get_point_cnt/10, hit_get_point_cnt%10);
	Led7RoundTime_Display((rount_time_m/60)/10, (rount_time_m/60)%10, (rount_time_m%60)/10, (rount_time_m%60)%10);

	Task_led_xl(0, 0x0F);
	Task_Upload_Display();

	pTask_1st_Flag = 1;
	return 1;
}

void Task_4_Player(void)
{
//	if(Task1_Flag == 0)
//		return;
//	Task1_Flag = 0;

	if(pTask_1st_Flag)
	{
		point_plus = get_player_available() - 1;

		last_average = Player[current_player].average;
		Player[current_player].average = (last_average*(all_turn_cnt-1) + hit_get_point_cnt)/(float)all_turn_cnt;
		if(Player[current_player].average>99.99)
			Player[current_player].average = 99.99;
		//Send data Display to all Addr

		//Send Blink cmd to Current User
		for(int i=0;i<MAX_PLAYER_NUM;i++)
		{
			if(i==current_player)
				Task_Blink_Line(current_player+1, 1, 1);
			else{
				Task_Blink_Line(i+1, 0, 1);
			}
		}
		pTask_1st_Flag = 0;
		return;
	}

	//Check Button
	if(BUT_NEW_STA_FLAG == 0)
		return;

	BUT_NEW_STA_FLAG = 0;
	if(PLUS_BUT_STATE == eButtonSingleClick)
	{
		PLUS_BUT_STATE = eButtonHoldOff;
		//HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET);

		turn_time_s = (int)TURN_TIME_S;
		hit_get_point_cnt++;
		if((hit_get_point_cnt < 100) && (all_turn_cnt < 100)){
			Led7HitCnt_Display(all_turn_cnt/10, all_turn_cnt%10, hit_get_point_cnt/10, hit_get_point_cnt%10);
		}

		Player[current_player].average = (last_average*(all_turn_cnt-1) + hit_get_point_cnt)/(float)all_turn_cnt;
		if(Player[current_player].average>99.99)
			Player[current_player].average = 99.99;

		//Update CTCN
		if(hit_get_point_cnt > Player[current_player].max_hit_get_point)
			Player[current_player].max_hit_get_point = hit_get_point_cnt;

		//Update  DHT & SDTL
		for(int i=0;i<MAX_PLAYER_NUM;i++)
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
		//Send Display
		Task_Upload_Display();
	}

	if(MINUS_BUT_STATE == eButtonSingleClick)
	{
		MINUS_BUT_STATE = eButtonHoldOff;

		//HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET);
		turn_time_s = (int)TURN_TIME_S;

		for(int i=0;i<MAX_PLAYER_NUM;i++)
		{
			if(Player[i].point == 0)
				continue;

			if(i==current_player){
				Player[current_player].point -= point_plus;
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

		//Send Display
		Task_Upload_Display();
		//Check all player Point to End task -> Next match
	}

	//Next round
	if((MODE_BUT_STATE == eButtonLongPressT1) && (NEXT_BUT_STATE == eButtonSingleClick))
	{
		NEXT_BUT_STATE = eButtonHoldOff;
		MODE_BUT_STATE = eButtonHoldOff;

		current_player = 0;
		hit_get_point_cnt = 0;
		rount_time_m = (int)ROUND_TIME_M;
		//while(!update_rand_addr());
		for(int i=0;i<MAX_PLAYER_NUM;i++)
		{
			Player[i].ledxl_mask = 0x0F;
			Player[i].addr = i+1;		//How to random???
			Player[i].point = start_point;
			Player[i].sum_point = 0;
			Player[i].sum_signed = 0;
		}
		Task_led_xl(0, 0x0F);
		Task_Upload_Display();
		pTask_1st_Flag = 1;
	}
	else if(NEXT_BUT_STATE == eButtonSingleClick)
	{
		NEXT_BUT_STATE = eButtonHoldOff;
		//HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET);

		hit_get_point_cnt = 0;
		turn_time_s = (int)TURN_TIME_S;
		for(int i=0;i<MAX_PLAYER_NUM;i++)
		{
			Player[i].sum_point = 0;
			Player[i].sum_signed = 0;
		}

		for(int i=0;i<MAX_PLAYER_NUM;i++)	//Change current player to next address
		{
			if(Player[current_player].addr == 4)
			{
				if(Player[i].addr == 1){
					all_turn_cnt++;
					current_player = i;
					break;
				}
			}
			else{
				if(Player[i].addr == (Player[current_player].addr+1)){
					current_player = i;
					break;
				}
			}
		}
		if((hit_get_point_cnt < 100) && (all_turn_cnt < 100)){
			Led7HitCnt_Display(all_turn_cnt/10, all_turn_cnt%10, hit_get_point_cnt/10, hit_get_point_cnt%10);
		}
		Task_Upload_Display();
		pTask_1st_Flag = 1;
	}
}

void Task_500ms(void)
{
	if(Task2_Flag == 0)
		return;
	Task2_Flag = 0;

	temp_s++;

	if(temp_s>=120){
		temp_s = 0;

		if(rount_time_m>0)
			rount_time_m--;

		Led7RoundTime_Display((rount_time_m/60)/10, (rount_time_m/60)%10, (rount_time_m%60)/10, (rount_time_m%60)%10);
	}

	if(turn_time_s > 0)
	{
		if((temp_s%2) == 0)
		{
			turn_time_s--;

			if(turn_time_s>=16){
				Led7TurnTime_Display(turn_time_s/10, turn_time_s%10, 8, 8);
			}
			else if(turn_time_s>7){
				Led7TurnTime_Display(turn_time_s/10, turn_time_s%10, 8, (turn_time_s%8));
				//HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET);
			}
			else{
				Led7TurnTime_Display(turn_time_s/10, turn_time_s%10, turn_time_s%8, 0);
				//HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET);
			}
		}
		else if(turn_time_s < 16){
			//HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_SET);
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

void Task_Blink_Line(uint8_t addr, uint8_t state, uint8_t line)
{
	printf("ADDR%d%d%d%d\n", (int)addr, (int)UPDATE_BLINK_STATE, (int)state, (int)line);
}

/* Private function -----------------------------------------------*/
static void Task_Upload_Display(void)
{
	for(int i=0;i<MAX_PLAYER_NUM;i++)
	{
		update_led7_data(i);
	}
	printf("ADDR02\n");
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
	for(int i=0;i<MAX_PLAYER_NUM;i++)
	{
		if(Player[i].point > 0){
			player++;
		}
	}
	return player;
}

//static uint8_t update_rand_addr(void)
//{
//	uint16_t rand_input = sys_millis;
//	uint8_t temp_buff[4] = {Player[0].addr, Player[1].addr, Player[2].addr, Player[3].addr};
//
//	if(Player[0].addr == 4)
//	{
//		Player[0].addr = 1;
//	}
//	else{
//		Player[0].addr +=1;
//	}
//
//	Player[0].addr = temp_buff[1];
//	Player[1].addr = rand_input%2==0?temp_buff[0]:temp_buff[3];
//	if(Player[1].addr == temp_buff[3])
//{
//		Player[2].addr = temp_buff[0];
//		Player[3].addr = temp_buff[2];
//	}
//
//}

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
