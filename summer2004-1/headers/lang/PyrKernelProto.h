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


#ifndef _PYRKERNELPROTO_H_
#define _PYRKERNELPROTO_H_

PyrClass* newClassObj(PyrClass *classObjSuperClass, 
	PyrSymbol* className, PyrSymbol* superClassName, 
	int numInstVars, int numClassVars, int numInstMethods, 
	int instFormat, int instFlags);
	
void reallocClassObj(PyrClass* classobj, 
	int numInstVars, int numClassVars, int numMethods,
	int instFormat, int instFlags);

int numInstVars(PyrClass* classobj);
int numClassVars(PyrClass* classobj);
int numSuperInstVars(PyrClass *superclassobj);
bool classFindInstVar(PyrClass* classobj, PyrSymbol *name, int *index);
bool classFindClassVar(PyrClass** classobj, PyrSymbol *name, int *index);

void buildClassTree();
void indexClassTree(PyrClass *classobj, int numSuperMethods);
void postClassTree(PyrClass *classobj, int level);
void setSelectorFlags();
void buildBigMethodMatrix();
void fillClassRow(PyrClass *classobj, struct PyrMethod** bigTable);

bool funcFindArg(PyrBlock* func, PyrSymbol *name, int *index);
bool funcFindVar(PyrBlock* func, PyrSymbol *name, int *index);
void addMethod(PyrClass *classobj, PyrMethod *method);


PyrMethod* classFindDirectMethod(PyrClass* classobj, PyrSymbol *name);

PyrBlock* newPyrBlock(int flags);
PyrMethod* newPyrMethod();
PyrClass* makeIntrinsicClass(PyrSymbol *className, PyrSymbol *superClassName,
	int numInstVars, int numClassVars);
void addIntrinsicVar(PyrClass *classobj, char *varName, PyrSlot *slot);



#endif