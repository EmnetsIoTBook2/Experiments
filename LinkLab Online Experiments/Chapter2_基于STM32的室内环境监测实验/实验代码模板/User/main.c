#include "stm32f10x.h"  
#include "dht11.h"
#include "usart.h"
#include "delay.h"

int main(void)
{
		DHT11_Data_TypeDef DHT11_Data;
	/*请在此处添加代码，初始化串口、初始化DHT11引脚 */

	while(1)
	{	
			if() /*请在此处添加自己的代码，	调用DHT11_Read_TempAndHumidity读取温湿度，判断是否读取成功*/ 
			{
				  /*请在此处添加自己的代码，把温湿度数值打印出来 */
			}			
			else
			{
				printf("Read DHT11 ERROR!\r\n");
			}
			delay_ms(20000);
			
	}
}

