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


#pragma once

PyrClass* newClassObj(PyrClass* classObjSuperClass, PyrSymbol* className, PyrSymbol* superClassName, int numInstVars,
                      int numClassVars, int numConsts, int numInstMethods, int instFormat, int instFlags);

void reallocClassObj(PyrClass* classobj, int numInstVars, int numClassVars, int numConsts, int numMethods,
                     int instFormat, int instFlags);

int numInstVars(PyrClass* classobj);
int numClassVars(PyrClass* classobj);
int numSuperInstVars(PyrClass* superclassobj);
bool classFindInstVar(PyrClass* classobj, PyrSymbol* name, int* index);
bool classFindClassVar(PyrClass** classobj, PyrSymbol* name, int* index);
bool classFindConst(PyrClass** classobj, PyrSymbol* name, int* index);

void buildClassTree();
void indexClassTree(PyrClass* classobj, int numSuperMethods);
void postClassTree(PyrClass* classobj, int level);
void setSelectorFlags();
void buildBigMethodMatrix();

bool funcFindArg(PyrBlock* func, PyrSymbol* name, int* index);
bool funcFindVar(PyrBlock* func, PyrSymbol* name, int* index);
void addMethod(PyrClass* classobj, PyrMethod* method);


PyrMethod* classFindDirectMethod(PyrClass* classobj, PyrSymbol* name);

PyrBlock* newPyrBlock(int flags);
PyrMethod* newPyrMethod();
PyrClass* makeIntrinsicClass(PyrSymbol* className, PyrSymbol* superClassName, int numInstVars, int numClassVars);
void addIntrinsicVar(PyrClass* classobj, const char* varName, PyrSlot* slot);
