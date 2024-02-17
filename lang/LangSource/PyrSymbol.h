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
#include "Hash.h"


struct PyrSymbol {
    // Assumes name is a valid c string (length is greater than 0), which this class takes ownership of.
    PyrSymbol(char* name, hash_t hash, int16 specialIndex) noexcept;
    // Assumes name is a valid c string (length is greater than 0), which this class takes ownership of.
    PyrSymbol(char* name, hash_t hash, size_t length, int16 specialIndex) noexcept;
    PyrSymbol() = delete;
    PyrSymbol(PyrSymbol&&) noexcept = default;
    PyrSymbol(const PyrSymbol&) noexcept = default;
    PyrSymbol& operator=(PyrSymbol&&) noexcept = default;
    PyrSymbol& operator=(const PyrSymbol&) noexcept = default;

    /* owning */ char* name;
    size_t length;
    hash_t hash;
    int16 specialIndex;
    uint8 flags;
    union {
        intptr_t index = 0; // index in row table or primitive table
        struct PyrClass* classobj; // pointer to class with this name.
        char* source; // source code for sym_Filename; used only during compilation.
    } u;
    struct classdep* classdep = nullptr;
};

enum : uint8 {
    sym_Selector = 1,
    sym_Class = 2,
    sym_Compiled = 4,
    sym_Called = 8,
    sym_Primitive = 16,
    sym_Setter = 32,
    sym_MetaClass = 64,
    sym_Filename = 128
};

inline uint8 build_flags(const char* name, size_t len) {
    uint8 out;
    if (isupper(name[0]))
        out |= sym_Class;
    if (len > 1 and name[0] == '_') // could be a symbol of an underscore (which is not a primitive)
        out |= sym_Primitive;
    if (len > 1 and name[len - 1] == '_')
        out |= sym_Setter;
    return out;
}

inline PyrSymbol::PyrSymbol(char* iname, hash_t ihash, int16 ispecialIndex) noexcept:
    name(iname),
    hash(ihash),
    length(strlen(iname)),
    specialIndex(ispecialIndex),
    flags(build_flags(iname, length)),
    u(),
    classdep(nullptr) {}

inline PyrSymbol::PyrSymbol(char* iname, hash_t ihash, size_t ilength, int16 ispecialIndex) noexcept:
    name(iname),
    hash(ihash),
    length(ilength),
    specialIndex(ispecialIndex),
    flags(build_flags(iname, ilength)),
    u(),
    classdep(nullptr) {}
