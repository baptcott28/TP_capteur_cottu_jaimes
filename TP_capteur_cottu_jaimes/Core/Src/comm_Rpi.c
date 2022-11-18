/*
 * comm_Rpi.c
 *
 *  Created on: Oct 20, 2022
 *      Author: bapti
 */

#include "comm_Rpi.h"
#include "BMP.h"

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;

uint8_t Rx_order_buffer[ORDER_SIZE];
uint8_t set_k[11]="SET_K=1234";
uint32_t temperature=0;
uint32_t pression=0;
int coefK=0;
char *pangle;

void comm_clean_Rx_order_buffer(void){
	for(int i=0;i<ORDER_SIZE+1;i++){
		Rx_order_buffer[i]=0;
	}
}

void comm_Rx_order_buffer_analyse(void){
	if((Rx_order_buffer[0]==71)&&(Rx_order_buffer[1]==69)&&(Rx_order_buffer[2]==84)&&(Rx_order_buffer[3]==95)){
		if((Rx_order_buffer[4]==84)){
			temperature=BMP_get_temperature();
			printf("temperature : %ld\r\n",temperature);
		}
		else if((Rx_order_buffer[4]==80)){
			pression=BMP_get_press();
			printf("pression : %ld\r\n",pression);
		}
		else if((Rx_order_buffer[4]==65)){
			//angle=get_angle()
			printf(pangle);
			printf("reçu GET_A\r\n");
		}
		else if((Rx_order_buffer[4]=75)){
			coefK=get_coef_k();
			printf("Coefficient K : %d\r\n", coefK);
		}
		else{
			printf("commande invalide\r\n");
		}
	}
	else if(Rx_order_buffer[0]==83){
		int rx_index=0;
		for(int i=0;i<11;i++){
			if(Rx_order_buffer[i]!=set_k[i]){
				printf("commande invalide\r\n");
			}
			else{
				rx_index++;
			}
		}
		if(rx_index==10){
			//set_k()
			printf("SET_K=SET_OK\r\n");
		}
		else{
			printf("commande invalide\r\n");
		}
	}
}

void comm_wait_for_order(void){
	printf("entree :\r\n");
	HAL_UART_Receive(&huart2, Rx_order_buffer,ORDER_SIZE,5000);
	printf("sortie : \r\n");
	HAL_UART_Transmit(&huart2, Rx_order_buffer, ORDER_SIZE,100);
	printf("\r\n");
	comm_Rx_order_buffer_analyse();
	comm_clean_Rx_order_buffer();
}
