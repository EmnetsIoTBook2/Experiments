#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "sys/uio.h"
#include "luid.h"

#include "net/netdev.h"
#include "shell.h"
#include "shell_commands.h"
#include "net/ieee802154/submac.h"
#include "net/ieee802154.h"
#include "net/netdev/ieee802154_submac.h"

#include "common.h"

#define MAX_LINE    (80)

/*目前只支持第一个电台*/  
#define RADIO_DEFAULT_ID (0U)

int retrans_num = 0;
int media_busy_num = 0;
int tx_packets_num = 0;

netdev_ieee802154_submac_t netdev_submac;

void _ack_timeout(void *arg);

uint8_t buffer[IEEE802154_FRAME_LEN_MAX];
uint8_t seq;

extern const netdev_driver_t netdev_submac_driver;

static void _netdev_isr_handler(event_t *event)
{
    (void)event;
    netdev_t *netdev = (netdev_t *)&netdev_submac;

    netdev->driver->isr(netdev);
}

void _print_addr(uint8_t *addr, size_t addr_len)
{
    for (size_t i = 0; i < addr_len; i++) {
        if (i != 0) {
            printf(":");
        }
        printf("%02x", (unsigned)addr[i]);
    }
}
static event_t _netdev_ev = { .handler = _netdev_isr_handler };

void recv(netdev_t *dev)
{
    uint8_t src[IEEE802154_LONG_ADDRESS_LEN], dst[IEEE802154_LONG_ADDRESS_LEN];
    int data_len;
    size_t mhr_len, src_len, dst_len;
    netdev_ieee802154_rx_info_t rx_info;
    le_uint16_t src_pan, dst_pan;

    putchar('\n');
    data_len = dev->driver->recv(dev, buffer, sizeof(buffer), &rx_info);
    if (data_len < 0) {
        puts("Couldn't read frame");
        return;
    }
    mhr_len = ieee802154_get_frame_hdr_len(buffer);
    if (mhr_len == 0) {
        puts("Unexpected MHR for incoming packet");
        return;
    }
    dst_len = ieee802154_get_dst(buffer, dst, &dst_pan);
    src_len = ieee802154_get_src(buffer, src, &src_pan);
    switch (buffer[0] & IEEE802154_FCF_TYPE_MASK) {
    case IEEE802154_FCF_TYPE_BEACON:
        puts("BEACON");
        break;
    case IEEE802154_FCF_TYPE_DATA:
        puts("DATA");
        break;
    case IEEE802154_FCF_TYPE_ACK:
        puts("ACK");
        break;
    case IEEE802154_FCF_TYPE_MACCMD:
        puts("MACCMD");
        break;
    default:
        puts("UNKNOWN");
        break;
    }
    printf("Dest. PAN: 0x%04x, Dest. addr.: ",
           byteorder_ltohs(dst_pan));
    _print_addr(dst, dst_len);
    printf("\nSrc. PAN: 0x%04x, Src. addr.: ",
           byteorder_ltohs(src_pan));
    _print_addr(src, src_len);
    printf("\nSecurity: ");
    if (buffer[0] & IEEE802154_FCF_SECURITY_EN) {
        printf("1, ");
    }
    else {
        printf("0, ");
    }
    printf("Frame pend.: ");
    if (buffer[0] & IEEE802154_FCF_FRAME_PEND) {
        printf("1, ");
    }
    else {
        printf("0, ");
    }
    printf("ACK req.: ");
    if (buffer[0] & IEEE802154_FCF_ACK_REQ) {
        printf("1, ");
    }
    else {
        printf("0, ");
    }
    printf("PAN comp.: ");
    if (buffer[0] & IEEE802154_FCF_PAN_COMP) {
        puts("1");
    }
    else {
        puts("0");
    }
    printf("Version: ");
    printf("%u, ", (unsigned)((buffer[1] & IEEE802154_FCF_VERS_MASK) >> 4));
    printf("Seq.: %u\n", (unsigned)ieee802154_get_seq(buffer));
    od_hex_dump(buffer + mhr_len, data_len - mhr_len, 0);
    printf("txt: ");
    for (int i = mhr_len; i < data_len; i++) {
        if ((buffer[i] > 0x1F) && (buffer[i] < 0x80)) {
            putchar((char)buffer[i]);
        }
        else {
            putchar('?');
        }
        if (((((i - mhr_len) + 1) % (MAX_LINE - sizeof("txt: "))) == 1) &&
            (i - mhr_len) != 0) {
            printf("\n     ");
        }
    }
    printf("\n");
    printf("RSSI: %i, LQI: %u\n\n", rx_info.rssi, rx_info.lqi);
}
static void _event_cb(netdev_t *dev, netdev_event_t event)
{
    (void)dev;
    if (event == NETDEV_EVENT_ISR) {
        event_post(EVENT_PRIO_HIGHEST, &_netdev_ev);
    }
    else {
        switch (event) {
        case NETDEV_EVENT_RX_COMPLETE:
        {
            recv(dev);
            return;
        }
        case NETDEV_EVENT_TX_COMPLETE:
        	retrans_num += netdev_submac.submac.retrans;
        	tx_packets_num++;
            printf("Tx complete, retrans=%d\n", netdev_submac.retrans);
            break;
        case NETDEV_EVENT_TX_COMPLETE_DATA_PENDING:
            puts("Tx complete with pending data");
            break;
        case NETDEV_EVENT_TX_MEDIUM_BUSY:
        	media_busy_num++;
            printf("Medium Busy\n");
            break;
        case NETDEV_EVENT_TX_NOACK:
        	retrans_num += netdev_submac.submac.retrans;
        	tx_packets_num++;
            printf("No ACK, retrans=%d\n", netdev_submac.retrans);
            break;
        default:
            assert(false);
        }
    }
}
static int _init(void)
{
    ieee802154_hal_test_init_devs();

    netdev_t *dev = (netdev_t *)&netdev_submac;

    dev->event_callback = _event_cb;
    netdev_ieee802154_submac_init(&netdev_submac,
                                  ieee802154_hal_test_get_dev(RADIO_DEFAULT_ID));
    dev->driver->init(dev);
    return 0;
}

uint8_t payload[] =
		"IEEE 802.15.4 is a technical standard which defines the operation of low-rate wireless personal\
		area networks (LR-WPANs). It specifies the physical layer and media access control for LR-WPANs,\
		and is maintained by the IEEE 802.15 working group, which defined the standard in 2003.";

static iolist_t iol_hdr;

static int send(uint8_t *dst, size_t dst_len,
                size_t len)
{
    uint8_t flags;
    uint8_t mhr[IEEE802154_MAX_HDR_LEN];
    int mhr_len;

    le_uint16_t src_pan, dst_pan;
    iolist_t iol_data = {
        .iol_base = payload,
        .iol_len = len,
        .iol_next = NULL,
    };

    flags = IEEE802154_FCF_TYPE_DATA | 0x20;
    src_pan = byteorder_btols(byteorder_htons(0x23));
    dst_pan = byteorder_btols(byteorder_htons(0x23));
    uint8_t src_len = 8;
    void *src = &netdev_submac.submac.ext_addr;

    /* fill MAC header, seq should be set by device */
    if ((mhr_len = ieee802154_set_frame_hdr(mhr, src, src_len,
                                            dst, dst_len,
                                            src_pan, dst_pan,
                                            flags, seq++)) < 0) {
        puts("txtsnd: Error preperaring frame");
        return 1;
    }

    iol_hdr.iol_next = &iol_data;
    iol_hdr.iol_base = mhr;
    iol_hdr.iol_len = mhr_len;

    netdev_t *dev = (netdev_t *)&netdev_submac;

    dev->driver->send(dev, &iol_hdr);
    return 0;
}

static inline int _dehex(char c, int default_)
{
    if ('0' <= c && c <= '9') {
        return c - '0';
    }
    else if ('A' <= c && c <= 'F') {
        return c - 'A' + 10;
    }
    else if ('a' <= c && c <= 'f') {
        return c - 'a' + 10;
    }
    else {
        return default_;
    }
}

static size_t _parse_addr(uint8_t *out, size_t out_len, const char *in)
{
    const char *end_str = in;
    uint8_t *out_end = out;
    size_t count = 0;
    int assert_cell = 1;

    if (!in || !*in) {
        return 0;
    }
    while (end_str[1]) {
        ++end_str;
    }

    while (end_str >= in) {
        int a = 0, b = _dehex(*end_str--, -1);
        if (b < 0) {
            if (assert_cell) {
                return 0;
            }
            else {
                assert_cell = 1;
                continue;
            }
        }
        assert_cell = 0;

        if (end_str >= in) {
            a = _dehex(*end_str--, 0);
        }

        if (++count > out_len) {
            return 0;
        }
        *out_end++ = (a << 4) | b;
    }
    if (assert_cell) {
        return 0;
    }
    /* out is reversed */

    while (out < --out_end) {
        uint8_t tmp = *out_end;
        *out_end = *out;
        *out++ = tmp;
    }

    return count;
}

int txtsnd(char *addr_str, size_t len)
{
    uint8_t addr[8];
    size_t res;
    res = _parse_addr(addr, sizeof(addr), addr_str);
    if (res == 0) {
        return 1;
    }
    return send(addr, res, len);
}


int main(void)
{
    _init();
    /* 启动 shell */
    puts("Initialization successful - starting the shell now");
    /*设置Phy层的通道数(11-30)*/  
	ieee802154_set_channel_number(&netdev_submac.submac, 11);
    /*设置发射功率*/  
    /*取值为-40、-20、-16、-8、-4、0,4 dbm */
	ieee802154_set_tx_power(&netdev_submac.submac, -40);
	/*发送50个数据包，每个10字节的负载*/  
    for (int i = 0; i < 50; i++) {
    	/*将MAC地址改为服务器的MAC地址，然后取消注释此代码*/  
		/*txtsnd("e6be03070f6ae036", 10);*/ 
		/*休眠1秒*/
		xtimer_msleep(1000);
    }
    printf("retrans number:%d\n", retrans_num);
    printf("media busy number:%d\n", media_busy_num);
    printf("tx packets number:%d\n", tx_packets_num);
    return 0;
}