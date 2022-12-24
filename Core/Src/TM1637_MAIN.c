/*
 * TM1637.c
 *
 *  Created on: Dec 4, 2022
 *      Author: user
 */
/* Includes ------------------------------------------------------------------*/
#include <TM1637_MAIN.h>
#include "main.h"

/* Private macro -------------------------------------------------------------*/
#define TM1637_CLK1_PIN		CLK_1_Pin
#define TM1637_CLK1_ON		HAL_GPIO_WritePin(CLK_1_GPIO_Port, CLK_1_Pin, GPIO_PIN_SET)
#define TM1637_CLK1_OFF		HAL_GPIO_WritePin(CLK_1_GPIO_Port, CLK_1_Pin, GPIO_PIN_RESET)
#define TM1637_DIO1_PIN		DIO_1_Pin
#define TM1637_DIO1_ON		HAL_GPIO_WritePin(DIO_1_GPIO_Port, DIO_1_Pin, GPIO_PIN_SET)
#define TM1637_DIO1_OFF		HAL_GPIO_WritePin(DIO_1_GPIO_Port, DIO_1_Pin, GPIO_PIN_RESET)
#define TM1637_DIO1_READ	HAL_GPIO_ReadPin(DIO_1_GPIO_Port, DIO_1_Pin)

#define TM1637_CLK2_PIN		CLK_2_Pin
#define TM1637_CLK2_ON		HAL_GPIO_WritePin(CLK_2_GPIO_Port, CLK_2_Pin, GPIO_PIN_SET)
#define TM1637_CLK2_OFF		HAL_GPIO_WritePin(CLK_2_GPIO_Port, CLK_2_Pin, GPIO_PIN_RESET)
#define TM1637_DIO2_PIN		DIO_2_Pin
#define TM1637_DIO2_ON		HAL_GPIO_WritePin(DIO_2_GPIO_Port, DIO_2_Pin, GPIO_PIN_SET)
#define TM1637_DIO2_OFF		HAL_GPIO_WritePin(DIO_2_GPIO_Port, DIO_2_Pin, GPIO_PIN_RESET)
#define TM1637_DIO2_READ	HAL_GPIO_ReadPin(DIO_2_GPIO_Port, DIO_2_Pin)

#define TM1637_CLK3_PIN		IO2_Pin
#define TM1637_CLK3_ON		HAL_GPIO_WritePin(IO2_GPIO_Port, IO2_Pin, GPIO_PIN_SET)
#define TM1637_CLK3_OFF		HAL_GPIO_WritePin(IO2_GPIO_Port, IO2_Pin, GPIO_PIN_RESET)
#define TM1637_DIO3_PIN		IO1_Pin
#define TM1637_DIO3_ON		HAL_GPIO_WritePin(IO1_GPIO_Port, IO1_Pin, GPIO_PIN_SET)
#define TM1637_DIO3_OFF		HAL_GPIO_WritePin(IO1_GPIO_Port, IO1_Pin, GPIO_PIN_RESET)
#define TM1637_DIO3_READ	HAL_GPIO_ReadPin(IO1_GPIO_Port, IO1_Pin)
/* Private typedef -----------------------------------------------------------*/


/* Public variables ---------------------------------------------------------*/

const uint8_t  TM1637_CODE_SEG_NO_DOT[13] = {0x77, 0x41, 0x3B, 0x6B, 0x4D, 0x6E, 0x7E, 0x43, 0x7F, 0x6F, 0x00, 0xFF, 0x5A};
const uint8_t  TM1637_CODE_SEG_HAS_DOT[13] = {0xF7, 0xC1, 0xBB, 0xEB, 0xCD, 0xEE, 0xFE, 0xC3, 0xFF, 0xEF, 0x00, 0xFF, 0x5A};

const uint8_t   TM1637_CODE_LED[9]={0x00,0x80,0x88,0x8C,0x9C,0xBC,0xFC,0xFD,0xFF};//VALUE_SHOW_LED[2]
const uint8_t   TM1637_CODE_LED_1[9]={0x00,0x02,0x03,0x43,0x63,0x73,0x77,0x7F,0xFF};//VALUE_SHOW_LED[3]

const uint8_t VALUE_SHOW_LED_2_CONST[11] = {0xE7,0x84,0xD3,0xD6,0xB4,0x76,0x77,0xC4,0xF7,0xF6,0x00};
const uint8_t VALUE_SHOW_LED_3_CONST[12] = {0xBE,0x12,0x7C,0x7A,0xD2,0xEA,0xEE,0x32,0xFE,0xFA,0x00, 0x11};

static uint8_t VALUE_SHOW_LED[6] = {10, 10, 10, 10, 0, 0};
static uint8_t VALUE_SHOW_LED_2[6] = {10, 10, 10, 10, 10, 10};
static uint8_t VALUE_SHOW_LED_3[6] = {10, 10, 10, 10, 10, 10};
/* Private function prototypes -----------------------------------------------*/
static void TM1637_GPIO_INIT(uint32_t Pin, uint32_t Mode);
static void Delay_us(unsigned long us);

static void Led7TurnTime_Start(void);
static void Led7TurnTime_Stop(void);
static uint8_t Led7TurnTime_Cask(void);
static void Led7TurnTime_Write(unsigned char oneByte);
static void Led7TurnTime_PWM(unsigned char oneByte);

static void Led7HitCnt_Start(void);
static void Led7HitCnt_Stop(void);
static uint8_t Led7HitCnt_Cask(void);
static void Led7HitCnt_Write(unsigned char oneByte);
static void Led7HitCnt_PWM(unsigned char oneByte);


static void Led7RoundTime_Start(void);
static void Led7RoundTime_Stop(void);
static uint8_t Led7RoundTime_Cask(void);
static void Led7RoundTime_Write(unsigned char oneByte);
static void Led7RoundTime_PWM(unsigned char oneByte);

/* Public function -----------------------------------------------------------*/
void Led7TurnTime_Display(int led7ch, int led7dv, int led_num_left, int led_num_right)
{
	__disable_irq();

	VALUE_SHOW_LED[4] = led7ch;
	VALUE_SHOW_LED[5] = led7dv;

	VALUE_SHOW_LED[2] = led_num_right;
	VALUE_SHOW_LED[3] = led_num_left;

	Led7TurnTime_Start();
	Led7TurnTime_Write(TM1637_I2C_COMM1);
	Led7TurnTime_Cask();
	Led7TurnTime_Stop();

	Led7TurnTime_Start();
	Led7TurnTime_Write(0xC0);
	Led7TurnTime_Cask();

	Led7TurnTime_Write(TM1637_CODE_LED[VALUE_SHOW_LED[0]]);
	Led7TurnTime_Cask();

	Led7TurnTime_Write(TM1637_CODE_LED[VALUE_SHOW_LED[1]]);
	Led7TurnTime_Cask();

	Led7TurnTime_Write(TM1637_CODE_LED[VALUE_SHOW_LED[2]]);
	Led7TurnTime_Cask();

	Led7TurnTime_Write(TM1637_CODE_LED_1[VALUE_SHOW_LED[3]]);
	Led7TurnTime_Cask();

	Led7TurnTime_Write(TM1637_CODE_SEG_NO_DOT[VALUE_SHOW_LED[4]]);
	Led7TurnTime_Cask();

	Led7TurnTime_Write(TM1637_CODE_SEG_NO_DOT[VALUE_SHOW_LED[5]]);
	Led7TurnTime_Cask();

	Led7TurnTime_Stop();

	Led7TurnTime_PWM(PWM8);

	__enable_irq();
}

void Led7HitCnt_Display(int slc_ch, int slc_dv, int sct_ch, int sct_dv)
{
	__disable_irq();

	VALUE_SHOW_LED_2[3] = slc_ch;
	VALUE_SHOW_LED_2[4] = slc_dv;

	VALUE_SHOW_LED_2[1] = sct_ch;
	VALUE_SHOW_LED_2[2] = sct_dv;

	Led7HitCnt_Start();
	Led7HitCnt_Write(TM1637_I2C_COMM1);
	Led7HitCnt_Cask();
	Led7HitCnt_Stop();

	Led7HitCnt_Start();
	Led7HitCnt_Write(0xC0);
	Led7HitCnt_Cask();

	Led7HitCnt_Write(VALUE_SHOW_LED_2_CONST[VALUE_SHOW_LED_2[0]]);
	Led7HitCnt_Cask();

	Led7HitCnt_Write(VALUE_SHOW_LED_2_CONST[VALUE_SHOW_LED_2[1]]);
	Led7HitCnt_Cask();

	Led7HitCnt_Write(VALUE_SHOW_LED_2_CONST[VALUE_SHOW_LED_2[2]]);
	Led7HitCnt_Cask();

	Led7HitCnt_Write(VALUE_SHOW_LED_2_CONST[VALUE_SHOW_LED_2[3]]);
	Led7HitCnt_Cask();

	Led7HitCnt_Write(VALUE_SHOW_LED_2_CONST[VALUE_SHOW_LED_2[4]]);
	Led7HitCnt_Cask();

	Led7HitCnt_Write(VALUE_SHOW_LED_2_CONST[VALUE_SHOW_LED_2[5]]);
	Led7HitCnt_Cask();

	Led7HitCnt_Stop();

	Led7HitCnt_PWM(PWM8);

	__enable_irq();
}


void Led7RoundTime_Display(int h_ch, int h_dv, int m_ch, int m_dv, uint8_t led)
{
	__disable_irq();

	VALUE_SHOW_LED_3[4] = h_ch;
	VALUE_SHOW_LED_3[3] = h_dv;

	VALUE_SHOW_LED_3[1] = m_ch;
	VALUE_SHOW_LED_3[0] = m_dv;

	VALUE_SHOW_LED_3[2] = 10 + led;

	Led7RoundTime_Start();
	Led7RoundTime_Write(TM1637_I2C_COMM1);
	Led7RoundTime_Cask();
	Led7RoundTime_Stop();

	Led7RoundTime_Start();
	Led7RoundTime_Write(0xC0);
	Led7RoundTime_Cask();

	Led7RoundTime_Write(VALUE_SHOW_LED_3_CONST[VALUE_SHOW_LED_3[0]]);
	Led7RoundTime_Cask();

	Led7RoundTime_Write(VALUE_SHOW_LED_3_CONST[VALUE_SHOW_LED_3[1]]);
	Led7RoundTime_Cask();

	Led7RoundTime_Write(VALUE_SHOW_LED_3_CONST[VALUE_SHOW_LED_3[2]]);
	Led7RoundTime_Cask();

	Led7RoundTime_Write(VALUE_SHOW_LED_3_CONST[VALUE_SHOW_LED_3[3]]);
	Led7RoundTime_Cask();

	Led7RoundTime_Write(VALUE_SHOW_LED_3_CONST[VALUE_SHOW_LED_3[4]]);
	Led7RoundTime_Cask();

	Led7RoundTime_Write(VALUE_SHOW_LED_3_CONST[VALUE_SHOW_LED_3[5]]);
	Led7RoundTime_Cask();

	Led7RoundTime_Stop();

	Led7RoundTime_PWM(PWM8);

	__enable_irq();
}

/* Private function prototypes -----------------------------------------------*/
static void Led7TurnTime_Start(void)
{
	TM1637_CLK1_ON;
	TM1637_GPIO_INIT(TM1637_DIO1_PIN, GPIO_MODE_OUTPUT_OD);
	TM1637_DIO1_ON;
	Delay_us(2);
	TM1637_DIO1_OFF;
}

static void Led7TurnTime_Write(unsigned char oneByte)
{
	TM1637_GPIO_INIT(TM1637_DIO1_PIN, GPIO_MODE_OUTPUT_OD);

	for(int i=0;i<8;i++)
	{
		TM1637_CLK1_OFF;
		if(oneByte&0x01) //????
		{
		  TM1637_DIO1_ON;
		}
		else
		{
		  TM1637_DIO1_OFF;
		}
		Delay_us(3);
		oneByte=oneByte>>1;
		TM1637_CLK1_ON;
		Delay_us(3);
	}
}

static uint8_t Led7TurnTime_Cask(void)
{
	TM1637_CLK1_OFF;
	Delay_us(5); //
	TM1637_GPIO_INIT(TM1637_DIO1_PIN, GPIO_MODE_INPUT);
	//while(TM1637_DIO1_READ);
	if(TM1637_DIO1_READ)
		return 0xFF;
	TM1637_CLK1_ON;
	Delay_us(2);
	TM1637_CLK1_OFF;
	return 1;
}

static void Led7TurnTime_Stop(void)
{
	TM1637_CLK1_OFF;
	Delay_us(2);
	TM1637_GPIO_INIT(TM1637_DIO1_PIN, GPIO_MODE_OUTPUT_OD);
	TM1637_DIO1_OFF;
	Delay_us(2);
	TM1637_CLK1_ON;
	Delay_us(2);
	TM1637_DIO1_ON;
}

static void Led7TurnTime_PWM(unsigned char PWM)
{
	Led7TurnTime_Start();
	Led7TurnTime_Write(PWM | Dis_ON);
	Led7TurnTime_Cask();
	Led7TurnTime_Stop();
}

/* Private function prototypes -----------------------------------------------*/
static void Led7HitCnt_Start(void)
{
	TM1637_CLK2_ON;
	TM1637_GPIO_INIT(TM1637_DIO2_PIN, GPIO_MODE_OUTPUT_OD);
	TM1637_DIO2_ON;
	Delay_us(2);
	TM1637_DIO2_OFF;
}

static void Led7HitCnt_Write(unsigned char oneByte)
{
	TM1637_GPIO_INIT(TM1637_DIO2_PIN, GPIO_MODE_OUTPUT_OD);

	for(int i=0;i<8;i++)
	{
		TM1637_CLK2_OFF;
		if(oneByte&0x01) //????
		{
		  TM1637_DIO2_ON;
		}
		else
		{
		  TM1637_DIO2_OFF;
		}
		Delay_us(3);
		oneByte=oneByte>>1;
		TM1637_CLK2_ON;
		Delay_us(3);
	}
}

static uint8_t Led7HitCnt_Cask(void)
{
	TM1637_CLK2_OFF;
	Delay_us(5); //
	TM1637_GPIO_INIT(TM1637_DIO2_PIN, GPIO_MODE_INPUT);
	//while(TM1637_DIO2_READ);
	if(TM1637_DIO2_READ)
		return 0xFF;
	TM1637_CLK2_ON;
	Delay_us(2);
	TM1637_CLK2_OFF;
	return 1;
}

static void Led7HitCnt_Stop(void)
{
	TM1637_CLK2_OFF;
	Delay_us(2);
	TM1637_GPIO_INIT(TM1637_DIO2_PIN, GPIO_MODE_OUTPUT_OD);
	TM1637_DIO2_OFF;
	Delay_us(2);
	TM1637_CLK2_ON;
	Delay_us(2);
	TM1637_DIO2_ON;
}

static void Led7HitCnt_PWM(unsigned char PWM)
{
	Led7HitCnt_Start();
	Led7HitCnt_Write(PWM | Dis_ON);
	Led7HitCnt_Cask();
	Led7HitCnt_Stop();
}

/* Private function prototypes -----------------------------------------------*/
static void Led7RoundTime_Start(void)
{
	TM1637_CLK3_ON;
	TM1637_GPIO_INIT(TM1637_DIO3_PIN, GPIO_MODE_OUTPUT_OD);
	TM1637_DIO3_ON;
	Delay_us(2);
	TM1637_DIO3_OFF;
}

static void Led7RoundTime_Write(unsigned char oneByte)
{
	TM1637_GPIO_INIT(TM1637_DIO3_PIN, GPIO_MODE_OUTPUT_OD);

	for(int i=0;i<8;i++)
	{
		TM1637_CLK3_OFF;
		if(oneByte&0x01) //????
		{
		  TM1637_DIO3_ON;
		}
		else
		{
		  TM1637_DIO3_OFF;
		}
		Delay_us(3);
		oneByte=oneByte>>1;
		TM1637_CLK3_ON;
		Delay_us(3);
	}
}

static uint8_t Led7RoundTime_Cask(void)
{
	TM1637_CLK3_OFF;
	Delay_us(5); //
	TM1637_GPIO_INIT(TM1637_DIO3_PIN, GPIO_MODE_INPUT);
	//while(TM1637_DIO3_READ);
	if(TM1637_DIO3_READ)
		return 0xFF;
	TM1637_CLK3_ON;
	Delay_us(2);
	TM1637_CLK3_OFF;
	return 1;
}

static void Led7RoundTime_Stop(void)
{
	TM1637_CLK3_OFF;
	Delay_us(2);
	TM1637_GPIO_INIT(TM1637_DIO3_PIN, GPIO_MODE_OUTPUT_OD);
	TM1637_DIO3_OFF;
	Delay_us(2);
	TM1637_CLK3_ON;
	Delay_us(2);
	TM1637_DIO3_ON;
}

static void Led7RoundTime_PWM(unsigned char PWM)
{
	Led7RoundTime_Start();
	Led7RoundTime_Write(PWM | Dis_ON);
	Led7RoundTime_Cask();
	Led7RoundTime_Stop();
}


static void TM1637_GPIO_INIT(uint32_t Pin, uint32_t Mode)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	GPIO_InitStruct.Pin = Pin;
	GPIO_InitStruct.Mode = Mode;
	if(Mode == GPIO_MODE_OUTPUT_OD){
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	}
	else{
		GPIO_InitStruct.Pull = GPIO_PULLUP;
	}
	if((Pin == IO1_Pin) || (Pin == IO2_Pin))
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	else
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

static void Delay_us(unsigned long us) //nus ??
{
  while(us--)
  {
    __ASM volatile ("nop");
    __ASM volatile ("nop");
    __ASM volatile ("nop");
    __ASM volatile ("nop");
    __ASM volatile ("nop");

    __ASM volatile ("nop");
    __ASM volatile ("nop");
    __ASM volatile ("nop");
    __ASM volatile ("nop");
    __ASM volatile ("nop");

    __ASM volatile ("nop");
    __ASM volatile ("nop");
    __ASM volatile ("nop");
    __ASM volatile ("nop");
    __ASM volatile ("nop");

    __ASM volatile ("nop");
    __ASM volatile ("nop");
    __ASM volatile ("nop");
    __ASM volatile ("nop");
    __ASM volatile ("nop");

    __ASM volatile ("nop");
    __ASM volatile ("nop");
    __ASM volatile ("nop");
    __ASM volatile ("nop");
    __ASM volatile ("nop");
    __ASM volatile ("nop");
    __ASM volatile ("nop");
  }
}
