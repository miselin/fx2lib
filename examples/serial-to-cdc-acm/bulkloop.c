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


#define SYNCDELAY SYNCDELAY4
#define REARMVAL 0x80
#define REARM() EP2BCL=REARMVAL

// -----------------------------------------------------------------------

#define SET_LINE_CODING		(0x20)
#define GET_LINE_CODING		(0x21)
#define SET_CONTROL_STATE	(0x22)

BYTE __xdata LineCode[7] = {0x60,0x09,0x00,0x00,0x00,0x00,0x08};

void Serial0Init () {
	if ((LineCode[0] == 0x60) && (LineCode[1] == 0x09 )) {		// 2400
 		sio0_init(2400);
	} else if ((LineCode[0] == 0xC0) && (LineCode[1] == 0x12 )) {	 // 4800
 		sio0_init(4800);
	} else if ((LineCode[0] == 0x80) && (LineCode[1] == 0x25 )) {	 // 9600
 		sio0_init(9600);
	} else if ((LineCode[0] == 0x00) && (LineCode[1] == 0x4B )) {	// 19200
 		sio0_init(19200);
	} else if ((LineCode[0] == 0x80) && (LineCode[1] == 0x70 )) {	// 28800
 		sio0_init(28800);
	} else if ((LineCode[0] == 0x00) && (LineCode[1] == 0x96 )) {	// 38400
 		sio0_init(38400);
	} else if ((LineCode[0] == 0x00) && (LineCode[1] == 0xE1 )) {	// 57600
 		sio0_init(57600);
	} else { //if ((LineCode[0] == 0x21) && (LineCode[1] == 0x20 )) {	// 115200 (LineCode[0] == 0x00) && (LineCode[1] == 0xC2 ))
 		sio0_init(115200);
	}
}

BOOL handleCDCCommand(BYTE cmd) {
    int i;

    switch(cmd) {
    case SET_LINE_CODING:
        
        EUSB = 0 ;
        SUDPTRCTL = 0x01;
        EP0BCL = 0x00;
        SUDPTRCTL = 0x00;
        EUSB = 1;
        
        while (EP0BCL != 7);
            SYNCDELAY;

        for (i=0;i<7;i++)
            LineCode[i] = EP0BUF[i];

	Serial0Init();
        return TRUE;

    case GET_LINE_CODING:
        
        SUDPTRCTL = 0x01;
        
        for (i=0;i<7;i++)
            EP0BUF[i] = LineCode[i];

        EP0BCH = 0x00;
        SYNCDELAY;
        EP0BCL = 7;
        SYNCDELAY;
        while (EP0CS & 0x02);
        SUDPTRCTL = 0x00;
        
        return TRUE;

    case SET_CONTROL_STATE:
        return TRUE;

    default:
        return FALSE;
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

void main() {

 REVCTL=0; // not using advanced endpoint controls

 d2off();
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
 sio0_init(9600);
 
 USE_USB_INTS(); 
 ENABLE_SUDAV();
 ENABLE_SOF();
 ENABLE_HISPEED();
 ENABLE_USBRESET();
 
 
 // only valid endpoints are 2/6
 EP2CFG = 0xA2; // 10100010
 SYNCDELAY;
 EP6CFG = 0xE2; // 11100010 
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
 EP2BCL = 0x80; // write once
 SYNCDELAY;
 EP2BCL = 0x80; // do it again

 
 // make it so we enumberate
 
	ES0 = 1; /* enable serial interrupts */
	PS0 = 0; /* set serial interrupts to low priority */

	TI = 1; /* clear transmit interrupt */
	RI = 0; /* clear receiver interrupt */

 EA=1; // global interrupt enable 

 
 d3off();

 
 while(TRUE) {
 
  if ( got_sud ) {
      handle_setupdata(); 
      got_sud=FALSE;
  }

  if ( !(EP2468STAT & bmEP2EMPTY) ) {
        WORD i;
        bytes = MAKEWORD(EP2BCH,EP2BCL);
        for (i=0;i<bytes;++i) {
         SBUF0 = EP2FIFOBUF[i];
         while(TI);
        }
	REARM();

/*
 	if  ( !(EP2468STAT & bmEP6FULL) ) { // wait for at least one empty in buffer
//                 printf ( "Sending data to ep6 in\n");
    
                 
                 for (i=0;i<bytes;++i) EP6FIFOBUF[i] = d;
                 
                 // can copy whole string w/ autoptr instead.
                 // or copy directly from one buf to another

                 // ARM ep6 out
                 EP6BCH=MSB(bytes);
                 SYNCDELAY;
                 EP6BCL=LSB(bytes); 

                 REARM(); // ep2
                 //printf ( "Re-Armed ep2\n" );

         } */
   } 
 }

}

void ISR_USART0(void) __interrupt 4 __critical {
	if (RI) {
		RI=0;
		d = SBUF0;
 	  if  ( !(EP2468STAT & bmEP6FULL) ) { // wait for at least one empty in buffer
                 EP6FIFOBUF[0] = d;
                 EP6BCH=MSB(1);
                 SYNCDELAY;
                 EP6BCL=LSB(1); 
          } 
	}
	if (TI) {
		TI=0;
//		transmit();
	}
}

// copied routines from setupdat.h

BOOL handle_get_descriptor() {
  return FALSE;
}

// value (low byte) = ep
#define VC_EPSTAT 0xB1

BOOL handle_vendorcommand(BYTE cmd) {

	if (handleCDCCommand(cmd))
            return TRUE;

 switch ( cmd ) {
 
     case VC_EPSTAT:
        {         
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
    EP2BCL=0x80;
    SYNCDELAY;
    EP2BCL=0X80;
    SYNCDELAY;
    RESETFIFO(0x86);
    return TRUE;
 } else 
    return FALSE;
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

__bit on5;
__xdata WORD sofct=0;
void sof_isr () __interrupt SOF_ISR __using 1 {
    ++sofct;
    if(sofct==8000) { // about 8000 sof interrupts per second at high speed
        on5=!on5;
        if (on5) {d5on();} else {d5off();}
        sofct=0;
    }
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

