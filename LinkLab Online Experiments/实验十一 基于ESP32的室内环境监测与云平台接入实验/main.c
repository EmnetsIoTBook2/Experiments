/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

#include "linkkit/dev_sign_api.h"
#include "linkkit/mqtt_api.h"
#include "linkkit/wrappers/wrappers.h"
#include "linkkit/infra/infra_compat.h"
#include "app_entry.h"


/*请在此处补充设备三元组信息 */
#define PRODUCT_KEY             "a1cbf9iLTtp"
#define PRODUCT_SECRET          "qujzmbG0vrossASS"  
#define DEVICE_NAME             "u2XaiGqu2tdGRL8QpR9L"
#define DEVICE_SECRET           "3f9f91d46a42ce32e3ece2f23b00fd21"
/* END */


#define TOPIC_PROPERTY_POST      "/sys/"PRODUCT_KEY"/"DEVICE_NAME"/thing/event/property/post"

#define MQTT_MSGLEN             (1024)

#define EXAMPLE_TRACE(fmt, ...)  \
    do { \
        HAL_Printf("%s|%03d :: ", __func__, __LINE__); \
        HAL_Printf(fmt, ##__VA_ARGS__); \
        HAL_Printf("%s", "\r\n"); \
    } while(0)

static int      user_argc;
static char   **user_argv;


int mqtt_client(void)
{
    int rc, msg_len, cnt = 0, ts;
    void *pclient;
    iotx_conn_info_pt pconn_info;
    iotx_mqtt_param_t mqtt_params;
    iotx_mqtt_topic_info_t topic_msg;
    char msg_pub[512];

    /* Device AUTH */
    if (0 != IOT_SetupConnInfo(PRODUCT_KEY, DEVICE_NAME, DEVICE_SECRET, (void **)&pconn_info)) {
        EXAMPLE_TRACE("AUTH request failed!");
        return -1;
    }

    /* Initialize MQTT parameter */
    memset(&mqtt_params, 0x0, sizeof(mqtt_params));

    mqtt_params.port = pconn_info->port;
    mqtt_params.host = pconn_info->host_name;
    mqtt_params.client_id = pconn_info->client_id;
    mqtt_params.username = pconn_info->username;
    mqtt_params.password = pconn_info->password;
    mqtt_params.pub_key = pconn_info->pub_key;

    mqtt_params.request_timeout_ms = 2000;
    mqtt_params.clean_session = 0;
    mqtt_params.keepalive_interval_ms = 60000;
    mqtt_params.read_buf_size = MQTT_MSGLEN;
    mqtt_params.write_buf_size = MQTT_MSGLEN;

    mqtt_params.handle_event.pcontext = NULL;

    pclient = IOT_MQTT_Construct(&mqtt_params);
    if (NULL == pclient) {
        EXAMPLE_TRACE("MQTT construct failed");
        return -1;
    }

    HAL_SleepMs(2000);
    
    /*请补充以下循环体代码，实现通过MQTT协议循环上报数据到云平台的功能，循环次数尚未定义 */
    while (cnt < 5) {
        cnt++;

        /*请在此处添加自己的代码，把待发送的MQTT数据写入msg_pub,把消息长度写入msg_len，属性和主题参考左侧题目描述，属性值自定义 */
        int ts = aos_now_ms();
				msg_len = snprintf(msg_pub, sizeof(msg_pub), "{"
				"\"id\": \"123\","
				"\"version\": \"1.0\","
				"\"params\": {"
				"\"CurrentTemperature\": {"
				"\"value\": 19.0"
				"},"
				"\"CurrentHumidity\": {"
				"\"value\": 17.0"
				"},"
				"},"
				"\"method\": \"thing.event.property.post\""
				"}");
        /* END */

        topic_msg.payload = (void *)msg_pub;
        topic_msg.payload_len = msg_len;


        /*请在此处添加自己的代码，把topic_msg通过MQTT协议发送出去 */
         rc = IOT_MQTT_Publish(pclient, TOPIC_PROPERTY_POST, &topic_msg);
        /* END */
        
        EXAMPLE_TRACE("packet-id=%lu, publish topic msg=%s", (uint32_t)rc, msg_pub);
        IOT_MQTT_Yield(pclient, 200);

        /*请在此处添加自己的代码，使程序暂停一段时间 */
         aos_msleep(1000);
        /* END */
    } 

    IOT_MQTT_Yield(pclient, 2000);

    IOT_MQTT_Destroy(&pclient);

    return 0;
}

int linkkit_main(void *paras)
{
    IOT_SetLogLevel(IOT_LOG_DEBUG);

    user_argc = 0;
    user_argv = NULL;

    if (paras != NULL) {
        app_main_paras_t *p = (app_main_paras_t *)paras;
        user_argc = p->argc;
        user_argv = p->argv;
    }

    HAL_SetProductKey(PRODUCT_KEY);
    HAL_SetDeviceName(DEVICE_NAME);
    HAL_SetDeviceSecret(DEVICE_SECRET);
    HAL_SetProductSecret(PRODUCT_SECRET);
    /* Choose Login Server */
    int domain_type = IOTX_CLOUD_REGION_SHANGHAI;
    IOT_Ioctl(IOTX_IOCTL_SET_DOMAIN, (void *)&domain_type);

    /* Choose Login  Method */
    int dynamic_register = 0;
    IOT_Ioctl(IOTX_IOCTL_SET_DYNAMIC_REGISTER, (void *)&dynamic_register);

    mqtt_client();
    IOT_DumpMemoryStats(IOT_LOG_DEBUG);
    IOT_SetLogLevel(IOT_LOG_NONE);

    EXAMPLE_TRACE("out of sample!");
    printf("\nend\n");

    while (1) {aos_msleep(5000);}

    return 0;
}
