#ifndef TA_SECURE_LAB_H
#define TA_SECURE_LAB_H

#include <tee_internal_api.h>
#include <tee_internal_api_extensions.h>
#include <tee_api_defines.h>
#include <trace.h>
#include <string.h>

#define SECURE_LAB_UUID_ID \
	{ 0xec0b0d54, 0xd7a4, 0x450b, \
		{ 0x9a, 0x45, 0x4b, 0x49, 0xf3, 0x9f, 0xf2, 0xf0} }

/* The function IDs implemented in this TA */
#define CMD_PTA_DHT11_READ_DATA		5	// invoke pta to read the dht11 data
#define CMD_RSA_ENC_PKCS1_OPER      6	// invoke encrypted function

/* The function IDs implemented in the pseudo TA*/
#define GPIO_ON					0	// pin18 high level
#define GPIO_OFF				1   // pin18 low level
#define TEST_PSEUDO_TA 			2	// print for test
#define DHT11_READ_DATA			3	// read the dht11 data from pin18
#define PTA_RPI3DHT11_UUID \
	{ 0xbd867e69, 0x58fb, 0x4675, \
		{ 0x83, 0x26, 0x5a, 0x68, 0xa3, 0xbf, 0xcc, 0x7f}}

#define   FAIL     		-1            /* Return value when operation fail */
#define   OK        	0            /* Return value when operation OK */

#endif
