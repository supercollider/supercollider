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
/*

A PyrSymbol is a unique string that resides in a global hash table.

*/

#pragma once

#include "SC_Types.h"
#include "PyrSlot.h"

struct PyrSymbol {
    char* name;
    long hash;
    short specialIndex;
    uint8 flags;
    uint8 length;
    union {
        intptr_t index; // index in row table or primitive table
        struct PyrClass* classobj; // pointer to class with this name.
        char* source; // source code for sym_Filename; used only during compilation.
    } u;
    struct classdep* classdep;
};

enum {
    sym_Selector = 1,
    sym_Class = 2,
    sym_Compiled = 4,
    sym_Called = 8,
    sym_Primitive = 16,
    sym_Setter = 32,
    sym_MetaClass = 64,
    sym_Filename = 128
};
