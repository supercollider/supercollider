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

Primitives for Symbol.

*/

#include <string.h>
#include <stdlib.h>
#include "PyrPrimitive.h"
#include "PyrSymbol.h"
#include "VMGlobals.h"
#include "PyrKernel.h"
#include "SCBase.h"

/*
int prSymbolString(struct VMGlobals *g, int numArgsPushed);
int prSymbolString(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a;
	PyrString *string;

	a = g->sp;
	if (NotSym(a)) return errWrongType;
	string = newPyrString(g->gc, slotRawSymbol(a)->name, 0, true);
	SetObject(a, string);
	return errNone;
}
*/

int prSymbolIsPrefix(struct VMGlobals *g, int numArgsPushed);
int prSymbolIsPrefix(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a, *b;
	int length;

	a = g->sp - 1;
	b = g->sp;
	if (!IsSym(a) || !IsSym(b)) return errWrongType;
	int32 alen = slotRawSymbol(a)->length;
	int32 blen = slotRawSymbol(b)->length;
	length = sc_min(alen, blen);
	if (memcmp(slotRawSymbol(a)->name, slotRawSymbol(b)->name, length) == 0) {
		SetTrue(a);
	} else {
		SetFalse(a);
	}
	return errNone;
}

int prSymbolClass(struct VMGlobals *g, int numArgsPushed);
int prSymbolClass(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a;
	PyrClass *classobj;
	//char firstChar;

	a = g->sp;
	if (slotRawSymbol(a)->flags & sym_Class) {
	//firstChar = slotRawSymbol(a)->name[0];
	//if (firstChar >= 'A' && firstChar <= 'Z') {
		classobj = slotRawSymbol(a)->u.classobj;
		if (classobj) {
			SetObject(a, classobj);
		} else {
			SetNil(a);
		}
	} else {
		SetNil(a);
	}
	return errNone;
}

int prSymbolIsSetter(struct VMGlobals *g, int numArgsPushed);
int prSymbolIsSetter(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a;

	a = g->sp;
	if (slotRawSymbol(a)->flags & sym_Setter) {
		SetTrue(a);
	} else {
		SetFalse(a);
	}
	return errNone;
}

int prSymbolAsSetter(struct VMGlobals *g, int numArgsPushed);
int prSymbolAsSetter(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a;
	char str[256];
	int len;

	a = g->sp;
	if (!(slotRawSymbol(a)->flags & sym_Setter)) {
		if ((slotRawSymbol(a)->flags & sym_Class) || (slotRawSymbol(a)->flags & sym_Primitive)) {
			error("Cannot convert class names or primitive names to setters.\n");
			return errFailed;
		}
		if (strlen(slotRawSymbol(a)->name)>255) {
			error("symbol name too long.\n");
			return errFailed;
		}
		strcpy(str, slotRawSymbol(a)->name);
		len = strlen(str);
		str[len] = '_';
		str[len+1] = 0;

		//postfl("prSymbolAsSetter %s\n", str);
		SetRaw(a, getsym(str));
	}
	return errNone;
}

int prSymbolAsGetter(struct VMGlobals *g, int numArgsPushed);
int prSymbolAsGetter(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a;
	char str[256];

	a = g->sp;
	if ((slotRawSymbol(a)->flags & sym_Setter)) {
		if ((slotRawSymbol(a)->flags & sym_Class) || (slotRawSymbol(a)->flags & sym_Primitive)) {
			error("Cannot convert class names or primitive names to getters.\n");
			return errFailed;
		}
		strcpy(str, slotRawSymbol(a)->name);
		str[strlen(str)-1] = 0;
		//postfl("prSymbolAsGetter %s\n", str);
		SetRaw(a, getsym(str));
	}
	return errNone;
}

int prSymbolIsClassName(struct VMGlobals *g, int numArgsPushed);
int prSymbolIsClassName(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a;

	a = g->sp;
	if (slotRawSymbol(a)->flags & sym_Class) {
		SetTrue(a);
	} else {
		SetFalse(a);
	}
	return errNone;
}

int prSymbolIsMetaClassName(struct VMGlobals *g, int numArgsPushed);
int prSymbolIsMetaClassName(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a;

	a = g->sp;
	if (slotRawSymbol(a)->flags & sym_MetaClass) {
		SetTrue(a);
	} else {
		SetFalse(a);
	}
	return errNone;
}

int prSymbol_AsInteger(struct VMGlobals *g, int numArgsPushed);
int prSymbol_AsInteger(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a = g->sp;

	char *str = slotRawSymbol(a)->name;
	SetInt(a, atoi(str));

	return errNone;
}

int prSymbol_PrimitiveIndex(struct VMGlobals *g, int numArgsPushed);
int prSymbol_PrimitiveIndex(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a = g->sp;

	SetInt(a, slotRawSymbol(a)->u.index);

	return errNone;
}

int prSymbol_SpecialIndex(struct VMGlobals *g, int numArgsPushed);
int prSymbol_SpecialIndex(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a = g->sp;

	SetInt(a, slotRawSymbol(a)->specialIndex);

	return errNone;
}


int prSymbol_AsFloat(struct VMGlobals *g, int numArgsPushed);
int prSymbol_AsFloat(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a = g->sp;

	char *str = slotRawSymbol(a)->name;
	SetFloat(a, atof(str));

	return errNone;
}

int prSymbol_matchOSCPattern(struct VMGlobals *g, int numArgsPushed);
int prSymbol_matchOSCPattern(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a, *b;
	int length;
	
	a = g->sp - 1;
	b = g->sp;
	if (!IsSym(a) || !IsSym(b)) return errWrongType;
//	int32 alen = slotRawSymbol(a)->length;
//	int32 blen = slotRawSymbol(b)->length;
//	length = sc_min(alen, blen);
	if (lo_pattern_match(slotRawSymbol(a)->name, slotRawSymbol(b)->name)) {
		SetTrue(a);
	} else {
		SetFalse(a);
	}
	return errNone;
}

int prSymbol_isMap(struct VMGlobals *g, int numArgsPushed);
int prSymbol_isMap(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a = g->sp;

	char *str = slotRawSymbol(a)->name;
	if(strlen(str)>1 && (str[0]=='a' || str[0]=='c') && str[1]>='0' && str[1]<='9')
		SetTrue(a);
	else
		SetFalse(a);

	return errNone;
}



void initSymbolPrimitives();
void initSymbolPrimitives()
{
	int base, index = 0;

	base = nextPrimitiveIndex();

	definePrimitive(base, index++, "_SymbolIsPrefix", prSymbolIsPrefix, 2, 0);
	//definePrimitive(base, index++, "_SymbolString", prSymbolString, 1, 0);
	definePrimitive(base, index++, "_SymbolClass", prSymbolClass, 1, 0);
	definePrimitive(base, index++, "_SymbolIsClassName", prSymbolIsClassName, 1, 0);
	definePrimitive(base, index++, "_SymbolIsMetaClassName", prSymbolIsMetaClassName, 1, 0);
	definePrimitive(base, index++, "_SymbolIsSetter", prSymbolIsSetter, 1, 0);
	definePrimitive(base, index++, "_SymbolAsSetter", prSymbolAsSetter, 1, 0);
	definePrimitive(base, index++, "_SymbolAsGetter", prSymbolAsGetter, 1, 0);
	definePrimitive(base, index++, "_Symbol_AsInteger", prSymbol_AsInteger, 1, 0);
	definePrimitive(base, index++, "_Symbol_PrimitiveIndex", prSymbol_PrimitiveIndex, 1, 0);
	definePrimitive(base, index++, "_Symbol_SpecialIndex", prSymbol_SpecialIndex, 1, 0);
	definePrimitive(base, index++, "_Symbol_AsFloat", prSymbol_AsFloat, 1, 0);
	definePrimitive(base, index++, "_Symbol_matchOSCPattern", prSymbol_matchOSCPattern, 2, 0);
	definePrimitive(base, index++, "_Symbol_IsMap", prSymbol_isMap, 1, 0);
}


#if _SC_PLUGINS_

#include "SCPlugin.h"

// export the function that SC will call to load the plug in.
#pragma export on
extern "C" { SCPlugIn* loadPlugIn(void); }
#pragma export off


// define plug in object
class APlugIn : public SCPlugIn
{
public:
	APlugIn();
	virtual ~APlugIn();

	virtual void AboutToCompile();
};

APlugIn::APlugIn()
{
	// constructor for plug in
}

APlugIn::~APlugIn()
{
	// destructor for plug in
}

void APlugIn::AboutToCompile()
{
	// this is called each time the class library is compiled.
	initSymbolPrimitives();
}

// This function is called when the plug in is loaded into SC.
// It returns an instance of APlugIn.
SCPlugIn* loadPlugIn()
{
	return new APlugIn();
}

#endif

