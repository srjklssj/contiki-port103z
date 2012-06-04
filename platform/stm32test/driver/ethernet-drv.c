#include <stdio.h>
#include <string.h>
#include "contiki-net.h"
#include "enc28j60.h"
#include "ethernet-drv.h"

#define BUF ((struct uip_eth_hdr *)&uip_buf[0])
#define IPBUF ((struct uip_ip_hdr *)&uip_buf[UIP_LLH_LEN])

PROCESS(ethernet_process, "Ethernet Process");

void dbg_printbuffer(char *s, unsigned int l, unsigned char *buf) {
	unsigned int i;

	printf("%s  %d\r\n", s, l);
	for (i = 0; i < l; ++i) {
		if ((0 == i % 16) && (0 != i))
			printf("\r\n");
		printf("%02x ", buf[i]);
	}
	printf("\r\n");
}

unsigned char ethernet_output(uip_lladdr_t *lladdr) {
	if (NULL == lladdr || ((0 == lladdr->addr[0]) && (0 == lladdr->addr[1]) 
				&& (0 == lladdr->addr[2]) && (0 == lladdr->addr[3])
				&& (0 == lladdr->addr[4]) && (0 == lladdr->addr[5]))) {
		(BUF->dest).addr[0] = 0x33;
		(BUF->dest).addr[1] = 0x33;
		(BUF->dest).addr[2] = IPBUF->destipaddr.u8[12];
		(BUF->dest).addr[3] = IPBUF->destipaddr.u8[13];
		(BUF->dest).addr[4] = IPBUF->destipaddr.u8[14];
		(BUF->dest).addr[5] = IPBUF->destipaddr.u8[15];
	} else
		memcpy(&BUF->dest, lladdr->addr, UIP_LLADDR_LEN);
	
	memcpy(&BUF->src, uip_lladdr.addr, 6);
	BUF->type = UIP_HTONS(UIP_ETHTYPE_IPV6);
	uip_len += sizeof(struct uip_eth_hdr);
	printf("SUT -- %u    TYPE -- %x    UL -- %d\r\n", uip_len, BUF->type,
			(IPBUF->len[0]) << 8 | (IPBUF->len[1]));

	// DEBUG
	dbg_printbuffer("send packet:", uip_len, uip_buf);
	// 

	Enc_PacketSend(uip_len, uip_buf);

	return 0;
}

static void ethernet_pollhandler(void) {
	uip_len = Enc_PacketReceive(UIP_CONF_BUFFER_SIZE, uip_buf);
	if (uip_len > 0) {
		//dbg_printbuffer("receive ethernet packet:", uip_len, uip_buf);
		if (uip_htons(UIP_ETHTYPE_IPV6) == BUF->type) {
			printf("\r\nipv6 packet incoming!\r\n\r\n");
			tcpip_input();
		} else
			uip_len = 0;
	}

	process_poll(&ethernet_process);
}

static void ethernet_exithandler(void) {
	printf("packet process exit.\r\n");
}

PROCESS_THREAD(ethernet_process, ev, data) {
	PROCESS_POLLHANDLER(ethernet_pollhandler());
	PROCESS_EXITHANDLER(ethernet_exithandler());

	PROCESS_BEGIN();

	process_poll(&ethernet_process);
	PROCESS_WAIT_EVENT_UNTIL(PROCESS_EVENT_EXIT == ev);
	
	PROCESS_END();
}

