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

#define NDEBUG
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "PyrKernel.h"
#include "PyrObject.h"
#include "PyrPrimitive.h"
#include "PyrPrimitiveProto.h"
#include "PyrSignal.h"
#include "PyrSched.h"
#include "PyrSignalPrim.h"
#include "PyrFilePrim.h"
#include "PyrMathPrim.h"
#include "PyrListPrim.h"
#include "Opcodes.h"
#include "SC_InlineUnaryOp.h"
#include "SC_InlineBinaryOp.h"
#include "PyrMessage.h"
#include "PyrParseNode.h"
#include "PyrLexer.h"
#include "PyrKernelProto.h"
#include "PyrInterpreter.h"
#include "PyrObjectProto.h"
#include "PyrArchiverT.h"
#include "PyrDeepCopier.h"
#include "PyrDeepFreezer.h"
//#include "Wacom.h"
#include "InitAlloc.h"

#define SANITYCHECK 0
int yyparse();

extern bool gTraceInterpreter;
PyrSymbol *s_recvmsg;

void initPatternPrimitives();

typedef struct {
	PrimitiveHandler func;
	PyrSymbol* name;
	unsigned short base;
	unsigned char numArgs; 
	unsigned char varArgs;
	unsigned char keyArgs;
} PrimitiveDef;

typedef struct {
	int size, maxsize;
	PrimitiveDef *table;
} PrimitiveTable;

extern PrimitiveTable gPrimitiveTable;

extern PyrSlot o_nullframe;


int slotStrVal(PyrSlot *slot, char *str, int maxlen)
{
	if (IsSym(slot)) {
		strncpy(str, slot->us->name, maxlen);
		return errNone;
	} else if (isKindOfSlot(slot, class_string)) {
		int len;
		len = sc_min(maxlen-1, slot->uo->size);
		memcpy(str, slot->uos->s, len);
		str[len] = 0;
		return errNone;
	}
	return errWrongType;
}

int slotPStrVal(PyrSlot *slot, unsigned char *str)
{
	if (IsSym(slot)) {
		strncpy((char*)str+1, slot->us->name, 255);
		str[0] = slot->us->length;
		return errNone;
	} else if (isKindOfSlot(slot, class_string)) {
		int len;
		len = sc_min(255, slot->uo->size);
		memcpy(str+1, slot->uos->s, len);
		str[0] = len;
		return errNone;
	}
	return errWrongType;
}

int instVarAt(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a, *b;
	int index;
	PyrObject *obj;
	
	a = g->sp - 1;
	b = g->sp;
	
	if (a->utag != tagObj) return errWrongType;
	if (b->utag != tagInt) return errIndexNotAnInteger;
	obj = a->uo;
	index = b->ui;
	if (index < 0 || index >= obj->size) return errIndexOutOfRange;
	a->ucopy = obj->slots[index].ucopy;
	return errNone;
}

int instVarPut(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a, *b, *c, *slot;
	int index;
	PyrObject *obj;
	
	a = g->sp - 2;
	b = g->sp - 1;
	c = g->sp;
	
	if (a->utag != tagObj) return errWrongType;
	if (b->utag != tagInt) return errIndexNotAnInteger;
	obj = a->uo;
	if (obj->obj_flags & obj_immutable) return errImmutableObject;
	index = b->ui;
	if (index < 0 || index >= obj->size) return errIndexOutOfRange;
	slot = obj->slots + index;
	slot->ucopy = c->ucopy;
	g->gc->GCWrite(obj, slot);
	return errNone;
}

int instVarSize(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a;
	PyrObject *obj;
	
	a = g->sp;
	if (a->utag != tagObj) {
		SetInt(a, 0);
		return errNone;
	}
	obj = a->uo;
	if (obj->obj_format == obj_notindexed) {
		SetInt(a, obj->size);
	} else {
		SetInt(a, 0);
	}
	return errNone;
}


int objectHash(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a;
	int hash;

	a = g->sp;
	
	hash = calcHash(a);
	SetInt(a, hash);
	return errNone;
}

int objectClass(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a;
	PyrClass *classobj;
	
	a = g->sp;
	classobj = classOfSlot(a);
	SetObject(a, classobj);
	return errNone;
}

int prPrimitiveError(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a;
	
	a = g->sp;
	a->ucopy = g->thread->primitiveError.ucopy;
	return errNone;
}

int prStackDepth(struct VMGlobals *g, int numArgsPushed);
int prStackDepth(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a;
	
	a = g->sp;
	SetInt(a, g->gc->StackDepth());
	return errNone;
}

extern void DumpStack(VMGlobals *g, PyrSlot *sp);

int prDumpStack(struct VMGlobals *g, int numArgsPushed)
{
	DumpStack(g, g->sp);
	return errNone;
}

void DumpDetailedBackTrace(VMGlobals *g);
int prDumpDetailedBackTrace(struct VMGlobals *g, int numArgsPushed);
int prDumpDetailedBackTrace(struct VMGlobals *g, int numArgsPushed)
{
	DumpDetailedBackTrace(g);
	return errNone;
}

int prPrimitiveErrorString(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a;
	PyrString *string;
	char *str;
	
	a = g->sp;
	switch (g->thread->primitiveError.ui) {
		case errReturn : str = "Return (not an error)."; break;
		case errNone : str = "No Error"; break;
		case errFailed : str = "Failed."; break;
		case errBadPrimitive : str = "Bad Primitive."; break;
		case errWrongType : str = "Wrong type."; break;
		case errIndexNotAnInteger : str = "Index not an Integer"; break;
		case errIndexOutOfRange : str = "Index out of range."; break;
		case errImmutableObject : str = "Attempted write to immutable object."; break;
		case errNotAnIndexableObject : str = "Not an indexable object."; break;
		case errRateMismatch : str = "Rate mismatch error."; break;
		case errSpectrumMismatch : str = "Spectrum sizes do not match."; break;
		case errOutOfMemory : str = "Out of memory."; break;
		case errCantCallOS : str = "operation cannot be called from this Process."; break;
		
		case errMsgTooLong : str = "fifo message too int."; break;
		case errFifoTooFull : str = "fifo too full."; break;
		case errFifoEmpty : str = "fifo empty."; break;
		default : str = "Failed.";
	}
	string = newPyrString(g->gc, str, 0, true);
	SetObject(a, string);
	return errNone;
}



int prPostString(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a;
	
	a = g->sp;
	//if (a->utag != tagObj) return errWrongType;
	// assume it is a string!
	postText(a->uos->s, a->uos->size);
	return errNone;
}

int prPostLine(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a;
	
	a = g->sp;
	//if (a->utag != tagObj) return errWrongType;
	// assume it is a string!
	postText(a->uos->s, a->uos->size);
	postChar('\n');
	return errNone;
}

int prDebugger(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a;
	
	a = g->sp;
	//Debugger();
	return errNone;
}




int prObjectString(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a;
	PyrString *string;
	char str[256];
	
	a = g->sp;
	if (postString(a, str)) {
		string = newPyrString(g->gc, str, 0, true);
		SetObject(a, string);
		return errNone;
	} else {
		return errFailed;
	}
}

int prFloat_AsStringPrec(struct VMGlobals *g, int numArgsPushed);
int prFloat_AsStringPrec(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a = g->sp - 1;
	PyrSlot *b = g->sp;
	
	int precision;
	int err = slotIntVal(b, &precision);
	if (err) return err;
	
	char fmt[8], str[32];
	sprintf(fmt, "%%.%dg", precision);
	sprintf(str, fmt, a->uf);
	
	PyrString *string = newPyrString(g->gc, str, 0, true);
	SetObject(a, string);
	return errNone;
	
}

int prAsCompileString(struct VMGlobals *g, int numArgsPushed);
int prAsCompileString(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a;
	PyrString *string;
	char str[256];
	int err;
	
	a = g->sp;
	err = asCompileString(a, str);
	if (err == errNone) {
		string = newPyrString(g->gc, str, 0, true);
		SetObject(a, string);
	}
	return err;
}


int prClassString(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a;
	PyrClass *classobj;
	PyrString *string;
	
	a = g->sp;
	classobj = classOfSlot(a);
	string = newPyrString(g->gc, classobj->name.us->name, 0, true);
	SetObject(a, string);
	return errNone;
}


int prPrimName(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a;
	PyrThread *thread;
	
	a = g->sp;
	thread = a->uot;
	if (thread->primitiveIndex.ui <= gPrimitiveTable.size) {
		SetSymbol(a, gPrimitiveTable.table[thread->primitiveIndex.ui].name);
	} else {
		SetSymbol(a, s_none);
	}
	return errNone;
}

int objectIsKindOf(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a, *b;
	PyrClass *classobj, *testclass;
	int objClassIndex, testClassIndex, maxSubclassIndex;
	
	a = g->sp - 1;
	b = g->sp;
	
	if (b->utag != tagObj) return errWrongType;
	testclass = (PyrClass*)b->uo;
	classobj = classOfSlot(a);
#if 0
	while (classobj) {
		if (classobj == testclass) {
			SetTrue(a);
			return errNone;
		}
		classobj = classobj->superclass.us->u.classobj;
	}
	SetFalse(a);
#else 	
	// constant time lookup method:
	
	objClassIndex = classobj->classIndex.ui;
	testClassIndex = testclass->classIndex.ui;
	maxSubclassIndex = testclass->maxSubclassIndex.ui;

	/*post("%s %s\n", classobj->name.us->name, testclass->name.us->name);
	post("objClassIndex %d\n", objClassIndex);
	post("testClassIndex %d\n", testClassIndex);
	post("maxSubclassIndex %d\n", maxSubclassIndex);*/

	if (objClassIndex >= testClassIndex && objClassIndex <= maxSubclassIndex) {
		SetTrue(a);
		return errNone;
	} else {
		SetFalse(a);
		return errNone;
	}

#endif
	return errNone;
}


int objectIsMemberOf(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a, *b;
	PyrClass *classobj, *testclass;
	
	a = g->sp - 1;
	b = g->sp;
	
	if (b->utag != tagObj) return errWrongType;
	testclass = (PyrClass*)b->uo;
	classobj = classOfSlot(a);
	if (classobj == testclass) {
		SetTrue(a);
	} else {
		SetFalse(a);
	}
	return errNone;
}

int objectIdentical(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a, *b;
	
	a = g->sp - 1;
	b = g->sp;
	
	if (a->ui == b->ui && a->utag == b->utag) {
		SetTrue(a);
	} else {
		SetFalse(a);
	}
	return errNone;
}

int objectNotIdentical(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a, *b;
	
	a = g->sp - 1;
	b = g->sp;
	
	if (a->ui != b->ui || a->utag != b->utag) {
		SetTrue(a);
	} else {
		SetFalse(a);
	}
	return errNone;
}


int basicNewClear(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a, *b;
	int size;
	PyrClass *classobj;
	PyrObject *newobj;
	
	a = g->sp - 1;
	b = g->sp;
	
	if (a->utag != tagObj) return errWrongType;
	classobj = (PyrClass*)a->uo;
	if (classobj->classFlags.ui & classHasIndexableInstances) {
		// create an indexable object
		if (b->utag != tagInt) {
			if (IsFloat(b)) {
				size = (int)b->uf;
			} else if (NotNil(b)) return errIndexNotAnInteger;
			else size = 8;
		} else {
			size = b->ui;
		}
		if (size < 0) size = 0;
	} else {
		size = 0;
	}
	newobj = instantiateObject(g->gc, classobj, size, true, true);
	SetObject(a, newobj);
	return errNone;
}	

int basicNewCopyArgsToInstanceVars(struct VMGlobals *g, int numArgsPushed);
int basicNewCopyArgsToInstanceVars(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a, *b;
	PyrClass *classobj;
	PyrObject *newobj;
	
	a = g->sp - numArgsPushed + 1;
	b = a + 1;
	
	if (a->utag != tagObj) return errWrongType;
	classobj = (PyrClass*)a->uo;
	if (classobj->classFlags.ui & classHasIndexableInstances) {
		error("CopyArgs : object has no instance variables.\n");
		return errFailed;
	}
	newobj = instantiateObject(g->gc, classobj, 0, true, true);
	SetObject(a, newobj);
	
	int length = sc_min(numArgsPushed-1, newobj->size);
	for (int i=0; i<length; ++i) {
		newobj->slots[i].ucopy = b[i].ucopy;
	}
	
	return errNone;
}	



int basicNew(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a, *b;
	int size;
	PyrClass *classobj;
	PyrObject *newobj;
		
	a = g->sp - 1;
	b = g->sp;
	
	if (a->utag != tagObj) return errWrongType;
	classobj = (PyrClass*)a->uo;
	if (classobj->classFlags.ui & classHasIndexableInstances) {
		// create an indexable object
		if (b->utag != tagInt) {
			if (IsFloat(b)) {
				size = (int)b->uf;
			} else if (NotNil(b)) return errIndexNotAnInteger;
			else size = 8;
		} else {
			size = b->ui;
		}
		if (size < 0) size = 0;
	} else {
		size = 0;
	}
	newobj = instantiateObject(g->gc, classobj, size, false, true);
	SetObject(a, newobj);
	return errNone;
}	


bool isClosed(PyrBlock* fundef);
bool isClosed(PyrBlock* fundef)
{
	return IsNil(&fundef->context) && fundef->classptr == class_fundef;
}

bool isWithinClosed(PyrBlock* fundef);
bool isWithinClosed(PyrBlock* fundef)
{
	while (fundef) {
		if (isClosed(fundef)) return true;
		fundef = fundef->context.uoblk;
	}
	return false;
}

int prFunctionDefAsFunction(struct VMGlobals *g, int numArgsPushed);
int prFunctionDefAsFunction(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a = g->sp;

	if (!isClosed(a->uoblk)) {
		dumpObjectSlot(a);
		error("Only closed FunctionDef may be converted to a Function using asFunction.\n");
		return errFailed;
	}

	PyrClosure* closure = (PyrClosure*)g->gc->New(2*sizeof(PyrSlot), 0, obj_notindexed, true);

	closure->classptr = gSpecialClasses[op_class_func]->u.classobj;
	closure->size = 2;
	closure->block.ucopy = a->ucopy;
	closure->context.ucopy = g->process->interpreter.uoi->context.ucopy;
	SetObject(a, closure);
	return errNone;
}

int prFunctionDefDumpContexts(struct VMGlobals *g, int numArgsPushed);
int prFunctionDefDumpContexts(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a = g->sp;

	int i=0;
	while (a->uoblk) {
		post("%2d context %s %08X\n", i++, a->uo->classptr->name.us->name, a->uoblk->context.ui);
		a = &a->uoblk->context;
	}
	return errNone;
}



int prFunctionDefIsClosed(struct VMGlobals *g, int numArgsPushed);
int prFunctionDefIsClosed(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a = g->sp;
	PyrBlock *block = a->uoblk;
	
	SetBool(a, isClosed(block));
	return errNone;
}

int prFunctionDefIsWithinClosed(struct VMGlobals *g, int numArgsPushed);
int prFunctionDefIsWithinClosed(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a = g->sp;
	PyrBlock *block = a->uoblk;
	
	SetBool(a, isWithinClosed(block));
	return errNone;
}

int blockValueArray(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *b;
	PyrObject *array;
	PyrList *list;
	double *pslot, *qslot;
	int m, size;
	
	//a = g->sp - numArgsPushed + 1;
	b = g->sp;
	
	if (b->utag == tagObj) {
		if (b->uo->classptr == class_array) {
			array = (PyrObject*)b->uo;
			above:
			size = array->size;
			
			pslot = (double*)(array->slots - 1);
			qslot = (double*)(b - 1);
			//pend = (double*)(pslot + size);
			//while (pslot<pend) *++qslot = *++pslot;
			for (m=0; m<size; ++m) *++qslot = *++pslot;

			g->sp += size - 1;
			return blockValue(g, size+numArgsPushed-1);
			
		} else if (b->uo->classptr == class_list) {
			list = b->uol;
			if (list->array.utag != tagObj) return errWrongType;
			array = list->array.uo; 
			if (array->classptr != class_array) return errWrongType;
			goto above;
		} else { // last arg is not a list or array, so pass as normal
			return blockValue(g, numArgsPushed);
		}
	} else {
		return blockValue(g, numArgsPushed);
	}
}

int blockValueEnvir(struct VMGlobals *g, int numArgsPushed);

int blockValueArrayEnvir(struct VMGlobals *g, int numArgsPushed);
int blockValueArrayEnvir(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *b;
	PyrObject *array;
	PyrList *list;
	double *pslot, *qslot;
	int m, size;
	
	//a = g->sp - numArgsPushed + 1;
	b = g->sp;
	
	if (b->utag == tagObj) {
		if (b->uo->classptr == class_array) {
			array = (PyrObject*)b->uo;
			above:
			size = array->size;
			
			pslot = (double*)(array->slots - 1);
			qslot = (double*)(b - 1);
			//pend = (double*)(pslot + size);
			//while (pslot<pend) *++qslot = *++pslot;
			for (m=0; m<size; ++m) *++qslot = *++pslot;

			g->sp += size - 1;
			return blockValueEnvir(g, size+numArgsPushed-1);
			
		} else if (b->uo->classptr == class_list) {
			list = b->uol;
			if (list->array.utag != tagObj) return errWrongType;
			array = list->array.uo; 
			if (array->classptr != class_array) return errWrongType;
			goto above;
		} else { // last arg is not a list or array, so pass as normal
			return blockValueEnvir(g, numArgsPushed);
		}
	} else {
		return blockValueEnvir(g, numArgsPushed);
	}
}

int blockValue(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a;
	PyrSlot *args;
	PyrSlot *vars;
	PyrFrame *frame;
	double *pslot, *qslot;
	PyrSlot *rslot;
	PyrObject *frameobj;
	PyrObject *proto;
	int i, m, mmax, numtemps;
	PyrBlock *block;
	PyrFrame *context;
	PyrFrame *caller;
	PyrFrame *homeContext;
	PyrClosure *closure;
	PyrMethodRaw *methraw;
	
	g->execMethod++; // for the benefit of primitives using interpreter recursion

	a = g->sp - numArgsPushed + 1;
	numArgsPushed -- ;
	g->numpop = 0;
	
	closure = (PyrClosure*)a->uo;
	block = closure->block.uoblk;
	context = closure->context.uof;
	
	proto = block->prototypeFrame.uo;
	methraw = METHRAW(block);
	numtemps = methraw->numtemps;
	caller = g->frame;

	if (methraw->needsHeapContext) {
		frameobj = g->gc->New(methraw->frameSize, 0, obj_slot, true);
		vars = frameobj->slots - 1;
		frame = (PyrFrame*)(vars + numtemps);
		frameobj->classptr = class_frame;
		frameobj->size = FRAMESIZE + numtemps; /// <- IS THIS WRONG ??
		SetObject(&frame->myself, frameobj);
		SetObject(&frame->method, block);
		frame->homeContext.ucopy = context->homeContext.ucopy;
		frame->context.ucopy = closure->context.ucopy;
		
		if (caller) {
			SetInt(&caller->ip, (int)g->ip);
			SetFrame(&frame->caller, g->frame);
		} else {
			SetNil(&frame->caller);
		}
		SetInt(&frame->ip,  0);


		g->ip = block->code.uob->b - 1;
		g->frame = frame;
		g->block = block;
		
		g->sp = a;
		args = g->sp;
		if (numArgsPushed <= methraw->numargs) {	/* not enough args pushed */
			/* push all args to frame */
			qslot = (double*)(args);
			pslot = (double*)(vars);

			for (m=0; m<numArgsPushed; ++m) *++pslot = *++qslot;
			
			/* push default arg values */
			pslot = (double*)(vars + numArgsPushed);
			qslot = (double*)(proto->slots + numArgsPushed - 1);
			for (m=0; m<numtemps - numArgsPushed; ++m) *++pslot = *++qslot;
		} else if (methraw->varargs) {
			PyrObject *list;
			double *lslot;
			
			/* push all normal args to frame */
			qslot = (double*)(args);
			pslot = (double*)(vars);
			for (m=0,mmax=methraw->numargs; m<mmax; ++m) *++pslot = *++qslot;
			
			/* push list */
			i = numArgsPushed - methraw->numargs;
			list = newPyrArray(g->gc, i, 0, false);
			list->size = i;
			
			rslot = (PyrSlot*)pslot+1;
			SetObject(rslot, list);
			//SetObject(vars + methraw->numargs + 1, list);
			
			/* put extra args into list */
			lslot = (double*)(list->slots - 1);
			// fixed and raw sizes are zero
			for (m=0; m<i; ++m) *++lslot = *++qslot;
			
			if (methraw->numvars) {
				/* push default keyword and var values */
				pslot = (double*)(vars + methraw->numargs + 1);
				qslot = (double*)(proto->slots + methraw->numargs);
				for (m=0,mmax=methraw->numvars; m<mmax; ++m) *++pslot = *++qslot;
			}
		} else {
			if (methraw->numargs) {
				/* push all args to frame */
				qslot = (double*)(args);
				pslot = (double*)(vars);
				for (m=0,mmax=methraw->numargs; m<mmax; ++m) *++pslot = *++qslot;
			}
			if (methraw->numvars) {
				/* push default keyword and var values */
				pslot = (double*)(vars + methraw->numargs);
				qslot = (double*)(proto->slots + methraw->numargs - 1);
				for (m=0,mmax=methraw->numvars; m<mmax; ++m) *++pslot = *++qslot;
			}
		}
		//g->sp--; // pop the closure
	} else {  // context frame on the stack
		vars = g->sp - numArgsPushed;
		frame = (PyrFrame*)(vars + numtemps);
		g->sp += FRAMESIZE + numtemps - numArgsPushed; // make space for context frame
		g->frame = frame;
		g->block = block;
		
		if (numArgsPushed <= methraw->numargs) {	/* not enough args pushed */

			/* push default arg values */
			pslot = (double*)(vars + numArgsPushed);
			qslot = (double*)(proto->slots + numArgsPushed - 1);
			//pend = pslot + numtemps - numArgsPushed;
			//while (pslot < pend) *++pslot = *++qslot;
			for (m=0,mmax=numtemps - numArgsPushed; m<mmax; ++m) *++pslot = *++qslot;
		} else if (methraw->varargs) {
			PyrObject *list;
			double *lslot;
			
			/* push list */
			i = numArgsPushed - methraw->numargs;
			list = newPyrArray(g->gc, i, 0, false);
			list->size = i;
			
			/* put extra args into list */
			qslot = (double*)(vars + methraw->numargs);
			lslot = (double*)(list->slots - 1);
			// fixed and raw sizes are zero
			for (m=0; m<i; ++m) *++lslot = *++qslot;
			
			SetObject(vars + methraw->numargs + 1, list);
			
			if (methraw->numvars) {
				/* push default keyword and var values */
				pslot = (double*)(vars + methraw->numargs + 1);
				qslot = (double*)(proto->slots + methraw->numargs);
				for (m=0,mmax=methraw->numvars; m<mmax; ++m) *++pslot = *++qslot;
			}


		} else {
			if (methraw->numvars) {
				/* push default var values */
				pslot = (double*)(vars + methraw->numargs);
				qslot = (double*)(proto->slots + methraw->numargs - 1);
				for (m=0,mmax=methraw->numvars; m<mmax; ++m) *++pslot = *++qslot;
			}
		}
		
		// set context variables after varargs have been removed from stack 
		SetNil(&frame->myself);
		SetObject(&frame->method, block);
		
		if (caller) {
			SetInt(&caller->ip, (int)g->ip);
			SetFrame(&frame->caller, caller);
		} else {
			SetNil(&frame->caller);
		}
		g->ip = block->code.uob->b - 1;

		frame->context.ucopy = closure->context.ucopy;
		frame->homeContext.ucopy = context->homeContext.ucopy;
		SetInt(&frame->ip,  0);
		
	}
	homeContext = frame->homeContext.uof;
	if (homeContext) {
		PyrMethodRaw *methraw;
		g->method = homeContext->method.uom;
		methraw = METHRAW(g->method);
		//g->receiver.ucopy = homeContext->vars[1 - meth->numargs - meth->numvars].ucopy;
		g->receiver.ucopy = homeContext->vars[1 - methraw->numtemps].ucopy;
	} else {
		g->receiver.ucopy = g->process->interpreter.ucopy;
	}
	return errNone;
}

int blockValueWithKeys(VMGlobals *g, int allArgsPushed, int numKeyArgsPushed);
int blockValueWithKeys(VMGlobals *g, int allArgsPushed, int numKeyArgsPushed)
{
	PyrSlot *a;
	PyrSlot *args;
	PyrSlot *vars;
	PyrFrame *frame;
	double *pslot, *qslot;
	PyrSlot *rslot;
	PyrObject *frameobj;
	PyrObject *proto;
	int i, j, m, mmax, numtemps, numArgsPushed;
	PyrBlock *block;
	PyrFrame *context;
	PyrFrame *caller;
	PyrFrame *homeContext;
	PyrClosure *closure;
	PyrMethodRaw *methraw;
	
	g->execMethod++; // for the benefit of primitives using interpreter recursion

	a = g->sp - allArgsPushed + 1;
	allArgsPushed -- ;
	g->numpop = 0;
	
	closure = (PyrClosure*)a->uo;
	block = closure->block.uoblk;
	context = closure->context.uof;
	
	proto = block->prototypeFrame.uo;
	methraw = METHRAW(block);
	numtemps = methraw->numtemps;
	caller = g->frame;
	numArgsPushed = allArgsPushed - (numKeyArgsPushed<<1);

	if (methraw->needsHeapContext) {
		frameobj = g->gc->New(methraw->frameSize, 0, obj_slot, true);
		vars = frameobj->slots - 1;
		frame = (PyrFrame*)(vars + numtemps);
		frameobj->classptr = class_frame;
		frameobj->size = FRAMESIZE + numtemps; /// <- IS THIS WRONG ??
		SetObject(&frame->myself, frameobj);
		SetObject(&frame->method, block);
		frame->homeContext.ucopy = context->homeContext.ucopy;
		frame->context.ucopy = closure->context.ucopy;
		
		if (caller) {
			SetInt(&caller->ip, (int)g->ip);
			SetFrame(&frame->caller, g->frame);
		} else {
			SetNil(&frame->caller);
		}
		SetInt(&frame->ip,  0);


		g->ip = block->code.uob->b - 1;
		g->frame = frame;
		g->block = block;
		
		g->sp = a;
		args = g->sp;
		if (numArgsPushed <= methraw->numargs) {	/* not enough args pushed */
			/* push all args to frame */
			qslot = (double*)(args);
			pslot = (double*)(vars);

			for (m=0; m<numArgsPushed; ++m) *++pslot = *++qslot;
			
			/* push default arg values */
			pslot = (double*)(vars + numArgsPushed);
			qslot = (double*)(proto->slots + numArgsPushed - 1);
			for (m=0; m<numtemps - numArgsPushed; ++m) *++pslot = *++qslot;
		} else if (methraw->varargs) {
			PyrObject *list;
			double *lslot;
			
			/* push all normal args to frame */
			qslot = (double*)(args);
			pslot = (double*)(vars);
			for (m=0,mmax=methraw->numargs; m<mmax; ++m) *++pslot = *++qslot;
			
			/* push list */
			i = numArgsPushed - methraw->numargs;
			list = newPyrArray(g->gc, i, 0, false);
			list->size = i;
			
			rslot = (PyrSlot*)pslot+1;
			SetObject(rslot, list);
			//SetObject(vars + methraw->numargs + 1, list);
			
			/* put extra args into list */
			lslot = (double*)(list->slots - 1);
			// fixed and raw sizes are zero
			//lend = lslot + i;
			//while (lslot < lend) *++lslot = *++qslot;
			for (m=0; m<i; ++m) *++lslot = *++qslot;
			
			if (methraw->numvars) {
				/* push default keyword and var values */
				pslot = (double*)(vars + methraw->numargs + 1);
				qslot = (double*)(proto->slots + methraw->numargs);
				for (m=0,mmax=methraw->numvars; m<mmax; ++m) *++pslot = *++qslot;
			}
		} else {
			if (methraw->numargs) {
				/* push all args to frame */
				qslot = (double*)(args);
				pslot = (double*)(vars);
				//pend = pslot + methraw->numargs;
				//while (pslot < pend) *++pslot = *++qslot;
				for (m=0,mmax=methraw->numargs; m<mmax; ++m) *++pslot = *++qslot;
			}
			if (methraw->numvars) {
				/* push default keyword and var values */
				pslot = (double*)(vars + methraw->numargs);
				qslot = (double*)(proto->slots + methraw->numargs - 1);
				//pend = pslot + methraw->numvars;
				//while (pslot<pend) *++pslot = *++qslot;
				for (m=0,mmax=methraw->numvars; m<mmax; ++m) *++pslot = *++qslot;
			}
		}
		// do keyword lookup:
		if (numKeyArgsPushed && methraw->posargs) {
			PyrSlot *key;
			PyrSymbol **name0, **name;
			name0 = block->argNames.uosym->symbols;
			key = g->sp + numArgsPushed + 1;
			for (i=0; i<numKeyArgsPushed; ++i, key+=2) {
				name = name0;
				for (j=0; j<methraw->posargs; ++j, ++name) {
					if (*name == key->us) {
						vars[j+1].ucopy = key[1].ucopy;
						goto found1;
					}
				}
				if (gKeywordError) {
					post("WARNING: keyword arg '%s' not found in call to function.\n",
						key->us->name);
				}
				found1: ;
			}
		}
		//g->sp--; // pop the closure
	} else {  // context frame on the stack
		vars = g->sp - allArgsPushed;
		frame = (PyrFrame*)(vars + numtemps);

		g->frame = frame;
		g->block = block;
		
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
		
		if (numArgsPushed <= methraw->numargs) {	/* not enough args pushed */

			/* push default arg values */
			pslot = (double*)(vars + numArgsPushed);
			qslot = (double*)(proto->slots + numArgsPushed - 1);
			//pend = pslot + numtemps - numArgsPushed;
			//while (pslot < pend) *++pslot = *++qslot;
			for (m=0,mmax=numtemps - numArgsPushed; m<mmax; ++m) *++pslot = *++qslot;
		} else if (methraw->varargs) {
			PyrObject *list;
			double *lslot;
			
			/* push list */
			i = numArgsPushed - methraw->numargs;
			list = newPyrArray(g->gc, i, 0, false);
			list->size = i;
			
			/* put extra args into list */
			qslot = (double*)(vars + methraw->numargs);
			lslot = (double*)(list->slots - 1);
			// fixed and raw sizes are zero
			for (m=0; m<i; ++m) *++lslot = *++qslot;
			
			SetObject(vars + methraw->numargs + 1, list);
			
			if (methraw->numvars) {
				/* push default keyword and var values */
				pslot = (double*)(vars + methraw->numargs + 1);
				qslot = (double*)(proto->slots + methraw->numargs);
				for (m=0,mmax=methraw->numvars; m<mmax; ++m) *++pslot = *++qslot;
			}


		} else {
			if (methraw->numvars) {
				/* push default var values */
				pslot = (double*)(vars + methraw->numargs);
				qslot = (double*)(proto->slots + methraw->numargs - 1);
				for (m=0,mmax=methraw->numvars; m<mmax; ++m) *++pslot = *++qslot;
			}
		}
		
		// do keyword lookup:
		if (numKeyArgsPushed && methraw->posargs) {
			PyrSlot *key;
			PyrSymbol **name0, **name;
			name0 = block->argNames.uosym->symbols;
			key = keywordstack;
			for (i=0; i<numKeyArgsPushed; ++i, key+=2) {
				name = name0;
				for (j=0; j<methraw->posargs; ++j, ++name) {
					if (*name == key->us) {
						vars[j+1].ucopy = key[1].ucopy;
						goto found2;
					}
				}
				if (gKeywordError) {
					post("WARNING: keyword arg '%s' not found in call to function.\n",
						key->us->name);
				}
				found2: ;
			}
		}
		
		// set context variables after varargs have been removed from stack 
		SetNil(&frame->myself);
		SetObject(&frame->method, block);
		
		if (caller) {
			SetInt(&caller->ip, (int)g->ip);
			SetFrame(&frame->caller, caller);
		} else {
			SetNil(&frame->caller);
		}
		g->ip = block->code.uob->b - 1;

		frame->context.ucopy = closure->context.ucopy;
		frame->homeContext.ucopy = context->homeContext.ucopy;
		SetInt(&frame->ip,  0);
		
	}
	homeContext = frame->homeContext.uof;
	if (homeContext) {
		PyrMethodRaw *methraw;
		g->method = homeContext->method.uom;
		methraw = METHRAW(g->method);
		//g->receiver.ucopy = homeContext->vars[1 - meth->numargs - meth->numvars].ucopy;
		g->receiver.ucopy = homeContext->vars[1 - methraw->numtemps].ucopy;
	} else {
		g->receiver.ucopy = g->process->interpreter.ucopy;
	}
	return errNone;
}

PyrObject* envirArray(PyrObject *envir);
bool envirArrayAt(PyrObject *array, PyrSlot *key, PyrSlot *res);

int blockValueEnvir(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a;
	PyrSlot *args;
	PyrSlot *vars;
	PyrFrame *frame;
	double *pslot, *qslot;
	PyrSlot *rslot;
	PyrObject *frameobj;
	PyrObject *proto;
	int i, m, mmax, numtemps;
	PyrBlock *block;
	PyrFrame *context;
	PyrFrame *caller;
	PyrFrame *homeContext;
	PyrClosure *closure;
	PyrMethodRaw *methraw;
	PyrSlot *curEnvirSlot;
	PyrObject *envArray;
	
	g->execMethod++; // for the benefit of primitives using interpreter recursion

	a = g->sp - numArgsPushed + 1;
	numArgsPushed -- ;
	g->numpop = 0;
	
	closure = (PyrClosure*)a->uo;
	block = closure->block.uoblk;
	context = closure->context.uof;
	
	proto = block->prototypeFrame.uo;
	methraw = METHRAW(block);
	numtemps = methraw->numtemps;
	caller = g->frame;

	if (methraw->needsHeapContext) {
		frameobj = g->gc->New(methraw->frameSize, 0, obj_slot, true);
		vars = frameobj->slots - 1;
		frame = (PyrFrame*)(vars + numtemps);
		frameobj->classptr = class_frame;
		frameobj->size = FRAMESIZE + numtemps; /// <- IS THIS WRONG ??
		SetObject(&frame->myself, frameobj);
		SetObject(&frame->method, block);
		frame->homeContext.ucopy = context->homeContext.ucopy;
		frame->context.ucopy = closure->context.ucopy;
		
		if (caller) {
			SetInt(&caller->ip, (int)g->ip);
			SetFrame(&frame->caller, g->frame);
		} else {
			SetNil(&frame->caller);
		}
		SetInt(&frame->ip,  0);


		g->ip = block->code.uob->b - 1;
		g->frame = frame;
		g->block = block;
		
		g->sp = a;
		args = g->sp;
		if (numArgsPushed <= methraw->numargs) {	/* not enough args pushed */
			/* push all args to frame */
			qslot = (double*)(args);
			pslot = (double*)(vars);

			for (m=0; m<numArgsPushed; ++m) *++pslot = *++qslot;
			
			/* push default arg values */
			pslot = (double*)(vars + numArgsPushed);
			qslot = (double*)(proto->slots + numArgsPushed - 1);
			for (m=0; m<numtemps - numArgsPushed; ++m) *++pslot = *++qslot;

			// replace defaults with environment variables
			curEnvirSlot = g->classvars[0].uo->slots + 1; // currentEnvironment is the second class var.
			envArray = envirArray(curEnvirSlot->uo);
			if (envArray) {
				PyrSymbol **argNames;
				argNames = block->argNames.uosym->symbols;
				for (m=numArgsPushed; m<methraw->numargs; ++m) {
					// replace the args with values from the environment if they exist
					PyrSlot keyslot;
					SetSymbol(&keyslot, argNames[m]);
					envirArrayAt(envArray, &keyslot, vars+m+1);
				}
			}
		} else if (methraw->varargs) {
			PyrObject *list;
			double *lslot;
			
			/* push all normal args to frame */
			qslot = (double*)(args);
			pslot = (double*)(vars);
			for (m=0,mmax=methraw->numargs; m<mmax; ++m) *++pslot = *++qslot;
			
			/* push list */
			i = numArgsPushed - methraw->numargs;
			list = newPyrArray(g->gc, i, 0, false);
			list->size = i;
			
			rslot = (PyrSlot*)pslot+1;
			SetObject(rslot, list);
			//SetObject(vars + methraw->numargs + 1, list);
			
			/* put extra args into list */
			lslot = (double*)(list->slots - 1);
			// fixed and raw sizes are zero
			for (m=0; m<i; ++m) *++lslot = *++qslot;
			
			if (methraw->numvars) {
				/* push default keyword and var values */
				pslot = (double*)(vars + methraw->numargs + 1);
				qslot = (double*)(proto->slots + methraw->numargs);
				for (m=0,mmax=methraw->numvars; m<mmax; ++m) *++pslot = *++qslot;
			}
		} else {
			if (methraw->numargs) {
				/* push all args to frame */
				qslot = (double*)(args);
				pslot = (double*)(vars);
				for (m=0,mmax=methraw->numargs; m<mmax; ++m) *++pslot = *++qslot;
			}
			if (methraw->numvars) {
				/* push default keyword and var values */
				pslot = (double*)(vars + methraw->numargs);
				qslot = (double*)(proto->slots + methraw->numargs - 1);
				for (m=0,mmax=methraw->numvars; m<mmax; ++m) *++pslot = *++qslot;
			}
		}
		//g->sp--; // pop the closure
	} else {  // context frame on the stack
		vars = g->sp - numArgsPushed;
		frame = (PyrFrame*)(vars + numtemps);
		g->sp += FRAMESIZE + numtemps - numArgsPushed; // make space for context frame
		g->frame = frame;
		g->block = block;
		
		if (numArgsPushed <= methraw->numargs) {	/* not enough args pushed */

			/* push default arg values */
			pslot = (double*)(vars + numArgsPushed);
			qslot = (double*)(proto->slots + numArgsPushed - 1);
			//pend = pslot + numtemps - numArgsPushed;
			//while (pslot < pend) *++pslot = *++qslot;
			for (m=0,mmax=numtemps - numArgsPushed; m<mmax; ++m) *++pslot = *++qslot;

			// replace defaults with environment variables
			curEnvirSlot = g->classvars[0].uo->slots + 1; // currentEnvironment is the second class var.
			envArray = envirArray(curEnvirSlot->uo);
			if (envArray) {
				PyrSymbol **argNames;
				argNames = block->argNames.uosym->symbols;
				for (m=numArgsPushed; m<methraw->numargs; ++m) {
					// replace the args with values from the environment if they exist
					PyrSlot keyslot;
					SetSymbol(&keyslot, argNames[m]);
					envirArrayAt(envArray, &keyslot, vars+m+1);
				}
			}

		} else if (methraw->varargs) {
			PyrObject *list;
			double *lslot;
			
			/* push list */
			i = numArgsPushed - methraw->numargs;
			list = newPyrArray(g->gc, i, 0, false);
			list->size = i;
			
			/* put extra args into list */
			qslot = (double*)(vars + methraw->numargs);
			lslot = (double*)(list->slots - 1);
			// fixed and raw sizes are zero
			for (m=0; m<i; ++m) *++lslot = *++qslot;
			
			SetObject(vars + methraw->numargs + 1, list);
			
			if (methraw->numvars) {
				/* push default keyword and var values */
				pslot = (double*)(vars + methraw->numargs + 1);
				qslot = (double*)(proto->slots + methraw->numargs);
				for (m=0,mmax=methraw->numvars; m<mmax; ++m) *++pslot = *++qslot;
			}


		} else {
			if (methraw->numvars) {
				/* push default var values */
				pslot = (double*)(vars + methraw->numargs);
				qslot = (double*)(proto->slots + methraw->numargs - 1);
				for (m=0,mmax=methraw->numvars; m<mmax; ++m) *++pslot = *++qslot;
			}
		}
		
		// set context variables after varargs have been removed from stack 
		SetNil(&frame->myself);
		SetObject(&frame->method, block);
		
		if (caller) {
			SetInt(&caller->ip, (int)g->ip);
			SetFrame(&frame->caller, caller);
		} else {
			SetNil(&frame->caller);
		}
		g->ip = block->code.uob->b - 1;

		frame->context.ucopy = closure->context.ucopy;
		frame->homeContext.ucopy = context->homeContext.ucopy;
		SetInt(&frame->ip,  0);
		
	}
	homeContext = frame->homeContext.uof;
	if (homeContext) {
		PyrMethodRaw *methraw;
		g->method = homeContext->method.uom;
		methraw = METHRAW(g->method);
		//g->receiver.ucopy = homeContext->vars[1 - meth->numargs - meth->numvars].ucopy;
		g->receiver.ucopy = homeContext->vars[1 - methraw->numtemps].ucopy;
	} else {
		g->receiver.ucopy = g->process->interpreter.ucopy;
	}
	return errNone;
}

int blockValueEnvirWithKeys(VMGlobals *g, int allArgsPushed, int numKeyArgsPushed);
int blockValueEnvirWithKeys(VMGlobals *g, int allArgsPushed, int numKeyArgsPushed)
{
	PyrSlot *a;
	PyrSlot *args;
	PyrSlot *vars;
	PyrFrame *frame;
	double *pslot, *qslot;
	PyrSlot *rslot;
	PyrObject *frameobj;
	PyrObject *proto;
	int i, j, m, mmax, numtemps, numArgsPushed;
	PyrBlock *block;
	PyrFrame *context;
	PyrFrame *caller;
	PyrFrame *homeContext;
	PyrClosure *closure;
	PyrMethodRaw *methraw;
	PyrSlot *curEnvirSlot;
	PyrObject *envArray;
	
	g->execMethod++; // for the benefit of primitives using interpreter recursion

	a = g->sp - allArgsPushed + 1;
	allArgsPushed -- ;
	g->numpop = 0;
	
	closure = (PyrClosure*)a->uo;
	block = closure->block.uoblk;
	context = closure->context.uof;
	
	proto = block->prototypeFrame.uo;
	methraw = METHRAW(block);
	numtemps = methraw->numtemps;
	caller = g->frame;
	numArgsPushed = allArgsPushed - (numKeyArgsPushed<<1);

	if (methraw->needsHeapContext) {
		frameobj = g->gc->New(methraw->frameSize, 0, obj_slot, true);
		vars = frameobj->slots - 1;
		frame = (PyrFrame*)(vars + numtemps);
		frameobj->classptr = class_frame;
		frameobj->size = FRAMESIZE + numtemps; /// <- IS THIS WRONG ??
		SetObject(&frame->myself, frameobj);
		SetObject(&frame->method, block);
		frame->homeContext.ucopy = context->homeContext.ucopy;
		frame->context.ucopy = closure->context.ucopy;
		
		if (caller) {
			SetInt(&caller->ip, (int)g->ip);
			SetFrame(&frame->caller, g->frame);
		} else {
			SetNil(&frame->caller);
		}
		SetInt(&frame->ip,  0);


		g->ip = block->code.uob->b - 1;
		g->frame = frame;
		g->block = block;
		
		g->sp = a;
		args = g->sp;
		if (numArgsPushed <= methraw->numargs) {	/* not enough args pushed */
			/* push all args to frame */
			qslot = (double*)(args);
			pslot = (double*)(vars);

			for (m=0; m<numArgsPushed; ++m) *++pslot = *++qslot;
			
			/* push default arg values */
			pslot = (double*)(vars + numArgsPushed);
			qslot = (double*)(proto->slots + numArgsPushed - 1);
			for (m=0; m<numtemps - numArgsPushed; ++m) *++pslot = *++qslot;
			
			// replace defaults with environment variables
			curEnvirSlot = g->classvars[0].uo->slots + 1; // currentEnvironment is the second class var.
			envArray = envirArray(curEnvirSlot->uo);
			if (envArray) {
				PyrSymbol **argNames;
				argNames = block->argNames.uosym->symbols;
				for (m=numArgsPushed; m<methraw->numargs; ++m) {
					// replace the args with values from the environment if they exist
					PyrSlot keyslot;
					SetSymbol(&keyslot, argNames[m]);
					envirArrayAt(envArray, &keyslot, vars+m+1);
				}
			}
			
			
		} else if (methraw->varargs) {
			PyrObject *list;
			double *lslot;
			
			/* push all normal args to frame */
			qslot = (double*)(args);
			pslot = (double*)(vars);
			for (m=0,mmax=methraw->numargs; m<mmax; ++m) *++pslot = *++qslot;
			
			/* push list */
			i = numArgsPushed - methraw->numargs;
			list = newPyrArray(g->gc, i, 0, false);
			list->size = i;
			
			rslot = (PyrSlot*)pslot+1;
			SetObject(rslot, list);
			//SetObject(vars + methraw->numargs + 1, list);
			
			/* put extra args into list */
			lslot = (double*)(list->slots - 1);
			// fixed and raw sizes are zero
			//lend = lslot + i;
			//while (lslot < lend) *++lslot = *++qslot;
			for (m=0; m<i; ++m) *++lslot = *++qslot;
			
			if (methraw->numvars) {
				/* push default keyword and var values */
				pslot = (double*)(vars + methraw->numargs + 1);
				qslot = (double*)(proto->slots + methraw->numargs);
				for (m=0,mmax=methraw->numvars; m<mmax; ++m) *++pslot = *++qslot;
			}
		} else {
			if (methraw->numargs) {
				/* push all args to frame */
				qslot = (double*)(args);
				pslot = (double*)(vars);
				//pend = pslot + methraw->numargs;
				//while (pslot < pend) *++pslot = *++qslot;
				for (m=0,mmax=methraw->numargs; m<mmax; ++m) *++pslot = *++qslot;
			}
			if (methraw->numvars) {
				/* push default keyword and var values */
				pslot = (double*)(vars + methraw->numargs);
				qslot = (double*)(proto->slots + methraw->numargs - 1);
				//pend = pslot + methraw->numvars;
				//while (pslot<pend) *++pslot = *++qslot;
				for (m=0,mmax=methraw->numvars; m<mmax; ++m) *++pslot = *++qslot;
			}
		}
		// do keyword lookup:
		if (numKeyArgsPushed && methraw->posargs) {
			PyrSymbol **name0, **name;
			PyrSlot *key;
			name0 = block->argNames.uosym->symbols;
			key = g->sp + numArgsPushed + 1;
			for (i=0; i<numKeyArgsPushed; ++i, key+=2) {
				name = name0;
				for (j=0; j<methraw->posargs; ++j, ++name) {
					if (*name == key->us) {
						vars[j+1].ucopy = key[1].ucopy;
						goto found1;
					}
				}
				if (gKeywordError) {
					post("WARNING: keyword arg '%s' not found in call to function.\n",
						key->us->name);
				}
				found1: ;
			}
		}
		//g->sp--; // pop the closure
	} else {  // context frame on the stack
		vars = g->sp - allArgsPushed;
		frame = (PyrFrame*)(vars + numtemps);

		g->frame = frame;
		g->block = block;
		
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
		
		if (numArgsPushed <= methraw->numargs) {	/* not enough args pushed */

			/* push default arg values */
			pslot = (double*)(vars + numArgsPushed);
			qslot = (double*)(proto->slots + numArgsPushed - 1);
			//pend = pslot + numtemps - numArgsPushed;
			//while (pslot < pend) *++pslot = *++qslot;
			for (m=0,mmax=numtemps - numArgsPushed; m<mmax; ++m) *++pslot = *++qslot;
			
			// replace defaults with environment variables
			curEnvirSlot = g->classvars[0].uo->slots + 1; // currentEnvironment is the second class var.
			envArray = envirArray(curEnvirSlot->uo);
			if (envArray) {
				PyrSymbol **argNames;
				argNames = block->argNames.uosym->symbols;
				for (m=numArgsPushed; m<methraw->numargs; ++m) {
					// replace the args with values from the environment if they exist
					PyrSlot keyslot;
					SetSymbol(&keyslot, argNames[m]);
					envirArrayAt(envArray, &keyslot, vars+m+1);
				}
			}

		} else if (methraw->varargs) {
			PyrObject *list;
			double *lslot;
			
			/* push list */
			i = numArgsPushed - methraw->numargs;
			list = newPyrArray(g->gc, i, 0, false);
			list->size = i;
			
			/* put extra args into list */
			qslot = (double*)(vars + methraw->numargs);
			lslot = (double*)(list->slots - 1);
			// fixed and raw sizes are zero
			for (m=0; m<i; ++m) *++lslot = *++qslot;
			
			SetObject(vars + methraw->numargs + 1, list);
			
			if (methraw->numvars) {
				/* push default keyword and var values */
				pslot = (double*)(vars + methraw->numargs + 1);
				qslot = (double*)(proto->slots + methraw->numargs);
				for (m=0,mmax=methraw->numvars; m<mmax; ++m) *++pslot = *++qslot;
			}


		} else {
			if (methraw->numvars) {
				/* push default var values */
				pslot = (double*)(vars + methraw->numargs);
				qslot = (double*)(proto->slots + methraw->numargs - 1);
				for (m=0,mmax=methraw->numvars; m<mmax; ++m) *++pslot = *++qslot;
			}
		}
		
		// do keyword lookup:
		if (numKeyArgsPushed && methraw->posargs) {
			PyrSymbol **name0, **name;
			PyrSlot *key;
			name0 = block->argNames.uosym->symbols;
			key = keywordstack;
			for (i=0; i<numKeyArgsPushed; ++i, key+=2) {
				name = name0;
				for (j=0; j<methraw->posargs; ++j, ++name) {
					if (*name == key->us) {
						vars[j+1].ucopy = key[1].ucopy;
						goto found2;
					}
				}
				if (gKeywordError) {
					post("WARNING: keyword arg '%s' not found in call to function.\n",
						key->us->name);
				}
				found2: ;
			}
		}
		
		// set context variables after varargs have been removed from stack 
		SetNil(&frame->myself);
		SetObject(&frame->method, block);
		
		if (caller) {
			SetInt(&caller->ip, (int)g->ip);
			SetFrame(&frame->caller, caller);
		} else {
			SetNil(&frame->caller);
		}
		g->ip = block->code.uob->b - 1;

		frame->context.ucopy = closure->context.ucopy;
		frame->homeContext.ucopy = context->homeContext.ucopy;
		SetInt(&frame->ip,  0);
		
	}
	homeContext = frame->homeContext.uof;
	if (homeContext) {
		PyrMethodRaw *methraw;
		g->method = homeContext->method.uom;
		methraw = METHRAW(g->method);
		//g->receiver.ucopy = homeContext->vars[1 - meth->numargs - meth->numvars].ucopy;
		g->receiver.ucopy = homeContext->vars[1 - methraw->numtemps].ucopy;
	} else {
		g->receiver.ucopy = g->process->interpreter.ucopy;
	}
	return errNone;
}


int objectPerform(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *recvrSlot, *selSlot, *listSlot;
	double *pslot, *qslot;
	PyrSymbol *selector;
	int m, mmax;
	
	recvrSlot = g->sp - numArgsPushed + 1;
	selSlot = recvrSlot + 1;
	if (IsSym(selSlot)) {
		selector = selSlot->us;
		// move args down one to fill selector's position
		pslot = (double*)(selSlot - 1);
		qslot = (double*)selSlot;
		for (m=0; m<numArgsPushed - 2; ++m) *++pslot = *++qslot;
		g->sp -- ;
		numArgsPushed -- ;
		// now the stack looks just like it would for a normal message send
	} else if (IsObj(selSlot)) {
		listSlot = selSlot;
		if (listSlot->uo->classptr == class_list) {
			listSlot = listSlot->uo->slots;
		}
		if (listSlot->utag != tagObj || listSlot->uo->classptr != class_array) {
			goto badselector;
		}
		PyrObject *array = listSlot->uo;
		if (array->size < 1) {
			error("Array must have a selector.\n");
			return errFailed; 
		}
		selSlot = array->slots;
		selector = selSlot->us;

		if (numArgsPushed>2) {
			qslot = (double*)recvrSlot + numArgsPushed;
			pslot = (double*)recvrSlot + numArgsPushed + array->size - 2;
			for (m=0; m<numArgsPushed - 2; ++m) *--pslot = *--qslot;
		}
		
		pslot = (double*)(recvrSlot);
		qslot = (double*)(selSlot);
		for (m=0,mmax=array->size-1; m<mmax; ++m) *++pslot = *++qslot;
		
		g->sp += array->size - 2;
		numArgsPushed += array->size - 2;
		// now the stack looks just like it would for a normal message send
		
	} else {
		badselector:
		error("perform selector not a Symbol or Array.\n");
		dumpObjectSlot(listSlot);
		return errWrongType; 
	}
	
	sendMessage(g, selector, numArgsPushed);
	g->numpop = 0;
	return errNone;
}

int objectPerformWithKeys(VMGlobals *g, int numArgsPushed, int numKeyArgsPushed);
int objectPerformWithKeys(VMGlobals *g, int numArgsPushed, int numKeyArgsPushed)
{
	PyrSlot *recvrSlot, *selSlot, *listSlot;
	double *pslot, *qslot;
	PyrSymbol *selector;
	int m, mmax;
	
	recvrSlot = g->sp - numArgsPushed + 1;
	selSlot = recvrSlot + 1;
	if (IsSym(selSlot)) {
		selector = selSlot->us;
		// move args down one to fill selector's position
		pslot = (double*)(selSlot - 1);
		qslot = (double*)selSlot;
		for (m=0; m<numArgsPushed - 2; ++m) *++pslot = *++qslot;
		g->sp -- ;
		numArgsPushed -- ;
		// now the stack looks just like it would for a normal message send
	} else if (IsObj(selSlot)) {
		listSlot = selSlot;
		if (listSlot->uo->classptr == class_list) {
			listSlot = listSlot->uo->slots;
		}
		if (listSlot->utag != tagObj || listSlot->uo->classptr != class_array) {
			goto badselector;
		}
		PyrObject *array = listSlot->uo;
		if (array->size < 1) {
			error("Array must have a selector.\n");
			return errFailed; 
		}
		selSlot = array->slots;
		selector = selSlot->us;

		if (numArgsPushed>2) {
			qslot = (double*)recvrSlot + numArgsPushed;
			pslot = (double*)recvrSlot + numArgsPushed + array->size - 2;
			for (m=0; m<numArgsPushed - 2; ++m) *--pslot = *--qslot;
		}
		
		pslot = (double*)(recvrSlot);
		qslot = (double*)(selSlot);
		for (m=0,mmax=array->size-1; m<mmax; ++m) *++pslot = *++qslot;
		
		g->sp += array->size - 2;
		numArgsPushed += array->size - 2;
		// now the stack looks just like it would for a normal message send
		
	} else {
		badselector:
		error("perform selector not a Symbol or Array.\n");
		dumpObjectSlot(listSlot);
		return errWrongType; 
	}
	
	sendMessageWithKeys(g, selector, numArgsPushed, numKeyArgsPushed);
	g->numpop = 0;
	return errNone;
}


int objectPerformList(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *recvrSlot, *selSlot, *listSlot;
	double *pslot, *qslot;
	PyrSymbol *selector;
	int m, mmax, numargslots;
	PyrObject *array;
	
	recvrSlot = g->sp - numArgsPushed + 1;
	selSlot = recvrSlot + 1;
	listSlot = g->sp;
	numargslots = numArgsPushed - 3;
	if (selSlot->utag != tagSym) {
		error("Selector not a Symbol :\n");
		return errWrongType;
	}
	selector = selSlot->us;
	if (listSlot->utag != tagObj) {
		return objectPerform(g, numArgsPushed);
	}
	if (listSlot->uo->classptr == class_array) {
		doarray:
		pslot = (double*)(recvrSlot);
		if (numargslots>0) {
			qslot = (double*)selSlot;
			for (m=0; m<numargslots; ++m) *++pslot = *++qslot;
		} else numargslots = 0;
		array = listSlot->uo;
		qslot = (double*)(array->slots - 1);
		for (m=0,mmax=array->size; m<mmax; ++m) *++pslot = *++qslot;
	} else if (listSlot->uo->classptr == class_list) {
		listSlot = listSlot->uo->slots;
		if (listSlot->utag != tagObj || listSlot->uo->classptr != class_array) {
			error("List array not an Array.\n");
			dumpObjectSlot(listSlot);
			return errWrongType; 
		}
		goto doarray;
	} else {
		return objectPerform(g, numArgsPushed);
	}
	g->sp += array->size - 2;
	numArgsPushed = numargslots + array->size + 1;
	// now the stack looks just like it would for a normal message send
	
	sendMessage(g, selector, numArgsPushed);
	g->numpop = 0;
	return errNone;
}


int objectPerformSelList(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *recvrSlot, *selSlot, *listSlot;
	double *pslot, *qslot;
	PyrSymbol *selector;
	int m, mmax;
	PyrObject *array;
	
	recvrSlot = g->sp - 1;
	listSlot = g->sp;

	if (listSlot->utag != tagObj) {
		error("Expected Array or List.. Got :\n");
		dumpObjectSlot(listSlot);
		return errWrongType;
	}
	if (listSlot->uo->classptr == class_array) {
		doarray:
		array = listSlot->uo;
		
		selSlot = array->slots;
		if (selSlot->utag != tagSym) {
			error("Selector not a Symbol :\n");
			return errWrongType;
		}
		selector = selSlot->us;
		
		pslot = (double*)(recvrSlot);
		qslot = (double*)(selSlot);
		for (m=0,mmax=array->size-1; m<mmax; ++m) *++pslot = *++qslot;
	} else if (listSlot->uo->classptr == class_list) {
		listSlot = listSlot->uo->slots;
		if (listSlot->utag != tagObj || listSlot->uo->classptr != class_array) {
			error("List array not an Array.\n");
			dumpObjectSlot(listSlot);
			return errWrongType; 
		}
		goto doarray;
	} else {
		error("Expected Array or List.. Got :\n");
		dumpObjectSlot(listSlot);
		return errWrongType;
	}
	g->sp += array->size - 2;
	numArgsPushed = array->size;
	// now the stack looks just like it would for a normal message send
	
	sendMessage(g, selector, numArgsPushed);
	g->numpop = 0;
	return errNone;
}


int arrayPerformMsg(struct VMGlobals *g, int numArgsPushed);
int arrayPerformMsg(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *recvrSlot, *selSlot, *arraySlot;
	double *pslot, *qslot;
	PyrSymbol *selector;
	int m, mmax, numargslots;
	PyrObject *array;
	
	arraySlot = g->sp - numArgsPushed + 1;
	array = arraySlot->uo;
	if (array->size < 2) {
		error("Array must contain a receiver and a selector.\n");
		return errFailed;
	}
	recvrSlot = array->slots;
	selSlot = recvrSlot + 1;
	numargslots = numArgsPushed - 1;
	if (selSlot->utag != tagSym) {
		error("Selector not a Symbol :\n");
		return errWrongType;
	}
	
	selector = selSlot->us;

	arraySlot->ucopy = recvrSlot->ucopy;

	if (numargslots>0) {
		qslot = (double*)arraySlot + numargslots + 1;
		pslot = (double*)arraySlot + numargslots + array->size - 1;
		for (m=0; m<numargslots; ++m) *--pslot = *--qslot;
	} else numargslots = 0;
	
	pslot = (double*)(arraySlot);
	qslot = (double*)(selSlot);
	for (m=0,mmax=array->size-2; m<mmax; ++m) *++pslot = *++qslot;
		
	g->sp += array->size - 2;
	numArgsPushed = numargslots + array->size - 1;
	
	// now the stack looks just like it would for a normal message send
	
	sendMessage(g, selector, numArgsPushed);
	g->numpop = 0;
	return errNone;
}

int objectDump(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a;
	
	a = g->sp;
	dumpObjectSlot(a);
	return errNone;
}


int prTotalFree(struct VMGlobals *g, int numArgsPushed);
int prTotalFree(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a = g->sp;
	SetInt(a, g->allocPool->TotalFree());
	return errNone;
}

int prLargestFreeBlock(struct VMGlobals *g, int numArgsPushed);
int prLargestFreeBlock(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a = g->sp;
	SetInt(a, g->allocPool->LargestFreeChunk());
	return errNone;
}

int dumpGCinfo(struct VMGlobals *g, int numArgsPushed);
int dumpGCinfo(struct VMGlobals *g, int numArgsPushed)
{
	g->gc->DumpInfo();
	return errNone;
}

int dumpGCAll(struct VMGlobals *g, int numArgsPushed);
int dumpGCAll(struct VMGlobals *g, int numArgsPushed)
{
	//g->gc->DumpEverything();
	return errNone;
}

int prGCSanity(struct VMGlobals *g, int numArgsPushed);
int prGCSanity(struct VMGlobals *g, int numArgsPushed)
{
	g->gc->SanityCheck();
	return errNone;
}

#if GCDEBUG
int prTraceAllPathsTo(struct VMGlobals *g, int numArgsPushed);
int prTraceAllPathsTo(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a = g->sp;
	g->gc->TracePathsTo(a->uo, false);
	return errNone;
}

int prTraceAnyPathsTo(struct VMGlobals *g, int numArgsPushed);
int prTraceAnyPathsTo(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a = g->sp;
	g->gc->TracePathsTo(a->uo, true);
	return errNone;
}

int prTraceAnyPathToAllInstancesOf(struct VMGlobals *g, int numArgsPushed);
int prTraceAnyPathToAllInstancesOf(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a = g->sp;
	g->gc->TraceAnyPathToAllInstancesOf(a->uoc->name.us);
	return errNone;
}
#endif

extern PyrClass *gClassList;

int prAllClasses(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a;
	PyrClass *classobj;
	PyrObject *array;
	int i;
	
	a = g->sp;
	
	array = newPyrArray(g->gc, gNumClasses, 0, true); 
	classobj = gClassList;
	for (i=0; classobj; ++i) {
		SetObject(array->slots + i, classobj);
		classobj = classobj->nextclass.uoc;
	}
	array->size = gNumClasses;
	SetObject(a, array);
	return errNone;
}

int prPostClassTree(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a;
	
	a = g->sp;
	postClassTree(a->uoc, 0);
	return errNone;
}

int prDumpBackTrace(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a;
	
	a = g->sp;
	DumpBackTrace(g);
	return errNone;
}

void MakeDebugFrame(VMGlobals *g, PyrFrame *frame, PyrSlot *outSlot);
void MakeDebugFrame(VMGlobals *g, PyrFrame *frame, PyrSlot *outSlot)
{
	int i, j;
	PyrMethod *meth;
	PyrMethodRaw *methraw;
	
	meth = frame->method.uom;
	methraw = METHRAW(meth);
	
	PyrObject* debugFrameObj = instantiateObject(g->gc, getsym("DebugFrame")->u.classobj, 0, false, true);
	SetObject(outSlot, debugFrameObj);
	
	SetObject(debugFrameObj->slots + 0, meth);
	SetInt(debugFrameObj->slots + 5, (int)meth);
	
	//int numtemps = methraw->numargs;
	int numargs = methraw->numargs;
	int numvars = methraw->numvars;
	int tempOffset = methraw->numtemps - 1;
	if (numargs) {
		PyrObject* argArray = (PyrObject*)newPyrArray(g->gc, numargs, 0, false);
		SetObject(debugFrameObj->slots + 1, argArray);
		for (i=0; i<numargs; ++i) {
			argArray->slots[i].uf = frame->vars[i - tempOffset].uf;
		}
		argArray->size = numargs;
	} else {
		SetNil(debugFrameObj->slots + 1);
	}
	if (numvars) {
		PyrObject* varArray = (PyrObject*)newPyrArray(g->gc, numvars, 0, false);
		SetObject(debugFrameObj->slots + 2, varArray);
		for (i=0,j=numargs; i<numvars; ++i,++j) {
			varArray->slots[i].uf = frame->vars[j - tempOffset].uf;
		}
		varArray->size = numvars;
	} else {
		SetNil(debugFrameObj->slots + 2);
	}
	
	if ((frame->caller.utag == tagHFrame || frame->caller.utag == tagSFrame) && frame->caller.uof) {
		//SetNil(debugFrameObj->slots + 3);
		MakeDebugFrame(g, frame->caller.uof, debugFrameObj->slots + 3);
		//postbuf("Caller:\n");
		//dumpObjectSlot(debugFrameObj->slots + 3);
	} else {
		SetNil(debugFrameObj->slots + 3);
	}
	
	if ((frame->context.utag == tagHFrame || frame->context.utag == tagSFrame) 
			&& frame->context.uof && frame->context.uof != frame) {
		MakeDebugFrame(g, frame->context.uof, debugFrameObj->slots + 4);
		//postbuf("Context:\n");
		//dumpObjectSlot(debugFrameObj->slots + 4);
	} else {
		SetNil(debugFrameObj->slots + 4);
	}
}


int prGetBackTrace(VMGlobals *g, int numArgsPushed);
int prGetBackTrace(VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a;
	
	a = g->sp;
	MakeDebugFrame(g, g->frame, a);
	
	return errNone;
}

int prObjectShallowCopy(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a;
	
	a = g->sp;
	switch (a->utag) {
		case tagObj :
			a->uo = copyObject(g->gc, a->uo, true);
			break;
		case tagHFrame :
			// fix this later
			SetNil(a);
			break;
		case tagSFrame :
			// fix this later
			SetNil(a);
			break;
		// the default case is to leave the argument unchanged on the stack			
	}
	return errNone;
}

int prObjectCopyImmutable(struct VMGlobals *g, int numArgsPushed);
int prObjectCopyImmutable(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a;
	
	a = g->sp;
	switch (a->utag) {
		case tagObj :
			if (a->uo->obj_flags & obj_immutable) {
				a->uo = copyObject(g->gc, a->uo, true);
			}
			break;
		case tagHFrame :
			// fix this later
			SetNil(a);
			break;
		case tagSFrame :
			// fix this later
			SetNil(a);
			break;
		// the default case is to leave the argument unchanged on the stack			
	}
	return errNone;
}

int prObjectIsMutable(struct VMGlobals *g, int numArgsPushed);
int prObjectIsMutable(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a;
	
	a = g->sp;
	if (a->utag == tagObj) {
		if (a->uo->obj_flags & obj_immutable) {
			SetFalse(a);
		} else {
			SetTrue(a);
		}
	} else {
		SetFalse(a);
	}
	return errNone;
}

int prObjectIsPermanent(struct VMGlobals *g, int numArgsPushed);
int prObjectIsPermanent(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a;
	
	a = g->sp;
	if (a->utag == tagObj) {
		if (a->uo->gc_color == obj_permanent) {
			SetTrue(a);
		} else {
			SetFalse(a);
		}
	} else {
		SetTrue(a);
	}
	return errNone;
}



int prDeepFreeze(struct VMGlobals *g, int numArgsPushed);
int prDeepFreeze(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a;
	
	if (g->processID != 0) {
		error("Permanent objects may only be created from the non real time virtual machine.\n");
		return errFailed;
	}

	a = g->sp;
	PyrDeepFreezer freezer(g);
	int err = freezer.doDeepFreeze(a);
	return err;
}


int prDeepCopy(struct VMGlobals *g, int numArgsPushed);
int prDeepCopy(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a;
	
	if (g->processID != 0) {
		error("Permanent objects may only be created from the non real time virtual machine.\n");
		return errFailed;
	}

	a = g->sp;
	PyrDeepCopier copier(g);
	int err = copier.doDeepCopy(a);
	return err;
}




struct RemoteMsg { 
	int32 processID, numSlots;
	PyrSlot slots[32];
};

bool IsSimpleLiteralSlot(PyrSlot* slot);
bool IsSimpleLiteralSlot(PyrSlot* slot)
{
	switch (slot->utag) {
		case tagObj : return slot->uo->IsPermanent();
		case tagHFrame : return false;
		case tagSFrame : return false;
		case tagInt : return true;
		case tagSym : return true;
		case tagChar : return true;
		case tagNil : return true;
		case tagFalse : return true;
		case tagTrue : return true;
		case tagInf : return true;
		case tagPtr : return false;
		default : return true;
	}
}



int prObjectCopyRange(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a, *b, *c;
	
	a = g->sp - 2;
	b = g->sp - 1;
	c = g->sp;
	
	if (a->utag != tagObj) return errWrongType;
	if (b->utag != tagInt) return errWrongType;
	if (c->utag != tagInt) return errWrongType;
	a->uo = copyObjectRange(g->gc, a->uo, b->ui, c->ui, true);

	return errNone;
}

void switchToThread(struct VMGlobals *g, struct PyrThread *newthread, int oldstate, int *numArgsPushed);

int haltInterpreter(struct VMGlobals *g, int numArgsPushed)
{
	switchToThread(g, g->process->mainThread.uot, tDone, &numArgsPushed);
	g->returnLevels = LONG_MAX;
	SetNil(g->sp);
	//hmm need to fix this to work only on main thread. //!!!
	//g->sp = g->gc->Stack()->slots - 1;
	
	return errReturn;
}



int prTraceOn(struct VMGlobals *g, int numArgsPushed);
int prTraceOn(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a;
	
	a = g->sp;
	gTraceInterpreter = IsTrue(a);
	return errNone;
}

int prKeywordError(struct VMGlobals *g, int numArgsPushed);
int prKeywordError(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a;
	
	a = g->sp;
	gKeywordError = IsTrue(a);
	return errNone;
}

int prFunDef_NumArgs(struct VMGlobals *g, int numArgsPushed);
int prFunDef_NumArgs(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a;
	PyrMethodRaw *methraw;
	
	a = g->sp;
	methraw = METHRAW(a->uoblk);
	SetInt(a, methraw->numargs);
	return errNone;
}

int prFunDef_NumVars(struct VMGlobals *g, int numArgsPushed);
int prFunDef_NumVars(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a;
	PyrMethodRaw *methraw;
	
	a = g->sp;
	methraw = METHRAW(a->uoblk);
	SetInt(a, methraw->numvars);
	return errNone;
}

int prFunDef_VarArgs(struct VMGlobals *g, int numArgsPushed);
int prFunDef_VarArgs(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a;
	PyrMethodRaw *methraw;
	
	a = g->sp;
	methraw = METHRAW(a->uoblk);
	if (methraw->varargs) { SetTrue(a); } else { SetFalse(a); }
	return errNone;
}


int undefinedPrimitive(struct VMGlobals *g, int numArgsPushed)
{
	error("A primitive was not bound. %d %d\n", g->primitiveIndex, gPrimitiveTable.size);
	dumpObject((PyrObject*)g->primitiveMethod);
	return errFailed;
}

void dumpByteCodes(PyrBlock *theBlock);

int prDumpByteCodes(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a;
	
	a = g->sp;
	dumpByteCodes(a->uoblk);
	return errNone;
}

int prObjectPointsTo(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a, *b, temp;
	PyrObject *obj;
	int i;
	
	a = g->sp - 1;
	b = g->sp;
	
	if (a->utag != tagObj) a->ucopy = o_false.ucopy;
	else {
		obj = a->uo;
		for (i=0; i<obj->size; ++i) {
			getIndexedSlot(obj, &temp, i);
			if (temp.utag == b->utag && temp.ui == b->ui) {
				a->ucopy = o_true.ucopy;
				return errNone;
			}
		}
		a->ucopy = o_false.ucopy;
	}
	return errNone;
}


int prObjectRespondsTo(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a, *b;
	PyrClass *classobj;
	PyrMethod *meth;
	PyrSymbol *selector;
	int index;
	
	a = g->sp - 1;
	b = g->sp;
	
	if (b->utag != tagSym) return errWrongType;

	classobj = classOfSlot(a);
	
	selector = b->us;
	index = classobj->classIndex.ui + selector->u.index;
	meth = gRowTable[index];
	if (meth->name.us != selector) {
		a->ucopy = o_false.ucopy;
	} else {
		a->ucopy = o_true.ucopy;
	}
	return errNone;
}

PyrMethod* GetFunctionCompileContext(VMGlobals* g);
PyrMethod* GetFunctionCompileContext(VMGlobals* g)
{
	PyrClass *classobj;
	PyrSymbol *classsym, *contextsym;
	PyrMethod *meth;
	// lookup interpreter class
	classsym = getsym("Interpreter");
	classobj = classsym->u.classobj;
	if (!classobj) {
		error("There is no Interpreter class.\n");
		return 0;
	}
	// lookup functionCompileContext method
	contextsym = getsym("functionCompileContext");
	int index = classobj->classIndex.ui + contextsym->u.index;
	meth = gRowTable[index];
	if (!meth || meth->name.us != contextsym) {
		error("compile context method 'functionCompileContext' not found.\n");
		return 0;
	}
	gCompilingClass = classobj;
	gCompilingMethod = meth;
	gCompilingBlock = (PyrBlock*)meth;
	return meth;
}

#if !SCPLAYER
int prCompileString(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a, *b;
	PyrString *string;
	PyrMethod *meth;
	
	a = g->sp - 1;
	b = g->sp;
	
	// check b is a string
	if (b->utag != tagObj) return errWrongType;
	if (!isKindOf(b->uo,  class_string)) return errWrongType;
	string = b->uos;
	
	gRootParseNode = NULL;
	initParserPool();
		//assert(g->gc->SanityCheck());
	startLexerCmdLine(string->s, string->size);
	compileErrors = 0;
	compilingCmdLine = true;
	gCompilingVMGlobals = g;
	compilingCmdLineErrorWindow = false;
		//assert(g->gc->SanityCheck());
	parseFailed = yyparse();
		//assert(g->gc->SanityCheck());
	if (!parseFailed && gRootParseNode) {
		PyrSlot slotResult;

		meth = GetFunctionCompileContext(g);
		if (!meth) return errFailed;
		
		((PyrBlockNode*)gRootParseNode)->isTopLevel = true;
		
		SetNil(&slotResult);
		COMPILENODE(gRootParseNode, &slotResult);

		if (slotResult.utag != tagObj 
			|| slotResult.uo->classptr != class_fundef) {
				compileErrors++;
			error("Compile did not return a FunctionDef..\n");
		}
		if (compileErrors) {
			SetNil(a);
		} else {
			PyrBlock *block;	
			PyrClosure *closure;
			
			block = slotResult.uoblk;
			// create a closure
			closure = (PyrClosure*)g->gc->New(2*sizeof(PyrSlot), 0, obj_notindexed, false);
			closure->classptr = class_func;
			closure->size = 2;
			SetObject(&closure->block, block);
			closure->context.ucopy = g->process->interpreter.uoi->context.ucopy;
			SetObject(a, closure);
		}
	} else {
		if (parseFailed) {
			compileErrors++;
			error("Command line parse failed\n");
		} else {
			postfl("<nothing to do>\n");
		}
		SetNil(a);
	}
	finiLexer();
	freeParserPool();
	
	pyr_pool_compile->FreeAll();
	//flushErrors();
	compilingCmdLine = false;
	
	return !(parseFailed || compileErrors) ? errNone : errFailed;
}
#endif

char sCodeStringIn[8192];
char sCodeStringOut[8192];

int prUGenCodeString(struct VMGlobals *g, int numArgsPushed);
int prUGenCodeString(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *aa, *bb, *cc, *dd, *ee;
	char *out = sCodeStringOut;
	char ugenPrefix[16];
	int err;
	
	aa = g->sp - 4;	// code string
	bb = g->sp - 3;	// ugen prefix
	ee = g->sp - 2;	// isDecl
	cc = g->sp - 1;	// input names
	dd = g->sp;		// input value strings
	
	int ugenIndex;
	err = slotIntVal(bb, &ugenIndex);
	if (err) return err;
	if (!isKindOfSlot(cc, class_array) && !isKindOfSlot(cc, class_symbolarray)) return errWrongType;
	if (!isKindOfSlot(dd, class_array)) return errWrongType;
	bool isDecl = IsTrue(ee);
	
	PyrObject *inputNamesObj = cc->uo;
	PyrObject *inputStringsObj = dd->uo;
	
	sprintf(ugenPrefix, "u%d", ugenIndex);
	int ugenPrefixSize = strlen(ugenPrefix);
	PyrString* codeStringObj = aa->uos;
	int codeStringSize = codeStringObj->size;
	if (codeStringSize > 8000) {
		error("input string too int.\n");
		return errFailed;
	}
	memcpy(sCodeStringIn, codeStringObj->s, codeStringSize);
	sCodeStringIn[codeStringSize] = 0;
		
	char* in = sCodeStringIn;
	int c;
	while ((c = *in++) != 0) {
		if (c == '@') {
			if (!isDecl) {
				if (*in != '@') {
					*out++ = 's';
					*out++ = '-';
					*out++ = '>';
				} else in++;
			}
			for (int j=0; j<ugenPrefixSize; ++j) {
				*out++ = ugenPrefix[j];
			}
		} else if (c == '$') {
			char name[64];
			int j=0;
			do {
				c = *in++;
				if (c == 0) break;
				if (!(c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z' || c >= '0' && c <= '9')) {
					--in;
					break;
				}
				name[j++] = c;
			} while (c);
			
			bool found = false;
			int nameSize = j;
			int slotIndex = -1;
			for (int j=0; j<inputNamesObj->size; ++j) {
				PyrSlot inputNameSlot;
				getIndexedSlot(inputNamesObj, &inputNameSlot, j);
				if (!IsSym(&inputNameSlot)) return errWrongType;
				PyrSymbol* inputSym = inputNameSlot.us;
				char *inputName = inputSym->name;
				int inputNameSize = inputSym->length;
				if (inputNameSize == nameSize && strncmp(inputName, name, nameSize)==0) {
					found = true;
					slotIndex = j;
					break;
				}
			}
			if (slotIndex >= 0) {
				PyrSlot *inputStringSlot = inputStringsObj->slots + slotIndex;
				if (!isKindOfSlot(inputStringSlot, class_string)) return errWrongType;
				PyrString *inputStringObj = inputStringSlot->uos;
				char *input = inputStringObj->s;
				int inputStringSize = inputStringObj->size;
				for (int j=0; j<inputStringSize; ++j) {
					*out++ = input[j];
				}
			} else {
				*out++ = '?';	*out++ = '?';
				for (int j=0; j<nameSize; ++j) {
					*out++ = name[j];
				}
				*out++ = '?';	*out++ = '?';
			}
		} else {
			*out++ = c;
		}
		if (out - sCodeStringOut > 8000) {
			*out++ = '\n';
			*out++ = '.';
			*out++ = '.';
			*out++ = '.';
			*out++ = '\n';
			break;
		}
	}
	*out++ = 0;
	PyrString* outString = newPyrString(g->gc, sCodeStringOut, 0, true);
	SetObject(aa, outString);

	return errNone;
}



/*void threadSanity(VMGlobals *g, PyrThread *thread);
void threadSanity(VMGlobals *g, PyrThread *thread)
{
	int state;
	g->gc->numToScan = 1000000;
	doGC(g, 0);
	assert(g->gc->SanityCheck());
	
	state = thread->state.ui;
	if (state == tYield) {
		if (!IsObj(&thread->method)) { error("thread method not an Object\n"); }
		else if (!isKindOf(thread->method.uo, class_method)) { error("thread method not a Method\n"); }
		else if (thread->method.uo->gc_color == gcColor.gcFree) { error("thread method is FREE\n"); }
		
		if (!IsObj(&thread->block)) { error("thread block not an Object\n"); }
		else if (!isKindOf(thread->block.uo, class_func)) { error("thread block not a Function\n"); }
		else if (thread->block.uo->gc_color == gcColor.gcFree) { error("thread block is FREE\n"); }

		if (IsObj(&thread->receiver) && thread->receiver.uo->gc_color == gcColor.gcFree) 
			{ error("thread receiver is FREE\n"); }

		FrameSanity(thread->frame.uof);
		
		oldthread->method.uom = g->method;
		oldthread->block.uoblk = g->block;
		SetFrame(&oldthread->frame, g->frame);
		oldthread->ip.ui = (int)g->ip;
		oldthread->sp.ui = (int)g->sp;


	} else if (state == tInit) {
	} else {
		postfl("bad state\n");
	}
}*/


PyrSymbol *s_prready;
PyrSymbol *s_prrunnextthread;

void switchToThread(VMGlobals *g, PyrThread *newthread, int oldstate, int *numArgsPushed);
void switchToThread(VMGlobals *g, PyrThread *newthread, int oldstate, int *numArgsPushed)
{
	PyrThread *oldthread;
	GC *gc;
	PyrObject *frameobj;
	PyrFrame *frame;
	
	oldthread = g->thread;
	if (newthread == oldthread) return;
	//postfl("->switchToThread %d %08X -> %08X\n", oldstate, oldthread, newthread);
	//post("->stack %08X  g->sp %08X [%d]  g->top %08X [%d]\n", 
	//	g->gc->Stack()->slots, g->sp, g->sp - g->gc->Stack()->slots, g->top, g->top - g->gc->Stack()->slots);
	//assert(g->gc->SanityCheck());
	//CallStackSanity(g);
	//gcDumpInfo(g->gc);
	gc = g->gc;

	oldthread->state.ui = oldstate;
	
	if (oldstate == tDone) {
		SetObject(&oldthread->stack, gc->Stack());
		gc->ToWhite(gc->Stack());
		gc->Stack()->size = 0;
		gc->GCWrite(oldthread, gc->Stack());
		SetNil(&oldthread->method);
		SetNil(&oldthread->block);
		SetNil(&oldthread->receiver);
	} else if (oldstate == tInit) {
		SetObject(&oldthread->stack, gc->Stack());
		gc->ToWhite(gc->Stack());
		gc->Stack()->size = 0;
		gc->GCWrite(oldthread, gc->Stack());
		SetNil(&oldthread->method);
		SetNil(&oldthread->block);
		SetNil(&oldthread->receiver);
	} else {
		// save old thread's state
		SetObject(&oldthread->stack, gc->Stack());
		gc->ToWhite(gc->Stack());
		gc->Stack()->size = g->sp - gc->Stack()->slots + 1;
		//post("else %08X %08X\n", oldthread->stack.uo, gc->Stack());
	
		oldthread->method.uom = g->method;
		oldthread->block.uoblk = g->block;
		SetFrame(&oldthread->frame, g->frame);
		oldthread->ip.ui = (int)g->ip;
		oldthread->sp.ui = (int)g->sp;
		oldthread->receiver.ucopy = g->receiver.ucopy;
		oldthread->numArgsPushed.ui = *numArgsPushed;
		oldthread->numpop.ui = g->numpop;
		
		// these are perhaps unecessary because a thread may not 
		// legally block within a C primitive
		oldthread->returnLevels.ui = g->returnLevels;
			
		//gc->ToGrey(oldthread);
		if (gc->ObjIsBlack(oldthread)) {
			gc->GCWriteBlack(gc->Stack());
			gc->GCWriteBlack(g->method);
			gc->GCWriteBlack(g->block);
			
			if (oldthread->frame.utag == tagHFrame) {
				frame = oldthread->frame.uof;
				frameobj = frame->myself.uo;
				gc->GCWriteBlack(frameobj);
			}
			
			gc->GCWriteBlack(&g->receiver);
		}
	}
	
	// restore new thread's state
	g->thread = newthread;
	SetObject(&g->process->curThread, newthread);
	gc->GCWrite(g->process, newthread);

	gc->SetStack(newthread->stack.uo);
	gc->ToBlack(gc->Stack());
	SetNil(&newthread->stack);
	
	g->method = newthread->method.uom;
	g->block = newthread->block.uoblk;
	g->frame = newthread->frame.uof;
	g->ip = (unsigned char *)newthread->ip.ui;
	g->sp = (PyrSlot*)newthread->sp.ui;
	g->receiver.ucopy = newthread->receiver.ucopy;
	
	g->rgen = (RGen*)(newthread->randData.uo->slots);

	*numArgsPushed = newthread->numArgsPushed.ui;
	
	// these are perhaps unecessary because a thread may not 
	// legally block within a C primitive
	g->numpop = newthread->numpop.ui;
	g->returnLevels = newthread->returnLevels.ui;
	
	//if (g->ip == (unsigned char *)-1) g->returnLevels = 1;
	//else g->execMethod = 1; //??
	g->execMethod = 1;
	
	//post("switchToThread ip %d\n", g->ip);
	//post("switchToThread newthread->ip.ui %d\n", newthread->ip.ui);
	//post("switchToThread oldthread->ip.ui %d\n", oldthread->ip.ui);
	//post("switchToThread returnLevels %d\n", g->returnLevels);
	
	// wipe out values which will become stale as new thread executes:
	newthread->method.ui = 0;
	newthread->block.ui = 0;
	newthread->frame.ui = 0;
	newthread->ip.ui = 0;
	newthread->sp.ui = 0;
	SetNil(&newthread->receiver);
	
	newthread->state.ui = tRunning;

	//post("old thread %08X stack %08X\n", oldthread, oldthread->stack.uo);
	//post("new thread %08X stack %08X\n", g->thread, g->thread->stack.uo);
	//post("main thread %08X stack %08X\n", g->process->mainThread.uot, g->process->mainThread.uot->stack.uo);
	
	//postfl("<-switchToThread\n");
	//post("<-stack %08X  g->sp %08X [%d]  g->top %08X [%d]\n", 
	//	g->gc->Stack()->slots, g->sp, g->sp - g->gc->Stack()->slots, g->top, g->top - g->gc->Stack()->slots);
	//assert(g->gc->SanityCheck());
	//CallStackSanity(g);
}

#if 0
void pushThread(VMGlobals *g, PyrObject *obj, PyrSlot *head, PyrThread* thread);
void pushThread(VMGlobals *g, PyrObject *obj, PyrSlot *head, PyrThread* thread)
{
	PyrSlot *tail = head + 1;
	
	//post("->pushThread\n");
	tail = head+1;
	if (NotNil(tail)) {
		SetObject(&tail->uot->nextThread, thread);
		g->gc->GCWrite(tail->uo, thread);
	} else {
		SetObject(head, thread);
		g->gc->GCWrite(obj, thread);
	}
	SetObject(tail, thread);
	g->gc->GCWrite(obj, thread);
}

PyrThread* popThread(VMGlobals *g, PyrObject *obj, PyrSlot *head);
PyrThread* popThread(VMGlobals *g, PyrObject *obj, PyrSlot *head)
{
	PyrSlot *tail = head + 1;
	PyrThread* thread;
	
	//post("->popThread\n");
	thread = head->uot;
	tail = head+1;
	if (tail->uot == thread) {
		SetNil(head);
		SetNil(tail);
	} else {
		head->ucopy = thread->nextThread.ucopy;
		g->gc->GCWrite(obj, head->uo);
	}
	return thread;
}
#endif

void initPyrThread(class GC *gc, PyrThread *thread, PyrSlot *func, int stacksize, PyrInt32Array* rgenArray, double time, bool collect);
void initPyrThread(class GC *gc, PyrThread *thread, PyrSlot *func, int stacksize, PyrInt32Array* rgenArray, double time, bool collect)
{
	PyrObject *array;
	
	thread->func.ucopy = func->ucopy;
	gc->GCWrite(thread, func);

	SetInt(&thread->stackSize, stacksize);
	
	array = newPyrArray(gc, stacksize, 0, collect);
	SetObject(&thread->stack, array);
	gc->GCWrite(thread, array);
	thread->state.ui = tInit;
		
	SetObject(&thread->randData, rgenArray);
	gc->GCWrite(thread, rgenArray);

	thread->time.uf = time;
}

extern PyrSymbol *s_prstart;

int prThreadInit(struct VMGlobals *g, int numArgsPushed);
int prThreadInit(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a, *b, *c;
	int stacksize, err;
	PyrThread *thread;
	
	//postfl("->prThreadInit\n");
	//assert(g->gc->SanityCheck());
	//CallStackSanity(g);
	a = g->sp - 2;	// thread
	b = g->sp - 1;	// function
	c = g->sp;		// stacksize
	
	thread = a->uot;
	
	if (b->utag != tagObj || !isKindOf(b->uo, class_func)) {
		error("Thread function arg not a Function.\n");
		return errWrongType;
	}
	
	err = slotIntVal(c, &stacksize);
	if (err) return err;
	
	initPyrThread(g->gc, thread, b, stacksize, (PyrInt32Array*)(g->thread->randData.uo), g->thread->time.uf, true);
	
	//postfl("<-prThreadInit\n");
	//assert(g->gc->SanityCheck());
	//CallStackSanity(g);
	return errNone;
}

int prThreadRandSeed(struct VMGlobals *g, int numArgsPushed);
int prThreadRandSeed(struct VMGlobals *g, int numArgsPushed)
{	
	PyrSlot *a = g->sp - 1;	// thread
	PyrSlot *b = g->sp;		// rand seed
	
	PyrThread *thread = a->uot;

	int32 seed;
	int err = slotIntVal(b, &seed);
	if (err) return err;

	PyrInt32Array *rgenArray = newPyrInt32Array(g->gc, 4, 0, true);
	rgenArray->size = 4;
	((RGen*)(rgenArray->i))->init(seed);

	if (thread == g->thread) {
		g->rgen = (RGen*)(rgenArray->i);
	}
	SetObject(&thread->randData, rgenArray);
	g->gc->GCWrite(thread, rgenArray);
	return errNone;
}



int prRoutineYield(struct VMGlobals *g, int numArgsPushed);
int prRoutineYield(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot value;
	
	//postfl("->prRoutineYield %d %08X\n", g->level, g->thread);
	//assert(g->gc->SanityCheck());
	//CallStackSanity(g);
	//postfl("->numArgsPushed %d\n", numArgsPushed);
	if (!isKindOf((PyrObject*)g->thread, class_routine)) {
		//debugf ("yield was called outside of a Routine.\n");
		error ("yield was called outside of a Routine.\n");
		return errFailed;
	}
	/*if (!g->thread->parent.uot) {
		error ("yield was called from a thread with no parent.\n");
		return errFailed;
	}*/
	value.ucopy = g->sp->ucopy;

	//debugf("yield from thread %08X to parent %08X\n", g->thread, g->thread->parent.uot);
	switchToThread(g, g->thread->parent.uot, tYieldToParent, &numArgsPushed);
	// on the other side of the looking glass, put the yielded value on the stack as the result..
	(g->sp - numArgsPushed + 1)->ucopy = value.ucopy;
	//postfl("<-numArgsPushed %d\n", numArgsPushed);
	//postfl("<-prRoutineYield %d\n", g->level);
	//assert(g->gc->SanityCheck());
	//CallStackSanity(g);
	return errNone;
}

int prRoutineAlwaysYield(struct VMGlobals *g, int numArgsPushed);
int prRoutineAlwaysYield(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot value;
	
	//postfl("->prRoutineAlwaysYield\n");
	//assert(g->gc->SanityCheck());
	//CallStackSanity(g);
	if (!isKindOf((PyrObject*)g->thread, class_routine)) {
		error ("alwaysYield was called outside of a Routine.\n");
		return errFailed;
	}
	/*if (!g->thread->parent.uot) {
		error ("alwaysYield was called from a thread with no parent.\n");
		return errFailed;
	}*/
	
	value.ucopy = g->sp->ucopy;
	g->thread->terminalValue.ucopy = value.ucopy;
	g->gc->GCWrite(g->thread, g->sp);
	
	//debugf("alwaysYield from thread %08X to parent %08X\n", g->thread, g->thread->parent.uot);
	switchToThread(g, g->thread->parent.uot, tDone, &numArgsPushed);
	
	// on the other side of the looking glass, put the yielded value on the stack as the result..
	(g->sp - numArgsPushed + 1)->ucopy = value.ucopy;
	
	//postfl("<-prRoutineAlwaysYield %d\n", g->level);
	//assert(g->gc->SanityCheck());
	//CallStackSanity(g);
	return errNone;
}

int prRoutineResume(struct VMGlobals *g, int numArgsPushed);
int prRoutineResume(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a, *b, threadSlot, value;
	PyrThread *thread;
	int state;
	
	//postfl("->prRoutineResume\n");
	//assert(g->gc->SanityCheck());
	//CallStackSanity(g);
	a = g->sp - 1;
	b = g->sp;
	
	thread = a->uot;
	state = thread->state.ui;
	//postfl("state %d\n", state);
	if (state == tInit) {
		threadSlot.ucopy = a->ucopy;
		value.ucopy = b->ucopy;

	//post("g->thread %08X\n", g->thread);
	//post("thread %08X\n", thread);
		SetObject(&thread->parent, g->thread);
		g->gc->GCWrite(thread, g->thread);

	//postfl("start into thread %08X from parent %08X\n", thread, g->thread);
		switchToThread(g, thread, tYieldToChild, &numArgsPushed);
		
		// set stack
		//post("set stack %08X %08X\n", g->sp, g->gc->Stack()->slots - 1);
		g->sp = g->gc->Stack()->slots - 1;
		g->receiver.ucopy = (++g->sp)->uf = threadSlot.ucopy;
		(++g->sp)->ucopy = value.ucopy;
		
		sendMessage(g, s_prstart, 2);
	} else if (state == tYieldToParent) {

		SetObject(&thread->parent, g->thread);
		g->gc->GCWrite(thread, g->thread);

		value.ucopy = b->ucopy;
	//debugf("resume into thread %08X from parent %08X\n", thread, g->thread);
		switchToThread(g, thread, tYieldToChild, &numArgsPushed);
		// on the other side of the looking glass, put the yielded value on the stack as the result..
		(g->sp - numArgsPushed + 1)->ucopy = value.ucopy;
	} else if (state == tDone) {
		a->ucopy = thread->terminalValue.ucopy;
	} else if (state == tRunning) {
		error("Tried to resume the running thread.\n");
		return errFailed;
	} else {
		error("Thread in strange state: %d\n", state);
		return errFailed;
	}
	//postfl("<-prRoutineResume %d %08X\n", g->level, g->thread);
	return errNone;
}

int prRoutineReset(struct VMGlobals *g, int numArgsPushed);
int prRoutineReset(struct VMGlobals *g, int numArgsPushed)
{
	PyrThread *thread;
	int state;
	
	//post("->prRoutineReset\n");
	//assert(g->gc->SanityCheck());
	//CallStackSanity(g);
	thread = g->sp->uot;
	state = thread->state.ui;
	if (state == tYieldToParent || state == tYieldToChild) {
		thread->state.ui = tInit;
		thread->stack.uo->size = 0;
	} else if (state == tDone) {
		PyrObject *array;
		thread->state.ui = tInit;
		array = newPyrArray(g->gc, thread->stackSize.ui, 0, true);
		SetObject(&thread->stack, array);
		g->gc->GCWrite(thread, array);
	} else if (state == tInit) {
		// do nothing
	} else if (state == tRunning) {
		error("A Routine cannot reset itself except by yieldAndReset.\n");
		return errFailed;
	} else {
		error("Routine in unknown state. %d\n", state);
		return errFailed;
	}
	//assert(g->gc->SanityCheck());
	//CallStackSanity(g);
	return errNone;
}

int prRoutineStop(struct VMGlobals *g, int numArgsPushed);
int prRoutineStop(struct VMGlobals *g, int numArgsPushed)
{
	PyrThread *thread;
	int state;
	//post("->prRoutineStop\n");
	//assert(g->gc->SanityCheck());
	//CallStackSanity(g);
	
	thread = g->sp->uot;
	state = thread->state.ui;


	if (state == tYieldToParent || state == tYieldToChild || state == tInit) {
		SetNil(&g->thread->terminalValue);
		thread->state.ui = tDone;
	} else if (state == tDone) {
		// do nothing
	} else if (state == tRunning) {
		error("Do not call .stop from within the Routine.\n");
		post("A Routine should stop itself using nil.alwaysYield.\n");
		return errFailed;
	} else {
		error("Routine in unknown state. %d\n", state);
		return errFailed;
	}
	//assert(g->gc->SanityCheck());
	//CallStackSanity(g);
	return errNone;
}


int prRoutineYieldAndReset(struct VMGlobals *g, int numArgsPushed);
int prRoutineYieldAndReset(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a, *b, value;
	int state;
	
	//post("->prRoutineYieldAndReset\n");
	//assert(g->gc->SanityCheck());
	//CallStackSanity(g);
	a = g->sp - 1;
	b = g->sp;
	
	if (!isKindOf((PyrObject*)g->thread, class_routine)) {
		error ("yieldAndReset was called outside of a Routine.\n");
		return errFailed;
	}
	/*if (!g->thread->parent.uot) {
		error ("yieldAndReset was called from a thread with no parent.\n");
		return errFailed;
	}*/
	value.ucopy = a->ucopy;

	if (IsFalse(b)) state = tYieldToParent;
	else state = tInit;
	switchToThread(g, g->thread->parent.uot, state, &numArgsPushed);
	// on the other side of the looking glass, put the yielded value on the stack as the result..
	(g->sp - numArgsPushed + 1)->ucopy = value.ucopy;

	//post("<-prRoutineYieldAndReset\n");
	//assert(g->gc->SanityCheck());
	//CallStackSanity(g);
	return errNone;
}


void schedAdd(VMGlobals *g, PyrObject* inQueue, double inTime, PyrSlot* inTask);
void schedClearUnsafe();

int prSystemClock_Clear(struct VMGlobals *g, int numArgsPushed);
int prSystemClock_Clear(struct VMGlobals *g, int numArgsPushed)
{
	//PyrSlot *a = g->sp;

	schedClearUnsafe();

	return errNone;
}

int prSystemClock_Sched(struct VMGlobals *g, int numArgsPushed);
int prSystemClock_Sched(struct VMGlobals *g, int numArgsPushed)
{
	//PyrSlot *a = g->sp - 2;
	PyrSlot *b = g->sp - 1;
	PyrSlot *c = g->sp;

	double delta, time;
	int err = slotDoubleVal(b, &delta);
	if (err) return errNone; // return nil OK, just don't schedule
	err = slotDoubleVal(&g->thread->time, &time);
	if (err) return errNone; // return nil OK, just don't schedule
	time += delta;
	PyrObject* inQueue = g->process->sysSchedulerQueue.uo;
	schedAdd(g, inQueue, time, c);

	return errNone;
}

int prSystemClock_SchedAbs(struct VMGlobals *g, int numArgsPushed);
int prSystemClock_SchedAbs(struct VMGlobals *g, int numArgsPushed)
{
	//PyrSlot *a = g->sp - 2;
	PyrSlot *b = g->sp - 1;
	PyrSlot *c = g->sp;

	double time;
	int err = slotDoubleVal(b, &time);
	if (err) return errNone; // return nil OK, just don't schedule
	PyrObject* inQueue = g->process->sysSchedulerQueue.uo;
	schedAdd(g, inQueue, time, c);

	return errNone;
}

int prElapsedTime(struct VMGlobals *g, int numArgsPushed);
int prElapsedTime(struct VMGlobals *g, int numArgsPushed)
{
	SetFloat(g->sp, elapsedTime());
	return errNone;
}

bool gBlork = false;

int prBlork(struct VMGlobals *g, int numArgsPushed);
int prBlork(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a;

	a = g->sp;
	if (IsTrue(a)) gBlork = true;
	else gBlork = false;
	
	return errNone;
}
	

#define PRIMGROWSIZE 480
PrimitiveTable gPrimitiveTable;

void initPrimitiveTable()
{
	int i;
	gPrimitiveTable.maxsize = PRIMGROWSIZE;
	gPrimitiveTable.size = 0;
	// pyrmalloc: 
	// lifetime: runtime. primitives are reloaded when library is compiled.
	gPrimitiveTable.table = (PrimitiveDef*)pyr_pool_runtime->Alloc(gPrimitiveTable.maxsize * sizeof(PrimitiveDef));
	MEMFAIL(gPrimitiveTable.table);
	for (i=0; i<gPrimitiveTable.maxsize; ++i) {
		gPrimitiveTable.table[i].func = undefinedPrimitive;
		gPrimitiveTable.table[i].name = s_none;
		gPrimitiveTable.table[i].base = 0;
		gPrimitiveTable.table[i].numArgs = 0;
		gPrimitiveTable.table[i].varArgs = 0;
		gPrimitiveTable.table[i].keyArgs = 0;
	}
}

void growPrimitiveTable(int newsize)
{
	PrimitiveDef *oldtable;
	int i, oldsize;
	//postfl("growPrimitiveTable %d %d\n", oldsize, newsize);
	oldtable = gPrimitiveTable.table;
	oldsize = gPrimitiveTable.maxsize;
	gPrimitiveTable.maxsize = newsize;
	// pyrmalloc: 
	// lifetime: runtime. primitives are reloaded when library is compiled.
	gPrimitiveTable.table = (PrimitiveDef*)pyr_pool_runtime->Alloc(gPrimitiveTable.maxsize * sizeof(PrimitiveDef));
	MEMFAIL(gPrimitiveTable.table);
	memcpy(gPrimitiveTable.table, oldtable, oldsize * sizeof(PrimitiveDef));
	for (i=oldsize; i<gPrimitiveTable.maxsize; ++i) {
		gPrimitiveTable.table[i].func = undefinedPrimitive;
		gPrimitiveTable.table[i].name = s_none;
		gPrimitiveTable.table[i].base = 0;
		gPrimitiveTable.table[i].numArgs = 0;
		gPrimitiveTable.table[i].varArgs = 0;
		gPrimitiveTable.table[i].keyArgs = 0;
	}
	pyr_pool_runtime->Free(oldtable);
}

int definePrimitive(int base, int index, char *name, PrimitiveHandler handler,
	int numArgs, int varArgs)
{
	int tableIndex;
	PyrSymbol *sym;
	
	if (name[0] != '_') {
		error("*** Primitive Name must begin with an underscore ***\n");
		postfl("name: '%s' index: %d\n", name, index);
		return -1;
	}
	tableIndex = base + index;
	if (tableIndex < 0) {
		error("*** Negative Primitive Index ***\n");
		postfl("name: '%s' index: %d\n", name, tableIndex);
		return -1;
	}
	if (tableIndex >= gPrimitiveTable.maxsize) {
		growPrimitiveTable(tableIndex + PRIMGROWSIZE);
	}
	if (gPrimitiveTable.table[tableIndex].func != undefinedPrimitive) {
		error("*** Duplicate Primitive Index ***\n");
		postfl("name: '%s' index: %d\n", name, tableIndex);
		return -1;
	}
	sym = getsym(name);
	gPrimitiveTable.table[tableIndex].func = handler;
	gPrimitiveTable.table[tableIndex].name = sym;
	gPrimitiveTable.table[tableIndex].base = base;
	gPrimitiveTable.table[tableIndex].numArgs = numArgs;
	gPrimitiveTable.table[tableIndex].varArgs = varArgs;
	gPrimitiveTable.table[tableIndex].keyArgs = 0;
	if (tableIndex > gPrimitiveTable.size) gPrimitiveTable.size = tableIndex;
	sym->u.index = tableIndex;
	return tableIndex;
}

int definePrimitiveWithKeys(int base, int index, char *name, 
	PrimitiveHandler handler, PrimitiveWithKeysHandler keyhandler,
	int numArgs, int varArgs)
{
	int tableIndex;
	PyrSymbol *sym;
	
	if (name[0] != '_') {
		error("*** Primitive Name must begin with an underscore ***\n");
		postfl("name: '%s' index: %d\n", name, index);
		return -1;
	}
	tableIndex = base + index;
	if (tableIndex < 0) {
		error("*** Negative Primitive Index ***\n");
		postfl("name: '%s' index: %d\n", name, tableIndex);
		return -1;
	}
	if (tableIndex+1 >= gPrimitiveTable.maxsize) {
		growPrimitiveTable(tableIndex + PRIMGROWSIZE);
	}
	if (gPrimitiveTable.table[tableIndex].func != undefinedPrimitive) {
		error("*** Duplicate Primitive Index ***\n");
		postfl("name: '%s' index: %d\n", name, tableIndex);
		return -1;
	}
	sym = getsym(name);
	gPrimitiveTable.table[tableIndex].func = handler;
	gPrimitiveTable.table[tableIndex].name = sym;
	gPrimitiveTable.table[tableIndex].base = base;
	gPrimitiveTable.table[tableIndex].numArgs = numArgs;
	gPrimitiveTable.table[tableIndex].varArgs = varArgs;
	gPrimitiveTable.table[tableIndex].keyArgs = 1;
	sym->u.index = tableIndex;

	tableIndex++;
	gPrimitiveTable.table[tableIndex].func = (PrimitiveHandler)keyhandler;
	gPrimitiveTable.table[tableIndex].name = sym;
	gPrimitiveTable.table[tableIndex].base = base;
	gPrimitiveTable.table[tableIndex].numArgs = numArgs;
	gPrimitiveTable.table[tableIndex].varArgs = varArgs;
	gPrimitiveTable.table[tableIndex].keyArgs = 1;
	if (tableIndex > gPrimitiveTable.size) gPrimitiveTable.size = tableIndex;
	return tableIndex;
}

int nextPrimitiveIndex()
{
	return gPrimitiveTable.size + 1;
}


void doPrimitive(VMGlobals* g, PyrMethod* meth, int numArgsPushed)
{
	
#if SANITYCHECK
	g->gc->SanityCheck();
#endif	
	
	//post("doPrimitive %s-%s\n", meth->ownerclass.uoc->name.us->name, meth->name.us->name);
        //printf("doPrimitive %s-%s\n", meth->ownerclass.uoc->name.us->name, meth->name.us->name);

	PyrMethodRaw *methraw = METHRAW(meth);
	int primIndex = methraw->specialIndex;
	
	PrimitiveDef *def = gPrimitiveTable.table + primIndex;
	int numArgsNeeded = def->numArgs;
	int diff = numArgsNeeded - numArgsPushed;
	
	if (diff != 0) { // incorrect num of args
		if (diff > 0) {  // not enough args
			double* pslot = (double*)(g->sp);
			double* qslot = (double*)(meth->prototypeFrame.uo->slots + numArgsPushed - 1);
			for (int m=0; m<diff; ++m) *++pslot = *++qslot;
			
			g->sp += diff;
		} else if (def->varArgs) { // has var args
			numArgsNeeded = numArgsPushed;
		} else {
			g->sp += numArgsNeeded - numArgsPushed; // remove excess args
		}
	}
	g->numpop = numArgsNeeded - 1;
	g->primitiveIndex = primIndex - def->base;
	g->primitiveMethod = meth;
	g->args = g->sp - numArgsNeeded;
	int err;
	try {
#if SANITYCHECK
	g->gc->SanityCheck();
#endif	
		err = (*def->func)(g, numArgsNeeded);
#if SANITYCHECK
	g->gc->SanityCheck();
#endif	
	} catch (std::exception& ex) {
		post("caught exception in primitive %s-%s\n", meth->ownerclass.uoc->name.us->name, meth->name.us->name);
		error(ex.what());
		err = errException;
	} catch (...) {
		post("caught exception in primitive %s-%s\n", meth->ownerclass.uoc->name.us->name, meth->name.us->name);
		err = errException;
	}
	if (err <= errNone) g->sp -= g->numpop;
	else {
		//post("primitive failed %s-%s\n", meth->ownerclass.uoc->name.us->name, meth->name.us->name);
		SetInt(&g->thread->primitiveIndex, methraw->specialIndex);
		SetInt(&g->thread->primitiveError, err);
		executeMethod(g, meth, numArgsNeeded);
	}
#if SANITYCHECK
	g->gc->SanityCheck();
#endif	
}

void doPrimitiveWithKeys(VMGlobals* g, PyrMethod* meth, int allArgsPushed, int numKeyArgsPushed)
{
	int i, j, m, diff, err;
	double *pslot, *qslot;
	int numArgsNeeded, numArgsPushed;
	
#if SANITYCHECK
	g->gc->SanityCheck();
#endif	
	//post("doPrimitive %s-%s\n", meth->ownerclass.uoc->name.us->name, meth->name.us->name);
        //printf("doPrimitive %s-%s\n", meth->ownerclass.uoc->name.us->name, meth->name.us->name);

	PyrMethodRaw *methraw = METHRAW(meth);
	int primIndex = methraw->specialIndex;
	PrimitiveDef *def = gPrimitiveTable.table + primIndex;
	g->primitiveIndex = primIndex - def->base;
	g->primitiveMethod = meth;
	
	if (def->keyArgs && numKeyArgsPushed) {
		g->numpop = allArgsPushed - 1;
		try {
			err = ((PrimitiveWithKeysHandler)def[1].func)(g, allArgsPushed, numKeyArgsPushed);
		} catch (std::exception& ex) {
			post("caught exception in primitive %s-%s\n", meth->ownerclass.uoc->name.us->name, meth->name.us->name);
			error(ex.what());
			err = errException;
		} catch (...) {
			post("caught exception in primitive %s-%s\n", meth->ownerclass.uoc->name.us->name, meth->name.us->name);
			err = errException;
		}
	} else {
		numArgsNeeded = def->numArgs;
		numArgsPushed = allArgsPushed - (numKeyArgsPushed << 1);
		
		if (numKeyArgsPushed) {
			// evacuate keyword args to separate area
			pslot = (double*)(keywordstack + (numKeyArgsPushed<<1));
			qslot = (double*)(g->sp + 1);
			for (m=0; m<numKeyArgsPushed; ++m) {
				*--pslot = *--qslot;
				*--pslot = *--qslot;
			}
		}
		
		diff = numArgsNeeded - numArgsPushed;
		if (diff != 0) { // incorrect num of args
			if (diff > 0) {  // not enough args
				g->sp += numArgsNeeded - allArgsPushed; // remove excess args
				pslot = (double*)(g->sp - diff);
				qslot = (double*)(meth->prototypeFrame.uo->slots + numArgsPushed - 1);
				for (m=0; m<diff; ++m) *++pslot = *++qslot;
			} else if (def->varArgs) { // has var args
				numArgsNeeded = numArgsPushed;
				g->sp += numArgsNeeded - allArgsPushed; // remove excess args
			} else {
				g->sp += numArgsNeeded - allArgsPushed; // remove excess args
			}
		}

		// do keyword lookup:
		if (numKeyArgsPushed && methraw->posargs) {
			PyrSymbol **name0, **name;
			PyrSlot *key, *vars;
			name0 = meth->argNames.uosym->symbols + 1;
			key = keywordstack;
			vars = g->sp - numArgsNeeded + 1;
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
		g->numpop = numArgsNeeded - 1;
		try {
			err = (*def->func)(g, numArgsNeeded);
		} catch (std::exception& ex) {
			post("caught exception in primitive %s-%s\n", meth->ownerclass.uoc->name.us->name, meth->name.us->name);
			error(ex.what());
			err = errException;
		} catch (...) {
			post("caught exception in primitive %s-%s\n", meth->ownerclass.uoc->name.us->name, meth->name.us->name);
			err = errException;
		}
	}
	if (err <= errNone) g->sp -= g->numpop;
	else {
		//post("primerr %d\n", err);
		SetInt(&g->thread->primitiveIndex, methraw->specialIndex);
		SetInt(&g->thread->primitiveError, err);
		executeMethod(g, meth, numArgsNeeded);
	}
#if SANITYCHECK
	g->gc->SanityCheck();
#endif	
}

void initPrimitives()
{
	int base, index;
	
	initPrimitiveTable();
	
	// unary operators
	base = nextPrimitiveIndex();
	definePrimitive(base, opNeg, "_Neg", doSpecialUnaryArithMsg, 1, 0);
	definePrimitive(base, opBitNot, "_BitNot", doSpecialUnaryArithMsg, 1, 0);
	definePrimitive(base, opAbs, "_Abs", doSpecialUnaryArithMsg, 1, 0);
	definePrimitive(base, opAsFloat, "_AsFloat", doSpecialUnaryArithMsg, 1, 0);
	definePrimitive(base, opAsInt, "_AsInt", doSpecialUnaryArithMsg, 1, 0);
	definePrimitive(base, opCeil, "_Ceil", doSpecialUnaryArithMsg, 1, 0);			// 5
	definePrimitive(base, opFloor, "_Floor", doSpecialUnaryArithMsg, 1, 0);
	definePrimitive(base, opFrac, "_Frac", doSpecialUnaryArithMsg, 1, 0);
	definePrimitive(base, opSign, "_Sign", doSpecialUnaryArithMsg, 1, 0);
	definePrimitive(base, opSquared, "_Squared", doSpecialUnaryArithMsg, 1, 0);
	definePrimitive(base, opCubed, "_Cubed", doSpecialUnaryArithMsg, 1, 0);			//10
	definePrimitive(base, opSqrt, "_Sqrt", doSpecialUnaryArithMsg, 1, 0);		
	definePrimitive(base, opExp, "_Exp", doSpecialUnaryArithMsg, 1, 0);
	definePrimitive(base, opRecip, "_Recip", doSpecialUnaryArithMsg, 1, 0);
	definePrimitive(base, opMIDICPS, "_MIDICPS", doSpecialUnaryArithMsg, 1, 0);
	definePrimitive(base, opCPSMIDI, "_CPSMIDI", doSpecialUnaryArithMsg, 1, 0);		//15
	
	definePrimitive(base, opMIDIRatio, "_MIDIRatio", doSpecialUnaryArithMsg, 1, 0);		
	definePrimitive(base, opRatioMIDI, "_RatioMIDI", doSpecialUnaryArithMsg, 1, 0);		
	definePrimitive(base, opDbAmp, "_DbAmp", doSpecialUnaryArithMsg, 1, 0);
	definePrimitive(base, opAmpDb, "_AmpDb", doSpecialUnaryArithMsg, 1, 0);	
	definePrimitive(base, opOctCPS, "_OctCPS", doSpecialUnaryArithMsg, 1, 0);
	definePrimitive(base, opCPSOct, "_CPSOct", doSpecialUnaryArithMsg, 1, 0);
	definePrimitive(base, opLog, "_Log", doSpecialUnaryArithMsg, 1, 0);				
	definePrimitive(base, opLog2, "_Log2", doSpecialUnaryArithMsg, 1, 0);
	definePrimitive(base, opLog10, "_Log10", doSpecialUnaryArithMsg, 1, 0);
	definePrimitive(base, opSin, "_Sin", doSpecialUnaryArithMsg, 1, 0);
	definePrimitive(base, opCos, "_Cos", doSpecialUnaryArithMsg, 1, 0);
	definePrimitive(base, opTan, "_Tan", doSpecialUnaryArithMsg, 1, 0);				
	definePrimitive(base, opArcSin, "_ArcSin", doSpecialUnaryArithMsg, 1, 0);
	definePrimitive(base, opArcCos, "_ArcCos", doSpecialUnaryArithMsg, 1, 0);
	definePrimitive(base, opArcTan, "_ArcTan", doSpecialUnaryArithMsg, 1, 0);
	definePrimitive(base, opSinH, "_SinH", doSpecialUnaryArithMsg, 1, 0);
	definePrimitive(base, opCosH, "_CosH", doSpecialUnaryArithMsg, 1, 0);			
	definePrimitive(base, opTanH, "_TanH", doSpecialUnaryArithMsg, 1, 0);
	definePrimitive(base, opRand, "_Rand", doSpecialUnaryArithMsg, 1, 0);
	definePrimitive(base, opRand2, "_Rand2", doSpecialUnaryArithMsg, 1, 0);
	definePrimitive(base, opLinRand, "_LinRand", doSpecialUnaryArithMsg, 1, 0);
	definePrimitive(base, opBiLinRand, "_BiLinRand", doSpecialUnaryArithMsg, 1, 0);

	definePrimitive(base, opSum3Rand, "_Sum3Rand", doSpecialUnaryArithMsg, 1, 0);
//	definePrimitive(base, opExpRand, "_ExpRand", doSpecialUnaryArithMsg, 1, 0);
//	definePrimitive(base, opBiExpRand, "_BiExpRand", doSpecialUnaryArithMsg, 1, 0);
//	definePrimitive(base, opGammaRand, "_GammaRand", doSpecialUnaryArithMsg, 1, 0);
//	definePrimitive(base, opGaussRand, "_GaussRand", doSpecialUnaryArithMsg, 1, 0);
//	definePrimitive(base, opPoiRand, "_PoiRand", doSpecialUnaryArithMsg, 1, 0);

	definePrimitive(base, opDistort, "_Distort", doSpecialUnaryArithMsg, 1, 0);
	definePrimitive(base, opSoftClip, "_SoftClip", doSpecialUnaryArithMsg, 1, 0);
	definePrimitive(base, opCoin, "_Coin", doSpecialUnaryArithMsg, 1, 0);

	definePrimitive(base, opRectWindow, "_RectWindow", doSpecialUnaryArithMsg, 1, 0);
	definePrimitive(base, opHanWindow, "_HanWindow", doSpecialUnaryArithMsg, 1, 0);
	definePrimitive(base, opWelchWindow, "_WelchWindow", doSpecialUnaryArithMsg, 1, 0);
	definePrimitive(base, opTriWindow, "_TriWindow", doSpecialUnaryArithMsg, 1, 0);

	definePrimitive(base, opSCurve, "_SCurve", doSpecialUnaryArithMsg, 1, 0);
	definePrimitive(base, opRamp, "_Ramp", doSpecialUnaryArithMsg, 1, 0);

	definePrimitive(base, opDigitValue, "_DigitValue", doSpecialUnaryArithMsg, 1, 0);	
	
	
	// binary operators
	base = nextPrimitiveIndex();
	definePrimitive(base, opAdd, "_Add", prAddNum, 2, 0);		
	definePrimitive(base, opSub, "_Sub", prSubNum, 2, 0); 
	definePrimitive(base, opMul, "_Mul", prMulNum, 2, 0); 

	definePrimitive(base, opIDiv, "_IDiv", doSpecialBinaryArithMsg, 2, 0); 
	definePrimitive(base, opFDiv, "_FDiv", doSpecialBinaryArithMsg, 2, 0); 
	definePrimitive(base, opMod, "_Mod", doSpecialBinaryArithMsg, 2, 0);
	definePrimitive(base, opEQ, "_EQ", doSpecialBinaryArithMsg, 2, 0);		
	definePrimitive(base, opNE, "_NE", doSpecialBinaryArithMsg, 2, 0); 
	definePrimitive(base, opLT, "_LT", doSpecialBinaryArithMsg, 2, 0); 
	definePrimitive(base, opGT, "_GT", doSpecialBinaryArithMsg, 2, 0); 
	definePrimitive(base, opLE, "_LE", doSpecialBinaryArithMsg, 2, 0);
	definePrimitive(base, opGE, "_GE", doSpecialBinaryArithMsg, 2, 0);
	//definePrimitive(base, opIdentical, "_Identical", doSpecialBinaryArithMsg, 2, 0);
	//definePrimitive(base, opNotIdentical, "_NotIdentical", doSpecialBinaryArithMsg, 2, 0);	
	
	definePrimitive(base, opMin, "_Min", doSpecialBinaryArithMsg, 2, 0); 
	definePrimitive(base, opMax, "_Max", doSpecialBinaryArithMsg, 2, 0);
	definePrimitive(base, opBitAnd, "_BitAnd", doSpecialBinaryArithMsg, 2, 0);
	definePrimitive(base, opBitOr, "_BitOr", doSpecialBinaryArithMsg, 2, 0);
	definePrimitive(base, opBitXor, "_BitXor", doSpecialBinaryArithMsg, 2, 0);	
	definePrimitive(base, opLCM, "_LCM", doSpecialBinaryArithMsg, 2, 0);
	definePrimitive(base, opGCD, "_GCD", doSpecialBinaryArithMsg, 2, 0);
	definePrimitive(base, opRound, "_Round", doSpecialBinaryArithMsg, 2, 0);
	definePrimitive(base, opTrunc, "_Trunc", doSpecialBinaryArithMsg, 2, 0);
	definePrimitive(base, opAtan2, "_Atan2", doSpecialBinaryArithMsg, 2, 0);	
	definePrimitive(base, opHypot, "_Hypot", doSpecialBinaryArithMsg, 2, 0);
	definePrimitive(base, opHypotx, "_HypotApx", doSpecialBinaryArithMsg, 2, 0);
	definePrimitive(base, opPow, "_Pow", doSpecialBinaryArithMsg, 2, 0);
	definePrimitive(base, opShiftLeft, "_ShiftLeft", doSpecialBinaryArithMsg, 2, 0);
	definePrimitive(base, opShiftRight, "_ShiftRight", doSpecialBinaryArithMsg, 2, 0);
	definePrimitive(base, opUnsignedShift, "_UnsignedShift", doSpecialBinaryArithMsg, 2, 0);
	definePrimitive(base, opFill, "_Fill", doSpecialBinaryArithMsg, 2, 0);
	definePrimitive(base, opRing1, "_Ring1", doSpecialBinaryArithMsg, 2, 0);	// a * (b + 1) == a * b + a
	definePrimitive(base, opRing2, "_Ring2", doSpecialBinaryArithMsg, 2, 0);	// a * b + a + b
	definePrimitive(base, opRing3, "_Ring3", doSpecialBinaryArithMsg, 2, 0);	// a*a*b
	definePrimitive(base, opRing4, "_Ring4", doSpecialBinaryArithMsg, 2, 0);	// a*a*b - a*b*b
	definePrimitive(base, opDifSqr, "_DifSqr", doSpecialBinaryArithMsg, 2, 0);	// a*a - b*b
	definePrimitive(base, opSumSqr, "_SumSqr", doSpecialBinaryArithMsg, 2, 0);	// a*a + b*b
	definePrimitive(base, opSqrSum, "_SqrSum", doSpecialBinaryArithMsg, 2, 0);	// (a + b)^2
	definePrimitive(base, opSqrDif, "_SqrDif", doSpecialBinaryArithMsg, 2, 0);	// (a - b)^2
	definePrimitive(base, opAbsDif, "_AbsDif", doSpecialBinaryArithMsg, 2, 0);	// abs(a - b)
	definePrimitive(base, opThresh, "_Thresh", doSpecialBinaryArithMsg, 2, 0);	// a * max(0,b)
	definePrimitive(base, opAMClip, "_AMClip", doSpecialBinaryArithMsg, 2, 0);	// a * max(0,b)
	definePrimitive(base, opScaleNeg, "_ScaleNeg", doSpecialBinaryArithMsg, 2, 0);	// a < 0 ? a*b : a
	definePrimitive(base, opClip2, "_Clip2", doSpecialBinaryArithMsg, 2, 0);	
	definePrimitive(base, opFold2, "_Fold2", doSpecialBinaryArithMsg, 2, 0);	
	definePrimitive(base, opWrap2, "_Wrap2", doSpecialBinaryArithMsg, 2, 0);	
	definePrimitive(base, opExcess, "_Excess", doSpecialBinaryArithMsg, 2, 0);	
	definePrimitive(base, opFirstArg, "_FirstArg", doSpecialBinaryArithMsg, 2, 0);	
	definePrimitive(base, opRandRange, "_RandRange", doSpecialBinaryArithMsg, 2, 0);
	definePrimitive(base, opExpRandRange, "_ExpRandRange", doSpecialBinaryArithMsg, 2, 0);

	// general primitives
	base = nextPrimitiveIndex();
	index = 0;
	definePrimitive(base, index++, "_Halt", haltInterpreter, 0, 0);
	definePrimitive(base, index++, "_InstVarAt", instVarAt, 2, 0);
	definePrimitive(base, index++, "_InstVarPut", instVarPut, 3, 0);
	definePrimitive(base, index++, "_InstVarSize", instVarSize, 1, 0);
	definePrimitive(base, index++, "_ObjectHash", objectHash, 1, 0);
	definePrimitive(base, index++, "_ObjectClass", objectClass, 1, 0);
	definePrimitive(base, index++, "_BasicNew", basicNew, 2, 0);
	definePrimitive(base, index++, "_BasicNewClear", basicNewClear, 2, 0);
	definePrimitive(base, index++, "_BasicNewCopyArgsToInstVars", basicNewCopyArgsToInstanceVars, 1, 1);
	//definePrimitive(base, index++, "_BasicNewCopyArgsByName", basicNewCopyArgsByName, 1, 1);
	
	definePrimitiveWithKeys(base, index, "_FunctionValue", blockValue, blockValueWithKeys, 1, 1);
	index+=2;
	definePrimitiveWithKeys(base, index, "_FunctionValueEnvir", blockValueEnvir, blockValueEnvirWithKeys, 1, 1);
	index+=2;
	
	definePrimitive(base, index++, "_FunctionValueArray", blockValueArray, 1, 1);
	definePrimitive(base, index++, "_FunctionValueArrayEnvir", blockValueArrayEnvir, 1, 1);
	definePrimitive(base, index++, "_FunctionDefAsFunction", prFunctionDefAsFunction, 1, 0);
	definePrimitive(base, index++, "_FunctionDefDumpContexts", prFunctionDefDumpContexts, 1, 0);
	definePrimitive(base, index++, "_FunctionDefIsClosed", prFunctionDefIsClosed, 1, 0);
	definePrimitive(base, index++, "_FunctionDefIsWithinClosed", prFunctionDefIsWithinClosed, 1, 0);
	
	definePrimitive(base, index++, "_ObjectIsKindOf", objectIsKindOf, 2, 0);
	definePrimitive(base, index++, "_ObjectIsMemberOf", objectIsMemberOf, 2, 0);
	definePrimitive(base, index++, "_ObjectDump", objectDump, 1, 0);
	definePrimitive(base, index++, "_TotalFree", prTotalFree, 1, 0);
	definePrimitive(base, index++, "_LargestFreeBlock", prLargestFreeBlock, 1, 0);

	definePrimitive(base, index++, "_GCInfo", dumpGCinfo, 1, 0);
	definePrimitive(base, index++, "_GCAll", dumpGCAll, 1, 0);
	definePrimitive(base, index++, "_GCSanity", prGCSanity, 1, 0);
#if GCDEBUG
	definePrimitive(base, index++, "_TraceAllPathsTo", prTraceAllPathsTo, 1, 0);
	definePrimitive(base, index++, "_TraceAnyPathsTo", prTraceAnyPathsTo, 1, 0);
	definePrimitive(base, index++, "_TraceAnyPathToAllInstancesOf", prTraceAnyPathToAllInstancesOf, 1, 0);
#endif
	
	definePrimitive(base, index++, "_Identical", objectIdentical, 2, 0);
	definePrimitive(base, index++, "_NotIdentical", objectNotIdentical, 2, 0);	
	definePrimitiveWithKeys(base, index, "_ObjectPerform", objectPerform, objectPerformWithKeys, 2, 1);	
	index+=2;
	definePrimitive(base, index++, "_ObjectPerformList", objectPerformList, 2, 1);	
	definePrimitive(base, index++, "_ObjectPerformMsg", objectPerformSelList, 2, 0);
	//definePrimitive(base, index++, "_ArrayPerformMsg", arrayPerformMsg, 1, 1);
	definePrimitive(base, index++, "_ObjectString", prObjectString, 1, 0);	
	definePrimitive(base, index++, "_Float_AsStringPrec", prFloat_AsStringPrec, 2, 0);	
	definePrimitive(base, index++, "_ObjectCompileString", prAsCompileString, 1, 0);	
	definePrimitive(base, index++, "_ClassString", prClassString, 1, 0);	
	definePrimitive(base, index++, "_PostString", prPostString, 1, 0);	
	definePrimitive(base, index++, "_PostLine", prPostLine, 1, 0);	
	definePrimitive(base, index++, "_HostDebugger", prDebugger, 1, 0);	
	definePrimitive(base, index++, "_Trace", prTraceOn, 1, 0);
	definePrimitive(base, index++, "_KeywordError", prKeywordError, 1, 0);

	definePrimitive(base, index++, "_PrimitiveError", prPrimitiveError, 1, 0);	
	definePrimitive(base, index++, "_PrimitiveErrorString", prPrimitiveErrorString, 1, 0);	
	definePrimitive(base, index++, "_DumpStack", prDumpStack, 1, 0);	
	definePrimitive(base, index++, "_DumpDetailedBackTrace", prDumpDetailedBackTrace, 1, 0);	
	definePrimitive(base, index++, "_StackDepth", prStackDepth, 1, 0);	
	definePrimitive(base, index++, "_PrimName", prPrimName, 1, 0);	
	definePrimitive(base, index++, "_ObjectShallowCopy", prObjectShallowCopy, 1, 0);	
	definePrimitive(base, index++, "_ObjectCopyImmutable", prObjectCopyImmutable, 1, 0);	
	definePrimitive(base, index++, "_ObjectCopyRange", prObjectCopyRange, 3, 0);	
	definePrimitive(base, index++, "_ObjectPointsTo", prObjectPointsTo, 2, 0);	
	definePrimitive(base, index++, "_ObjectRespondsTo", prObjectRespondsTo, 2, 0);	
	definePrimitive(base, index++, "_ObjectIsMutable", prObjectIsMutable, 1, 0);
	definePrimitive(base, index++, "_ObjectIsPermanent", prObjectIsPermanent, 1, 0);
	definePrimitive(base, index++, "_ObjectDeepFreeze", prDeepFreeze, 1, 0);
	definePrimitive(base, index++, "_ObjectDeepCopy", prDeepCopy, 1, 0);

#if !SCPLAYER	
	definePrimitive(base, index++, "_CompileExpression", prCompileString, 2, 0);	
#endif
	definePrimitive(base, index++, "_GetBackTrace", prGetBackTrace, 1, 0);	
	definePrimitive(base, index++, "_DumpBackTrace", prDumpBackTrace, 1, 0);	
	definePrimitive(base, index++, "_DumpByteCodes", prDumpByteCodes, 1, 0);	

	definePrimitive(base, index++, "_AllClasses", prAllClasses, 1, 0);	
	definePrimitive(base, index++, "_DumpClassSubtree", prPostClassTree, 1, 0);	

//	definePrimitive(base, index++, "_TabletTracking", prTabletTracking, 1, 0);	

	definePrimitive(base, index++, "_FunDef_NumArgs", prFunDef_NumArgs, 1, 0);	
	definePrimitive(base, index++, "_FunDef_NumVars", prFunDef_NumVars, 1, 0);	
	definePrimitive(base, index++, "_FunDef_VarArgs", prFunDef_VarArgs, 1, 0);	

	definePrimitive(base, index++, "_Thread_Init", prThreadInit, 3, 0);	
	definePrimitive(base, index++, "_Thread_RandSeed", prThreadRandSeed, 2, 0);	
//	definePrimitive(base, index++, "_ThreadRun", prThreadRun, 2, 0);	
//	definePrimitive(base, index++, "_RunNextThread", prRunNextThread, 1, 0);	
	definePrimitive(base, index++, "_RoutineYield", prRoutineYield, 1, 0);	
	definePrimitive(base, index++, "_RoutineAlwaysYield", prRoutineAlwaysYield, 1, 0);	
	definePrimitive(base, index++, "_RoutineResume", prRoutineResume, 2, 0);	
	definePrimitive(base, index++, "_RoutineReset", prRoutineReset, 1, 0);	
	definePrimitive(base, index++, "_RoutineYieldAndReset", prRoutineYieldAndReset, 2, 0);	
	definePrimitive(base, index++, "_RoutineStop", prRoutineStop, 1, 0);	
	definePrimitive(base, index++, "_SystemClock_Clear", prSystemClock_Clear, 1, 0);	
	definePrimitive(base, index++, "_SystemClock_Sched", prSystemClock_Sched, 3, 0);	
	definePrimitive(base, index++, "_SystemClock_SchedAbs", prSystemClock_SchedAbs, 3, 0);	
	definePrimitive(base, index++, "_ElapsedTime", prElapsedTime, 1, 0);	
	
//	definePrimitive(base, index++, "_IsDemo", prIsDemo, 1, 0);	
	definePrimitive(base, index++, "_Blork", prBlork, 1, 0);	
	definePrimitive(base, index++, "_UGenCodeString", prUGenCodeString, 5, 0);	

	//void initOscilPrimitives();
	//void initControllerPrimitives();
	
	//initOscilPrimitives();
	//initControllerPrimitives();
	initMathPrimitives();
	initSignalPrimitives();
	initArrayPrimitives();
	
void initSymbolPrimitives();
	initSymbolPrimitives();

void initArchiverPrimitives();
	 initArchiverPrimitives();

void initArrayPrimitives();
	 initArrayPrimitives();

void initBitPrimitives();
	 initBitPrimitives();

void initCharPrimitives();
	 initCharPrimitives();

void initFilePrimitives();
	 initFilePrimitives();

void initStringPrimitives();
	 initStringPrimitives();

void initListPrimitives();
	 initListPrimitives();
	
void initUnixPrimitives();
	 initUnixPrimitives();

void init_OSC_primitives();
	 init_OSC_primitives();

/*  these probably should be moved out of the Lang code
into an App init primitives section */
void initGUIPrimitives();
        initGUIPrimitives();

void initSCViewPrimitives();
        initSCViewPrimitives();

void initMIDIPrimitives();
		initMIDIPrimitives();

void initSpeechPrimitives();
		initSpeechPrimitives();     
                
void initCocoaFilePrimitives();
        initCocoaFilePrimitives();

// CR ADDED
void initRendezvousPrimitives();
	initRendezvousPrimitives();

	s_recvmsg = getsym("receiveMsg");
	post("\tNumPrimitives = %d\n", nextPrimitiveIndex());
	
}


void initThreads();
void initThreads()
{
	s_prrunnextthread = getsym("prRunNextThread");
	s_prready = getsym("prReady");
}


