/* (c) 2012 g33k @ libxenon.org */

#include "isd2100.h"


void ISD_init(void) {
	/* MOSI */
	PINHIGH(ISDPORT, MOSI);
	PINOUT(ISDDDR, MOSI);

	/* SCK */
	PINHIGH(ISDPORT, SCK);
	PINOUT(ISDDDR, SCK);

	/* SS */
	PINHIGH(ISDPORT, SS);
	PINOUT(ISDDDR, SS);
	
	/* MISO */
	PINHIGH(ISDPORT, MISO);
	PININ(ISDDDR, MISO);

	/* BSY */
	PINHIGH(ISDPORT, BSY);
	PININ(ISDDDR, BSY);

	/* SPI */
	SPCR = (1<<CPHA) | (1<<CPOL) | (1<<SPE) | (1<<MSTR) | (1<<SPR1) | (1<<SPR0);
}


uint8_t ISD_execute(uint8_t cmd, uint8_t *buffer, uint8_t len) {
	uint8_t status;
	
	PINLOW(ISDPORT, SS);
	
	status = ISD_rwByte(cmd);
	while (len--) {
		*buffer = ISD_rwByte(*buffer);
		buffer++;
	}

	PINHIGH(ISDPORT, SS);
	
	return status;
}


uint8_t ISD_startTransfer(uint8_t cmd, uint8_t *buffer, uint8_t len) {
	uint8_t status;
	
	PINLOW(ISDPORT, SS);
	
	status = ISD_rwByte(cmd);
	while (len--) {
		*buffer = ISD_rwByte(*buffer);
		buffer++;
	}
	
	return status;
}


void ISD_stopTransfer(void) {
	PINHIGH(ISDPORT, SS);
}


void ISD_transfer(uint8_t *buffer, uint8_t len) {
	while (len--) {
		*buffer = ISD_rwByte(*buffer);
		buffer++;
	}
}


uint8_t ISD_rwByte(uint8_t out) {
	while (!PINGET(ISDPIN, BSY));
	SPDR = out;
	while (!(SPSR & (1<<SPIF)));
	while (!PINGET(ISDPIN, BSY));
	return SPDR;
}
