/*
 * comm_Rpi.h
 *
 *  Created on: Oct 20, 2022
 *      Author: bapti
 */

#include "main.h"
#include <string.h>
#include <stdio.h>
#include "stm32f4xx_hal.h"


#ifndef INC_COMM_RPI_H_
#define INC_COMM_RPI_H_

#define ORDER_SIZE 12

#define TEMP_SIZE 20
#define PRESS_SIZE 20
#define ANGLE_SIZE 20
#define COEFK_SIZE 20

extern UART_HandleTypeDef huart1;

void comm_Rx_order_buffer_analyse(void);
void comm_wait_for_order(void);
void comm_clean_Rx_order_buffer(void);

#endif /* INC_COMM_RPI_H_ */
