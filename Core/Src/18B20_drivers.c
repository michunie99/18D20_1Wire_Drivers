/*
 * 18B20_drivers.c
 *
 *  Created on: Oct 9, 2021
 *      Author: michu
 */
#include "18B20_drivers.h"

//Should it be like this ???
#define DQ_Pin GPIO_PIN_15
#define DQ_GPIO_Port GPIOB

//instructions
#define CONVERT_T 0x44
#define READ_SCRATCH 0xBE
#define READ_ROM 0x33
#define SKIP_ROM  0xCC

//times for communication
#define MASTER_REST 550
#define SLAVE_PRESENCE 80
#define COMMUNICATION_START_STOP 15
#define MASTER_WRITE_ZERO 100
#define MASTER_READ 45

extern TIM_HandleTypeDef htim1;

uint8_t reset(void){
	uint8_t result;
	HAL_GPIO_WritePin(DQ_GPIO_Port, DQ_Pin, 0);
	delay_us(MASTER_REST);
	HAL_GPIO_WritePin(DQ_GPIO_Port, DQ_Pin, 1);
	delay_us(SLAVE_PRESENCE);
	result = HAL_GPIO_ReadPin(DQ_GPIO_Port, DQ_Pin);
	delay_us(MASTER_REST - SLAVE_PRESENCE);
	return result;
}

void delay_us(uint16_t delay){
	__HAL_TIM_SET_COUNTER(&htim1,0);
	while(__HAL_TIM_GET_COUNTER(&htim1) < delay);
}

void send_zero(void){
	HAL_GPIO_WritePin(DQ_GPIO_Port, DQ_Pin, 0);
	delay_us(MASTER_WRITE_ZERO);
	HAL_GPIO_WritePin(DQ_GPIO_Port, DQ_Pin, 1);
	delay_us(COMMUNICATION_START_STOP);
}

void send_one(void){
	HAL_GPIO_WritePin(DQ_GPIO_Port, DQ_Pin, 0);
	delay_us(COMMUNICATION_START_STOP);
	HAL_GPIO_WritePin(DQ_GPIO_Port, DQ_Pin, 1);
	delay_us(60 - COMMUNICATION_START_STOP);
}

void send_byte(uint8_t byte){
	uint8_t i = 8;
	while(i){
		if(byte & 1)
			send_one();
		else
			send_zero();
		byte >>= 1;
		i--;
	}
}

uint8_t read_byte(void){
	uint8_t data = 0, i = 0;
	while(i < 8){
		HAL_GPIO_WritePin(DQ_GPIO_Port, DQ_Pin, 0);
		delay_us(5);
		HAL_GPIO_WritePin(DQ_GPIO_Port, DQ_Pin, 1);
		delay_us(COMMUNICATION_START_STOP);
		data += (1 << i)*HAL_GPIO_ReadPin(DQ_GPIO_Port, DQ_Pin);
		delay_us(MASTER_READ);
		i++;
	}
	return data;
}

float read_temp(void){
	uint8_t is_ok;
	uint16_t temp;
	float temp_dec;
	//Request temperature
	is_ok = reset();
	send_byte(SKIP_ROM);
	send_byte(CONVERT_T);
	HAL_GPIO_WritePin(DQ_GPIO_Port, DQ_Pin, 1);
	HAL_Delay(500);
	//read data
	is_ok = reset();
	send_byte(SKIP_ROM);
	send_byte(READ_SCRATCH);
	temp = 0;
	temp += read_byte();
	temp += ((uint16_t) read_byte()) << 8;

	temp_dec = ((float) (temp & ~(0x1f << 11)))/16 * ((temp & (0x1f << 11)) ? -1 : 1);
	return temp_dec;
}

uint64_t read_rom(void){
	uint8_t is_ok, i = 8;
	uint64_t code = 0;
	is_ok = reset();
	send_byte(READ_ROM);
	while(i--){
		code += read_byte();
		if(i != 1)
			code <<= 8;
	}
	return code;
}

