#include <Lab_CA.h>
#include "MQTTClient.h"

#define ADDRESS     "ssl://192.168.31.160:8883" // ip:port
#define CLIENTID    "ExampleClientSub"   // Client ID
#define TOPIC       "data"               // topic
#define PAYLOAD     "123123"
#define QOS         0                   // at most once
#define TIMEOUT     10000L

#define ENCRYPTED_DATA_LEN 256 // the encrypted data length
char encrypted_data[ENCRYPTED_DATA_LEN] = ""; // the encrypted data
#define DECRYPTED_DATA_LEN 256 // the decrypted data max length
char decrypted_data[DECRYPTED_DATA_LEN] = ""; // the decrypted data

/****************************************
* The received data format is hex.
* We need to convert the format to string
*****************************************/
void ByteOrNum(char c, int *temp) {
	if (c >= 'a' && c <= 'f') {
		*temp += 10 + c - 'a';
	} else if (c >= '0' && c <= '9') {
		*temp += c - '0';
	}
}
void hex2str(char* str) {
	int len = strlen(str);
	int j = 0;
	char tempChar[1];
	for (int i = 0; i < len; i+=6) {
		int temp =  0;
		ByteOrNum(str[i + 2], &temp);
		temp *= 16;
		ByteOrNum(str[i + 3], &temp);
		sprintf(tempChar, "%c", temp);
		encrypted_data[j++] = tempChar[0];
	}
}

/*
*  Decrypt received data
*/
void Data_Decrypt(char* received_data) {
    // printf("The data received is below: %s\n", received_data);
    hex2str(received_data);

    TEEC_Result res;
	TEEC_Context ctx;
	TEEC_Session sess;
	TEEC_Operation op;
	TEEC_UUID uuid = SECURE_LAB_SUB_UUID_ID;
	uint32_t err_origin;
    
    /* Initialize a context connecting us to the TEE */
    res = TEEC_InitializeContext(NULL, &ctx);
	if (res != TEEC_SUCCESS) {
		errx(1, "TEEC_InitializeContext failed with code 0x%x", res);
    }

	res = TEEC_OpenSession(&ctx, &sess, &uuid,
			       TEEC_LOGIN_PUBLIC, NULL, NULL, &err_origin);
	if (res != TEEC_SUCCESS) {
		errx(1, "TEEC_Opensession failed with code 0x%x origin 0x%x", res, err_origin);
    }
    
    /* Invoke the secure world function */
    memset(&op, 0, sizeof(op));
    op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT,
                                    TEEC_MEMREF_TEMP_OUTPUT,
                                    TEEC_NONE,
                                    TEEC_NONE);
    op.params[0].tmpref.buffer = encrypted_data;
    op.params[0].tmpref.size = ENCRYPTED_DATA_LEN;
    op.params[1].tmpref.buffer = decrypted_data;
    op.params[1].tmpref.size = DECRYPTED_DATA_LEN;
    
    res = TEEC_InvokeCommand(&sess, CMD_RSA_DEC_PKCS1_OPER, &op,
                                &err_origin);
    if (res != TEEC_SUCCESS) {
        errx(1, "TEEC_InvokeCommand failed with code 0x%x origin 0x%x", res, err_origin);
    }
    printf("The decrypt data is :\n%s\n", decrypted_data);
    /*Close the session and destroy the context*/
    TEEC_CloseSession(&sess);
	TEEC_FinalizeContext(&ctx);
    return ;
}

/****************************************
* The mqtt functions
*****************************************/
volatile MQTTClient_deliveryToken deliveredtoken;

/* deliver func */
void delivered(void *context, MQTTClient_deliveryToken dt)
{
    printf("Message with token value %d delivery confirmed\n", dt);
    deliveredtoken = dt;
}
/* received message */
int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
    Data_Decrypt((char*)message->payload);
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}
/* connect lost */
void connlost(void *context, char *cause)
{
    printf("\nConnection lost\n");
    printf("cause: %s\n", cause);
}

void mqtt_sub() {
    MQTTClient client; 
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer; // connect options
    MQTTClient_SSLOptions ssl_opts = MQTTClient_SSLOptions_initializer;  // SSL options
	int result;
    // create mqtt client
    result = MQTTClient_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
    if (result != MQTTCLIENT_SUCCESS) {
        printf("Failed to create client, return code %d\n", result);
        MQTTClient_destroy(&client);
    }
    // set callback functions
    result = MQTTClient_setCallbacks(client, NULL, connlost, msgarrvd, delivered);
    if (result != MQTTCLIENT_SUCCESS) {
        printf("Failed to set callbacks, return code %d\n", result);
        MQTTClient_destroy(&client);
    }
    // set SSL certs
    conn_opts.ssl = &ssl_opts;
  	conn_opts.ssl->trustStore = "/etc/mosquitto/certs/ca.crt";
    conn_opts.ssl->privateKey = "/etc/mosquitto/certs/clientsub.key";             
  	conn_opts.ssl->keyStore = "/etc/mosquitto/certs/clientsub.crt";                  
    conn_opts.ssl->enableServerCertAuth = 1;                        
    conn_opts.ssl->verify = 1; 
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;
    // connect MQTT broker
    result = MQTTClient_connect(client, &conn_opts);
    if (result != MQTTCLIENT_SUCCESS) {
        printf("Failed to connect, return code %d\n", result);
        MQTTClient_destroy(&client);
	return;
    }

    printf("Subscribing to topic \"%s\"\nfor client \"%s\" using QoS%d\n\n"
           "Press Q<Enter> to quit\n\n", TOPIC, CLIENTID, QOS);
    // subscribe MQTT topic
    result = MQTTClient_subscribe(client, TOPIC, QOS);
    if (result != MQTTCLIENT_SUCCESS) {
    	printf("Failed to subscribe, return code %d\n", result);
    } else {
    	int ch;
    	do {
        	ch = getchar();
    	} while (ch != 'Q' && ch != 'q'); // Press Q<Enter> to quit
        // unsubscribe the topic
        result = MQTTClient_unsubscribe(client, TOPIC);
        if (result != MQTTCLIENT_SUCCESS) {
        	printf("Failed to unsubscribe, return code %d\n", result);
        }
    }
    // disconnect to broker
    result = MQTTClient_disconnect(client, 10000);
    if (result != MQTTCLIENT_SUCCESS) {
    	printf("Failed to disconnect, return code %d\n", result);
    }

}

int main(int argc, char* argv[])
{
    // subscribe the mqtt topic
    mqtt_sub();
    
    return 0;
}
