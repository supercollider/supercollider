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
#include "PyrKernelProto.h"
#include "PyrPrimitive.h"
#include "PyrPrimitiveProto.h"
#include "PyrListPrim.h"
#include "SC_InlineUnaryOp.h"
#include "SC_InlineBinaryOp.h"
#include "PyrSignal.h"
#include "PyrMessage.h"
#include "PyrSched.h"
#include "SC_RGen.h"
#include <stdlib.h>
#include <string.h>

int objectPerform(VMGlobals *g, int numArgsPushed);

int ivxIdentDict_array, ivxIdentDict_size;

int class_array_index, class_array_maxsubclassindex;
int class_identdict_index, class_identdict_maxsubclassindex;

PyrClass *class_identdict;
PyrSymbol *s_delta, *s_dur, *s_stretch;

#define HASHSYMBOL(sym) (sym >> 5)

#define ISKINDOF(obj, lo, hi)  (\
	objClassIndex = obj->classptr->classIndex.ui,	\
	objClassIndex >= lo && objClassIndex <= hi)


int prArrayMultiChanExpand(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a, *slot, *slots1, *slots2, *slots3, *slots4;
	PyrObject *obj1, *obj2, *obj3, *obj4;
	int i, j, size, len, maxlen;
	
	a = g->sp;
	obj1 = a->uo;
	size = obj1->size;
	slots1 = obj1->slots;
	maxlen = -1;
	for (j=0; j<size; ++j) {
		slot = slots1 + j;
		if (slot->utag == tagObj) {
			if (slot->uo->classptr == class_array) {
				len = slot->uo->size;
				maxlen = len > maxlen ? len : maxlen;
			}
		}
	}
	if (maxlen > 0) {
		obj2 = newPyrArray(g->gc, maxlen, 0, true);
		obj2->size = maxlen;
		slots2 = obj2->slots;
		for (i=0; i<maxlen; ++i) {
			obj3 = newPyrArray(g->gc, size, 0, false);
			obj3->size = size;
			SetObject(slots2 + i, obj3);
			slots1 = obj1->slots;
			slots3 = obj3->slots;
			for (j=0; j<size; ++j) {
				slot = slots1 + j;
				if (slot->utag == tagObj) {
					if (slot->uo->classptr == class_array && slot->uo->size > 0) {
						obj4 = slot->uo;
						slots4 = obj4->slots;
						slots3[j].ucopy = slots4[i % obj4->size].ucopy;
					} else {
						slots3[j].ucopy = slot->ucopy;
					}
				} else {
					slots3[j].ucopy = slot->ucopy;
				}
			}
		}
		SetObject(a, obj2);
	} else {
		SetNil(a);
	}
	return errNone;
}


int arrayAtIdentityHash(PyrObject *array, PyrSlot *key)
{
	PyrSlot *slots, *test;
	unsigned int i, start, end, hash, maxHash;
	
	hash = calcHash(key);
	maxHash = array->size;
	start = hash % maxHash;
	end = array->size;
	slots = array->slots;
	for (i=start; i<end; i++) {
		test = slots + i;
		if (IsNil(test) || (test->utag == key->utag && test->ui == key->ui)) {
			return i;
		}
	}
	end = start - 1;
	for (i=0; i<=end; i++) {
		test = slots + i;
		if (IsNil(test) || (test->utag == key->utag && test->ui == key->ui)) {
			return i;
		}
	}
	return -1;
} 


int prArray_AtIdentityHash(struct VMGlobals *g, int numArgsPushed);
int prArray_AtIdentityHash(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a, *b;
	PyrObject *array;
	int index;
	
	a = g->sp - 1;  // array
	b = g->sp;		// key

	array = a->uo;
	index = arrayAtIdentityHash(array, b);
	SetInt(a, index);
	return errNone;
}


int arrayAtIdentityHashInPairs(PyrObject *array, PyrSlot *key)
{
	PyrSlot *slots, *test;
	unsigned int i, start, end, hash, maxHash;
	
	hash = calcHash(key);
	maxHash = array->size >> 1;
	start = (hash % maxHash) << 1;
	end = array->size;
	slots = array->slots;
	for (i=start; i<end; i+=2) {
		test = slots + i;
		if (IsNil(test) || (test->utag == key->utag && test->ui == key->ui)) {
			return i;
		}
	}
	end = start - 2;
	for (i=0; i<=end; i+=2) {
		test = slots + i;
		if (IsNil(test) || (test->utag == key->utag && test->ui == key->ui)) {
			return i;
		}
	}
	return -2;
} 


int arrayAtIdentityHashInPairsWithHash(PyrObject *array, PyrSlot *key, int hash)
{
	PyrSlot *slots, *test;
	unsigned int i, start, end, maxHash;
	
	maxHash = array->size >> 1;
	start = (hash % maxHash) << 1;
	end = array->size;
	slots = array->slots;
	for (i=start; i<end; i+=2) {
		test = slots + i;
		if (IsNil(test) || (test->utag == key->utag && test->ui == key->ui)) {
			return i;
		}
	}
	end = start - 2;
	for (i=0; i<=end; i+=2) {
		test = slots + i;
		if (IsNil(test) || (test->utag == key->utag && test->ui == key->ui)) {
			return i;
		}
	}
	return -2;
} 


int identDictPut(struct VMGlobals *g, PyrObject *dict, PyrSlot *key, PyrSlot *value);
int identDictPut(struct VMGlobals *g, PyrObject *dict, PyrSlot *key, PyrSlot *value)
{
	PyrSlot *slot, *newslot;
	int i, index, size;
	PyrObject *array;
	
	array = dict->slots[ivxIdentDict_array].uo;
	if (!isKindOf((PyrObject*)array, class_array)) return errFailed;
	
	index = arrayAtIdentityHashInPairs(array, key);
	slot = array->slots + index;
	slot[1].ucopy = value->ucopy;
	g->gc->GCWrite(array, value);
	if (IsNil(slot)) {
		slot->ucopy = key->ucopy;
		g->gc->GCWrite(array, key);
		size = ++dict->slots[ivxIdentDict_size].ui;
		if (array->size < size*3) {
			PyrObject *newarray;
			newarray = newPyrArray(g->gc, size*3, 0, false);
			newarray->size = ARRAYMAXINDEXSIZE(newarray);
			nilSlots(newarray->slots, newarray->size);
			slot = array->slots;
			for (i=0; i<array->size; i+=2, slot+=2) {
				if (NotNil(slot)) {
					index = arrayAtIdentityHashInPairs(newarray, slot);
					newslot = newarray->slots + index;
					newslot[0].ucopy = slot[0].ucopy;
					newslot[1].ucopy = slot[1].ucopy;
				}
			}
			dict->slots[ivxIdentDict_array].uo = newarray;
			g->gc->GCWrite(dict, newarray);
		}
	}
	return errNone;
}

int prIdentDict_Put(struct VMGlobals *g, int numArgsPushed);
int prIdentDict_Put(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a, *b, *c;
	
	a = g->sp - 2;  // dict
	b = g->sp - 1;	// key
	c = g->sp;		// value
	if(IsNil(b)) return errWrongType;
	return identDictPut(g, a->uo, b, c);
}	

int prIdentDict_PutGet(struct VMGlobals *g, int numArgsPushed);
int prIdentDict_PutGet(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a, *b, *c, *d, *slot, *newslot;
	int i, index, size;
	PyrObject *dict;
	PyrObject *array;
	
	a = g->sp - 2;  // dict
	b = g->sp - 1;	// key
	c = g->sp;		// value
	d = ++g->sp;	// push the stack to save the receiver
	
	d->ucopy = a->ucopy;
	dict = d->uo;
	array = dict->slots[ivxIdentDict_array].uo;
	if (!isKindOf((PyrObject*)array, class_array)) {
		SetNil(a);
		--g->sp;
		return errFailed;
	}
	
	index = arrayAtIdentityHashInPairs(array, b);
	slot = array->slots + index;
	a->ucopy = slot[1].ucopy;
	slot[1].ucopy = c->ucopy;
	g->gc->GCWrite(array, c);
	if (IsNil(slot)) {
		slot->ucopy = b->ucopy;
		g->gc->GCWrite(array, b);
		size = ++dict->slots[ivxIdentDict_size].ui;
		if (array->size < size*3) {
			PyrObject *newarray;
			newarray = newPyrArray(g->gc, size*3, 0, true);
			newarray->size = ARRAYMAXINDEXSIZE(newarray);
			nilSlots(newarray->slots, newarray->size);
			slot = array->slots;
			for (i=0; i<array->size; i+=2, slot+=2) {
				if (NotNil(slot)) {
					index = arrayAtIdentityHashInPairs(newarray, slot);
					newslot = newarray->slots + index;
					newslot[0].ucopy = slot[0].ucopy;
					newslot[1].ucopy = slot[1].ucopy;
				}
			}
			dict->slots[ivxIdentDict_array].uo = newarray;
			g->gc->GCWrite(dict, newarray);
		}
	}
	--g->sp;
	return errNone;
}	



int prArray_AtIdentityHashInPairs(struct VMGlobals *g, int numArgsPushed);
int prArray_AtIdentityHashInPairs(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a, *b;
	unsigned int i;
	
	a = g->sp - 1;  // array
	b = g->sp;		// key
	
	i = arrayAtIdentityHashInPairs(a->uo, b);
	SetInt(a, i);
	return errNone;
}

int prIdentDict_At(struct VMGlobals *g, int numArgsPushed);
int prIdentDict_At(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a, *b;
	unsigned int index;
	int objClassIndex;
	
	a = g->sp - 1;  // dict
	b = g->sp;		// key
	

	PyrSlot *arraySlot = a->uo->slots + ivxIdentDict_array;
	
	if (!IsObj(arraySlot)) return errFailed;
	
	PyrObject *array = arraySlot->uo;
	
	if (!ISKINDOF(array, class_array_index, class_array_maxsubclassindex)) return errFailed;

	index = arrayAtIdentityHashInPairs(array, b);
	a->ucopy = array->slots[index + 1].ucopy;
	
	return errNone;
}

int prSymbol_envirGet(struct VMGlobals *g, int numArgsPushed);
int prSymbol_envirGet(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a;
	unsigned int index;
	int objClassIndex;
	
	a = g->sp;  // key
	
	PyrSlot* currentEnvironmentSlot = g->classvars[class_object->classIndex.ui].uo->slots + 1;
	PyrObject *dict = currentEnvironmentSlot->uo;

	if (!IsObj(currentEnvironmentSlot)) {
Bail:
		SetNil(a);
		return errNone;
	}

	if (!ISKINDOF(dict, class_identdict_index, class_identdict_maxsubclassindex)) goto Bail;

	PyrSlot *arraySlot = dict->slots + ivxIdentDict_array;
	
	if (!IsObj(arraySlot)) goto Bail;
	
	PyrObject *array = arraySlot->uo;
	
	if (!ISKINDOF(array, class_array_index, class_array_maxsubclassindex)) goto Bail;
	
	index = arrayAtIdentityHashInPairs(array, a);
	a->ucopy = array->slots[index + 1].ucopy;
	
	return errNone;
}


int prSymbol_envirPut(struct VMGlobals *g, int numArgsPushed);
int prSymbol_envirPut(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a, *b;
	unsigned int index;
	int objClassIndex;
	
	a = g->sp - 1;  // key
	b = g->sp;  // value
	
	PyrSlot* currentEnvironmentSlot = g->classvars[class_object->classIndex.ui].uo->slots + 1;
	PyrObject *dict = currentEnvironmentSlot->uo;

	if (!IsObj(currentEnvironmentSlot)) {
Bail:
		SetNil(a);
		return errNone;
	}

	if (!ISKINDOF(dict, class_identdict_index, class_identdict_maxsubclassindex)) goto Bail;
	
	int err = identDictPut(g, dict, a, b);
	if (err) return err;
	
	a->ucopy = b->ucopy;
	
	return errNone;
}

void identDictAt(PyrObject *dict, PyrSlot *inKey, PyrSlot *outValue);
void identDictAt(PyrObject *dict, PyrSlot *inKey, PyrSlot *outValue)
{
	PyrObject* array = dict->slots[ivxIdentDict_array].uo;
	//if (!ISKINDOF(array, class_array_index, class_array_maxsubclassindex)) return errFailed;
	
	int32 index = arrayAtIdentityHashInPairs(array, inKey);
	outValue->ucopy = array->slots[index + 1].ucopy;
}

int prEvent_Delta(struct VMGlobals *g, int numArgsPushed);
int prEvent_Delta(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a, key, dur, stretch;
	double fdur, fstretch;
	int err, index, objClassIndex;
	PyrObject *array;
	
	a = g->sp;  // dict
	
	array = a->uo->slots[ivxIdentDict_array].uo;
	if (!ISKINDOF(array, class_array_index, class_array_maxsubclassindex)) return errFailed;
	
	SetSymbol(&key, s_delta);
	index = arrayAtIdentityHashInPairs(array, &key);
	a->ucopy = array->slots[index + 1].ucopy;
	
	if (IsNil(a)) {
		SetSymbol(&key, s_dur);
		index = arrayAtIdentityHashInPairs(array, &key);
		dur.ucopy = array->slots[index + 1].ucopy;
		
		err = slotDoubleVal(&dur, &fdur);
		if (err) return err;
		
		SetSymbol(&key, s_stretch);
		index = arrayAtIdentityHashInPairs(array, &key);
		stretch.ucopy = array->slots[index + 1].ucopy;
		
		err = slotDoubleVal(&stretch, &fstretch);
		if (err) return err;
		
		a->uf = fdur * fstretch;
	}
	
	return errNone;
}

PyrObject* envirArray(PyrObject *dict);
PyrObject* envirArray(PyrObject *dict)
{
	PyrObject *array;
	int objClassIndex;
	
	if (ISKINDOF(dict, class_identdict_index, class_identdict_maxsubclassindex)) {
		PyrSlot *arraySlot = dict->slots + ivxIdentDict_array;
		if ((IsObj(arraySlot) && (array = arraySlot->uo)->classptr == class_array)) {
			return array;
		}
	}
	return NULL;
}

bool envirArrayAt(PyrObject *array, PyrSlot *key, PyrSlot *res);
bool envirArrayAt(PyrObject *array, PyrSlot *key, PyrSlot *res)
{
	PyrSlot *slot;
	int i;
	
	i = arrayAtIdentityHashInPairsWithHash(array, key, calcHash(key));
	if (i >= 0) {
		slot = array->slots + i;
		if (NotNil(slot)) {
			slot ++;
			res->ucopy = slot->ucopy;
			return true;
		}
	}
	return false;
}


bool getEnvirValue(VMGlobals *g, PyrSymbol *keyword, PyrSlot *value);
bool getEnvirValue(VMGlobals *g, PyrSymbol *keyword, PyrSlot *value)
{
	PyrObject* envirClassVars = g->classvars[class_object->classIndex.ui].uo;
	PyrSlot* curEnvirSlot = envirClassVars->slots + 1;
	if (isKindOfSlot(curEnvirSlot, class_identdict)) {
		int objClassIndex;
		PyrObject* array = curEnvirSlot->uo->slots[ivxIdentDict_array].uo;
		if (!ISKINDOF(array, class_array_index, class_array_maxsubclassindex)) goto fail;
		PyrSlot keyslot;
		SetSymbol(&keyslot, keyword);
		return envirArrayAt(array, &keyslot, value);
	} else {
		fail:
		value->ucopy = o_nil.ucopy;
		return false;
	}
}  

void PriorityQueueAdd(struct VMGlobals *g, PyrObject* queueobj, PyrSlot* item, double time);
void PriorityQueueAdd(struct VMGlobals *g, PyrObject* queueobj, PyrSlot* item, double time)
{
	PyrObject *schedq, *newschedq;
	int size, maxsize;

	PyrSlot *schedqSlot = queueobj->slots;
	if (!IsObj(schedqSlot)) {
		size = 16;
		schedq = newPyrArray(g->gc, size, 0, true);
		SetObject(schedqSlot, schedq);
		g->gc->GCWrite(queueobj, schedq);
	} else {
		schedq = schedqSlot->uo;
		maxsize = ARRAYMAXINDEXSIZE(schedq);
		size = schedq->size;
		if (size+2 > maxsize) {
			double *pslot, *qslot;

			newschedq = newPyrArray(g->gc, maxsize*2, 0, true);
			newschedq->size = size;
			
			pslot = (double*)schedq->slots - 1;
			qslot = (double*)newschedq->slots - 1;
			for (int i=0; i<size; ++i) *++qslot = *++pslot;
			
			SetObject(schedqSlot, newschedq);
			g->gc->GCWrite(queueobj, newschedq);
			
			schedq = newschedq;
		}
	}
	
	addheap(g, schedq, time, item);
}

int prPriorityQueueAdd(struct VMGlobals *g, int numArgsPushed);
int prPriorityQueueAdd(struct VMGlobals *g, int numArgsPushed)
{
	
	PyrSlot *a = g->sp - 2;	// priority queue
	PyrSlot *b = g->sp - 1;	// time
	PyrSlot *c = g->sp;		// item
	
	double time;
	int err = slotDoubleVal(b, &time);
	if (err) return errNone;	// nil is OK, nothing gets added
	
	PriorityQueueAdd(g, a->uo, c, time);
	return errNone;
}


void PriorityQueuePop(PyrObject *queueobj, PyrSlot *result);
void PriorityQueuePop(PyrObject *queueobj, PyrSlot *result)
{
	PyrSlot *schedqSlot = queueobj->slots;
	
	if (IsObj(schedqSlot)) {
		PyrObject *schedq = schedqSlot->uo;
		double time;
		if (!getheap(schedq, &time, result)) {
			SetNil(result);
		}
	} else {
		SetNil(result);
	}
}

void PriorityQueueTop(PyrObject *queueobj, PyrSlot *result);
void PriorityQueueTop(PyrObject *queueobj, PyrSlot *result)
{
	PyrSlot *schedqSlot = queueobj->slots;
	
	if (IsObj(schedqSlot)) {
		PyrObject *schedq = schedqSlot->uo;
		if (schedq->size > 0) {
			result->ucopy = schedq->slots[0].ucopy;
		} else {
			SetNil(result);
		}
	} else {
		SetNil(result);
	}
}

void PriorityQueueClear(PyrObject *queueobj);
void PriorityQueueClear(PyrObject *queueobj)
{
	PyrSlot *schedqSlot = queueobj->slots;
	
	if (IsObj(schedqSlot)) {
		PyrObject *schedq = schedqSlot->uo;
		schedq->size = 0;
	}
}

bool PriorityQueueEmpty(PyrObject *queueobj);
bool PriorityQueueEmpty(PyrObject *queueobj)
{
	PyrSlot *schedqSlot = queueobj->slots;
	
	if (IsObj(schedqSlot)) {
		PyrObject *schedq = schedqSlot->uo;
		if (schedq->size > 0) {
			return false;
		}
	}
	return true;
}

int prPriorityQueuePop(struct VMGlobals *g, int numArgsPushed);
int prPriorityQueuePop(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot* a = g->sp;	// priority queue
	
	PriorityQueuePop(a->uo, a);
	return errNone;
}

int prPriorityQueueTop(struct VMGlobals *g, int numArgsPushed);
int prPriorityQueueTop(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot* a = g->sp;	// priority queue
	
	PriorityQueueTop(a->uo, a);
	return errNone;
}

int prPriorityQueueClear(struct VMGlobals *g, int numArgsPushed);
int prPriorityQueueClear(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot* a = g->sp;	// priority queue
	
	PriorityQueueClear(a->uo);
	return errNone;
}

int prPriorityQueueEmpty(struct VMGlobals *g, int numArgsPushed);
int prPriorityQueueEmpty(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a;
	
	a = g->sp;	// priority queue
	
	if (PriorityQueueEmpty(a->uo)) {
		SetTrue(a);
	} else {
		SetFalse(a);
	}
	return errNone;
}

void PriorityQueuePostpone(PyrObject* queueobj, double time);
void PriorityQueuePostpone(PyrObject* queueobj, double time)
{
	PyrSlot *schedqSlot = queueobj->slots;
	
	if (IsObj(schedqSlot)) {
		PyrObject *schedq = schedqSlot->uo;
		PyrSlot* slots = schedq->slots;
		for (int i=0; i < schedq->size; i+=2) {
			slots[i].uf += time;
		}
	}
}

int prPriorityQueuePostpone(struct VMGlobals *g, int numArgsPushed);
int prPriorityQueuePostpone(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a = g->sp - 1;	// priority queue
	PyrSlot *b = g->sp;		// time
	
	double time;
	int err = slotDoubleVal(b, &time);
	if (err) return err;
	
	PyrObject *queueobj = a->uo;
	PriorityQueuePostpone(queueobj, time);
	return errNone;
}


void initListPrimitives();
void initListPrimitives()
{
	int base, index;
		
	base = nextPrimitiveIndex();
	index = 0;
	definePrimitive(base, index++, "_Array_AtIdentityHash", prArray_AtIdentityHash, 2, 0);
	definePrimitive(base, index++, "_Array_AtIdentityHashInPairs", prArray_AtIdentityHashInPairs, 2, 0);
	definePrimitive(base, index++, "_IdentDict_Put", prIdentDict_Put, 3, 0);
	definePrimitive(base, index++, "_IdentDict_PutGet", prIdentDict_PutGet, 3, 0);
	definePrimitive(base, index++, "_IdentDict_At", prIdentDict_At, 2, 0);
	definePrimitive(base, index++, "_Symbol_envirGet", prSymbol_envirGet, 1, 0);
	definePrimitive(base, index++, "_Symbol_envirPut", prSymbol_envirPut, 2, 0);
	definePrimitive(base, index++, "_ArrayMultiChannelExpand", prArrayMultiChanExpand, 1, 0);

	definePrimitive(base, index++, "_PriorityQueueAdd", prPriorityQueueAdd, 3, 0);
	definePrimitive(base, index++, "_PriorityQueuePop", prPriorityQueuePop, 1, 0);
	definePrimitive(base, index++, "_PriorityQueueTop", prPriorityQueueTop, 1, 0);
	definePrimitive(base, index++, "_PriorityQueueClear", prPriorityQueueClear, 1, 0);
	definePrimitive(base, index++, "_PriorityQueueEmpty", prPriorityQueueEmpty, 1, 0);
	definePrimitive(base, index++, "_PriorityQueuePostpone", prPriorityQueueEmpty, 2, 0);
	
	definePrimitive(base, index++, "_Event_Delta", prEvent_Delta, 1, 0);
}

void initPatterns();
void initPatterns()
{
	PyrSymbol *sym;
	
	ivxIdentDict_array = instVarOffset("IdentityDictionary", "array");
	ivxIdentDict_size  = instVarOffset("IdentityDictionary", "size");
	
	sym = getsym("IdentityDictionary");
	class_identdict = sym ? sym->u.classobj : NULL;
	class_identdict_index = class_identdict->classIndex.ui;
	class_identdict_maxsubclassindex = class_identdict->maxSubclassIndex.ui;

	class_array_index = class_array->classIndex.ui;
	class_array_maxsubclassindex = class_array->maxSubclassIndex.ui;
	
	s_delta = getsym("delta");
	s_dur = getsym("dur");
	s_stretch = getsym("stretch");

}
