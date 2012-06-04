#include <debug-uart.h>
#include <string.h>
#include <stdio.h>
#include <stm32f10x_map.h>
#include <stm32f10x_dma.h>
#include <gpio.h>
//#include <nvic.h>

/*******************************************************************/
/*	Modify by Kauffman Sun
	2012/05/07
*/
#define SYSCLK ((unsigned int)MCK)
#define NOP() asm volatile("NOP\n\t" \
		"NOP\n\t" \
		"NOP\n\t" \
		"NOP\n\t");

void dbg_setup_uart_default(void) {
	unsigned int tmpreg;
	unsigned int integerdivider;
	unsigned int fractionaldivider;


	RCC->APB2ENR |= (RCC_APB2ENR_IOPAEN| RCC_APB2ENR_AFIOEN);
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

	GPIO_CONF_OUTPUT_PORT(A, 9, ALT_PUSH_PULL, 50);
	GPIO_CONF_INPUT_PORT(A, 10, FLOATING);

	tmpreg = USART1->CR2;
	tmpreg &= 0xCFFF;	// 1 stop bit
	USART1->CR2 = tmpreg;

	tmpreg = USART1->CR1;
	tmpreg &= 0xE9F3;
	tmpreg |= 0x04 | 0x08;
	USART1->CR1 = tmpreg;

	tmpreg = USART1->CR3;
	tmpreg &= 0xFCFF;
	USART1->CR3 = tmpreg;

	// Set Baudrate
	integerdivider = (((unsigned int)0x19 * SYSCLK) / ((unsigned int)0x04 * 115200));
	fractionaldivider = integerdivider - (0x64 * (tmpreg >> 0x04));
	tmpreg = (integerdivider / 0x64) << 0x04;
	tmpreg |= ((fractionaldivider * 0x10 + 0x32) / 0x64) & (unsigned char)0x0F;

	USART1->BRR = (unsigned short)tmpreg;

	// Enable USART1
	USART1->CR1 |= (unsigned short)0x2000;
	
	// 使能引脚重映射功能
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_AFIOEN;
}

void rs232_send(unsigned char *buf, int length) {
	int i = 0;
	
	while (length--) {
		USART1->DR = buf[i++] & (unsigned short)0x01FF;
		while (!(USART1->SR & (unsigned short)0x0080)) ;
	}
}

void rs232_put(unsigned char c) {
	rs232_send(&c, 1);
}

/**************** Modify END *******************************8*/

