#include <stdio.h>
#include "spi-arch.h"
#include "cc2520.h"
#include "system-basis.h"

// PB8 <--> RST
#define CC2520_DO_RST() (GPIOB->ODR &= ~(0x01 << 8))
#define CC2520_UNDO_RST() (GPIOB->ODR |= 0x01 << 8)

// PB12 <--> NSS
#define CC2520_SELECT() (GPIOB->ODR &= ~(0x01 << 12))
#define CC2520_UNSELECT() (GPIOB->ODR |= 0x01 << 12)

#define DUMMY 0xFF

unsigned char cc2520_command(unsigned char cmd) {
	unsigned char val;
	
	CC2520_SELECT();
	val = SPI2_RW(cmd);
	CC2520_UNSELECT();

	return val;
}

unsigned char cc2520_rreg(unsigned char addr) {
	unsigned char value;

	CC2520_SELECT();
	SPI2_RW(addr | REG_READ);
	value = SPI2_RW(DUMMY);
	CC2520_UNSELECT();
	
	return value;
}

void cc2520_wreg(unsigned char addr, unsigned char val) {
	CC2520_SELECT();
	SPI2_RW(addr | REG_WRITE);
	SPI2_RW(val);
	CC2520_UNSELECT();
}

void cc2520_wfifo(unsigned char *buffer, unsigned char length) {
	unsigned char i;
	
	cc2520_command(CMD_SFLUSHTX);
	CC2520_SELECT();
	SPI2_RW(TXFIFO_WRITE);
	SPI2_RW(length + 2);
	for (i = 0; i < length; ++i)
		SPI2_RW(buffer[i]);
	CC2520_UNSELECT();
}

unsigned char cc2520_rfifo(unsigned char *buffer) {
	unsigned char len, i;

	CC2520_SELECT();
	SPI2_RW(RXFIFO_READ);
	len = SPI2_RW(DUMMY);
	for (i = 0; i < len; ++i)
		buffer[i] = SPI2_RW(DUMMY);
	CC2520_UNSELECT();
	cc2520_command(CMD_SFLUSHRX);
	return len;
}

unsigned char cc2520_rram(unsigned char addrh, unsigned char addrl) {
	unsigned char val;

	CC2520_SELECT();
	SPI2_RW(addrh | MEM_READ);
	SPI2_RW(addrl);
	val = SPI2_RW(DUMMY);
	CC2520_UNSELECT();
	
	return val;
}

void cc2520_init(void) {
// ------------------------------------- GPIO ----------------------------------------//
	// enable cc2520 related gpio and clks
	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;

	// RST -- PB8
	GPIO_CONF_OUTPUT_PORT(B, 8, PUSH_PULL, 50);
// ---------------------------------- GPIO END ----------------------------------------//
	
// ------------------------------------ CC2520 ----------------------------------------//
	CC2520_UNSELECT();
	
	// reset
	CC2520_DO_RST();
	DELAY_MS(10);
	CC2520_UNDO_RST();
	DELAY_MS(10);

	// ocs
	cc2520_command(CMD_SXOSCON);
	DELAY_MS(10);
	
	//cc2520_wreg(FRMFILT0, 0x00);	// no frame filtering
	cc2520_command(CMD_SFLUSHRX);
	cc2520_command(CMD_SFLUSHTX);
	DELAY_MS(10);
// ---------------------------------- CC2520 END --------------------------------------//
}

void cc2520_dbg_pbuffer(unsigned char *buffer, unsigned char length) {
	unsigned char i, j = 0;
	
	for (i = 0; i < length; ++i) {
		if ((0 == i % 16) && (0 != i)) {
			printf("   ");
			while (j != i) {
				if ((buffer[j] >= 32) && (buffer[j] < 127))
					printf("%c", buffer[j]);
				else
					printf(".");
				++j;
			}
			printf("\r\n");
		}
		printf("%02x ", buffer[i]);
	}
	j = i;
	while (i++ % 16) {
		printf("   ");
	}
	i = j;
	while (j-- % 16);
	j++;
	printf ("   ");
	while (j != i) {
		if ((buffer[j] >= 32) && (buffer[j] < 127))
			printf("%c", buffer[j]);
		else
			printf(".");
		++j;
	}
	printf("\r\n");
}

