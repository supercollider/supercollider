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


void ReadName(FILE *file, int32* name);
void ReadName(FILE *file, int32* name)
{
	uint32 namelen = readInt8(file);
	if (namelen >= kSCNameByteLen) {
		throw std::runtime_error("ParamSpec name too long > 31 chars");
		namelen = 31;
	}
	memset(name, 0, kSCNameByteLen);
	readData(file, (char*)name, namelen);	
}


void ParamSpec_Read(ParamSpec* inParamSpec, FILE *file);
void ParamSpec_Read(ParamSpec* inParamSpec, FILE *file)
{
	ReadName(file, inParamSpec->mName);
	//printf("param name '%s'\n", (char*)inParamSpec->mName);
	inParamSpec->mIndex = readInt16_be(file);
	//printf("param index %d\n", inParamSpec->mIndex);
	inParamSpec->mHash = Hash(inParamSpec->mName);
	//printf("param hash %d\n", inParamSpec->mHash);
}

void InputSpec_Read(InputSpec* inInputSpec, FILE *file);
void InputSpec_Read(InputSpec* inInputSpec, FILE *file)
{
	inInputSpec->mFromUnitIndex = (int16)readInt16_be(file);
	inInputSpec->mFromOutputIndex = (int16)readInt16_be(file);
	//printf("   inp %d %d\n", inInputSpec->mFromUnitIndex, inInputSpec->mFromOutputIndex);
	
	inInputSpec->mWireIndex = -1;
}

void OutputSpec_Read(OutputSpec* inOutputSpec, FILE *file);
void OutputSpec_Read(OutputSpec* inOutputSpec, FILE *file)
{
	inOutputSpec->mCalcRate = readInt8(file);
	//printf("   outp %d\n", inOutputSpec->mCalcRate);

	inOutputSpec->mWireIndex = -1;
	inOutputSpec->mBufferIndex = -1;
	inOutputSpec->mNumConsumers = 0;
}

void UnitSpec_Read(UnitSpec* inUnitSpec, FILE *file);
void UnitSpec_Read(UnitSpec* inUnitSpec, FILE *file)
{
	int32 name[kSCNameLen];
	ReadName(file, name);
	
	inUnitSpec->mUnitDef = GetUnitDef(name);
	if (!inUnitSpec->mUnitDef) {
		char str[256];
		sprintf(str, "UGen '%s' not installed.", (char*)name);
		throw std::runtime_error(str);
		return;
	}
	inUnitSpec->mCalcRate = readInt8(file);
	//printf("unit %s %d\n", name, inUnitSpec->mCalcRate);
	
	inUnitSpec->mNumInputs = readInt16_be(file);
	inUnitSpec->mNumOutputs = readInt16_be(file);
	inUnitSpec->mSpecialIndex = readInt16_be(file);
	inUnitSpec->mInputSpec = (InputSpec*)malloc(sizeof(InputSpec) * inUnitSpec->mNumInputs);
	inUnitSpec->mOutputSpec = (OutputSpec*)malloc(sizeof(OutputSpec) * inUnitSpec->mNumOutputs);
	for (int i=0; i<inUnitSpec->mNumInputs; ++i) {
		InputSpec_Read(inUnitSpec->mInputSpec + i, file);
	}
	for (int i=0; i<inUnitSpec->mNumOutputs; ++i) {
		OutputSpec_Read(inUnitSpec->mOutputSpec + i, file);
	}
	int numPorts = inUnitSpec->mNumInputs + inUnitSpec->mNumOutputs;
	inUnitSpec->mAllocSize = inUnitSpec->mUnitDef->mAllocSize + numPorts * (sizeof(Wire*) +  sizeof(float*));
}

GraphDef* GraphDef_Read(World *inWorld, FILE *file, GraphDef* inList);

GraphDef* GraphDefLib_Read(World *inWorld, FILE *file, GraphDef* inList);
GraphDef* GraphDefLib_Read(World *inWorld, FILE *file, GraphDef* inList)
{
	//printf("->GraphDefLib_Read a\n");
	int32 magic = readInt32_be(file);
	if (magic != 'SCgf') return inList;
	//printf("GraphDefLib_Read b\n");
	
	/*int32 version = */ readInt32_be(file);
		
	int32 numDefs = readInt16_be(file);
		
	for (int i=0; i<numDefs; ++i) {
		inList = GraphDef_Read(inWorld, file, inList);
	}
	return inList;
}


void ChooseMulAddFunc(GraphDef *graphDef, UnitSpec* unitSpec);
void DoBufferColoring(World *inWorld, GraphDef *inGraphDef);

GraphDef* GraphDef_Read(World *inWorld, FILE *file, GraphDef* inList)
{
	//printf("->GraphDef_Read\n");

	int32 name[kSCNameLen];
	ReadName(file, name);
	//printf("\n\nname %s\n", name);

	GraphDef* graphDef = (GraphDef*)malloc(sizeof(GraphDef));
		
	graphDef->mNodeDef.fCtor = (NodeCtorFunc)&Graph_Ctor;
	graphDef->mNodeDef.fDtor = (NodeDtorFunc)&Graph_Dtor;
	graphDef->mNodeDef.mAllocSize = sizeof(Graph);
	
	memcpy((char*)graphDef->mNodeDef.mName, (char*)name, kSCNameByteLen);

	graphDef->mNodeDef.mHash = Hash(graphDef->mNodeDef.mName);
	
	graphDef->mNumConstants = readInt16_be(file);
	//printf("numconstants %d\n", graphDef->mNumConstants);
	graphDef->mConstants = (float*)malloc(graphDef->mNumConstants * sizeof(float));
	for (int i=0; i<graphDef->mNumConstants; ++i) {
		graphDef->mConstants[i] = readFloat_be(file);
	}

	graphDef->mNumControls = readInt16_be(file);
	//printf("numcontrols %d\n", graphDef->mNumControls);
	graphDef->mInitialControlValues = (float32*)malloc(sizeof(float32) * graphDef->mNumControls);
	for (int i=0; i<graphDef->mNumControls; ++i) {
		graphDef->mInitialControlValues[i] = readFloat_be(file);
	}
	
	graphDef->mNumParamSpecs = readInt16_be(file);
	//printf("numparams %d\n", graphDef->mNumParamSpecs);
	if (graphDef->mNumParamSpecs) {
		int hashTableSize = NEXTPOWEROFTWO(graphDef->mNumParamSpecs);
		graphDef->mParamSpecTable = new HashTable<ParamSpec, Malloc>(&gMalloc, hashTableSize, false);
		graphDef->mParamSpecs = (ParamSpec*)malloc(graphDef->mNumParamSpecs * sizeof(ParamSpec));
		for (int i=0; i<graphDef->mNumParamSpecs; ++i) {
			ParamSpec *paramSpec = graphDef->mParamSpecs + i;
			ParamSpec_Read(paramSpec, file);
			graphDef->mParamSpecTable->Add(paramSpec);
		}
	} else {
		graphDef->mParamSpecTable = 0;
		graphDef->mParamSpecs = 0;
	}

	graphDef->mNumWires = graphDef->mNumConstants;
	graphDef->mNumUnitSpecs = readInt16_be(file);
	//printf("num units %d\n", graphDef->mNumUnitSpecs);
	graphDef->mUnitSpecs = (UnitSpec*)malloc(sizeof(UnitSpec) * graphDef->mNumUnitSpecs);
	graphDef->mTotalAllocSize = 0;
	graphDef->mNumCalcUnits = 0;
	for (int i=0; i<graphDef->mNumUnitSpecs; ++i) {
		UnitSpec *unitSpec = graphDef->mUnitSpecs + i;
		UnitSpec_Read(unitSpec, file);
		if (unitSpec->mCalcRate != calc_Scalar) graphDef->mNumCalcUnits++;
		graphDef->mTotalAllocSize += unitSpec->mAllocSize;
		graphDef->mNumWires += unitSpec->mNumOutputs;
	}
	//printf("num wires %d\n", graphDef->mNumWires);
	
	DoBufferColoring(inWorld, graphDef);
	//printf("num wire bufs %d  max %d\n", graphDef->mNumWireBufs, inWorld->hw->mMaxWireBufs);
	
	
	//GraphDef *check = World_GetGraphDef(inWorld, graphDef->mNodeDef.mName);
	//printf("check %08X\n", check);
	
	//printf("<-GraphDef_Read %d\n", res);
	
	//GraphDef_Dump(graphDef);
	//printf("<<-GraphDef_Read %d\n", res);
	
	graphDef->mWiresAllocSize = graphDef->mNumWires * sizeof(Wire);
	graphDef->mUnitsAllocSize = graphDef->mNumUnitSpecs * sizeof(Unit*);
	graphDef->mCalcUnitsAllocSize = graphDef->mNumCalcUnits * sizeof(Unit*);

	graphDef->mTotalAllocSize += graphDef->mWiresAllocSize;
	graphDef->mTotalAllocSize += graphDef->mUnitsAllocSize;
	graphDef->mTotalAllocSize += graphDef->mCalcUnitsAllocSize;

	graphDef->mControlAllocSize = graphDef->mNumControls * sizeof(float);
	graphDef->mTotalAllocSize += graphDef->mControlAllocSize;
	
	graphDef->mMapControlsAllocSize = graphDef->mNumControls * sizeof(float*);
	graphDef->mTotalAllocSize += graphDef->mMapControlsAllocSize;
	
	//printf("<-GraphDef_Read %d %d\n", res, graphDef->mTotalAllocSize);
	graphDef->mNext = inList;
	graphDef->mRefCount = 1;
	
	return graphDef;
}


void GraphDef_Define(World *inWorld, GraphDef *inList)
{
	//printf("GraphDef_Define\n");
	GraphDef *graphDef = inList;
	while (graphDef) {
		GraphDef *next = graphDef->mNext;
		
		GraphDef* previousDef = World_GetGraphDef(inWorld, graphDef->mNodeDef.mName);
		if (previousDef) {
			if (!World_RemoveGraphDef(inWorld, previousDef)) {
				printf("World_RemoveGraphDef failed? shouldn't happen.\n");
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
	//printf("GraphDef_DeleteMsg\n");
	DeleteGraphDefMsg msg;
	msg.mDef = inDef;
	inWorld->hw->mDeleteGraphDefs.Write(msg);
}

GraphDef* GraphDef_Load(World *inWorld, const char *filename, GraphDef *inList)
{	
	//printf("->GraphDef_Load %s\n", filename);
	FILE *file = fopen(filename, "r");
	
	if (!file) {
		printf("*** ERROR: fopen '%s' err '%s'\n", filename, dlerror());
		system("pwd");
		return 0;
	}
	
	try {
		inList = GraphDefLib_Read(inWorld, file, inList);
	} catch (std::exception& exc) {
		fprintf(stdout, "exception in GrafDef_Load: %s\n", exc.what());
	} catch (...) {
		fprintf(stdout, "unknown exception in GrafDef_Load\n");
	}
	
	fclose(file);
	
	return inList;
}


GraphDef* GraphDef_LoadDir(World *inWorld, char *dirname, GraphDef *inList)
{
	//printf("->GraphDef_LoadDir\n");
	DIR *dir = opendir(dirname);	
	if (!dir) {
		printf("*** ERROR: open directory failed '%s'\n", dirname); fflush(stdout);
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
		//printf("'%s'\n", entrypathname);
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
	//printf("<-GraphDef_LoadDir\n");
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
	//printf("GraphDef_Free\n");
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
	printf("mName '%s'\n", (char*)inNodeDef->mName);
	printf("mHash %d\n", inNodeDef->mHash);
	printf("mAllocSize %lu\n", inNodeDef->mAllocSize);
}

void GraphDef_Dump(GraphDef *inGraphDef)
{
	NodeDef_Dump(&inGraphDef->mNodeDef);
	
	printf("mNumControls %d\n", inGraphDef->mNumControls);
	printf("mNumWires %d\n", inGraphDef->mNumWires);
	printf("mNumUnitSpecs %d\n", inGraphDef->mNumUnitSpecs);
	printf("mNumWireBufs %d\n", inGraphDef->mNumWireBufs);

	for (int i=0; i<inGraphDef->mNumControls; ++i) {
		printf("   %d mInitialControlValues %g\n", i, inGraphDef->mInitialControlValues[i]);
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
	refs = (int16*)calloc(maxsize, sizeof(int16));	
	stack = (int16*)calloc(maxsize, sizeof(int16));	
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
	//printf(">alloc stk %d\n", stackPtr);
	if (stackPtr) {
		outIndex = stack[--stackPtr];
	} else {
		outIndex = nextIndex++;
	}
	refs[outIndex] = count;
	//printf("<alloc %d  refs %d  stk %d  nxt %d\n", outIndex, refs[outIndex], stackPtr, nextIndex);
	return outIndex;
}

inline void BufColorAllocator::release(int inIndex)
{
	//printf(">rel %d  refs %d  stk %d\n", inIndex, refs[inIndex], stackPtr);
	if (refs[inIndex] == 0) exit(1);
	if (--refs[inIndex] == 0) stack[stackPtr++] = inIndex;
	//printf("<rel %d  refs %d  stk %d\n", inIndex, refs[inIndex], stackPtr);
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
			//printf("\n%d %s\n", j, unitSpec->mUnitDef->mUnitDefName);
			
			// set wire index, release inputs
			for (int i=unitSpec->mNumInputs-1; i>=0; --i) {
				InputSpec *inputSpec = unitSpec->mInputSpec + i;
				//printf("r %d %d\n", i, inputSpec->mFromUnitIndex);
				if (inputSpec->mFromUnitIndex >= 0) {
					UnitSpec *outUnit = inGraphDef->mUnitSpecs + inputSpec->mFromUnitIndex;
					OutputSpec *outputSpec = outUnit->mOutputSpec + inputSpec->mFromOutputIndex;
					inputSpec->mWireIndex = outputSpec->mWireIndex;
						//printf("release? %d %s %d  %d %d\n", inputSpec->mFromUnitIndex, outUnit->mUnitDef->mUnitDefName, 
						//	i, outputSpec->mBufferIndex, outputSpec->mCalcRate);
					if (outputSpec->mCalcRate == calc_FullRate) {
						//printf("release %d %s %d  %d\n", inputSpec->mFromUnitIndex, outUnit->mUnitDef->mUnitDefName, i, outputSpec->mBufferIndex);
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
					//printf("alloc %d %s %d  %d %d\n", j, unitSpec->mUnitDef->mUnitDefName, i, bufIndex, outputSpec->mNumConsumers);
				}
			}
		}
		inGraphDef->mNumWireBufs = bufColor.NumBufs();
		if (inWorld->hw->mAudioDriver->Active()) {
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
