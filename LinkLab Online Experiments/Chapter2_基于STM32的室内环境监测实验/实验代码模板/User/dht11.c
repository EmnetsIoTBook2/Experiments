#include "dht11.h"
#include "stm32f10x.h"  
#include "delay.h"
#define      DHT11_Dout_IN()	                          GPIO_ReadInputDataBit ( GPIOE, GPIO_Pin_6 )
#define      DHT11_Dout_0	                            GPIO_ResetBits (GPIOE,GPIO_Pin_6) 
#define      DHT11_Dout_1	                            GPIO_SetBits (GPIOE,GPIO_Pin_6) 


void DHT11_GPIO_Config ( void )
{		
		GPIO_InitTypeDef GPIO_InitStructure; 
		RCC_APB2PeriphClockCmd ( RCC_APB2Periph_GPIOE, ENABLE );												   
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;	
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  	GPIO_Init ( GPIOE, &GPIO_InitStructure );		  
	
}
void DHT11_Mode_IPU(void)
{
 	  GPIO_InitTypeDef GPIO_InitStructure;
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU ; 
	  GPIO_Init(GPIOE, &GPIO_InitStructure);	 
	
}
void DHT11_Mode_Out_PP(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;												   
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;	
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_Init(GPIOE, &GPIO_InitStructure);	 	 
	
}
uint8_t DHT11_ReadByte ( void )
{
  /*请在此处添加代码，从DHT11读取一个字节*/ 
}
uint8_t DHT11_Read_TempAndHumidity(DHT11_Data_TypeDef *DHT11_Data)
{  
 /*请在此处添加代码，一次完整的数据传输为40bit，高位先出 8bit 湿度整数 + 8bit 湿度小数 + 8bit 温度整数 + 8bit 温度小数 + 8bit 校验 */ 
}
