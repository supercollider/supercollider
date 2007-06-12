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
#include "Unroll.h"

void Unit_ChooseMulAddFunc(Unit* unit);

////////////////////////////////////////////////////////////////////////////////

void Graph_Dtor(Graph *inGraph)
{
	//printf("->Graph_Dtor %d\n", inGraph->mNode.mID);
	World *world = inGraph->mNode.mWorld;
	for (int i = 0; i<inGraph->mNumUnits; ++i) {
		//printf("%d\n", i);
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
	//printf("<-Graph_Dtor\n");
}

////////////////////////////////////////////////////////////////////////////////

void Graph_Ctor(World *inWorld, GraphDef *inGraphDef, Graph *graph, sc_msg_iter *msg)
{	
	//printf("->Graph_Ctor\n");
	
	// hit the memory allocator only once.
	char *memory = (char*)World_Alloc(inWorld, inGraphDef->mTotalAllocSize);
	graph->mMemory = memory;
	
	// allocate space for children
	graph->mNumUnits = inGraphDef->mNumUnitSpecs;
	inWorld->mNumUnits += graph->mNumUnits;
	inWorld->mNumGraphs ++;	
	
	graph->mUnits = (Unit**)memory; 
	memory += inGraphDef->mUnitsAllocSize;

	// set calc func
	graph->mNode.mCalcFunc = (NodeCalcFunc)&Graph_Calc;
	
	graph->mSampleOffset = 0; // calculate this from OSC time offset.. later
	
	// allocate wires
	graph->mNumWires = inGraphDef->mNumWires;
	graph->mWire = (Wire*)memory;
	memory += inGraphDef->mWiresAllocSize;

	graph->mNumCalcUnits = inGraphDef->mNumCalcUnits;
	graph->mCalcUnits = (Unit**)memory;
	memory += inGraphDef->mCalcUnitsAllocSize;
	
	// initialize controls
	graph->mNumControls = inGraphDef->mNumControls;
	graph->mControls = (float*)memory;
	memory += inGraphDef->mControlAllocSize;
	
	graph->mMapControls = (float**)memory;
	memory += inGraphDef->mMapControlsAllocSize;
	
	for (int i=0; i<graph->mNumControls; ++i) {
		graph->mControls[i] = inGraphDef->mInitialControlValues[i];
		graph->mMapControls[i] = graph->mControls + i;
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
	{
		float *constants = inGraphDef->mConstants;
		Wire *wire = graphWires;
		for (int i=0; i<inGraphDef->mNumConstants; ++i, ++wire) {
			wire->mFromUnit = 0;
			wire->mCalcRate = calc_Scalar;
			wire->mBuffer = &wire->mScalarValue;
			wire->mScalarValue = constants[i];
		}
	}
	
	// initialize units
	//printf("initialize units\n");
	float *bufspace = inWorld->hw->mWireBufSpace;
	int wireCtr=inGraphDef->mNumConstants;
	int calcCtr=0;
	for (int i=0; i<graph->mNumUnits; ++i) {
		// construct unit from spec
		UnitSpec *unitSpec = inGraphDef->mUnitSpecs + i;
		Unit *unit = Unit_New(inWorld, unitSpec, memory);
		
		// set parent
		unit->mParent = graph;
		unit->mParentIndex = i;
		if (unit->mCalcRate == calc_Full) {
			unit->mRate = &inWorld->mFullRate;
		} else {
			unit->mRate = &inWorld->mBufRate;
		}
		unit->mBufLength = unit->mRate->mBufLength;
		
		graph->mUnits[i] = unit;
		
		// hook up unit inputs
		//printf("hook up unit inputs\n");
		InputSpec *inputSpec = unitSpec->mInputSpec;
		for (int j=0; j<unitSpec->mNumInputs; ++j, ++inputSpec) {			
			Wire *wire = graphWires + inputSpec->mWireIndex;
			unit->mInput[j] = wire;
			unit->mInBuf[j] = wire->mBuffer;
		}
		// hook up unit outputs
		//printf("hook up unit outputs\n");
		for (int j=0; j<unitSpec->mNumOutputs; ++j) {
			OutputSpec *outputSpec = unitSpec->mOutputSpec + j;
			Wire *wire = graph->mWire + wireCtr++;
			wire->mFromUnit = unit;
			wire->mCalcRate = outputSpec->mCalcRate;
			if (wire->mCalcRate == calc_FullRate) {
				wire->mBuffer = bufspace + outputSpec->mBufferIndex;
			} else {
				wire->mBuffer = &wire->mScalarValue;
			}
			//wire->mScalarValue = 0.;
			unit->mOutput[j] = wire;
			unit->mOutBuf[j] = wire->mBuffer;
		}
		
		//printf("->mUnitCtorFunc %08X\n", unit->mUnitDef->mUnitCtorFunc);
		(*unit->mUnitDef->mUnitCtorFunc)(unit);
		//printf("<-mUnitCtorFunc\n");
		if (unit->mCalcRate != calc_Scalar) {
			graph->mCalcUnits[calcCtr++] = unit;
		}
	}
	inGraphDef->mRefCount ++ ;
	//printf("<-Graph_Ctor\n");
}

void Graph_Calc(Graph *inGraph)
{
	//printf("->Graph_Calc\n");
	for (int i=0; i<inGraph->mNumCalcUnits; ++i) {
		Unit *unit = inGraph->mCalcUnits[i];
		/*printf("  unit %d %s  i", i, unit->mUnitDef->mUnitDefName);
		for (int j=0; j<unit->mNumInputs; ++j) {
			printf(" %g", ZIN0(j));
		}
		printf("\n");*/
		(unit->mCalcFunc)(unit, unit->mBufLength);
		/*printf("    o");
		for (int j=0; j<unit->mNumOutputs; ++j) {
			printf(" %g", ZOUT0(j));
		}
		printf("\n");*/
	}
	//printf("<-Graph_Calc\n");
}

void Graph_CalcTrace(Graph *inGraph);
void Graph_CalcTrace(Graph *inGraph)
{
	//printf("->Graph_Calc\n");
	for (int i=0; i<inGraph->mNumCalcUnits; ++i) {
		Unit *unit = inGraph->mCalcUnits[i];
		printf("  unit %d %s\n    in ", i, (char*)unit->mUnitDef->mUnitDefName);
		for (int j=0; j<unit->mNumInputs; ++j) {
			printf(" %g", ZIN0(j));
		}
		printf("\n");
		(unit->mCalcFunc)(unit, unit->mBufLength);
		printf("    out");
		for (int j=0; j<unit->mNumOutputs; ++j) {
			printf(" %g", ZOUT0(j));
		}
		printf("\n");
	}
	inGraph->mNode.mCalcFunc = (NodeCalcFunc)&Graph_Calc;
	//printf("<-Graph_Calc\n");
}

void Graph_Trace(Graph *inGraph)
{
	inGraph->mNode.mCalcFunc = (NodeCalcFunc)&Graph_CalcTrace;
}

void Graph_EndCalc(Graph* inGraph);
void Graph_EndCalc(Graph* inGraph)
{
	// zero outputs?
	Node_Delete(&inGraph->mNode);
}

void Graph_End(Graph* inGraph)
{
	//printf("->Graph_End\n");
	inGraph->mNode.mCalcFunc = (NodeCalcFunc)&Graph_EndCalc;
}



void Graph_SetControl(Graph* inGraph, int inIndex, float inValue)
{
	//printf("Graph_SetControl %d %d %g\n", inGraph->mNode.mID, inIndex, inValue);
	if (inIndex < 0 || inIndex >= GRAPHDEF(inGraph)->mNumControls) return;
	inGraph->mControls[inIndex] = inValue;
	inGraph->mControlTouched = inGraph->mNode.mWorld->mBufCounter;
	//printf("<<Graph_SetControl %08X %08X %g\n", inGraph->mControls + inIndex, inGraph->mMapControls[inIndex], inGraph->mControls[inIndex]);
}

void Graph_SetControl(Graph* inGraph, int32 *inName, int inIndex, float inValue)
{
	//printf("Graph_SetControl %d %s %d %g\n", inGraph->mNode.mID, inName, inIndex, inValue);
	GraphDef *def = (GraphDef*)(inGraph->mNode.mDef);
	HashTable<ParamSpec, Malloc>* table = def->mParamSpecTable;
	if (!table) return;
	ParamSpec *spec = table->Get(inName);
	if (!spec) return;
	//printf("spec->mName %s\n", spec->mName);
	//printf("spec->mIndex %d\n", spec->mIndex);
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
	//printf("Graph_SetControl %d %d %g\n", inGraph->mNode.mID, inIndex, inValue);
	if (inIndex < 0 || inIndex >= GRAPHDEF(inGraph)->mNumControls) return;
	World *world = inGraph->mNode.mWorld;
	if (inBus < -1 || inBus >= world->mNumControlBusChannels) return;
	if (inBus == -1) {
		inGraph->mMapControls[inIndex] = inGraph->mControls + inIndex;
	} else {
		inGraph->mMapControls[inIndex] = world->mControlBus + inBus;
	}
}

