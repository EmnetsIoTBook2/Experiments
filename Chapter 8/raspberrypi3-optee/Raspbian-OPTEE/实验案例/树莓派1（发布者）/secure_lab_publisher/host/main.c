#include <Lab_CA.h>

char rsa_encryptData[256] = {0}; // the encrypted temperature data
uint32_t rsa_encryptLen = 256; // the length of encrypted data

int Data_Read() {
    TEEC_Result res;
	TEEC_Context ctx;
	TEEC_Session sess;
	TEEC_Operation op;
	TEEC_UUID uuid = SECURE_LAB_UUID_ID;
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
    op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_OUTPUT,
                                    TEEC_VALUE_OUTPUT,
                                    TEEC_NONE,
                                    TEEC_NONE);


    res = TEEC_InvokeCommand(&sess, CMD_PTA_DHT11_READ_DATA, &op,
                                &err_origin);
    if (res != TEEC_SUCCESS) {
        errx(1, "TEEC_InvokeCommand failed with code 0x%x origin 0x%x", res, err_origin);
    }

    printf("\n  start to receive dht11 data\n");
    printf("RH:%d.%d\n", op.params[0].value.a, op.params[0].value.b);
    printf("TMP:%d.%d\n", op.params[1].value.a, op.params[1].value.b);
    
    /*Close the session and destroy the context*/
    TEEC_CloseSession(&sess);
	TEEC_FinalizeContext(&ctx);
}

int Data_ReadandEncrypt() {
    TEEC_Result res;
	TEEC_Context ctx;
	TEEC_Session sess;
	TEEC_Operation data; // the struct of data
	TEEC_UUID uuid = SECURE_LAB_UUID_ID; // the uuid of TA
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
    /* Prepare for the parameter */
    memset(&data, 0, sizeof(data));
    data.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_OUTPUT,
                                    TEEC_NONE,
                                    TEEC_NONE,
                                    TEEC_NONE);
    data.params[0].tmpref.buffer = rsa_encryptData; // prepare for received data
    data.params[0].tmpref.size = rsa_encryptLen;  // prepare for received data
    /* Invoke the secure world function */
    res = TEEC_InvokeCommand(&sess, CMD_RSA_ENC_PKCS1_OPER, // command ID
                        &data, &err_origin); 
    if (res != TEEC_SUCCESS) {
        errx(1, "TEEC_InvokeCommand failed with code 0x%x origin 0x%x", res, err_origin);
    }
    printf("We have successfully sent data!\n");
    
    /*Close the session and destroy the context*/
    TEEC_CloseSession(&sess);
	TEEC_FinalizeContext(&ctx);
}

void Data_Send() {
    char systemCall[1760] = "mosquitto_pub -h 192.168.31.160 -p 8883 -t data --cafile /etc/mosquitto/certs/ca.crt\
                     --cert /etc/mosquitto/certs/clientpub.crt --key /etc/mosquitto/certs/clientpub.key -m \""; 
    char tempbuf[6];
	for (int i = 0; i < rsa_encryptLen; i++) {
		sprintf(tempbuf, "0x%02x, ", rsa_encryptData[i]);
		strncat(systemCall, tempbuf, 6);
	}
    strncat(systemCall, "\"", 1);
    // invoke the system call
	system(systemCall);
}

int main(int argc, char *argv[])
{
    // /*We will use DHT11 sensor to read temperature data*/
    // /*The DHT11 driver is in secure world privileged mode*/
    while (1) {

        // read the data and encrypt it in secure world
        Data_ReadandEncrypt(); 
        
        // send the data to mqtt server
        Data_Send(); 
        
        // delay 3 seconds
        sleep(3);  
    
    }
    
}