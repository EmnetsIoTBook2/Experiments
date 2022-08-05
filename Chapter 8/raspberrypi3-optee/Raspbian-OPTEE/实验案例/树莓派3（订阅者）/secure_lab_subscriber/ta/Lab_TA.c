#include <Lab_TA.h>


/*
********************************************************************
*                           RSA public key
*    The public key can be represented by E(exponent) and N(modulus)
********************************************************************
*/
TEE_OperationHandle OperationHandle = TEE_HANDLE_NULL;
TEE_ObjectHandle KeyPairObjHandle = TEE_HANDLE_NULL;
uint32_t maxKeySize = 0U;
TEE_Attribute PrivateKeyAttr[3]; // used for setting the public key

uint32_t PrivateKey2048Len_N = 256U;
char Privatekey2048_N[] = 
{
    0xca, 0xaa, 0x20, 0x34, 0x4c, 0xa9, 0x91, 0x90, 0xc8, 0x53, 0x86, 0x28, 0xd4, 0x66,
    0xd3, 0xa6, 0x4e, 0xe5, 0x35, 0x4a, 0x34, 0xbd, 0x9f, 0x53, 0xe6, 0x58, 0xf7, 0x06, 0xf9,
    0x5d, 0x29, 0x80, 0xce, 0xc2, 0xa5, 0x9c, 0xa0, 0xe2, 0x63, 0x79, 0xc1, 0xdc, 0x17, 0x9d,
    0xfd, 0x95, 0x93, 0x3a, 0xbc, 0x9d, 0xd3, 0xf5, 0xfb, 0x7a, 0xee, 0x31, 0xb7, 0x39, 0x01,
    0x18, 0x87, 0xa7, 0x2c, 0x00, 0xd1, 0x2e, 0xfb, 0x04, 0xed, 0xa3, 0x92, 0xc9, 0xa2, 0x99,
    0xa6, 0x4d, 0x6b, 0x82, 0x14, 0x9a, 0x00, 0x06, 0xc6, 0x16, 0x56, 0x83, 0x48, 0xad, 0x27,
    0x0e, 0x27, 0x6b, 0x47, 0x2d, 0x84, 0xd4, 0xaf, 0xea, 0xb7, 0x03, 0x00, 0xbe, 0xcd, 0x91,
    0xd4, 0x33, 0x96, 0x41, 0x61, 0x43, 0xf9, 0x37, 0x18, 0x14, 0x67, 0xf0, 0x48, 0x68, 0xa1,
    0xfe, 0x50, 0x73, 0xda, 0xce, 0xbb, 0xff, 0x2e, 0x49, 0x0c, 0x17, 0x09, 0xdd, 0x62, 0x3f,
    0xbc, 0xef, 0x30, 0x45, 0xdb, 0x4d, 0xa9, 0x48, 0xe6, 0xaf, 0xe3, 0x44, 0x67, 0x32, 0xcf,
    0xa3, 0x6d, 0xf6, 0x61, 0x49, 0x97, 0x74, 0x58, 0xde, 0x57, 0xd5, 0xf3, 0x07, 0x96, 0x8e,
    0xb0, 0xae, 0x4d, 0xfc, 0xad, 0xf0, 0xfc, 0x49, 0xe2, 0x18, 0x43, 0x93, 0x7a, 0x21, 0x75,
    0x26, 0xd2, 0x7f, 0xa3, 0x1f, 0xe9, 0xb4, 0x2e, 0x43, 0x4d, 0xe9, 0x0e, 0x77, 0x0b, 0xe4,
    0x1c, 0x7b, 0x84, 0x57, 0x67, 0x43, 0xbc, 0xb4, 0x42, 0xed, 0x16, 0x38, 0xc3, 0x03, 0xfe,
    0x5a, 0x3d, 0xd6, 0x90, 0xc0, 0x15, 0x00, 0xa0, 0x2c, 0x11, 0x22, 0xdd, 0x31, 0x11, 0x2a,
    0x2e, 0xcb, 0x41, 0x2a, 0x54, 0x3b, 0x5e, 0xe0, 0x4a, 0xe7, 0x90, 0x99, 0x98, 0x06, 0x4e,
    0xad, 0x51, 0x36, 0x0c, 0xba, 0xa6, 0xb8, 0x63, 0xa4, 0x25, 0x9b, 0xb5, 0xd6, 0xe3, 0x84,
    0xd0, 0xaf
};
uint32_t PrivateKey2048Len_D = 256U;
char Privatekey2048_D[] =
{
    0xc8, 0x0a, 0xcf, 0x48, 0xf0, 0x56, 0xfc, 0x31, 0x2e, 0x1b, 0x8b, 0xb4, 0x67, 0x9b,
    0xab, 0xd4, 0x5f, 0xd1, 0xcd, 0x1d, 0x17, 0x1c, 0x08, 0xd5, 0xb6, 0xec, 0x1f, 0xc1, 0x89,
    0x88, 0x94, 0xbb, 0xe8, 0xda, 0xa6, 0xc0, 0x08, 0xea, 0x0d, 0x88, 0x94, 0x7b, 0x1b, 0xf0,
    0xbd, 0xea, 0xd3, 0x6d, 0xc9, 0xd9, 0xfb, 0xeb, 0xb4, 0xb4, 0x2d, 0xc3, 0x72, 0xf8, 0x58,
    0x85, 0x8e, 0xf8, 0xca, 0xed, 0x6f, 0x8d, 0x71, 0x06, 0xf7, 0x4e, 0x5b, 0x48, 0x9e, 0x30,
    0x0d, 0xc0, 0x23, 0x9b, 0x3d, 0x9c, 0x98, 0xe4, 0x16, 0xc8, 0xdb, 0xd2, 0x30, 0x7d, 0x81,
    0xac, 0x6d, 0xea, 0x6d, 0x52, 0x97, 0xe7, 0x9c, 0x51, 0x11, 0x1e, 0x6c, 0xb0, 0xe9, 0x96,
    0xc5, 0x1f, 0xba, 0x73, 0x84, 0x7f, 0xa4, 0xd1, 0xe5, 0xf9, 0xd6, 0x72, 0x8d, 0xf6, 0x81,
    0xd7, 0xd6, 0x2f, 0xd5, 0x4d, 0xf4, 0xb1, 0x38, 0x4b, 0xec, 0xd1, 0x9d, 0xf3, 0x82, 0xd7,
    0x88, 0x65, 0x4e, 0xb0, 0xb8, 0xdf, 0x1c, 0x7c, 0x5c, 0x8e, 0xf4, 0xac, 0x13, 0x8f, 0x5a,
    0xe9, 0x4f, 0x97, 0x30, 0xab, 0x42, 0x27, 0xcb, 0x31, 0x9d, 0x51, 0xf8, 0x50, 0x95, 0xb9,
    0x75, 0xda, 0x80, 0x7e, 0x33, 0x0c, 0xc0, 0xfb, 0xd2, 0x7d, 0x22, 0xda, 0x8e, 0x2b, 0xe4,
    0x02, 0x04, 0x6f, 0xad, 0xa3, 0x0d, 0x4c, 0x73, 0xc7, 0x1d, 0x53, 0xc4, 0x09, 0x92, 0x97,
    0x65, 0x8b, 0xac, 0xb4, 0xfd, 0x9d, 0x3c, 0xc7, 0x1d, 0x9c, 0x42, 0x2c, 0x3d, 0xce, 0x65,
    0x78, 0x6b, 0x14, 0xf1, 0xdb, 0x17, 0xc3, 0x3a, 0x82, 0xd8, 0x31, 0x70, 0x29, 0xe3, 0x81,
    0x3f, 0xc7, 0x83, 0xbd, 0x66, 0x84, 0xab, 0x28, 0xde, 0x67, 0x55, 0x52, 0xd4, 0xd4, 0x25,
    0x6d, 0xb8, 0xeb, 0x79, 0x4e, 0x57, 0xa1, 0xe4, 0xd3, 0x16, 0x1c, 0xed, 0x88, 0x86, 0x4f,
    0xfc, 0xf1
};
uint32_t PrivateKey2048Len_E = 256U;
char Privatekey2048_E[] = 
{
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01
};

/*
 * Called when the instance of the TA is created. This is the first call in
 * the TA.
 */
TEE_Result TA_CreateEntryPoint(void)
{
	DMSG("has been called");

	return TEE_SUCCESS;
}

/*
 * Called when the instance of the TA is destroyed if the TA has not
 * crashed or panicked. This is the last call in the TA.
 */
void TA_DestroyEntryPoint(void)
{
	DMSG("has been called");
}

/*
 * Called when a new session is opened to the TA. *sess_ctx can be updated
 * with a value to be able to identify this session in subsequent calls to the
 * TA. In this function you will normally do the global initialization for the
 * TA.
 */
TEE_Result TA_OpenSessionEntryPoint(uint32_t param_types,
		TEE_Param __maybe_unused params[4],
		void __maybe_unused **sess_ctx)
{
	/* Unused parameters */
	(void)&params;
	(void)&sess_ctx;

	/* If return value != TEE_SUCCESS the session will not be created. */
	return TEE_SUCCESS;
}

/*
 * Called when a session is closed, sess_ctx hold the value that was
 * assigned by TA_OpenSessionEntryPoint().
 */
void TA_CloseSessionEntryPoint(void __maybe_unused *sess_ctx)
{
	(void)&sess_ctx; /* Unused parameter */
	IMSG("Goodbye!\n");
}

/*
 * Set RSA Public Key function
 */
 static TEE_Result SetRsaPrivateKey() {
     TEE_Result res = FAIL;
     maxKeySize = (PrivateKey2048Len_N)*8U;
     // 1) Clean attribute array
     TEE_MemFill(PrivateKeyAttr, 0, 3*(sizeof(TEE_Attribute)));
     // 2) Set attribute[0] data with N data
     PrivateKeyAttr[0].attributeID = TEE_ATTR_RSA_MODULUS;
     PrivateKeyAttr[0].content.ref.buffer = Privatekey2048_N;
     PrivateKeyAttr[0].content.ref.length = PrivateKey2048Len_N;
     // 3) Set attribute[1] data with E data
     PrivateKeyAttr[1].attributeID = TEE_ATTR_RSA_PUBLIC_EXPONENT;
     PrivateKeyAttr[1].content.ref.buffer = Privatekey2048_E;
     PrivateKeyAttr[1].content.ref.length = PrivateKey2048Len_E;
     // 4) Set attribute[1] data with D data
     PrivateKeyAttr[2].attributeID = TEE_ATTR_RSA_PRIVATE_EXPONENT;
     PrivateKeyAttr[2].content.ref.buffer = Privatekey2048_D;
     PrivateKeyAttr[2].content.ref.length = PrivateKey2048Len_D;
     // 5) Allocate the public key handle
     res = TEE_AllocateTransientObject(TEE_TYPE_RSA_KEYPAIR, maxKeySize, &KeyPairObjHandle);
     if (res != TEE_SUCCESS) {
         EMSG("Allocate PrivateKey handle failed, res=0x%x", res);
         return res;
     }
     // 6) Populate the attribute
     res = TEE_PopulateTransientObject(KeyPairObjHandle, PrivateKeyAttr, 3);
     if (res != TEE_SUCCESS) {
         EMSG("Populate PrivateKey handle failed, res=0x%x", res);
         TEE_FreeTransientObject(KeyPairObjHandle);
         KeyPairObjHandle = TEE_HANDLE_NULL;
         return res;
     }
     return res;
 }
/*
 * RSA Encryption Operation function
 */
static TEE_Result RsaDecOper(char* inBuf, uint32_t inBufLen, char* outBuf, uint32_t* outBufLen) {
    int res = FAIL;
    // 1) Allocate the operation handle
    res = TEE_AllocateOperation(&OperationHandle, TEE_ALG_RSAES_PKCS1_V1_5, TEE_MODE_DECRYPT, maxKeySize);
    if (res != TEE_SUCCESS) {
        EMSG("the allocate operate handle failed, res=0x%x", res);
        return res;
    }
    // 2) Set the key object into operation handle
    res = TEE_SetOperationKey(OperationHandle, KeyPairObjHandle);
    if (res != TEE_SUCCESS) {
        EMSG("setting the key object failed, res=0x%x", res);
        TEE_FreeOperation(OperationHandle);
        OperationHandle = TEE_HANDLE_NULL;
        return res;
    }
    // 3) invoke the TEE encryption API
    res = TEE_AsymmetricDecrypt(OperationHandle, NULL, 0, inBuf, inBufLen, outBuf, outBufLen);
    if (res != TEE_SUCCESS) {
        EMSG("Asymmetric decrypt failed, res=0x%x", res);
        TEE_FreeOperation(OperationHandle);
        OperationHandle = TEE_HANDLE_NULL;
        return res;
    }

    return res;
}

TEE_Result rsa_decrypt(uint32_t param_types, TEE_Param params[4]) {
    uint32_t exp_param_types = TEE_PARAM_TYPES (
                           TEE_PARAM_TYPE_MEMREF_INPUT,
						   TEE_PARAM_TYPE_MEMREF_OUTPUT,
						   TEE_PARAM_TYPE_NONE,
						   TEE_PARAM_TYPE_NONE);

	if (param_types != exp_param_types)
		return TEE_ERROR_BAD_PARAMETERS;
    char* input = NULL;         // the encrypted data
    char* inputLen = 0;         // the length of encrypted data
    char* output = NULL;        // the decrypted data
    uint32_t outputLen = 0;     // the length of decrypted data

    input = params[0].memref.buffer;
    inputLen = params[0].memref.size;
    output = params[1].memref.buffer;
    outputLen = params[1].memref.size;

    SetRsaPrivateKey(); // set the rsa private key into handle
    RsaDecOper(input, inputLen, output, &outputLen);
    
    return TEE_SUCCESS;
}

/*
 * Called when a TA is invoked. sess_ctx hold that value that was
 * assigned by TA_OpenSessionEntryPoint(). The rest of the paramters
 * comes from normal world.
 */
TEE_Result TA_InvokeCommandEntryPoint(void __maybe_unused *sess_ctx,
			uint32_t cmd_id,
			uint32_t param_types, TEE_Param params[4])
{
	(void)&sess_ctx; /* Unused parameter */

	switch (cmd_id) {
    case CMD_RSA_DEC_PKCS1_OPER:
        return rsa_decrypt(param_types, params);
	default:
		return TEE_ERROR_BAD_PARAMETERS;
	}
}
