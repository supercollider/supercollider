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

PyrSlot is a value holder for SC variables.
A PyrSlot is an 8-byte value which is either a double precision float or a
32-bit tag plus a 32-bit value.

*/

#pragma once

#if (__SIZEOF_POINTER__ == 8) || defined(__x86_64__) || defined(_M_X64) || defined(__LP64__) || defined(_WIN64)
#    include "PyrSlot64.h"
#elif (__SIZEOF_POINTER__ == 4) || defined(__i386__) || defined(_M_IX86) || defined(__ILP32__) || defined(_WIN32)      \
    || defined(__ppc__) || defined(__arm__)
#    include "PyrSlot32.h"
#else
#    error "no PyrSlot imlementation for this platform"
#endif

#include <string>

extern PyrSlot o_nil, o_true, o_false, o_inf;
extern PyrSlot o_fhalf, o_fnegone, o_fzero, o_fone, o_ftwo;
extern PyrSlot o_negone, o_zero, o_one, o_two;
extern PyrSlot o_emptyarray, o_onenilarray, o_argnamethis;

extern PyrSymbol* s_object; // "Object"
extern PyrSymbol* s_this; // "this"
extern PyrSymbol* s_super; // "super"

void dumpPyrSlot(PyrSlot* slot);
void slotString(PyrSlot* slot, char* str);
void slotOneWord(PyrSlot* slot, char* str);
bool postString(PyrSlot* slot, char* str);
const char* slotSymString(PyrSlot* slot);
int asCompileString(PyrSlot* slot, char* str);

int slotIntVal(PyrSlot* slot, int* value);
int slotFloatVal(PyrSlot* slot, float* value);
int slotDoubleVal(PyrSlot* slot, double* value);
int slotStrVal(PyrSlot* slot, char* str, int maxlen);
std::tuple<int, std::string> slotStdStrVal(PyrSlot* slot);
std::tuple<int, std::string> slotStrStdStrVal(PyrSlot* slot);
int slotStrLen(PyrSlot* slot);
int slotPStrVal(PyrSlot* slot, unsigned char* str);
int slotSymbolVal(PyrSlot* slot, PyrSymbol** symbol);

template <typename numeric_type> inline void setSlotVal(PyrSlot* slot, numeric_type value);

template <> inline void setSlotVal<int>(PyrSlot* slot, int value) { SetInt(slot, value); }

template <> inline void setSlotVal<double>(PyrSlot* slot, double value) { SetFloat(slot, value); }
