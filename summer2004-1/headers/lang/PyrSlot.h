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
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
/*

PyrSlot is a value holder for SC variables.
A PyrSlot is an 8-byte value which is either a double precision float or a 
32-bit tag plus a 32-bit value.

*/

#ifndef _PYRSLOT_H_
#define _PYRSLOT_H_

#include "SC_Endian.h"
#include "PyrSymbol.h"

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
	tagObj     = 0x7FF90001,
	tagHFrame  = 0x7FF90002,
	tagSFrame  = 0x7FF90003,
	tagInt     = 0x7FF90004,
	tagSym     = 0x7FF90005,
	tagChar    = 0x7FF90006,
	tagNil     = 0x7FF90007,	// nil, false, and true are indicated by the tag alone.
	tagFalse   = 0x7FF90008,	// the lower 32 bits are zero.
	tagTrue    = 0x7FF90009,
	tagInf     = 0x7FF9000A,
	tagPtr     = 0x7FF9000B,
	/* anything else is a double */
	tagUnused  = 0x7FF9000E
	
	
#if !DOUBLESLOTS	
	,tagFloat = 0x7FF9000F /* used only to initialized 4 byte float tags, never compared with */
#endif
};

struct RGBColor8 {
	unsigned char c[4];
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
			void *ptr;
			struct RGBColor8 r;
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
			struct PyrParseNode *opn;
			struct PyrProcess *op;
			struct PyrThread *ot;
			struct PyrInterpreter *oi;
			struct PyrPlug *plug;
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
//int
#define ui s.u.i
//PyrObject
#define uo s.u.o
//PyrSymbol
#define us s.u.s
//RGBColor8
#define ur s.u.r
#define uc s.u.c
#define uoc s.u.oc
#define uof s.u.of
#define uol s.u.ol
#define uod s.u.od
#define uob s.u.ob
#define uop s.u.op
#define uoi s.u.oi
#define uod s.u.od
//string
#define uos s.u.os
#define uot s.u.ot
//method
#define uom s.u.om
//symbol array
#define uosym s.u.osym
#define uoblk s.u.oblk
#define uopn s.u.opn
#define uptr s.u.ptr
#define uplug s.u.plug

#if DOUBLESLOTS
#define uf f
#else
#define uf s.u.f
#endif

#define ucopy f

/* 
	Note that on the PowerPC, the fastest way to copy a slot is to
	copy the double field, not the struct.
*/

/* some macros for setting values of slots */
inline void SetInt(PyrSlot* slot, int val)    {  (slot)->utag = tagInt;  (slot)->ui = (val); }
inline void SetObject(PyrSlot* slot, void* val) {  (slot)->utag = tagObj;   (slot)->uo = (PyrObject*)(val); }
inline void SetSymbol(PyrSlot* slot, PyrSymbol *val) {  (slot)->utag = tagSym;   (slot)->us = (val); }
inline void SetChar(PyrSlot* slot, char val)   {  (slot)->utag = tagChar;  (slot)->uc = (val); }
inline void SetPtr(PyrSlot* slot, void* val)    {  (slot)->utag = tagPtr;  (slot)->uptr = (void*)(val); }
inline void SetObjectOrNil(PyrSlot* slot, PyrObject* val)  
{ 
	if (val) { 
		(slot)->utag = tagObj; 
		(slot)->uo = (val); 
	} else {
		(slot)->utag = tagNil;  
		(slot)->ui = 0; 
	}
}
									
inline void SetTrue(PyrSlot* slot)   { (slot)->utag = tagTrue; (slot)->ui = 0; }
inline void SetFalse(PyrSlot* slot)   { (slot)->utag = tagFalse; (slot)->ui = 0; }
inline void SetBool(PyrSlot* slot, bool test)	{ (slot)->utag = ((test) ? tagTrue : tagFalse); (slot)->ui = 0;  }
inline void SetNil(PyrSlot* slot)    { (slot)->utag = tagNil;  (slot)->ui = 0; }
inline void SetInf(PyrSlot* slot)   { (slot)->utag = tagInf;  (slot)->ui = 0; }

#if DOUBLESLOTS
inline void SetFloat(PyrSlot* slot, double val)    { (slot)->uf = (val); }
#else
inline void SetFloat(PyrSlot* slot, double val)    { (slot)->utag = s_float; (slot)->uf = (val); }
#endif

inline bool IsObj(PyrSlot* slot) { return ((slot)->utag == tagObj); }
inline bool NotObj(PyrSlot* slot) { return ((slot)->utag != tagObj); }

inline bool IsNil(PyrSlot* slot) { return ((slot)->utag == tagNil); }
inline bool NotNil(PyrSlot* slot) { return ((slot)->utag != tagNil); }

inline bool IsFalse(PyrSlot* slot) { return ((slot)->utag == tagFalse); }
inline bool IsTrue(PyrSlot* slot) { return ((slot)->utag == tagTrue); }

inline bool SlotEq(PyrSlot* a, PyrSlot* b) { return ((a)->ui == (b)->ui && (a)->utag == (b)->utag); }

inline bool IsSym(PyrSlot* slot) { return ((slot)->utag == tagSym); }
inline bool NotSym(PyrSlot* slot) { return ((slot)->utag != tagSym); }

inline bool IsInt(PyrSlot* slot) { return ((slot)->utag == tagInt); }
inline bool NotInt(PyrSlot* slot) { return ((slot)->utag != tagInt); }

inline bool IsFloatTag(int tag)  { return ((tag & 0xFFFFFFF0) != 0x7FF90000); }
inline bool IsFloat(PyrSlot* slot) { return (((slot)->utag & 0xFFFFFFF0) != 0x7FF90000); }
inline bool NotFloat(PyrSlot* slot) { return (((slot)->utag & 0xFFFFFFF0) == 0x7FF90000); }

inline bool IsInf(PyrSlot* slot) { return ((slot)->utag == tagInf); }
inline bool IsPtr(PyrSlot* slot) { return ((slot)->utag == tagPtr); }

inline bool IsFrame(PyrSlot* slot) { return ((slot)->utag == tagHFrame || (slot)->utag == tagSFrame); }


void dumpPyrSlot(PyrSlot* slot);
void slotString(PyrSlot *slot, char *str);
void slotOneWord(PyrSlot *slot, char *str);
bool postString(PyrSlot *slot, char *str);
char *slotSymString(PyrSlot* slot);
int asCompileString(PyrSlot *slot, char *str);

int slotIntVal(PyrSlot* slot, int *value);
int slotFloatVal(PyrSlot* slot, float *value);
int slotDoubleVal(PyrSlot *slot, double *value);
int slotStrVal(PyrSlot *slot, char *str, int maxlen);
int slotPStrVal(PyrSlot *slot, unsigned char *str);
int slotSymbolVal(PyrSlot *slot, PyrSymbol **symbol);

extern PyrSlot o_nil, o_true, o_false, o_inf;
extern PyrSlot o_pi, o_twopi;
extern PyrSlot o_fhalf, o_fnegone, o_fzero, o_fone, o_ftwo;
extern PyrSlot o_negtwo, o_negone, o_zero, o_one, o_two;
extern PyrSlot o_emptyarray, o_onenilarray, o_argnamethis;

extern PyrSymbol *s_object; // "Object"
extern PyrSymbol *s_this; // "this"
extern PyrSymbol *s_super; // "super"

inline int slotFloatVal(PyrSlot *slot, float *value)
{
	if (IsFloat(slot)) {
		*value = slot->uf;
		return errNone;
	} else if (IsInt(slot)) {
		 *value = slot->ui;
		return errNone;
	}
	return errWrongType;
}

inline int slotIntVal(PyrSlot *slot, int *value)
{
	if (IsInt(slot)) {
		 *value = slot->ui;
		return errNone;
	} else if (IsFloat(slot)) {
		*value = (int)slot->uf;
		return errNone;
	}  
	return errWrongType;
}

inline int slotDoubleVal(PyrSlot *slot, double *value)
{
	if (IsFloat(slot)) {
		*value = slot->uf;
		return errNone;
	} else if (IsInt(slot)) {
		 *value = slot->ui;
		return errNone;
	}
	return errWrongType;
}

inline int slotSymbolVal(PyrSlot *slot, PyrSymbol **symbol)
{
	if (!IsSym(slot)) return errWrongType;
	*symbol = slot->us;
	return errNone;
}

inline void slotCopy(PyrSlot *dst, PyrSlot *src, int num)
{
	double *dstp = (double*)dst - 1;
	double *srcp = (double*)src - 1;
	for (int i=0;i<num;++i) { *++dstp = *++srcp; } 
}


#endif
