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

//ec0b0d54-d7a4-450b-9a45-4b49f39ff2f0
#define SECURE_LAB_UUID_ID \
    {0xec0b0d54, 0xd7a4, 0x450b, \
        { 0x9a, 0x45, 0x4b, 0x49, 0xf3, 0x9f, 0xf2, 0xf0}}

#define CMD_PTA_DHT11_READ_DATA           5U     /*< Command ID of using pseudo TA to read DHT11 sensor data */
#define CMD_RSA_ENC_PKCS1_OPER            6U     /*< Command ID of using RSA algorithm to signa data */


#define   FAIL     -1            /* Return value when operation fail */
#define   OK        0            /* Return value when operation OK */

#endif
