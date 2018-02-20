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

Primitives for Char.

*/

#include <ctype.h>
#include "PyrPrimitive.h"
#include "VMGlobals.h"
#include "SC_PrimRegistry.hpp"

LIBSCLANG_PRIMITIVE_GROUP( Char );

SCLANG_DEFINE_PRIMITIVE( ToLower, 1 )
{
	PyrSlot *a;

	a = g->sp;

	SetRawChar(a, tolower(slotRawChar(a)));

	return errNone;
}

SCLANG_DEFINE_PRIMITIVE( ToUpper, 1 )
{
	PyrSlot *a;

	a = g->sp;

	SetRawChar(a, toupper(slotRawChar(a)));

	return errNone;
}

SCLANG_DEFINE_PRIMITIVE( IsLower, 1 )
{
	PyrSlot *a = g->sp;

	if (islower(slotRawChar(a))) { SetTrue(a); }
	else { SetFalse(a); }

	return errNone;
}

SCLANG_DEFINE_PRIMITIVE( IsUpper, 1 )
{
	PyrSlot *a = g->sp;

	if (isupper(slotRawChar(a))) { SetTrue(a); }
	else { SetFalse(a); }

	return errNone;
}

SCLANG_DEFINE_PRIMITIVE( IsAlpha, 1 )
{
	PyrSlot *a;

	a = g->sp;

	if (isalpha(slotRawChar(a))) { SetTrue(a); }
	else { SetFalse(a); }

	return errNone;
}

SCLANG_DEFINE_PRIMITIVE( IsAlphaNum, 1 )
{
	PyrSlot *a;

	a = g->sp;

	if (isalnum(slotRawChar(a))) { SetTrue(a); }
	else { SetFalse(a); }

	return errNone;
}

SCLANG_DEFINE_PRIMITIVE( IsControl, 1 )
{
	PyrSlot *a;

	a = g->sp;

	if (iscntrl(slotRawChar(a))) { SetTrue(a); }
	else { SetFalse(a); }

	return errNone;
}

SCLANG_DEFINE_PRIMITIVE( IsDigit, 1 )
{
	PyrSlot *a;

	a = g->sp;

	if (isdigit(slotRawChar(a))) { SetTrue(a); }
	else { SetFalse(a); }

	return errNone;
}

SCLANG_DEFINE_PRIMITIVE( IsPrint, 1 )
{
	PyrSlot *a;

	a = g->sp;

	if (isprint(slotRawChar(a))) { SetTrue(a); }
	else { SetFalse(a); }

	return errNone;
}

SCLANG_DEFINE_PRIMITIVE( IsPunct, 1 )
{
	PyrSlot *a;

	a = g->sp;

	if (ispunct(slotRawChar(a))) { SetTrue(a); }
	else { SetFalse(a); }

	return errNone;
}

SCLANG_DEFINE_PRIMITIVE( IsSpace, 1 )
{
	PyrSlot *a;

	a = g->sp;

	if (isspace(slotRawChar(a))) { SetTrue(a); }
	else { SetFalse(a); }

	return errNone;
}

SCLANG_DEFINE_PRIMITIVE( AsciiValue, 1 )
{
	PyrSlot *a;

	a = g->sp;

	SetTagRaw(a, tagInt);

	return errNone;
}

SCLANG_DEFINE_PRIMITIVE( DigitValue, 1 )
{
	PyrSlot *a;
	char c;

	a = g->sp;

	c = slotRawChar(a);
	if (c >= '0' && c <= '9') {
        SetInt(a, c - '0');
	} else if (c >= 'a' && c <= 'z') {
        SetInt(a, c - 'a' + 10);
	} else if (c >= 'A' && c <= 'Z') {
        SetInt(a, c - 'A' + 10);
	} else {
		return errFailed;
	}

	return errNone;
}

SCLANG_DEFINE_PRIMITIVE( AsAscii, 1 )
{
	PyrSlot *a;

	a = g->sp;
    SetChar(a, slotRawInt(a) & 255);

	return errNone;
}

SCLANG_DEFINE_PRIMITIVE( AsDigit, 1 )
{
	PyrSlot *a;
	int c;

	a = g->sp;

	c = slotRawInt(a);
	if (c >= 0 && c <= 9) {
        SetChar(a, slotRawInt(a) + '0');
	} else if (c >= 10 && c <= 35) {
        SetChar(a, slotRawInt(a) + 'A' - 10);
	} else {
		return errFailed;
	}

	return errNone;
}
