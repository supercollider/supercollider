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

#include "PyrMessage.h"
#include "PyrPrimitiveProto.h"
#include "PyrInterpreter.h"
#include "PyrPrimitive.h"
#include "GC.h"
#include "bullet.h"
#include <stdlib.h>
#ifndef NDEBUG
# define NDEBUG
#endif
#include <assert.h>
#include "PredefinedSymbols.h"

#define DEBUGMETHODS 0
#define SANITYCHECK 0
#define METHODMETER 0

PyrMethod **gRowTable;

PyrSlot keywordstack[MAXKEYSLOTS];
bool gKeywordError = true;
extern bool gTraceInterpreter;

long cvxUniqueMethods;
extern long ivxIdentDict_array;

void StoreToImmutable(VMGlobals *g);

void CallStackSanity(VMGlobals *g, char *tagstr);

void initUniqueMethods()
{
	PyrClass *dummyclass;
	cvxUniqueMethods = classVarOffset("Object", "uniqueMethods", &dummyclass);
}

void sendMessageWithKeys(VMGlobals *g, PyrSymbol *selector, long numArgsPushed, long numKeyArgsPushed)
{
	PyrMethod *meth = NULL;
	PyrMethodRaw *methraw;
	PyrSlot *recvrSlot, *sp;
	PyrClass *classobj;
	long index, classIndex;
	PyrObject *obj;
		
	//postfl("->sendMessage\n");
#if SANITYCHECK
	g->gc->SanityCheck();
	CallStackSanity(g, "sendMessageWithKeys");
#endif
	recvrSlot = g->sp - numArgsPushed + 1;
	
	classobj = classOfSlot(recvrSlot);
	
	lookup_again:	
	index = classobj->classIndex.ui + selector->u.index;
	meth = gRowTable[index];
	
	if (meth->name.us != selector) {
		doesNotUnderstandWithKeys(g, selector, numArgsPushed, numKeyArgsPushed);
	} else {
		methraw = METHRAW(meth);
		//postfl("methraw->methType %d\n", methraw->methType);
		switch (methraw->methType) {
			case methNormal : /* normal msg send */
				executeMethodWithKeys(g, meth, numArgsPushed, numKeyArgsPushed);
				break;
			case methReturnSelf : /* return self */
				g->sp -= numArgsPushed - 1;
				break;
			case methReturnLiteral : /* return literal */
				sp = g->sp -= numArgsPushed - 1;
				sp[0].ucopy = meth->selectors.ucopy; /* in this case selectors is just a single value */
				break;
			case methReturnArg : /* return an argument */
				sp = g->sp -= numArgsPushed - 1;
				index = methraw->specialIndex; // zero is index of the first argument
				if (index < numArgsPushed) {
					sp[0].ucopy = sp[index].ucopy;
				} else {
					sp[0].ucopy = meth->prototypeFrame.uo->slots[index].ucopy;
				}
				break;
			case methReturnInstVar : /* return inst var */
				sp = g->sp -= numArgsPushed - 1;
				index = methraw->specialIndex;
				sp[0].ucopy = recvrSlot->uo->slots[index].ucopy;
				break;
			case methAssignInstVar : /* assign inst var */
				sp = g->sp -= numArgsPushed - 1;
				index = methraw->specialIndex;
				obj = recvrSlot->uo;
				if (obj->obj_flags & obj_immutable) { StoreToImmutable(g); return; }
				if (numArgsPushed >= 2) {
					obj->slots[index].ucopy = sp[1].ucopy;
					g->gc->GCWrite(obj, sp + 1);
				} else {
					SetNil(&obj->slots[index]);
				}
				sp[0].ucopy = recvrSlot->ucopy;
				break;
			case methReturnClassVar : /* return class var */
				sp = g->sp -= numArgsPushed - 1;
				index = methraw->specialIndex;
				classIndex = meth->selectors.us->u.classobj->classIndex.ui;
				sp[0].ucopy = g->classvars[classIndex].uo->slots[index].ucopy;
				break;
			case methAssignClassVar : /* assign class var */
				sp = g->sp -= numArgsPushed - 1;
				index = methraw->specialIndex;
				classIndex = meth->selectors.us->u.classobj->classIndex.ui;
				obj = g->classvars[classIndex].uo;
				if (numArgsPushed >= 2) {
					obj->slots[index].ucopy = sp[1].ucopy;
					g->gc->GCWrite(obj, sp + 1);
				} else {
					SetNil(&obj->slots[index]);
				}
				sp[0].ucopy = recvrSlot->ucopy;
				break;
			case methRedirect : /* send a different selector to self, e.g. this.subclassResponsibility */
				if (numArgsPushed < methraw->numargs) { // not enough args pushed
					/* push default arg values */
					double *pslot, *qslot;
					long m, mmax;
					pslot = (double*)g->sp;
					qslot = (double*)(meth->prototypeFrame.uo->slots + numArgsPushed - 1);
					for (m=0, mmax=methraw->numargs - numArgsPushed; m<mmax; ++m) *++pslot = *++qslot;
					numArgsPushed = methraw->numargs;
					g->sp += mmax;
				}
				selector = meth->selectors.us;
				goto lookup_again;
			case methForward : /* forward to an instance variable */
				if (numArgsPushed < methraw->numargs) { // not enough args pushed
					/* push default arg values */
					double *pslot, *qslot;
					long m, mmax;
					pslot = (double*)g->sp;
					qslot = (double*)(meth->prototypeFrame.uo->slots + numArgsPushed - 1);
					for (m=0, mmax=methraw->numargs - numArgsPushed; m<mmax; ++m) *++pslot = *++qslot;
					numArgsPushed = methraw->numargs;
					g->sp += mmax;
				}
				selector = meth->selectors.us;
				index = methraw->specialIndex;
				recvrSlot->ucopy = recvrSlot->uo->slots[index].ucopy;
							
				classobj = classOfSlot(recvrSlot);
				
				goto lookup_again;
			case methPrimitive : /* primitive */
				doPrimitiveWithKeys(g, meth, numArgsPushed, numKeyArgsPushed);
#if SANITYCHECK
	g->gc->SanityCheck();
#endif
				break;
		}
	}
#if SANITYCHECK
	g->gc->SanityCheck();
	CallStackSanity(g, "<sendMessageWithKeys");
#endif
	//postfl("<-sendMessage\n");
}


void sendMessage(VMGlobals *g, PyrSymbol *selector, long numArgsPushed)
{
	PyrMethod *meth = NULL;
	PyrMethodRaw *methraw;
	PyrSlot *recvrSlot, *sp;
	PyrClass *classobj;
	long index, classIndex;
	PyrObject *obj;
		
	//postfl("->sendMessage\n");
#if SANITYCHECK
	g->gc->SanityCheck();
	CallStackSanity(g, "sendMessage");
#endif
	recvrSlot = g->sp - numArgsPushed + 1;
	
	classobj = classOfSlot(recvrSlot);
	
	lookup_again:	
	index = classobj->classIndex.ui + selector->u.index;
	meth = gRowTable[index];
	
	if (meth->name.us != selector) {
		doesNotUnderstand(g, selector, numArgsPushed);
	} else {
		methraw = METHRAW(meth);
		//postfl("methraw->methType %d\n", methraw->methType);
		switch (methraw->methType) {
			case methNormal : /* normal msg send */
				executeMethod(g, meth, numArgsPushed);
				break;
			case methReturnSelf : /* return self */
				g->sp -= numArgsPushed - 1;
				break;
			case methReturnLiteral : /* return literal */
				sp = g->sp -= numArgsPushed - 1;
				sp[0].ucopy = meth->selectors.ucopy; /* in this case selectors is just a single value */
				break;
			case methReturnArg : /* return an argument */
				sp = g->sp -= numArgsPushed - 1;
				index = methraw->specialIndex; // zero is index of the first argument
				if (index < numArgsPushed) {
					sp[0].ucopy = sp[index].ucopy;
				} else {
					sp[0].ucopy = meth->prototypeFrame.uo->slots[index].ucopy;
				}
				break;
			case methReturnInstVar : /* return inst var */
				sp = g->sp -= numArgsPushed - 1;
				index = methraw->specialIndex;
				sp[0].ucopy = recvrSlot->uo->slots[index].ucopy;
				break;
			case methAssignInstVar : /* assign inst var */
				sp = g->sp -= numArgsPushed - 1;
				index = methraw->specialIndex;
				obj = recvrSlot->uo;
				if (obj->obj_flags & obj_immutable) { StoreToImmutable(g); return; }
				if (numArgsPushed >= 2) {
					obj->slots[index].ucopy = sp[1].ucopy;
					g->gc->GCWrite(obj, sp + 1);
				} else {
					SetNil(&obj->slots[index]);
				}
				sp[0].ucopy = recvrSlot->ucopy;
				break;
			case methReturnClassVar : /* return class var */
				sp = g->sp -= numArgsPushed - 1;
				index = methraw->specialIndex;
				classIndex = meth->selectors.us->u.classobj->classIndex.ui;
				sp[0].ucopy = g->classvars[classIndex].uo->slots[index].ucopy;
				break;
			case methAssignClassVar : /* assign class var */
				sp = g->sp -= numArgsPushed - 1;
				index = methraw->specialIndex;
				classIndex = meth->selectors.us->u.classobj->classIndex.ui;
				obj = g->classvars[classIndex].uo;
				if (numArgsPushed >= 2) {
					obj->slots[index].ucopy = sp[1].ucopy;
					g->gc->GCWrite(obj, sp + 1);
				} else {
					SetNil(&obj->slots[index]);
				}
				sp[0].ucopy = recvrSlot->ucopy;
				break;
			case methRedirect : /* send a different selector to self, e.g. this.subclassResponsibility */
				selector = meth->selectors.us;
				goto lookup_again;
			case methForward : /* forward to an instance variable */
				selector = meth->selectors.us;
				index = methraw->specialIndex;
				recvrSlot->ucopy = recvrSlot->uo->slots[index].ucopy;
				
							
				classobj = classOfSlot(recvrSlot);
				
				goto lookup_again;
			case methPrimitive : /* primitive */
				doPrimitive(g, meth, numArgsPushed);
#if SANITYCHECK
	g->gc->SanityCheck();
#endif
				break;
			/*
			case methMultDispatchByClass : {
				index = methraw->specialIndex;
				if (index < numArgsPushed) {
					classobj = sp[index].uo->classptr;
					selector = meth->selectors.us;
					goto lookup_again;
				} else {
					doesNotUnderstand(g, selector, numArgsPushed);
				}
			} break;
			case methMultDispatchByValue : {
				index = methraw->specialIndex;
				if (index < numArgsPushed) {
					index = arrayAtIdentityHashInPairs(array, b);
					meth = meth->selectors.uo->slots[index + 1].uom;
					goto meth_select_again;
				} else {
					doesNotUnderstand(g, selector, numArgsPushed);
				}
			} break;
			*/
				
		}
	}
#if SANITYCHECK
	g->gc->SanityCheck();
	CallStackSanity(g, "<sendMessage");
#endif
	//postfl("<-sendMessage\n");
}


void sendSuperMessageWithKeys(VMGlobals *g, PyrSymbol *selector, long numArgsPushed, long numKeyArgsPushed)
{
	PyrMethod *meth = NULL;
	PyrMethodRaw *methraw;
	PyrSlot *recvrSlot, *sp;
	PyrClass *classobj;
	long index, classIndex;
	PyrObject *obj;
		
	//postfl("->sendMessage\n");
#if SANITYCHECK
	g->gc->SanityCheck();
	CallStackSanity(g, "sendSuperMessageWithKeys");
#endif
	recvrSlot = g->sp - numArgsPushed + 1;
	
	classobj = g->method->ownerclass.uoc->superclass.us->u.classobj;
	//assert(isKindOfSlot(recvrSlot, classobj));

	lookup_again:	
	index = classobj->classIndex.ui + selector->u.index;
	meth = gRowTable[index];
	
	if (meth->name.us != selector) {
		doesNotUnderstandWithKeys(g, selector, numArgsPushed, numKeyArgsPushed);
	} else {
		methraw = METHRAW(meth);
		//postfl("methraw->methType %d\n", methraw->methType);
		switch (methraw->methType) {
			case methNormal : /* normal msg send */
				executeMethodWithKeys(g, meth, numArgsPushed, numKeyArgsPushed);
				break;
			case methReturnSelf : /* return self */
				g->sp -= numArgsPushed - 1;
				break;
			case methReturnLiteral : /* return literal */
				sp = g->sp -= numArgsPushed - 1;
				sp[0].ucopy = meth->selectors.ucopy; /* in this case selectors is just a single value */
				break;
			case methReturnArg : /* return an argument */
				sp = g->sp -= numArgsPushed - 1;
				index = methraw->specialIndex; // zero is index of the first argument
				if (index < numArgsPushed) {
					sp[0].ucopy = sp[index].ucopy;
				} else {
					sp[0].ucopy = meth->prototypeFrame.uo->slots[index].ucopy;
				}
				break;
			case methReturnInstVar : /* return inst var */
				sp = g->sp -= numArgsPushed - 1;
				index = methraw->specialIndex;
				sp[0].ucopy = recvrSlot->uo->slots[index].ucopy;
				break;
			case methAssignInstVar : /* assign inst var */
				sp = g->sp -= numArgsPushed - 1;
				index = methraw->specialIndex;
				obj = recvrSlot->uo;
				if (obj->obj_flags & obj_immutable) { StoreToImmutable(g); return; }
				if (numArgsPushed >= 2) {
					obj->slots[index].ucopy = sp[1].ucopy;
					g->gc->GCWrite(obj, sp + 1);
				} else {
					SetNil(&obj->slots[index]);
				}
				sp[0].ucopy = recvrSlot->ucopy;
				break;
			case methReturnClassVar : /* return class var */
				sp = g->sp -= numArgsPushed - 1;
				index = methraw->specialIndex;
				classIndex = meth->selectors.us->u.classobj->classIndex.ui;
				sp[0].ucopy = g->classvars[classIndex].uo->slots[index].ucopy;
				break;
			case methAssignClassVar : /* assign class var */
				sp = g->sp -= numArgsPushed - 1;
				index = methraw->specialIndex;
				classIndex = meth->selectors.us->u.classobj->classIndex.ui;
				obj = g->classvars[classIndex].uo;
				if (numArgsPushed >= 2) {
					obj->slots[index].ucopy = sp[1].ucopy;
					g->gc->GCWrite(obj, sp + 1);
				} else {
					SetNil(&obj->slots[index]);
				}
				sp[0].ucopy = recvrSlot->ucopy;
				break;
			case methRedirect : /* send a different selector to self, e.g. this.subclassResponsibility */
				if (numArgsPushed < methraw->numargs) { // not enough args pushed
					/* push default arg values */
					double *pslot, *qslot;
					long m, mmax;
					pslot = (double*)g->sp;
					qslot = (double*)(meth->prototypeFrame.uo->slots + numArgsPushed - 1);
					for (m=0, mmax=methraw->numargs - numArgsPushed; m<mmax; ++m) *++pslot = *++qslot;
					numArgsPushed = methraw->numargs;
					g->sp += mmax;
				}
				selector = meth->selectors.us;
				goto lookup_again;
			case methForward : /* forward to an instance variable */
				if (numArgsPushed < methraw->numargs) { // not enough args pushed
					/* push default arg values */
					double *pslot, *qslot;
					long m, mmax;
					pslot = (double*)g->sp;
					qslot = (double*)(meth->prototypeFrame.uo->slots + numArgsPushed - 1);
					for (m=0, mmax=methraw->numargs - numArgsPushed; m<mmax; ++m) *++pslot = *++qslot;
					numArgsPushed = methraw->numargs;
					g->sp += mmax;
				}
				selector = meth->selectors.us;
				index = methraw->specialIndex;
				recvrSlot->ucopy = recvrSlot->uo->slots[index].ucopy;
							
				classobj = classOfSlot(recvrSlot);
				
				goto lookup_again;
			case methPrimitive : /* primitive */
				doPrimitiveWithKeys(g, meth, numArgsPushed, numKeyArgsPushed);
#if SANITYCHECK
	g->gc->SanityCheck();
#endif
				break;
		}
	}
#if SANITYCHECK
	g->gc->SanityCheck();
	CallStackSanity(g, "<sendSuperMessageWithKeys");
#endif
	//postfl("<-sendMessage\n");
}


void sendSuperMessage(VMGlobals *g, PyrSymbol *selector, long numArgsPushed)
{
	PyrMethod *meth = NULL;
	PyrMethodRaw *methraw;
	PyrSlot *recvrSlot, *sp;
	PyrClass *classobj;
	long index, classIndex;
	PyrObject *obj;
		
	//postfl("->sendMessage\n");
#if SANITYCHECK
	g->gc->SanityCheck();
	CallStackSanity(g, "sendSuperMessage");
#endif
	recvrSlot = g->sp - numArgsPushed + 1;
	
	classobj = g->method->ownerclass.uoc->superclass.us->u.classobj;
	//assert(isKindOfSlot(recvrSlot, classobj));
	
	lookup_again:	
	index = classobj->classIndex.ui + selector->u.index;
	meth = gRowTable[index];
	
	if (meth->name.us != selector) {
		doesNotUnderstand(g, selector, numArgsPushed);
	} else {
		methraw = METHRAW(meth);
		//postfl("methraw->methType %d\n", methraw->methType);
		switch (methraw->methType) {
			case methNormal : /* normal msg send */
				executeMethod(g, meth, numArgsPushed);
				break;
			case methReturnSelf : /* return self */
				g->sp -= numArgsPushed - 1;
				break;
			case methReturnLiteral : /* return literal */
				sp = g->sp -= numArgsPushed - 1;
				sp[0].ucopy = meth->selectors.ucopy; /* in this case selectors is just a single value */
				break;
			case methReturnArg : /* return an argument */
				sp = g->sp -= numArgsPushed - 1;
				index = methraw->specialIndex; // zero is index of the first argument
				if (index < numArgsPushed) {
					sp[0].ucopy = sp[index].ucopy;
				} else {
					sp[0].ucopy = meth->prototypeFrame.uo->slots[index].ucopy;
				}
				break;
			case methReturnInstVar : /* return inst var */
				sp = g->sp -= numArgsPushed - 1;
				index = methraw->specialIndex;
				sp[0].ucopy = recvrSlot->uo->slots[index].ucopy;
				break;
			case methAssignInstVar : /* assign inst var */
				sp = g->sp -= numArgsPushed - 1;
				index = methraw->specialIndex;
				obj = recvrSlot->uo;
				if (obj->obj_flags & obj_immutable) { StoreToImmutable(g); return; }
				if (numArgsPushed >= 2) {
					obj->slots[index].ucopy = sp[1].ucopy;
					g->gc->GCWrite(obj, sp + 1);
				} else {
					SetNil(&obj->slots[index]);
				}
				sp[0].ucopy = recvrSlot->ucopy;
				break;
			case methReturnClassVar : /* return class var */
				sp = g->sp -= numArgsPushed - 1;
				index = methraw->specialIndex;
				classIndex = meth->selectors.us->u.classobj->classIndex.ui;
				sp[0].ucopy = g->classvars[classIndex].uo->slots[index].ucopy;
				break;
			case methAssignClassVar : /* assign class var */
				sp = g->sp -= numArgsPushed - 1;
				index = methraw->specialIndex;
				classIndex = meth->selectors.us->u.classobj->classIndex.ui;
				obj = g->classvars[classIndex].uo;
				if (numArgsPushed >= 2) {
					obj->slots[index].ucopy = sp[1].ucopy;
					g->gc->GCWrite(obj, sp + 1);
				} else {
					SetNil(&obj->slots[index]);
				}
				sp[0].ucopy = recvrSlot->ucopy;
				break;
			case methRedirect : /* send a different selector to self, e.g. this.subclassResponsibility */
				selector = meth->selectors.us;
				goto lookup_again;
			case methForward : /* forward to an instance variable */
				selector = meth->selectors.us;
				index = methraw->specialIndex;
				recvrSlot->ucopy = recvrSlot->uo->slots[index].ucopy;
				
							
				classobj = classOfSlot(recvrSlot);
				
				goto lookup_again;
			case methPrimitive : /* primitive */
				doPrimitive(g, meth, numArgsPushed);
#if SANITYCHECK
	g->gc->SanityCheck();
#endif
				break;
			/*
			case methMultDispatchByClass : {
				index = methraw->specialIndex;
				if (index < numArgsPushed) {
					classobj = sp[index].uo->classptr;
					selector = meth->selectors.us;
					goto lookup_again;
				} else {
					doesNotUnderstand(g, selector, numArgsPushed);
				}
			} break;
			case methMultDispatchByValue : {
				index = methraw->specialIndex;
				if (index < numArgsPushed) {
					index = arrayAtIdentityHashInPairs(array, b);
					meth = meth->selectors.uo->slots[index + 1].uom;
					goto meth_select_again;
				} else {
					doesNotUnderstand(g, selector, numArgsPushed);
				}
			} break;
			*/
				
		}
	}
#if SANITYCHECK
	g->gc->SanityCheck();
	CallStackSanity(g, "<sendSuperMessage");
#endif
	//postfl("<-sendMessage\n");
}

long arrayAtIdentityHashInPairs(PyrObject *array, PyrSlot *key);

extern PyrClass *class_identdict;
void doesNotUnderstandWithKeys(VMGlobals *g, PyrSymbol *selector, 
	long numArgsPushed, long numKeyArgsPushed)
{
	double *qslot, *pslot, *pend;
	long i, index;
	PyrSlot *uniqueMethodSlot, *arraySlot, *recvrSlot, *selSlot, *slot;
	PyrClass *classobj;
	PyrMethod *meth;
	PyrObject *array;

#if SANITYCHECK
	g->gc->SanityCheck();
#endif
	// move args up by one to make room for selector
	qslot = (double*)(g->sp + 1);
	pslot = (double*)(g->sp + 2);
	pend = pslot - numArgsPushed + 1;
	while (pslot > pend) *--pslot = *--qslot;

	selSlot = g->sp - numArgsPushed + 2;
	SetSymbol(selSlot, selector);	
	g->sp++;

	recvrSlot = selSlot - 1;
	
	classobj = classOfSlot(recvrSlot);

	index = classobj->classIndex.ui + s_nocomprendo->u.index;
	meth = gRowTable[index];


	if (meth->ownerclass.uoc == class_object) {
		// lookup instance specific method
		uniqueMethodSlot = g->classvars[class_object->classIndex.ui].uo->slots + cvxUniqueMethods;
		if (isKindOfSlot(uniqueMethodSlot, class_identdict)) {
			arraySlot = uniqueMethodSlot->uo->slots + ivxIdentDict_array;
			if ((IsObj(arraySlot) && (array = arraySlot->uo)->classptr == class_array)) {
				i = arrayAtIdentityHashInPairs(array, recvrSlot);
				if (i >= 0) {
					slot = array->slots + i;
					if (NotNil(slot)) {
						++slot;
						if (isKindOfSlot(slot, class_identdict)) {
							arraySlot = slot->uo->slots + ivxIdentDict_array;
							if ((IsObj(arraySlot) && (array = arraySlot->uo)->classptr == class_array)) {
								i = arrayAtIdentityHashInPairs(array, selSlot);
								if (i >= 0) {
									slot = array->slots + i;
									if (NotNil(slot)) {
										++slot;
										selSlot->ucopy = recvrSlot->ucopy; 
										recvrSlot->ucopy = slot->ucopy;
										blockValueWithKeys(g, numArgsPushed+1, numKeyArgsPushed);
										return;
									}
								}
							}
						}
					}
				}
			}
		}
	}

	executeMethodWithKeys(g, meth, numArgsPushed+1, numKeyArgsPushed);
	
#if SANITYCHECK
	g->gc->SanityCheck();
#endif
}

int blockValue(struct VMGlobals *g, int numArgsPushed);

void doesNotUnderstand(VMGlobals *g, PyrSymbol *selector, 
	long numArgsPushed)
{
	double *qslot, *pslot, *pend;
	long i, index;
	PyrSlot *uniqueMethodSlot, *arraySlot, *recvrSlot, *selSlot, *slot;
	PyrClass *classobj;
	PyrMethod *meth;
	PyrObject *array;

#if SANITYCHECK
	g->gc->SanityCheck();
#endif
	// move args up by one to make room for selector
	qslot = (double*)(g->sp + 1);
	pslot = (double*)(g->sp + 2);
	pend = pslot - numArgsPushed + 1;
	while (pslot > pend) *--pslot = *--qslot;

	selSlot = g->sp - numArgsPushed + 2;
	SetSymbol(selSlot, selector);	
	g->sp++;

	recvrSlot = selSlot - 1;
	
	classobj = classOfSlot(recvrSlot);

	index = classobj->classIndex.ui + s_nocomprendo->u.index;
	meth = gRowTable[index];

	
	if (meth->ownerclass.uoc == class_object) {
		// lookup instance specific method
		uniqueMethodSlot = g->classvars[class_object->classIndex.ui].uo->slots + cvxUniqueMethods;
		if (isKindOfSlot(uniqueMethodSlot, class_identdict)) {
			arraySlot = uniqueMethodSlot->uo->slots + ivxIdentDict_array;
			if ((IsObj(arraySlot) && (array = arraySlot->uo)->classptr == class_array)) {
				i = arrayAtIdentityHashInPairs(array, recvrSlot);
				if (i >= 0) {
					slot = array->slots + i;
					if (NotNil(slot)) {
						++slot;
						if (isKindOfSlot(slot, class_identdict)) {
							arraySlot = slot->uo->slots + ivxIdentDict_array;
							if ((IsObj(arraySlot) && (array = arraySlot->uo)->classptr == class_array)) {
								i = arrayAtIdentityHashInPairs(array, selSlot);
								if (i >= 0) {
									slot = array->slots + i;
									if (NotNil(slot)) {
										++slot;
										selSlot->ucopy = recvrSlot->ucopy; 
										recvrSlot->ucopy = slot->ucopy;
										blockValue(g, numArgsPushed+1);
										return;
									}
								}
							}
						}
					}
				}
			}
		}
	}

	executeMethod(g, meth, numArgsPushed+1);
	
#if SANITYCHECK
	g->gc->SanityCheck();
#endif
}


void executeMethodWithKeys(VMGlobals *g, PyrMethod *meth, long allArgsPushed, long numKeyArgsPushed)
{
	PyrMethodRaw *methraw;
	PyrFrame *frame;
	PyrFrame *caller;
	double *pslot, *qslot;
	PyrSlot *rslot;
	PyrSlot *vars;
	PyrObject *frameobj;
	PyrObject *proto;
	long i, j, m, mmax, numtemps, numargs, numArgsPushed;
		
#if SANITYCHECK
	g->gc->SanityCheck();
	CallStackSanity(g, "executeMethodWithKeys");
#endif
#if DEBUGMETHODS
	if (gTraceInterpreter) {
		if (g->method) {
			postfl(" %s-%s -> %s-%s\n", 
				g->method->ownerclass.uoc->name.us->name, g->method->name.us->name,
				meth->ownerclass.uoc->name.us->name, meth->name.us->name);
		} else {
			postfl(" top -> %s-%s\n", 
				meth->ownerclass.uoc->name.us->name, meth->name.us->name);
		}
	}
#endif
#if METHODMETER
	if (gTraceInterpreter) {
		meth->callMeter.ui++;
	}
#endif
	g->execMethod = 10;
	
	proto = meth->prototypeFrame.uo;
	methraw = METHRAW(meth);
	numtemps = methraw->numtemps;
	numargs = methraw->numargs;
	caller = g->frame;
	numArgsPushed = allArgsPushed - (numKeyArgsPushed<<1);
	//DumpStack(g, g->sp);
	//postfl("executeMethod allArgsPushed %d numKeyArgsPushed %d\n", allArgsPushed, numKeyArgsPushed);
	
	if (methraw->needsHeapContext) {
		frameobj = g->gc->New(methraw->frameSize, 0, obj_slot, true);
		vars = frameobj->slots - 1;
		frame = (PyrFrame*)(vars + numtemps);
		frameobj->classptr = class_frame;
		frameobj->size = FRAMESIZE + proto->size; /// <- IS THIS WRONG ??
		SetObject(&frame->myself, frameobj);
		SetObject(&frame->method, meth);
		SetFrame(&frame->homeContext, frame);
		SetFrame(&frame->context, frame);
		//***SetNil(&frame->context);

		if (caller) {
			SetInt(&caller->ip, (long)g->ip);
			SetFrame(&frame->caller, caller);
		} else {
			SetNil(&frame->caller);
		}
		SetInt(&frame->ip,  0);
		g->method = meth;
		
		g->ip = meth->code.uob->b - 1;
		g->frame = frame;
		g->block = (PyrBlock*)meth;
			
		g->sp -= allArgsPushed;
		qslot = (double*)(g->sp);
		pslot = (double*)(vars);
		
		if (numArgsPushed <= numargs) {	/* not enough args pushed */
			/* push all args to frame */
			for (m=0,mmax=numArgsPushed; m<mmax; ++m) *++pslot = *++qslot;
			
			/* push default arg & var values */
			pslot = (double*)(vars + numArgsPushed);
			qslot = (double*)(proto->slots + numArgsPushed - 1);
			for (m=0, mmax=numtemps - numArgsPushed; m<mmax; ++m) *++pslot = *++qslot;
		} else if (methraw->varargs) {
			PyrObject *list;
			double *lslot;
						
			/* push all normal args to frame */
			for (m=0,mmax=numargs; m<mmax; ++m) *++pslot = *++qslot;
			
			/* push list */
			i = numArgsPushed - numargs;
			list = newPyrArray(g->gc, i, 0, false);
			list->size = i;
			
			rslot = (PyrSlot*)pslot+1;
			SetObject(rslot, list);
			//SetObject(vars + numargs + 1, list);
			
			/* put extra args into list */
			lslot = (double*)(list->slots - 1);
			// fixed and raw sizes are zero
			for (m=0,mmax=i; m<mmax; ++m) *++lslot = *++qslot;
			
			if (methraw->numvars) {
				/* push default keyword and var values */
				pslot = (double*)(vars + numargs + 1);
				qslot = (double*)(proto->slots + numargs);
				for (m=0,mmax=methraw->numvars; m<mmax; ++m) *++pslot = *++qslot;
			}
		} else {
			/* push all args to frame */
			for (m=0,mmax=numargs; m<mmax; ++m) *++pslot = *++qslot;
			
			if (methraw->numvars) {
				/* push default keyword and var values */
				pslot = (double*)(vars + numargs);
				qslot = (double*)(proto->slots + numargs - 1);
				for (m=0,mmax=methraw->numvars; m<mmax; ++m) *++pslot = *++qslot;
			}
		}
		// do keyword lookup:
		if (numKeyArgsPushed && methraw->posargs) {
			PyrSymbol **name0, **name;
			PyrSlot *key;
			name0 = meth->argNames.uosym->symbols + 1;
			key = g->sp + numArgsPushed + 1;
			for (i=0; i<numKeyArgsPushed; ++i, key+=2) {
				name = name0;
				for (j=1; j<methraw->posargs; ++j, ++name) {
					if (*name == key->us) {
						vars[j+1].ucopy = key[1].ucopy;
						goto found1;
					}
				}
				if (gKeywordError) {
					post("WARNING: keyword arg '%s' not found in call to %s-%s\n",
						key->us->name, meth->ownerclass.uoc->name.us->name, meth->name.us->name);
				}
				found1: ;
			}
		}
	} else {  // context frame on the stack
		vars = g->sp - allArgsPushed;
		frame = (PyrFrame*)(vars + numtemps);
		
		g->method = meth;
		g->frame = frame;
		g->block = (PyrBlock*)meth;
		
		if (numKeyArgsPushed) {
			// evacuate keyword args to separate area
			pslot = (double*)(keywordstack + (numKeyArgsPushed<<1));
			qslot = (double*)(g->sp + 1);
			for (m=0; m<numKeyArgsPushed; ++m) {
				*--pslot = *--qslot;
				*--pslot = *--qslot;
			}
		}

		g->sp += FRAMESIZE + numtemps - allArgsPushed; // make space for context frame

		if (numArgsPushed <= numargs) {	/* not enough args pushed */
			/* push default arg values */
			pslot = (double*)(vars + numArgsPushed);
			qslot = (double*)(proto->slots + numArgsPushed - 1);
			for (m=0,mmax=numtemps - numArgsPushed; m<mmax; ++m) *++pslot = *++qslot;
		} else if (methraw->varargs) {
			PyrObject *list;
			double *lslot;
			
			/* push list */
			i = numArgsPushed - numargs;
			list = newPyrArray(g->gc, i, 0, false);
			list->size = i;
			
			/* put extra args into list */
			qslot = (double*)(vars + numargs);
			lslot = (double*)(list->slots - 1);
			// fixed and raw sizes are zero
			for (m=0,mmax=i; m<mmax; ++m) *++lslot = *++qslot;
			
			SetObject(vars + numargs + 1, list);
			
			if (methraw->numvars) {
				/* push default keyword and var values */
				pslot = (double*)(vars + numargs + 1);
				qslot = (double*)(proto->slots + numargs);
				for (m=0,mmax=methraw->numvars; m<mmax; ++m) *++pslot = *++qslot;
			}
		} else {
			if (methraw->numvars) {
				pslot = (double*)(vars + numargs);
				qslot = (double*)(proto->slots + numargs - 1);
				for (m=0,mmax=methraw->numvars; m<mmax; ++m) *++pslot = *++qslot;
			}
		}
		
		// do keyword lookup:
		if (numKeyArgsPushed && methraw->posargs) {
			PyrSymbol **name0, **name;
			PyrSlot *key;
			name0 = meth->argNames.uosym->symbols + 1;
			key = keywordstack;
			for (i=0; i<numKeyArgsPushed; ++i, key+=2) {
				name = name0;
				for (j=1; j<methraw->posargs; ++j, ++name) {
					if (*name == key->us) {
						vars[j+1].ucopy = key[1].ucopy;
						goto found2;
					}
				}
				if (gKeywordError) {
					post("WARNING: keyword arg '%s' not found in call to %s-%s\n",
						key->us->name, meth->ownerclass.uoc->name.us->name, meth->name.us->name);
				}
				found2: ;
			}
		}
		
		// set context variables after varargs have been removed from stack 
		SetNil(&frame->myself);
		SetObject(&frame->method, meth);
		SetFrame(&frame->homeContext, frame);
		if (caller) {
			SetInt(&caller->ip, (long)g->ip);
			SetFrame(&frame->caller, caller);
		} else {
			SetNil(&frame->caller);
		}
		g->ip = meth->code.uob->b - 1;
		SetFrame(&frame->context, frame);
		//***SetNil(&frame->context);
		SetInt(&frame->ip,  0);
	}
	g->receiver.ucopy = vars[1].ucopy;
#if SANITYCHECK
	g->gc->SanityCheck();
	CallStackSanity(g, "<executeMethodWithKeys");
#endif
}


void executeMethod(VMGlobals *g, PyrMethod *meth, long numArgsPushed)
{
	PyrMethodRaw *methraw;
	PyrFrame *frame;
	PyrFrame *caller;
	double *pslot, *qslot;
	PyrSlot *rslot;
	PyrSlot *vars;
	PyrObject *frameobj;
	PyrObject *proto;
	long i, m, mmax, numtemps, numargs;
		
#if SANITYCHECK
	g->gc->SanityCheck();
	CallStackSanity(g, "executeMethod");
#endif
#if DEBUGMETHODS
	if (gTraceInterpreter) {
		if (g->method) {
			postfl(" %s-%s -> %s-%s\n", 
				g->method->ownerclass.uoc->name.us->name, g->method->name.us->name,
				meth->ownerclass.uoc->name.us->name, meth->name.us->name);
		} else {
			postfl(" top -> %s-%s\n", 
				meth->ownerclass.uoc->name.us->name, meth->name.us->name);
		}
	}
#endif
#if METHODMETER
	if (gTraceInterpreter) {
		meth->callMeter.ui++;
	}
#endif
	g->execMethod = 20;
	
	proto = meth->prototypeFrame.uo;
	methraw = METHRAW(meth);
	numtemps = methraw->numtemps;
	numargs = methraw->numargs;
	caller = g->frame;
	//DumpStack(g, g->sp);
	//postfl("executeMethod allArgsPushed %d numKeyArgsPushed %d\n", allArgsPushed, numKeyArgsPushed);
	
	if (methraw->needsHeapContext) {
		frameobj = g->gc->New(methraw->frameSize, 0, obj_slot, true);
		
		vars = frameobj->slots - 1;
		frame = (PyrFrame*)(vars + numtemps);
		frameobj->classptr = class_frame;
		frameobj->size = FRAMESIZE + proto->size; /// <- IS THIS WRONG ??
		SetObject(&frame->myself, frameobj);
		SetObject(&frame->method, meth);
		SetFrame(&frame->homeContext, frame);
		SetFrame(&frame->context, frame);
		//***SetNil(&frame->context);

		if (caller) {
			SetInt(&caller->ip, (long)g->ip);
			SetFrame(&frame->caller, caller);
		} else {
			SetNil(&frame->caller);
		}
		SetInt(&frame->ip,  0);
		g->method = meth;
		
		g->ip = meth->code.uob->b - 1;
		g->frame = frame;
		g->block = (PyrBlock*)meth;
			
		g->sp -= numArgsPushed;
		qslot = (double*)(g->sp);
		pslot = (double*)(vars);
		
		if (numArgsPushed <= numargs) {	/* not enough args pushed */
			/* push all args to frame */
			for (m=0,mmax=numArgsPushed; m<mmax; ++m) *++pslot = *++qslot;
			
			/* push default arg & var values */
			pslot = (double*)(vars + numArgsPushed);
			qslot = (double*)(proto->slots + numArgsPushed - 1);
			for (m=0, mmax=numtemps - numArgsPushed; m<mmax; ++m) *++pslot = *++qslot;
		} else if (methraw->varargs) {
			PyrObject *list;
			double *lslot;
						
			/* push all normal args to frame */
			for (m=0,mmax=numargs; m<mmax; ++m) *++pslot = *++qslot;
			
			/* push list */
			i = numArgsPushed - numargs;
			list = newPyrArray(g->gc, i, 0, false);
			list->size = i;
			
			rslot = (PyrSlot*)pslot+1;
			SetObject(rslot, list);
			//SetObject(vars + numargs + 1, list);
			
			/* put extra args into list */
			lslot = (double*)(list->slots - 1);
			// fixed and raw sizes are zero
			for (m=0,mmax=i; m<mmax; ++m) *++lslot = *++qslot;
			
			if (methraw->numvars) {
				/* push default keyword and var values */
				pslot = (double*)(vars + numargs + 1);
				qslot = (double*)(proto->slots + numargs);
				for (m=0,mmax=methraw->numvars; m<mmax; ++m) *++pslot = *++qslot;
			}
		} else {
			/* push all args to frame */
			for (m=0,mmax=numargs; m<mmax; ++m) *++pslot = *++qslot;
			
			if (methraw->numvars) {
				/* push default keyword and var values */
				pslot = (double*)(vars + numargs);
				qslot = (double*)(proto->slots + numargs - 1);
				for (m=0,mmax=methraw->numvars; m<mmax; ++m) *++pslot = *++qslot;
			}
		}
	} else {  // context frame on the stack
		vars = g->sp - numArgsPushed;
		frame = (PyrFrame*)(vars + numtemps);
		
		g->method = meth;
		g->frame = frame;
		g->block = (PyrBlock*)meth;
		
		g->sp += FRAMESIZE + numtemps - numArgsPushed; // make space for context frame

		if (numArgsPushed <= numargs) {	/* not enough args pushed */
			/* push default arg values */
			pslot = (double*)(vars + numArgsPushed);
			qslot = (double*)(proto->slots + numArgsPushed - 1);
			for (m=0,mmax=numtemps - numArgsPushed; m<mmax; ++m) *++pslot = *++qslot;
		} else if (methraw->varargs) {
			PyrObject *list;
			double *lslot;
			
			/* push list */
			i = numArgsPushed - numargs;
			list = newPyrArray(g->gc, i, 0, false);
			list->size = i;
			
			/* put extra args into list */
			qslot = (double*)(vars + numargs);
			lslot = (double*)(list->slots - 1);
			// fixed and raw sizes are zero
			for (m=0,mmax=i; m<mmax; ++m) *++lslot = *++qslot;
			
			SetObject(vars + numargs + 1, list);
			
			if (methraw->numvars) {
				/* push default keyword and var values */
				pslot = (double*)(vars + numargs + 1);
				qslot = (double*)(proto->slots + numargs);
				for (m=0,mmax=methraw->numvars; m<mmax; ++m) *++pslot = *++qslot;
			}
		} else {
			if (methraw->numvars) {
				pslot = (double*)(vars + numargs);
				qslot = (double*)(proto->slots + numargs - 1);
				for (m=0,mmax=methraw->numvars; m<mmax; ++m) *++pslot = *++qslot;
			}
		}
		
		// set context variables after varargs have been removed from stack 
		SetNil(&frame->myself);
		SetObject(&frame->method, meth);
		SetFrame(&frame->homeContext, frame);
		if (caller) {
			SetInt(&caller->ip, (long)g->ip);
			SetFrame(&frame->caller, caller);
		} else {
			SetNil(&frame->caller);
		}
		g->ip = meth->code.uob->b - 1;
		SetFrame(&frame->context, frame);
		//***SetNil(&frame->context);
		SetInt(&frame->ip,  0);
	}
	g->receiver.ucopy = vars[1].ucopy;
#if SANITYCHECK
	g->gc->SanityCheck();
	CallStackSanity(g, "<executeMethod");
#endif
}

void returnFromBlock(VMGlobals *g)
{
	PyrFrame *curframe;
	PyrFrame *returnFrame;
	PyrFrame *homeContext;
	PyrBlock *block;
	PyrMethod *meth;
	PyrMethodRaw *methraw;
	PyrMethodRaw *blockraw;
	PyrSlot *resultSlot;
	
	//if (gTraceInterpreter) postfl("->returnFromBlock\n");
	//printf("->returnFromBlock\n");
#if SANITYCHECK
	g->gc->SanityCheck();
	CallStackSanity(g, "returnFromBlock");
#endif
	curframe = g->frame;
	returnFrame = curframe->caller.uof;
	if (returnFrame) {
		block = curframe->method.uoblk;
		blockraw = METHRAW(block);
		if (blockraw->needsHeapContext) {
			g->sp[-1].ucopy = g->sp[0].ucopy;
			--g->sp;
		} else {
			resultSlot = curframe->vars - blockraw->popSize;
			resultSlot->ucopy = g->sp[0].ucopy;
			g->sp = resultSlot;
		}
		g->frame = returnFrame;
		g->ip = (unsigned char *)returnFrame->ip.ui;
		g->block = returnFrame->method.uoblk;
		homeContext = returnFrame->homeContext.uof;
		meth = homeContext->method.uom;
		methraw = METHRAW(meth);
		g->receiver.ucopy = homeContext->vars[1 - methraw->numtemps].ucopy; //??
		g->method = meth;
	} else {
		////// this should never happen .
		
		error("return from Function at top of call stack.\n");
		g->method = NULL;
		g->block = NULL;
		g->frame = NULL;
		g->returnLevels = LONG_MAX;
		g->sp = g->gc->Stack()->slots - 1;
	}
	//if (gTraceInterpreter) postfl("<-returnFromBlock\n");
#if SANITYCHECK
	g->gc->SanityCheck();
	CallStackSanity(g, str);
#endif
}


void returnFromMethod(VMGlobals *g)
{
	PyrFrame *returnFrame, *curframe, *frame, *homeContext, *popframe;
	PyrMethod *meth;
	PyrMethodRaw *methraw;
	PyrSlot *resultSlot;
	curframe = g->frame;
	
	//assert(curframe->context.uof == NULL);
	
	//if (gTraceInterpreter) 
	//	post("returnFromMethod %s-%s\n", g->method->ownerclass.uoc->name.us->name, g->method->name.us->name);
#if SANITYCHECK
	g->gc->SanityCheck();
#endif
	homeContext = curframe->context.uof->homeContext.uof;
	if (homeContext == NULL) {
		null_return:
		/*
		post("return all the way out. sd %d\n", g->sp - g->gc->Stack()->slots);
		postfl("%s-%s\n", 
			g->method->ownerclass.uoc->name.us->name, g->method->name.us->name
		);
		DumpStack(g, g->sp);
		*/
		// return all the way out.
		PyrSlot *bottom = g->gc->Stack()->slots;
		bottom->ucopy = g->sp->ucopy;
		g->sp = bottom; // ??!! pop everybody
		g->method = NULL;
		g->block = NULL;
		g->frame = NULL;
		g->returnLevels = LONG_MAX;
	} else {
		returnFrame = homeContext->caller.uof;
		
		if (returnFrame == NULL) goto null_return;
		
		// make sure homeContext is a caller and find earliest stack frame
		frame = curframe;
		popframe = NULL;
		
		while (frame != homeContext) {
			if (!frame->myself.uof) {
				popframe = frame;
			}
			frame = frame->caller.uof;
			if (!frame) {
				error(BULLET BULLET BULLET " Out of context return.\n");
				goto null_return;
			}

		}
		if (!homeContext->myself.uof) {
			popframe = homeContext;
		}

		if (popframe) {
			resultSlot = popframe->vars - METHRAW(popframe->method.uoblk)->popSize;
			resultSlot->ucopy = g->sp[0].ucopy;
			g->sp = resultSlot;
		}

		// return to it
		g->ip = (unsigned char *)returnFrame->ip.ui;
		g->frame = returnFrame;
		g->block = returnFrame->method.uoblk;
						
		homeContext = returnFrame->homeContext.uof;
		meth = homeContext->method.uom;
		methraw = METHRAW(meth);
#if DEBUGMETHODS
if (gTraceInterpreter) {
	postfl("%s-%s <- %s-%s\n", 
		meth->ownerclass.uoc->name.us->name, meth->name.us->name,
		g->method->ownerclass.uoc->name.us->name, g->method->name.us->name
	);
}
#endif

		g->method = meth;
		g->receiver.ucopy = homeContext->vars[1 - methraw->numtemps].ucopy; 
		
	}
#if SANITYCHECK
	g->gc->SanityCheck();
#endif
}


void keywordFixStack(VMGlobals *g, PyrMethod *meth, PyrMethodRaw *methraw, long allArgsPushed, 
		long numKeyArgsPushed)
{
	double *pslot, *qslot;
	long i, j, m, diff, numArgsPushed, numArgsNeeded;
	
	
	if (numKeyArgsPushed) {
		// evacuate keyword args to separate area
		pslot = (double*)(keywordstack + (numKeyArgsPushed<<1));
		qslot = (double*)(g->sp + 1);
		for (m=0; m<numKeyArgsPushed; ++m) {
			*--pslot = *--qslot;
			*--pslot = *--qslot;
		}
	}
	
	numArgsPushed = allArgsPushed - (numKeyArgsPushed<<1);
	numArgsNeeded = methraw->numargs;
	diff = numArgsNeeded - numArgsPushed;
	if (diff != 0) { // incorrect num of args
		if (diff > 0) {  // not enough args
			pslot = (double*)(g->sp);
			qslot = (double*)(meth->prototypeFrame.uo->slots + numArgsPushed - 1);
			for (m=0; m<diff; ++m) *++pslot = *++qslot;
		} else if (methraw->varargs) { // has var args
			numArgsNeeded = numArgsPushed;
		}
	}
	g->sp -= allArgsPushed - 1;
	
	// do keyword lookup:
	if (numKeyArgsPushed && methraw->posargs) {
		PyrSymbol **name0, **name;
		PyrSlot *key, *vars;
		name0 = meth->argNames.uosym->symbols + 1;
		key = keywordstack;
		vars = g->sp;
		for (i=0; i<numKeyArgsPushed; ++i, key+=2) {
			name = name0;
			for (j=1; j<methraw->posargs; ++j, ++name) {
				if (*name == key->us) {
					vars[j].ucopy = key[1].ucopy;
					goto found;
				}
			}
			if (gKeywordError) {
				post("WARNING: keyword arg '%s' not found in call to %s-%s\n",
					key->us->name, meth->ownerclass.uoc->name.us->name, meth->name.us->name);
			}
			found: ;
		}
	}
}

