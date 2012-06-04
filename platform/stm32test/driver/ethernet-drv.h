#ifndef __PACKET_DRV_H__
#define __PACKET_DRV_H__

#include "contiki.h"

PROCESS_NAME(ethernet_process);
unsigned char ethernet_output(uip_lladdr_t *lladdr);

#endif

