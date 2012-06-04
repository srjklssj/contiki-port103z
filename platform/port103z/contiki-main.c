#include <stm32f10x_map.h>
#include <stm32f10x_dma.h>
#include <gpio.h>
#include <nvic.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <contiki.h>
#include <contiki-net.h>
#include "ethernet-drv.h"
#include "spi-arch.h"
#include "cpu-init.h"
#include "system-basis.h"
#include "enc28j60.h"
#include "cc2520.h"

#define PRINT6ADDR(addr) printf("%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x", ((u8_t *)addr)[0], ((u8_t *)addr)[1], ((u8_t *)addr)[2], ((u8_t *)addr)[3], ((u8_t *)addr)[4], ((u8_t *)addr)[5], ((u8_t *)addr)[6], ((u8_t *)addr)[7], ((u8_t *)addr)[8], ((u8_t *)addr)[9], ((u8_t *)addr)[10], ((u8_t *)addr)[11], ((u8_t *)addr)[12], ((u8_t *)addr)[13], ((u8_t *)addr)[14], ((u8_t *)addr)[15])

unsigned int idle_count = 0;
unsigned char HostIEEEADDR[8] = {0x12, 0x00, 0x00, 0x00, 0x77, 0x77, 0x77, 0x01};

PROCINIT(&etimer_process, &ethernet_process, &tcpip_process);

int main() {
	cpu_init();
	clock_init();
	spi_init();
	
	// uip_lladdr must set befor uip_init();
	uip_lladdr.addr[0] = 0x24; uip_lladdr.addr[1] = 0x00;
	uip_lladdr.addr[2] = 0x10; uip_lladdr.addr[3] = 0x58;
	uip_lladdr.addr[4] = 0x55; uip_lladdr.addr[5] = 0x54;
	Enc_Init(uip_lladdr.addr);

	// register interrupt
	GPIO_CONF_INPUT_PORT(A, 3, PU_PD);
	AFIO->EXTICR[0] &= ~AFIO_EXTICR1_EXTI3;
	AFIO->EXTICR[0] |= AFIO_EXTICR1_EXTI3_PA;
	EXTI->IMR |= EXTI_IMR_MR3;
	EXTI->FTSR |= EXTI_FTSR_TR3;	// falling edge
	EXTI->PR |= EXTI_PR_PR3;

	NVIC->IPR[2] &= ~NVIC_IPR2_PRI_9;
	NVIC->IPR[2] |= 0x01 << 8;
	NVIC->ISER[0] |= NVIC_ISER_SETENA_9;	// enable interrupt
	
	cc2520_init();
	cc2520_wreg(FRMFILT0, cc2520_rreg(FRMFILT0) | (0x01 << 1));	// be a coordinator
	cc2520_set_rxmode();
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

