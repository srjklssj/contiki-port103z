#ifndef __DEBUG_UART_H__1V2039076V__
#define __DEBUG_UART_H__1V2039076V__

/* ADD BY KAUFFMAN */
#include <stdio.h>
#include <string.h>

#define dbg_setup_uart dbg_setup_uart_default
#define dbg_send_bytes rs232_send
#define dbg_putchar rs232_put

unsigned char rs232_buffer[512];

#define dbg_printf(...) do { \
			sprintf((char *)rs232_buffer, __VA_ARGS__); \
			rs232_send(rs232_buffer, strlen((char *)rs232_buffer)); \
		} while (0)

void rs232_send(unsigned char *buf, int length);
void rs232_put(unsigned char c);
void dbg_setup_uart_default(void);

/* ADD END */

#endif /* __DEBUG_UART_H__1V2039076V__ */
