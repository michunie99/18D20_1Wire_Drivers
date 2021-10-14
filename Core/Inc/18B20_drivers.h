/*
 * 18D20_drivers.h
 *
 *  Created on: Oct 9, 2021
 *      Author: michu
 */

#ifndef INC_18B20_DRIVERS_H_
#define INC_18B20_DRIVERS_H_

#include "stm32f4xx_hal.h"
#include "stdint.h"
#include <stdio.h>



void delay_us(uint16_t delay);
void send_byte(uint8_t byte);
uint8_t read_byte(void);
uint8_t reset(void);
float read_temp(void);
uint64_t read_rom(void);

#endif /* INC_18B20_DRIVERS_H_ */
