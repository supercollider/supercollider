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

#ifndef _PYRPRIMITIVEPROTO_H_
#define _PYRPRIMITIVEPROTO_H_

#include "PyrPrimitive.h"

int basicNew(VMGlobals *g, int numArgsPushed);
int basicNewClear(VMGlobals *g, int numArgsPushed);
int basicSwap(VMGlobals *g, int numArgsPushed);
int instVarAt(VMGlobals *g, int numArgsPushed);
int instVarPut(VMGlobals *g, int numArgsPushed);
int instVarSize(VMGlobals *g, int numArgsPushed);
int objectHash(VMGlobals *g, int numArgsPushed);
int objectClass(VMGlobals *g, int numArgsPushed);
int blockValue(VMGlobals *g, int numArgsPushed);
int blockValueWithKeys(VMGlobals *g, int allArgsPushed, int numKeyArgsPushed);
int blockValueArray(VMGlobals *g, int numArgsPushed);
int blockSpawn(VMGlobals *g, int numArgsPushed);

int objectIsKindOf(VMGlobals *g, int numArgsPushed);
int objectIsMemberOf(VMGlobals *g, int numArgsPushed);
int objectDump(VMGlobals *g, int numArgsPushed);
int haltInterpreter(VMGlobals *g, int numArgsPushed);
int objectIdentical(VMGlobals *g, int numArgsPushed);
int objectNotIdentical(VMGlobals *g, int numArgsPushed);
int objectPerform(VMGlobals *g, int numArgsPushed);
int objectPerformList(VMGlobals *g, int numArgsPushed);
int objectPerformSelList(VMGlobals *g, int numArgsPushed);
int undefinedPrimitive(VMGlobals *g, int numArgsPushed);

int prObjectString(VMGlobals *g, int numArgsPushed);
int prClassString(VMGlobals *g, int numArgsPushed);
int prSymbolString(VMGlobals *g, int numArgsPushed);
int prSymbolClass(VMGlobals *g, int numArgsPushed);
int prPostString(VMGlobals *g, int numArgsPushed);
int prPostLine(VMGlobals *g, int numArgsPushed);
int prFlushPostBuf(VMGlobals *g, int numArgsPushed);

int prPrimitiveError(VMGlobals *g, int numArgsPushed);
int prPrimitiveErrorString(VMGlobals *g, int numArgsPushed);
int prDumpStack(VMGlobals *g, int numArgsPushed);
int prDebugger(VMGlobals *g, int numArgsPushed);
int prPrimName(VMGlobals *g, int numArgsPushed);
int prObjectShallowCopy(VMGlobals *g, int numArgsPushed);
int prObjectCopyRange(VMGlobals *g, int numArgsPushed);
int prObjectPointsTo(VMGlobals *g, int numArgsPushed);
int prObjectRespondsTo(VMGlobals *g, int numArgsPushed);

int prCompileString(VMGlobals *g, int numArgsPushed);
int prDumpBackTrace(VMGlobals *g, int numArgsPushed);
int prDumpByteCodes(VMGlobals *g, int numArgsPushed);
int prAllClasses(VMGlobals *g, int numArgsPushed);
int prPostClassTree(VMGlobals *g, int numArgsPushed);

void initPrimitiveTable();
void growPrimitiveTable(int newsize);

void initPrimitives();
void doPrimitive(VMGlobals* g, struct PyrMethod* meth, int numArgsPushed);
void doPrimitiveWithKeys(VMGlobals* g, struct PyrMethod* meth, int allArgsPushed, int numKeysPushed);

#endif
