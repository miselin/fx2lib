// Copyright (C) 2015 Tim 'mithro' Ansell
// Copyright (C) 2009-2012 Chris McClelland
// Copyright (C) 2009 Ubixum, Inc. 
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

/** \file softserial.h
 * defines functions to print to a serial console with bitbanging.
 **/

#include "fx2types.h"

#ifndef SOFTSERIAL_H
#define SOFTSERIAL_H

#ifndef SOFT_USART_TX
__sbit __at 0xB3 SOFT_USART_TX; //0xB7 USART; // Port D7
#endif

void soft_sio0_init( DWORD baud_rate ) __critical ; // baud_rate max should be 57600 since int=2 bytes

void soft_putchar(char c);
char soft_getchar();

#endif // SOFTSERIAL_H
