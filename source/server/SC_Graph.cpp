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


#include "SC_Graph.h"
#include "SC_GraphDef.h"
#include "SC_Unit.h"
#include "SC_UnitSpec.h"
#include "SC_UnitDef.h"
#include "SC_HiddenWorld.h"
#include "SC_Wire.h"
#include "SC_WireSpec.h"
#include "SC_BufSpec.h"
#include <stdio.h>
#include <string.h>
#include "SC_Prototypes.h"
#include "SC_Errors.h"
#include "Unroll.h"

void Unit_ChooseMulAddFunc(Unit* unit);

////////////////////////////////////////////////////////////////////////////////

void Graph_Dtor(Graph *inGraph)
{
	//scprintf("->Graph_Dtor %d\n", inGraph->mNode.mID);
	World *world = inGraph->mNode.mWorld;
	for (int i = 0; i<inGraph->mNumUnits; ++i) {
		//scprintf("%d\n", i);
		Unit *unit = inGraph->mUnits[i];
		UnitDtorFunc dtor = unit->mUnitDef->mUnitDtorFunc;
		if (dtor) (dtor)(unit);
	}
	world->mNumUnits -= inGraph->mNumUnits;
	world->mNumGraphs --;
	
	GraphDef* def = (GraphDef*)inGraph->mNode.mDef;
	if (--def->mRefCount <= 0) {
		GraphDef_DeleteMsg(world, def);
	}
	
	World_Free(world, inGraph->mMemory);
	Node_Dtor(&inGraph->mNode);
	//scprintf("<-Graph_Dtor\n");
}

////////////////////////////////////////////////////////////////////////////////

void Graph_FirstCalc(Graph *inGraph);


void Graph_Ctor(World *inWorld, GraphDef *inGraphDef, Graph *graph, sc_msg_iter *msg)
{	
	//scprintf("->Graph_Ctor\n");
	
	// hit the memory allocator only once.
	char *memory = (char*)World_Alloc(inWorld, inGraphDef->mTotalAllocSize);
	graph->mMemory = memory;
	
	// allocate space for children
	int numUnits = inGraphDef->mNumUnitSpecs;
	graph->mNumUnits = numUnits;
	inWorld->mNumUnits += numUnits;
	inWorld->mNumGraphs ++;	
	
	graph->mUnits = (Unit**)memory; 
	memory += inGraphDef->mUnitsAllocSize;

	// set calc func
	graph->mNode.mCalcFunc = (NodeCalcFunc)&Graph_FirstCalc;
	
	// allocate wires
	graph->mNumWires = inGraphDef->mNumWires;
	graph->mWire = (Wire*)memory;
	
	memory += inGraphDef->mWiresAllocSize;

	graph->mNumCalcUnits = inGraphDef->mNumCalcUnits;
	graph->mCalcUnits = (Unit**)memory;
	memory += inGraphDef->mCalcUnitsAllocSize;
	
	// initialize controls
	int numControls = inGraphDef->mNumControls;
	graph->mNumControls = numControls;
	graph->mControls = (float*)memory;
	memory += inGraphDef->mControlAllocSize;
	
	graph->mMapControls = (float**)memory;
	memory += inGraphDef->mMapControlsAllocSize;
	
	{
		float*  graphControls = graph->mControls;
		float*  initialControlValues = inGraphDef->mInitialControlValues;
		float** graphMapControls = graph->mMapControls;
		for (int i=0; i<numControls; ++i, ++graphControls) {
			*graphControls = initialControlValues[i];
			graphMapControls[i] = graphControls;
		}
	}
	
	// set controls
	while (msg->remain() >= 8) {
		if (msg->nextTag('i') == 's') {
			int32* name = msg->gets4();
			float32 value = msg->getf();
			Graph_SetControl(graph, name, 0, value);
		} else {
			int32 index = msg->geti();
			float32 value = msg->getf();
			Graph_SetControl(graph, index, value);
		}
	}

	graph->mControlTouched = inWorld->mBufCounter;

	// set up scalar values
	Wire *graphWires = graph->mWire;
	int numConstants = inGraphDef->mNumConstants;
	{
		float *constants = inGraphDef->mConstants;
		Wire *wire = graphWires;
		for (int i=0; i<numConstants; ++i, ++wire) {
			wire->mFromUnit = 0;
			wire->mCalcRate = calc_ScalarRate;
			wire->mBuffer = &wire->mScalarValue;
			wire->mScalarValue = constants[i];
		}
	}
	
	graph->mSampleOffset = inWorld->mSampleOffset;
	graph->mRGen = inWorld->mRGen; // defaults to rgen zero.
	
	// initialize units
	//scprintf("initialize units\n");
	Unit** calcUnits = graph->mCalcUnits;
	int calcCtr=0;

	float *bufspace = inWorld->hw->mWireBufSpace;
	int wireCtr = numConstants;
	for (int i=0; i<numUnits; ++i) {
		// construct unit from spec
		UnitSpec *unitSpec = inGraphDef->mUnitSpecs + i;
		Unit *unit = Unit_New(inWorld, unitSpec, memory);
		
		// set parent
		unit->mParent = graph;
		unit->mParentIndex = i;
		
		graph->mUnits[i] = unit;
		
		{
			// hook up unit inputs
			//scprintf("hook up unit inputs\n");
			InputSpec *inputSpec = unitSpec->mInputSpec;
			Wire** unitInput = unit->mInput;
			float** unitInBuf = unit->mInBuf;
			int numInputs = unitSpec->mNumInputs;
			for (int j=0; j<numInputs; ++j, ++inputSpec) {			
				Wire *wire = graphWires + inputSpec->mWireIndex;
				unitInput[j] = wire;
				unitInBuf[j] = wire->mBuffer;
			}
		}
		
		{
			// hook up unit outputs
			//scprintf("hook up unit outputs\n");
			Wire** unitOutput = unit->mOutput;
			float** unitOutBuf = unit->mOutBuf;
			int numOutputs = unitSpec->mNumOutputs;
			Wire *wire = graphWires + wireCtr;
			wireCtr += numOutputs;
			int unitCalcRate = unit->mCalcRate;
			if (unitCalcRate == calc_FullRate) {
				OutputSpec *outputSpec = unitSpec->mOutputSpec;
				for (int j=0; j<numOutputs; ++j, ++wire, ++outputSpec) {
					wire->mFromUnit = unit;
					wire->mCalcRate = calc_FullRate;
					wire->mBuffer = bufspace + outputSpec->mBufferIndex;
					unitOutput[j] = wire;
					unitOutBuf[j] = wire->mBuffer;
				}
				calcUnits[calcCtr++] = unit;
			} else {
				for (int j=0; j<numOutputs; ++j, ++wire) {
					wire->mFromUnit = unit;
					wire->mCalcRate = unitCalcRate;
					wire->mBuffer = &wire->mScalarValue;
					unitOutput[j] = wire;
					unitOutBuf[j] = wire->mBuffer;
				}
				if (unitCalcRate != calc_ScalarRate) {
					calcUnits[calcCtr++] = unit;
				}
			}
		}
	}

	inGraphDef->mRefCount ++ ;
}

void Graph_FirstCalc(Graph *inGraph)
{
	//scprintf("->Graph_FirstCalc\n");
	int numUnits = inGraph->mNumUnits;
	Unit **units = inGraph->mUnits;
	for (int i=0; i<numUnits; ++i) {
		Unit *unit = units[i];
		// call constructor
		(*unit->mUnitDef->mUnitCtorFunc)(unit);
	}
	//scprintf("<-Graph_FirstCalc\n");
	
	inGraph->mNode.mCalcFunc = (NodeCalcFunc)&Graph_Calc;
	// now do actual graph calculation
	Graph_Calc(inGraph);
}

void Graph_Calc(Graph *inGraph)
{
	//scprintf("->Graph_Calc\n");
	int numCalcUnits = inGraph->mNumCalcUnits;
	Unit **calcUnits = inGraph->mCalcUnits;
	for (int i=0; i<numCalcUnits; ++i) {
		Unit *unit = calcUnits[i];
		(unit->mCalcFunc)(unit, unit->mBufLength);
	}
	//scprintf("<-Graph_Calc\n");
}

void Graph_CalcTrace(Graph *inGraph);
void Graph_CalcTrace(Graph *inGraph)
{
	scprintf("\nTRACE %d\n", inGraph->mNode.mID);
	int numCalcUnits = inGraph->mNumCalcUnits;
	Unit **calcUnits = inGraph->mCalcUnits;
	for (int i=0; i<numCalcUnits; ++i) {
		Unit *unit = calcUnits[i];
		scprintf("  unit %d %s\n    in ", i, (char*)unit->mUnitDef->mUnitDefName);
		for (int j=0; j<unit->mNumInputs; ++j) {
			scprintf(" %g", ZIN0(j));
		}
		scprintf("\n");
		(unit->mCalcFunc)(unit, unit->mBufLength);
		scprintf("    out");
		for (int j=0; j<unit->mNumOutputs; ++j) {
			scprintf(" %g", ZOUT0(j));
		}
		scprintf("\n");
	}
	inGraph->mNode.mCalcFunc = (NodeCalcFunc)&Graph_Calc;
}

void Graph_Trace(Graph *inGraph)
{
	if (inGraph->mNode.mCalcFunc == (NodeCalcFunc)&Graph_Calc) {
		inGraph->mNode.mCalcFunc = (NodeCalcFunc)&Graph_CalcTrace;
	}
}


int Graph_GetControl(Graph* inGraph, int inIndex, float& outValue)
{
	if (inIndex < 0 || inIndex >= GRAPHDEF(inGraph)->mNumControls) return kSCErr_IndexOutOfRange;
	outValue = inGraph->mControls[inIndex];
	return kSCErr_None;
}

int Graph_GetControl(Graph* inGraph, int32 *inName, int inIndex, float& outValue)
{
	GraphDef *def = (GraphDef*)(inGraph->mNode.mDef);
	HashTable<ParamSpec, Malloc>* table = def->mParamSpecTable;
	if (!table) return kSCErr_IndexOutOfRange;
	ParamSpec *spec = table->Get(inName);
	if (!spec) return kSCErr_IndexOutOfRange;
	int index = spec->mIndex + inIndex;	
	return Graph_GetControl(inGraph, index, outValue);
}

void Graph_SetControl(Graph* inGraph, int inIndex, float inValue)
{
	if (inIndex < 0 || inIndex >= GRAPHDEF(inGraph)->mNumControls) return;
	inGraph->mControls[inIndex] = inValue;
	inGraph->mControlTouched = inGraph->mNode.mWorld->mBufCounter;
}

void Graph_SetControl(Graph* inGraph, int32 *inName, int inIndex, float inValue)
{
	GraphDef *def = (GraphDef*)(inGraph->mNode.mDef);
	HashTable<ParamSpec, Malloc>* table = def->mParamSpecTable;
	if (!table) return;
	ParamSpec *spec = table->Get(inName);
	if (!spec) return;
	int index = spec->mIndex + inIndex;	
	Graph_SetControl(inGraph, index, inValue);
}



void Graph_MapControl(Graph* inGraph, int32 *inName, int inIndex, int inBus)
{
	GraphDef *def = (GraphDef*)(inGraph->mNode.mDef);
	HashTable<ParamSpec, Malloc>* table = def->mParamSpecTable;
	if (!table) return;
	ParamSpec *spec = table->Get(inName);
	if (!spec) return;
	int index = spec->mIndex + inIndex;	
	Graph_MapControl(inGraph, index, inBus);
}

void Graph_MapControl(Graph* inGraph, int inIndex, int inBus)
{
	if (inIndex < 0 || inIndex >= GRAPHDEF(inGraph)->mNumControls) return;
	World *world = inGraph->mNode.mWorld;
	if (inBus < -1 || inBus >= world->mNumControlBusChannels) return;
	if (inBus == -1) {
		inGraph->mMapControls[inIndex] = inGraph->mControls + inIndex;
	} else {
		inGraph->mMapControls[inIndex] = world->mControlBus + inBus;
	}
}

