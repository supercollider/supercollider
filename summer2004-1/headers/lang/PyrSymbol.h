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

A PyrSymbol is a unique string that resides in a global hash table.

*/

#ifndef _PYRSYMBOL_H_
#define _PYRSYMBOL_H_

#include "SCBase.h"

struct PyrSymbol {
	char *name;
	long hash;
	short specialIndex;
	uint8 flags;
	uint8 length;
	union {
		long index; // index in row table or primitive table
		struct PyrClass *classobj;	// pointer to class with this name.	
	} u;
	struct classdep *classdep;	
};

enum { 
	sym_Selector = 1, 
	sym_Class = 2, 
	sym_Compiled = 4, 
	sym_Called = 8,
	sym_Primitive = 16,
	sym_Setter = 32,
	sym_MetaClass = 64,
	sym_Filename = 128
};

PyrSymbol* getsym(const char *name);
PyrSymbol* getmetasym(const char *name);
PyrSymbol* findsym(const char *name);

#endif