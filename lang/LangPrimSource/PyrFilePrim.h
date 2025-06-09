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

std::int64_t prFileDelete(VMGlobals* g, std::int64_t numArgsPushed);
std::int64_t prFileDeleteAll(VMGlobals* g, std::int64_t numArgsPushed);
std::int64_t prFileOpen(VMGlobals* g, std::int64_t numArgsPushed);
std::int64_t prFileClose(VMGlobals* g, std::int64_t numArgsPushed);
std::int64_t prFileSeek(VMGlobals* g, std::int64_t numArgsPushed);
std::int64_t prFilePos(VMGlobals* g, std::int64_t numArgsPushed);
std::int64_t prFileLength(VMGlobals* g, std::int64_t numArgsPushed);
std::int64_t prFileWrite(VMGlobals* g, std::int64_t numArgsPushed);
std::int64_t prFileReadLine(VMGlobals* g, std::int64_t numArgsPushed);

std::int64_t prFilePutRGB(VMGlobals* g, std::int64_t numArgsPushed);
std::int64_t prFilePutInt32(VMGlobals* g, std::int64_t numArgsPushed);
std::int64_t prFilePutInt16(VMGlobals* g, std::int64_t numArgsPushed);
std::int64_t prFilePutInt8(VMGlobals* g, std::int64_t numArgsPushed);
std::int64_t prFilePutChar(VMGlobals* g, std::int64_t numArgsPushed);
std::int64_t prFilePutFloat(VMGlobals* g, std::int64_t numArgsPushed);
std::int64_t prFilePutDouble(VMGlobals* g, std::int64_t numArgsPushed);

std::int64_t prFileGetRGB(VMGlobals* g, std::int64_t numArgsPushed);
std::int64_t prFileGetInt32(VMGlobals* g, std::int64_t numArgsPushed);
std::int64_t prFileGetInt16(VMGlobals* g, std::int64_t numArgsPushed);
std::int64_t prFileGetInt8(VMGlobals* g, std::int64_t numArgsPushed);
std::int64_t prFileGetChar(VMGlobals* g, std::int64_t numArgsPushed);
std::int64_t prFileGetFloat(VMGlobals* g, std::int64_t numArgsPushed);
std::int64_t prFileGetDouble(VMGlobals* g, std::int64_t numArgsPushed);

std::int64_t prFilePutString(VMGlobals* g, std::int64_t numArgsPushed);
std::int64_t prFileRead(VMGlobals* g, std::int64_t numArgsPushed);
