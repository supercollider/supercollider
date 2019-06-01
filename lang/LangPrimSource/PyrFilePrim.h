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

struct PyrFile : public PyrObjectHdr {
    PyrSlot fileptr;
};

void initFilePrimitives();

long prFileDelete(VMGlobals* g, long numArgsPushed);
long prFileOpen(VMGlobals* g, long numArgsPushed);
long prFileClose(VMGlobals* g, long numArgsPushed);
long prFileSeek(VMGlobals* g, long numArgsPushed);
long prFilePos(VMGlobals* g, long numArgsPushed);
long prFileLength(VMGlobals* g, long numArgsPushed);
long prFileWrite(VMGlobals* g, long numArgsPushed);
long prFileReadLine(VMGlobals* g, long numArgsPushed);

long prFilePutRGB(VMGlobals* g, long numArgsPushed);
long prFilePutInt32(VMGlobals* g, long numArgsPushed);
long prFilePutInt16(VMGlobals* g, long numArgsPushed);
long prFilePutInt8(VMGlobals* g, long numArgsPushed);
long prFilePutChar(VMGlobals* g, long numArgsPushed);
long prFilePutFloat(VMGlobals* g, long numArgsPushed);
long prFilePutDouble(VMGlobals* g, long numArgsPushed);

long prFileGetRGB(VMGlobals* g, long numArgsPushed);
long prFileGetInt32(VMGlobals* g, long numArgsPushed);
long prFileGetInt16(VMGlobals* g, long numArgsPushed);
long prFileGetInt8(VMGlobals* g, long numArgsPushed);
long prFileGetChar(VMGlobals* g, long numArgsPushed);
long prFileGetFloat(VMGlobals* g, long numArgsPushed);
long prFileGetDouble(VMGlobals* g, long numArgsPushed);

long prFilePutString(VMGlobals* g, long numArgsPushed);
long prFileRead(VMGlobals* g, long numArgsPushed);
