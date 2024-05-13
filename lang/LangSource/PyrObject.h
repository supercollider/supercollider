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

PyrObject represents the structure of all SC Objects.

*/

#pragma once

#include "PyrSlot.h"

#include <vector>

/* special gc colors */
enum {
    obj_permanent = 1, // sent to gc->New as a flag
    obj_gcmarker = 2 // gc treadmill marker
};

/* obj flag fields */
enum { obj_inaccessible = 4, obj_immutable = 16, obj_marked = 128 };

/* format types : */
enum {
    obj_notindexed,
    obj_slot,
    obj_double,
    obj_float,
    obj_int32,
    obj_int16,
    obj_int8,
    obj_char,
    obj_symbol,

    NUMOBJFORMATS
};


/*
    PyrObjectHdr : object header fields
    prev, next : pointers in the GC treadmill
    classptr : pointer to the object's class
    size : number of slots or indexable elements.

    obj_format : what kind of data this object holds
    obj_sizeclass : power of two size class of the object
    obj_flags :
        immutable : set if object may not be updated.
        finalize : set if object requires finalization.
        marked : used by garbage collector debug sanity check. may be used by primitives but must be cleared before
   exiting primitive. gc_color : GC color : black, grey, white, free, permanent scratch1 : undefined value. may be used
   within primitives as a temporary scratch value.
*/

struct PyrObjectHdr {
    struct PyrObjectHdr *prev, *next;
    struct PyrClass* classptr;
    int size;

    unsigned char obj_format;
    unsigned char obj_sizeclass;
    unsigned char obj_flags;
    unsigned char gc_color;

    int scratch1;

    int SizeClass() { return obj_sizeclass; }

    void SetMark() { obj_flags |= obj_marked; }
    void ClearMark() { obj_flags &= ~obj_marked; }
    bool IsMarked() const { return obj_flags & obj_marked; }
    bool IsPermanent() const { return gc_color == obj_permanent; }
    bool IsImmutable() const { return obj_flags & obj_immutable; }
    bool IsMutable() const { return !IsImmutable(); }
};

struct PyrObject : public PyrObjectHdr {
    PyrSlot slots[1];
};

struct PyrList : public PyrObjectHdr {
    PyrSlot array;
};

struct PyrDoubleArray : public PyrObjectHdr {
    double d[1];
};

struct PyrFloatArray : public PyrObjectHdr {
    float f[1];
};

struct PyrInt32Array : public PyrObjectHdr {
    uint32 i[1];
};

struct PyrInt16Array : public PyrObjectHdr {
    uint16 i[1];
};

struct PyrInt8Array : public PyrObjectHdr {
    uint8 b[1];
};

struct PyrString : public PyrObjectHdr {
    char s[1];
};

struct PyrSymbolArray : public PyrObjectHdr {
    PyrSymbol* symbols[1];
};

extern struct PyrClass* class_object;
extern struct PyrClass* class_array;
extern struct PyrClass *class_list, *class_method, *class_fundef, *class_frame, *class_class;
extern struct PyrClass *class_symbol, *class_nil;
extern struct PyrClass *class_boolean, *class_true, *class_false;
extern struct PyrClass *class_int, *class_char, *class_float, *class_complex;
extern struct PyrClass* class_rawptr;
extern struct PyrClass* class_string;
extern struct PyrClass *class_magnitude, *class_number, *class_collection;
extern struct PyrClass* class_sequenceable_collection;
extern struct PyrClass* class_arrayed_collection;
extern struct PyrClass* class_simple_number;
extern struct PyrClass* class_signal;
extern struct PyrClass* class_wavetable;
extern struct PyrClass* class_rawarray;
extern struct PyrClass* class_int8array;
extern struct PyrClass* class_int16array;
extern struct PyrClass* class_int32array;
extern struct PyrClass* class_symbolarray;
extern struct PyrClass* class_floatarray;
extern struct PyrClass* class_doublearray;
extern struct PyrClass *class_func, *class_absfunc;
extern struct PyrClass* class_stream;
extern struct PyrClass* class_process;
extern struct PyrClass* class_interpreter;
extern struct PyrClass* class_thread;
extern struct PyrClass* class_routine;
extern struct PyrClass* class_finalizer;
extern struct PyrClass* class_server_shm_interface;

extern PyrSymbol* s_none;
extern PyrSymbol* s_object;
extern PyrSymbol* s_bag;
extern PyrSymbol* s_set;
extern PyrSymbol* s_identityset;
extern PyrSymbol* s_dictionary;
extern PyrSymbol* s_identitydictionary;
extern PyrSymbol* s_linkedlist;
extern PyrSymbol* s_sortedlist;
extern PyrSymbol* s_array;
extern PyrSymbol *s_list, *s_method, *s_fundef, *s_frame, *s_class;
extern PyrSymbol *s_symbol, *s_nil;
extern PyrSymbol *s_boolean, *s_true, *s_false;
extern PyrSymbol *s_int, *s_char, *s_color, *s_float, *s_complex;
extern PyrSymbol* s_rawptr;
extern PyrSymbol* s_string;
extern PyrSymbol *s_magnitude, *s_number, *s_collection;
extern PyrSymbol* s_ordered_collection;
extern PyrSymbol* s_sequenceable_collection;
extern PyrSymbol* s_arrayed_collection;
extern PyrSymbol* s_simple_number;
extern PyrSymbol* s_signal;
extern PyrSymbol* s_wavetable;
extern PyrSymbol* s_int8array;
extern PyrSymbol* s_int16array;
extern PyrSymbol* s_int32array;
extern PyrSymbol* s_symbolarray;
extern PyrSymbol* s_floatarray;
extern PyrSymbol* s_doublearray;
extern PyrSymbol* s_rect;
extern PyrSymbol* s_stream;
extern PyrSymbol* s_process;
extern PyrSymbol* s_main;
extern PyrSymbol* s_thread;
extern PyrSymbol* s_routine;
extern PyrSymbol* s_env;

extern PyrSymbol *s_run, *s_stop, *s_tick;
extern PyrSymbol* s_next;
extern PyrSymbol* s_at;
extern PyrSymbol* s_put;
extern PyrSymbol *s_series, *s_copyseries, *s_putseries;
extern PyrSymbol* s_value;
extern PyrSymbol* s_performList;
extern PyrSymbol* s_superPerformList;
extern PyrSymbol *s_new, *s_ref;
extern PyrSymbol *s_synth, *s_environment, *s_event;
extern PyrSymbol* s_interpreter;
extern PyrSymbol* s_finalizer;
extern PyrSymbol* s_awake;
extern PyrSymbol* s_systemclock;
extern PyrSymbol* s_server_shm_interface;
extern PyrSymbol *s_interpretCmdLine, *s_interpretPrintCmdLine;


extern int gFormatElemSize[NUMOBJFORMATS];
extern int gFormatElemCapc[NUMOBJFORMATS];
extern int gFormatElemTag[NUMOBJFORMATS];

void dumpObject(PyrObject* obj);
void dumpObjectSlot(PyrSlot* slot);

bool respondsTo(PyrSlot* slot, PyrSymbol* selector);
bool isSubclassOf(struct PyrClass* classobj, struct PyrClass* testclass);

extern struct PyrClass* gTagClassTable[16];

inline struct PyrClass* classOfSlot(PyrSlot* slot) {
    PyrClass* classobj;
    int tag;
    if (IsFloat(slot))
        classobj = class_float;
    else if ((tag = GetTag(slot) & 0xF) == 1)
        classobj = slotRawObject(slot)->classptr;
    else
        classobj = gTagClassTable[tag];

    return classobj;
}

typedef int (*ObjFuncPtr)(struct VMGlobals*, struct PyrObject*);

void stringFromPyrString(PyrString* obj, char* str, int maxlength);
void pstringFromPyrString(PyrString* obj, unsigned char* str, int maxlength);

int instVarOffset(const char* classname, const char* instvarname);
int classVarOffset(const char* classname, const char* classvarname, PyrClass** classobj);

void fillSlots(PyrSlot* slot, int size, PyrSlot* fillslot);
void nilSlots(PyrSlot* slot, int size);
void zeroSlots(PyrSlot* slot, int size);

int calcHash(PyrSlot* a);
int getIndexedFloat(struct PyrObject* obj, int index, float* value);
int getIndexedDouble(struct PyrObject* obj, int index, double* value);

inline int getIndexedVal(struct PyrObject* obj, int index, float* value) { return getIndexedFloat(obj, index, value); }

inline int getIndexedVal(struct PyrObject* obj, int index, double* value) {
    return getIndexedDouble(obj, index, value);
}

void getIndexedSlot(struct PyrObject* obj, PyrSlot* a, int index);
int putIndexedSlot(struct VMGlobals* g, struct PyrObject* obj, PyrSlot* c, int index);
int putIndexedFloat(PyrObject* obj, double val, int index);

inline long ARRAYMAXINDEXSIZE(PyrObjectHdr* obj) { return (1L << obj->obj_sizeclass); }

inline long MAXINDEXSIZE(PyrObjectHdr* obj) { return ((1L << obj->obj_sizeclass) * gFormatElemCapc[obj->obj_format]); }

std::tuple<int, std::vector<std::string>> PyrCollToVectorStdString(PyrObject* coll);

void InstallFinalizer(VMGlobals* g, PyrObject* inObj, int slotIndex, ObjFuncPtr inFunc);
