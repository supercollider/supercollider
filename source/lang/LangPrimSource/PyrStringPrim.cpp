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

Primitives for String.

*/

#include "PyrPrimitive.h"
#include "PyrKernel.h"
#include "GC.h"
#include "Hash.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

int prStringAsSymbol(struct VMGlobals *g, int numArgsPushed);
int prStringAsSymbol(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a;
	char str[256];
	int len;

	a = g->sp;
	len = sc_min(255, a->uo->size);
	memcpy(str, a->uos->s, len);
	str[len] = 0;
	
	a->us = getsym(str);
	a->utag = tagSym;
	
	return errNone;
}

int prString_AsInteger(struct VMGlobals *g, int numArgsPushed);
int prString_AsInteger(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a = g->sp;

	char str[256];
	int err = slotStrVal(a, str, 255);
	if (err) return err;
	
	SetInt(a, atoi(str));
		
	return errNone;
}

int prString_AsFloat(struct VMGlobals *g, int numArgsPushed);
int prString_AsFloat(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a = g->sp;

	char str[256];
	int err = slotStrVal(a, str, 255);
	if (err) return err;
	
	SetFloat(a, atof(str));
		
	return errNone;
}

int memcmpi(char *a, char *b, int len)
{
	for (int i=0; i<len; ++i) {
		char aa = toupper(a[i]);
		char bb = toupper(b[i]);
		if (aa < bb) return -1;
		if (aa > bb) return 1;
	}
	return 0;	
}

int prStringCompare(struct VMGlobals *g, int numArgsPushed);
int prStringCompare(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a, *b, *c;
	int cmp, length;
	
	a = g->sp - 2;
	b = g->sp - 1;
	c = g->sp;
	
	if (b->utag != tagObj || !isKindOf(b->uo, class_string)) {
		SetNil(a);
		return errNone;
	}
	length = sc_min(a->uo->size, b->uo->size);
	if (IsTrue(c)) cmp = memcmpi(a->uos->s, b->uos->s, length);
	else cmp = memcmp(a->uos->s, b->uos->s, length);
	if (cmp == 0) {
		if (a->uo->size < b->uo->size) cmp = -1;
		else if (a->uo->size > b->uo->size) cmp = 1;
	}
	SetInt(a, cmp);
	return errNone;
}

int prStringHash(struct VMGlobals *g, int numArgsPushed);
int prStringHash(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a = g->sp;
	int hash = Hash(a->uos->s, a->uos->size);
	SetInt(a, hash);
	return errNone;
}

#ifndef SC_WIN32
#include <glob.h>

int prStringPathMatch(struct VMGlobals *g, int numArgsPushed);
int prStringPathMatch(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a = g->sp;

	char pattern[1024];
	int err = slotStrVal(a, pattern, 1023);
	if (err) return err;
	
	glob_t pglob;

	int gflags = GLOB_MARK | GLOB_TILDE;
#ifdef SC_DARWIN
	gflags |= GLOB_QUOTE;
#endif

	int gerr = glob(pattern, gflags, NULL, &pglob);
	if (gerr) {
		pglob.gl_pathc = 0;
	}
	PyrObject* array = newPyrArray(g->gc, pglob.gl_pathc, 0, true);
	SetObject(a, array);
	if (gerr) return errNone;
	
	for (int i=0; i<pglob.gl_pathc; ++i) {
		PyrObject *string = (PyrObject*)newPyrString(g->gc, pglob.gl_pathv[i], 0, true);
		SetObject(array->slots+i, string);
		g->gc->GCWrite(array, string);
		array->size++;
	}
	
	globfree(&pglob);
	
	return errNone;
}
#else //#ifndef SC_WIN32
int prStringPathMatch(struct VMGlobals *g, int numArgsPushed);
int prStringPathMatch(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a = g->sp;

	char pattern[1024];
	int err = slotStrVal(a, pattern, 1023);
	if (err) return err;
	
  WIN32_FIND_DATA findData;
  HANDLE hFind;
  int nbPaths = 0;

  hFind = ::FindFirstFile(pattern, &findData);
  if (hFind == INVALID_HANDLE_VALUE) {
    nbPaths = 0;
  }

	PyrObject* array = newPyrArray(g->gc, nbPaths , 0, true);
	SetObject(a, array);
	if (hFind == INVALID_HANDLE_VALUE) 
    return errNone;
    
  do {
		PyrObject *string = (PyrObject*)newPyrString(g->gc, findData.cFileName, 0, true);
		SetObject(array->slots+i, string);
		g->gc->GCWrite(array, string);
		array->size++;
  } while( ::FindNextFile(hFind, &findData);
	return errNone;
}
#endif //#ifndef SC_WIN32

int prString_Getenv(struct VMGlobals* g, int numArgsPushed);
int prString_Getenv(struct VMGlobals* g, int /* numArgsPushed */)
{
	PyrSlot* arg = g->sp;
	char key[256];
	char* value;
	int err;

	err = slotStrVal(arg, key, 256);
	if (err) return err;
	
	value = getenv(key);

	if (value) {
		PyrString* pyrString = newPyrString(g->gc, value, 0, true);
		if (!pyrString) return errFailed;
		SetObject(arg, pyrString);
	} else {
		SetNil(arg);
	}

	return errNone;
}

int prString_Setenv(struct VMGlobals* g, int numArgsPushed);
int prString_Setenv(struct VMGlobals* g, int /* numArgsPushed */)
{
	PyrSlot* args = g->sp - 1;
	char key[256];
	int err;

	err = slotStrVal(args+0, key, 256);
	if (err) return err;
	
	if (IsNil(args+1)) {
		unsetenv(key);
	} else {
		char value[1024];
		err = slotStrVal(args+1, value, 1024);
		if (err) return err;
		setenv(key, value, 1);
	}
	
	return errNone;
}

void initStringPrimitives();
void initStringPrimitives()
{
	int base, index = 0;
		
	base = nextPrimitiveIndex();

	definePrimitive(base, index++, "_StringCompare", prStringCompare, 3, 0);	
	definePrimitive(base, index++, "_StringHash", prStringHash, 1, 0);	
	definePrimitive(base, index++, "_StringPathMatch", prStringPathMatch, 1, 0);	
	definePrimitive(base, index++, "_StringAsSymbol", prStringAsSymbol, 1, 0);	
	definePrimitive(base, index++, "_String_AsInteger", prString_AsInteger, 1, 0);	
	definePrimitive(base, index++, "_String_AsFloat", prString_AsFloat, 1, 0);	
	definePrimitive(base, index++, "_String_Getenv", prString_Getenv, 1, 0);
    definePrimitive(base, index++, "_String_Setenv", prString_Setenv, 2, 0);
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
	initStringPrimitives();
}

// This function is called when the plug in is loaded into SC.
// It returns an instance of APlugIn.
SCPlugIn* loadPlugIn()
{
	return new APlugIn();
}

#endif
