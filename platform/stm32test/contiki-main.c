#include <stm32f10x_map.h>
#include <stm32f10x_dma.h>
#include <gpio.h>
#include <nvic.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <contiki.h>
#include <contiki-net.h>
#include "cpu-init.h"
#include "ethernet-drv.h"
#include "spi-arch.h"
#include "debug-uart.h"
#include "system-basis.h"
#include "enc28j60.h"
#include "cc2500.h"
#include "cc2520.h"

#define PRINT6ADDR(addr) printf("%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x", ((u8_t *)addr)[0], ((u8_t *)addr)[1], ((u8_t *)addr)[2], ((u8_t *)addr)[3], ((u8_t *)addr)[4], ((u8_t *)addr)[5], ((u8_t *)addr)[6], ((u8_t *)addr)[7], ((u8_t *)addr)[8], ((u8_t *)addr)[9], ((u8_t *)addr)[10], ((u8_t *)addr)[11], ((u8_t *)addr)[12], ((u8_t *)addr)[13], ((u8_t *)addr)[14], ((u8_t *)addr)[15])

unsigned int idle_count = 0;
unsigned char HostIEEEADDR[8] = {0x12, 0x00, 0x00, 0x00, 0x77, 0x77, 0x77, 0x01};

PROCINIT(&etimer_process, &ethernet_process, &tcpip_process);//, &uip_fw_process);

int main() {
	cpu_init();
	clock_init();
	spi_init();
	
	// uip_lladdr must set befor uip_init();
	uip_lladdr.addr[0] = 0x24; uip_lladdr.addr[1] = 0x00;
	uip_lladdr.addr[2] = 0x10; uip_lladdr.addr[3] = 0x58;
	uip_lladdr.addr[4] = 0x55; uip_lladdr.addr[5] = 0x54;
	Enc_Init(uip_lladdr.addr);
	
	cc2500_init();
	cc2500_cmd(CCxxx0_SIDLE);	// turn off cc2500
	
	printf("Hardware has initialized.\r\n");

	process_init();
	process_start(&etimer_process, NULL);
	ctimer_init();
	procinit_init();
	
	// initialize uip
	{
		uip_ipaddr_t ip_addr;
		
		// add global unicast address
		uip_ip6addr(&ip_addr, 0x2001, 0x0250, 0x4000, 0x1000, 
				0x0000, 0x0000, 0x0000, 0x0000);
		uip_ds6_set_addr_iid(&ip_addr, &uip_lladdr);
		uip_ds6_prefix_add(&ip_addr, 64, 0);
		uip_ds6_addr_add(&ip_addr, 0, ADDR_AUTOCONF);
		printf("ip:");
		PRINT6ADDR(ip_addr.u8);
		printf("\r\n");

		tcpip_set_outputfunc(ethernet_output);
	}

	// CC2520 TEST
	
	{
		unsigned char length;
		char cc2520_buffer[128];
		static char *test_str = "Ok... I need a sample to test cc2520.\r\n";

		cc2520_init();
#if 0
		// send something
		cc2520_buffer[0] = 0x01;
		cc2520_buffer[1] = (0x03 << 6) | (0x01 << 4) | (0x02 << 2);
		cc2520_buffer[2] = 0x01;
		cc2520_buffer[3] = 0xFF;
		cc2520_buffer[4] = 0xFF;
		cc2520_buffer[5] = 0xFF;
		cc2520_buffer[6] = 0xFF;
		cc2520_buffer[7] = 0xFF;	// broad cast
		cc2520_buffer[8] = 0xFF;
		cc2520_buffer[9] = HostIEEEADDR[0];
		cc2520_buffer[10] = HostIEEEADDR[1];
		cc2520_buffer[11] = HostIEEEADDR[2];
		cc2520_buffer[12] = HostIEEEADDR[3];
		cc2520_buffer[13] = HostIEEEADDR[4];
		cc2520_buffer[14] = HostIEEEADDR[5];
		cc2520_buffer[15] = HostIEEEADDR[6];
		cc2520_buffer[16] = HostIEEEADDR[7];
		length = strlen(test_str);
		memcpy(cc2520_buffer + 17, test_str, length);
		length += 17;
		while (1) {
			printf("send packet\r\n");
			cc2520_wfifo((unsigned char *)cc2520_buffer, length);
			cc2520_txpacket();
			DELAY_MS(500);
		}
#else	// receive
		cc2520_set_rxmode();
		cc2520_wreg(FRMFILT0, cc2520_rreg(FRMFILT0) | (0x01 << 1));	// be a coordinator
		//cc2520_wreg(FRMFILT0, cc2520_rreg(FRMFILT0) & ~0x01);
		cc2520_buffer[0] = cc2520_rram(0x03, RAM_IEEEADR);
		cc2520_buffer[1] = cc2520_rram(0x03, RAM_IEEEADR + 1);
		cc2520_buffer[2] = cc2520_rram(0x03, RAM_IEEEADR + 2);
		cc2520_buffer[3] = cc2520_rram(0x03, RAM_IEEEADR + 3);
		cc2520_buffer[4] = cc2520_rram(0x03, RAM_IEEEADR + 4);
		cc2520_buffer[5] = cc2520_rram(0x03, RAM_IEEEADR + 5);
		cc2520_buffer[6] = cc2520_rram(0x03, RAM_IEEEADR + 6);
		cc2520_buffer[7] = cc2520_rram(0x03, RAM_IEEEADR + 7);
		cc2520_dbg_pbuffer((unsigned char *)cc2520_buffer, 8);
		while (1) {
			if (cc2520_haspacket()) {
				printf("new packet -- ");
				length = cc2520_rreg(RXFIFOCNT);
				printf("RXFIFOCNT -- %d", length);
				length = cc2520_rfifo((unsigned char *)cc2520_buffer);
				cc2520_wreg(EXCFLAG1, ~CC2520_RX_FRM_DONE);	// clear the flag
				//cc2520_set_rxmode();
				cc2520_buffer[length] = '\0';
				printf(" FIFO -- %d\r\n", length);
				cc2520_dbg_pbuffer((unsigned char *)cc2520_buffer, length);
				printf("\r\n");
			}
		}
#endif

	}
	// -------------------- CC2520 TEST END --------------------------- //
	

	autostart_start(autostart_processes);
	printf("Processes running\r\n");

	while(1) {
		do {
		} while(process_run() > 0);
		idle_count++;
		/* Idle! */
		/* Stop processor clock */
		/* asm("wfi"::); */ 
	}
	return 0;
}

void uip_log(char *m) {
	printf("uIP: '%s'\r\n", m);
}

