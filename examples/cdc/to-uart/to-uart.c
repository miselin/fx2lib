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
