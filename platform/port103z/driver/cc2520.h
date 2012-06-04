#ifndef __CC2520_H__
#define __CC2520_H__

#include "gpio.h"
#include "stm32f10x_map.h"

// ------------------ cc2520 realated ----------------------- //
#define CC2520_PSDU_Len                127

#define REG_READ                       (0x80)
#define REG_WRITE                      (0xC0)
#define MEM_READ                       (0x10)
#define MEM_WRITE                      (0x20)
#define RXFIFO_READ                    (0x30)
#define TXFIFO_WRITE                   (0x3A)
// ------------------ cc2520 cmd ----------------------- //
#define CMD_SNOP                       (0x00)
#define CMD_IBUFLD                     (0x02)
#define CMD_SIBUFEX            	       (0x03)
#define CMD_SSAMPLECCA                 (0x04)
#define CMD_SRES                       (0x09)

#define CMD_SXOSCON                    (0x40)
#define CMD_SRXON                      (0x42)
#define CMD_STXON                      (0x43)
#define CMD_SRFOFF                     (0x45)

#define CMD_SFLUSHRX                   (0x47)
#define CMD_SFLUSHTX                   (0x48)
// ------------------------ regs --------------------------- //
#define FRMFILT0                       (0x00)
#define FRMFILT1                       (0x01)
#define SRCMATCH                       (0x02)
#define SRCSHORTEN0                    (0x04)
#define SRCSHORTEN1                    (0x05)
#define SRCSHORTEN2                    (0x06)
#define SRCEXTEN0                      (0x08)
#define SRCEXTEN1                      (0x09)
#define SRCEXTEN2                      (0x0A)
#define FRMCTRL0                       (0x0C)
#define FRMCTRL1                       (0x0D)
#define RXENABLE0                      (0x0E)
#define RXENABLE1                      (0x0F)
#define EXCFLAG0                       (0x10)
#define EXCFLAG1                       (0x11)
#define EXCFLAG2                       (0x12)
#define EXCMASKA0                      (0x14)
#define EXCMASKA1                      (0x15)
#define EXCMASKA2                      (0x16)
#define EXCMASKB0                      (0x18)
#define EXCMASKB1                      (0x19)
#define EXCMASKB2                      (0x1A)
#define EXCBINDX0                      (0x1C)
#define EXCBINDX1                      (0x1D)
#define EXCBINDY0                      (0x1E)
#define EXCBINDY1                      (0x1F)
#define GPIOCTRL0                      (0x20)
#define GPIOCTRL1                      (0x21)
#define GPIOCTRL2                      (0x22)
#define GPIOCTRL3                      (0x23)
#define GPIOCTRL4                      (0x24)
#define GPIOCTRL5                      (0x25)
#define GPIOPOLARITY                   (0x26)
#define GPIOCTRL                       (0x28)
#define DPUCON                         (0x2A)
#define DPUSTAT                        (0x2C)
#define FREQCTRL                       (0x2E)
#define FREQTUNE                       (0x2F)
#define TXPOWER                        (0x30)
#define FSMSTAT0                       (0x32)
#define FSMSTAT1                       (0x33)
#define FIFOPCTRL                      (0x34)
#define FSMCTRL                        (0x35)
#define CCACTRL0                       (0x36)
#define CCACTRL1                       (0x37)
#define RSSI                           (0x38)
#define RSSISTAT                       (0x39)

#define RXFIRST                        (0x3C)
#define RXFIFOCNT                      (0x3E)
#define TXFIFOCNT                      (0x3F)
// -------------------------- PSDU ----------------------------- //
#define FRAME_TYPE_BEACCON              0x00
#define FRAME_TYPE_DATA                 0x01
#define FRAME_TYPE_ACK                  0x02
#define FRAME_TYPE_MAC                  0x03

#define SECURITY_ENABLE                 0x00
#define FRAME_PENDING                   0x00
#define ACKNOWLEDGMENT_REQUEST          0x00
#define PAN_ID_COMPRESSION              0x00
#define DEST_ADDRESSING_MODE            0x03
#define FRAME_VERSION                   0x01
#define SOURCE_ADDRESSING_MODE          0x03

#define SEQUENCE_NUMBER                 0x02
// -------------------------- RAM ----------------------------- //
#define RAM_IEEEADR                    (0xEA)
#define RAM_PANID                      (0xF2)	
#define RAM_SHORTADR                   (0xF4)
// ------------------ Exception Flags ------------------------- //
// EXCFLAG0
#define CC2520_RF_IDLE                 (0x01)
#define CC2520_TX_FRM_DONE             (0x02)
#define CC2520_TX_ACK_DONE             (0x04)
#define CC2520_TX_UNDERFLOW            (0x08)
#define CC2520_TX_OVERFLOW             (0x10)
#define CC2520_RX_UNDERFLOW            (0x20)
#define CC2502_RX_OVERFLOW             (0x40)
#define CC2520_RXENABLE_ZERO           (0x80)
// EXCFLAG1
#define CC2520_RX_FRM_DONE             (0x01)
#define CC2520_RX_FRM_ACCEPTED         (0x02)
#define CC2520_SRC_MATCH_DONE          (0x04)
#define CC2520_SRC_MATCH_FOUND         (0x08)
#define CC2520_FIFOP                   (0x10)
#define CC2520_SFD                     (0x20)
#define CC2520_DPU_DONE_L              (0x40)
#define CC2520_DPU_DONE_H              (0x80)
// EXCFLAG2
#define CC2520_MEMADDR_ERROR           (0x01)
#define CC2520_USAGE_ERROR             (0x02)
#define CC2520_OPERAND_ERROR           (0x04)
#define CC2520_SPI_ERROR               (0x08)
#define CC2520_RF_NO_LOCK              (0x10)
#define CC2520_RX_FRM_ABORTED          (0x20)
#define CC2520_RXBUFMOV_TIMEOUT        (0x40)
#define CC2520_UNUSED                  (0x80)

#define cc2520_txpacket() do {							\
			cc2520_wreg(EXCFLAG0, ~CC2520_TX_FRM_DONE);		\
			cc2520_command(CMD_SRFOFF);				\
			cc2520_command(CMD_STXON);				\
			while (!(cc2520_rreg(EXCFLAG0) & CC2520_TX_FRM_DONE));	\
			cc2520_wreg(EXCFLAG0, ~CC2520_TX_FRM_DONE);		\
		} while (0);
#define cc2520_haspacket() (cc2520_rreg(EXCFLAG1) & CC2520_RX_FRM_DONE)
#define cc2520_set_rxmode() do {					\
			cc2520_command(CMD_SRFOFF);			\
			cc2520_wreg(EXCFLAG1, ~CC2520_RX_FRM_DONE);	\
			cc2520_command(CMD_SRXON);			\
		} while (0);

void cc2520_dbg_pbuffer(unsigned char *buffer, unsigned char length);

void cc2520_init(void);
unsigned char cc2520_command(unsigned char cmd);
unsigned char cc2520_rreg(unsigned char addr);
void cc2520_wreg(unsigned char addr, unsigned char val);
void cc2520_wfifo(unsigned char *buffer, unsigned char length);
unsigned char cc2520_rfifo(unsigned char *buffer);
unsigned char cc2520_rram(unsigned char addrh, unsigned char addrl);

#endif

