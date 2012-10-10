/* (c) 2012 g33k @ libxenon.org */

#ifndef _ISD2100_H_
#define _ISD2100_H_


#include <avr/io.h>

#include <inttypes.h>


#define ISDDDR	DDRB
#define ISDPORT	PORTB
#define ISDPIN	PINB

#define SS		0
#define SCK		1
#define MOSI	2
#define MISO	3
#define BSY		4

#define PINOUT(DDR, PIN)		(DDR |= (1 << PIN))
#define PININ(DDR, PIN)			(DDR &= ~(1 << PIN))
#define PINHIGH(PORT, PIN)	(PORT |= (1 << PIN))
#define PINLOW(PORT, PIN)		(PORT &= ~(1 << PIN))
#define PINGET(PORT, PIN)		(PORT & (1 << PIN))


#define ISD21_STATUS 0x40
#define ISD21_INT    0x46
#define ISD21_PWRUP  0x10
#define ISD21_PWRDWN 0x12
#define ISD21_RESET  0x14
#define ISD21_ID     0x48
#define ISD21_READ   0xA2
#define ISD21_WRITE  0xA0
#define ISD21_FLUSH  0x26
#define ISD21_VP     0xA6
#define ISD21_VPRN   0xAE
#define ISD21_VM     0xB0
#define ISD21_VMRN   0xBC
#define ISD21_STOP   0x2A

void ISD_init(void);
uint8_t ISD_execute(uint8_t cmd, uint8_t *buffer, uint8_t len);
uint8_t ISD_startTransfer(uint8_t cmd, uint8_t *buffer, uint8_t len);
void ISD_stopTransfer(void);
void ISD_transfer(uint8_t *buffer, uint8_t len);
uint8_t ISD_rwByte(uint8_t out);

#endif
