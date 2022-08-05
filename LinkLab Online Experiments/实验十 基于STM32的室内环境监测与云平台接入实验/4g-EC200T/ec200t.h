#ifndef __EC200T_H
#define __EC200T_H	
#include "usart.h"
#include <stm32f10x.h>
#include "delay.h"
void Clear_Buffer(void);	
void EC200T_Init(void);
void EC200T_CreateTCPSokcet(void);
void EC200T_TCPSend(uint8_t *data);
void EC200T_RECData(void);
void Clear_Buffer(void);
void MQTT_Init(void);
u8 Mqttaliyun_Savedata(u8 *t_payload,u8 temp,u8 humi);
void CParsejson(void *data);
void Pub_Msgdata(void);

/*请在此处补充设备三元组信息 */
#define ProductKey     "a1cbf9iLTtp"          
#define DeviceName     "u2XaiGqu2tdGRL8QpR9L"  
#define DeviceSecret   "3f9f91d46a42ce32e3ece2f23b00fd21" 
#define PubTopic       "/sys/a1cbf9iLTtp/u2XaiGqu2tdGRL8QpR9L/thing/event/property/post"
#define SubTopic       "/sys/a1cbf9iLTtp/u2XaiGqu2tdGRL8QpR9L/thing/service/property/set"
/* END */ 

#endif









