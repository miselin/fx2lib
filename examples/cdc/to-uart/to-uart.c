/**
 * Copyright (C) 2009 Ubixum, Inc. 
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 **/
#include <stdio.h>

#include <fx2regs.h>
#include <fx2macros.h>
#include <serial.h>
#include <delay.h>
#include <autovector.h>
#include <lights.h>
#include <setupdat.h>
#include <eputils.h>
#include <i2c.h>

#include "cdc.h"

#define SYNCDELAY SYNCDELAY4
#define REARMVAL 0x80
#define REARM() EP2BCL=REARMVAL

// -----------------------------------------------------------------------

BOOL cdcuser_set_line_rate(DWORD baud_rate) {
        if (baud_rate > 115200 || baud_rate < 2400)
            baud_rate = 115200;
	sio0_init(baud_rate);
	return TRUE;
}

void cdcuser_receive_data(BYTE* data, WORD length) {
        WORD i;
        for (i=0; i < length ; ++i) {
		SBUF0 = data[i];
		while(TI);
	}
}

void ISR_USART0(void) __interrupt 4 __critical {
	if (RI) {
		RI=0;
		if (!cdc_can_send()) {
			// Mark overflow
		} else {
			cdc_queue_data(SBUF0);
		}
		// FIXME: Should use a timer, rather then sending one byte at a
		// time.
		cdc_send_queued_data();
	}
	if (TI) {
		TI=0;
//		transmit();
	}
}

// -----------------------------------------------------------------------

volatile WORD bytes;
volatile __bit gotbuf;
volatile BYTE icount;
volatile __bit got_sud;
DWORD lcount;
__bit on;
volatile char d;

char hex(BYTE value) {
	if (value > 0x0f) {
		return '?';
	} else if (value > 0x09) {
		return 'a'+(value-0x0a);
	} else {
		return '0'+value;
	}
}

extern __xdata char dev_serial[];
void patch_serial(BYTE index, BYTE value) {
	dev_serial[index*4] = hex(value >> 4);
	dev_serial[index*4+2] = hex(value & 0xf);
}

void main() {
	BYTE tempbyte = 0;
	REVCTL=0; // not using advanced endpoint controls

	on=0;
	lcount=0;
	got_sud=FALSE;
	icount=0;
	gotbuf=FALSE;
	bytes=0;

	// renumerate
 	RENUMERATE_UNCOND(); 
 
	SETCPUFREQ(CLK_48M);
	SETIF48MHZ();
 
	USE_USB_INTS(); 
	ENABLE_SUDAV();
	ENABLE_SOF();
	ENABLE_HISPEED();
	ENABLE_USBRESET();

	dev_serial[0] = 'f';
	//pSerial[2] = ((WORD)'e') << 8;

        eeprom_read(0x51, 0xf8, 1, &tempbyte);
	patch_serial(0, tempbyte);
        eeprom_read(0x51, 0xf8+1, 1, &tempbyte);
	patch_serial(1, tempbyte);
        eeprom_read(0x51, 0xf8+2, 1, &tempbyte);
	patch_serial(2, tempbyte);
        eeprom_read(0x51, 0xf8+3, 1, &tempbyte);
	patch_serial(3, tempbyte);
        eeprom_read(0x51, 0xf8+4, 1, &tempbyte);
	patch_serial(4, tempbyte);
        eeprom_read(0x51, 0xf8+5, 1, &tempbyte);
	patch_serial(5, tempbyte);
        eeprom_read(0x51, 0xf8+6, 1, &tempbyte);
	patch_serial(6, tempbyte);
        eeprom_read(0x51, 0xf8+7, 1, &tempbyte);
	patch_serial(7, tempbyte);
 
	// only valid endpoints are 2/6
	// Activate, OUT Direction, BULK Type, 512  bytes Size, Double buffered
	CDC_H2D_EP(CFG) = 0xA2; // 10100010
	SYNCDELAY;
	// Activate, IN  Direction, BULK Type, 512  bytes Size, Double buffered
	CDC_D2H_EP(CFG) = 0xE2; // 11100010
	SYNCDELAY;

	EP1INCFG &= ~bmVALID;
	SYNCDELAY;
	EP1OUTCFG &= ~bmVALID;
	SYNCDELAY;

	EP4CFG &= ~bmVALID;
	SYNCDELAY;

	EP8CFG &= ~bmVALID;
	SYNCDELAY; 
 
	// arm ep2
	CDC_H2D_EP(BCL) = 0x80; // write once
	SYNCDELAY;
	CDC_H2D_EP(BCL) = 0x80; // do it again

	// make it so we enumerate
 
	ES0 = 1; /* enable serial interrupts */
	PS0 = 0; /* set serial interrupts to low priority */

	TI = 1; /* clear transmit interrupt */
	RI = 0; /* clear receiver interrupt */

	EA=1; // global interrupt enable 

	while(TRUE) {
		if ( got_sud ) {
			handle_setupdata(); 
			got_sud=FALSE;
		}

		cdc_receive_poll();
	}
}

// copied routines from setupdat.h

BOOL handle_get_descriptor() {
	return FALSE;
}

// value (low byte) = ep
#define VC_EPSTAT 0xB1

BOOL handle_vendorcommand(BYTE cmd) {
	if (cdc_handle_command(cmd))
		return TRUE;

	switch ( cmd ) {
	case VC_EPSTAT: {         
		__xdata BYTE* pep= ep_addr(SETUPDAT[2]);
		//printf ( "ep %02x\n" , *pep );
		if (pep) {
			EP0BUF[0] = *pep;
			EP0BCH=0;
			EP0BCL=1;
			return TRUE;
		} 
	}
	default:
		//printf ( "Need to implement vendor command: %02x\n", cmd );
	}
	return FALSE;
}

// this firmware only supports 0,0
BOOL handle_get_interface(BYTE ifc, BYTE* alt_ifc) { 
	//printf ( "Get Interface\n" );
	if (ifc==0) {*alt_ifc=0; return TRUE;} else { return FALSE;}
}
BOOL handle_set_interface(BYTE ifc, BYTE alt_ifc) { 
	//printf ( "Set interface %d to alt: %d\n" , ifc, alt_ifc );
 
	if (ifc==0&&alt_ifc==0) {
		// SEE TRM 2.3.7
		// reset toggles
		RESETTOGGLE(0x02);
		RESETTOGGLE(0x86);
		// restore endpoints to default condition
		RESETFIFO(0x02);
		CDC_H2D_EP(BCL)=0x80;
		SYNCDELAY;
		CDC_H2D_EP(BCL)=0X80;
		SYNCDELAY;
		RESETFIFO(0x86);
		return TRUE;
	} else {
		return FALSE;
	}
}

// get/set configuration
BYTE handle_get_configuration() {
	return 1; 
}
BOOL handle_set_configuration(BYTE cfg) { 
	return cfg==1 ? TRUE : FALSE; // we only handle cfg 1
}

// copied usb jt routines from usbjt.h
void sudav_isr() __interrupt SUDAV_ISR {  
	got_sud=TRUE;
	CLEAR_SUDAV();
}

//__bit on5;
//__xdata WORD sofct=0;
void sof_isr () __interrupt SOF_ISR __using 1 {
//	++sofct;
//	if(sofct==8000) { // about 8000 sof interrupts per second at high speed
//		on5=!on5;
//		if (on5) {d5on();} else {d5off();}
//		sofct=0;
//	}
	CLEAR_SOF();
}

void usbreset_isr() __interrupt USBRESET_ISR {
	handle_hispeed(FALSE);
	CLEAR_USBRESET();
}
void hispeed_isr() __interrupt HISPEED_ISR {
	handle_hispeed(TRUE);
	CLEAR_HISPEED();
}
