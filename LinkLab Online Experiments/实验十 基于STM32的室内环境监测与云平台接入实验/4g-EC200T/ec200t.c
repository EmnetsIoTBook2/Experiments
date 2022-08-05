#include "ec200t.h"
#include "string.h"
#include "usart.h"
#include "wdg.h"
#include "common.h"
#include "dht11.h"
#define EC200T_Usart(fmt, ...) USART_printf(USART1, fmt, ##__VA_ARGS__)
DHT11_Data_TypeDef DHT11_Data;

char *strx, *extstrx;
extern char RxBuffer[250]; //接收缓冲,
extern u8 RxCounter;
void Proscess1(void *data);
void Clear_Buffer(void) //清空缓存
{
  u8 i;
  Uart1_SendStr(RxBuffer);
  for (i = 0; i < 250; i++)
    RxBuffer[i] = 0; //缓存
  RxCounter = 0;
  IWDG_Feed(); //喂狗
}
void EC200T_Init(void)
{
  Uart1_SendStr("本次测试MQTT发数据到阿里云平台!\r\n");
  delay_ms(300);
  printf("AT\r\n");
  delay_ms(300);
  strx = strstr((const char *)RxBuffer, (const char *)"OK"); //返回OK
  Clear_Buffer();
  while (strx == NULL)
  {
    Clear_Buffer();
    printf("AT\r\n");
    delay_ms(300);
    strx = strstr((const char *)RxBuffer, (const char *)"OK"); //返回OK
  }
  printf("ATE0&W\r\n"); //关闭回显
  delay_ms(300);
  printf("AT+CIMI\r\n"); //获取卡号，类似是否存在卡的意思，比较重要。
  delay_ms(300);
  strx = strstr((const char *)RxBuffer, (const char *)"460"); //返460，表明识别到卡了
  Clear_Buffer();
  while (strx == NULL)
  {
    Clear_Buffer();
    printf("AT+CIMI\r\n"); //获取卡号，类似是否存在卡的意思，比较重要。
    delay_ms(300);
    strx = strstr((const char *)RxBuffer, (const char *)"460"); //返回OK,说明卡是存在的
  }

  printf("AT+CREG?\r\n"); //查看是否注册GSM网络
  delay_ms(500);
  strx = strstr((const char *)RxBuffer, (const char *)"+CREG: 0,1");    //返回正常
  extstrx = strstr((const char *)RxBuffer, (const char *)"+CREG: 0,5"); //返回正常，漫游
  while (strx == NULL && extstrx == NULL)
  {
    Clear_Buffer();
    printf("AT+CREG?\r\n"); //查看是否注册GSM网络
    delay_ms(500);
    strx = strstr((const char *)RxBuffer, (const char *)"+CREG: 0,1");    //返回正常
    extstrx = strstr((const char *)RxBuffer, (const char *)"+CREG: 0,5"); //返回正常，漫游
  }
  Clear_Buffer();
  /////////////////////////////////////
  printf("AT+CGREG?\r\n"); //查看是否注册GPRS网络
  delay_ms(500);
  strx = strstr((const char *)RxBuffer, (const char *)"+CGREG: 0,1");    //，这里重要，只有注册成功，才可以进行GPRS数据传输。
  extstrx = strstr((const char *)RxBuffer, (const char *)"+CGREG: 0,5"); //返回正常，漫游
  while (strx == NULL && extstrx == NULL)
  {
    Clear_Buffer();
    printf("AT+CGREG?\r\n"); //查看是否注册GPRS网络
    delay_ms(500);
    strx = strstr((const char *)RxBuffer, (const char *)"+CGREG: 0,1");    //，这里重要，只有注册成功，才可以进行GPRS数据传输。
    extstrx = strstr((const char *)RxBuffer, (const char *)"+CGREG: 0,5"); //返回正常，漫游
  }
  Clear_Buffer();
  printf("AT+COPS?\r\n"); //查看注册到哪个运营商，支持移动 联通 电信
  delay_ms(500);
  Clear_Buffer();
  printf("AT+QMTDISC=0\r\n"); //关闭socket连接
  delay_ms(500);
  Clear_Buffer();
  printf("AT+QICSGP=1,1,\"CMNET\",\"\",\"\",0\r\n"); //接入APN，无用户名和密码
  delay_ms(500);
  Clear_Buffer();
  printf("AT+QIACT=1\r\n"); //激活
  delay_ms(500);
  Clear_Buffer();
  printf("AT+QIACT?\r\n"); //获取当前卡的IP地址
  delay_ms(500);
  Clear_Buffer();
}
void MQTT_Init(void)
{
  printf("AT+QMTCFG=\"aliauth\",0,\"%s\",\"%s\",\"%s\"\r\n", ProductKey, DeviceName, DeviceSecret);
  delay_ms(300);
  printf("AT+QMTOPEN=0,\"iot-as-mqtt.cn-shanghai.aliyuncs.com\",1883\r\n"); //通过TCP方式去连接MQTT阿里云服务器
  delay_ms(300);
  strx = strstr((const char *)RxBuffer, (const char *)"+QMTOPEN: 0,0"); //看下返回状态
  while (strx == NULL)
  {
    strx = strstr((const char *)RxBuffer, (const char *)"+QMTOPEN: 0,0"); //确认返回值正确
  }
  Uart1_SendStr("连接阿里云平台成功\r\n");
  Clear_Buffer();
  printf("AT+QMTCONN=0,\"4G_IOT_22\"\r\n"); //去登录MQTT服务器
  delay_ms(300);
  strx = strstr((const char *)RxBuffer, (const char *)"+QMTCONN: 0,0,0"); //看下返回状态
  while (strx == NULL)
  {
    strx = strstr((const char *)RxBuffer, (const char *)"+QMTCONN: 0,0,0"); //看下返回状态
  }
  Uart1_SendStr("登录阿里云平台成功\r\n");
  Clear_Buffer();
}


//访问阿里云需要提交JSON数据
u8 Mqttaliyun_Savedata(u8 *t_payload, u8 temp, u8 humi)
{

/*请补充以下循环体代码，实现通过MQTT协议循环上报数据到云平台的功能，循环次数尚未定义 */
char json[] = "{\"id\":\"26\",\"version\":\"1.0\",\"params\":{\
                  \"CurrentTemperature\":{\"value\":%d},\
                  \"CurrentHumidity\":{\"value\":%d}},\
                  \"method\":\
                  \"thing.event.property.post\"}"; 
/* END */ 

  char t_json[200];
  unsigned short json_len;
  sprintf(t_json, json, temp, humi);

  json_len = strlen(t_json) / sizeof(char);
  memcpy(t_payload, t_json, json_len);
  return json_len;
}


//发布数据到阿里云显示
void Pub_Msgdata(void)
{
  char *strx;
  u8 data_len, datastr[10], send_jason[200];
  DHT11_Read_TempAndHumidity(&DHT11_Data);

  data_len = Mqttaliyun_Savedata(send_jason, (int)DHT11_Data.temp_int, (int)DHT11_Data.humi_int);
  sprintf((char *)datastr, "%d", data_len);
  printf("AT+QMTPUBEX=0,0,0,0,\"%s\",%s\r\n", PubTopic, datastr); //发布消息
  delay_ms(200);
  Uart2_SendDATA(send_jason, data_len);
  delay_ms(200);

  strx = strstr((const char *)RxBuffer, (const char *)"+QMTPUBEX: 0,0,0"); //发布成功   +QMTPUBEX: 0,0,0
  while (strx == NULL)
  {
    strx = strstr((const char *)RxBuffer, (const char *)"+QMTPUBEX: 0,0,0");
  }
  EC200T_Usart("阿里云平台消息发布成功\r\n");
  EC200T_Usart("当前温度：%d℃  湿度：%d％RH\r\n", (int)DHT11_Data.temp_int, (int)DHT11_Data.humi_int);
  EC200T_Usart("\r\n");
  delay_ms(1000);
}
