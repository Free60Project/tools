#include "GenericTypeDefs.h"
#include "Compiler.h"
#include "XSPI.h"

BYTE eraseCycle = 0x00;

BOOL XNANDWaitReady(WORD timeout)
{
	do {
		if (!(XSPIReadByte(0x04) & 0x01))
			return TRUE;
	} while (timeout--);

	return FALSE;
}

WORD XNANDGetStatus()
{
	return XSPIReadWord(0x04);
}

WORD XNANDClearStatus()
{
	BYTE tmp[4];

	XSPIRead(4, tmp);
	XSPIWrite(4, tmp);	
}

WORD XNANDReadStart(DWORD block)
{
	WORD res;
	WORD tries = 0x1000;

	XNANDClearStatus();

	XSPIWriteDWORD(0x0C, block << 9);

	XSPIWriteByte(0x08, 0x03);

	if (!XNANDWaitReady(0x1000))
		return 0x8011;

	res = XNANDGetStatus();

	XSPIWrite0(0x0C);

	return res;
}

void XNANDReadProcess(BYTE *buffer, BYTE words) {
	while (words--) {
		XSPIWrite0(0x08);
		XSPIRead(0x10, buffer);
		buffer+=4;
	}
}

WORD XNANDErase(DWORD block)
{
	WORD res;
	BYTE tmp[4];
// when block bitwises anded with 7 is anything but 0 don't erase the block.	
	if(eraseCycle != 0)
	{
		res = (block & (eraseCycle));
//		if(!(block && eraseCycle)) // test to see if erase block level reached
		if(res != 0)
		{
//			 XNANDClearStatus();
//			 return XNANDGetStatus();
			return 0x200; // this may well screw things up
		}
	}

	XNANDClearStatus();

	XSPIRead(0, tmp);
	tmp[0] |= 0x08;
	XSPIWrite(0, tmp);

	XSPIWriteDWORD(0x0C, block << 14);

	if (!XNANDWaitReady(0x1000))
		return 0x8001;

	XSPIWriteByte(0x08, 0xAA);
	XSPIWriteByte(0x08, 0x55);

	if (!XNANDWaitReady(0x1000))
		return 0x8002;

	XSPIWriteByte(0x08, 0x5);

	if (!XNANDWaitReady(0x1000))
		return 0x8003;

	return XNANDGetStatus();	
}

void XNANDWriteStart()
{
	XNANDClearStatus();
	XSPIWrite0(0x0C);
}

void XNANDWriteProcess(BYTE *buffer, BYTE words) {
	while (words--) {
		XSPIWrite(0x10, buffer);
		XSPIWriteByte(0x08, 0x01);
		buffer += 4;
	}
}

WORD XNANDWriteExecute(DWORD block) {
	WORD res;
	WORD tries = 0x1000;

	XSPIWriteDWORD(0x0C, block << 9);

	XSPIWriteByte(0x08, 0x55);
	XSPIWriteByte(0x08, 0xAA);
	XSPIWriteByte(0x08, 0x4);

	if (!XNANDWaitReady(0x1000))
		return 0x8021;

	return XNANDGetStatus();	
}
