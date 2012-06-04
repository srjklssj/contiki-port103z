#include <stdio.h>
#include "cc2520.h"
#include "system-basis.h"

#define CC2520_DO_RST() (GPIOC->ODR &= ~(0x01 << 13))
#define CC2520_UNDO_RST() (GPIOC->ODR |= 0x01 << 13)

#define CC2520_SELECT() (GPIOB->ODR &= ~(0x01 << 10))
#define CC2520_UNSELECT() (GPIOB->ODR |= 0x01 << 10)

#define CC2520_CLK_UP() (GPIOB->ODR |= 0x01 << 7)
#define CC2520_CLK_DOWN() (GPIOB->ODR &= ~(0x01 << 7))

#define MISO_IN() (GPIOB->IDR & (0x01 << 11))
#define MOSI_HIGH() (GPIOB->ODR |= 0x01 << 12)
#define MOSI_LOW() (GPIOB->ODR &= ~(0x01 << 12))

static unsigned char cc2520_spi_read(void) {
	unsigned char i;
	unsigned char tmp = 0x00;

	for (i = 0; i < 8; ++i) {
		tmp = tmp << 1;
		CC2520_CLK_UP();
		if (MISO_IN())
			tmp |= 0x01;
		CC2520_CLK_DOWN();
	}
	return tmp;
}

static void cc2520_spi_write(unsigned char data) {
	unsigned char i;
	
	for (i = 0x80; i != 0;) {
		if ((data) & i)
			MOSI_HIGH();
		else
			MOSI_LOW();
		CC2520_CLK_UP();
		i = i >> 1;
		CC2520_CLK_DOWN();
	}
}

static unsigned char cc2520_spi_rw(unsigned char data) {
	unsigned char i;
	unsigned char tmp = 0x00;

	for (i = 0x80; i != 0;) {
		tmp = tmp << 1;
		if ((data) & i)
			MOSI_HIGH();
		else
			MOSI_LOW();
		CC2520_CLK_UP();
		if (MISO_IN())
			tmp |= 0x01;
		i = i >> 1;
		CC2520_CLK_DOWN();
	}
	return tmp;
}

unsigned char cc2520_command(unsigned char cmd) {
	unsigned char val;
	
	CC2520_SELECT();
	val = cc2520_spi_rw(cmd);
	CC2520_UNSELECT();

	return val;
}

unsigned char cc2520_rreg(unsigned char addr) {
	unsigned char value;

	CC2520_SELECT();
	cc2520_spi_write(addr | REG_READ);
	value = cc2520_spi_read();
	CC2520_UNSELECT();
	
	return value;
}

void cc2520_wreg(unsigned char addr, unsigned char val) {
	CC2520_SELECT();
	cc2520_spi_write(addr | REG_WRITE);
	cc2520_spi_write(val);
	CC2520_UNSELECT();
}

void cc2520_wfifo(unsigned char *buffer, unsigned char length) {
	unsigned char i;
	
	cc2520_command(CMD_SFLUSHTX);
	CC2520_SELECT();
	cc2520_spi_write(TXFIFO_WRITE);
	cc2520_spi_write(length + 2);
	for (i = 0; i < length; ++i)
		cc2520_spi_write(buffer[i]);
	CC2520_UNSELECT();
}

unsigned char cc2520_rfifo(unsigned char *buffer) {
	unsigned char len, i;

	CC2520_SELECT();
	cc2520_spi_write(RXFIFO_READ);
	len = cc2520_spi_read();
	for (i = 0; i < len; ++i)
		buffer[i] = cc2520_spi_read();
	CC2520_UNSELECT();
	cc2520_command(CMD_SFLUSHRX);
	return len;
}

unsigned char cc2520_rram(unsigned char addrh, unsigned char addrl) {
	unsigned char val;

	CC2520_SELECT();
	cc2520_spi_write(addrh | MEM_READ);
	cc2520_spi_write(addrl);
	val = cc2520_spi_read();
	CC2520_UNSELECT();
	
	return val;
}

void cc2520_init(void) {
	// ------------------ GPIO -----------------------//
	// enable cc2520 related gpio and clks
	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;

	// rst -- PC13
	GPIO_CONF_OUTPUT_PORT(C, 13, PUSH_PULL, 50);

	// csn -- PB10  sck -- PB7  so -- PB11  si -- PB12
	GPIO_CONF_OUTPUT_PORT(B, 10, PUSH_PULL, 50);
	GPIO_CONF_OUTPUT_PORT(B, 7, PUSH_PULL, 50);
	GPIO_CONF_INPUT_PORT(B, 11, PU_PD);
	GPIO_CONF_OUTPUT_PORT(B, 12, PUSH_PULL, 50);
	// ----------------- GPIO END----------------------//
	
	// ------------------ CC2520 ---------------------//
	CC2520_UNSELECT();
	CC2520_CLK_DOWN();
	
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
	// --------------- CC2520 END ---------------------//
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

