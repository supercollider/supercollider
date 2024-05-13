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

Functions for defining language primitives.

*/

#pragma once

#include "PyrSlot.h"

typedef int (*PrimitiveHandler)(struct VMGlobals* g, int numArgsPushed);
typedef int (*PrimitiveWithKeysHandler)(struct VMGlobals* g, int numArgsPushed, int numKeyArgsPushed);

int nextPrimitiveIndex();
int definePrimitive(int base, int index, const char* name, PrimitiveHandler handler, int numArgs, int varArgs);
int definePrimitiveWithKeys(int base, int index, const char* name, PrimitiveHandler handler,
                            PrimitiveWithKeysHandler keyhandler, int numArgs, int varArgs);
int getPrimitiveNumArgs(int index);
PyrSymbol* getPrimitiveName(int index);
