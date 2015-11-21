/**
 * Copyright (C) 2015 Tim 'mithro' Ansell
 * Copyright (C) 2009-2012 Chris McClelland
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


#include <fx2regs.h>
#include <fx2macros.h>
#include <softserial.h>

#define BAUD 32

void soft_sio0_init( DWORD baud_rate ) __critical {
	SOFT_USART_TX = 1;
	OED |= 0xff; //0x80;
}

void soft_putchar(char c) {
	(void)c; /* argument passed in DPL */
	__asm
		mov a, dpl
		mov r1, #9
		clr c
	loop:
		mov _SOFT_USART_TX, c
		rrc a
		mov r0, #BAUD
		djnz r0, .
		nop
		djnz r1, loop

		;; Stop bit
		setb _SOFT_USART_TX
		mov r0, #BAUD
		djnz r0, .
	__endasm;
}
