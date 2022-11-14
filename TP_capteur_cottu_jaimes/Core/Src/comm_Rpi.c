/*
 * comm_Rpi.c
 *
 *  Created on: Oct 20, 2022
 *      Author: bapti
 */

#include "comm_Rpi.h"
#include "BMP.h"

uint8_t Rx_order_buffer[ORDER_SIZE];
uint8_t set_k[11]="SET_K=1234";
uint32_t temp=0;
uint32_t press=0;
char *ptemp;
char *ppress;
char *pangle;
char *pcoefK;

void clean_Rx_order_buffer(void){
	for(int i=0;i<ORDER_SIZE+1;i++){
		Rx_order_buffer[i]=0;
	}
}

void Rx_order_buffer_analyse(void){
	if((Rx_order_buffer[0]==71)&&(Rx_order_buffer[1]==69)&&(Rx_order_buffer[2]==84)&&(Rx_order_buffer[3]==95)){
		if((Rx_order_buffer[4]==84)){
			temp=BMP_get_temperature();
			ptemp=(char*)&temp;
			printf(ptemp);
			printf("recu GET_T\r\n");
		}
		else if((Rx_order_buffer[4]==80)){
			press=BMP_get_press();
			ppress=(char*)&press;
			printf(ppress);
			printf("reçu GET_P\r\n");
		}
		else if((Rx_order_buffer[4]==65)){
			//angle=get_angle()
			printf(pangle);
			printf("reçu GET_A\r\n");
		}
		else if((Rx_order_buffer[4]=75)){
			//coefK=get_coefK()
			printf(pcoefK);
			printf("reçu GET_K\r\n");
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

void wait_for_order(void){
	HAL_UART_Receive(&huart1, Rx_order_buffer,ORDER_SIZE,5000);
	Rx_order_buffer_analyse();
	clean_Rx_order_buffer();
}
