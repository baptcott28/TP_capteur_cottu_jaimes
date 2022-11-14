/*
 * motor.c
 *
 *  Created on: Nov 14, 2022
 *      Author: bapti
 */

#include "motor.h"

uint8_t aData[MOTOR_MODE_AUTO];			//<! buffer de data
CAN_TxHeaderTypeDef pHeader;			//<! Header de la trame CAN
uint32_t pTxMailbox;					//<! Mailbox de gestion de l'envoie des données

uint8_t motor_CAN_Init_Start(void){
	pHeader.StdId=STD_ID;
	pHeader.IDE=CAN_ID_STD;
	pHeader.RTR=CAN_RTR_DATA;
	pHeader.DLC=MOTOR_MODE_AUTO;
	pHeader.TransmitGlobalTime=DISABLE;

	if(HAL_CAN_Start(&hcan1)==HAL_OK){
		return 1;
	}
	else{
		printf("\r\nProblème :\r\nHAL_CAN_Start()");
		return 0;
	}
}

uint8_t motor_tourne(uint8_t angular_position, uint8_t rotation_direction){
	aData[0]=angular_position;
	aData[1]=rotation_direction;
	if(HAL_CAN_AddTxMessage(&hcan1, &pHeader, aData, &pTxMailbox)==HAL_OK){
		return 1;
	}
	else{
		printf("Problème :\r\n Pas de transmission de trame CAN\r\n");
		return 0;
	}
}

