/*
 * motor.h
 *
 *  Created on: Nov 14, 2022
 *      Author: bapti
 */

#ifndef SRC_MOTOR_H_
#define SRC_MOTOR_H_

#include "main.h"
#include <stdio.h>
#include <stdlib.h>

#define MOTOR_MODE_AUTO 2			//<! Mode automatique : data[position_absolue, sens_rotation]
#define ANGLE_POSITIVE 0x00
#define ANGLE_NEGATIVE 0x01
#define ANGLE_90 0x5A
#define ANGLE_UNITE 0x01

#define STD_ID 0x61					//<! Motor_ID in automatic mode

//liaison avec la temperature
#define TEMPERATURE_REF 8311
#define COEF_K 1000

extern CAN_HandleTypeDef hcan1;

uint8_t motor_CAN_Init_Start(void);
uint8_t motor_tourne(uint8_t _angular_position, uint8_t rotation_direction);
void motor_handle(void);

#endif /* SRC_MOTOR_H_ */
