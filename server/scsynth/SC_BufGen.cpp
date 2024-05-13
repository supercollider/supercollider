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


#include "SC_BufGen.h"
#include "SC_World.h"
#include "SC_Unit.h"
#include "SC_InterfaceTable.h"
#include <stdio.h>
#include <stdlib.h>
#ifndef _MSC_VER
#    include <dirent.h>
#endif //_MSC_VER
#include <string.h>
#include "SC_Prototypes.h"
#include "SC_Str4.h"

extern InterfaceTable gInterfaceTable;

bool BufGen_Create(const char* inName, BufGenFunc inFunc) {
    BufGen* bufGen = (BufGen*)malloc(sizeof(BufGen));
    str4cpy(bufGen->mBufGenName, inName);
    bufGen->mHash = Hash(bufGen->mBufGenName);

    bufGen->mBufGenFunc = inFunc;

    if (!AddBufGen(bufGen)) {
        free(bufGen);
        return false;
    }
    return true;
}
