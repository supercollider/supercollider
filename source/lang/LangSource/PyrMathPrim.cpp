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

#include "PyrKernel.h"
#include "PyrPrimitive.h"
#include "PyrMathPrim.h"
#include "MiscInlineMath.h"
#include "SC_InlineUnaryOp.h"
#include "SC_InlineBinaryOp.h"
#include "PyrSignal.h"
#include "PyrParseNode.h"
#include "PyrMessage.h"
#include "clz.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "SC_Endian.h"


inline bool IsSignal(PyrSlot* slot) { return ((slot)->utag == tagObj && (slot)->uo->classptr == class_signal); }
inline bool NotSignal(PyrSlot* slot) { return ((slot)->utag != tagObj || (slot)->uo->classptr != class_signal); }

int prAddNum(VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a, *b;
	PyrSymbol *msg;
	
	a = g->sp - 1;
	b = g->sp;

	switch (a->utag) {
		case tagInt :
			switch (b->utag) {
				case tagInt :
					a->ui = a->ui + b->ui; 
					break;
				case tagChar : case tagHFrame : case tagSFrame : case tagPtr : 
				case tagNil : case tagFalse : case tagTrue : case tagInf :  
					goto send_normal_2;
				case tagSym : 
					a->us = b->us; a->utag = tagSym;
					break;
				case tagObj :
					if (isKindOf(b->uo, class_signal)) {
						a->uo = signal_add_xf(g, b->uo, a->ui); a->utag = tagObj; 
					} else {
						goto send_normal_2;
					}
					break;
				default :
					a->uf = a->ui + b->uf; 
					break;
			}
			break;
		case tagChar : case tagHFrame : case tagSFrame : case tagPtr : 
		case tagNil : case tagFalse : case tagTrue : case tagInf : 
			goto send_normal_2;
		case tagSym : 
			// leave self in 'a'
			break;
		case tagObj :
			if (isKindOf(a->uo, class_signal)) {
				switch (b->utag) {
					case tagInt :
						a->uo = signal_add_xf(g, a->uo, b->ui); //a->utag = tagObj; 
						break;
					case tagChar : case tagHFrame : case tagSFrame : case tagPtr : 
					case tagNil : case tagFalse : case tagTrue : case tagInf : 
						goto send_normal_2;
					case tagSym : 
						a->us = b->us; a->utag = tagSym;
						break;
					case tagObj : 
						if (isKindOf(b->uo, class_signal)) {
							a->uo = signal_add_xx(g, a->uo, b->uo); //a->utag = tagObj; 
						} else goto send_normal_2;
						break;
					default : // double
						a->uo = signal_add_xf(g, a->uo, b->uf); //a->utag = tagObj; 
						break;
				}
			} else {
				goto send_normal_2;
			}
			break;
		default : // double
			switch (b->utag) {
				case tagInt :
					a->uf = a->uf + b->ui; 
					break;
				case tagChar : case tagHFrame : case tagSFrame : case tagPtr : 
				case tagNil : case tagFalse : case tagTrue : case tagInf : 
					goto send_normal_2;
				case tagSym : 
					a->us = b->us; a->utag = tagSym;
					break;
				case tagObj :
					if (isKindOf(b->uo, class_signal)) {
						a->uo = signal_add_xf(g, b->uo, a->uf);  a->utag = tagObj; break;
					} else goto send_normal_2;
					break;
				default : // double
					a->uf = a->uf + b->uf; break;
					break;
			}
			break;
	}
	g->sp-- ; // drop
	g->numpop = 0;
	return errNone;
	
	send_normal_2:
	if (numArgsPushed != -1)  // special case flag meaning it is a primitive
		return errFailed;	// arguments remain on the stack
	
	msg = gSpecialBinarySelectors[g->primitiveIndex];
	sendMessage(g, msg, 2);
	return errNone;	
}

int prMulNum(VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a, *b;
	PyrSymbol *msg;
	
	a = g->sp - 1;
	b = g->sp;

	switch (a->utag) {
		case tagInt :
			switch (b->utag) {
				case tagInt :
					a->ui = a->ui * b->ui; //a->utag = tagInt; 
					break;
				case tagChar : case tagHFrame : case tagSFrame : case tagPtr : 
				case tagNil : case tagFalse : case tagTrue : case tagInf :  
					goto send_normal_2;
				case tagSym : 
					a->us = b->us; a->utag = tagSym;
					break;
				case tagObj :
					if (isKindOf(b->uo, class_signal)) {
						a->uo = signal_mul_xf(g, b->uo, a->ui); a->utag = tagObj; 
					} else {
						goto send_normal_2;
					}
					break;
				default :
					a->uf = a->ui * b->uf; 
					break;
			}
			break;
		case tagChar : case tagHFrame : case tagSFrame : case tagPtr : 
		case tagNil : case tagFalse : case tagTrue : case tagInf : 
			goto send_normal_2;
		case tagSym : 
			//a->us = a->us; a->utag = tagSym;
			break;
		case tagObj :
			if (isKindOf(a->uo, class_signal)) {
				switch (b->utag) {
					case tagInt :
						a->uo = signal_mul_xf(g, a->uo, b->ui); //a->utag = tagObj; 
						break;
					case tagChar : case tagHFrame : case tagSFrame : case tagPtr : 
					case tagNil : case tagFalse : case tagTrue : case tagInf : 
						goto send_normal_2;
					case tagSym : 
						a->us = b->us; a->utag = tagSym;
						break;
					case tagObj : 
						if (isKindOf(b->uo, class_signal)) {
							a->uo = signal_mul_xx(g, a->uo, b->uo); //a->utag = tagObj; 
						} else goto send_normal_2;
						break;
					default : // double
						a->uo = signal_mul_xf(g, a->uo, b->uf); //a->utag = tagObj; 
						break;
				}
			} else {
				goto send_normal_2;
			}
			break;
		default : // double
			switch (b->utag) {
				case tagInt :
					a->uf = a->uf * b->ui; 
					break;
				case tagChar : case tagHFrame : case tagSFrame : case tagPtr : 
				case tagNil : case tagFalse : case tagTrue : case tagInf : 
					goto send_normal_2;
				case tagSym : 
					a->us = b->us; a->utag = tagSym;
					break;
				case tagObj :
					if (isKindOf(b->uo, class_signal)) {
						a->uo = signal_mul_xf(g, b->uo, a->uf);  a->utag = tagObj; break;
					} else goto send_normal_2;
					break;
				default : // double
					a->uf = a->uf * b->uf; break;
					break;
			}
			break;
	}
	g->sp-- ; // drop
	g->numpop = 0;
	return errNone;
	
	send_normal_2:
	if (numArgsPushed != -1)  // special case flag meaning it is a primitive
		return errFailed;	// arguments remain on the stack
	
	msg = gSpecialBinarySelectors[g->primitiveIndex];
	sendMessage(g, msg, 2);
	return errNone;	
}

int prSubNum(VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a, *b;
	PyrSymbol *msg;
	
	a = g->sp - 1;
	b = g->sp;

	switch (a->utag) {
		case tagInt :
			switch (b->utag) {
				case tagInt :
					a->ui = a->ui - b->ui; //a->utag = tagInt; 
					break;
				case tagChar : case tagHFrame : case tagSFrame : case tagPtr : 
				case tagNil : case tagFalse : case tagTrue : case tagInf : 
					goto send_normal_2;
				case tagSym : 
					a->us = b->us; a->utag = tagSym;
					break;
				case tagObj :
					if (isKindOf(b->uo, class_signal)) {
						a->uo = signal_sub_fx(g, a->ui, b->uo); a->utag = tagObj; 
					} else {
						goto send_normal_2;
					}
					break;
				default :
					a->uf = a->ui - b->uf; 
					break;
			}
			break;
		case tagChar : case tagHFrame : case tagSFrame : case tagPtr : 
		case tagNil : case tagFalse : case tagTrue : case tagInf : 
			goto send_normal_2;
		case tagSym : 
			//a->us = a->us; a->utag = tagSym;
			break;
		case tagObj :
			if (isKindOf(a->uo, class_signal)) {
				switch (b->utag) {
					case tagInt :
						a->uo = signal_sub_xf(g, a->uo, b->ui); //a->utag = tagObj; 
						break;
					case tagChar : case tagHFrame : case tagSFrame : case tagPtr : 
					case tagNil : case tagFalse : case tagTrue : case tagInf : 
						goto send_normal_2;
					case tagSym : 
						a->us = b->us; a->utag = tagSym;
						break;
					case tagObj : 
						if (isKindOf(b->uo, class_signal)) {
							a->uo = signal_sub_xx(g, a->uo, b->uo); //a->utag = tagObj; 
						} else goto send_normal_2;
						break;
					default : // double
						a->uo = signal_sub_xf(g, a->uo, b->uf); //a->utag = tagObj; 
						break;
				}
			} else {
				goto send_normal_2;
			}
			break;
		default : // double
			switch (b->utag) {
				case tagInt :
					a->uf = a->uf - b->ui; 
					break;
				case tagChar : case tagHFrame : case tagSFrame : case tagPtr : 
				case tagNil : case tagFalse : case tagTrue : case tagInf : 
					goto send_normal_2;
				case tagSym : 
					a->us = b->us; a->utag = tagSym;
					break;
				case tagObj :
					if (isKindOf(b->uo, class_signal)) {
						a->uo = signal_sub_fx(g, a->uf, b->uo);  a->utag = tagObj; break;
					} else goto send_normal_2;
					break;
				default : // double
					a->uf = a->uf - b->uf; break;
					break;
			}
			break;
	}
	g->sp-- ; // drop
	g->numpop = 0;
	return errNone;
	
	send_normal_2:
	if (numArgsPushed != -1)  // special case flag meaning it is a primitive
		return errFailed;	// arguments remain on the stack
	
	msg = gSpecialBinarySelectors[g->primitiveIndex];
	sendMessage(g, msg, 2);
	return errNone;	
}

int prAddInt(VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a, *b;
	PyrSymbol *msg;
	
	a = g->sp - 1;
	b = g->sp;

	switch (b->utag) {
		case tagInt :
			a->ui = a->ui + b->ui; //a->utag = tagInt; 
			break;
		case tagChar : case tagPtr : case tagHFrame : case tagSFrame :
		case tagNil : case tagFalse : case tagTrue : case tagInf : 
			goto send_normal_2;
		case tagSym : 
			a->us = b->us; a->utag = tagSym;
			break;
		case tagObj :
			if (isKindOf(b->uo, class_signal)) {
				a->uo = signal_add_xf(g, b->uo, a->ui); a->utag = tagObj; 
			} else {
				goto send_normal_2;
			}
			break;
		default :
			a->uf = a->ui + b->uf; 
			break;
	}
	g->sp-- ; // drop
	g->numpop = 0;
	return errNone;		
	
	send_normal_2:
	if (numArgsPushed != -1)  // special case flag meaning it is a primitive
		return errFailed;	// arguments remain on the stack
	
	msg = gSpecialBinarySelectors[g->primitiveIndex];
	sendMessage(g, msg, 2);
	return errNone;	
}



int prAddFloat(VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a, *b;
	PyrSymbol *msg;
	
	a = g->sp - 1;
	b = g->sp;

	switch (b->utag) {
		case tagInt :
			a->uf = a->uf + b->ui; break;
			break;
		case tagChar : case tagPtr : case tagHFrame : case tagSFrame :
		case tagNil : case tagFalse : case tagTrue : case tagInf : 
			goto send_normal_2;
		case tagSym : 
			a->us = b->us; a->utag = tagSym;
			break;
		case tagObj :
			if (isKindOf(b->uo, class_signal)) {
				a->uo = signal_add_xf(g, b->uo, a->uf); a->utag = tagObj; 
			} else {
				goto send_normal_2;
			}
			break;
		default :
			a->uf = a->uf + b->uf; 
			break;
	}
	g->sp-- ; // drop
	g->numpop = 0;
	return errNone;		
	
	send_normal_2:
	if (numArgsPushed != -1)  // special case flag meaning it is a primitive
		return errFailed;	// arguments remain on the stack
	
	msg = gSpecialBinarySelectors[g->primitiveIndex];
	sendMessage(g, msg, 2);
	return errNone;	
}

int prSubInt(VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a, *b;
	PyrSymbol *msg;
	
	a = g->sp - 1;
	b = g->sp;

	switch (b->utag) {
		case tagInt :
			a->ui = a->ui - b->ui; //a->utag = tagInt; 
			break;
		case tagChar : case tagPtr : case tagHFrame : case tagSFrame :
		case tagNil : case tagFalse : case tagTrue : case tagInf : 
			goto send_normal_2;
		case tagSym : 
			a->us = b->us; a->utag = tagSym;
			break;
		case tagObj :
			if (isKindOf(b->uo, class_signal)) {
				a->uo = signal_sub_xf(g, b->uo, a->ui); a->utag = tagObj; 
			} else {
				goto send_normal_2;
			}
			break;
		default :
			a->uf = a->ui - b->uf; 
			break;
	}
	g->sp-- ; // drop
	g->numpop = 0;
	return errNone;		
	
	send_normal_2:
	if (numArgsPushed != -1)  // special case flag meaning it is a primitive
		return errFailed;	// arguments remain on the stack
	
	msg = gSpecialBinarySelectors[g->primitiveIndex];
	sendMessage(g, msg, 2);
	return errNone;	
}



int prSubFloat(VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a, *b;
	PyrSymbol *msg;
	
	a = g->sp - 1;
	b = g->sp;

	switch (b->utag) {
		case tagInt :
			a->uf = a->uf - b->ui; break;
			break;
		case tagChar : case tagPtr : case tagHFrame : case tagSFrame :
		case tagNil : case tagFalse : case tagTrue : case tagInf : 
			goto send_normal_2;
		case tagSym : 
			a->us = b->us; a->utag = tagSym;
			break;
		case tagObj :
			if (isKindOf(b->uo, class_signal)) {
				a->uo = signal_sub_xf(g, b->uo, a->uf); a->utag = tagObj; break;
			} else {
				goto send_normal_2;
			}
			break;
		default :
			a->uf = a->uf - b->uf; 
			break;
	}
	g->sp-- ; // drop
	g->numpop = 0;
	return errNone;		
	
	send_normal_2:
	if (numArgsPushed != -1)  // special case flag meaning it is a primitive
		return errFailed;	// arguments remain on the stack
	
	msg = gSpecialBinarySelectors[g->primitiveIndex];
	sendMessage(g, msg, 2);
	return errNone;	
}

int prMulInt(VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a, *b;
	PyrSymbol *msg;
	
	a = g->sp - 1;
	b = g->sp;

	switch (b->utag) {
		case tagInt :
			a->ui = a->ui * b->ui; //a->utag = tagInt; break;
			break;
		case tagChar : case tagPtr : case tagHFrame : case tagSFrame :
		case tagNil : case tagFalse : case tagTrue : case tagInf : 
			goto send_normal_2;
		case tagSym : 
			a->us = b->us; a->utag = tagSym;
			break;
		case tagObj :
			if (isKindOf(b->uo, class_signal)) {
				a->uo = signal_mul_xf(g, b->uo, a->ui); a->utag = tagObj; break;
			} else {
				goto send_normal_2;
			}
			break;
		default :
			a->uf = a->ui * b->uf; 
			break;
	}
	g->sp-- ; // drop
	g->numpop = 0;
	return errNone;		
	
	send_normal_2:
	if (numArgsPushed != -1)  // special case flag meaning it is a primitive
		return errFailed;	// arguments remain on the stack
	
	msg = gSpecialBinarySelectors[g->primitiveIndex];
	sendMessage(g, msg, 2);
	return errNone;	
}



int prMulFloat(VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a, *b;
	PyrSymbol *msg;
		
	a = g->sp - 1;
	b = g->sp;

	switch (b->utag) {
		case tagInt :
			a->uf = a->uf * b->ui; break;
			break;
		case tagChar : case tagPtr : case tagHFrame : case tagSFrame :
		case tagNil : case tagFalse : case tagTrue : case tagInf : 
			goto send_normal_2;
		case tagSym : 
			a->us = b->us; a->utag = tagSym;
			break;
		case tagObj :
			if (isKindOf(b->uo, class_signal)) {
				a->uo = signal_mul_xf(g, b->uo, a->uf); a->utag = tagObj; break;
			} else {
				goto send_normal_2;
			}
			break;
		default :
			a->uf = a->uf * b->uf; 
			break;
	}
	g->sp-- ; // drop
	g->numpop = 0;
	return errNone;		
	
	send_normal_2:
	if (numArgsPushed != -1)  // special case flag meaning it is a primitive
		return errFailed;	// arguments remain on the stack
	
	msg = gSpecialBinarySelectors[g->primitiveIndex];
	sendMessage(g, msg, 2);
	return errNone;	
}


int prNthPrime(VMGlobals *g, int numArgsPushed);
int prNthPrime(VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a;
	int n, p;
	
	a = g->sp;
	n = a->ui;
	p = nthPrime(n);
	if (p == 0) {
		SetNil(a);
	} else {
		SetInt(a, p);
	}
	return errNone;	
}

int prPrevPrime(VMGlobals *g, int numArgsPushed);
int prPrevPrime(VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a;
	int n, p, i;
	
	a = g->sp;
	n = a->ui;
	i = prevPrime(n);
	p = nthPrime(i);
	if (p == 0) {
		SetNil(a);
	} else {
		SetInt(a, p);
	}
	return errNone;	
}

int prNextPrime(VMGlobals *g, int numArgsPushed);
int prNextPrime(VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a;
	int n, p, i;
	
	a = g->sp;
	n = a->ui;
	i = nextPrime(n);
	p = nthPrime(i);
	if (p == 0) {
		SetNil(a);
	} else {
		SetInt(a, p);
	}
	return errNone;	
}


int prIsPrime(VMGlobals *g, int numArgsPushed);
int prIsPrime(VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a;
	int n, p, sqrtn, i;
	
	a = g->sp;
	n = a->ui;
	SetNil(a);
	if (n <= 2) {
		if (n == 2) { SetTrue(a); }
		else { SetFalse(a); }
	} else if (n <= nthPrime(NUMPRIMES-1)) {
		// do a search of the primes table
		i = findPrime(n);
		if (i >= 0) { SetTrue(a); }
		else { SetFalse(a); }
	} else {
		sqrtn = (int)sqrt(n);
		for (i=0; i<NUMPRIMES; ++i) {
			p = nthPrime(i);
			if (n % p == 0) { SetFalse(a); break; }
			if (p >= sqrtn) { SetTrue(a); break; }
		}
	}
	return errNone;	
}

int prAs32Bits(VMGlobals *g, int numArgsPushed);
int prAs32Bits(VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a = g->sp;
	// return an integer that is a bit pattern for the 32 bit float representation
	union { float f; int32 i; } u;
	u.f = a->uf;
	SetInt(a, u.i);
	return errNone;
}

int prHigh32Bits(VMGlobals *g, int numArgsPushed);
int prHigh32Bits(VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a = g->sp;
	
#if BYTE_ORDER == BIG_ENDIAN
	union { struct { uint32 hi, lo; } i; double f; } du;
#else
	union { struct { uint32 lo, hi; } i; double f; } du;
#endif

	du.f = a->uf;
	SetInt(a, du.i.hi);
	return errNone;
}

int prLow32Bits(VMGlobals *g, int numArgsPushed);
int prLow32Bits(VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a = g->sp;
	
#if BYTE_ORDER == BIG_ENDIAN
	union { struct { uint32 hi, lo; } i; double f; } du;
#else
	union { struct { uint32 lo, hi; } i; double f; } du;
#endif

	du.f = a->uf;
	SetInt(a, du.i.lo);
	return errNone;
}


int mathClipInt(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a, *b, *c;
	double lo, hi;
	int err;
	
	a = g->sp - 2;
	b = g->sp - 1;
	c = g->sp;
	
	if (IsSym(b)) {
		*a = *b;
	} else if (IsSym(c)) {
		*a = *c;
	} else if (IsInt(b) && IsInt(c)) {
		a->ui = sc_clip(a->ui, b->ui, c->ui);
	} else {
		err = slotDoubleVal(b, &lo);
		if (err) return err;
		err = slotDoubleVal(c, &hi);
		if (err) return err;
		a->uf = sc_clip(a->ui, lo, hi);
	}
	return errNone;
}

int mathClipFloat(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a, *b, *c;
	double lo, hi;
	int err;
	
	a = g->sp - 2;
	b = g->sp - 1;
	c = g->sp;
	
	if (IsSym(b)) {
		*a = *b;
	} else if (IsSym(c)) {
		*a = *c;
	} else {
		err = slotDoubleVal(b, &lo);
		if (err) return err;
		err = slotDoubleVal(c, &hi);
		if (err) return err;
		a->uf = sc_clip(a->uf, lo, hi);
	}
	return errNone;
}

int mathClipSignal(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a, *b, *c;
	float lo, hi;
	int err;
	PyrObject *sig;
	
	a = g->sp - 2;
	b = g->sp - 1;
	c = g->sp;
	
	if (IsSym(b)) {
		*a = *b;
	} else if (IsSym(c)) {
		*a = *c;
	} else if (IsSignal(b) && IsSignal(c)) {
		sig = signal_clip_x(g, a->uo, b->uo, c->uo);
		SetObject(a, sig);
	} else {
		err = slotFloatVal(b, &lo);
		if (err) return err;
		err = slotFloatVal(c, &hi);
		if (err) return err;
		sig = signal_clip_f(g, a->uo, lo, hi);
		SetObject(a, sig);
	}
	return errNone;
}

int mathWrapInt(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a, *b, *c;
	double lo, hi;
	int err;
	
	a = g->sp - 2;
	b = g->sp - 1;
	c = g->sp;
	
	if (IsSym(b)) {
		*a = *b;
	} else if (IsSym(c)) {
		*a = *c;
	} else if (IsInt(b) && IsInt(c)) {
		a->ui = sc_mod((int)(a->ui - b->ui), (int)(c->ui - b->ui + 1)) + b->ui;
	} else {
		err = slotDoubleVal(b, &lo);
		if (err) return err;
		err = slotDoubleVal(c, &hi);
		if (err) return err;
		a->uf = sc_mod(a->uf - lo, hi - lo) + lo;
	}
	return errNone;
}

int mathWrapFloat(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a, *b, *c;
	double lo, hi;
	int err;
	
	a = g->sp - 2;
	b = g->sp - 1;
	c = g->sp;
	
	if (IsSym(b)) {
		*a = *b;
	} else if (IsSym(c)) {
		*a = *c;
	} else {
		err = slotDoubleVal(b, &lo);
		if (err) return err;
		err = slotDoubleVal(c, &hi);
		if (err) return err;
		a->uf = sc_mod(a->uf - lo, hi - lo) + lo;
	}
	return errNone;
}

int mathWrapSignal(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a, *b, *c;
	float lo, hi;
	int err;
	PyrObject *sig;
	
	a = g->sp - 2;
	b = g->sp - 1;
	c = g->sp;
	
	if (IsSym(b)) {
		*a = *b;
	} else if (IsSym(c)) {
		*a = *c;
	} else if (IsSignal(b) && IsSignal(c)) {
		sig = signal_wrap_x(g, a->uo, b->uo, c->uo);
		SetObject(a, sig);
	} else {
		err = slotFloatVal(b, &lo);
		if (err) return err;
		err = slotFloatVal(c, &hi);
		if (err) return err;
		sig = signal_wrap_f(g, a->uo, lo, hi);
		SetObject(a, sig);
	}
	return errNone;
}

int mathFoldInt(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a, *b, *c;
	double lo, hi;
	int err;
	
	a = g->sp - 2;
	b = g->sp - 1;
	c = g->sp;
	
	if (IsSym(b)) {
		*a = *b;
	} else if (IsSym(c)) {
		*a = *c;
	} else if (IsInt(b) && IsInt(c)) {
		a->ui = sc_fold(a->ui - b->ui, 0, c->ui - b->ui + 1) + b->ui;
	} else {
		err = slotDoubleVal(b, &lo);
		if (err) return err;
		err = slotDoubleVal(c, &hi);
		if (err) return err;
		a->uf = sc_wrap(a->uf, lo, hi);
	}
	return errNone;
}

int mathFoldFloat(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a, *b, *c;
	double lo, hi;
	int err;
	
	a = g->sp - 2;
	b = g->sp - 1;
	c = g->sp;
	
	if (IsSym(b)) {
		*a = *b;
	} else if (IsSym(c)) {
		*a = *c;
	} else {
		err = slotDoubleVal(b, &lo);
		if (err) return err;
		err = slotDoubleVal(c, &hi);
		if (err) return err;
		a->uf = sc_fold(a->uf, lo, hi);
	}
	return errNone;
}

int mathFoldSignal(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a, *b, *c;
	float lo, hi;
	int err;
	PyrObject *sig;
	
	a = g->sp - 2;
	b = g->sp - 1;
	c = g->sp;
	
	if (IsSym(b)) {
		*a = *b;
	} else if (IsSym(c)) {
		*a = *c;
	} else if (IsSignal(b) && IsSignal(c)) {
		sig = signal_fold_x(g, a->uo, b->uo, c->uo);
		SetObject(a, sig);
	} else {
		err = slotFloatVal(b, &lo);
		if (err) return err;
		err = slotFloatVal(c, &hi);
		if (err) return err;
		sig = signal_fold_f(g, a->uo, lo, hi);
		SetObject(a, sig);
	}
	return errNone;
}

int prSimpleNumberSeries(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a = g->sp - 2;
	PyrSlot *b = g->sp - 1;
	PyrSlot *c = g->sp;
	
	int err, size;
	
	if (IsInt(a) && (IsInt(b) || IsNil(b)) && IsInt(c)) {
		int first, second, last, step, val;
		first = a->ui;
		last = c->ui;
		second = IsInt(b) ? b->ui : (first < last ? first + 1 : first - 1);
		step = second - first;

		size = ((last - first) / step) + 1;

		PyrObject *obj = newPyrArray(g->gc, size, 0, true);
		obj->size = size;
		PyrSlot *slots = obj->slots;
		val = first;
		for (int i=0; i<size; ++i) {
			SetInt(slots+i, val);
			val += step;
		}
		SetObject(a, obj);
	} else {
		double first, second, last, step, val;
		err = slotDoubleVal(a, &first);
		if (err) return err;
		err = slotDoubleVal(c, &last);
		if (err) return err;
		err = slotDoubleVal(b, &second);
		if (err) {
			if (first < last) second = first + 1.;
			else second = first - 1.;
		}
		
		step = second - first;
		size = (int)floor((last - first) / step + 0.001) + 1;
		PyrObject *obj = newPyrArray(g->gc, size, 0, true);
		obj->size = size;
		PyrSlot *slots = obj->slots;
		val = first;
		for (int i=0; i<size; ++i) {
			val = first + step * i;
			SetFloat(slots+i, val);
		}
		SetObject(a, obj);
	}
	return errNone;
/*	series { arg second, last;
		var step, size;
		second = second ?? { if (this < last) { this + 1 } { this - 1 } };
		step = second - this;
		size = floor((last - this) / step).asInteger + 1;
		^Array.series(size, this, step)
	}*/

}

void initMathPrimitives()
{
	int base, index;
		
	base = nextPrimitiveIndex();
	index = 0;
	definePrimitive(base, index++, "_AddInt", prAddInt, 2, 0);
	definePrimitive(base, index++, "_SubInt", prSubInt, 2, 0);
	definePrimitive(base, index++, "_MulInt", prMulInt, 2, 0);
	definePrimitive(base, index++, "_AddFloat", prAddFloat, 2, 0);
	definePrimitive(base, index++, "_SubFloat", prSubFloat, 2, 0);
	definePrimitive(base, index++, "_MulFloat", prMulFloat, 2, 0);
	definePrimitive(base, index++, "_NthPrime", prNthPrime, 1, 0);
	definePrimitive(base, index++, "_PrevPrime", prPrevPrime, 1, 0);
	definePrimitive(base, index++, "_NextPrime", prNextPrime, 1, 0);
	definePrimitive(base, index++, "_IsPrime", prIsPrime, 1, 0);
	definePrimitive(base, index++, "_As32Bits", prAs32Bits, 1, 0);
	definePrimitive(base, index++, "_High32Bits", prHigh32Bits, 1, 0);
	definePrimitive(base, index++, "_Low32Bits", prLow32Bits, 1, 0);
	
	definePrimitive(base, index++, "_ClipInt", mathClipInt, 3, 0);
	definePrimitive(base, index++, "_ClipFloat", mathClipFloat, 3, 0);
	definePrimitive(base, index++, "_ClipSignal", mathClipSignal, 3, 0);
	definePrimitive(base, index++, "_WrapInt", mathWrapInt, 3, 0);
	definePrimitive(base, index++, "_WrapFloat", mathWrapFloat, 3, 0);
	definePrimitive(base, index++, "_WrapSignal", mathWrapSignal, 3, 0);
	definePrimitive(base, index++, "_FoldInt", mathFoldInt, 3, 0);
	definePrimitive(base, index++, "_FoldFloat", mathFoldFloat, 3, 0);
	definePrimitive(base, index++, "_FoldSignal", mathFoldSignal, 3, 0);
	
	definePrimitive(base, index++, "_SimpleNumberSeries", prSimpleNumberSeries, 3, 0);
}

