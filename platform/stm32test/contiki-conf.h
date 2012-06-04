#ifndef __CONTIKI_CONF_H__CDBB4VIH3I__
#define __CONTIKI_CONF_H__CDBB4VIH3I__
#include <stdint.h>

typedef uint8_t u8_t;
typedef uint16_t u16_t;
typedef uint32_t u32_t;
typedef int8_t s8_t;
typedef int16_t s16_t;
typedef int32_t s32_t;
typedef unsigned int clock_time_t;
typedef unsigned int uip_stats_t;

#define CCIF
#define CLIF
#define AUTOSTART_ENABLE	1
#define CLOCK_CONF_SECOND	32

#ifndef BV
#define BV(x) (1<<(x))
#endif

/* uIP configuration */
#define uip_ipaddr_copy(dest, src) memcpy(dest, src, sizeof(*dest))

// General
#define UIP_CONF_LLH_LEN	14
#define UIP_CONF_BROADCAST	1
#define UIP_CONF_LOGGING	1
#define UIP_CONF_BUFFER_SIZE	1500

// IPv6
#define UIP_CONF_IPV6			1
#define UIP_CONF_IPV6_CHECKS		1
#define UIP_CONF_IPV6_QUEUE_PKT		0
#define UIP_CONF_ICMP6			1
#define UIP_CONF_NETIF_MAX_ADDRESSES	3

/*********************/

/* Prefix for relocation sections in ELF files */
#define REL_SECT_PREFIX ".rel"

#define CC_BYTE_ALIGNED __attribute__ ((packed, aligned(1)))

#define USB_EP1_SIZE 64
#define USB_EP2_SIZE 64

//#define RAND_MAX 0x7fff
#endif /* __CONTIKI_CONF_H__CDBB4VIH3I__ */