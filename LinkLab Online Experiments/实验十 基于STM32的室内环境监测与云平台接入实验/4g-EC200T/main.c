#include "stm32f10x.h"
#include "delay.h"
#include "usart.h"
#include "wdg.h"
#include "ec200t.h"
#include "stdlib.h"
#include "string.h"
#include "stdio.h"
#include "math.h"
#include "dht11.h"
int main(void)
{

	delay_init();
	uart_init(115200);
	uart2_init(115200);
	DHT11_Init();
	IWDG_Init(7, 625);
	EC200T_Init();
	MQTT_Init();
	while (1)
	{
		Pub_Msgdata();
		delay_ms(500);
		IWDG_Feed();
	}
}
