#include "enc28j60.h"
#include "spi-arch.h"
#include <stdio.h>
#include "debug-uart.h"

static unsigned char Enc28j60Bank;
static unsigned short NextPacketPtr;

unsigned char Enc_ReadOp(unsigned char op, unsigned char address)
{
	unsigned char dat = 0;

	Enc_Select();

	dat = op | (address & ADDR_MASK);
	SPI1_RW(dat);
	dat = SPI1_RW(0xFF);
	// do dummy read if needed (for mac and mii, see datasheet page 29)
	if (address & 0x80)
	{
		dat = SPI1_RW(0xFF);
	}
	// release CS
	Enc_NoSelect();
	return dat;
}

void Enc_WriteOp(unsigned char op, unsigned char address, unsigned char data)
{
	unsigned char dat = 0;

	Enc_Select();
	// issue write command
	dat = op | (address & ADDR_MASK);
	SPI1_RW(dat);
	// write data
	dat = data;
	SPI1_RW(dat);
	Enc_NoSelect();
}

void Enc_ReadBuffer(unsigned short len, unsigned char* data)
{
	Enc_Select();
	// issue read command
	SPI1_RW(ENC28J60_READ_BUF_MEM);
	while (len--)
	{
		*data++ = (unsigned char) SPI1_RW(0);
	}
	*data = '\0';
	Enc_NoSelect();
}

void Enc_WriteBuffer(unsigned short len, unsigned char* data)
{
	Enc_Select();
	// issue write command
	SPI1_RW(ENC28J60_WRITE_BUF_MEM);

	while (len--)
	{
		SPI1_RW(*data++);
	}
	Enc_NoSelect();
}

void Enc_SetBank(unsigned char address)
{
	// set the bank (if needed)
	if ((address & BANK_MASK) != Enc28j60Bank)
	{
		// set the bank
		Enc_WriteOp(ENC28J60_BIT_FIELD_CLR, ECON1, (ECON1_BSEL1 | ECON1_BSEL0));
		Enc_WriteOp(ENC28J60_BIT_FIELD_SET, ECON1, (address & BANK_MASK) >> 5);
		Enc28j60Bank = (address & BANK_MASK);
	}
}

unsigned char Enc_Read(unsigned char address)
{
	// set the bank
	Enc_SetBank(address);
	
	// do the read
	return Enc_ReadOp(ENC28J60_READ_CTRL_REG, address);
}

void Enc_Write(unsigned char address, unsigned char data)
{
	// set the bank
	Enc_SetBank(address);
	// do the write
	Enc_WriteOp(ENC28J60_WRITE_CTRL_REG, address, data);
}

void Enc_PhyWrite(unsigned char address, unsigned short data)
{
	// set the PHY register address
	Enc_Write(MIREGADR, address);
	// write the PHY data
 	Enc_Write(MIWRL, data);
	Enc_Write(MIWRH, data >> 8);
	// wait until the PHY write completes
	while (Enc_Read(MISTAT) & MISTAT_BUSY)
	{
		//Del_10us(1);
		//_nop_();
	}
}

void Enc_ClkOut(unsigned char clk)
{
	//setup clkout: 2 is 12.5MHz:
	Enc_Write(ECOCON, clk & 0x7);
}

void Enc_Init(unsigned char* macaddr)
{
	Enc_NoSelect();

	// perform system reset
	Enc_WriteOp(ENC28J60_SOFT_RESET, 0, ENC28J60_SOFT_RESET);
	
	// check CLKRDY bit to see if reset is complete
	// The CLKRDY does not work. See Rev. B4 Silicon Errata point. Just wait.
	while(!(Enc_Read(ESTAT) & ESTAT_CLKRDY));
	
	// do bank 0 stuff
	// initialize receive buffer
	// 16-bit transfers, must write low byte first
	// set receive buffer start address
	NextPacketPtr = RXSTART_INIT;
	// Rx start
	Enc_Write(ERXSTL, RXSTART_INIT & 0xFF);
	Enc_Write(ERXSTH, RXSTART_INIT >> 8);
	// set receive pointer address
	Enc_Write(ERXRDPTL, RXSTART_INIT & 0xFF);
 	Enc_Write(ERXRDPTH, RXSTART_INIT >> 8);
	// RX end
	Enc_Write(ERXNDL, RXSTOP_INIT & 0xFF);
	Enc_Write(ERXNDH, RXSTOP_INIT >> 8);
	// TX start
	Enc_Write(ETXSTL, TXSTART_INIT & 0xFF);
	Enc_Write(ETXSTH, TXSTART_INIT >> 8);
 	// TX end
	Enc_Write(ETXNDL, TXSTOP_INIT & 0xFF);
	Enc_Write(ETXNDH, TXSTOP_INIT >> 8);
	
	// do bank 1 stuff, packet filter:
	// For broadcast packets we allow only ARP packtets
	// All other packets should be unicast only for our mac (MAADR)
	//
	// The pattern to match on is therefore
	// Type     ETH.DST
	// ARP      BROADCAST
	// 06 08 -- ff ff ff ff ff ff -> ip checksum for theses bytes=f7f9
	// in binary these poitions are:11 0000 0011 1111
	// This is hex 303F->EPMM0=0x3f,EPMM1=0x30
	Enc_Write(ERXFCON, 0x00);//ERXFCON_UCEN | ERXFCON_CRCEN | ERXFCON_PMEN);
	Enc_Write(EPMM0, 0x3f);
	Enc_Write(EPMM1, 0x30);
	Enc_Write(EPMCSL, 0xf9);
	Enc_Write(EPMCSH, 0xf7);

	// do bank 2 stuff
	// enable MAC receive
	Enc_Write(MACON1, MACON1_MARXEN | MACON1_TXPAUS | MACON1_RXPAUS);
	// bring MAC out of reset
	Enc_Write(MACON2, 0x00);
	// enable automatic padding to 60bytes and CRC operations
	Enc_WriteOp(ENC28J60_BIT_FIELD_SET, MACON3, MACON3_PADCFG0 | MACON3_TXCRCEN | MACON3_FRMLNEN | MACON3_FULDPX);
	// set inter-frame gap (non-back-to-back)
	Enc_Write(MAIPGL, 0x12);
	Enc_Write(MAIPGH, 0x0C);
	// set inter-frame gap (back-to-back)
	Enc_Write(MABBIPG, 0x15);
	// Set the maximum packet size which the controller will accept
	// Do not send packets longer than MAX_FRAMELEN:
	Enc_Write(MAMXFLL, MAX_FRAMELEN & 0xFF); 
	Enc_Write(MAMXFLH, MAX_FRAMELEN >> 8);
	
	// do bank 3 stuff
	// write MAC address
	// NOTE: MAC address in ENC28J60 is byte-backward
	Enc_Write(MAADR5, macaddr[0]);  
	Enc_Write(MAADR4, macaddr[1]);
	Enc_Write(MAADR3, macaddr[2]);
	Enc_Write(MAADR2, macaddr[3]);
	Enc_Write(MAADR1, macaddr[4]);
	Enc_Write(MAADR0, macaddr[5]);

	Enc_PhyWrite(PHCON1, PHCON1_PDPXMD);

	// no loopback of transmitted frames
	Enc_PhyWrite(PHCON2, PHCON2_HDLDIS);
	// switch to bank 0
	Enc_SetBank(ECON1);
	// enable interrutps
	Enc_WriteOp(ENC28J60_BIT_FIELD_SET, EIE, EIE_INTIE | EIE_PKTIE);
	// enable packet reception
	Enc_WriteOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_RXEN);

	// Ö¸Ê¾µÆ×´Ì¬:0x476 is PHLCON LEDA(ÂÌ)=links status, LEDB(ºì)=receive/transmit
	Enc_PhyWrite(PHLCON, 0x476);
	Enc_ClkOut(2);					// change clkout from 6.25MHz to 12.5MHz
}

// read the revision of the chip:
unsigned char Enc_GetRev(void)
{
	return(Enc_Read(EREVID));
}

void Enc_PacketSend(unsigned short len, unsigned char* packet)
{
	// Set the write pointer to start of transmit buffer area
	Enc_Write(EWRPTL, TXSTART_INIT & 0xFF);
	Enc_Write(EWRPTH, TXSTART_INIT >> 8);

	// Set the TXND pointer to correspond to the packet size given
	Enc_Write(ETXNDL, (TXSTART_INIT + len) & 0xFF);
	Enc_Write(ETXNDH, (TXSTART_INIT + len) >> 8);

	// write per-packet control byte (0x00 means use macon3 settings)
	Enc_WriteOp(ENC28J60_WRITE_BUF_MEM, 0, 0x00);

	// copy the packet into the transmit buffer
	Enc_WriteBuffer(len, packet);

	// send the contents of the transmit buffer onto the network
	Enc_WriteOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_TXRTS);

	// Reset the transmit logic problem. See Rev. B4 Silicon Errata point 12.
	if ((Enc_Read(EIR) & EIR_TXERIF))
	{
		Enc_WriteOp(ENC28J60_BIT_FIELD_CLR, ECON1, ECON1_TXRTS);
	}
}

/*-----------------------------------------------------------------
 Gets a packet from the network receive buffer, if one is available.
 The packet will by headed by an ethernet header.
      maxlen  The maximum acceptable length of a retrieved packet.
      packet  Pointer where packet data should be stored.
 Returns: Packet length in bytes if a packet was retrieved, zero otherwise.
-------------------------------------------------------------------*/
unsigned short Enc_PacketReceive(unsigned short maxlen, unsigned char* packet)
{
	unsigned short rxstat;
	unsigned short len;

	// check if a packet has been received and buffered
	//if( !(Enc_Read(EIR) & EIR_PKTIF) ){
	// The above does not work. See Rev. B4 Silicon Errata point 6.
	if (Enc_Read(EPKTCNT) == 0)
	{
		return(0);
	}

	// Set the read pointer to the start of the received packet
	Enc_Write(ERDPTL, (NextPacketPtr));
	Enc_Write(ERDPTH, (NextPacketPtr) >> 8);

	// read the next packet pointer
	NextPacketPtr = Enc_ReadOp(ENC28J60_READ_BUF_MEM, 0);
	NextPacketPtr |= Enc_ReadOp(ENC28J60_READ_BUF_MEM, 0) << 8;

	// read the packet length (see datasheet page 43)
	len = Enc_ReadOp(ENC28J60_READ_BUF_MEM, 0);
	len |= Enc_ReadOp(ENC28J60_READ_BUF_MEM, 0) << 8;

	len -= 4; //remove the CRC count
	// read the receive status (see datasheet page 43)
	rxstat = Enc_ReadOp(ENC28J60_READ_BUF_MEM, 0);
	rxstat |= Enc_ReadOp(ENC28J60_READ_BUF_MEM, 0) << 8;
	// limit retrieve length
	if (len > maxlen - 1)
	{
		len = maxlen - 1;
	}

	// check CRC and symbol errors (see datasheet page 44, table 7-3):
	// The ERXFCON.CRCEN is set by default. Normally we should not
	// need to check this.
	if ((rxstat & 0x80) == 0)
	{
		// invalid
		len = 0;
	}
	else
	{
		// copy the packet from the receive buffer
		Enc_ReadBuffer(len, packet);
	}
	// Move the RX read pointer to the start of the next received packet
	// This frees the memory we just read out
	Enc_Write(ERXRDPTL, (NextPacketPtr));
	Enc_Write(ERXRDPTH, (NextPacketPtr) >> 8);

	// decrement the packet counter indicate we are done with this packet
	Enc_WriteOp(ENC28J60_BIT_FIELD_SET, ECON2, ECON2_PKTDEC);
	return(len);
}

