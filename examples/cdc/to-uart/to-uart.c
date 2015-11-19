/*
#include <stdio.h>

#include <fx2regs.h>
#include <fx2macros.h>
#include <serial.h>
#include <delay.h>
#include <autovector.h>
#include <lights.h>
#include <setupdat.h>
#include <eputils.h>

#include "cdc.h"

#define SYNCDELAY SYNCDELAY4
#define REARMVAL 0x80
#define REARM() EP2BCL=REARMVAL

#include <fx2regs.h>
#include <fx2macros.h>
#include <serial.h>

#include "cdc.h"
*/

#include <fx2ints.h>

// -----------------------------------------------------------------------

BOOL cdcuser_set_line_rate(DWORD baud_rate) {
        if (baud_rate > 115200 || baud_rate < 2400)
            baud_rate = 115200;
	sio0_init(baud_rate);
	return TRUE;
}


volatile __bit byte_sending = TRUE;
void cdcuser_receive_data(BYTE* data, WORD length) {
        WORD i;
        for (i=0; i < length ; ++i) {
		// Wait for any previous byte to send. byte_sending is cleared
		// in the USART0 ISR.
		while(byte_sending);
		byte_sending = TRUE;
		SBUF0 = data[i];
	}
}

// Serial port 0 transmit or receive interrupt
// *MUST* Clear TI and RI bits.
void ISR_USART0(void) __interrupt TI_0_ISR __critical {
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
		byte_sending = FALSE;
	}
}
