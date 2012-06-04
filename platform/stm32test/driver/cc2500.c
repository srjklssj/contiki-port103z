#include <stdio.h>
#include "cc2500.h"
#include "spi-arch.h"
#include "system-basis.h"

void cc2500_cmd(unsigned char cmd) {
	CC2500_SELECT();
	SPI2_RW(cmd);
	CC2500_UNSELECT();
}

void cc2500_wreg(unsigned char addr, unsigned char val) {
	CC2500_SELECT();
	addr &= 0x7F;
	SPI2_RW(addr);
	SPI2_RW(val);
	CC2500_UNSELECT();
}

unsigned char cc2500_rreg(unsigned char addr) {
	unsigned char val;
	
	CC2500_SELECT();
	SPI2_RW(addr | 0x80);
	val = SPI2_RW(0xFF);
	CC2500_UNSELECT();
	return val;
}

void cc2500_init(void) {
	int i;

	// GDO0
	GPIO_CONF_INPUT_PORT(B, 1, PU_PD);

	// reset cc2500
	cc2500_cmd(CCxxx0_SIDLE);
	DELAY_MS(20);
	CC2500_SELECT();
	DELAY_MS(20);
	CC2500_UNSELECT();
	DELAY_MS(40);
	cc2500_cmd(CCxxx0_SRES);

	// write regs
	cc2500_wreg(CCxxx0_FSCAL1,	0x09);
	cc2500_wreg(CCxxx0_FSCTRL0,	0x00);
	cc2500_wreg(CCxxx0_FREQ2,	0x5D);
	cc2500_wreg(CCxxx0_FREQ1,	0x93);
	cc2500_wreg(CCxxx0_FREQ0,	0xB1);
	cc2500_wreg(CCxxx0_MDMCFG4,	0x2D);
	cc2500_wreg(CCxxx0_MDMCFG3,	0x3B);
	cc2500_wreg(CCxxx0_MDMCFG2,	0x73); 
	cc2500_wreg(CCxxx0_MDMCFG1,	0xA2);
	cc2500_wreg(CCxxx0_MDMCFG0,	0xF8);
	cc2500_wreg(CCxxx0_CHANNR,	0x00);
	cc2500_wreg(CCxxx0_DEVIATN,	0x01);
	cc2500_wreg(CCxxx0_FREND1,	0x56);
	cc2500_wreg(CCxxx0_FREND0,	0x10);
	cc2500_wreg(CCxxx0_MCSM1,	0x00);
	cc2500_wreg(CCxxx0_MCSM0,	0x18);
	cc2500_wreg(CCxxx0_FOCCFG,	0x15);
	cc2500_wreg(CCxxx0_BSCFG,	0x6C);
	cc2500_wreg(CCxxx0_AGCCTRL2,	0x07);
	cc2500_wreg(CCxxx0_AGCCTRL1,	0x00);
	cc2500_wreg(CCxxx0_AGCCTRL0,	0x91);
	cc2500_wreg(CCxxx0_FSCAL3,	0xEA);
	cc2500_wreg(CCxxx0_FSCAL2,	0x0A);
	cc2500_wreg(CCxxx0_FSCAL1,	0x00);
	cc2500_wreg(CCxxx0_FSCAL0,	0x11);
	cc2500_wreg(CCxxx0_FSTEST,	0x59);
	cc2500_wreg(CCxxx0_TEST2,	0x8F);
	cc2500_wreg(CCxxx0_TEST1,	0x21);
	cc2500_wreg(CCxxx0_TEST0,	0x0B);
	cc2500_wreg(CCxxx0_IOCFG2,	0x24);
	cc2500_wreg(CCxxx0_IOCFG0,	0x06);
	cc2500_wreg(CCxxx0_PKTCTRL1,	0x00);
	cc2500_wreg(CCxxx0_PKTCTRL0,	0x05);
	cc2500_wreg(CCxxx0_ADDR,	0x00);
	cc2500_wreg(CCxxx0_PKTLEN,	0xFF);

	CC2500_SELECT();
	SPI2_RW(CCxxx0_PATABLE | WRITE_BURST);
	for (i = 0; i < 8; ++i)
		SPI2_RW(0xFF);
	CC2500_UNSELECT();

	// set cc2500 in rx mode
	CC2500_RX_MODE();
}

unsigned char cc2500_read_status(unsigned char addr) {
	unsigned char value;

	CC2500_SELECT();
	SPI2_RW(addr | READ_BURST);
	value = SPI2_RW(0xFF);
	CC2500_UNSELECT();

	return value;
}

int cc2500_receive_packet(unsigned char *buffer) {
	unsigned char status, bytes_in_fifo;
	unsigned char total_bytes, need_to_receive;
	unsigned char i = 0;
	unsigned short timer_counter = 1;

	// make sure cc2500 is working on rx mode
	//status = 0x01 | (cc2500_read_status(CCxxx0_RXBYTES));
	status = cc2500_read_status(CCxxx0_RXBYTES);
	if((status & 0x7F) == 0x00) {
		status = cc2500_read_status(CCxxx0_MARCSTATE);
		if((status != 0x0D) && (status != 0x08))
			CC2500_RX_MODE();
		return 0;
	}
	// rx fifo overflow, flush it
	if (0x80 == (status & 0x80)) {
		CC2500_RX_MODE();
		printf("CC2500:rx fifo overflow\r\n");
		return -1;
	}

	CC2500_SELECT();
	status = SPI2_RW(CCxxx0_RXFIFO_Muti);
	
	bytes_in_fifo = (status & CC2500_FIFO_BYTES_AVAILABLE_BM);
	if (bytes_in_fifo) {
		total_bytes = need_to_receive = SPI2_RW(0xFF);
		--bytes_in_fifo;
		i = 0;
	} else {	// no need to check, but the warning ...
		CC2500_UNSELECT();
		return 0;
	}

	while (need_to_receive) {
		need_to_receive -= bytes_in_fifo;
		while (bytes_in_fifo--)
			buffer[i++] = SPI2_RW(0xFF);
		CC2500_UNSELECT();
		if (need_to_receive) {
			CC2500_SELECT();
			status = SPI2_RW(CCxxx0_RXFIFO_Muti);
			bytes_in_fifo = (status & CC2500_FIFO_BYTES_AVAILABLE_BM);
		}
		if (!(timer_counter++)) {
			printf("CC2500:receive time out!\r\n");
			CC2500_UNSELECT();
			return -1;
		}
	}
	CC2500_UNSELECT();
	return (int)((unsigned int)total_bytes);
	/*
	for(i = 0; i < len; ++i)
		buffer[i] = SPI2_RW(0xFF);
	CC2500_UNSELECT();
	CC2500_RX_MODE();

	return len;
	*/
}

int cc2500_send_packet(unsigned char len, unsigned char *buffer) {
	unsigned char i;
	unsigned char sendp, freespace_in_fifo;
	
	if (len > MAX_PACKET_SIZE) {
		printf("cc2500:packet size too large!\r\n");
		return -1;
	}

	cc2500_cmd(CCxxx0_SIDLE);
	cc2500_cmd(CCxxx0_SFTX);	// flash tx fifo

	sendp = (len < RFTXDBUFFSIZE) ? len : RFTXDBUFFSIZE;
	CC2500_SELECT();
	SPI2_RW(CCxxx0_TXFIFO_Muti);
	SPI2_RW(len);
	for(i = 0; i < sendp; ++i) {
		SPI2_RW(buffer[i]);
	}
	CC2500_UNSELECT();
	cc2500_cmd(CCxxx0_STX);
	
	while (sendp < len) {
		DELAY_US(20);
		freespace_in_fifo = RFTXDBUFFSIZE - (cc2500_read_status(CCxxx0_TXBYTES) & 0x7F);
		freespace_in_fifo = (len - sendp < freespace_in_fifo) ? (len - sendp) : freespace_in_fifo;
		//printf("freespace in fifo %d\r\n", freespace_in_fifo);
		CC2500_SELECT();
		SPI2_RW(CCxxx0_TXFIFO_Muti);
		for (i = 0; i < freespace_in_fifo; ++i)
			SPI2_RW(buffer[sendp++]);
		CC2500_UNSELECT();
	}	

	while(!GDO0_IN());	//wait high
	while(GDO0_IN());	//wait low, end send 

	//DELAY_MS(10);	// no need
	return 0;
}

