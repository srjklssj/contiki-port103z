#ifndef __CC2500_H__
#define __CC2500_H__

#include "stm32f10x_map.h"

#define CCxxx0_IOCFG2		0x00	// GDO2 output pin configuration
#define CCxxx0_IOCFG1		0x01	// GDO1 output pin configuration
#define CCxxx0_IOCFG0		0x02	// GDO0 output pin configuration
#define CCxxx0_FIFOTHR		0x03	// RX FIFO and TX FIFO thresholds
#define CCxxx0_SYNC1		0x04	// Sync word, high byte
#define CCxxx0_SYNC0		0x05	// Sync word, low byte
#define CCxxx0_PKTLEN		0x06	// Packet length
#define CCxxx0_PKTCTRL1		0x07	// Packet automation control
#define CCxxx0_PKTCTRL0		0x08	// Packet automation control
#define CCxxx0_ADDR		0x09	// Device address
#define CCxxx0_CHANNR		0x0A	// Channel number
#define CCxxx0_FSCTRL1		0x0B	// Frequency synthesizer control
#define CCxxx0_FSCTRL0		0x0C	// Frequency synthesizer control
#define CCxxx0_FREQ2		0x0D	// Frequency control word, high byte
#define CCxxx0_FREQ1		0x0E	// Frequency control word, middle byte
#define CCxxx0_FREQ0		0x0F	// Frequency control word, low byte
#define CCxxx0_MDMCFG4		0x10	// Modem configuration
#define CCxxx0_MDMCFG3		0x11	// Modem configuration
#define CCxxx0_MDMCFG2		0x12	// Modem configuration
#define CCxxx0_MDMCFG1		0x13	// Modem configuration
#define CCxxx0_MDMCFG0		0x14	// Modem configuration
#define CCxxx0_DEVIATN		0x15	// Modem deviation setting
#define CCxxx0_MCSM2		0x16	// Main Radio Control State Machine configuration
#define CCxxx0_MCSM1		0x17	// Main Radio Control State Machine configuration
#define CCxxx0_MCSM0		0x18	// Main Radio Control State Machine configuration
#define CCxxx0_FOCCFG		0x19	// Frequency Offset Compensation configuration
#define CCxxx0_BSCFG		0x1A	// Bit Synchronization configuration
#define CCxxx0_AGCCTRL2		0x1B	// AGC control
#define CCxxx0_AGCCTRL1		0x1C	// AGC control
#define CCxxx0_AGCCTRL0		0x1D	// AGC control
#define CCxxx0_WOREVT1 		0x1E	// High byte Event 0 timeout
#define CCxxx0_WOREVT0 		0x1F	// Low byte Event 0 timeout
#define CCxxx0_WORCTRL 		0x20	// Wake On Radio control
#define CCxxx0_FREND1		0x21	// Front end RX configuration
#define CCxxx0_FREND0		0x22	// Front end TX configuration
#define CCxxx0_FSCAL3		0x23	// Frequency synthesizer calibration
#define CCxxx0_FSCAL2		0x24	// Frequency synthesizer calibration
#define CCxxx0_FSCAL1		0x25	// Frequency synthesizer calibration
#define CCxxx0_FSCAL0		0x26	// Frequency synthesizer calibration
#define CCxxx0_RCCTRL1 		0x27	// RC oscillator configuration
#define CCxxx0_RCCTRL0 		0x28	// RC oscillator configuration

//------------------------ control reg ---------------------------//
#define CCxxx0_FSTEST		0x29	// Frequency synthesizer calibration control
#define CCxxx0_PTEST		0x2A	// Production test
#define CCxxx0_AGCTEST 		0x2B	// AGC test
#define CCxxx0_TEST2		0x2C	// Various test settings
#define CCxxx0_TEST1		0x2D	// Various test settings
#define CCxxx0_TEST0		0x2E	// Various test settings

// ------------- Command --------------//
#define CCxxx0_SRES 		0x30	// Reset chip.
#define CCxxx0_SFSTXON		0x31	// Enable and calibrate frequency synthesizer (if MCSM0.FS_AUTOCAL=1).
                                	// If in RX/TX: Go to a wait state where only the synthesizer is
                                	// running (for quick RX / TX turnaround).
#define CCxxx0_SXOFF		0x32	// Turn off crystal oscillator.
#define CCxxx0_SCAL 		0x33	// Calibrate frequency synthesizer and turn it off
                                	// (enables quick start).
#define CCxxx0_SRX  		0x34	// Enable RX. Perform calibration first if coming from IDLE and
                                	// MCSM0.FS_AUTOCAL=1.
#define CCxxx0_STX  		0x35	// In IDLE state: Enable TX. Perform calibration first if
                                	// MCSM0.FS_AUTOCAL=1. If in RX state and CCA is enabled:
                                	// Only go to TX if channel is clear.
#define CCxxx0_SIDLE		0x36	// Exit RX / TX, turn off frequency synthesizer and exit
                                	// Wake-On-Radio mode if applicable.
#define CCxxx0_SAFC 		0x37	// Perform AFC adjustment of the frequency synthesizer
#define CCxxx0_SWOR		0x38	// Start automatic RX polling sequence (Wake-on-Radio)
#define CCxxx0_SPWD 		0x39	// Enter power down mode when CSn goes high.
#define CCxxx0_SFRX 		0x3A	// Flush the RX FIFO buffer.
#define CCxxx0_SFTX 		0x3B	// Flush the TX FIFO buffer.
#define CCxxx0_SWORRST		0x3C	// Reset real time clock.
#define CCxxx0_SNOP 		0x3D	// No operation. May be used to pad strobe commands to two
                                	// bytes for simpler software.

// --------- information reg -------- //
#define CCxxx0_PARTNUM 		0x30
#define CCxxx0_VERSION 		0x31
#define CCxxx0_FREQEST 		0x32
#define CCxxx0_LQI   		0x33
#define CCxxx0_RSSI  		0x34
#define CCxxx0_MARCSTATE	0x35
#define CCxxx0_WORTIME1		0x36
#define CCxxx0_WORTIME0		0x37
#define CCxxx0_PKTSTATUS	0x38
#define CCxxx0_VCO_VC_DAC	0x39
#define CCxxx0_TXBYTES 		0x3A
#define CCxxx0_RXBYTES 		0x3B

// --------- function reg ------------ //
#define CCxxx0_PATABLE		0x3E         //set PA value
#define CCxxx0_TXFIFO_One	0x3F
#define CCxxx0_TXFIFO_Muti	0x7f
#define CCxxx0_RXFIFO_Muti	0xff
#define CCxxx0_RXFIFO_one	0xbF
// ----------------------------------- end ------------------------------------------ //

#define WRITE_BURST		0x40
#define READ_SINGLE		0x80
#define READ_BURST		0xC0

#define CC2500_FIFO_BYTES_AVAILABLE_BM	0x0F

#define RFRXDBUFFSIZE		64
#define RFTXDBUFFSIZE		64

#define MAX_PACKET_SIZE		255

// needs wait until cc2500 'so' pin become low.
#define CC2500_SELECT()		({GPIOA->ODR &= ~(0x01 << 11); \
					while (GPIOB->IDR & (0x01 << 14));})

#define CC2500_UNSELECT()	(GPIOA->ODR |= 0x01 << 11)

// set cc2500 in rx mode
#define CC2500_RX_MODE() do {					\
				cc2500_cmd(CCxxx0_SIDLE);	\
				cc2500_cmd(CCxxx0_SFRX);	\
				cc2500_cmd(CCxxx0_SRX);		\
			} while (0)

#define GDO0_IN() (GPIOB->IDR & (0x01 << 1))

void cc2500_cmd(unsigned char cmd);
void cc2500_wreg(unsigned char addr, unsigned char val);
unsigned char cc2500_rreg(unsigned char addr);

void cc2500_init(void);
unsigned char cc2500_read_status(unsigned char addr);
int cc2500_receive_packet(unsigned char *buffer);
int cc2500_send_packet(unsigned char len, unsigned char *buffer);

#endif

