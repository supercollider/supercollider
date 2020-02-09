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

#pragma once

// generic pyrslot implementation
#define PYR_SLOTS_GENERIC

#include "SC_Endian.h"
#include "SC_Types.h"
#include "PyrErrors.h"

#include <cstddef>
#include <cassert>
#include <vector>

struct PyrSymbol;

enum class PyrTag : long {
    tagNotInitialized = 0, // uninitialized slots have a tag of 0
    tagObj,
    tagInt,
    tagSym,
    tagChar,
    tagNil, // nil, false, and true are indicated by the tag alone.
    tagFalse,
    tagTrue,
    tagPtr,
    /* anything else is a double */
    tagFloat,
    tagUnused,
};

typedef struct pyrslot {
    PyrTag tag;

    union {
        int64 c; /* char */
        int64 i;
        double f;
        void* ptr;
        struct PyrObject* o;
        PyrSymbol* s;
        struct PyrMethod* om;
        struct PyrBlock* oblk;
        struct PyrClass* oc;
        struct PyrFrame* of;
        struct PyrList* ol;
        struct PyrString* os;
        struct PyrInt8Array* ob;
        struct PyrDoubleArray* od;
        struct PyrSymbolArray* osym;
        struct PyrProcess* op;
        struct PyrThread* ot;
        struct PyrInterpreter* oi;
    } u;
} PyrSlot;


/* tag setter function */
inline PyrTag GetTag(const PyrSlot* slot) { return slot->tag; }

/* tag checking functions */
inline bool IsObj(const PyrSlot* slot) { return slot->tag == PyrTag::tagObj; }
inline bool NotObj(const PyrSlot* slot) { return slot->tag != PyrTag::tagObj; }

inline bool IsNil(const PyrSlot* slot) { return slot->tag == PyrTag::tagNil; }
inline bool NotNil(const PyrSlot* slot) { return slot->tag != PyrTag::tagNil; }

inline bool IsFalse(const PyrSlot* slot) { return slot->tag == PyrTag::tagFalse; }
inline bool IsTrue(const PyrSlot* slot) { return slot->tag == PyrTag::tagTrue; }

inline bool IsSym(const PyrSlot* slot) { return slot->tag == PyrTag::tagSym; }
inline bool NotSym(const PyrSlot* slot) { return slot->tag != PyrTag::tagSym; }

inline bool IsChar(const PyrSlot* slot) { return slot->tag == PyrTag::tagChar; }
inline bool NotChar(const PyrSlot* slot) { return slot->tag != PyrTag::tagChar; }

inline bool IsInt(const PyrSlot* slot) { return slot->tag == PyrTag::tagInt; }
inline bool NotInt(const PyrSlot* slot) { return slot->tag != PyrTag::tagInt; }

inline bool IsFloat(const PyrSlot* slot) { return slot->tag == PyrTag::tagFloat; }
inline bool NotFloat(const PyrSlot* slot) { return slot->tag != PyrTag::tagFloat; }

inline bool IsPtr(const PyrSlot* slot) { return slot->tag == PyrTag::tagPtr; }
inline bool NotPtr(const PyrSlot* slot) { return slot->tag != PyrTag::tagPtr; }


/* setter functions */
inline void SetInt(PyrSlot* slot, int val) {
    slot->tag = PyrTag::tagInt;
    slot->u.i = val;
}
inline void SetObject(PyrSlot* slot, struct PyrObjectHdr* val) {
    slot->tag = PyrTag::tagObj;
    slot->u.o = (struct PyrObject*)(val);
}
inline void SetSymbol(PyrSlot* slot, PyrSymbol* val) {
    slot->tag = PyrTag::tagSym;
    slot->u.s = val;
}
inline void SetChar(PyrSlot* slot, char val) {
    slot->tag = PyrTag::tagChar;
    slot->u.c = val;
}
inline void SetPtr(PyrSlot* slot, void* val) {
    slot->tag = PyrTag::tagPtr;
    slot->u.ptr = (void*)val;
}

inline void SetObjectOrNil(PyrSlot* slot, struct PyrObject* val) {
    if (val) {
        slot->tag = PyrTag::tagObj;
        slot->u.o = val;
    } else {
        slot->tag = PyrTag::tagNil;
        slot->u.i = 0;
    }
}

inline void SetTrue(PyrSlot* slot) {
    slot->tag = PyrTag::tagTrue;
    slot->u.i = 0;
}
inline void SetFalse(PyrSlot* slot) {
    slot->tag = PyrTag::tagFalse;
    slot->u.i = 0;
}
inline void SetBool(PyrSlot* slot, bool test) {
    slot->tag = (test ? PyrTag::tagTrue : PyrTag::tagFalse);
    slot->u.i = 0;
}
inline void SetNil(PyrSlot* slot) {
    slot->tag = PyrTag::tagNil;
    slot->u.i = 0;
}
inline void SetFloat(PyrSlot* slot, double val) {
    slot->tag = PyrTag::tagFloat;
    slot->u.f = val;
}

/* raw setter functions, no typecheck */
inline void SetRawChar(PyrSlot* slot, int val) {
    assert(IsChar(slot));
    slot->u.c = val;
}
inline void SetRaw(PyrSlot* slot, int val) {
    assert(IsInt(slot));
    slot->u.i = val;
}
inline void SetRaw(PyrSlot* slot, long val) {
    assert(IsInt(slot));
    slot->u.i = val;
}
inline void SetRaw(PyrSlot* slot, PyrObject* val) {
    assert(IsObj(slot));
    slot->u.o = val;
}
inline void SetRaw(PyrSlot* slot, PyrSymbol* val) {
    assert(IsSym(slot));
    slot->u.s = val;
}
inline void SetRaw(PyrSlot* slot, void* val) {
    assert(IsPtr(slot));
    slot->u.ptr = val;
}
inline void SetRaw(PyrSlot* slot, double val) {
    assert(IsFloat(slot));
    slot->u.f = val;
}
inline void SetTagRaw(PyrSlot* slot, PyrTag tag) { slot->tag = static_cast<PyrTag>(tag); }

/* slot comparison */
inline bool SlotEq(PyrSlot* a, PyrSlot* b) { return (a->tag == b->tag) && (a->u.i == b->u.i); }

/* extract numeric value */
template <typename numeric_type> inline int slotVal(PyrSlot* slot, numeric_type* value) {
    if (IsFloat(slot)) {
        *value = static_cast<numeric_type>(slot->u.f);
        return errNone;
    } else if (IsInt(slot)) {
        *value = static_cast<numeric_type>(slot->u.i);
        return errNone;
    }
    return errWrongType;
}

inline int slotFloatVal(PyrSlot* slot, float* value) { return slotVal<float>(slot, value); }

inline int slotIntVal(PyrSlot* slot, int* value) { return slotVal<int>(slot, value); }

inline int slotDoubleVal(PyrSlot* slot, double* value) { return slotVal<double>(slot, value); }

/* get symbol */
inline int slotSymbolVal(PyrSlot* slot, PyrSymbol** symbol) {
    if (!IsSym(slot))
        return errWrongType;
    *symbol = slot->u.s;
    return errNone;
}

/* raw access functions */
inline void* slotRawPtr(PyrSlot* slot) {
    assert(IsPtr(slot) || (slot->u.ptr == NULL && IsNil(slot)));
    return slot->u.ptr;
}

inline PyrBlock* slotRawBlock(PyrSlot* slot) { return slot->u.oblk; }

inline PyrSymbolArray* slotRawSymbolArray(PyrSlot* slot) { return slot->u.osym; }

inline PyrDoubleArray* slotRawDoubleArray(PyrSlot* slot) { return slot->u.od; }

inline PyrInt8Array* slotRawInt8Array(PyrSlot* slot) { return slot->u.ob; }

inline PyrMethod* slotRawMethod(PyrSlot* slot) { return slot->u.om; }

inline const PyrMethod* slotRawMethod(const PyrSlot* slot) { return slot->u.om; }

inline PyrThread* slotRawThread(PyrSlot* slot) { return slot->u.ot; }

inline PyrString* slotRawString(PyrSlot* slot) { return slot->u.os; }

inline PyrList* slotRawList(PyrSlot* slot) { return slot->u.ol; }

inline PyrFrame* slotRawFrame(PyrSlot* slot) { return slot->u.of; }

inline PyrClass* slotRawClass(PyrSlot* slot) { return slot->u.oc; }

inline const PyrClass* slotRawClass(const PyrSlot* slot) { return slot->u.oc; }

inline PyrInterpreter* slotRawInterpreter(PyrSlot* slot) { return slot->u.oi; }

inline PyrSymbol* slotRawSymbol(PyrSlot* slot) { return slot->u.s; }

inline const PyrSymbol* slotRawSymbol(const PyrSlot* slot) { return slot->u.s; }

inline int slotRawChar(const PyrSlot* slot) { return slot->u.c; }

inline int slotRawInt(const PyrSlot* slot) {
    assert(IsInt(slot));
    return slot->u.i;
}

inline double slotRawFloat(const PyrSlot* slot) {
    assert(IsFloat(slot));
    return slot->u.f;
}

inline PyrObject* slotRawObject(PyrSlot* slot) {
    assert(IsObj(slot));
    return slot->u.o;
}

inline const PyrObject* slotRawObject(const PyrSlot* slot) {
    assert(IsObj(slot));
    return slot->u.o;
}

/* slot copy functions */
inline void slotCopy(PyrSlot* dst, const PyrSlot* src) { *dst = *src; }

inline void slotCopy(PyrSlot* dst, const PyrSlot* src, int num) {
    for (int i = 0; i < num; ++i)
        slotCopy(dst + i, src + i);
}
