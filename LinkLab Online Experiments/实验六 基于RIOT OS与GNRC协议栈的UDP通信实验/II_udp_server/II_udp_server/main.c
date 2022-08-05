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
#include "shell.h"
#include "msg.h"
#include "net/udp.h"
#include "net/netif.h"
#include "net/gnrc/pkt.h"
#include "errno.h"
#include "net/gnrc/netif/hdr.h"
#include "net/gnrc/pktdump.h"

#define MAIN_QUEUE_SIZE     (8)
#define APP_MTU				500
#define UDP_PORT			123
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];
static kernel_pid_t udp_server_pid;
static char udp_server_thread_buf[1024];
static char recv_buf[APP_MTU];

extern void start_server(uint16_t port, kernel_pid_t app_pid);
extern int _gnrc_netif_config(int argc, char **argv);


static int _recv(gnrc_pktsnip_t *pkt)
{
	gnrc_pktsnip_t *ptr = pkt;
	gnrc_pktsnip_t *data_ptr = NULL;
	bool is_udp = false;
	/* The first part of the chain list should be UNDEF
	 * and contains the data part
	 */
	while (ptr) {
		if (ptr->type == GNRC_NETTYPE_UNDEF) {
			if (ptr->size > APP_MTU) {
				gnrc_pktbuf_release(pkt);
				return -ENOMEM;
			}
			data_ptr = ptr;
		}
		else if (ptr->type == GNRC_NETTYPE_UDP) {
			is_udp = true;
		}
		else if (is_udp && ptr->type ==GNRC_NETTYPE_NETIF) {
			gnrc_netif_hdr_print(ptr->data);
		}
		ptr = ptr->next;
	}

	if (is_udp && data_ptr != NULL) {
		memcpy(recv_buf, data_ptr->data, data_ptr->size);
		printf("APP data: %s\n", recv_buf);
		gnrc_pktbuf_release(pkt);
		return 0;
	}
	else {
		gnrc_pktbuf_release(pkt);
		return -ENOTSUP;
	}
}

static void *udp_server_func(void *arg)
{
	(void)arg;
	msg_t msg, reply;
    msg_t msg_queue[GNRC_PKTDUMP_MSG_QUEUE_SIZE];

    /* setup the message queue */
    msg_init_queue(msg_queue, GNRC_PKTDUMP_MSG_QUEUE_SIZE);
    reply.content.value = (uint32_t)(-ENOTSUP);
    reply.type = GNRC_NETAPI_MSG_TYPE_ACK;

    while (true) {
		msg_receive(&msg);
		switch (msg.type) {
			case GNRC_NETAPI_MSG_TYPE_RCV:
                if (_recv(msg.content.ptr) < 0) {
					msg_reply(&msg, &reply);
                }
                break;
            default:
            	msg_reply(&msg, &reply);
            	break;
		}
    }
    return NULL;
}

int main(void)
{
    /* we need a message queue for the thread running the shell in order to
     * receive potentially fast incoming networking packets */
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);
    puts("RIOT network stack example application");
	udp_server_pid = thread_create(udp_server_thread_buf, sizeof(udp_server_thread_buf), THREAD_PRIORITY_MAIN-1,
									THREAD_CREATE_STACKTEST, udp_server_func, NULL, "udp server thread");
	start_server((uint16_t)UDP_PORT, udp_server_pid);
    /* start shell */
    puts("UDP server started");
	netif_t *netif = NULL;
	ipv6_addr_t addr;
	while ((netif=netif_iter(netif))) {
		netif_get_opt(netif, NETOPT_IPV6_ADDR, 0, &addr,
                          sizeof(addr));
       	char addr_str[IPV6_ADDR_MAX_STR_LEN];
		printf("server address: ");
    	ipv6_addr_to_str(addr_str, &addr, sizeof(addr_str));
    	printf("%s,", addr_str);
    	printf(" port:%d\n", UDP_PORT);
	}
    return 0;
}
