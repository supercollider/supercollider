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

#include "PyrKernel.h"

static constexpr size_t temporaryKeywordStackCapacity = 128;
extern PyrSlot temporaryKeywordStack[temporaryKeywordStackCapacity];
extern bool gKeywordError;
extern PyrMethod** gRowTable;

void initUniqueMethods();

void prepareArgsForExecute(VMGlobals* g, PyrBlock* block, PyrFrame* callFrame, long totalSuppliedArgs,
                           long numKwArgsSupplied, bool isMethod, PyrObject* optionalEnvirLookup = nullptr);

void executeMethod(VMGlobals* g, PyrBlock* meth, long totalNumArgsPushed, long numKwArgsPushed);

void sendMessage(VMGlobals* g, PyrSymbol* selector, long numArgsPushed, long numKeyArgsPushed);
void sendSuperMessage(VMGlobals* g, PyrSymbol* selector, long numArgsPushed, long numKeyArgsPushed);

void doesNotUnderstand(VMGlobals* g, PyrSymbol* selector, long numArgsPushed, long numKeyArgsPushed);

void returnFromBlock(VMGlobals* g);
void returnFromMethod(VMGlobals* g);

int keywordFixStack(VMGlobals* g, PyrMethod* meth, PyrMethodRaw* methraw, long allArgsPushed, long numKeyArgsPushed);
