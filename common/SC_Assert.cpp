/*
    SuperCollider real time audio synthesis system
    Copyright (c) 2002 James McCartney. All rights reserved.
    http://www.audiosynth.com

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
*/


#include "SC_Assert.h"

namespace boost {
void assertion_failed(char const* expr, char const* function, char const* file, long line) {
    printf("ASSERTION FAILED: %s \nfunc: %s, file: %s:%li\n", expr, function, file, line);

    if (ASSERTS_ARE_FATAL) {
        assert(0);
    } else {
        SC_BREAK;
    }
}

void assertion_failed_msg(char const* expr, char const* msg, char const* function, char const* file, long line) {
    printf("ASSERTION FAILED: %s (expr)\nfunc: %s, file: %s:%li\n", msg, function, file, line);

    if (ASSERTS_ARE_FATAL) {
        assert(0);
    } else {
        SC_BREAK;
    }
}
}
