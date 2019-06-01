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

#include "SC_Endian.h"
#include "SC_Types.h"
#include "PyrErrors.h"

#include <cassert>
#include <cstddef>

struct PyrSymbol;

/*
    Pyrite slots are the size of an 8 byte double. If the upper bits
    indicate that the double is a 'Not-A-Number' then the upper 32
    bits are used as a tag to indicate one of a number of other types
    whose data is in the lower 32 bits.
*/

/* some DSPs like the TIC32 do not support 8 byte doubles */
/* on such CPUs, set DOUBLESLOTS to zero */

#define DOUBLESLOTS 1

/* use the high order bits of an IEEE double NaN as a tag */
enum {
    tagObj = 0x7FF90001,
    tagInt = 0x7FF90002,
    tagSym = 0x7FF90003,
    tagChar = 0x7FF90004,
    tagNil = 0x7FF90005, // nil, false, and true are indicated by the tag alone.
    tagFalse = 0x7FF90006, // the lower 32 bits are zero.
    tagTrue = 0x7FF90007,
    tagPtr = 0x7FF90008,
    /* anything else is a double */
    tagUnused = 0x7FF9000D


#if !DOUBLESLOTS
    ,
    tagFloat = 0x7FF9000F /* used only to initialized 4 byte float tags, never compared with */
#endif
};

typedef union pyrslot {
    double f;
    struct {
#if BYTE_ORDER == BIG_ENDIAN
        int tag;
#endif // BIG_ENDIAN
        union {
            int c; /* char */
            int i;
            float f;
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
#if BYTE_ORDER == LITTLE_ENDIAN
        // need to swap on intel <sk>
        int tag;
#endif // LITTLE_ENDIAN
    } s;
} PyrSlot;

/*
    these are some defines to make accessing the structure less verbose.
    obviously it polutes the namespace of identifiers beginning with 'u'.
*/
#define utag s.tag
// int
#define ui s.u.i
// PyrObject
#define uo s.u.o
// PyrSymbol
#define us s.u.s
#define uc s.u.c
#define uoc s.u.oc
#define uof s.u.of
#define uol s.u.ol
#define uod s.u.od
#define uob s.u.ob
#define uop s.u.op
#define uoi s.u.oi
#define uod s.u.od
// string
#define uos s.u.os
#define uot s.u.ot
// method
#define uom s.u.om
// symbol array
#define uosym s.u.osym
#define uoblk s.u.oblk
#define uptr s.u.ptr

#if DOUBLESLOTS
#    define uf f
#else
#    define uf s.u.f
#endif

/*
    Note that on the PowerPC, the fastest way to copy a slot is to
    copy the double field, not the struct.
*/

inline int GetTag(const PyrSlot* slot) { return slot->utag; }

/* some macros for setting values of slots */
inline void SetInt(PyrSlot* slot, int val) {
    (slot)->utag = tagInt;
    (slot)->ui = (val);
}
inline void SetObject(PyrSlot* slot, struct PyrObjectHdr* val) {
    (slot)->utag = tagObj;
    (slot)->uo = (PyrObject*)(val);
}
inline void SetSymbol(PyrSlot* slot, PyrSymbol* val) {
    (slot)->utag = tagSym;
    (slot)->us = (val);
}
inline void SetChar(PyrSlot* slot, char val) {
    (slot)->utag = tagChar;
    (slot)->uc = (val);
}
inline void SetPtr(PyrSlot* slot, void* val) {
    (slot)->utag = tagPtr;
    (slot)->uptr = (void*)(val);
}
inline void SetObjectOrNil(PyrSlot* slot, PyrObject* val) {
    if (val) {
        (slot)->utag = tagObj;
        (slot)->uo = (val);
    } else {
        (slot)->utag = tagNil;
        (slot)->ui = 0;
    }
}

inline void SetTrue(PyrSlot* slot) {
    (slot)->utag = tagTrue;
    (slot)->ui = 0;
}
inline void SetFalse(PyrSlot* slot) {
    (slot)->utag = tagFalse;
    (slot)->ui = 0;
}
inline void SetBool(PyrSlot* slot, bool test) {
    (slot)->utag = ((test) ? tagTrue : tagFalse);
    (slot)->ui = 0;
}
inline void SetNil(PyrSlot* slot) {
    (slot)->utag = tagNil;
    (slot)->ui = 0;
}

#if DOUBLESLOTS
inline void SetFloat(PyrSlot* slot, double val) { (slot)->uf = (val); }
#else
inline void SetFloat(PyrSlot* slot, double val) {
    (slot)->utag = s_float;
    (slot)->uf = (val);
}
#endif

inline bool IsObj(const PyrSlot* slot) { return ((slot)->utag == tagObj); }
inline bool NotObj(const PyrSlot* slot) { return ((slot)->utag != tagObj); }

inline bool IsNil(const PyrSlot* slot) { return ((slot)->utag == tagNil); }
inline bool NotNil(const PyrSlot* slot) { return ((slot)->utag != tagNil); }

inline bool IsFalse(const PyrSlot* slot) { return ((slot)->utag == tagFalse); }
inline bool IsTrue(const PyrSlot* slot) { return ((slot)->utag == tagTrue); }

inline bool SlotEq(PyrSlot* a, PyrSlot* b) { return ((a)->ui == (b)->ui && (a)->utag == (b)->utag); }

inline bool IsSym(const PyrSlot* slot) { return ((slot)->utag == tagSym); }
inline bool NotSym(const PyrSlot* slot) { return ((slot)->utag != tagSym); }

inline bool IsChar(const PyrSlot* slot) { return ((slot)->utag == tagChar); }
inline bool NotChar(const PyrSlot* slot) { return ((slot)->utag != tagChar); }

inline bool IsInt(const PyrSlot* slot) { return ((slot)->utag == tagInt); }
inline bool NotInt(const PyrSlot* slot) { return ((slot)->utag != tagInt); }

inline bool IsFloatTag(int tag) { return ((tag & 0xFFFFFFF0) != 0x7FF90000); }
inline bool IsFloat(const PyrSlot* slot) { return (((slot)->utag & 0xFFFFFFF0) != 0x7FF90000); }
inline bool NotFloat(const PyrSlot* slot) { return (((slot)->utag & 0xFFFFFFF0) == 0x7FF90000); }

inline bool IsPtr(const PyrSlot* slot) { return ((slot)->utag == tagPtr); }
inline bool NotPtr(const PyrSlot* slot) { return ((slot)->utag != tagPtr); }

inline void SetRawChar(PyrSlot* slot, int val) {
    assert(IsChar(slot));
    slot->uc = val;
}
inline void SetRaw(PyrSlot* slot, int val) {
    assert(IsInt(slot));
    slot->ui = val;
}
inline void SetRaw(PyrSlot* slot, long val) {
    assert(IsInt(slot));
    slot->ui = val;
}
inline void SetRaw(PyrSlot* slot, PyrObject* val) {
    assert(IsObj(slot));
    slot->uo = val;
}
inline void SetRaw(PyrSlot* slot, PyrSymbol* val) {
    assert(IsSym(slot));
    slot->us = val;
}
inline void SetRaw(PyrSlot* slot, void* val) {
    assert(IsPtr(slot));
    slot->uptr = val;
}
inline void SetRaw(PyrSlot* slot, double val) {
    assert(IsFloat(slot));
    SetFloat(slot, val);
}

inline void SetTagRaw(PyrSlot* slot, int tag) { slot->utag = tag; }

template <typename numeric_type> inline int slotVal(PyrSlot* slot, numeric_type* value) {
    if (IsFloat(slot)) {
        *value = static_cast<numeric_type>(slot->uf);
        return errNone;
    } else if (IsInt(slot)) {
        *value = static_cast<numeric_type>(slot->ui);
        return errNone;
    }
    return errWrongType;
}


inline int slotFloatVal(PyrSlot* slot, float* value) { return slotVal<float>(slot, value); }

inline int slotIntVal(PyrSlot* slot, int* value) { return slotVal<int>(slot, value); }

inline int slotDoubleVal(PyrSlot* slot, double* value) { return slotVal<double>(slot, value); }

inline int slotSymbolVal(PyrSlot* slot, PyrSymbol** symbol) {
    if (!IsSym(slot))
        return errWrongType;
    *symbol = slot->us;
    return errNone;
}

inline void* slotRawPtr(PyrSlot* slot) {
    assert(IsPtr(slot) || (slot->s.u.ptr == NULL && IsNil(slot)));
    return slot->s.u.ptr;
}

inline PyrBlock* slotRawBlock(PyrSlot* slot) { return slot->s.u.oblk; }

inline PyrSymbolArray* slotRawSymbolArray(PyrSlot* slot) { return slot->s.u.osym; }

inline PyrDoubleArray* slotRawDoubleArray(PyrSlot* slot) { return slot->s.u.od; }

inline PyrInt8Array* slotRawInt8Array(PyrSlot* slot) { return slot->s.u.ob; }

inline PyrMethod* slotRawMethod(PyrSlot* slot) { return slot->s.u.om; }

inline const PyrMethod* slotRawMethod(const PyrSlot* slot) { return slot->s.u.om; }

inline PyrThread* slotRawThread(PyrSlot* slot) { return slot->s.u.ot; }

inline PyrString* slotRawString(PyrSlot* slot) { return slot->s.u.os; }

inline PyrList* slotRawList(PyrSlot* slot) { return slot->s.u.ol; }

inline PyrFrame* slotRawFrame(PyrSlot* slot) { return slot->s.u.of; }

inline PyrClass* slotRawClass(PyrSlot* slot) { return slot->s.u.oc; }

inline const PyrClass* slotRawClass(const PyrSlot* slot) { return slot->s.u.oc; }

inline PyrInterpreter* slotRawInterpreter(PyrSlot* slot) { return slot->s.u.oi; }

inline PyrSymbol* slotRawSymbol(PyrSlot* slot) {
    assert(IsSym(slot));
    return slot->s.u.s;
}

inline const PyrSymbol* slotRawSymbol(const PyrSlot* slot) { return slot->s.u.s; }

inline int slotRawChar(const PyrSlot* slot) {
    assert(IsChar(slot));
    return slot->s.u.c;
}

inline int slotRawInt(const PyrSlot* slot) {
    assert(IsInt(slot));
    return slot->s.u.i;
}

inline double slotRawFloat(const PyrSlot* slot) {
    assert(IsFloat(slot));
    return slot->uf;
}

inline PyrObject* slotRawObject(PyrSlot* slot) {
    assert(IsObj(slot));
    return slot->s.u.o;
}

inline const PyrObject* slotRawObject(const PyrSlot* slot) {
    assert(IsObj(slot));
    return slot->s.u.o;
}


inline void slotCopy(PyrSlot* dst, const PyrSlot* src) {
    double* dstp = (double*)dst;
    double* srcp = (double*)src;
    *dstp = *srcp;
}

inline void slotCopy(PyrSlot* dst, const PyrSlot* src, int num) {
    double* dstp = (double*)dst - 1;
    double* srcp = (double*)src - 1;
    for (int i = 0; i < num; ++i) {
        *++dstp = *++srcp;
    }
}

#undef uptr
#undef uoblk
#undef uosym
#undef uom
#undef uot
#undef uos
#undef uod
#undef uob
#undef uol
#undef uof
#undef uoc
#undef utag
#undef us
#undef uc
#undef uo
#undef uoi
#undef ui
#undef uf

#undef uop
