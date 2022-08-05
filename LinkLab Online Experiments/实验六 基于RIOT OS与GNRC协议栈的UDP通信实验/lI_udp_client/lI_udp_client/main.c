/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Example application for demonstrating the RIOT network stack
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>
#include <xtimer.h>
#include "shell.h"
#include "msg.h"

#define MAIN_QUEUE_SIZE     (8)
#define SERVER_IP			"fe80::e4be:307:f6a:e836"
#define SERVER_PORT			123

static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];
extern void send(char *addr_str, uint16_t port, char *data, int len);



int main(void)
{
    /* we need a message queue for the thread running the shell in order to
     * receive potentially fast incoming networking packets */
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);
    puts("RIOT network stack example application");
    printf("server ip= %s\n", SERVER_IP);
	char str[] = "1234567890";
	for (int i = 0; i < 10; i++) {
		printf("send %d packet\n", i+1);
		send(SERVER_IP, SERVER_PORT, str, strlen(str));
		xtimer_msleep(1000);
	}
    /* should be never reached */
    return 0;
}
