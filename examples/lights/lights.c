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

#include <fx2regs.h>

#include <lights.h>
#include <delay.h>

void main(void)
{
    BYTE n = 0;

    init_lights();
    for (;;) {
        if (n == 0) {
            d1on();
        } else if (n == 1) {
            d2on();
        } else if (n == 2) {
            d1off();
        } else {
            d2off();
        }
        delay(250);
        n = (n + 1) % 4;
    }
}
