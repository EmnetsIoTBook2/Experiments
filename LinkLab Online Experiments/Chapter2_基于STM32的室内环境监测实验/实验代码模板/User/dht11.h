#ifndef _DHT11_H
#define _DHT11_H
#include "stm32f10x.h"
typedef struct
{
	uint8_t  humi_int;		
	uint8_t  humi_deci;	 	
	uint8_t  temp_int;	 	
	uint8_t  temp_deci;	 	
	uint8_t  check_sum;	 	
		                 
} DHT11_Data_TypeDef;


void DHT11_GPIO_Config ( void );
uint8_t DHT11_Read_TempAndHumidity ( DHT11_Data_TypeDef * DHT11_Data );
#endif
