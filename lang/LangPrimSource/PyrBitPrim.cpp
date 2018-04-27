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

Primitives for some bit operations.

*/

#include "PyrPrimitive.h"
#include "VMGlobals.h"
#include "clz.h"

#include "SC_PrimRegistry.hpp"

LIBSCLANG_PRIMITIVE_GROUP( Bit );

SCLANG_DEFINE_PRIMITIVE( NumBits, 1 )
{
	PyrSlot *a = g->sp;
	SetRaw(a, NUMBITS(slotRawInt(a)));
	return errNone;
}

SCLANG_DEFINE_PRIMITIVE( Log2Ceil, 1 )
{
	PyrSlot *a = g->sp;
	SetRaw(a, LOG2CEIL(slotRawInt(a)));
	return errNone;
}

SCLANG_DEFINE_PRIMITIVE( CLZ, 1 )
{
	PyrSlot *a = g->sp;
	SetRaw(a, CLZ(slotRawInt(a)));
	return errNone;
}

SCLANG_DEFINE_PRIMITIVE( CTZ, 1 )
{
	PyrSlot *a = g->sp;
	SetRaw(a, CTZ(slotRawInt(a)));
	return errNone;
}

SCLANG_DEFINE_PRIMITIVE( NextPowerOfTwo, 1 )
{
	PyrSlot *a;

	a = g->sp;

	SetRaw(a, NEXTPOWEROFTWO(slotRawInt(a)));
	return errNone;
}

SCLANG_DEFINE_PRIMITIVE( IsPowerOfTwo, 1 )
{
	PyrSlot *a = g->sp;
	SetBool(a, ISPOWEROFTWO(slotRawInt(a)));
	return errNone;
}

SCLANG_DEFINE_PRIMITIVE( BinaryGrayCode, 1 )
{
	PyrSlot *a;

	a = g->sp;
	SetRaw(a, GRAYCODE(slotRawInt(a)));
	return errNone;
}

SCLANG_DEFINE_PRIMITIVE( SetBit, 3 )
{
	PyrSlot *a = g->sp - 2;
	PyrSlot *b = g->sp - 1;
	PyrSlot *c = g->sp;

	int32 bit, mask;
	int err = slotIntVal(b, &bit);
	if (err) return err;
	bit = bit & 31;
	mask = 1L << bit;
	if (IsFalse(c)) {
		SetRaw(a, slotRawInt(a) & ~mask);
	} else {
		SetRaw(a, slotRawInt(a) |  mask);
	}
	return errNone;
}

SCLANG_DEFINE_PRIMITIVE( HammingDistance, 2 )
{
	PyrSlot *a = g->sp - 1;
	PyrSlot *b = g->sp;

	if (NotInt(a) || NotInt(b))
		return errWrongType;

	int aInt = slotRawInt(a);
	int bInt = slotRawInt(b);

	int count = 0, mask = 1;
	for(int i = 0; i < 32; i++) {
		if((aInt & mask) != (bInt & mask)) count = count + 1;
		mask = mask << 1;
	}
	SetRaw(a, count);

	return errNone;
}
