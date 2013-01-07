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

#ifndef _UnitDef_
#define _UnitDef_

#include "SC_Types.h"
#include "SC_Unit.h"
#include "HashTable.h"

struct PlugInCmd
{
	int32 mCmdName[kSCNameLen];
	int32 mHash;
	PlugInCmdFunc mFunc;
	void *mUserData;
};

struct UnitCmd
{
	int32 mCmdName[kSCNameLen];
	int32 mHash;
	UnitCmdFunc mFunc;
};

struct UnitDef
{
	int32 mUnitDefName[kSCNameLen];
	int32 mHash;

	size_t mAllocSize;
	UnitCtorFunc mUnitCtorFunc;
	UnitDtorFunc mUnitDtorFunc;

	HashTable<UnitCmd, Malloc>* mCmds;
	uint32 mFlags;
};

extern "C" {
bool UnitDef_Create(const char *inName, size_t inAllocSize,
	UnitCtorFunc inCtor, UnitDtorFunc inDtor, uint32 inFlags);
bool UnitDef_AddCmd(const char *inUnitDefName, const char *inCmdName, UnitCmdFunc inFunc);
bool PlugIn_DefineCmd(const char *inCmdName, PlugInCmdFunc inFunc, void *inUserData);
}

int Unit_DoCmd(World *inWorld, int inSize, char *inData);

inline int32* GetKey(UnitCmd *inCmd) { return inCmd->mCmdName; }
inline int32 GetHash(UnitCmd *inCmd) { return inCmd->mHash; }


#endif
