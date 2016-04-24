/*
	SuperCollider real time audio synthesis system
	Copyright (c) 2002 James McCartney. All rights reserved.
	Copyright (c) 2009 Tim Blechmann
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

#ifndef _PYRSLOTGENERIC_H_
#define _PYRSLOTGENERIC_H_

// generic pyrslot implementation
#define PYR_SLOTS_GENERIC

#include "SC_Endian.h"
#include "SC_Types.h"
#include "PyrErrors.h"

#include <cstddef>
#include <cassert>

#include <boost/config.hpp>

struct PyrSymbol;

enum {
	tagNotInitialized, // uninitialized slots have a tag of 0
	tagObj,
	tagInt,
	tagSym,
	tagChar,
	tagNil,		// nil, false, and true are indicated by the tag alone.
	tagFalse,
	tagTrue,
	tagPtr,
	/* anything else is a double */
	tagFloat,
	tagUnused,
};

typedef struct pyrslot {
	int64 tag;

	union {
		int64 c; /* char */
		int64 i;
		double f;
		void *ptr;
		struct PyrObject *o;
		PyrSymbol *s;
		struct PyrMethod *om;
		struct PyrBlock *oblk;
		struct PyrClass *oc;
		struct PyrFrame *of;
		struct PyrList *ol;
		struct PyrString *os;
		struct PyrInt8Array *ob;
		struct PyrDoubleArray *od;
		struct PyrSymbolArray *osym;
		struct PyrProcess *op;
		struct PyrThread *ot;
		struct PyrInterpreter *oi;
	} u;
} PyrSlot;


/* tag setter function */
BOOST_FORCEINLINE int GetTag(const PyrSlot* slot)  { return slot->tag; }

/* tag checking functions */
BOOST_FORCEINLINE bool IsObj(const PyrSlot* slot)  { return slot->tag == tagObj; }
BOOST_FORCEINLINE bool NotObj(const PyrSlot* slot) { return slot->tag != tagObj; }

BOOST_FORCEINLINE bool IsNil(const PyrSlot* slot)  { return slot->tag == tagNil; }
BOOST_FORCEINLINE bool NotNil(const PyrSlot* slot) { return slot->tag != tagNil; }

BOOST_FORCEINLINE bool IsFalse(const PyrSlot* slot) { return slot->tag == tagFalse; }
BOOST_FORCEINLINE bool IsTrue(const PyrSlot* slot)  { return slot->tag == tagTrue; }

BOOST_FORCEINLINE bool IsSym(const PyrSlot* slot)  { return slot->tag == tagSym; }
BOOST_FORCEINLINE bool NotSym(const PyrSlot* slot) { return slot->tag != tagSym; }

BOOST_FORCEINLINE bool IsChar(const PyrSlot* slot)  { return slot->tag == tagChar; }
BOOST_FORCEINLINE bool NotChar(const PyrSlot* slot) { return slot->tag != tagChar; }

BOOST_FORCEINLINE bool IsInt(const PyrSlot* slot)  { return slot->tag == tagInt; }
BOOST_FORCEINLINE bool NotInt(const PyrSlot* slot) { return slot->tag != tagInt; }

BOOST_FORCEINLINE bool IsFloat(const PyrSlot* slot)  { return slot->tag == tagFloat; }
BOOST_FORCEINLINE bool NotFloat(const PyrSlot* slot) { return slot->tag != tagFloat; }

BOOST_FORCEINLINE bool IsPtr(const PyrSlot* slot) { return slot->tag == tagPtr; }
BOOST_FORCEINLINE bool NotPtr(const PyrSlot* slot) { return slot->tag != tagPtr; }


/* setter functions */
BOOST_FORCEINLINE void SetInt(PyrSlot* slot, int val)           { slot->tag = tagInt;  slot->u.i = val; }
BOOST_FORCEINLINE void SetObject(PyrSlot* slot, struct PyrObjectHdr* val)      { slot->tag = tagObj;  slot->u.o = (struct PyrObject*)(val); }
BOOST_FORCEINLINE void SetSymbol(PyrSlot* slot, PyrSymbol *val) { slot->tag = tagSym;  slot->u.s = val; }
BOOST_FORCEINLINE void SetChar(PyrSlot* slot, char val)         { slot->tag = tagChar; slot->u.c = val; }
BOOST_FORCEINLINE void SetPtr(PyrSlot* slot, void* val)         { slot->tag = tagPtr;  slot->u.ptr = (void*)val; }

BOOST_FORCEINLINE void SetObjectOrNil(PyrSlot* slot, struct PyrObject* val)
{
	if (val) {
		slot->tag = tagObj;
		slot->u.o = val;
	} else {
		slot->tag = tagNil;
		slot->u.i = 0;
	}
}

BOOST_FORCEINLINE void SetTrue(PyrSlot* slot)            { slot->tag = tagTrue;                     slot->u.i = 0; }
BOOST_FORCEINLINE void SetFalse(PyrSlot* slot)           { slot->tag = tagFalse;                    slot->u.i = 0; }
BOOST_FORCEINLINE void SetBool(PyrSlot* slot, bool test) { slot->tag = (test ? tagTrue : tagFalse); slot->u.i = 0; }
BOOST_FORCEINLINE void SetNil(PyrSlot* slot)             { slot->tag = tagNil;                      slot->u.i = 0; }
BOOST_FORCEINLINE void SetFloat(PyrSlot* slot, double val)    { slot->tag = tagFloat; slot->u.f = val; }


/* raw setter functions, no typecheck */
BOOST_FORCEINLINE void SetRawChar(PyrSlot* slot, int val)     { assert(IsChar(slot));  slot->u.c = val; }
BOOST_FORCEINLINE void SetRaw(PyrSlot* slot, int val)         { assert(IsInt(slot));   slot->u.i = val; }
BOOST_FORCEINLINE void SetRaw(PyrSlot* slot, long val)         { assert(IsInt(slot));   slot->u.i = val; }
BOOST_FORCEINLINE void SetRaw(PyrSlot* slot, PyrObject * val) { assert(IsObj(slot));   slot->u.o = val; }
BOOST_FORCEINLINE void SetRaw(PyrSlot* slot, PyrSymbol * val) { assert(IsSym(slot));   slot->u.s = val; }
BOOST_FORCEINLINE void SetRaw(PyrSlot* slot, void * val)      { assert(IsPtr(slot));   slot->u.ptr = val; }
BOOST_FORCEINLINE void SetRaw(PyrSlot* slot, double val)      { assert(IsFloat(slot)); slot->u.f = val; }
BOOST_FORCEINLINE void SetTagRaw(PyrSlot* slot, int tag) { slot->tag = tag; }

/* slot comparison */
BOOST_FORCEINLINE bool SlotEq(PyrSlot* a, PyrSlot* b)
{
	return (a->tag == b->tag) && (a->u.i == b->u.i);
}

/* extract numeric value */
template <typename numeric_type>
BOOST_FORCEINLINE int slotVal(PyrSlot * slot, numeric_type *value)
{
	if (IsFloat(slot)) {
		*value = static_cast<numeric_type>(slot->u.f);
		return errNone;
	} else if (IsInt(slot)) {
		 *value = static_cast<numeric_type>(slot->u.i);
		return errNone;
	}
	return errWrongType;
}

BOOST_FORCEINLINE int slotFloatVal(PyrSlot *slot, float *value)
{
	return slotVal<float>(slot, value);
}

BOOST_FORCEINLINE int slotIntVal(PyrSlot *slot, int *value)
{
	return slotVal<int>(slot, value);
}

BOOST_FORCEINLINE int slotDoubleVal(PyrSlot *slot, double *value)
{
	return slotVal<double>(slot, value);
}

/* get symbol */
BOOST_FORCEINLINE int slotSymbolVal(PyrSlot *slot, PyrSymbol **symbol)
{
	if (!IsSym(slot)) return errWrongType;
	*symbol = slot->u.s;
	return errNone;
}

/* raw access functions */
BOOST_FORCEINLINE void* slotRawPtr(PyrSlot *slot)
{
	assert(IsPtr(slot) || (slot->u.ptr == NULL && IsNil(slot)));
	return slot->u.ptr;
}

BOOST_FORCEINLINE PyrBlock* slotRawBlock(PyrSlot *slot)
{
	return slot->u.oblk;
}

BOOST_FORCEINLINE PyrSymbolArray* slotRawSymbolArray(PyrSlot *slot)
{
	return slot->u.osym;
}

BOOST_FORCEINLINE PyrDoubleArray* slotRawDoubleArray(PyrSlot *slot)
{
	return slot->u.od;
}

BOOST_FORCEINLINE PyrInt8Array* slotRawInt8Array(PyrSlot *slot)
{
	return slot->u.ob;
}

BOOST_FORCEINLINE PyrMethod* slotRawMethod(PyrSlot *slot)
{
	return slot->u.om;
}

BOOST_FORCEINLINE const PyrMethod* slotRawMethod(const PyrSlot *slot)
{
	return slot->u.om;
}

BOOST_FORCEINLINE PyrThread* slotRawThread(PyrSlot *slot)
{
	return slot->u.ot;
}

BOOST_FORCEINLINE PyrString* slotRawString(PyrSlot *slot)
{
	return slot->u.os;
}

BOOST_FORCEINLINE PyrList* slotRawList(PyrSlot *slot)
{
	return slot->u.ol;
}

BOOST_FORCEINLINE PyrFrame* slotRawFrame(PyrSlot *slot)
{
	return slot->u.of;
}

BOOST_FORCEINLINE PyrClass* slotRawClass(PyrSlot *slot)
{
	return slot->u.oc;
}

BOOST_FORCEINLINE const PyrClass* slotRawClass(const PyrSlot *slot)
{
	return slot->u.oc;
}

BOOST_FORCEINLINE PyrInterpreter* slotRawInterpreter(PyrSlot *slot)
{
	return slot->u.oi;
}

BOOST_FORCEINLINE PyrSymbol* slotRawSymbol(PyrSlot *slot)
{
	return slot->u.s;
}

BOOST_FORCEINLINE const PyrSymbol* slotRawSymbol(const PyrSlot *slot)
{
	return slot->u.s;
}

BOOST_FORCEINLINE int slotRawChar(const PyrSlot *slot)
{
	return slot->u.c;
}

BOOST_FORCEINLINE int slotRawInt(const PyrSlot *slot)
{
	assert(IsInt(slot));
	return slot->u.i;
}

BOOST_FORCEINLINE double slotRawFloat(const PyrSlot *slot)
{
	assert(IsFloat(slot));
	return slot->u.f;
}

BOOST_FORCEINLINE PyrObject* slotRawObject(PyrSlot *slot)
{
	assert(IsObj(slot));
	return slot->u.o;
}

BOOST_FORCEINLINE const PyrObject* slotRawObject(const PyrSlot *slot)
{
	assert(IsObj(slot));
	return slot->u.o;
}

/* slot copy functions */
BOOST_FORCEINLINE void slotCopy(PyrSlot *dst, const PyrSlot *src)
{
	*dst = *src;
}

BOOST_FORCEINLINE void slotCopy(PyrSlot *dst, const PyrSlot *src, int num)
{
	for (int i=0; i<num; ++i)
		slotCopy(dst + i, src + i);
}

#endif
