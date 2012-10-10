/* (c) 2012 g33k @ libxenon.org */

#ifndef _MAIN_H_
#define _MAIN_H_

#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/power.h>
#include <avr/wdt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include <LUFA/Version.h>
#include <LUFA/Drivers/Board/Buttons.h>
#include <LUFA/Drivers/Board/LEDs.h>
#include <LUFA/Drivers/Peripheral/Serial.h>
#include <LUFA/Drivers/USB/USB.h>

#include "Descriptors.h"
#include "isd2100.h"


/* Vendor specific requests */
#define REQ_STATUS		0x80
#define REQ_INT  			0x81
#define REQ_POWERUP		0x82
#define REQ_POWERDOWN	0x83
#define REQ_RESET			0x84
#define REQ_ID				0x85
#define REQ_READ			0x86
#define REQ_WRITE			0x87
#define REQ_FLUSH			0x88
#define REQ_VP				0x89
#define REQ_VPRN			0x8A
#define REQ_VM				0x8B
#define REQ_VMRN			0x8C
#define REQ_STOP			0x8D


void SetupHardware(void);
void ISD21_Task(void);

void getStatus(void);
void getInt(void);
void getId(void);
void readFlash(uint8_t *data);

void EVENT_USB_Device_ConfigurationChanged(void);
void EVENT_USB_Device_Connect(void);
void EVENT_USB_Device_Disconnect(void);
void EVENT_USB_Device_ConfigurationChanged(void);
void EVENT_USB_Device_UnhandledControlRequest(void);

#endif
