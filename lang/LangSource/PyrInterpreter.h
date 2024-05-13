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

#include "PyrSlot.h"
#include "VMGlobals.h"
#include "SC_Export.h"

extern bool gRunningInterpreterThread;

extern int gNumClasses;
extern int gNumClassVars;

bool initInterpreter(VMGlobals* g, PyrSymbol* selector, int numArgsPushed);
bool initRuntime(VMGlobals* g, int poolSize, AllocPool* inPool);
void Interpret(VMGlobals* g);

int doSpecialUnaryArithMsg(VMGlobals* g, int numArgsPushed);
int prSpecialBinaryArithMsg(VMGlobals* g, int numArgsPushed);
int doSpecialBinaryArithMsg(VMGlobals* g, int numArgsPushed, bool isPrimitive);
void DumpBackTrace(VMGlobals* g);
void DumpStack(VMGlobals* g, PyrSlot* sp);
void DumpFrame(struct PyrFrame* frame);
bool FrameSanity(PyrFrame* frame, const char* tagstr);
struct PyrProcess* newPyrProcess(VMGlobals* g, struct PyrClass* classobj);
void startProcess(VMGlobals* g, PyrSymbol* selector);
SCLANG_DLLEXPORT_C void runInterpreter(VMGlobals* g, PyrSymbol* selector, int numArgsPushed);
