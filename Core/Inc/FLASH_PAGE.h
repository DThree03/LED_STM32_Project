/*
 * FLASH_PAGE.h
 *
 *  Created on: Dec 10, 2022
 *      Author: user
 */

#ifndef INC_FLASH_PAGE_H_
#define INC_FLASH_PAGE_H_

#include "stm32f1xx_hal.h"

#define USER_INFO_FLASH_ADDR	0x0801FC00

uint32_t Flash_Write_Data (uint32_t StartPageAddress, uint32_t *Data, uint16_t numberofwords);

void Flash_Read_Data (uint32_t StartPageAddress, uint32_t *RxBuf, uint16_t numberofwords);

#endif /* INC_FLASH_PAGE_H_ */
