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

#include "PyrObject.h"

void initSymbols();
void initClasses();
void buildClassTree();

void freePyrSlot(PyrSlot* slot);
void freePyrObject(PyrObject* obj);

bool objAddIndexedSlot(PyrObject* obj, PyrSlot* slot);
bool objAddIndexedSymbol(PyrSymbolArray* obj, PyrSymbol* symbol);
bool objAddIndexedObject(PyrObject* obj, PyrObject* obj2);

void CallStackSanity(struct VMGlobals* g, const char* tagstr);
bool FrameSanity(struct PyrFrame* frame, const char* tagstr);

void dumpBadObject(PyrObject* obj);
void initRawRegistry();
