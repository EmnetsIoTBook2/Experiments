#ifndef TA_SECURE_LAB_SUB_H
#define TA_SECURE_LAB_SUB_H

#include <tee_internal_api.h>
#include <tee_internal_api_extensions.h>
#include <tee_api_defines.h>
#include <trace.h>
#include <string.h>

#define SECURE_LAB_SUB_UUID_ID \
	{0x76d3f3ae, 0x5e08, 0x4e15, \
        { 0x83, 0xe1, 0xed, 0xa7, 0x25, 0x82, 0x8a, 0xa5}}

/* The function IDs implemented in this TA */
#define CMD_RSA_DEC_PKCS1_OPER      7	// invoke encrypted function

#define   FAIL     		-1            /* Return value when operation fail */
#define   OK        	0            /* Return value when operation OK */

#endif
