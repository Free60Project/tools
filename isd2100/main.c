/* (c) 2012 g33k @ libxenon.org */

#include "main.h"


uint8_t buffer[EPSIZE];
uint32_t toread;
uint32_t towrite;


int main(void) {
	SetupHardware();
	sei();

	toread = 0;
	towrite = 0;
	for (;;) {
		ISD21_Task();
		USB_USBTask();
	}
}


void SetupHardware(void) {
	MCUSR &= ~(1 << WDRF);
	wdt_disable();

	clock_prescale_set(clock_div_1);
	
	USB_Init();
	ISD_init();
}


void ISD21_Task() {
	uint8_t cnt;
	
	if (USB_DeviceState != DEVICE_STATE_Configured) {
		return;
	}
	
	/* IN */
	if (toread) {
		Endpoint_SelectEndpoint(FLASHER_STREAM_IN_EPNUM);
		if (Endpoint_IsConfigured() && Endpoint_IsINReady() && Endpoint_IsReadWriteAllowed()) {
			cnt = MIN(EPSIZE, toread);
			memset(buffer, 0, cnt);
			ISD_transfer(buffer, cnt);

			Endpoint_Write_Stream_LE(buffer, cnt, NULL);
			Endpoint_ClearIN();

			toread -= cnt;
			if (!toread) {
				ISD_stopTransfer();
			}
		}
	}
	
	
	/* OUT */
	if (towrite) {
		Endpoint_SelectEndpoint(FLASHER_STREAM_OUT_EPNUM);
		if (Endpoint_IsConfigured() && Endpoint_IsOUTReceived() && Endpoint_IsReadWriteAllowed()) {
			cnt = MIN(EPSIZE, towrite);
			Endpoint_Read_Stream_LE(buffer, cnt, NULL);
			Endpoint_ClearIN();

			ISD_transfer(buffer, cnt);

			towrite -= cnt;
			if (!towrite) {
				ISD_stopTransfer();
			}
		}
	}
}


void EVENT_USB_Device_ConfigurationChanged(void)  {
	if (!(Endpoint_ConfigureEndpoint(FLASHER_STREAM_OUT_EPNUM, EP_TYPE_BULK, EPSIZE, 1))) {
	}	
	
	if (!(Endpoint_ConfigureEndpoint(FLASHER_STREAM_IN_EPNUM, EP_TYPE_BULK, EPSIZE, 1))) {
	}
}


void EVENT_USB_Device_ControlRequest(void) {
	if(USB_ControlRequest.bmRequestType != (REQDIR_HOSTTODEVICE | REQTYPE_VENDOR)) {
		return;
	}
	
	Endpoint_ClearSETUP();
	
	uint8_t data[8];
	/* arg0 big-endian, arg1 little-endian */
	Endpoint_Read_Control_Stream_LE(&data, sizeof(data));
	Endpoint_ClearIN();
	
	
	switch(USB_ControlRequest.bRequest) {
		case REQ_STATUS:
			getStatus();
			break;

		case REQ_INT:
			getInt();
			break;

		case REQ_POWERUP:
			ISD_execute(ISD21_PWRUP, NULL, 0);
			break;

		case REQ_POWERDOWN:
			ISD_execute(ISD21_PWRDWN, NULL, 0);
			break;

		case REQ_RESET:
			ISD_execute(ISD21_RESET, NULL, 0);
			break;

		case REQ_ID:
			getId();
			break;

		case REQ_READ:
			toread = *((uint32_t*)(&data[4]));
			ISD_startTransfer(ISD21_READ, &data[1], 3);
			break;

		case REQ_WRITE:
			towrite = *((uint32_t*)(&data[4]));
			ISD_startTransfer(ISD21_WRITE, &data[1], 3);
			break;

		case REQ_FLUSH:
			buffer[0] = 0x01;
			ISD_execute(ISD21_FLUSH, buffer, 1);
			break;

		case REQ_VP:
			ISD_execute(ISD21_VP, &data[2], 2);
			break;

		case REQ_VPRN:
			ISD_execute(ISD21_VPRN, &data[3], 1);
			break;

		case REQ_VM:
			ISD_execute(ISD21_VM, &data[2], 2);
			break;

		case REQ_VMRN:
			ISD_execute(ISD21_VMRN, &data[3], 1);
			break;

		case REQ_STOP:
			ISD_execute(ISD21_STOP, NULL, 0);
			break;
	}
}


void getStatus(void) {
	uint8_t ep = Endpoint_GetCurrentEndpoint();
	Endpoint_SelectEndpoint(FLASHER_STREAM_IN_EPNUM);

	if (Endpoint_IsConfigured() && Endpoint_IsINReady() && Endpoint_IsReadWriteAllowed()) {
		buffer[0] = ISD_execute(ISD21_STATUS, NULL, 0);
		Endpoint_Write_Stream_LE(buffer, 1, NULL);
		Endpoint_ClearIN();
	}

	Endpoint_SelectEndpoint(ep);
}


void getInt(void) {
	uint8_t ep = Endpoint_GetCurrentEndpoint();
	Endpoint_SelectEndpoint(FLASHER_STREAM_IN_EPNUM);

	if (Endpoint_IsConfigured() && Endpoint_IsINReady() && Endpoint_IsReadWriteAllowed()) {
		memset(buffer, 0, 2);
		buffer[0] = ISD_execute(ISD21_INT, &buffer[1], 1);
		Endpoint_Write_Stream_LE(buffer, 2, NULL);
		Endpoint_ClearIN();
	}

	Endpoint_SelectEndpoint(ep);
}


void getId(void) {
	uint8_t ep = Endpoint_GetCurrentEndpoint();
	Endpoint_SelectEndpoint(FLASHER_STREAM_IN_EPNUM);

	if (Endpoint_IsConfigured() && Endpoint_IsINReady() && Endpoint_IsReadWriteAllowed()) {
		memset(buffer, 0, 4);
		ISD_execute(ISD21_ID, buffer, 4);
		Endpoint_Write_Stream_LE(buffer, 4, NULL);
		Endpoint_ClearIN();
	}

	Endpoint_SelectEndpoint(ep);
}
