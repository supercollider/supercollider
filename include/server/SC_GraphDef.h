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


#ifndef _SC_GraphDef_
#define _SC_GraphDef_

#include "SC_SynthDef.h"
#include "HashTable.h"

struct ParamSpec
{
	int32 mName[kSCNameLen];
	int32 mIndex;
	int32 mHash;
};

typedef HashTable<ParamSpec, Malloc> ParamSpecTable;

struct GraphDef
{
	NodeDef mNodeDef;

	uint32 mNumControls;
	uint32 mNumAudioControls;

	uint32 mNumWires;
	uint32 mNumConstants;
	uint32 mNumUnitSpecs;
	uint32 mNumWireBufs;
	uint32 mNumCalcUnits;

	float32 *mInitialControlValues;
	float32 *mConstants;

	struct UnitSpec *mUnitSpecs;

	size_t mWiresAllocSize, mUnitsAllocSize, mCalcUnitsAllocSize;
	size_t mControlAllocSize, mMapControlsAllocSize, mMapControlRatesAllocSize;

	uint32 mNumParamSpecs;
	ParamSpec *mParamSpecs;
	ParamSpecTable *mParamSpecTable;

	int mRefCount;
	struct GraphDef* mNext;

	struct GraphDef *mOriginal;

	uint32 mNumVariants;
	struct GraphDef* mVariants;
};
typedef struct GraphDef GraphDef;

GraphDef* GraphDef_Recv(World *inWorld, char *buffer, GraphDef *inList);
GraphDef* GraphDef_Load(struct World *inWorld, const char *filename, GraphDef* inList);
GraphDef* GraphDef_LoadDir(struct World *inWorld, char *dirname, GraphDef* inList);
GraphDef* GraphDef_LoadGlob(World *inWorld, const char *pattern, GraphDef *inList);
void GraphDef_Remove(World *inWorld, int32 *inName);
void GraphDef_DeleteMsg(struct World *inWorld, GraphDef *inDef);
void GraphDef_Dump(GraphDef *inGraphDef);
int32 GetHash(ParamSpec* inParamSpec);
int32* GetKey(ParamSpec* inParamSpec);

#endif
