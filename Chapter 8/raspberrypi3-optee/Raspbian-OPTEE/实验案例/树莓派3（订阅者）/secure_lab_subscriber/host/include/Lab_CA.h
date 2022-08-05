#ifndef LAB_CA_HEADER_H
#define LAB_CA_HEADER_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include "tee_client_api.h"

//76d3f3ae-5e08-4e15-83e1-eda725828aa5 
#define SECURE_LAB_SUB_UUID_ID \
    {0x76d3f3ae, 0x5e08, 0x4e15, \
        { 0x83, 0xe1, 0xed, 0xa7, 0x25, 0x82, 0x8a, 0xa5}}

#define CMD_RSA_DEC_PKCS1_OPER            7


#define   FAIL     -1            /* Return value when operation fail */
#define   OK        0            /* Return value when operation OK */

#endif
