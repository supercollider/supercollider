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


#include "SC_Win32Utils.h"
#include "SC_Graph.h"
#include "SC_GraphDef.h"
#include "SC_Unit.h"
#include "SC_UnitSpec.h"
#include "SC_UnitDef.h"
#include "SC_HiddenWorld.h"
#include "SC_WorldOptions.h"
#include "SC_Wire.h"
#include "SC_WireSpec.h"
#include <stdio.h>
#include <string.h>
#include "SC_Prototypes.h"
#include "SC_Errors.h"
#include "Unroll.h"

void Unit_ChooseMulAddFunc(Unit* unit);

////////////////////////////////////////////////////////////////////////////////

void Graph_FirstCalc(Graph *inGraph);

void Graph_Dtor(Graph *inGraph)
{
	//scprintf("->Graph_Dtor %d\n", inGraph->mNode.mID);
	World *world = inGraph->mNode.mWorld;
	uint32 numUnits = inGraph->mNumUnits;
	Unit** graphUnits = inGraph->mUnits;
	if (inGraph->mNode.mCalcFunc != (NodeCalcFunc)Graph_FirstCalc) {
		// the above test insures that dtors are not called if ctors have not been called.
		for (uint32 i = 0; i<numUnits; ++i) {
			Unit *unit = graphUnits[i];
			UnitDtorFunc dtor = unit->mUnitDef->mUnitDtorFunc;
			if (dtor) (dtor)(unit);
		}
	}
	world->mNumUnits -= numUnits;
	world->mNumGraphs --;

	GraphDef* def = GRAPHDEF(inGraph);
	if (--def->mRefCount <= 0) {
		if (world->mRealTime) GraphDef_DeleteMsg(world, def);
		else GraphDef_Free(def);
	}

	Node_Dtor(&inGraph->mNode);
	//scprintf("<-Graph_Dtor\n");
}

////////////////////////////////////////////////////////////////////////////////

int Graph_New(struct World *inWorld, struct GraphDef *inGraphDef, int32 inID,
              struct sc_msg_iter* args, Graph** outGraph,bool argtype)//true for normal args , false for setn type args
{
	Graph* graph;
	int err = Node_New(inWorld, &inGraphDef->mNodeDef, inID, (Node**)&graph);
	if (err) return err;
	Graph_Ctor(inWorld, inGraphDef, graph, args,argtype);
	*outGraph = graph;
	return err;
}


void Graph_Ctor(World *inWorld, GraphDef *inGraphDef, Graph *graph, sc_msg_iter *msg,bool argtype)//true for normal args , false for setn type args
{
	//scprintf("->Graph_Ctor\n");

	// hit the memory allocator only once.
	char *memory = (char*)graph + sizeof(Graph);

	// allocate space for children
	uint32 numUnits = inGraphDef->mNumUnitSpecs;
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
	uint32 numControls = inGraphDef->mNumControls;
	graph->mNumControls = numControls;
	graph->mControls = (float*)memory;
	memory += inGraphDef->mControlAllocSize;

	graph->mAudioBusOffsets = (int32*)memory;
	memory += inGraphDef->mAudioMapBusOffsetSize;

	graph->mMapControls = (float**)memory;
	memory += inGraphDef->mMapControlsAllocSize;

	graph->mControlRates = (int*)memory;
	memory += inGraphDef->mMapControlRatesAllocSize;

	{
		float*  graphControls = graph->mControls;
		float*  initialControlValues = inGraphDef->mInitialControlValues;
		float** graphMapControls = graph->mMapControls;
		/* add */
		int* graphControlRates = graph->mControlRates;
		for (uint32 i=0; i<numControls; ++i, ++graphControls) {
			*graphControls = initialControlValues[i];
			graphMapControls[i] = graphControls;
			/* add */
			graphControlRates[i] = 0;  // init to 0 for now... control bus is 1, audio is 2
			graph->mAudioBusOffsets[i] = -1;
		}
	}

	// set controls
	//if argtype == true -> normal args as always
	//if argtype == false -> setn type args
	if(argtype) {
		while( msg->remain()>=8) {
			int i = 0;
			int loop = 0;
			if (msg->nextTag('i') == 's') {
				int32* name = msg->gets4();
				int32 hash = Hash(name);
				do {
					switch (msg->nextTag('f') ) {
						case  'f' :
						case  'i' :
						{
							float32 value = msg->getf();
							Graph_SetControl(graph, hash, name, i, value);
							break;
						}
						case 's' :
						{
							const char* string = msg->gets();
							if ( *string == 'c') {
								int bus = sc_atoi(string+1);
								Graph_MapControl(graph, hash, name, i, bus);
							} else {
								if (*string == 'a') {
									int bus = sc_atoi(string+1);
									Graph_MapAudioControl(graph, hash, name, i, bus);
								}
							}
							break;
						}
						case ']':
							msg->count++;
							loop -= 1;
							break;
						case '[':
							msg->count++;
							loop += 1;
							i -= 1;
							break;
					}
					++i;
				}
				while (loop);
			} else {
				int32 index = msg->geti();
				do {
					switch (msg->nextTag('f') ) {
						case  'f' :
						case  'i' :
						{
							float32 value = msg->getf();
							Graph_SetControl(graph, index + i, value);
							break;
						}
						case 's' :
						{
							const char* string = msg->gets();
							if ( *string == 'c') {
								int bus = sc_atoi(string+1);
								Graph_MapControl(graph, index + i, bus);
							} else {
								if (*string == 'a') {
									int bus = sc_atoi(string+1);
									Graph_MapAudioControl(graph, index + i, bus);
								}
							}
							break;
						}
						case ']':
							msg->count++;
							loop -= 1;
							break;
						case '[':
							msg->count++;
							loop += 1;
							i -= 1;
							break;
					}
					++i;
				}
				while (loop);
			}
		}
	}


	//{
//	    while( msg->remain()>=8) {
//		int i = 0;
//		int loop = 0;
//		if (msg->nextTag('i') == 's') {
//		    int32* name = msg->gets4();
//		    int32 hash = Hash(name);
//		    if (msg->nextTag('f') == '[' ) {
//			    msg->count++;
//			    loop = 1;
//		    }
//		    do {
//			if (msg->nextTag('f') == 's' ) {
//			    const char* string = msg->gets();
//			    if ( *string == 'c') {
//				int bus = sc_atoi(string+1);
//				Graph_MapControl(graph, hash, name, i, bus);
//			    }
//			} else {
//			    if (msg->nextTag('f') == ']' ) {
//				msg->count++;
//				loop = 0;
//			    } else {
//				float32 value = msg->getf();
//				Graph_SetControl(graph, hash, name, i, value);
//			    }
//			}
//			++i;
//		    }
//		    while (loop);
//		} else {
//		    int32 index = msg->geti();
//		    if (msg->nextTag('f') == '[' ) {
//			msg->count++;
//			loop = 1;
//		    }
//		    do {
//			if (msg->nextTag('f') == 's') {
//			    const char* string = msg->gets();
//			    if (*string == 'c') {
//				int bus = sc_atoi(string+1);
//				Graph_MapControl(graph, index + i, bus);
//			    }
//			} else {
//			    if (msg->nextTag('f') == ']' ) {
//				msg->count++;
//				loop = 0;
//			    } else {
//				float32 value = msg->getf();
//				Graph_SetControl(graph, index + i, value);
//			    }
//			}
//			++i;
//		    }
//		    while (loop);
//		}
//	    }
//
//	}
	else{
		while (msg->remain()) {
			if (msg->nextTag('i') == 's') {
				int32* name = msg->gets4();
				int32 hash = Hash(name);
				int32 n = msg->geti();
				for (int i=0; msg->remain() && i<n; ++i) {
					if (msg->nextTag('f') == 's') {
						const char* string = msg->gets();
						if (*string == 'c') {
							int bus = sc_atoi(string+1);
							Graph_MapControl(graph, hash, name, i, bus);
							//Node_MapControl(node, hash, name, i, bus);
						} else {
							if (*string == 'a') {
								int bus = sc_atoi(string+1);
								Graph_MapAudioControl(graph, hash, name, i, bus);
							}
						}
					} else {
						float32 value = msg->getf();
						Graph_SetControl(graph, hash, name, i, value);
						//Node_SetControl(node, hash, name, i, value);
					}
				}
			} else {
				int32 index = msg->geti();
				int32 n = msg->geti();
				for (int i=0; msg->remain() && i<n; ++i) {
					if (msg->nextTag('f') == 's') {
						const char* string = msg->gets();
						if (*string == 'c') {
							int bus = sc_atoi(string+1);
							Graph_MapControl(graph, index+i, bus);
							//Node_MapControl(node, index+i, bus);
						} else {
							if (*string == 'a') {
								int bus = sc_atoi(string+1);
								Graph_MapAudioControl(graph, index + i, bus);
							}
						}
					} else {
						float32 value = msg->getf();
						Graph_SetControl(graph, index+i, value);
						//Node_SetControl(node, index+i, value);
					}
				}
			}
		}
	}

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
	graph->mSubsampleOffset = inWorld->mSubsampleOffset;
	graph->mRGen = inWorld->mRGen; // defaults to rgen zero.

	graph->mLocalAudioBusUnit = NULL;
	graph->mLocalControlBusUnit = NULL;

	graph->localBufNum = 0;
	graph->localMaxBufNum = 0; // this is set from synth

	// initialize units
	//scprintf("initialize units\n");
	Unit** calcUnits = graph->mCalcUnits;
	Unit** graphUnits = graph->mUnits;
	int calcCtr=0;

	float *bufspace = inWorld->hw->mWireBufSpace;
	uint32 wireCtr = numConstants; // never more than numConstants + numOutputs
	UnitSpec *unitSpec = inGraphDef->mUnitSpecs;
	for (uint32 i=0; i<numUnits; ++i, ++unitSpec) {
		// construct unit from spec
		Unit *unit = Unit_New(inWorld, unitSpec, memory);

		// set parent
		unit->mParent = graph;
		unit->mParentIndex = i;

		graphUnits[i] = unit;

		{
			// hook up unit inputs
			//scprintf("hook up unit inputs\n");
			InputSpec *inputSpec = unitSpec->mInputSpec;
			Wire** unitInput = unit->mInput;
			float** unitInBuf = unit->mInBuf;
			uint32 numInputs = unitSpec->mNumInputs;
			for (uint32 j=0; j<numInputs; ++j, ++inputSpec) {
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
			uint32 numOutputs = unitSpec->mNumOutputs;
			Wire *wire = graphWires + wireCtr;
			wireCtr += numOutputs;
			int unitCalcRate = unit->mCalcRate;
			if (unitCalcRate == calc_FullRate) {
				OutputSpec *outputSpec = unitSpec->mOutputSpec;
				for (uint32 j=0; j<numOutputs; ++j, ++wire, ++outputSpec) {
					wire->mFromUnit = unit;
					wire->mCalcRate = calc_FullRate;
					wire->mBuffer = bufspace + outputSpec->mBufferIndex;
					unitOutput[j] = wire;
					unitOutBuf[j] = wire->mBuffer;
				}
				calcUnits[calcCtr++] = unit;
			} else {
				for (uint32 j=0; j<numOutputs; ++j, ++wire) {
					wire->mFromUnit = unit;
					wire->mCalcRate = unitCalcRate;
					wire->mBuffer = &wire->mScalarValue;
					unitOutput[j] = wire;
					unitOutBuf[j] = wire->mBuffer;
				}
				if (unitCalcRate == calc_BufRate) {
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
	uint32 numUnits = inGraph->mNumUnits;
	Unit **units = inGraph->mUnits;
	for (uint32 i=0; i<numUnits; ++i) {
		Unit *unit = units[i];
		// call constructor
		(*unit->mUnitDef->mUnitCtorFunc)(unit);
	}
	//scprintf("<-Graph_FirstCalc\n");

	inGraph->mNode.mCalcFunc = (NodeCalcFunc)&Graph_Calc;
	// now do actual graph calculation
	Graph_Calc(inGraph);
}

void Node_NullCalc(struct Node* /*inNode*/);

void Graph_NullFirstCalc(Graph *inGraph)
{
	//scprintf("->Graph_FirstCalc\n");
	uint32 numUnits = inGraph->mNumUnits;
	Unit **units = inGraph->mUnits;
	for (uint32 i=0; i<numUnits; ++i) {
		Unit *unit = units[i];
		// call constructor
		(*unit->mUnitDef->mUnitCtorFunc)(unit);
	}
	//scprintf("<-Graph_FirstCalc\n");

	inGraph->mNode.mCalcFunc = &Node_NullCalc;
}

inline void Graph_Calc_unit(Unit * unit)
{
	(unit->mCalcFunc)(unit, unit->mBufLength);
}

void Graph_Calc(Graph *inGraph)
{
	//scprintf("->Graph_Calc\n");
	uint32 numCalcUnits = inGraph->mNumCalcUnits;
	Unit **calcUnits = inGraph->mCalcUnits;

	int unroll8 = numCalcUnits / 8;
	int remain8 = numCalcUnits % 8;
	int i = 0;

	for (int j = 0; j!=unroll8; i += 8, ++j) {
		Graph_Calc_unit(calcUnits[i]);
		Graph_Calc_unit(calcUnits[i+1]);
		Graph_Calc_unit(calcUnits[i+2]);
		Graph_Calc_unit(calcUnits[i+3]);
		Graph_Calc_unit(calcUnits[i+4]);
		Graph_Calc_unit(calcUnits[i+5]);
		Graph_Calc_unit(calcUnits[i+6]);
		Graph_Calc_unit(calcUnits[i+7]);
	}

	int unroll4 = remain8 / 4;
	int remain4 = remain8 % 4;
	if (unroll4) {
		Graph_Calc_unit(calcUnits[i]);
		Graph_Calc_unit(calcUnits[i+1]);
		Graph_Calc_unit(calcUnits[i+2]);
		Graph_Calc_unit(calcUnits[i+3]);
		i+=4;
	}

	int unroll2 = remain4 / 2;
	int remain2 = remain4 % 2;
	if (unroll2) {
		Graph_Calc_unit(calcUnits[i]);
		Graph_Calc_unit(calcUnits[i+1]);
		i += 2;
	}

	if (remain2)
		Graph_Calc_unit(calcUnits[i]);

	//scprintf("<-Graph_Calc\n");
}

void Graph_CalcTrace(Graph *inGraph);
void Graph_CalcTrace(Graph *inGraph)
{
	uint32 numCalcUnits = inGraph->mNumCalcUnits;
	Unit **calcUnits = inGraph->mCalcUnits;
	scprintf("\nTRACE %d  %s    #units: %d\n", inGraph->mNode.mID, inGraph->mNode.mDef->mName, numCalcUnits);
	for (uint32 i=0; i<numCalcUnits; ++i) {
		Unit *unit = calcUnits[i];
		scprintf("  unit %d %s\n    in ", i, (char*)unit->mUnitDef->mUnitDefName);
		for (uint32 j=0; j<unit->mNumInputs; ++j) {
			scprintf(" %g", ZIN0(j));
		}
		scprintf("\n");
		(unit->mCalcFunc)(unit, unit->mBufLength);
		scprintf("    out");
		for (uint32 j=0; j<unit->mNumOutputs; ++j) {
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


int Graph_GetControl(Graph* inGraph, uint32 inIndex, float& outValue)
{
	if (inIndex >= GRAPHDEF(inGraph)->mNumControls) return kSCErr_IndexOutOfRange;
	outValue = inGraph->mControls[inIndex];
	return kSCErr_None;
}

int Graph_GetControl(Graph* inGraph, int32 inHash, int32 *inName, uint32 inIndex, float& outValue)
{
	ParamSpecTable* table = GRAPH_PARAM_TABLE(inGraph);
	ParamSpec *spec = table->Get(inHash, inName);
	if (!spec || inIndex >= spec->mNumChannels) return kSCErr_IndexOutOfRange;
	return Graph_GetControl(inGraph, spec->mIndex + inIndex, outValue);
}

void Graph_SetControl(Graph* inGraph, uint32 inIndex, float inValue)
{
	if (inIndex >= GRAPHDEF(inGraph)->mNumControls) return;
	inGraph->mControlRates[inIndex] = 0;
	float *ptr = inGraph->mControls + inIndex;
	inGraph->mMapControls[inIndex] = ptr; // unmap the control
	*ptr = inValue;
}

void Graph_SetControl(Graph* inGraph, int32 inHash, int32 *inName, uint32 inIndex, float inValue)
{
	ParamSpecTable* table = GRAPH_PARAM_TABLE(inGraph);
	ParamSpec *spec = table->Get(inHash, inName);
	if (!spec || inIndex >= spec->mNumChannels) return;
	//printf("setting: %s: to value %f\n", spec->mName, inValue);
	Graph_SetControl(inGraph, spec->mIndex + inIndex, inValue);
}



void Graph_MapControl(Graph* inGraph, int32 inHash, int32 *inName, uint32 inIndex, uint32 inBus)
{
	ParamSpecTable* table = GRAPH_PARAM_TABLE(inGraph);
	ParamSpec *spec = table->Get(inHash, inName);
	if (!spec || inIndex >= spec->mNumChannels) return;
	//printf("mapping: %s: to bus index %i\n", spec->mName, inBus);
	Graph_MapControl(inGraph, spec->mIndex + inIndex, inBus);
}

void Graph_MapControl(Graph* inGraph, uint32 inIndex, uint32 inBus)
{
	if (inIndex >= GRAPHDEF(inGraph)->mNumControls) return;
	World *world = inGraph->mNode.mWorld;
	if (inBus >= 0x80000000) {
		inGraph->mControlRates[inIndex] = 0;
		inGraph->mMapControls[inIndex] = inGraph->mControls + inIndex;
	} else if (inBus < world->mNumControlBusChannels) {
		inGraph->mControlRates[inIndex] = 1;
		inGraph->mMapControls[inIndex] = world->mControlBus + inBus;
	}
}

void Graph_MapAudioControl(Graph* inGraph, int32 inHash, int32 *inName, uint32 inIndex, uint32 inBus)
{
	ParamSpecTable* table = GRAPH_PARAM_TABLE(inGraph);
	ParamSpec *spec = table->Get(inHash, inName);
	if (!spec || inIndex >= spec->mNumChannels) return;
	//printf("mapping: %s: to bus index %i\n", spec->mName, inBus);
	if (spec) Graph_MapAudioControl(inGraph, spec->mIndex + inIndex, inBus);
}

void Graph_MapAudioControl(Graph* inGraph, uint32 inIndex, uint32 inBus)
{
	if (inIndex >= GRAPHDEF(inGraph)->mNumControls) return;
	World *world = inGraph->mNode.mWorld;
	/* what is the below doing??? it is unmapping by looking for negative ints */
	if (inBus >= 0x80000000) {
		inGraph->mControlRates[inIndex] = 0;
		inGraph->mMapControls[inIndex] = inGraph->mControls + inIndex;
	} else if (inBus < world->mNumAudioBusChannels) {
	inGraph->mControlRates[inIndex] = 2;
		inGraph->mAudioBusOffsets[inIndex] = inBus;
		inGraph->mMapControls[inIndex] = world->mAudioBus + (inBus * world->mBufLength);
	}
}
