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

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "PyrKernel.h"
#include "PyrObject.h"
#include "PyrPrimitive.h"
#include "PyrPrimitiveProto.h"
#include "PyrSignal.h"
#include "PyrSched.h"
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
#include "SC_LanguageConfig.hpp"
#include "SC_Filesystem.hpp"
#include "SC_Version.hpp"
#include "SC_PrimRegistry.hpp"
#include <map>

#ifdef _WIN32
# include <direct.h>
#else
# include <sys/param.h>
#endif

#ifdef SC_QT
#  include "QtCollider.h"
#endif

#include <boost/filesystem/path.hpp> // path

#ifdef __clang__
#pragma clang diagnostic ignored "-Warray-bounds"
#endif

// ------ sclang primitive groups ----------
// This ensures that by the time initPrimitives() is called, each group's TU has been initialized
// and its primitives and symbols registered in the registries found in SC_PrimRegistry.hpp.

INIT_LIBSCLANG_PRIMITIVE_GROUP( Archiver );
INIT_LIBSCLANG_PRIMITIVE_GROUP( Array );
INIT_LIBSCLANG_PRIMITIVE_GROUP( Bit );
INIT_LIBSCLANG_PRIMITIVE_GROUP( Char );
INIT_LIBSCLANG_PRIMITIVE_GROUP( File );
INIT_LIBSCLANG_PRIMITIVE_GROUP( List );
INIT_LIBSCLANG_PRIMITIVE_GROUP( Math );
INIT_LIBSCLANG_PRIMITIVE_GROUP( OSCData );
INIT_LIBSCLANG_PRIMITIVE_GROUP( Platform );
INIT_LIBSCLANG_PRIMITIVE_GROUP( SCDoc );
INIT_LIBSCLANG_PRIMITIVE_GROUP( Sched );
INIT_LIBSCLANG_PRIMITIVE_GROUP( Signal );
INIT_LIBSCLANG_PRIMITIVE_GROUP( String );
INIT_LIBSCLANG_PRIMITIVE_GROUP( Symbol );
INIT_LIBSCLANG_PRIMITIVE_GROUP( Unix );

#if !defined(_WIN32) && !defined(SC_IPHONE) && !defined(__OpenBSD__) && !defined(__NetBSD__)
INIT_LIBSCLANG_PRIMITIVE_GROUP( Serial );
#endif

#ifdef HAVE_WII
INIT_LIBSCLANG_PRIMITIVE_GROUP( Wii );
#endif

#ifdef __APPLE__
INIT_LIBSCLANG_PRIMITIVE_GROUP( CoreAudio );
INIT_LIBSCLANG_PRIMITIVE_GROUP( Speech );
#endif

#ifdef __linux__
INIT_LIBSCLANG_PRIMITIVE_GROUP( LID );
#endif

#if defined(__APPLE__) || defined(HAVE_ALSA) || defined(HAVE_PORTMIDI)
INIT_LIBSCLANG_PRIMITIVE_GROUP( MIDI );
#endif

#ifdef SC_IDE
INIT_LIBSCLANG_PRIMITIVE_GROUP( ScIDE );
#endif

#ifdef SC_HIDAPI
INIT_LIBSCLANG_PRIMITIVE_GROUP( HID );
#endif

// ----------------------------------------

namespace bfs = boost::filesystem;

int yyparse();

extern bool gTraceInterpreter;

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


int getPrimitiveNumArgs(int index)
{
	return gPrimitiveTable.table[index].numArgs;
}

PyrSymbol* getPrimitiveName(int index)
{
	return gPrimitiveTable.table[index].name;
}

int slotStrLen(PyrSlot *slot)
{
	if (IsSym(slot))
		return slotRawSymbol(slot)->length;
	if (isKindOfSlot(slot, class_string))
		return slotRawObject(slot)->size;

	return -1;
}

int slotStrVal(PyrSlot *slot, char *str, int maxlen)
{
	if (IsSym(slot)) {
		strncpy(str, slotRawSymbol(slot)->name, maxlen);
		return errNone;
	} else if (isKindOfSlot(slot, class_string)) {
		int len;
		len = sc_min(maxlen-1, slotRawObject(slot)->size);
		memcpy(str, slotRawString(slot)->s, len);
		str[len] = 0;
		return errNone;
	}
	return errWrongType;
}

int slotPStrVal(PyrSlot *slot, unsigned char *str)
{
	if (IsSym(slot)) {
		strncpy((char*)str+1, slotRawSymbol(slot)->name, 255);
		str[0] = slotRawSymbol(slot)->length;
		return errNone;
	} else if (isKindOfSlot(slot, class_string)) {
		int len;
		len = sc_min(255, slotRawObject(slot)->size);
		memcpy(str+1, slotRawString(slot)->s, len);
		str[0] = len;
		return errNone;
	}
	return errWrongType;
}

int blockValueEnvir(struct VMGlobals *g, int numArgsPushed);

SCLANG_DEFINE_PRIMITIVE( InstVarAt, 2 )
{
	PyrSlot *a, *b;
	int index;

	a = g->sp - 1;
	b = g->sp;

	if (NotObj(a)) return errWrongType;

	PyrObject *obj = slotRawObject(a);

	if (IsInt(b)) {
		index = slotRawInt(b);
		if (index < 0 || index >= obj->size) return errIndexOutOfRange;
		slotCopy(a,&obj->slots[index]);
	} else if (IsSym(b)) {
		PyrSlot *instVarNamesSlot = &obj->classptr->instVarNames;
		if (!isKindOfSlot(instVarNamesSlot, class_symbolarray)) return errFailed;
		PyrSymbolArray *instVarNames = slotRawSymbolArray(instVarNamesSlot);
		PyrSymbol **names = instVarNames->symbols;
		PyrSymbol *name = slotRawSymbol(b);
		for (int i=0; i<instVarNames->size; ++i) {
			if (names[i] == name) {
				slotCopy(a,&obj->slots[i]);
				return errNone;
			}
		}
		return errFailed;
	} else return errWrongType;
	return errNone;
}

SCLANG_DEFINE_PRIMITIVE( InstVarPut, 3 )
{
	PyrSlot *a, *b, *c, *slot;
	int index;
	PyrObject *obj;

	a = g->sp - 2;
	b = g->sp - 1;
	c = g->sp;

	if (NotObj(a)) return errWrongType;
	obj = slotRawObject(a);
	if (obj->IsImmutable()) return errImmutableObject;

	if (IsInt(b)) {
		index = slotRawInt(b);
		if (index < 0 || index >= obj->size) return errIndexOutOfRange;
		slot = obj->slots + index;
		slotCopy(slot,c);
		g->gc->GCWrite(obj, slot);
	} else if (IsSym(b)) {
		PyrSlot *instVarNamesSlot = &obj->classptr->instVarNames;
		if (!IsObj(instVarNamesSlot)) return errFailed;
		PyrSymbolArray *instVarNames = slotRawSymbolArray(instVarNamesSlot);
		PyrSymbol **names = instVarNames->symbols;
		PyrSymbol *name = slotRawSymbol(b);
		for (int i=0; i<instVarNames->size; ++i) {
			if (names[i] == name) {
				slot = obj->slots + i;
				slotCopy(slot,c);
				g->gc->GCWrite(obj, slot);
				return errNone;
			}
		}
		post("WARNING: %s instVarPut '%s' failed.\n", slotRawSymbol(&obj->classptr->name)->name, name->name);
		return errNone;
	} else return errWrongType;
	return errNone;
}

SCLANG_DEFINE_PRIMITIVE( InstVarSize, 1 )
{
	PyrSlot *a;
	PyrObject *obj;

	a = g->sp;
	if (NotObj(a)) {
		SetInt(a, 0);
		return errNone;
	}
	obj = slotRawObject(a);
	if (obj->obj_format == obj_notindexed) {
		SetInt(a, obj->size);
	} else {
		SetInt(a, 0);
	}
	return errNone;
}


SCLANG_DEFINE_PRIMITIVE( ObjectHash, 1 )
{
	PyrSlot *a;
	int hash;

	a = g->sp;

	hash = calcHash(a);
	SetInt(a, hash);
	return errNone;
}

SCLANG_DEFINE_PRIMITIVE( ObjectClass, 1 )
{
	PyrSlot *a;
	PyrClass *classobj;

	a = g->sp;
	classobj = classOfSlot(a);
	SetObject(a, classobj);
	return errNone;
}

SCLANG_DEFINE_PRIMITIVE( PrimitiveError, 1 )
{
	PyrSlot *a;

	a = g->sp;
	slotCopy(a,&g->thread->primitiveError);
	return errNone;
}

SCLANG_DEFINE_PRIMITIVE( StackDepth, 1 )
{
	PyrSlot *a;

	a = g->sp;
	SetInt(a, g->gc->StackDepth());
	return errNone;
}

extern void DumpStack(VMGlobals *g, PyrSlot *sp);

SCLANG_DEFINE_PRIMITIVE( DumpStack, 1 )
{
	DumpStack(g, g->sp);
	return errNone;
}

void DumpDetailedBackTrace(VMGlobals *g);

SCLANG_DEFINE_PRIMITIVE( DumpDetailedBackTrace, 1 )
{
	DumpDetailedBackTrace(g);
	return errNone;
}

SCLANG_DEFINE_PRIMITIVE( PrimitiveErrorString, 1 )
{
	PyrSlot *a;
	PyrString *string;
	std::string str;
	std::exception_ptr lastPrimitiveException;
	char *lastPrimitiveExceptionClass, *lastPrimitiveExceptionMethod;

	a = g->sp;
	switch (slotRawInt(&g->thread->primitiveError)) {
		case errReturn : str = "Return (not an error)."; break;
		case errNone : str = "No Error"; break;
		case errFailed : str = "Failed."; break;
		case errBadPrimitive : str = "Bad Primitive."; break;
		case errWrongType : str = "Wrong type."; break;
		case errIndexNotAnInteger : str = "Index not an Integer"; break;
		case errIndexOutOfRange : str = "Index out of range."; break;
		case errImmutableObject : str = "Attempted write to immutable object."; break;
		case errNotAnIndexableObject : str = "Not an indexable object."; break;
		case errStackOverflow : str = "Stack overflow."; break;
		case errOutOfMemory : str = "Out of memory."; break;
		case errCantCallOS : str = "Operation cannot be called from this Process. Try using AppClock instead of SystemClock."; break;
		case errException : {
			lastPrimitiveException = g->lastExceptions[g->thread].first;
			PyrMethod *meth = g->lastExceptions[g->thread].second;
			lastPrimitiveExceptionClass = slotRawSymbol(&slotRawClass(&meth->ownerclass)->name)->name;
			lastPrimitiveExceptionMethod = slotRawSymbol(&meth->name)->name;
			if (lastPrimitiveException) {
				try {
					std::rethrow_exception(lastPrimitiveException);
				} catch(const std::exception& e) {

					const char *errorString = e.what();
					str = std::string("caught exception \'") + errorString + "\' in primitive in method " + lastPrimitiveExceptionClass + ":" + lastPrimitiveExceptionMethod;
					break;
				}
			} else {
				str = std::string("caught unknown exception in primitive in method ") + lastPrimitiveExceptionClass + ":" + lastPrimitiveExceptionMethod;
				break;
			}
			break;
		}
		default : str = "Failed.";
	}
	string = newPyrString(g->gc, str.c_str(), 0, true);
	SetObject(a, string);
	return errNone;
}



SCLANG_DEFINE_PRIMITIVE( PostString, 1 )
{
	PyrSlot *a;

	a = g->sp;
	//if (NotObj(a)) return errWrongType;
	// assume it is a string!
	postText(slotRawString(a)->s, slotRawString(a)->size);
	return errNone;
}

SCLANG_DEFINE_PRIMITIVE( PostLine, 1 )
{
	PyrSlot *a;

	a = g->sp;
	//if (NotObj(a)) return errWrongType;
	// assume it is a string!
	postText(slotRawString(a)->s, slotRawString(a)->size);
	postChar('\n');
	return errNone;
}

SCLANG_DEFINE_PRIMITIVE( HostDebugger, 1 )
{
	PyrSlot *a;

	a = g->sp;
	//Debugger();
	return errNone;
}

SCLANG_DEFINE_PRIMITIVE( ObjectString, 1 )
{
	PyrSlot *a;
	PyrString *string;
	char str[256];

	a = g->sp;
	if (IsSym(a)) {
		string = newPyrString(g->gc, slotRawSymbol(a)->name, 0, true);
		SetObject(a, string);
		return errNone;
	} else if (postString(a, str)) {
		string = newPyrString(g->gc, str, 0, true);
		SetObject(a, string);
		return errNone;
	} else {
		return errFailed;
	}
}

SCLANG_DEFINE_PRIMITIVE( Float_AsStringPrec, 2 )
{
	PyrSlot *a = g->sp - 1;
	PyrSlot *b = g->sp;

	int precision;
	int err = slotIntVal(b, &precision);
	if (err) return err;

	char fmt[8], str[256];
	// if our precision is bigger than our stringsize, we can generate a very nasty buffer overflow here. So
	if( precision <= 0 ) precision = 1;
	if( precision >= 200 ) precision = 200; // Nothing is that big anyway. And we know we will be smaller than our 256 char string

	sprintf(fmt, "%%.%dg", precision);
	sprintf(str, fmt, slotRawFloat(a));

	PyrString *string = newPyrString(g->gc, str, 0, true);
	SetObject(a, string);
	return errNone;

}

SCLANG_DEFINE_PRIMITIVE( ObjectCompileString, 1 )
{
	PyrSlot *a;
	PyrString *string;
	int err = errNone;

	a = g->sp;
	if (IsSym(a)) {
		int len = strlen(slotRawSymbol(a)->name) + 1;
		if (len < 255) {
			char str[256];
			sprintf(str, "'%s'", slotRawSymbol(a)->name);
			string = newPyrString(g->gc, str, 0, true);
		} else {
			char *str = (char*)malloc(len+2);
			sprintf(str, "'%s'", slotRawSymbol(a)->name);
			string = newPyrString(g->gc, str, 0, true);
			free(str);
		}
	} else {
		char str[256];
		err = asCompileString(a, str);
		if (err) return err;
		string = newPyrString(g->gc, str, 0, true);
	}
	SetObject(a, string);
	return err;
}


SCLANG_DEFINE_PRIMITIVE( ClassString, 1 )
{
	PyrSlot *a;
	PyrClass *classobj;
	PyrString *string;

	a = g->sp;
	classobj = classOfSlot(a);
	string = newPyrString(g->gc, slotRawSymbol(&classobj->name)->name, 0, true);
	SetObject(a, string);
	return errNone;
}

SCLANG_DEFINE_PRIMITIVE( PrimName, 1 )
{
	PyrSlot *a;
	PyrThread *thread;

	a = g->sp;
	thread = slotRawThread(a);
	if (slotRawInt(&thread->primitiveIndex) <= gPrimitiveTable.size) {
		SetSymbol(a, gPrimitiveTable.table[slotRawInt(&thread->primitiveIndex)].name);
	} else {
		SetSymbol(a, s_none);
	}
	return errNone;
}

SCLANG_DEFINE_PRIMITIVE( ObjectIsKindOf, 2 )
{
	PyrSlot *a, *b;
	PyrClass *classobj, *testclass;
	int objClassIndex, testClassIndex, maxSubclassIndex;

	a = g->sp - 1;
	b = g->sp;

	if (NotObj(b)) return errWrongType;
	testclass = (PyrClass*)slotRawObject(b);
	classobj = classOfSlot(a);
#if 0
	while (classobj) {
		if (classobj == testclass) {
			SetTrue(a);
			return errNone;
		}
		classobj = slotRawSymbol(&classobj->superclass)->u.classobj;
	}
	SetFalse(a);
#else
	// constant time lookup method:

	objClassIndex = slotRawInt(&classobj->classIndex);
	testClassIndex = slotRawInt(&testclass->classIndex);
	maxSubclassIndex = slotRawInt(&testclass->maxSubclassIndex);

	/*post("%s %s\n", slotRawSymbol(&classobj->name)->name, testclass->name.us->name);
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

SCLANG_DEFINE_PRIMITIVE( ObjectIsMemberOf, 2 )
{
	PyrSlot *a, *b;
	PyrClass *classobj, *testclass;

	a = g->sp - 1;
	b = g->sp;

	if (NotObj(b)) return errWrongType;
	testclass = (PyrClass*)slotRawObject(b);
	classobj = classOfSlot(a);
	if (classobj == testclass) {
		SetTrue(a);
	} else {
		SetFalse(a);
	}
	return errNone;
}

SCLANG_DEFINE_PRIMITIVE( Identical, 2 )
{
	PyrSlot *a, *b;

	a = g->sp - 1;
	b = g->sp;

	if (SlotEq(a, b))
		SetTrue(a);
	else
		SetFalse(a);
	return errNone;
}

SCLANG_DEFINE_PRIMITIVE( NotIdentical, 2 )
{
	PyrSlot *a, *b;

	a = g->sp - 1;
	b = g->sp;

	if ( !SlotEq(a, b) )
		SetTrue(a);
	else
		SetFalse(a);
	return errNone;
}

SCLANG_DEFINE_PRIMITIVE( BasicNewClear, 2 )
{
	PyrSlot *a, *b;
	int size;
	PyrClass *classobj;
	PyrObject *newobj;

	a = g->sp - 1;
	b = g->sp;

	if (NotObj(a)) return errWrongType;
	classobj = (PyrClass*)slotRawObject(a);
	if (slotRawInt(&classobj->classFlags) & classHasIndexableInstances) {
		// create an indexable object
		if (NotInt(b)) {
			if (IsFloat(b)) {
				size = (int)slotRawFloat(b);
			} else if (NotNil(b)) return errIndexNotAnInteger;
			else size = 8;
		} else {
			size = slotRawInt(b);
		}
		if (size < 0) size = 0;
	} else {
		size = 0;
	}
	newobj = instantiateObject(g->gc, classobj, size, true, true);
	SetObject(a, newobj);
	return errNone;
}

SCLANG_DEFINE_VARARGS_PRIMITIVE( BasicNewCopyArgsToInstVars, 1 )
{
	PyrSlot *a, *b;
	PyrClass *classobj;
	PyrObject *newobj;

	a = g->sp - numArgsPushed + 1;
	b = a + 1;

	if (NotObj(a)) return errWrongType;
	classobj = (PyrClass*)slotRawObject(a);
	if (slotRawInt(&classobj->classFlags) & classHasIndexableInstances) {
		error("CopyArgs : object has no instance variables.\n");
		return errFailed;
	}
	newobj = instantiateObject(g->gc, classobj, 0, true, true);
	SetObject(a, newobj);

	int length = sc_min(numArgsPushed-1, newobj->size);
	for (int i=0; i<length; ++i) {
		slotCopy(&newobj->slots[i],&b[i]);
	}

	return errNone;
}

SCLANG_DEFINE_PRIMITIVE( BasicNew, 2 )
{
	PyrSlot *a, *b;
	int size;
	PyrClass *classobj;
	PyrObject *newobj;

	a = g->sp - 1;
	b = g->sp;

	if (NotObj(a)) return errWrongType;
	classobj = (PyrClass*)slotRawObject(a);
	if (slotRawInt(&classobj->classFlags) & classHasIndexableInstances) {
		// create an indexable object
		if (NotInt(b)) {
			if (IsFloat(b)) {
				size = (int)slotRawFloat(b);
			} else if (NotNil(b)) return errIndexNotAnInteger;
			else size = 8;
		} else {
			size = slotRawInt(b);
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
	return IsNil(&fundef->contextDef) && fundef->classptr == class_fundef;
}

bool isWithinClosed(PyrBlock* fundef);
bool isWithinClosed(PyrBlock* fundef)
{
	while (fundef) {
		if (isClosed(fundef)) return true;
		fundef = slotRawBlock(&fundef->contextDef);
	}
	return false;
}

SCLANG_DEFINE_PRIMITIVE( FunctionDefAsFunction, 1 )
{
	PyrSlot *a = g->sp;

	if (!isClosed(slotRawBlock(a))) {
		dumpObjectSlot(a);
		error("Only closed FunctionDef may be converted to a Function using asFunction.\n");
		return errFailed;
	}

	PyrClosure* closure = (PyrClosure*)g->gc->New(2*sizeof(PyrSlot), 0, obj_notindexed, true);

	closure->classptr = gSpecialClasses[op_class_func]->u.classobj;
	closure->size = 2;
	slotCopy(&closure->block,a);
	slotCopy(&closure->context,&slotRawInterpreter(&g->process->interpreter)->context);
	SetObject(a, closure);
	return errNone;
}

SCLANG_DEFINE_PRIMITIVE( FunctionDefDumpContexts, 1 )
{
	PyrSlot *a = g->sp;

	int i=0;
	while (slotRawBlock(a)) {
		post("%2d context %s %p\n", i++, slotRawSymbol(&slotRawObject(a)->classptr->name)->name, slotRawInt(&slotRawBlock(a)->contextDef));
		a = &slotRawBlock(a)->contextDef;
	}
	return errNone;
}

SCLANG_DEFINE_PRIMITIVE( FunctionDefIsClosed, 1 )
{
	PyrSlot *a = g->sp;
	PyrBlock *block = slotRawBlock(a);

	SetBool(a, isClosed(block));
	return errNone;
}

SCLANG_DEFINE_PRIMITIVE( FunctionDefIsWithinClosed, 1 )
{
	PyrSlot *a = g->sp;
	PyrBlock *block = slotRawBlock(a);

	SetBool(a, isWithinClosed(block));
	return errNone;
}

void reallocStack(struct VMGlobals *g, int stackNeeded, int stackDepth)
{
	//PyrThread *thread = g->thread;
	PyrGC *gc = g->gc;
	int newStackSize = NEXTPOWEROFTWO(stackNeeded);

	PyrObject* array = newPyrArray(gc, newStackSize, 0, false);
	memcpy(array->slots, gc->Stack()->slots, stackDepth * sizeof(PyrSlot));
	gc->SetStack(array);
	gc->ToBlack(gc->Stack());
	g->sp = array->slots + stackDepth - 1;
}

SCLANG_DEFINE_VARARGS_PRIMITIVE( FunctionValueArray, 1 )
{
	PyrSlot *b;
	PyrObject *array;
	PyrList *list;
	PyrSlot *pslot, *qslot;
	int m, size;

	//a = g->sp - numArgsPushed + 1;
	b = g->sp;

	if (IsObj(b)) {
		if (slotRawObject(b)->classptr == class_array) {
			array = (PyrObject*)slotRawObject(b);
			above:
			size = array->size;

			PyrObject *stack = g->gc->Stack();
			int stackDepth = g->sp - stack->slots + 1;
			int stackSize = ARRAYMAXINDEXSIZE(stack);
			int stackNeeded = stackDepth + array->size + 64;  // 64 to allow extra for normal stack operations.
			if (stackNeeded > stackSize) {
				reallocStack(g, stackNeeded, stackDepth);
				b = g->sp;
			}

			pslot = array->slots - 1;
			qslot = b - 1;
			//pend = (double*)(pslot + size);
			//while (pslot<pend) slotCopy(++qslot, ++pslot);
			for (m=0; m<size; ++m) slotCopy(++qslot, ++pslot);

			g->sp += size - 1;
			return blockValue(g, size+numArgsPushed-1);

		} else if (slotRawObject(b)->classptr == class_list) {
			list = slotRawList(b);
			if (NotObj(&list->array)) return errWrongType;
			array = slotRawObject(&list->array);
			if (array->classptr != class_array) return errWrongType;
			goto above;
		} else { // last arg is not a list or array, so pass as normal
			return blockValue(g, numArgsPushed);
		}
	} else {
		return blockValue(g, numArgsPushed);
	}
}

SCLANG_DEFINE_VARARGS_PRIMITIVE( FunctionValueArrayEnvir, 1 )
{
	PyrSlot *b;
	PyrObject *array;
	PyrList *list;
	PyrSlot *pslot, *qslot;
	int m, size;

	//a = g->sp - numArgsPushed + 1;
	b = g->sp;

	if (IsObj(b)) {
		if (slotRawObject(b)->classptr == class_array) {
			array = (PyrObject*)slotRawObject(b);
			above:
			size = array->size;

			PyrObject *stack = g->gc->Stack();
			int stackDepth = g->sp - stack->slots + 1;
			int stackSize = ARRAYMAXINDEXSIZE(stack);
			int stackNeeded = stackDepth + array->size + 64;  // 64 to allow extra for normal stack operations.
			if (stackNeeded > stackSize) {
				reallocStack(g, stackNeeded, stackDepth);
				b = g->sp;
			}

			pslot = array->slots - 1;
			qslot = b - 1;
			//pend = (double*)(pslot + size);
			//while (pslot<pend) slotCopy(++qslot, ++pslot);
			for (m=0; m<size; ++m) slotCopy(++qslot, ++pslot);

			g->sp += size - 1;
			return blockValueEnvir(g, size+numArgsPushed-1);

		} else if (slotRawObject(b)->classptr == class_list) {
			list = slotRawList(b);
			if (NotObj(&list->array)) return errWrongType;
			array = slotRawObject(&list->array);
			if (array->classptr != class_array) return errWrongType;
			goto above;
		} else { // last arg is not a list or array, so pass as normal
			return blockValueEnvir(g, numArgsPushed);
		}
	} else {
		return blockValueEnvir(g, numArgsPushed);
	}
}

HOT int blockValue(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *args;
	PyrSlot *vars;
	PyrFrame *frame;
	PyrSlot *pslot, *qslot;
	PyrSlot *rslot;
	PyrObject *proto;
	int i, m, mmax, numtemps;
	PyrBlock *block;
	PyrFrame *context;
	PyrFrame *caller;
	PyrFrame *homeContext;
	PyrClosure *closure;
	PyrMethodRaw *methraw;

#if TAILCALLOPTIMIZE
	int tailCall = g->tailCall;
	if (tailCall) {
		if (tailCall == 1) {
			returnFromMethod(g);
		} else {
			returnFromBlock(g);
		}
	}
#endif

	g->execMethod = 30;

	args = g->sp - numArgsPushed + 1;

	numArgsPushed -- ;
	g->numpop = 0;

	closure = (PyrClosure*)slotRawObject(args);
	block = slotRawBlock(&closure->block);
	context = slotRawFrame(&closure->context);

	proto = IsObj(&block->prototypeFrame) ? slotRawObject(&block->prototypeFrame) : NULL;
	methraw = METHRAW(block);
	numtemps = methraw->numtemps;
	caller = g->frame;

	frame = (PyrFrame*)g->gc->NewFrame(methraw->frameSize, 0, obj_slot, methraw->needsHeapContext);
	vars = frame->vars - 1;
	frame->classptr = class_frame;
	frame->size = FRAMESIZE + numtemps;
	SetObject(&frame->method, block);
	slotCopy(&frame->homeContext,&context->homeContext);
	slotCopy(&frame->context,&closure->context);

	if (caller) {
		SetPtr(&caller->ip, g->ip);
		SetObject(&frame->caller, g->frame);
	} else {
		SetInt(&frame->caller, 0);
	}
	SetPtr(&frame->ip,  0);


	g->sp = args - 1;
	g->ip = slotRawInt8Array(&block->code)->b - 1;
	g->frame = frame;
	g->block = block;

	if (numArgsPushed <= methraw->numargs) {	/* not enough args pushed */
		/* push all args to frame */
		qslot = args;
		pslot = vars;

		for (m=0; m<numArgsPushed; ++m) slotCopy(++pslot, ++qslot);

		/* push default arg values */
		pslot = vars + numArgsPushed;
		qslot = proto->slots + numArgsPushed - 1;
		for (m=0; m<numtemps - numArgsPushed; ++m) slotCopy(++pslot, ++qslot);
	} else if (methraw->varargs) {
		PyrObject *list;
		PyrSlot *lslot;

		/* push all normal args to frame */
		qslot = args;
		pslot = vars;
		for (m=0,mmax=methraw->numargs; m<mmax; ++m) slotCopy(++pslot, ++qslot);

		/* push list */
		i = numArgsPushed - methraw->numargs;
		list = newPyrArray(g->gc, i, 0, false);
		list->size = i;

		rslot = pslot+1;
		SetObject(rslot, list);
		//SetObject(vars + methraw->numargs + 1, list);

		/* put extra args into list */
		lslot = list->slots - 1;
		// fixed and raw sizes are zero
		for (m=0; m<i; ++m) slotCopy(++lslot, ++qslot);

		if (methraw->numvars) {
			/* push default keyword and var values */
			pslot = vars + methraw->numargs + 1;
			qslot = proto->slots + methraw->numargs;
			for (m=0,mmax=methraw->numvars; m<mmax; ++m) slotCopy(++pslot, ++qslot);
		}
	} else {
		if (methraw->numargs) {
			/* push all args to frame */
			qslot = args;
			pslot = vars;
			for (m=0,mmax=methraw->numargs; m<mmax; ++m) slotCopy(++pslot, ++qslot);
		}
		if (methraw->numvars) {
			/* push default keyword and var values */
			pslot = vars + methraw->numargs;
			qslot = proto->slots + methraw->numargs - 1;
			for (m=0,mmax=methraw->numvars; m<mmax; ++m) slotCopy(++pslot, ++qslot);
		}
	}

	homeContext = slotRawFrame(&frame->homeContext);
	if (homeContext) {
		PyrMethodRaw *methraw;
		g->method = slotRawMethod(&homeContext->method);
		methraw = METHRAW(g->method);
		slotCopy(&g->receiver,&homeContext->vars[0]);
	} else {
		slotCopy(&g->receiver,&g->process->interpreter);
	}

	return errNone;
}

int blockValueWithKeys(VMGlobals *g, int allArgsPushed, int numKeyArgsPushed);
int blockValueWithKeys(VMGlobals *g, int allArgsPushed, int numKeyArgsPushed)
{
	PyrSlot *args;
	PyrSlot *vars;
	PyrFrame *frame;
	PyrSlot *pslot, *qslot;
	PyrSlot *rslot;
	PyrObject *proto;
	int i, j, m, mmax, numtemps, numArgsPushed;
	PyrBlock *block;
	PyrFrame *context;
	PyrFrame *caller;
	PyrFrame *homeContext;
	PyrClosure *closure;
	PyrMethodRaw *methraw;

#if TAILCALLOPTIMIZE
	int tailCall = g->tailCall;
	if (tailCall) {
		if (tailCall == 1) {
			returnFromMethod(g);
		} else {
			returnFromBlock(g);
		}
	}
#endif

	g->execMethod = 40;

	args = g->sp - allArgsPushed + 1;

	allArgsPushed -- ;
	g->numpop = 0;

	closure = (PyrClosure*)slotRawObject(args);
	block = slotRawBlock(&closure->block);
	context = slotRawFrame(&closure->context);

	proto = IsObj(&block->prototypeFrame) ? slotRawObject(&block->prototypeFrame) : NULL;

	methraw = METHRAW(block);
	numtemps = methraw->numtemps;
	caller = g->frame;
	numArgsPushed = allArgsPushed - (numKeyArgsPushed<<1);

	frame = (PyrFrame*)g->gc->NewFrame(methraw->frameSize, 0, obj_slot, methraw->needsHeapContext);
	vars = frame->vars - 1;
	frame->classptr = class_frame;
	frame->size = FRAMESIZE + numtemps;
	SetObject(&frame->method, block);
	slotCopy(&frame->homeContext,&context->homeContext);
	slotCopy(&frame->context,&closure->context);

	if (caller) {
		SetPtr(&caller->ip, g->ip);
		SetObject(&frame->caller, g->frame);
	} else {
		SetInt(&frame->caller, 0);
	}
	SetPtr(&frame->ip,  0);

	g->sp = args - 1;
	g->ip = slotRawInt8Array(&block->code)->b - 1;
	g->frame = frame;
	g->block = block;

	if (numArgsPushed <= methraw->numargs) {	/* not enough args pushed */
		/* push all args to frame */
		qslot = args;
		pslot = vars;

		for (m=0; m<numArgsPushed; ++m) slotCopy(++pslot, ++qslot);

		/* push default arg values */
		pslot = vars + numArgsPushed;
		qslot = proto->slots + numArgsPushed - 1;
		for (m=0; m<numtemps - numArgsPushed; ++m) slotCopy(++pslot, ++qslot);
	} else if (methraw->varargs) {
		PyrObject *list;
		PyrSlot *lslot;

		/* push all normal args to frame */
		qslot = args;
		pslot = vars;
		for (m=0,mmax=methraw->numargs; m<mmax; ++m) slotCopy(++pslot, ++qslot);

		/* push list */
		i = numArgsPushed - methraw->numargs;
		list = newPyrArray(g->gc, i, 0, false);
		list->size = i;

		rslot = pslot+1;
		SetObject(rslot, list);
		//SetObject(vars + methraw->numargs + 1, list);

		/* put extra args into list */
		lslot = list->slots - 1;
		// fixed and raw sizes are zero
		//lend = lslot + i;
		//while (lslot < lend) slotCopy(++lslot, ++qslot);
		for (m=0; m<i; ++m) slotCopy(++lslot, ++qslot);

		if (methraw->numvars) {
			/* push default keyword and var values */
			pslot = vars + methraw->numargs + 1;
			qslot = proto->slots + methraw->numargs;
			for (m=0,mmax=methraw->numvars; m<mmax; ++m) slotCopy(++pslot, ++qslot);
		}
	} else {
		if (methraw->numargs) {
			/* push all args to frame */
			qslot = args;
			pslot = vars;
			//pend = pslot + methraw->numargs;
			//while (pslot < pend) slotCopy(++pslot, ++qslot);
			for (m=0,mmax=methraw->numargs; m<mmax; ++m) slotCopy(++pslot, ++qslot);
		}
		if (methraw->numvars) {
			/* push default keyword and var values */
			pslot = vars + methraw->numargs;
			qslot = proto->slots + methraw->numargs - 1;
			//pend = pslot + methraw->numvars;
			//while (pslot<pend) slotCopy(++pslot, ++qslot);
			for (m=0,mmax=methraw->numvars; m<mmax; ++m) slotCopy(++pslot, ++qslot);
		}
	}
	// do keyword lookup:
	if (numKeyArgsPushed && methraw->posargs) {
		PyrSlot *key;
		PyrSymbol **name0, **name;
		name0 = slotRawSymbolArray(&block->argNames)->symbols;
		key = args + numArgsPushed + 1;
		for (i=0; i<numKeyArgsPushed; ++i, key+=2) {
			name = name0;
			for (j=0; j<methraw->posargs; ++j, ++name) {
				if (*name == slotRawSymbol(key)) {
					slotCopy(&vars[j+1],&key[1]);
					goto found1;
				}
			}
			if (gKeywordError) {
				post("WARNING: keyword arg '%s' not found in call to function.\n",
					slotRawSymbol(key)->name);
			}
			found1: ;
		}
	}

	homeContext = slotRawFrame(&frame->homeContext);
	if (homeContext) {
		PyrMethodRaw *methraw;
		g->method = slotRawMethod(&homeContext->method);
		methraw = METHRAW(g->method);
		slotCopy(&g->receiver,&homeContext->vars[0]);
	} else {
		slotCopy(&g->receiver,&g->process->interpreter);
	}
	return errNone;
}

bool identDict_lookupNonNil(PyrObject *dict, PyrSlot *key, int hash, PyrSlot *result);

int blockValueEnvir(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *args;
	PyrSlot *vars;
	PyrFrame *frame;
	PyrSlot *pslot, *qslot;
	PyrSlot *rslot;
	PyrObject *proto;
	int i, m, mmax, numtemps;
	PyrBlock *block;
	PyrFrame *context;
	PyrFrame *caller;
	PyrFrame *homeContext;
	PyrClosure *closure;
	PyrMethodRaw *methraw;
	PyrSlot *curEnvirSlot;

#if TAILCALLOPTIMIZE
	int tailCall = g->tailCall;
	if (tailCall) {
		if (tailCall == 1) {
			returnFromMethod(g);
		} else {
			returnFromBlock(g);
		}
	}
#endif

	g->execMethod = 50;

	args = g->sp - numArgsPushed + 1;

	numArgsPushed -- ;
	g->numpop = 0;

	closure = (PyrClosure*)slotRawObject(args);
	block = slotRawBlock(&closure->block);
	context = slotRawFrame(&closure->context);

	proto = IsObj(&block->prototypeFrame) ? slotRawObject(&block->prototypeFrame) : NULL;

	methraw = METHRAW(block);
	numtemps = methraw->numtemps;
	caller = g->frame;

	frame = (PyrFrame*)g->gc->NewFrame(methraw->frameSize, 0, obj_slot, methraw->needsHeapContext);
	vars = frame->vars - 1;
	frame->classptr = class_frame;
	frame->size = FRAMESIZE + numtemps;
	SetObject(&frame->method, block);
	slotCopy(&frame->homeContext,&context->homeContext);
	slotCopy(&frame->context,&closure->context);

	if (caller) {
		SetPtr(&caller->ip, g->ip);
		SetObject(&frame->caller, g->frame);
	} else {
		SetInt(&frame->caller, 0);
	}
	SetPtr(&frame->ip,  0);


	g->sp = args - 1;
	g->ip = slotRawInt8Array(&block->code)->b - 1;
	g->frame = frame;
	g->block = block;

	if (numArgsPushed <= methraw->numargs) {	/* not enough args pushed */
		/* push all args to frame */
		qslot = args;
		pslot = vars;

		for (m=0; m<numArgsPushed; ++m) slotCopy(++pslot, ++qslot);

		/* push default arg values */
		pslot = vars + numArgsPushed;
		qslot = proto->slots + numArgsPushed - 1;
		for (m=0; m<numtemps - numArgsPushed; ++m) slotCopy(++pslot, ++qslot);

		// replace defaults with environment variables
		curEnvirSlot = &g->classvars->slots[1]; // currentEnvironment is the second class var.

		if (isKindOfSlot(curEnvirSlot, s_identitydictionary->u.classobj)) {
			PyrSymbol **argNames;
			argNames = slotRawSymbolArray(&block->argNames)->symbols;
			for (m=numArgsPushed; m<methraw->numargs; ++m) {
				// replace the args with values from the environment if they exist
				PyrSlot keyslot;
				SetSymbol(&keyslot, argNames[m]);
				identDict_lookupNonNil(slotRawObject(curEnvirSlot), &keyslot, calcHash(&keyslot), vars+m+1);
			}
		}
	} else if (methraw->varargs) {
		PyrObject *list;
		PyrSlot *lslot;

		/* push all normal args to frame */
		qslot = args;
		pslot = vars;
		for (m=0,mmax=methraw->numargs; m<mmax; ++m) slotCopy(++pslot, ++qslot);

		/* push list */
		i = numArgsPushed - methraw->numargs;
		list = newPyrArray(g->gc, i, 0, false);
		list->size = i;

		rslot = pslot+1;
		SetObject(rslot, list);
		//SetObject(vars + methraw->numargs + 1, list);

		/* put extra args into list */
		lslot = list->slots - 1;
		// fixed and raw sizes are zero
		for (m=0; m<i; ++m) slotCopy(++lslot, ++qslot);

		if (methraw->numvars) {
			/* push default keyword and var values */
			pslot = vars + methraw->numargs + 1;
			qslot = proto->slots + methraw->numargs;
			for (m=0,mmax=methraw->numvars; m<mmax; ++m) slotCopy(++pslot, ++qslot);
		}
	} else {
		if (methraw->numargs) {
			/* push all args to frame */
			qslot = args;
			pslot = vars;
			for (m=0,mmax=methraw->numargs; m<mmax; ++m) slotCopy(++pslot, ++qslot);
		}
		if (methraw->numvars) {
			/* push default keyword and var values */
			pslot = vars + methraw->numargs;
			qslot = proto->slots + methraw->numargs - 1;
			for (m=0,mmax=methraw->numvars; m<mmax; ++m) slotCopy(++pslot, ++qslot);
		}
	}

	homeContext = slotRawFrame(&frame->homeContext);
	if (homeContext) {
		PyrMethodRaw *methraw;
		g->method = slotRawMethod(&homeContext->method);
		methraw = METHRAW(g->method);
		slotCopy(&g->receiver,&homeContext->vars[0]);
	} else {
		slotCopy(&g->receiver,&g->process->interpreter);
	}
	return errNone;
}

int blockValueEnvirWithKeys(VMGlobals *g, int allArgsPushed, int numKeyArgsPushed);
int blockValueEnvirWithKeys(VMGlobals *g, int allArgsPushed, int numKeyArgsPushed)
{
	PyrSlot *args;
	PyrSlot *vars;
	PyrFrame *frame;
	PyrSlot *pslot, *qslot;
	PyrSlot *rslot;
	PyrObject *proto;
	int i, j, m, mmax, numtemps, numArgsPushed;
	PyrBlock *block;
	PyrFrame *context;
	PyrFrame *caller;
	PyrFrame *homeContext;
	PyrClosure *closure;
	PyrMethodRaw *methraw;
	PyrSlot *curEnvirSlot;

#if TAILCALLOPTIMIZE
	int tailCall = g->tailCall;
	if (tailCall) {
		if (tailCall == 1) {
			returnFromMethod(g);
		} else {
			returnFromBlock(g);
		}
	}
#endif

	g->execMethod = 60;

	args = g->sp - allArgsPushed + 1;

	allArgsPushed -- ;
	g->numpop = 0;

	closure = (PyrClosure*)slotRawObject(args);
	block = slotRawBlock(&closure->block);
	context = slotRawFrame(&closure->context);

	proto = IsObj(&block->prototypeFrame) ? slotRawObject(&block->prototypeFrame) : NULL;

	methraw = METHRAW(block);
	numtemps = methraw->numtemps;
	caller = g->frame;
	numArgsPushed = allArgsPushed - (numKeyArgsPushed<<1);

	frame = (PyrFrame*)g->gc->NewFrame(methraw->frameSize, 0, obj_slot, methraw->needsHeapContext);
	vars = frame->vars - 1;
	frame->classptr = class_frame;
	frame->size = FRAMESIZE + numtemps;
	SetObject(&frame->method, block);
	slotCopy(&frame->homeContext,&context->homeContext);
	slotCopy(&frame->context,&closure->context);

	if (caller) {
		SetPtr(&caller->ip, g->ip);
		SetObject(&frame->caller, g->frame);
	} else {
		SetInt(&frame->caller, 0);
	}
	SetPtr(&frame->ip,  0);


	g->sp = args - 1;
	g->ip = slotRawInt8Array(&block->code)->b - 1;
	g->frame = frame;
	g->block = block;

	if (numArgsPushed <= methraw->numargs) {	/* not enough args pushed */
		/* push all args to frame */
		qslot = args;
		pslot = vars;

		for (m=0; m<numArgsPushed; ++m) slotCopy(++pslot, ++qslot);

		/* push default arg values */
		pslot = vars + numArgsPushed;
		qslot = proto->slots + numArgsPushed - 1;
		for (m=0; m<numtemps - numArgsPushed; ++m) slotCopy(++pslot, ++qslot);

		// replace defaults with environment variables
		curEnvirSlot = &g->classvars->slots[1]; // currentEnvironment is the second class var.

		if (isKindOfSlot(curEnvirSlot, s_identitydictionary->u.classobj)) {
			PyrSymbol **argNames;
			argNames = slotRawSymbolArray(&block->argNames)->symbols;
			for (m=numArgsPushed; m<methraw->numargs; ++m) {
				// replace the args with values from the environment if they exist
				PyrSlot keyslot;
				SetSymbol(&keyslot, argNames[m]);
				identDict_lookupNonNil(slotRawObject(curEnvirSlot), &keyslot, calcHash(&keyslot), vars+m+1);
			}
		}


	} else if (methraw->varargs) {
		PyrObject *list;
		PyrSlot *lslot;

		/* push all normal args to frame */
		qslot = args;
		pslot = vars;
		for (m=0,mmax=methraw->numargs; m<mmax; ++m) slotCopy(++pslot, ++qslot);

		/* push list */
		i = numArgsPushed - methraw->numargs;
		list = newPyrArray(g->gc, i, 0, false);
		list->size = i;

		rslot = pslot+1;
		SetObject(rslot, list);
		//SetObject(vars + methraw->numargs + 1, list);

		/* put extra args into list */
		lslot = list->slots - 1;
		// fixed and raw sizes are zero
		//lend = lslot + i;
		//while (lslot < lend) slotCopy(++lslot, ++qslot);
		for (m=0; m<i; ++m) slotCopy(++lslot, ++qslot);

		if (methraw->numvars) {
			/* push default keyword and var values */
			pslot = vars + methraw->numargs + 1;
			qslot = proto->slots + methraw->numargs;
			for (m=0,mmax=methraw->numvars; m<mmax; ++m) slotCopy(++pslot, ++qslot);
		}
	} else {
		if (methraw->numargs) {
			/* push all args to frame */
			qslot = args;
			pslot = vars;
			//pend = pslot + methraw->numargs;
			//while (pslot < pend) slotCopy(++pslot, ++qslot);
			for (m=0,mmax=methraw->numargs; m<mmax; ++m) slotCopy(++pslot, ++qslot);
		}
		if (methraw->numvars) {
			/* push default keyword and var values */
			pslot = vars + methraw->numargs;
			qslot = proto->slots + methraw->numargs - 1;
			//pend = pslot + methraw->numvars;
			//while (pslot<pend) slotCopy(++pslot, ++qslot);
			for (m=0,mmax=methraw->numvars; m<mmax; ++m) slotCopy(++pslot, ++qslot);
		}
	}
	// do keyword lookup:
	if (numKeyArgsPushed && methraw->posargs) {
		PyrSymbol **name0, **name;
		PyrSlot *key;
		name0 = slotRawSymbolArray(&block->argNames)->symbols;
		key = args + numArgsPushed + 1;
		for (i=0; i<numKeyArgsPushed; ++i, key+=2) {
			name = name0;
			for (j=0; j<methraw->posargs; ++j, ++name) {
				if (*name == slotRawSymbol(key)) {
					slotCopy(&vars[j+1],&key[1]);
					goto found1;
				}
			}
			if (gKeywordError) {
				post("WARNING: keyword arg '%s' not found in call to function.\n",
					slotRawSymbol(key)->name);
			}
			found1: ;
		}
	}

	homeContext = slotRawFrame(&frame->homeContext);
	if (homeContext) {
		PyrMethodRaw *methraw;
		g->method = slotRawMethod(&homeContext->method);
		methraw = METHRAW(g->method);
		slotCopy(&g->receiver,&homeContext->vars[0]);
	} else {
		slotCopy(&g->receiver,&g->process->interpreter);
	}
	return errNone;
}


int objectPerform(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *recvrSlot, *selSlot, *listSlot;
	PyrSlot *pslot, *qslot;
	PyrSymbol *selector;
	int m, mmax;

	recvrSlot = g->sp - numArgsPushed + 1;
	selSlot = recvrSlot + 1;

	if (IsSym(selSlot)) {
		selector = slotRawSymbol(selSlot);

		// move args down one to fill selector's position
		pslot = selSlot - 1;
		qslot = selSlot;
		for (m = 0; m < numArgsPushed - 2; ++m)
            slotCopy(++pslot, ++qslot);
		g->sp--;
		numArgsPushed--;
		// now the stack looks just like it would for a normal message send
	} else if (IsObj(selSlot)) {
        // if a List was passed, cast it to an Array, else throw an error
		listSlot = selSlot;
		if (slotRawObject(listSlot)->classptr == class_list) {
			listSlot = slotRawObject(listSlot)->slots;
		}
		if (NotObj(listSlot) || slotRawObject(listSlot)->classptr != class_array) {
			goto badselector;
		}

		PyrObject *array = slotRawObject(listSlot);

		if (array->size < 1) {
			error("Array must have a selector.\n");
			return errFailed;
		}

		selSlot = array->slots;

        // check the first slot to see if it's a symbol
        if (NotSym(selSlot)) {
            error("First element of array must be a Symbol selector.\n");
            dumpObjectSlot(selSlot);
            return errWrongType;
        }

		selector = slotRawSymbol(selSlot);

		if (numArgsPushed > 2) {
			qslot = recvrSlot + numArgsPushed;
			pslot = recvrSlot + numArgsPushed + array->size - 2;
			for (m = 0; m < numArgsPushed - 2; ++m)
                slotCopy(--pslot, --qslot);
		}

		pslot = recvrSlot;
		qslot = selSlot;
		for (m = 0, mmax = array->size-1; m < mmax; ++m)
            slotCopy(++pslot, ++qslot);

		g->sp += array->size - 2;
		numArgsPushed += array->size - 2;
		// now the stack looks just like it would for a normal message send

	} else {
		badselector:
		error("perform selector not a Symbol or Array.\n");
		dumpObjectSlot(selSlot);
		return errWrongType;
	}

	sendMessage(g, selector, numArgsPushed);
	g->numpop = 0;
	return errNone;
}

int objectPerformWithKeys(VMGlobals *g, int numArgsPushed, int numKeyArgsPushed)
{
	PyrSlot *recvrSlot, *selSlot, *listSlot;
	PyrSlot *pslot, *qslot;
	PyrSymbol *selector;
	int m, mmax;

	recvrSlot = g->sp - numArgsPushed + 1;
	selSlot = recvrSlot + 1;
	if (IsSym(selSlot)) {
		selector = slotRawSymbol(selSlot);
		// move args down one to fill selector's position
		pslot = selSlot - 1;
		qslot = selSlot;
		for (m=0; m<numArgsPushed - 2; ++m) slotCopy(++pslot, ++qslot);
		g->sp -- ;
		numArgsPushed -- ;
		// now the stack looks just like it would for a normal message send
	} else if (IsObj(selSlot)) {
		listSlot = selSlot;
		if (slotRawObject(listSlot)->classptr == class_list) {
			listSlot = slotRawObject(listSlot)->slots;
		}
		if (NotObj(listSlot) || slotRawObject(listSlot)->classptr != class_array) {
			goto badselector;
		}
		PyrObject *array = slotRawObject(listSlot);
		if (array->size < 1) {
			error("Array must have a selector.\n");
			return errFailed;
		}
		selSlot = array->slots;
		selector = slotRawSymbol(selSlot);

		if (numArgsPushed>2) {
			qslot = recvrSlot + numArgsPushed;
			pslot = recvrSlot + numArgsPushed + array->size - 2;
			for (m=0; m<numArgsPushed - 2; ++m) *--pslot = *--qslot;
		}

		pslot = recvrSlot;
		qslot = selSlot;
		for (m=0,mmax=array->size-1; m<mmax; ++m) slotCopy(++pslot, ++qslot);

		g->sp += array->size - 2;
		numArgsPushed += array->size - 2;
		// now the stack looks just like it would for a normal message send

	} else {
		badselector:
		error("perform selector not a Symbol or Array.\n");
		dumpObjectSlot(selSlot);
		return errWrongType;
	}

	sendMessageWithKeys(g, selector, numArgsPushed, numKeyArgsPushed);
	g->numpop = 0;
	return errNone;
}

SCLANG_DEFINE_VARARGS_PRIMITIVE( ObjectPerformList, 2 )
{
	PyrSlot *recvrSlot, *selSlot, *listSlot;
	PyrSlot *pslot, *qslot;
	PyrSymbol *selector;
	int m, mmax, numargslots;
	PyrObject *array;


	recvrSlot = g->sp - numArgsPushed + 1;
	selSlot = recvrSlot + 1;
	listSlot = g->sp;
	numargslots = numArgsPushed - 3;
	if (NotSym(selSlot)) {
		error("Selector not a Symbol :\n");
		return errWrongType;
	}
	selector = slotRawSymbol(selSlot);

	if (NotObj(listSlot)) {
		return objectPerform(g, numArgsPushed);
	}
	if (slotRawObject(listSlot)->classptr == class_array) {
		doarray:
		array = slotRawObject(listSlot);

		PyrObject *stack = g->gc->Stack();
		int stackDepth = g->sp - stack->slots + 1;
		int stackSize = ARRAYMAXINDEXSIZE(stack);
		int stackNeeded = stackDepth + array->size + 64;  // 64 to allow extra for normal stack operations.
		if (stackNeeded > stackSize) {
			reallocStack(g, stackNeeded, stackDepth);
			recvrSlot = g->sp - numArgsPushed + 1;
			selSlot = recvrSlot + 1;
		}

		pslot = recvrSlot;
		if (numargslots>0) {
			qslot = selSlot;
			for (m=0; m<numargslots; ++m) slotCopy(++pslot, ++qslot);
		} else numargslots = 0;
		qslot = array->slots - 1;
		for (m=0,mmax=array->size; m<mmax; ++m) slotCopy(++pslot, ++qslot);
	} else if (slotRawObject(listSlot)->classptr == class_list) {
		listSlot = slotRawObject(listSlot)->slots;
		if (NotObj(listSlot) || slotRawObject(listSlot)->classptr != class_array) {
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


int objectSuperPerform(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *recvrSlot, *selSlot, *listSlot;
	PyrSlot *pslot, *qslot;
	PyrSymbol *selector;
	int m, mmax;

	recvrSlot = g->sp - numArgsPushed + 1;

	PyrClass* classobj = slotRawSymbol(&slotRawClass(&g->method->ownerclass)->superclass)->u.classobj;
	if (!isKindOfSlot(recvrSlot, classobj)) {
		error("superPerform must be called with 'this' as the receiver.\n");
		return errFailed;
	}

	selSlot = recvrSlot + 1;
	if (IsSym(selSlot)) {
		selector = slotRawSymbol(selSlot);
		// move args down one to fill selector's position
		pslot = selSlot - 1;
		qslot = selSlot;
		for (m=0; m<numArgsPushed - 2; ++m) slotCopy(++pslot, ++qslot);
		g->sp -- ;
		numArgsPushed -- ;
		// now the stack looks just like it would for a normal message send
	} else if (IsObj(selSlot)) {
		listSlot = selSlot;
		if (slotRawObject(listSlot)->classptr == class_list) {
			listSlot = slotRawObject(listSlot)->slots;
		}
		if (NotObj(listSlot) || slotRawObject(listSlot)->classptr != class_array) {
			goto badselector;
		}
		PyrObject *array = slotRawObject(listSlot);
		if (array->size < 1) {
			error("Array must have a selector.\n");
			return errFailed;
		}
		selSlot = array->slots;
		selector = slotRawSymbol(selSlot);

		if (numArgsPushed>2) {
			qslot = recvrSlot + numArgsPushed;
			pslot = recvrSlot + numArgsPushed + array->size - 2;
			for (m=0; m<numArgsPushed - 2; ++m) slotCopy(--pslot, --qslot);
		}

		pslot = recvrSlot;
		qslot = selSlot;
		for (m=0,mmax=array->size-1; m<mmax; ++m) slotCopy(++pslot, ++qslot);

		g->sp += array->size - 2;
		numArgsPushed += array->size - 2;
		// now the stack looks just like it would for a normal message send

	} else {
		badselector:
		error("perform selector not a Symbol or Array.\n");
		dumpObjectSlot(selSlot);
		return errWrongType;
	}

	sendSuperMessage(g, selector, numArgsPushed);
	g->numpop = 0;
	return errNone;
}

int objectSuperPerformWithKeys(VMGlobals *g, int numArgsPushed, int numKeyArgsPushed)
{
	PyrSlot *recvrSlot, *selSlot, *listSlot;
	PyrSlot *pslot, *qslot;
	PyrSymbol *selector;
	int m, mmax;

	recvrSlot = g->sp - numArgsPushed + 1;

	PyrClass* classobj = slotRawSymbol(&slotRawClass(&g->method->ownerclass)->superclass)->u.classobj;
	if (!isKindOfSlot(recvrSlot, classobj)) {
		error("superPerform must be called with 'this' as the receiver.\n");
		return errFailed;
	}

	selSlot = recvrSlot + 1;
	if (IsSym(selSlot)) {
		selector = slotRawSymbol(selSlot);
		// move args down one to fill selector's position
		pslot = selSlot - 1;
		qslot = selSlot;
		for (m=0; m<numArgsPushed - 2; ++m) slotCopy(++pslot, ++qslot);
		g->sp -- ;
		numArgsPushed -- ;
		// now the stack looks just like it would for a normal message send
	} else if (IsObj(selSlot)) {
		listSlot = selSlot;
		if (slotRawObject(listSlot)->classptr == class_list) {
			listSlot = slotRawObject(listSlot)->slots;
		}
		if (NotObj(listSlot) || slotRawObject(listSlot)->classptr != class_array) {
			goto badselector;
		}
		PyrObject *array = slotRawObject(listSlot);
		if (array->size < 1) {
			error("Array must have a selector.\n");
			return errFailed;
		}
		selSlot = array->slots;
		selector = slotRawSymbol(selSlot);

		if (numArgsPushed>2) {
			qslot = recvrSlot + numArgsPushed;
			pslot = recvrSlot + numArgsPushed + array->size - 2;
			for (m=0; m<numArgsPushed - 2; ++m) *--pslot = *--qslot;
		}

		pslot = recvrSlot;
		qslot = selSlot;
		for (m=0,mmax=array->size-1; m<mmax; ++m) slotCopy(++pslot, ++qslot);

		g->sp += array->size - 2;
		numArgsPushed += array->size - 2;
		// now the stack looks just like it would for a normal message send

	} else {
		badselector:
		error("perform selector not a Symbol or Array.\n");
		dumpObjectSlot(selSlot);
		return errWrongType;
	}

	sendSuperMessageWithKeys(g, selector, numArgsPushed, numKeyArgsPushed);
	g->numpop = 0;
	return errNone;
}


SCLANG_DEFINE_VARARGS_PRIMITIVE( SuperPerformList, 2 )
{
	PyrSlot *recvrSlot, *selSlot, *listSlot;
	PyrSlot *pslot, *qslot;
	PyrSymbol *selector;
	int m, mmax, numargslots;
	PyrObject *array;

	recvrSlot = g->sp - numArgsPushed + 1;
	selSlot = recvrSlot + 1;
	listSlot = g->sp;
	numargslots = numArgsPushed - 3;
	if (NotSym(selSlot)) {
		error("Selector not a Symbol :\n");
		return errWrongType;
	}
	selector = slotRawSymbol(selSlot);
	if (NotObj(listSlot)) {
		return objectPerform(g, numArgsPushed);
	}
	if (slotRawObject(listSlot)->classptr == class_array) {
		doarray:
		pslot = recvrSlot;
		if (numargslots>0) {
			qslot = selSlot;
			for (m=0; m<numargslots; ++m) slotCopy(++pslot, ++qslot);
		} else numargslots = 0;
		array = slotRawObject(listSlot);
		qslot = array->slots - 1;
		for (m=0,mmax=array->size; m<mmax; ++m) slotCopy(++pslot, ++qslot);
	} else if (slotRawObject(listSlot)->classptr == class_list) {
		listSlot = slotRawObject(listSlot)->slots;
		if (NotObj(listSlot) || slotRawObject(listSlot)->classptr != class_array) {
			error("List array not an Array.\n");
			dumpObjectSlot(listSlot);
			return errWrongType;
		}
		goto doarray;
	} else {
		return objectSuperPerform(g, numArgsPushed);
	}
	g->sp += array->size - 2;
	numArgsPushed = numargslots + array->size + 1;
	// now the stack looks just like it would for a normal message send

	sendSuperMessage(g, selector, numArgsPushed);
	g->numpop = 0;
	return errNone;
}

SCLANG_DEFINE_PRIMITIVE( ObjectPerformMsg, 2 )
{
	PyrSlot *recvrSlot, *selSlot, *listSlot;
	PyrSlot *pslot, *qslot;
	PyrSymbol *selector;
	int m, mmax;
	PyrObject *array;

	recvrSlot = g->sp - 1;
	listSlot = g->sp;

	if (NotObj(listSlot)) {
		error("Expected Array or List.. Got :\n");
		dumpObjectSlot(listSlot);
		return errWrongType;
	}
	if (slotRawObject(listSlot)->classptr == class_array) {
		doarray:
		array = slotRawObject(listSlot);

		selSlot = array->slots;
		if (NotSym(selSlot)) {
			error("Selector not a Symbol :\n");
			return errWrongType;
		}
		selector = slotRawSymbol(selSlot);

		pslot = recvrSlot;
		qslot = selSlot;
		for (m=0,mmax=array->size-1; m<mmax; ++m) slotCopy(++pslot, ++qslot);
	} else if (slotRawObject(listSlot)->classptr == class_list) {
		listSlot = slotRawObject(listSlot)->slots;
		if (NotObj(listSlot) || slotRawObject(listSlot)->classptr != class_array) {
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

SCLANG_DEFINE_VARARGS_PRIMITIVE( ArrayPerformMsg, 1 )
{
	PyrSlot *recvrSlot, *selSlot, *arraySlot;
	PyrSlot *pslot, *qslot;
	PyrSymbol *selector;
	int m, mmax, numargslots;
	PyrObject *array;

	arraySlot = g->sp - numArgsPushed + 1;
	array = slotRawObject(arraySlot);
	if (array->size < 2) {
		error("Array must contain a receiver and a selector.\n");
		return errFailed;
	}
	recvrSlot = array->slots;
	selSlot = recvrSlot + 1;
	numargslots = numArgsPushed - 1;
	if (NotSym(selSlot)) {
		error("Selector not a Symbol :\n");
		return errWrongType;
	}

	selector = slotRawSymbol(selSlot);

	slotCopy(arraySlot,recvrSlot);

	if (numargslots>0) {
		qslot = arraySlot + numargslots + 1;
		pslot = arraySlot + numargslots + array->size - 1;
		for (m=0; m<numargslots; ++m) slotCopy(--pslot, --qslot);
	} else numargslots = 0;

	pslot = arraySlot;
	qslot = selSlot;
	for (m=0,mmax=array->size-2; m<mmax; ++m) slotCopy(++pslot, ++qslot);

	g->sp += array->size - 2;
	numArgsPushed = numargslots + array->size - 1;

	// now the stack looks just like it would for a normal message send

	sendMessage(g, selector, numArgsPushed);
	g->numpop = 0;
	return errNone;
}

SCLANG_DEFINE_PRIMITIVE( ObjectDump, 1 )
{
	PyrSlot *a;

	a = g->sp;
	dumpObjectSlot(a);
	return errNone;
}


SCLANG_DEFINE_PRIMITIVE( TotalFree, 1 )
{
	PyrSlot *a = g->sp;
	SetInt(a, g->allocPool->TotalFree());
	return errNone;
}

SCLANG_DEFINE_PRIMITIVE( LargestFreeBlock, 1 )
{
	PyrSlot *a = g->sp;
	SetInt(a, g->allocPool->LargestFreeChunk());
	return errNone;
}

SCLANG_DEFINE_PRIMITIVE( GCInfo, 1 )
{
	g->gc->DumpInfo();
	return errNone;
}

SCLANG_DEFINE_PRIMITIVE( GCDumpGrey, 1 )
{
	g->gc->DumpGrey();
	return errNone;
}

SCLANG_DEFINE_PRIMITIVE( GCDumpSet, 2 )
{
	PyrSlot *b = g->sp;
	int set;
	int err = slotIntVal(b, &set);
	if (err) return err;

	g->gc->DumpSet(set);
	return errNone;
}

SCLANG_DEFINE_PRIMITIVE( GCSanity, 1 )
{
	g->gc->SanityCheck();
	return errNone;
}

#if GCDEBUG
SCLANG_DEFINE_PRIMITIVE( TraceAllPathsTo, 1 )
{
	PyrSlot *a = g->sp;
	g->gc->TracePathsTo(slotRawObject(a), false);
	return errNone;
}

SCLANG_DEFINE_PRIMITIVE( TraceAnyPathsTo, 1 )
{
	PyrSlot *a = g->sp;
	g->gc->TracePathsTo(slotRawObject(a), true);
	return errNone;
}

SCLANG_DEFINE_PRIMITIVE( TraceAnyPathToAllInstancesOf, 1 )
{
	PyrSlot *a = g->sp;
	g->gc->TraceAnyPathToAllInstancesOf(slotRawClass(a)->name.us);
	return errNone;
}
#endif

extern PyrClass *gClassList;

SCLANG_DEFINE_PRIMITIVE( AllClasses, 1 )
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
		classobj = slotRawClass(&classobj->nextclass);
	}
	array->size = gNumClasses;
	SetObject(a, array);
	return errNone;
}

SCLANG_DEFINE_PRIMITIVE( DumpClassSubtree, 1 )
{
	PyrSlot *a;

	a = g->sp;
	postClassTree(slotRawClass(a), 0);
	return errNone;
}

SCLANG_DEFINE_PRIMITIVE( DumpBackTrace, 1 )
{
	PyrSlot *a;

	a = g->sp;
	DumpBackTrace(g);
	return errNone;
}

/* the DebugFrameConstructor uses a work queue in order to avoid recursions, which could lead to stack overflows */
struct DebugFrameConstructor
{
	void makeDebugFrame (VMGlobals *g, PyrFrame *frame, PyrSlot *outSlot)
	{
		workQueue.push_back(std::make_pair(frame, outSlot));
		run_queue(g);
	}

private:
	void run_queue(VMGlobals *g)
	{
		while (!workQueue.empty()) {
			WorkQueueItem work = workQueue.back();
			workQueue.pop_back();
			fillDebugFrame(g, work.first, work.second);
		}
	}

	void fillDebugFrame(VMGlobals *g, PyrFrame *frame, PyrSlot *outSlot)
	{
		PyrMethod *meth = slotRawMethod(&frame->method);
		PyrMethodRaw * methraw = METHRAW(meth);

		PyrObject* debugFrameObj = instantiateObject(g->gc, getsym("DebugFrame")->u.classobj, 0, false, false);
		SetObject(outSlot, debugFrameObj);

		SetObject(debugFrameObj->slots + 0, meth);
		SetPtr(debugFrameObj->slots + 5, meth);

		int numargs = methraw->numargs;
		int numvars = methraw->numvars;
		if (numargs) {
			PyrObject* argArray = (PyrObject*)newPyrArray(g->gc, numargs, 0, false);
			SetObject(debugFrameObj->slots + 1, argArray);
			for (int i=0; i<numargs; ++i)
				slotCopy(&argArray->slots[i], &frame->vars[i]);

			argArray->size = numargs;
		} else
			SetNil(debugFrameObj->slots + 1);

		if (numvars) {
			PyrObject* varArray = (PyrObject*)newPyrArray(g->gc, numvars, 0, false);
			SetObject(debugFrameObj->slots + 2, varArray);
			for (int i=0, j=numargs; i<numvars; ++i,++j)
				slotCopy(&varArray->slots[i], &frame->vars[j]);

			varArray->size = numvars;
		} else
			SetNil(debugFrameObj->slots + 2);

		if (slotRawFrame(&frame->caller)) {
			WorkQueueItem newWork = std::make_pair(slotRawFrame(&frame->caller), debugFrameObj->slots + 3);
			workQueue.push_back(newWork);
		} else
			SetNil(debugFrameObj->slots + 3);

		if (IsObj(&frame->context) && slotRawFrame(&frame->context) == frame)
			SetObject(debugFrameObj->slots + 4, debugFrameObj);
		else if (NotNil(&frame->context)) {
			WorkQueueItem newWork = std::make_pair(slotRawFrame(&frame->context), debugFrameObj->slots + 4);
			workQueue.push_back(newWork);
		} else
			SetNil(debugFrameObj->slots + 4);
	}

	typedef std::pair<PyrFrame*, PyrSlot*> WorkQueueItem;
	typedef std::vector<WorkQueueItem> WorkQueueType;
	WorkQueueType workQueue;
};

static void MakeDebugFrame(VMGlobals *g, PyrFrame *frame, PyrSlot *outSlot)
{
	DebugFrameConstructor constructor;
	constructor.makeDebugFrame(g, frame, outSlot);
}

SCLANG_DEFINE_PRIMITIVE( GetBackTrace, 1 )
{
	PyrSlot *a;

	a = g->sp;
	MakeDebugFrame(g, g->frame, a);

	return errNone;
}

SCLANG_DEFINE_PRIMITIVE( ObjectShallowCopy, 1 )
{
	PyrSlot *a;

	a = g->sp;
	switch (GetTag(a)) {
		case tagObj :
			SetRaw(a, copyObject(g->gc, slotRawObject(a), true));
			break;
		// the default case is to leave the argument unchanged on the stack
	}
	return errNone;
}

SCLANG_DEFINE_PRIMITIVE( ObjectCopyImmutable, 1 )
{
	PyrSlot *a;

	a = g->sp;
	switch (GetTag(a)) {
		case tagObj :
			if (slotRawObject(a)->obj_flags & obj_immutable) {
				SetRaw(a, copyObject(g->gc, slotRawObject(a), true));
			}
			break;
	}
	return errNone;
}

SCLANG_DEFINE_PRIMITIVE( ObjectIsMutable, 1 )
{
	PyrSlot *a;

	a = g->sp;
	if (IsObj(a)) {
		if (slotRawObject(a)->obj_flags & obj_immutable) {
			SetFalse(a);
		} else {
			SetTrue(a);
		}
	} else {
		SetFalse(a);
	}
	return errNone;
}

SCLANG_DEFINE_PRIMITIVE( ObjectIsPermanent, 1 )
{
	PyrSlot *a;

	a = g->sp;
	if (IsObj(a)) {
		if (slotRawObject(a)->gc_color == obj_permanent) {
			SetTrue(a);
		} else {
			SetFalse(a);
		}
	} else {
		SetTrue(a);
	}
	return errNone;
}

SCLANG_DEFINE_PRIMITIVE( ObjectDeepFreeze, 1 )
{
	PyrSlot *a;

	a = g->sp;
	PyrDeepFreezer freezer(g);
	int err = freezer.doDeepFreeze(a);
	return err;
}

SCLANG_DEFINE_PRIMITIVE( ObjectDeepCopy, 1 )
{
	PyrSlot *a;

	a = g->sp;
	PyrDeepCopier copier(g);
	int err = copier.doDeepCopy(a);
	return err;
}

bool IsSimpleLiteralSlot(PyrSlot* slot);
bool IsSimpleLiteralSlot(PyrSlot* slot)
{
	switch (GetTag(slot)) {
		case tagObj : return slotRawObject(slot)->IsPermanent();
		case tagInt : return true;
		case tagSym : return true;
		case tagChar : return true;
		case tagNil : return true;
		case tagFalse : return true;
		case tagTrue : return true;
		case tagPtr : return false;
		default : return true;
	}
}

SCLANG_DEFINE_PRIMITIVE( ObjectCopyRange, 3 )
{
	PyrSlot *a, *b, *c;

	a = g->sp - 2;
	b = g->sp - 1;
	c = g->sp;

	if (NotObj(a)) return errWrongType;
	if (NotInt(b)) return errWrongType;
	if (NotInt(c)) return errWrongType;
	SetRaw(a, copyObjectRange(g->gc, slotRawObject(a), slotRawInt(b), slotRawInt(c), true));

	return errNone;
}


SCLANG_DEFINE_PRIMITIVE( ObjectCopySeries, 4 )
{
	PyrSlot *a, *b, *c, *d;

	a = g->sp - 3;
	b = g->sp - 2;
	c = g->sp - 1;
	d = g->sp;

	PyrObject *inobj = slotRawObject(a);
	PyrObject *newobj;

	int size = inobj->size;
	int flags = ~(obj_immutable) & inobj->obj_flags;

	int first, second, last;

	if (IsInt(b)) first = slotRawInt(b);
	else if (IsNil(b)) first = 0;
	else return errWrongType;

	if (IsInt(d)) {
		last = slotRawInt(d);
		if (last < 0 && IsNil(b)) {
zerolength:
			newobj = g->gc->New(0, flags, inobj->obj_format, true);
			newobj->size = 0;
			newobj->classptr = inobj->classptr;
			SetRaw(a, newobj);
			return errNone;
		}
	} else if (IsNil(d)) {
		if (first >= size) goto zerolength;
		last = size - 1;
	} else return errWrongType;

	if (IsInt(c)) second = slotRawInt(c);
	else if (IsNil(c)) second = first < last ? first + 1 : first - 1;
	else return errWrongType;

	int step = second - first;

	int elemsize = gFormatElemSize[inobj->obj_format];
	int length;

	if (step > 0) {
		length = (last - first) / step + 1;
	} else if (step < 0) {
		length = (first - last) / -step + 1;
	} else return errFailed;

	int numbytes = length * elemsize;

	newobj = g->gc->New(numbytes, flags, inobj->obj_format, true);
	newobj->size = 0;
	newobj->classptr = inobj->classptr;

	for (int i=first, j=0; j<length; i+=step, ++j) {
		PyrSlot slot;
		if (i >= 0 && i < inobj->size) {
			getIndexedSlot(inobj, &slot, i);
			int err = putIndexedSlot(g, newobj, &slot, newobj->size++);
			if (err) return err;
		}
	}

	SetRaw(a, newobj);
	return errNone;
}

void switchToThread(VMGlobals *g, PyrThread *newthread, int oldstate, int *numArgsPushed);

SCLANG_DEFINE_PRIMITIVE( Halt, 1 )
{
	switchToThread(g, slotRawThread(&g->process->mainThread), tDone, &numArgsPushed);
	// return all the way out.
	//PyrSlot *bottom = g->gc->Stack()->slots;
	//slotCopy(bottom,g->sp);
	//g->sp = bottom; // ??!! pop everybody
	g->method = NULL;
	g->block = NULL;
	g->frame = NULL;
	SetNil(g->sp);
	longjmp(g->escapeInterpreter, 3);
	//hmm need to fix this to work only on main thread. //!!!
	//g->sp = g->gc->Stack()->slots - 1;

	return errReturn;
}


SCLANG_DEFINE_PRIMITIVE( CanCallOS, 1 )
{
	PyrSlot *a = g->sp;

	SetBool(a, g->canCallOS);

	return errNone;
}

extern bool gGenerateTailCallByteCodes;

SCLANG_DEFINE_PRIMITIVE( GetTailCallOptimize, 1 )
{
	PyrSlot *a = g->sp;

	SetBool(a, gGenerateTailCallByteCodes);

	return errNone;
}

SCLANG_DEFINE_PRIMITIVE( SetTailCallOptimize, 2 )
{
	//PyrSlot *a = g->sp - 1;

#if TAILCALLOPTIMIZE
	PyrSlot *b = g->sp;
	if (IsTrue(b)) {
		gGenerateTailCallByteCodes = true;
	} else if (IsFalse(b)) {
		gGenerateTailCallByteCodes = false;
	} else return errWrongType;
#endif

	return errNone;
}


SCLANG_DEFINE_PRIMITIVE( Trace, 1 )
{
	PyrSlot *a;

	a = g->sp;
	gTraceInterpreter = IsTrue(a);
	return errNone;
}

SCLANG_DEFINE_PRIMITIVE( KeywordError, 1 )
{
	PyrSlot *a;

	a = g->sp;
	gKeywordError = IsTrue(a);
	return errNone;
}

SCLANG_DEFINE_PRIMITIVE( FunDef_NumArgs, 1 )
{
	PyrSlot *a;
	PyrMethodRaw *methraw;

	a = g->sp;
	methraw = METHRAW(slotRawBlock(a));
	SetInt(a, methraw->numargs);
	return errNone;
}

SCLANG_DEFINE_PRIMITIVE( FunDef_NumVars, 1 )
{
	PyrSlot *a;
	PyrMethodRaw *methraw;

	a = g->sp;
	methraw = METHRAW(slotRawBlock(a));
	SetInt(a, methraw->numvars);
	return errNone;
}

SCLANG_DEFINE_PRIMITIVE( FunDef_VarArgs, 1 )
{
	PyrSlot *a;
	PyrMethodRaw *methraw;

	a = g->sp;
	methraw = METHRAW(slotRawBlock(a));
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

SCLANG_DEFINE_PRIMITIVE( DumpByteCodes, 1 )
{
	PyrSlot *a;

	a = g->sp;
	dumpByteCodes(slotRawBlock(a));
	return errNone;
}

SCLANG_DEFINE_PRIMITIVE( ObjectPointsTo, 2 )
{
	PyrSlot *a, *b, temp;
	PyrObject *obj;
	int i;

	a = g->sp - 1;
	b = g->sp;

	if (NotObj(a)) slotCopy(a,&o_false);
	else {
		obj = slotRawObject(a);
		for (i=0; i<obj->size; ++i) {
			getIndexedSlot(obj, &temp, i);
			if (SlotEq(&temp, b)) {
				slotCopy(a,&o_true);
				return errNone;
			}
		}
		slotCopy(a,&o_false);
	}
	return errNone;
}


SCLANG_DEFINE_PRIMITIVE( ObjectRespondsTo, 2 )
{
	PyrSlot *a, *b;
	PyrClass *classobj;
	PyrMethod *meth;
	PyrSymbol *selector;
	int index;

	a = g->sp - 1;
	b = g->sp;

	classobj = classOfSlot(a);

	if (IsSym(b)) {

		selector = slotRawSymbol(b);
		index = slotRawInt(&classobj->classIndex) + selector->u.index;
		meth = gRowTable[index];
		if (slotRawSymbol(&meth->name) != selector) {
			slotCopy(a,&o_false);
		} else {
			slotCopy(a,&o_true);
		}
	} else if (isKindOfSlot(b, class_array)) {
		int size = slotRawObject(b)->size;
		PyrSlot *slot = slotRawObject(b)->slots;
		for (int i=0; i<size; ++i, ++slot) {

			if (NotSym(slot)) return errWrongType;

			selector = slotRawSymbol(slot);
			index = slotRawInt(&classobj->classIndex) + selector->u.index;
			meth = gRowTable[index];
			if (slotRawSymbol(&meth->name) != selector) {
				slotCopy(a,&o_false);
				return errNone;
			}
		}
		slotCopy(a,&o_true);
	} else return errWrongType;
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
	int index = slotRawInt(&classobj->classIndex) + contextsym->u.index;
	meth = gRowTable[index];
	if (!meth || slotRawSymbol(&meth->name) != contextsym) {
		error("compile context method 'functionCompileContext' not found.\n");
		return 0;
	}
	gCompilingClass = classobj;
	gCompilingMethod = meth;
	gCompilingBlock = (PyrBlock*)meth;
	return meth;
}

#if !SCPLAYER
SCLANG_DEFINE_PRIMITIVE( CompileExpression, 2 )
{
	PyrSlot *a, *b;
	PyrString *string;
	PyrMethod *meth;

	a = g->sp - 1;
	b = g->sp;

	// check b is a string
	if (NotObj(b)) return errWrongType;
	if (!isKindOf(slotRawObject(b),  class_string)) return errWrongType;
	string = slotRawString(b);

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

		((PyrBlockNode*)gRootParseNode)->mIsTopLevel = true;

		SetNil(&slotResult);
		COMPILENODE(gRootParseNode, &slotResult, true);

		if (NotObj(&slotResult)
			|| slotRawObject(&slotResult)->classptr != class_fundef) {
				compileErrors++;
			error("Compile did not return a FunctionDef..\n");
		}
		if (compileErrors) {
			SetNil(a);
		} else {
			PyrBlock *block;
			PyrClosure *closure;

			block = slotRawBlock(&slotResult);
			// create a closure
			closure = (PyrClosure*)g->gc->New(2*sizeof(PyrSlot), 0, obj_notindexed, false);
			closure->classptr = class_func;
			closure->size = 2;
			SetObject(&closure->block, block);
			slotCopy(&closure->context,&slotRawInterpreter(&g->process->interpreter)->context);
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

SCLANG_DEFINE_PRIMITIVE( UGenCodeString, 5 )
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

	PyrObject *inputNamesObj = slotRawObject(cc);
	PyrObject *inputStringsObj = slotRawObject(dd);

	sprintf(ugenPrefix, "u%d", ugenIndex);
	int ugenPrefixSize = strlen(ugenPrefix);
	PyrString* codeStringObj = slotRawString(aa);
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
				if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'))) {
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
				PyrSymbol* inputSym = slotRawSymbol(&inputNameSlot);
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
				PyrString *inputStringObj = slotRawString(inputStringSlot);
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

	state = slotRawInt(&thread->state);
	if (state == tYield) {
		if (!IsObj(&thread->method)) { error("thread method not an Object\n"); }
		else if (!isKindOf(slotRawObject(&thread->method), class_method)) { error("thread method not a Method\n"); }
		else if (slotRawObject(&thread->method)->gc_color == gcColor.gcFree) { error("thread method is FREE\n"); }

		if (!IsObj(&thread->block)) { error("thread block not an Object\n"); }
		else if (!isKindOf(slotRawObject(&thread->block), class_func)) { error("thread block not a Function\n"); }
		else if (slotRawObject(&thread->block)->gc_color == gcColor.gcFree) { error("thread block is FREE\n"); }

		if (IsObj(&thread->receiver) &slotRawObject(&& thread->receiver)->gc_color == gcColor.gcFree)
			{ error("thread receiver is FREE\n"); }

		FrameSanity(thread->frame.uof);

		oldthread->method.uom = g->method;
		oldthread->block.uoblk = g->block;
		SetObject(&oldthread->frame, g->frame);
		slotRawInt(&oldthread->ip) = (int)g->ip;
		slotRawInt(&oldthread->sp) = (int)g->sp;


	} else if (state == tInit) {
	} else {
		postfl("bad state\n");
	}
}*/

void switchToThread(VMGlobals *g, PyrThread *newthread, int oldstate, int *numArgsPushed)
{
	PyrThread *oldthread;
	PyrGC *gc;
	PyrFrame *frame;

#if TAILCALLOPTIMIZE
	g->tailCall = 0; // ?? prevent a crash. is there a way to allow a TCO ?
#endif

	oldthread = g->thread;
	if (newthread == oldthread) return;
	//postfl("->switchToThread %d %p -> %p\n", oldstate, oldthread, newthread);
	//post("->switchToThread from %s:%s\n", slotRawClass(&g->method->ownerclass)->name.us->name, g->slotRawSymbol(&method->name)->name);
	//post("->stack %p  g->sp %p [%d]  g->top %p [%d]\n",
	//	g->gc->Stack()->slots, g->sp, g->sp - g->gc->Stack()->slots, g->top, g->top - g->gc->Stack()->slots);
	//assert(g->gc->SanityCheck());
	//CallStackSanity(g, "switchToThreadA");
	//gcDumpInfo(g->gc);
	gc = g->gc;

	// save environment in oldthread
	PyrSlot* currentEnvironmentSlot = &g->classvars->slots[1];
	slotCopy(&oldthread->environment,currentEnvironmentSlot);
	gc->GCWrite(oldthread, currentEnvironmentSlot);

	SetRaw(&oldthread->state, oldstate);

	if (oldstate == tDone) {
		SetObject(&oldthread->stack, gc->Stack());
		gc->ToWhite(gc->Stack());
		gc->Stack()->size = 0;
		gc->GCWrite(oldthread, gc->Stack());
		SetNil(&oldthread->method);
		SetNil(&oldthread->block);
		SetNil(&oldthread->receiver);
		SetNil(&oldthread->frame);
		SetRaw(&oldthread->ip, (void*)0);
		SetRaw(&oldthread->sp, (void*)0);
		SetRaw(&oldthread->numArgsPushed, 0);
		SetRaw(&oldthread->numpop, 0);
		SetNil(&oldthread->parent);
	} else if (oldstate == tInit) {
		SetObject(&oldthread->stack, gc->Stack());
		gc->ToWhite(gc->Stack());
		gc->Stack()->size = 0;
		gc->GCWrite(oldthread, gc->Stack());
		SetNil(&oldthread->method);
		SetNil(&oldthread->block);
		SetNil(&oldthread->receiver);
		SetNil(&oldthread->frame);
		SetRaw(&oldthread->ip, (void*)0);
		SetRaw(&oldthread->sp, (void*)0);
		SetRaw(&oldthread->numArgsPushed, 0);
		SetRaw(&oldthread->numpop, 0);
		SetNil(&oldthread->parent);
	} else {
		// save old thread's state
		SetObject(&oldthread->stack, gc->Stack());
		gc->ToWhite(gc->Stack());
		gc->Stack()->size = g->sp - gc->Stack()->slots + 1;
		//post("else %p %p\n", slotRawObject(&oldthread->stack), gc->Stack());

		SetObject(&oldthread->method, g->method);
		SetObject(&oldthread->block, g->block);
		SetObject(&oldthread->frame, g->frame);
		SetPtr(&oldthread->ip, g->ip);
		SetPtr(&oldthread->sp, g->sp);
		slotCopy(&oldthread->receiver,&g->receiver);
		SetRaw(&oldthread->numArgsPushed, *numArgsPushed);
		SetRaw(&oldthread->numpop, g->numpop);

		//gc->ToGrey(oldthread);
		if (gc->ObjIsBlack(oldthread)) {
			gc->GCWriteBlack(gc->Stack());
			gc->GCWriteBlack(g->method);
			gc->GCWriteBlack(g->block);

			frame = slotRawFrame(&oldthread->frame);
			gc->GCWriteBlack(frame);

			gc->GCWriteBlack(&g->receiver);
		}
	}

	// restore new thread's state
	g->thread = newthread;
	SetObject(&g->process->curThread, newthread);
	gc->GCWrite(g->process, newthread);

	gc->SetStack(slotRawObject(&newthread->stack));
	gc->ToBlack(gc->Stack());
	SetNil(&newthread->stack);

	g->method = slotRawMethod(&newthread->method);
	g->block = slotRawBlock(&newthread->block);
	g->frame = slotRawFrame(&newthread->frame);
	g->ip = (unsigned char *)slotRawPtr(&newthread->ip);
	g->sp = (PyrSlot*)slotRawPtr(&newthread->sp);
	slotCopy(&g->receiver,&newthread->receiver);

	g->rgen = (RGen*)(slotRawObject(&newthread->randData)->slots);

	*numArgsPushed = slotRawInt(&newthread->numArgsPushed);

	// these are perhaps unecessary because a thread may not
	// legally block within a C primitive
	g->numpop = slotRawInt(&newthread->numpop);

	g->execMethod = 99;

	//post("switchToThread ip %p\n", g->ip);
	//post(slotRawInt(&"switchToThread newthread->ip) %d\n", slotRawInt(&newthread->ip));
	//post(slotRawInt(&"switchToThread oldthread->ip) %d\n", slotRawInt(&oldthread->ip));

	// wipe out values which will become stale as new thread executes:
	SetNil(&newthread->method);
	SetNil(&newthread->block);
	SetNil(&newthread->frame);
	SetRaw(&newthread->ip, (void*)0);
	SetRaw(&newthread->sp, (void*)0);
	SetNil(&newthread->receiver);

	SetRaw(&newthread->state, tRunning);


        // set new environment
        slotCopy(currentEnvironmentSlot,&g->thread->environment);
        g->gc->GCWrite(g->classvars, currentEnvironmentSlot);

	//post("old thread %p stack %p\n", oldthread, slotRawObject(&oldthread->stack));
	//post("new thread %p stack %p\n", g->thread, slotRawObject(&g->thread->stack));
	//post("main thread %p stack %p\n", slotRawThread(&g->process->mainThread), slotRawObject(&slotRawThread(&g->process->mainThread)->stack));

	//postfl("<-switchToThread\n");
	//post("<-stack %p  g->sp %p [%d]  g->top %p [%d]\n",
	//	g->gc->Stack()->slots, g->sp, g->sp - g->gc->Stack()->slots, g->top, g->top - g->gc->Stack()->slots);
	//assert(g->gc->SanityCheck());
	//CallStackSanity(g, "switchToThreadB");
	//post("switchToThread ip2 %p\n", g->ip);
}

void initPyrThread(VMGlobals *g, PyrThread *thread, PyrSlot *func, int stacksize, PyrInt32Array* rgenArray,
	double beats, double seconds, PyrSlot* clock, bool runGC);
void initPyrThread(VMGlobals *g, PyrThread *thread, PyrSlot *func, int stacksize, PyrInt32Array* rgenArray,
	double beats, double seconds, PyrSlot* clock, bool runGC)
{
	PyrObject *array;
	PyrGC* gc = g->gc;

	slotCopy(&thread->func, func);
	gc->GCWrite(thread, func);

	array = newPyrArray(gc, stacksize, 0, runGC);
	SetObject(&thread->stack, array);
	gc->GCWriteNew(thread, array); // we know array is white so we can use GCWriteNew

	SetInt(&thread->state, tInit);

	SetPtr(&thread->ip, 0);
	SetPtr(&thread->sp, 0);

	SetObject(&thread->randData, rgenArray);
	gc->GCWrite(thread, rgenArray);

	SetFloat(&thread->beats, beats);
	SetFloat(&thread->seconds, seconds);
	SetInt(&thread->numArgsPushed, 0);
	SetInt(&thread->numpop, 0);

	if (IsNil(clock)) {
		SetObject(&thread->clock, s_systemclock->u.classobj);
	} else {
		slotCopy(&thread->clock,clock);
		gc->GCWrite(thread, clock);
	}

	PyrSlot* currentEnvironmentSlot = &g->classvars->slots[1];
	slotCopy(&thread->environment,currentEnvironmentSlot);
	gc->GCWrite(thread, currentEnvironmentSlot);

	if(g->process) { // check we're not just starting up
		slotCopy(&thread->executingPath,&g->process->nowExecutingPath);
		gc->GCWrite(thread, &g->process->nowExecutingPath);
	}

	SetInt(&thread->stackSize, stacksize);
}

extern PyrSymbol *s_prstart;

SCLANG_DEFINE_PRIMITIVE( Thread_Init, 3 )
{
	PyrSlot *a, *b, *c;
	int stacksize, err;
	PyrThread *thread;

	//postfl("->prThreadInit\n");
	//assert(g->gc->SanityCheck());
	//CallStackSanity(g, "prThreadInit");
	a = g->sp - 2;	// thread
	b = g->sp - 1;	// function
	c = g->sp;	// stacksize

	thread = slotRawThread(a);

	if (NotObj(b) || !isKindOf(slotRawObject(b), class_func)) {
		error("Thread function arg not a Function.\n");
		return errWrongType;
	}

	err = slotIntVal(c, &stacksize);
	if (err) return err;

	stacksize = std::max(stacksize, EVALSTACKDEPTH);

	double beats, seconds;
	err = slotDoubleVal(&g->thread->beats, &beats);
	if (err) return err;
	err = slotDoubleVal(&g->thread->seconds, &seconds);
	if (err) return err;

	initPyrThread(g, thread, b, stacksize, (PyrInt32Array*)(slotRawObject(&g->thread->randData)),
	beats, seconds, &g->thread->clock, true);

	//postfl("<-prThreadInit\n");
	//assert(g->gc->SanityCheck());
	//CallStackSanity(g, "<prThreadInit");
	return errNone;
}

SCLANG_DEFINE_PRIMITIVE( Thread_RandSeed, 2 )
{
	PyrSlot *a = g->sp - 1;	// thread
	PyrSlot *b = g->sp;		// rand seed

	PyrThread *thread = slotRawThread(a);

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
	g->gc->GCWriteNew(thread, rgenArray); // we know rgenArray is white so we can use GCWriteNew

	return errNone;
}

SCLANG_DEFINE_PRIMITIVE( Thread_GetRandData, 1 )
{
	PyrSlot *a = g->sp;	// thread

	PyrThread *thread = slotRawThread(a);

	RGen* rgen = (RGen*)slotRawObject(&thread->randData)->slots;

	PyrInt32Array *rgenArray = newPyrInt32Array(g->gc, 4, 0, false);
	rgenArray->size = 3;

	rgenArray->i[0] = rgen->s1;
	rgenArray->i[1] = rgen->s2;
	rgenArray->i[2] = rgen->s3;

	SetObject(a, rgenArray);
	return errNone;
}

SCLANG_DEFINE_PRIMITIVE( Thread_SetRandData, 2 )
{
	PyrSlot *a = g->sp - 1;	// thread
	PyrSlot *b = g->sp;		// rand data array

	if (!isKindOfSlot(b, class_int32array)) return errWrongType;
	if (slotRawObject(b)->size < 3) return errWrongType;

	PyrThread *thread = slotRawThread(a);

	RGen* rgen = (RGen*)slotRawObject(&thread->randData)->slots;

	PyrInt32Array *rgenArray = (PyrInt32Array*)slotRawObject(b);

	rgen->s1 = rgenArray->i[0];
	rgen->s2 = rgenArray->i[1];
	rgen->s3 = rgenArray->i[2];

	return errNone;
}

#if 0
int32 timeseed();

int transformMainThreadToRoutine(VMGlobals *g)
{
	PyrProcess* process = g->process;
	if (g->thread != process->mainThread.uot) return errFailed;
	//if (g->thread != process->curThread.uot) return errFailed;

	PyrThread* curthread = (PyrThread*)slotRawObject(&process->mainThread);

	// create a new main thread
	PyrThread* newthread = (PyrThread*)instantiateObject(g->gc, class_thread, 0, true, false);

	PyrInt32Array *rgenArray = newPyrInt32Array(g->gc, 4, 0, false);
	rgenArray->size = 4;
	((RGen*)(rgenArray->i))->init(timeseed());

	PyrSlot clockSlot;
	SetObject(&clockSlot, s_systemclock->u.classobj);
	initPyrThread(g, newthread, &o_nil, EVALSTACKDEPTH, rgenArray, 0., 0., &clockSlot, false);
	slotRawInt(&newthread->sp) = (int)slotRawObject(&newthread->stack)->slots - 1;
	SetObject(&process->mainThread, newthread);
	g->gc->GCWrite(process, newthread);

	curthread->classptr = class_routine;
	PyrSlot *cmdFunc = &process->interpreter.uoi->cmdFunc;
	slotCopy(&curthread->func,cmdFunc);
	g->gc->GCWrite(curthread, cmdFunc);

	return errNone;
}

void schedAdd(VMGlobals *g, PyrObject* inQueue, double inSeconds, PyrSlot* inTask);
#endif


SCLANG_DEFINE_PRIMITIVE( RoutineYield, 1 )
{
	PyrSlot value;

	//postfl("->prRoutineYield %p\n", g->thread);
	//assert(g->gc->SanityCheck());
	//CallStackSanity(g, "prRoutineYield");
	//postfl("->numArgsPushed %d\n", numArgsPushed);

	slotCopy(&value,g->sp);

	if (!isKindOf((PyrObject*)g->thread, class_routine)) {
		error ("yield was called outside of a Routine.\n");
		return errFailed;
	}

	PyrThread *parent = slotRawThread(&g->thread->parent);
	SetNil(&g->thread->parent);
	slotCopy(&g->process->nowExecutingPath, &g->thread->oldExecutingPath);
	//debugf("yield from thread %p to parent %p\n", g->thread, slotRawThread(&g->thread->parent));
	switchToThread(g, parent, tSuspended, &numArgsPushed);

	// on the other side of the looking glass, put the yielded value on the stack as the result..
	slotCopy((g->sp - numArgsPushed + 1),&value);

	//postfl("<-numArgsPushed %d\n", numArgsPushed);
	//postfl("<-prRoutineYield\n");
	//assert(g->gc->SanityCheck());
	//CallStackSanity(g, "<prRoutineYield");
	return errNone;
}

SCLANG_DEFINE_PRIMITIVE( RoutineAlwaysYield, 1 )
{
	PyrSlot value;

	//postfl("->prRoutineAlwaysYield ip %p\n", g->ip);
	//assert(g->gc->SanityCheck());
	//CallStackSanity(g, "prRoutineAlwaysYield");
	if (!isKindOf((PyrObject*)g->thread, class_routine)) {
		error ("alwaysYield was called outside of a Routine.\n");
		return errFailed;
	}

	slotCopy(&value,g->sp);
	slotCopy(&g->thread->terminalValue,&value);
	g->gc->GCWrite(g->thread, g->sp);

	PyrThread *parent = slotRawThread(&g->thread->parent);
	SetNil(&g->thread->parent);
	slotCopy(&g->process->nowExecutingPath, &g->thread->oldExecutingPath);
	//post("alwaysYield from thread %p to parent %p\n", g->thread, parent);
	switchToThread(g, parent, tDone, &numArgsPushed);

	// on the other side of the looking glass, put the yielded value on the stack as the result..
	slotCopy((g->sp - numArgsPushed + 1),&value);

	//postfl("<-prRoutineAlwaysYield ip %p\n", g->ip);
	//assert(g->gc->SanityCheck());
	//CallStackSanity(g, "<prRoutineAlwaysYield");
	return errNone;
}

SCLANG_DEFINE_PRIMITIVE( RoutineResume, 2 )
{
	PyrSlot *a, *b, threadSlot, value;
	PyrThread *thread;
	int state;

	//assert(g->gc->SanityCheck());
	//CallStackSanity(g, "prRoutineResume");
	a = g->sp - 1;
	b = g->sp;

	thread = slotRawThread(a);
	state = slotRawInt(&thread->state);
	//postfl("->prRoutineResume %d\n", state);

	slotCopy(&thread->oldExecutingPath,&g->process->nowExecutingPath);
	slotCopy(&g->process->nowExecutingPath, &thread->executingPath);
	if (state == tInit) {
		slotCopy(&threadSlot,a);
		slotCopy(&value,b);

		//post("g->thread %p\n", g->thread);
		//post("thread %p\n", thread);
		SetObject(&thread->parent, g->thread);
		g->gc->GCWrite(thread, g->thread);

		slotCopy(&thread->beats, &g->thread->beats);
		slotCopy(&thread->seconds, &g->thread->seconds);
		slotCopy(&thread->clock, &g->thread->clock);
		g->gc->GCWrite(thread, &g->thread->beats);
		g->gc->GCWrite(thread, &g->thread->seconds);
		g->gc->GCWrite(thread, &g->thread->clock);

		//postfl("start into thread %p from parent %p\n", thread, g->thread);
		switchToThread(g, thread, tSuspended, &numArgsPushed);

		// set stack
		//post("set stack %p %p\n", g->sp, g->gc->Stack()->slots - 1);
		g->sp = g->gc->Stack()->slots - 1;
		slotCopy((++g->sp), &threadSlot);
		slotCopy(&g->receiver, &threadSlot);
		slotCopy((++g->sp),&value);

		sendMessage(g, s_prstart, 2);
	} else if (state == tSuspended) {

		if (IsNil(&thread->parent)) {
			SetObject(&thread->parent, g->thread);
		}
		g->gc->GCWrite(thread, g->thread);

		slotCopy(&thread->beats, &g->thread->beats);
		slotCopy(&thread->seconds, &g->thread->seconds);
		slotCopy(&thread->clock,&g->thread->clock);
		g->gc->GCWrite(thread, &g->thread->beats);
		g->gc->GCWrite(thread, &g->thread->seconds);
		g->gc->GCWrite(thread, &g->thread->clock);

		slotCopy(&value,b);
	//debugf("resume into thread %p from parent %p\n", thread, g->thread);
		switchToThread(g, thread, tSuspended, &numArgsPushed);
		// on the other side of the looking glass, put the yielded value on the stack as the result..
		slotCopy((g->sp - numArgsPushed + 1),&value);
	} else if (state == tDone) {
		slotCopy(a,&thread->terminalValue);
	} else if (state == tRunning) {
		error("Tried to resume the running thread.\n");
		return errFailed;
	} else {
		error("Thread in strange state: %d\n", state);
		return errFailed;
	}
	//postfl("<-prRoutineResume\n");
	//CallStackSanity(g);
	return errNone;
}

SCLANG_DEFINE_PRIMITIVE( RoutineReset, 1 )
{
	PyrThread *thread;
	int state;

	//assert(g->gc->SanityCheck());
	//CallStackSanity(g, "prRoutineReset");
	thread = slotRawThread(g->sp);
	state = slotRawInt(&thread->state);
	//post("->prRoutineReset %d\n", state);
	if (state == tSuspended) {
		SetRaw(&thread->state, tInit);
		slotRawObject(&thread->stack)->size = 0;
		SetNil(&thread->method);
		SetNil(&thread->block);
		SetNil(&thread->receiver);
		SetNil(&thread->frame);
		SetRaw(&thread->ip, (void*)0);
		SetRaw(&thread->sp, (void*)0);
		SetRaw(&thread->numArgsPushed, 0);
		SetRaw(&thread->numpop, 0);
		SetNil(&thread->parent);
	} else if (state == tDone) {
		SetRaw(&thread->state, tInit);
		slotRawObject(&thread->stack)->size = 0;
		SetNil(&thread->method);
		SetNil(&thread->block);
		SetNil(&thread->receiver);
		SetNil(&thread->frame);
		SetRaw(&thread->ip, (void*)0);
		SetRaw(&thread->sp, (void*)0);
		SetRaw(&thread->numArgsPushed, 0);
		SetRaw(&thread->numpop, 0);
		SetNil(&thread->parent);
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
	//CallStackSanity(g, "<prRoutineReset");
	return errNone;
}

SCLANG_DEFINE_PRIMITIVE( RoutineStop, 1 )
{
	PyrThread *thread;
	int state;
	//post("->prRoutineStop\n");
	//assert(g->gc->SanityCheck());
	//CallStackSanity(g, "prRoutineStop");

	thread = slotRawThread(g->sp);
	state = slotRawInt(&thread->state);


	if (state == tSuspended || state == tInit) {
		SetNil(&g->thread->terminalValue);
		SetRaw(&thread->state, tDone);
		slotRawObject(&thread->stack)->size = 0;
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
	//CallStackSanity(g, "<prRoutineStop");
	return errNone;
}

SCLANG_DEFINE_PRIMITIVE( RoutineYieldAndReset, 2 )
{
	PyrSlot *a, *b, value;
	int state;

	//post("->prRoutineYieldAndReset\n");
	//assert(g->gc->SanityCheck());
	//CallStackSanity(g, "prRoutineYieldAndReset");
	a = g->sp - 1;
	b = g->sp;

	if (!isKindOf((PyrObject*)g->thread, class_routine)) {
		error ("yieldAndReset was called outside of a Routine.\n");
		return errFailed;
	}
	/*if (!slotRawThread(&g->thread->parent)) {
		error ("yieldAndReset was called from a thread with no parent.\n");
		return errFailed;
	}*/
	slotCopy(&value,a);

	if (IsFalse(b)) state = tSuspended;
	else state = tInit;

	PyrThread *parent = slotRawThread(&g->thread->parent);
	SetNil(&g->thread->parent);
	switchToThread(g, parent, state, &numArgsPushed);
	// on the other side of the looking glass, put the yielded value on the stack as the result..
	slotCopy((g->sp - numArgsPushed + 1),&value);

	//slotCopy(&g->process->nowExecutingPath, &g->thread->oldExecutingPath);

	//post("<-prRoutineYieldAndReset\n");
	//assert(g->gc->SanityCheck());
	//CallStackSanity(g, "prRoutineYieldAndReset");
	return errNone;
}

bool gBlork = false;

SCLANG_DEFINE_PRIMITIVE( Blork, 1 )
{
	PyrSlot *a;

	a = g->sp;
	if (IsTrue(a)) gBlork = true;
	else gBlork = false;

	return errNone;
}

SCLANG_DEFINE_PRIMITIVE( MainOverwriteMsg, 1 )
{
	PyrSlot *a = g->sp;
	PyrString* string = newPyrString(g->gc, overwriteMsg.c_str(), 0, false);
	SetObject(a, string);
	return errNone;
}

SCLANG_DEFINE_PRIMITIVE( AppClock_SchedNotify, 1 )
{
	//NOTE: the _AppClock_SchedNotify primitive shall be redefined by language clients
	// if they wish to respond to AppClock scheduling notifications
	return errNone;
}

enum {includePaths, excludePaths};

static int prLanguageConfig_getLibraryPaths(struct VMGlobals * g, int numArgsPushed, int pathType)
{
	PyrSlot *result = g->sp;

	typedef SC_LanguageConfig::DirVector DirVector;

	const DirVector& dirVector = (pathType == includePaths) ? gLanguageConfig->includedDirectories()
															 : gLanguageConfig->excludedDirectories();

	size_t numberOfPaths = dirVector.size();
	PyrObject * resultArray = newPyrArray(g->gc, numberOfPaths, 0, true);
	SetObject(result, resultArray);

	for (size_t i = 0; i != numberOfPaths; ++i) {
		const std::string& utf8_path = SC_Codecvt::path_to_utf8_str(dirVector[i]);
		PyrString * pyrString = newPyrString(g->gc, utf8_path.c_str(), 0, true);
		SetObject(resultArray->slots + i, pyrString);
		g->gc->GCWriteNew( resultArray,  pyrString ); // we know pyrString is white so we can use GCWriteNew
		resultArray->size++;
	}
	return errNone;
}

SCLANG_DEFINE_PRIMITIVE( LanguageConfig_getIncludePaths, 1 )
{
	return prLanguageConfig_getLibraryPaths(g, numArgsPushed, includePaths);
}

SCLANG_DEFINE_PRIMITIVE( LanguageConfig_getExcludePaths, 1 )
{
	return prLanguageConfig_getLibraryPaths(g, numArgsPushed, excludePaths);
}

static int prLanguageConfig_addLibraryPath(struct VMGlobals * g, int numArgsPushed, int pathType)
{
	PyrSlot *removeString = g->sp;

	char path[MAXPATHLEN];
	bool error = slotStrVal(removeString, path, MAXPATHLEN);
	if (error)
		return errWrongType;

	const bfs::path& native_path = SC_Codecvt::utf8_str_to_path(path);
	if (pathType == includePaths)
		gLanguageConfig->addIncludedDirectory(native_path);
	else
		gLanguageConfig->addExcludedDirectory(native_path);
	return errNone;
}

SCLANG_DEFINE_PRIMITIVE( LanguageConfig_addIncludePath, 2 )
{
	return prLanguageConfig_addLibraryPath(g, numArgsPushed, includePaths);
}

SCLANG_DEFINE_PRIMITIVE( LanguageConfig_addExcludePath, 2 )
{
	return prLanguageConfig_addLibraryPath(g, numArgsPushed, excludePaths);
}

static int prLanguageConfig_removeLibraryPath(struct VMGlobals * g, int numArgsPushed, int pathType)
{
	PyrSlot *dirString = g->sp;

	char path[MAXPATHLEN];
	bool error = slotStrVal(dirString, path, MAXPATHLEN);
	if (error)
		return errWrongType;

	const bfs::path& native_path = SC_Codecvt::utf8_str_to_path(path);
	if (pathType == includePaths)
		gLanguageConfig->removeIncludedDirectory(native_path);
	else
		gLanguageConfig->removeExcludedDirectory(native_path);
	return errNone;
}

SCLANG_DEFINE_PRIMITIVE( LanguageConfig_removeIncludePath, 2 )
{
	return prLanguageConfig_removeLibraryPath(g, numArgsPushed, includePaths);
}

SCLANG_DEFINE_PRIMITIVE( LanguageConfig_removeExcludePath, 2 )
{
	return prLanguageConfig_removeLibraryPath(g, numArgsPushed, excludePaths);
}

SCLANG_DEFINE_PRIMITIVE( LanguageConfig_getCurrentConfigPath, 1 )
{
	PyrSlot *a = g->sp;
	const std::string& config_path = SC_Codecvt::path_to_utf8_str(gLanguageConfig->getConfigPath());
	PyrString* str = newPyrString(g->gc, config_path.c_str(), 0, false);
    if(str->size == 0) {
        SetNil(a);
    } else {
        SetObject(a, str);
    }

	return errNone;
}

SCLANG_DEFINE_PRIMITIVE( LanguageConfig_writeConfigFile, 2 )
{
	PyrSlot *fileString = g->sp;
	bfs::path config_path;

	if (NotNil(fileString)) {
		char path[MAXPATHLEN];
		bool error = slotStrVal(fileString, path, MAXPATHLEN);
		if (error)
			return errWrongType;

		config_path = SC_Codecvt::utf8_str_to_path(path);
		gLanguageConfig->writeLibraryConfigYAML(config_path);
	} else {
		config_path =
			SC_Filesystem::instance().getDirectory(SC_Filesystem::DirName::UserConfig)
			/ "sclang_conf.yaml";
	}

	if (!gLanguageConfig->writeLibraryConfigYAML(config_path))
		return errFailed;
	else
		return errNone;
}

SCLANG_DEFINE_PRIMITIVE( LanguageConfig_getPostInlineWarnings, 1 )
{
	PyrSlot *result = g->sp;
	SetBool(result, SC_LanguageConfig::getPostInlineWarnings());
	return errNone;
}

SCLANG_DEFINE_PRIMITIVE( LanguageConfig_setPostInlineWarnings, 2 )
{
	PyrSlot *arg = g->sp;

	if (IsTrue(arg))
		SC_LanguageConfig::setPostInlineWarnings(true);
	else if (IsFalse(arg))
		SC_LanguageConfig::setPostInlineWarnings(false);
	else
		return errWrongType;

	return errNone;
}

SCLANG_DEFINE_PRIMITIVE( SC_VersionMajor, 1 )
{
	PyrSlot *result = g->sp;
	SetInt(result, SC_VersionMajor);
	return errNone;
}

SCLANG_DEFINE_PRIMITIVE( SC_VersionMinor, 1 )
{
	PyrSlot *result = g->sp;
	SetInt(result, SC_VersionMinor);
	return errNone;
}

SCLANG_DEFINE_PRIMITIVE( SC_VersionPatch, 1 )
{
	PyrSlot *result = g->sp;
	SetObject(result, newPyrString(g->gc, SC_VersionPatch, 0, 1));
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

int definePrimitive(int base, int index, const char *name, PrimitiveHandler handler,
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

int definePrimitiveWithKeys(int base, int index, const char *name,
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


#ifdef GC_SANITYCHECK
	g->gc->SanityCheck();
#endif

	//post("doPrimitive %s:%s\n", slotRawSymbol(&slotRawClass(&meth->ownerclass)->name)->name, slotRawSymbol(&meth->name)->name);

	PyrMethodRaw *methraw = METHRAW(meth);
	int primIndex = methraw->specialIndex;

	PrimitiveDef *def = gPrimitiveTable.table + primIndex;
	int numArgsNeeded = def->numArgs;
	int diff = numArgsNeeded - numArgsPushed;

	if (diff != 0) { // incorrect num of args
		if (diff > 0) {  // not enough args
			PyrSlot* pslot = g->sp;
			PyrSlot* qslot = slotRawObject(&meth->prototypeFrame)->slots + numArgsPushed - 1;
			for (int m=0; m<diff; ++m) slotCopy(++pslot, ++qslot);

			g->sp += diff;
		} else if (def->varArgs) { // has var args
			numArgsNeeded = numArgsPushed;
		} else {
			g->sp += diff; // remove excess args
		}
	}
	g->numpop = numArgsNeeded - 1;
	g->primitiveIndex = primIndex - def->base;
	g->primitiveMethod = meth;
	g->args = g->sp - numArgsNeeded;
	int err;
	try {
#ifdef GC_SANITYCHECK
	g->gc->SanityCheck();
#endif
		err = (*def->func)(g, numArgsNeeded);
#ifdef GC_SANITYCHECK
	g->gc->SanityCheck();
#endif
	} catch (std::exception& ex) {
		g->lastExceptions[g->thread] = std::make_pair(std::current_exception(), meth);
		err = errException;
	} catch (...) {
		g->lastExceptions[g->thread] = std::make_pair(nullptr, meth);
		err = errException;
	}
	if (err <= errNone) g->sp -= g->numpop;
	else {
		//post("primitive failed %s:%s\n", slotRawSymbol(&slotRawClass(&meth->ownerclass)->name)->name, slotRawSymbol(&meth->name)->name);
		SetInt(&g->thread->primitiveIndex, methraw->specialIndex);
		SetInt(&g->thread->primitiveError, err);
		executeMethod(g, meth, numArgsNeeded);
	}
#ifdef GC_SANITYCHECK
	g->gc->SanityCheck();
#endif
}

void doPrimitiveWithKeys(VMGlobals* g, PyrMethod* meth, int allArgsPushed, int numKeyArgsPushed)
{
	int i, j, m, diff, err;
	PyrSlot *pslot, *qslot;
	int numArgsNeeded, numArgsPushed;

#ifdef GC_SANITYCHECK
	g->gc->SanityCheck();
#endif
	//post("doPrimitive %s:%s\n", slotRawSymbol(&slotRawClass(&meth->ownerclass)->name)->name, slotRawSymbol(&meth->name)->name);
        //printf("doPrimitive %s:%s\n", slotRawSymbol(&slotRawClass(&meth->ownerclass)->name)->name, slotRawSymbol(&meth->name)->name);

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
			g->lastExceptions[g->thread] = std::make_pair(std::current_exception(), meth);
			err = errException;
		} catch (...) {
			g->lastExceptions[g->thread] = std::make_pair(nullptr, meth);
			err = errException;
		}
		if (err <= errNone) g->sp -= g->numpop;
		else {
			//post("primerr %d\n", err);
			SetInt(&g->thread->primitiveIndex, methraw->specialIndex);
			SetInt(&g->thread->primitiveError, err);
			executeMethodWithKeys(g, meth, allArgsPushed, numKeyArgsPushed);
		}
	} else {
		numArgsNeeded = def->numArgs;
		numArgsPushed = allArgsPushed - (numKeyArgsPushed << 1);

		if (numKeyArgsPushed) {
			// evacuate keyword args to separate area
			pslot = keywordstack + (numKeyArgsPushed<<1);
			qslot = g->sp + 1;
			for (m=0; m<numKeyArgsPushed; ++m) {
				slotCopy(--pslot, --qslot);
				slotCopy(--pslot, --qslot);
			}
		}

		diff = numArgsNeeded - numArgsPushed;
		if (diff != 0) { // incorrect num of args
			if (diff > 0) {  // not enough args
				g->sp += numArgsNeeded - allArgsPushed; // remove excess args
				pslot = g->sp - diff;
				qslot = slotRawObject(&meth->prototypeFrame)->slots + numArgsPushed - 1;
				for (m=0; m<diff; ++m) slotCopy(++pslot, ++qslot);
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
			name0 = slotRawSymbolArray(&meth->argNames)->symbols + 1;
			key = keywordstack;
			vars = g->sp - numArgsNeeded + 1;
			for (i=0; i<numKeyArgsPushed; ++i, key+=2) {
				name = name0;
				for (j=1; j<methraw->posargs; ++j, ++name) {
					if (*name == slotRawSymbol(key)) {
						slotCopy(&vars[j],&key[1]);
						goto found;
					}
				}
				if (gKeywordError) {
					post("WARNING: keyword arg '%s' not found in call to %s:%s\n",
						slotRawSymbol(key)->name, slotRawSymbol(&slotRawClass(&meth->ownerclass)->name)->name, slotRawSymbol(&meth->name)->name);
				}
				found: ;
			}
		}
		g->numpop = numArgsNeeded - 1;
		try {
			err = (*def->func)(g, numArgsNeeded);
		} catch (std::exception& ex) {
			g->lastExceptions[g->thread] = std::make_pair(std::current_exception(), meth);
			err = errException;
		} catch (...) {
			g->lastExceptions[g->thread] = std::make_pair(nullptr, meth);
			err = errException;
		}
		if (err <= errNone) g->sp -= g->numpop;
		else {
			//post("primerr %d\n", err);
			SetInt(&g->thread->primitiveIndex, methraw->specialIndex);
			SetInt(&g->thread->primitiveError, err);
			executeMethod(g, meth, numArgsNeeded);
		}
	}
#ifdef GC_SANITYCHECK
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

	definePrimitive(base, opIDiv, "_IDiv", prSpecialBinaryArithMsg, 3, 0);
	definePrimitive(base, opFDiv, "_FDiv", prSpecialBinaryArithMsg, 3, 0);
	definePrimitive(base, opMod, "_Mod", prSpecialBinaryArithMsg, 3, 0);
	definePrimitive(base, opEQ, "_EQ", prSpecialBinaryArithMsg, 3, 0);
	definePrimitive(base, opNE, "_NE", prSpecialBinaryArithMsg, 3, 0);
	definePrimitive(base, opLT, "_LT", prSpecialBinaryArithMsg, 3, 0);
	definePrimitive(base, opGT, "_GT", prSpecialBinaryArithMsg, 3, 0);
	definePrimitive(base, opLE, "_LE", prSpecialBinaryArithMsg, 3, 0);
	definePrimitive(base, opGE, "_GE", prSpecialBinaryArithMsg, 3, 0);
	//definePrimitive(base, opIdentical, "_Identical", prSpecialBinaryArithMsg, 3, 0);
	//definePrimitive(base, opNotIdentical, "_NotIdentical", prSpecialBinaryArithMsg, 3, 0);

	definePrimitive(base, opMin, "_Min", prSpecialBinaryArithMsg, 3, 0);
	definePrimitive(base, opMax, "_Max", prSpecialBinaryArithMsg, 3, 0);
	definePrimitive(base, opBitAnd, "_BitAnd", prSpecialBinaryArithMsg, 3, 0);
	definePrimitive(base, opBitOr, "_BitOr", prSpecialBinaryArithMsg, 3, 0);
	definePrimitive(base, opBitXor, "_BitXor", prSpecialBinaryArithMsg, 3, 0);
	definePrimitive(base, opLCM, "_LCM", prSpecialBinaryArithMsg, 3, 0);
	definePrimitive(base, opGCD, "_GCD", prSpecialBinaryArithMsg, 3, 0);
	definePrimitive(base, opRound, "_Round", prSpecialBinaryArithMsg, 3, 0);
	definePrimitive(base, opRoundUp, "_RoundUp", prSpecialBinaryArithMsg, 3, 0);
	definePrimitive(base, opTrunc, "_Trunc", prSpecialBinaryArithMsg, 3, 0);
	definePrimitive(base, opAtan2, "_Atan2", prSpecialBinaryArithMsg, 3, 0);
	definePrimitive(base, opHypot, "_Hypot", prSpecialBinaryArithMsg, 3, 0);
	definePrimitive(base, opHypotx, "_HypotApx", prSpecialBinaryArithMsg, 3, 0);
	definePrimitive(base, opPow, "_Pow", prSpecialBinaryArithMsg, 3, 0);
	definePrimitive(base, opShiftLeft, "_ShiftLeft", prSpecialBinaryArithMsg, 3, 0);
	definePrimitive(base, opShiftRight, "_ShiftRight", prSpecialBinaryArithMsg, 3, 0);
	definePrimitive(base, opUnsignedShift, "_UnsignedShift", prSpecialBinaryArithMsg, 3, 0);
	definePrimitive(base, opFill, "_Fill", prSpecialBinaryArithMsg, 3, 0);
	definePrimitive(base, opRing1, "_Ring1", prSpecialBinaryArithMsg, 3, 0);	// a * (b + 1) == a * b + a
	definePrimitive(base, opRing2, "_Ring2", prSpecialBinaryArithMsg, 3, 0);	// a * b + a + b
	definePrimitive(base, opRing3, "_Ring3", prSpecialBinaryArithMsg, 3, 0);	// a*a*b
	definePrimitive(base, opRing4, "_Ring4", prSpecialBinaryArithMsg, 3, 0);	// a*a*b - a*b*b
	definePrimitive(base, opDifSqr, "_DifSqr", prSpecialBinaryArithMsg, 3, 0);	// a*a - b*b
	definePrimitive(base, opSumSqr, "_SumSqr", prSpecialBinaryArithMsg, 3, 0);	// a*a + b*b
	definePrimitive(base, opSqrSum, "_SqrSum", prSpecialBinaryArithMsg, 3, 0);	// (a + b)^2
	definePrimitive(base, opSqrDif, "_SqrDif", prSpecialBinaryArithMsg, 3, 0);	// (a - b)^2
	definePrimitive(base, opAbsDif, "_AbsDif", prSpecialBinaryArithMsg, 3, 0);	// abs(a - b)
	definePrimitive(base, opThresh, "_Thresh", prSpecialBinaryArithMsg, 3, 0);	// a * max(0,b)
	definePrimitive(base, opAMClip, "_AMClip", prSpecialBinaryArithMsg, 3, 0);	// a * max(0,b)
	definePrimitive(base, opScaleNeg, "_ScaleNeg", prSpecialBinaryArithMsg, 3, 0);	// a < 0 ? a*b : a
	definePrimitive(base, opClip2, "_Clip2", prSpecialBinaryArithMsg, 3, 0);
	definePrimitive(base, opFold2, "_Fold2", prSpecialBinaryArithMsg, 3, 0);
	definePrimitive(base, opWrap2, "_Wrap2", prSpecialBinaryArithMsg, 3, 0);
	definePrimitive(base, opExcess, "_Excess", prSpecialBinaryArithMsg, 3, 0);
	definePrimitive(base, opFirstArg, "_FirstArg", prSpecialBinaryArithMsg, 3, 0);
	definePrimitive(base, opRandRange, "_RandRange", prSpecialBinaryArithMsg, 3, 0);
	definePrimitive(base, opExpRandRange, "_ExpRandRange", prSpecialBinaryArithMsg, 3, 0);

	definePrimitiveWithKeys(base, nextPrimitiveIndex(), "_FunctionValue", blockValue, blockValueWithKeys, 1, 1);
	definePrimitiveWithKeys(base, nextPrimitiveIndex(), "_FunctionValueEnvir", blockValueEnvir, blockValueEnvirWithKeys, 1, 1);
	definePrimitiveWithKeys(base, nextPrimitiveIndex(), "_ObjectPerform", objectPerform, objectPerformWithKeys, 2, 1);
	definePrimitiveWithKeys(base, nextPrimitiveIndex(), "_SuperPerform", objectSuperPerform, objectSuperPerformWithKeys, 2, 1);

void initGUIPrimitives();
	initGUIPrimitives();

#ifdef SC_QT
	QtCollider::initPrimitives();
#endif
	// run primitive and symbol definers
	SC_PrimDefiner::instance().run_all();
	SC_SymbolDefiner::instance().run_all();

	// run custom initializers
	SC_InitializerDefiner::instance().run_all();

	post("\tFound %d primitives.\n", nextPrimitiveIndex());
}

void deinitPrimitives()
{
	// run custom deinitializers
	SC_DeinitializerDefiner::instance().run_all();
}
