/*
 * BMP.h
 *
 *  Created on: Oct 20, 2022
 *      Author: bapti
 */
#include "main.h"
#include <string.h>
#include <stdio.h>
#include "stm32f4xx_hal.h"

#ifndef SRC_BMP_H_
#define SRC_BMP_H_

#define BMP280_ADDR 0x77<<1				// read request
#define BMP280_ADD_ID 0xD0				// Id adress
#define BMP280_ADD_CONFIG 0xF4			// config adress
#define BMP280_ADD_CALIBRATION 0x88		// first trimming bit adress
#define BMP280_ADD_TEMP 0xFA
#define BMP280_ADD_PRESS 0xF7

#define MODE_NORMAL 0b11
#define PRESSION 0b101
#define TEMPERATURE 0b010
#define CONFIG 0x57
#define CALIB_PARAM 25

extern I2C_HandleTypeDef hi2c1;

void BMP_get_ID(void);
void BMP_send_Configuration(void);
void BMP_get_calibration_temp_press(void);
uint32_t BMP_get_temperature(void);
uint32_t BMP_get_press(void);

#endif /* SRC_BMP_H_ */
