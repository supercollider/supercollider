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


#include "clz.h"
#include "SC_Graph.h"
#include "SC_GraphDef.h"
#include "SC_Wire.h"
#include "SC_WireSpec.h"
#include "SC_UnitSpec.h"
#include "SC_UnitDef.h"
#include "SC_HiddenWorld.h"
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <stdexcept>
#include "dlfcn.h"
#include "ReadWriteMacros.h"
#include "SC_Prototypes.h"
#include "SC_CoreAudio.h"

extern Malloc gMalloc;

int32 GetHash(ParamSpec* inParamSpec)
{
	return inParamSpec->mHash;
}

int32* GetKey(ParamSpec* inParamSpec)
{
	return inParamSpec->mName;
}


void ReadName(char*& buffer, int32* name);
void ReadName(char*& buffer, int32* name)
{
	uint32 namelen = readInt8(buffer);
	if (namelen >= kSCNameByteLen) {
		throw std::runtime_error("ParamSpec name too long > 31 chars");
		namelen = 31;
	}
	memset(name, 0, kSCNameByteLen);
	readData(buffer, (char*)name, namelen);	
}


void ParamSpec_Read(ParamSpec* inParamSpec, char*& buffer);
void ParamSpec_Read(ParamSpec* inParamSpec, char*& buffer)
{
	ReadName(buffer, inParamSpec->mName);
	inParamSpec->mIndex = readInt16_be(buffer);
	inParamSpec->mHash = Hash(inParamSpec->mName);
}

void InputSpec_Read(InputSpec* inInputSpec, char*& buffer);
void InputSpec_Read(InputSpec* inInputSpec, char*& buffer)
{
	inInputSpec->mFromUnitIndex = (int16)readInt16_be(buffer);
	inInputSpec->mFromOutputIndex = (int16)readInt16_be(buffer);
	
	inInputSpec->mWireIndex = -1;
}

void OutputSpec_Read(OutputSpec* inOutputSpec, char*& buffer);
void OutputSpec_Read(OutputSpec* inOutputSpec, char*& buffer)
{
	inOutputSpec->mCalcRate = readInt8(buffer);

	inOutputSpec->mWireIndex = -1;
	inOutputSpec->mBufferIndex = -1;
	inOutputSpec->mNumConsumers = 0;
}

void UnitSpec_Read(UnitSpec* inUnitSpec, char*& buffer);
void UnitSpec_Read(UnitSpec* inUnitSpec, char*& buffer)
{
	int32 name[kSCNameLen];
	ReadName(buffer, name);
	
	inUnitSpec->mUnitDef = GetUnitDef(name);
	if (!inUnitSpec->mUnitDef) {
		char str[256];
		sprintf(str, "UGen '%s' not installed.", (char*)name);
		throw std::runtime_error(str);
		return;
	}
	inUnitSpec->mCalcRate = readInt8(buffer);
	
	inUnitSpec->mNumInputs = readInt16_be(buffer);
	inUnitSpec->mNumOutputs = readInt16_be(buffer);
	inUnitSpec->mSpecialIndex = readInt16_be(buffer);
	inUnitSpec->mInputSpec = (InputSpec*)malloc(sizeof(InputSpec) * inUnitSpec->mNumInputs);
	inUnitSpec->mOutputSpec = (OutputSpec*)malloc(sizeof(OutputSpec) * inUnitSpec->mNumOutputs);
	for (int i=0; i<inUnitSpec->mNumInputs; ++i) {
		InputSpec_Read(inUnitSpec->mInputSpec + i, buffer);
	}
	for (int i=0; i<inUnitSpec->mNumOutputs; ++i) {
		OutputSpec_Read(inUnitSpec->mOutputSpec + i, buffer);
	}
	int numPorts = inUnitSpec->mNumInputs + inUnitSpec->mNumOutputs;
	inUnitSpec->mAllocSize = inUnitSpec->mUnitDef->mAllocSize + numPorts * (sizeof(Wire*) +  sizeof(float*));
}

GraphDef* GraphDef_Read(World *inWorld, char*& buffer, GraphDef* inList);

GraphDef* GraphDefLib_Read(World *inWorld, char* buffer, GraphDef* inList);
GraphDef* GraphDefLib_Read(World *inWorld, char* buffer, GraphDef* inList)
{
	int32 magic = readInt32_be(buffer);
	if (magic != 'SCgf') return inList;
	
	/*int32 version = */ readInt32_be(buffer);
		
	int32 numDefs = readInt16_be(buffer);
		
	for (int i=0; i<numDefs; ++i) {
		inList = GraphDef_Read(inWorld, buffer, inList);
	}
	return inList;
}


void ChooseMulAddFunc(GraphDef *graphDef, UnitSpec* unitSpec);
void DoBufferColoring(World *inWorld, GraphDef *inGraphDef);

GraphDef* GraphDef_Read(World *inWorld, char*& buffer, GraphDef* inList)
{
	int32 name[kSCNameLen];
	ReadName(buffer, name);

	GraphDef* graphDef = (GraphDef*)malloc(sizeof(GraphDef));
		
	graphDef->mNodeDef.mAllocSize = sizeof(Graph);
	
	memcpy((char*)graphDef->mNodeDef.mName, (char*)name, kSCNameByteLen);

	graphDef->mNodeDef.mHash = Hash(graphDef->mNodeDef.mName);
	
	graphDef->mNumConstants = readInt16_be(buffer);
	graphDef->mConstants = (float*)malloc(graphDef->mNumConstants * sizeof(float));
	for (int i=0; i<graphDef->mNumConstants; ++i) {
		graphDef->mConstants[i] = readFloat_be(buffer);
	}

	graphDef->mNumControls = readInt16_be(buffer);
	graphDef->mInitialControlValues = (float32*)malloc(sizeof(float32) * graphDef->mNumControls);
	for (int i=0; i<graphDef->mNumControls; ++i) {
		graphDef->mInitialControlValues[i] = readFloat_be(buffer);
	}
	
	graphDef->mNumParamSpecs = readInt16_be(buffer);
	if (graphDef->mNumParamSpecs) {
		int hashTableSize = NEXTPOWEROFTWO(graphDef->mNumParamSpecs);
		graphDef->mParamSpecTable = new ParamSpecTable(&gMalloc, hashTableSize, false);
		graphDef->mParamSpecs = (ParamSpec*)malloc(graphDef->mNumParamSpecs * sizeof(ParamSpec));
		for (int i=0; i<graphDef->mNumParamSpecs; ++i) {
			ParamSpec *paramSpec = graphDef->mParamSpecs + i;
			ParamSpec_Read(paramSpec, buffer);
			graphDef->mParamSpecTable->Add(paramSpec);
		}
	} else {
		// empty table to eliminate test in Graph_SetControl
		graphDef->mParamSpecTable = new ParamSpecTable(&gMalloc, 4, false);
		graphDef->mParamSpecs = 0;
	}

	graphDef->mNumWires = graphDef->mNumConstants;
	graphDef->mNumUnitSpecs = readInt16_be(buffer);
	graphDef->mUnitSpecs = (UnitSpec*)malloc(sizeof(UnitSpec) * graphDef->mNumUnitSpecs);
	graphDef->mNumCalcUnits = 0;
	for (int i=0; i<graphDef->mNumUnitSpecs; ++i) {
		UnitSpec *unitSpec = graphDef->mUnitSpecs + i;
		UnitSpec_Read(unitSpec, buffer);
		
		if (unitSpec->mCalcRate != calc_ScalarRate) graphDef->mNumCalcUnits++;
		
		if (unitSpec->mCalcRate == calc_FullRate) unitSpec->mRateInfo = &inWorld->mFullRate;
		else unitSpec->mRateInfo = &inWorld->mBufRate;
		
		graphDef->mNodeDef.mAllocSize += unitSpec->mAllocSize;
		graphDef->mNumWires += unitSpec->mNumOutputs;
	}
	
	DoBufferColoring(inWorld, graphDef);
		
	graphDef->mWiresAllocSize = graphDef->mNumWires * sizeof(Wire);
	graphDef->mUnitsAllocSize = graphDef->mNumUnitSpecs * sizeof(Unit*);
	graphDef->mCalcUnitsAllocSize = graphDef->mNumCalcUnits * sizeof(Unit*);

	graphDef->mNodeDef.mAllocSize += graphDef->mWiresAllocSize;
	graphDef->mNodeDef.mAllocSize += graphDef->mUnitsAllocSize;
	graphDef->mNodeDef.mAllocSize += graphDef->mCalcUnitsAllocSize;

	graphDef->mControlAllocSize = graphDef->mNumControls * sizeof(float);
	graphDef->mNodeDef.mAllocSize += graphDef->mControlAllocSize;
	
	graphDef->mMapControlsAllocSize = graphDef->mNumControls * sizeof(float*);
	graphDef->mNodeDef.mAllocSize += graphDef->mMapControlsAllocSize;
	
	graphDef->mNext = inList;
	graphDef->mRefCount = 1;
	
	return graphDef;
}


void GraphDef_Define(World *inWorld, GraphDef *inList)
{
	GraphDef *graphDef = inList;
	while (graphDef) {
		GraphDef *next = graphDef->mNext;
		
		GraphDef* previousDef = World_GetGraphDef(inWorld, graphDef->mNodeDef.mName);
		if (previousDef) {
			if (!World_RemoveGraphDef(inWorld, previousDef)) {
				scprintf("World_RemoveGraphDef failed? shouldn't happen.\n");
			}
			if (--previousDef->mRefCount == 0) {
				GraphDef_DeleteMsg(inWorld, previousDef);
			}
		}
		World_AddGraphDef(inWorld, graphDef);
		graphDef->mNext = 0;
		graphDef = next;
	}
}

void GraphDef_DeleteMsg(World *inWorld, GraphDef *inDef)
{
	DeleteGraphDefMsg msg;
	msg.mDef = inDef;
	inWorld->hw->mDeleteGraphDefs.Write(msg);
}

GraphDef* GraphDef_Recv(World *inWorld, char *buffer, GraphDef *inList)
{		
	
	try {
		inList = GraphDefLib_Read(inWorld, buffer, inList);
	} catch (std::exception& exc) {
		scprintf("exception in GrafDef_Load: %s\n", exc.what());
	} catch (...) {
		scprintf("unknown exception in GrafDef_Load\n");
	}
		
	return inList;
}

GraphDef* GraphDef_Load(World *inWorld, const char *filename, GraphDef *inList)
{	
	FILE *file = fopen(filename, "r");
	if (!file) {
		scprintf("*** ERROR: can't fopen '%s'\n", filename);
		return 0;
	}

	fseek(file, 0, SEEK_END);
	int size = ftell(file);
	char *buffer = (char*)malloc(size);
	if (!buffer) {
		scprintf("*** ERROR: can't malloc buffer size %d\n", size);
		return 0;
	}
	fseek(file, 0, SEEK_SET);
	fread(buffer, 1, size, file);
	fclose(file);
	
	
	try {
		inList = GraphDefLib_Read(inWorld, buffer, inList);
	} catch (std::exception& exc) {
		scprintf("exception in GrafDef_Load: %s\n", exc.what());
	} catch (...) {
		scprintf("unknown exception in GrafDef_Load\n");
	}
	
	free(buffer);
	
	return inList;
}


GraphDef* GraphDef_LoadDir(World *inWorld, char *dirname, GraphDef *inList)
{
	DIR *dir = opendir(dirname);	
	if (!dir) {
		scprintf("*** ERROR: open directory failed '%s'\n", dirname); 
		return 0;
	}
	
	for (;;) {
		struct dirent *de;
		
		de = readdir(dir);
		if (!de) break;
		
        if (de->d_name[0] == '.') continue;
		char *entrypathname = (char*)malloc(strlen(dirname) + strlen((char*)de->d_name) + 2);
		strcpy(entrypathname, dirname);
		strcat(entrypathname, "/");
		strcat(entrypathname, (char*)de->d_name);
		if (de->d_type == DT_DIR) {
			inList = GraphDef_LoadDir(inWorld, entrypathname, inList);
		} else {
			int dnamelen = strlen(de->d_name);
			if (strncmp(de->d_name+dnamelen-9, ".scsyndef", 9)==0) {
				inList = GraphDef_Load(inWorld, entrypathname, inList);
			}	
		}
		free(entrypathname);
	}
	
	closedir(dir);
	return inList;
}

void UnitSpec_Free(UnitSpec *inUnitSpec);
void UnitSpec_Free(UnitSpec *inUnitSpec)
{
	free(inUnitSpec->mInputSpec);
	free(inUnitSpec->mOutputSpec);
}

void GraphDef_Free(GraphDef *inGraphDef)
{
	for (int i=0; i<inGraphDef->mNumUnitSpecs; ++i) {
		UnitSpec_Free(inGraphDef->mUnitSpecs + i);
	}
	delete inGraphDef->mParamSpecTable;
	free(inGraphDef->mParamSpecs);
	free(inGraphDef->mInitialControlValues);
	free(inGraphDef->mConstants);
	free(inGraphDef->mUnitSpecs);
	free(inGraphDef);
}

void NodeDef_Dump(NodeDef *inNodeDef)
{
	scprintf("mName '%s'\n", (char*)inNodeDef->mName);
	scprintf("mHash %d\n", inNodeDef->mHash);
	scprintf("mAllocSize %lu\n", inNodeDef->mAllocSize);
}

void GraphDef_Dump(GraphDef *inGraphDef)
{
	NodeDef_Dump(&inGraphDef->mNodeDef);
	
	scprintf("mNumControls %d\n", inGraphDef->mNumControls);
	scprintf("mNumWires %d\n", inGraphDef->mNumWires);
	scprintf("mNumUnitSpecs %d\n", inGraphDef->mNumUnitSpecs);
	scprintf("mNumWireBufs %d\n", inGraphDef->mNumWireBufs);

	for (int i=0; i<inGraphDef->mNumControls; ++i) {
		scprintf("   %d mInitialControlValues %g\n", i, inGraphDef->mInitialControlValues[i]);
	}

	for (int i=0; i<inGraphDef->mNumWires; ++i) {
		//WireSpec_Dump(inGraphDef->mWireSpec + i);
	}
	for (int i=0; i<inGraphDef->mNumUnitSpecs; ++i) {
		//UnitSpec_Dump(inGraphDef->mUnitSpecs + i);
	}
}

/*
SynthBufferAllocator
{
	var nextBufIndex = 0;
	var stack;
	var refs;
	
	*new {
		^super.new.init
	}
	init {
		refs = Bag.new;
	}
	alloc { arg count;
		var bufNumber;
		if (stack.size > 0, { 
			bufNumber = stack.pop 
		},{
			bufNumber = nextBufIndex;
			nextBufIndex = nextBufIndex + 1;
		});
		refs.add(bufNumber, count);
		^bufNumber
	}
	release { arg bufNumber;
		refs.remove(bufNumber);
		if (refs.includes(bufNumber).not, { stack = stack.add(bufNumber) });
	}
	numBufs { ^nextBufIndex }
}
*/

struct BufColorAllocator
{
	int16 *refs;
	int16 *stack;
	int16 stackPtr;
	int16 nextIndex;
	
	BufColorAllocator(int maxsize);
	~BufColorAllocator();
	
	int alloc(int count);
	void release(int inIndex);
	int NumBufs() { return nextIndex; }
};

inline BufColorAllocator::BufColorAllocator(int maxsize)
{
	refs = (int16*)zalloc(maxsize, sizeof(int16));	
	stack = (int16*)zalloc(maxsize, sizeof(int16));	
	stackPtr = 0;
	nextIndex = 0;
}

inline BufColorAllocator::~BufColorAllocator()
{
	free(refs);
	free(stack);
}

inline int BufColorAllocator::alloc(int count)
{
	int outIndex;
	if (stackPtr) {
		outIndex = stack[--stackPtr];
	} else {
		outIndex = nextIndex++;
	}
	refs[outIndex] = count;
	return outIndex;
}

inline void BufColorAllocator::release(int inIndex)
{
	if (refs[inIndex] == 0) exit(1);
	if (--refs[inIndex] == 0) stack[stackPtr++] = inIndex;
}

void DoBufferColoring(World *inWorld, GraphDef *inGraphDef)
{
	// count consumers of outputs
	for (int j=0; j<inGraphDef->mNumUnitSpecs; ++j) {
		UnitSpec *unitSpec = inGraphDef->mUnitSpecs + j;
		for (int i=0; i<unitSpec->mNumInputs; ++i) {
			InputSpec *inputSpec = unitSpec->mInputSpec + i;
			if (inputSpec->mFromUnitIndex >= 0) {
				UnitSpec *outUnit = inGraphDef->mUnitSpecs + inputSpec->mFromUnitIndex;
				OutputSpec *outputSpec = outUnit->mOutputSpec + inputSpec->mFromOutputIndex;
				outputSpec->mNumConsumers ++;
			}
		}
	}

	// buffer coloring
	{
		BufColorAllocator bufColor(inGraphDef->mNumUnitSpecs);
		int wireIndexCtr = inGraphDef->mNumConstants;
		for (int j=0; j<inGraphDef->mNumUnitSpecs; ++j) {
			UnitSpec *unitSpec = inGraphDef->mUnitSpecs + j;
			
			// set wire index, release inputs
			for (int i=unitSpec->mNumInputs-1; i>=0; --i) {
				InputSpec *inputSpec = unitSpec->mInputSpec + i;
				if (inputSpec->mFromUnitIndex >= 0) {
					UnitSpec *outUnit = inGraphDef->mUnitSpecs + inputSpec->mFromUnitIndex;
					OutputSpec *outputSpec = outUnit->mOutputSpec + inputSpec->mFromOutputIndex;
					inputSpec->mWireIndex = outputSpec->mWireIndex;
						//	i, outputSpec->mBufferIndex, outputSpec->mCalcRate);
					if (outputSpec->mCalcRate == calc_FullRate) {
						bufColor.release(outputSpec->mBufferIndex);
					}
				} else {
					inputSpec->mWireIndex = inputSpec->mFromOutputIndex;
				}
			}
			
			// set wire index, alloc outputs
			for (int i=0; i<unitSpec->mNumOutputs; ++i) {
				OutputSpec *outputSpec = unitSpec->mOutputSpec + i;
				outputSpec->mWireIndex = wireIndexCtr++;
				if (outputSpec->mCalcRate == calc_FullRate) {
					int bufIndex = bufColor.alloc(outputSpec->mNumConsumers);
					outputSpec->mBufferIndex = bufIndex;
				}
			}
		}
		inGraphDef->mNumWireBufs = bufColor.NumBufs();
		//scprintf("num wire bufs = %d\n", inGraphDef->mNumWireBufs);
		if (inWorld->mRunning)
		{
			// cannot reallocate interconnect buffers while running audio.
			if (inGraphDef->mNumWireBufs > inWorld->hw->mMaxWireBufs) {
				throw std::runtime_error("exceeded number of interconnect buffers.");
			}
		} else {
			inWorld->hw->mMaxWireBufs = sc_max(inWorld->hw->mMaxWireBufs, inGraphDef->mNumWireBufs);
		}
	}
	
	// multiply buf indices by buf length for proper offset
	int bufLength = inWorld->mBufLength;
	for (int j=0; j<inGraphDef->mNumUnitSpecs; ++j) {
		UnitSpec *unitSpec = inGraphDef->mUnitSpecs + j;
		for (int i=0; i<unitSpec->mNumOutputs; ++i) {
			OutputSpec *outputSpec = unitSpec->mOutputSpec + i;
			if (outputSpec->mCalcRate == calc_FullRate) {
				outputSpec->mBufferIndex *= bufLength;
			}
		}
	}	
}
