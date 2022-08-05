#include "sys.h"
#include "usart.h"
#include "stdio.h"
#include "string.h"
#include "stm32f10x_tim.h"
unsigned char uart1_getok;
//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB
#if 1
#pragma import(__use_no_semihosting)
//标准库需要的支持函数
struct __FILE
{
	int handle;
};

FILE __stdout;
//定义_sys_exit()以避免��用半主机模式
void _sys_exit(int x)
{
	x = x;
}
void _ttywrch(int ch)
{
	ch = ch;
}
//重定义fputc函数
// int fputc(int ch, FILE *f)
// {
// 	while((USART2->SR&0X40)==0);//循环发送,直到发送完毕
//     USART2->DR = (u8) ch;
// 	return ch;
// }
#ifdef __GNUC__
// int _write(int fd, char *ptr, int len)
// {
// 	Usart_SendArray(DEBUG_USARTx, ptr, len);
// 	return len;
// }

int _write(int fd, char *pBuffer, int size)
{
	for (int i = 0; i < size; i++)
	{

		while ((USART2->SR & 0X40) == 0)
			; //循环发送,直到发送完毕
		USART2->DR = (u8)(pBuffer[i]);
	}
	return size;
}

#else

int fputc(int ch, FILE *f)
{
	while ((USART2->SR & 0X40) == 0)
		; //循环发送,直到发送完毕
	USART2->DR = (u8)ch;
	return ch;
}

#endif

#endif

void UART1_send_byte(char data)
{
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
		;
	USART_SendData(USART1, data);
}
void UART2_send_byte(char data)
{
	while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET)
		;
	USART_SendData(USART2, data);
}

char RxBuffer[250]; //接收缓冲,
u8 RxCounter;
unsigned char RxCounter1, RxBuffer1[200]; //接收缓冲
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART_RX_STA = 0; //接收状态标记
extern u8 Timeout;
//初始化IO 串口1
//bound:波特率
void uart_init(u32 bound)
{
	//GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE); //使能USART1，GPIOA时钟
	USART_DeInit(USART1);														  //复位串口1
																				  //USART1_TX   PA.9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;									  //PA.9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);			//初始化PA9

	//USART1_RX	  PA.10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //浮空输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);		  //初始化PA10

	//Usart1 NVIC 配置

	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; //抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		  //子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			  //IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);							  //根据指定的参数初始化VIC寄存器

	//USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;										//一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;						//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;							//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;								//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//收发模式

	USART_Init(USART1, &USART_InitStructure);	   //初始化串口
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); //开启接收中断
	USART_Cmd(USART1, ENABLE);					   //使能串口
}
//初始化IO 串2
//bound:波特率
void uart2_init(u32 bound)
{
	//GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  //使能，GPIOA时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE); //USART2
	USART_DeInit(USART2);								   //复位串口2
														   //USART2_TX   PA.2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;			   //PA.2
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);			//初始化PA2

	//USART2_RX	  PA.3
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //浮空输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);				  //初始化PA3

	//Usart1 NVIC 配置

	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; //抢占优先级0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		  //子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			  //IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);							  //根据指定的参数初始化VIC寄存器

	//USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;										//115200
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;						//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;							//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;								//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//收发模式
	USART_Init(USART2, &USART_InitStructure);										//初始化串口

	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); //开启中断
	USART_Cmd(USART2, ENABLE);					   //使能串口
}

void Uart1_SendStr(char *SendBuf) //串口1打印数据
{
	while (*SendBuf)
	{
		while ((USART1->SR & 0X40) == 0)
			; //等待发送完成
		USART1->DR = (u8)*SendBuf;
		SendBuf++;
	}
}

void Uart2_SendDATA(u8 *SendBuf, u8 len) //串口2打印数据
{
	u8 i;
	for (i = 0; i < len; i++)
	{
		while ((USART2->SR & 0X40) == 0)
		{
		} //等待发送完成
		USART2->DR = (u8)SendBuf[i];
	}
}

void USART1_IRQHandler(void) //串口1中断服务程序
{
	volatile char Res;
	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) //接收中断，可以扩展来控制
	{
		Res = USART_ReceiveData(USART1); //接收模块的数据;
	}
}

void USART2_IRQHandler(void) //串口2中断服务程序
{
	volatile char Res;
	if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) //接收模块返回的数据
	{

		RxBuffer[RxCounter++] = USART_ReceiveData(USART2); //接收模块的数据
	}
}
