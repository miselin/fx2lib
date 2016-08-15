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

/** \file lights.h
 * macros for turning lights on the EZ-USB development board on and off.
 **/

#ifndef LIGHTS_H
#define LIGHTS_H

#include "fx2types.h"

#define init_lights() IFCONFIG &= ~3; PORTACFG = 0x00

#define d1on() OEA |= 0x01; IOA &= ~0x01
#define d2on() OEA |= 0x02; IOA &= ~0x02

#define d1off() OEA |= 0x01; IOA |= 0x01
#define d2off() OEA |= 0x02; IOA |= 0x02

#endif
