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


#ifdef _WIN32
#    include "SC_Win32Utils.h"
#endif

#include "clz.h"
#include "SC_Graph.h"
#include "SC_GraphDef.h"
#include "SC_Wire.h"
#include "SC_WireSpec.h"
#include "SC_UnitSpec.h"
#include "SC_UnitDef.h"
#include "SC_HiddenWorld.h"
#include "SC_WorldOptions.h"
#include "ReadWriteMacros.h"
#include "SC_Prototypes.h"
#include "SC_CoreAudio.h"
#include "SC_SequencedCommand.h"
#include "SC_Errors.h"
#include "SC_Filesystem.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <stdexcept>
#include <string>

#include <boost/filesystem/operations.hpp> // recursive_directory_iterator
#include <boost/filesystem/string_file.hpp> // load_string_file

namespace bfs = boost::filesystem;

extern Malloc gMalloc;

const size_t ERR_BUF_SIZE(256);

int32 GetHash(ParamSpec* inParamSpec) { return inParamSpec->mHash; }

int32* GetKey(ParamSpec* inParamSpec) { return inParamSpec->mName; }


void ReadName(char*& buffer, int32* name);
void ReadName(char*& buffer, int32* name) {
    uint32 namelen = readUInt8(buffer);
    if (namelen >= kSCNameByteLen) {
        std::ostringstream os;
        os << "name too long (> " << kSCNameByteLen - 1 << " chars): " << std::string(buffer, namelen);
        throw std::runtime_error(os.str());
    }
    memset(name, 0, kSCNameByteLen);
    readData(buffer, (char*)name, namelen);
}

void ReadNodeDefName(char*& buffer, int32* name);
void ReadNodeDefName(char*& buffer, int32* name) {
    uint32 namelen = readUInt8(buffer);
    if (namelen >= kSCNodeDefNameByteLen) {
        std::ostringstream os;
        os << "node definition name too long (> " << kSCNodeDefNameByteLen - 1
           << " chars): " << std::string(buffer, namelen);
        throw std::runtime_error(os.str());
    }
    memset(name, 0, kSCNodeDefNameByteLen);
    readData(buffer, (char*)name, namelen);
}

void ParamSpec_Read(ParamSpec* inParamSpec, char*& buffer);
void ParamSpec_Read(ParamSpec* inParamSpec, char*& buffer) {
    ReadName(buffer, inParamSpec->mName);
    inParamSpec->mIndex = readInt32_be(buffer);
    inParamSpec->mHash = Hash(inParamSpec->mName);
}

void ParamSpec_ReadVer1(ParamSpec* inParamSpec, char*& buffer);
void ParamSpec_ReadVer1(ParamSpec* inParamSpec, char*& buffer) {
    ReadName(buffer, inParamSpec->mName);
    inParamSpec->mIndex = readInt16_be(buffer);
    inParamSpec->mHash = Hash(inParamSpec->mName);
}

void InputSpec_Read(InputSpec* inInputSpec, char*& buffer);
void InputSpec_Read(InputSpec* inInputSpec, char*& buffer) {
    inInputSpec->mFromUnitIndex = readInt32_be(buffer);
    inInputSpec->mFromOutputIndex = readInt32_be(buffer);

    inInputSpec->mWireIndex = -1;
}

void InputSpec_ReadVer1(InputSpec* inInputSpec, char*& buffer);
void InputSpec_ReadVer1(InputSpec* inInputSpec, char*& buffer) {
    inInputSpec->mFromUnitIndex = (int16)readInt16_be(buffer);
    inInputSpec->mFromOutputIndex = (int16)readInt16_be(buffer);

    inInputSpec->mWireIndex = -1;
}

void OutputSpec_Read(OutputSpec* inOutputSpec, char*& buffer);
void OutputSpec_Read(OutputSpec* inOutputSpec, char*& buffer) {
    inOutputSpec->mCalcRate = readInt8(buffer);
    inOutputSpec->mWireIndex = -1;
    inOutputSpec->mBufferIndex = -1;
    inOutputSpec->mNumConsumers = 0;
}

void UnitSpec_Read(UnitSpec* inUnitSpec, char*& buffer);
void UnitSpec_Read(UnitSpec* inUnitSpec, char*& buffer) {
    int32 name[kSCNameLen];
    ReadName(buffer, name);

    inUnitSpec->mUnitDef = GetUnitDef(name);
    if (!inUnitSpec->mUnitDef) {
        char str[ERR_BUF_SIZE];
        snprintf(str, ERR_BUF_SIZE, "UGen '%s' not installed.", (char*)name);
        throw std::runtime_error(str);
        return;
    }
    inUnitSpec->mCalcRate = readInt8(buffer);

    inUnitSpec->mNumInputs = readInt32_be(buffer);
    inUnitSpec->mNumOutputs = readInt32_be(buffer);
    inUnitSpec->mSpecialIndex = readInt16_be(buffer);
    inUnitSpec->mInputSpec = (InputSpec*)malloc(sizeof(InputSpec) * inUnitSpec->mNumInputs);
    inUnitSpec->mOutputSpec = (OutputSpec*)malloc(sizeof(OutputSpec) * inUnitSpec->mNumOutputs);
    for (uint32 i = 0; i < inUnitSpec->mNumInputs; ++i) {
        InputSpec_Read(inUnitSpec->mInputSpec + i, buffer);
    }
    for (uint32 i = 0; i < inUnitSpec->mNumOutputs; ++i) {
        OutputSpec_Read(inUnitSpec->mOutputSpec + i, buffer);
    }
    uint64 numPorts = inUnitSpec->mNumInputs + inUnitSpec->mNumOutputs;
    inUnitSpec->mAllocSize = inUnitSpec->mUnitDef->mAllocSize + numPorts * (sizeof(Wire*) + sizeof(float*));
}

void UnitSpec_ReadVer1(UnitSpec* inUnitSpec, char*& buffer);
void UnitSpec_ReadVer1(UnitSpec* inUnitSpec, char*& buffer) {
    int32 name[kSCNameLen];
    ReadName(buffer, name);

    inUnitSpec->mUnitDef = GetUnitDef(name);
    if (!inUnitSpec->mUnitDef) {
        char str[ERR_BUF_SIZE];
        snprintf(str, ERR_BUF_SIZE, "UGen '%s' not installed.", (char*)name);
        throw std::runtime_error(str);
        return;
    }
    inUnitSpec->mCalcRate = readInt8(buffer);

    inUnitSpec->mNumInputs = readInt16_be(buffer);
    inUnitSpec->mNumOutputs = readInt16_be(buffer);
    inUnitSpec->mSpecialIndex = readInt16_be(buffer);
    inUnitSpec->mInputSpec = (InputSpec*)malloc(sizeof(InputSpec) * inUnitSpec->mNumInputs);
    inUnitSpec->mOutputSpec = (OutputSpec*)malloc(sizeof(OutputSpec) * inUnitSpec->mNumOutputs);
    for (uint32 i = 0; i < inUnitSpec->mNumInputs; ++i) {
        InputSpec_ReadVer1(inUnitSpec->mInputSpec + i, buffer);
    }
    for (uint32 i = 0; i < inUnitSpec->mNumOutputs; ++i) {
        OutputSpec_Read(inUnitSpec->mOutputSpec + i, buffer);
    }
    uint64 numPorts = inUnitSpec->mNumInputs + inUnitSpec->mNumOutputs;
    inUnitSpec->mAllocSize = inUnitSpec->mUnitDef->mAllocSize + numPorts * (sizeof(Wire*) + sizeof(float*));
}

GraphDef* GraphDef_Read(World* inWorld, char*& buffer, GraphDef* inList, int32 inVersion);
GraphDef* GraphDef_ReadVer1(World* inWorld, char*& buffer, GraphDef* inList, int32 inVersion);

GraphDef* GraphDefLib_Read(World* inWorld, char* buffer, GraphDef* inList);
GraphDef* GraphDefLib_Read(World* inWorld, char* buffer, GraphDef* inList) {
    int32 magic = readInt32_be(buffer);
    if (magic != (('S' << 24) | ('C' << 16) | ('g' << 8) | 'f') /*'SCgf'*/)
        return inList;

    int32 version = readInt32_be(buffer);

    uint32 numDefs, i;
    switch (version) {
    case 2:
        numDefs = readInt16_be(buffer);

        for (i = 0; i < numDefs; ++i) {
            inList = GraphDef_Read(inWorld, buffer, inList, version);
        }
        return inList;
        break;
    case 1:
    case 0:
        numDefs = readInt16_be(buffer);

        for (i = 0; i < numDefs; ++i) {
            inList = GraphDef_ReadVer1(inWorld, buffer, inList, version); // handles 1 and 0
        }
        return inList;
        break;

    default:
        return inList;
        break;
    }
}


void ChooseMulAddFunc(GraphDef* graphDef, UnitSpec* unitSpec);
void DoBufferColoring(World* inWorld, GraphDef* inGraphDef);

void GraphDef_ReadVariant(World* inWorld, char*& buffer, GraphDef* inGraphDef, GraphDef* inVariant) {
    memcpy(inVariant, inGraphDef, sizeof(GraphDef));

    inVariant->mNumVariants = 0;
    inVariant->mVariants = 0;

    ReadName(buffer, inVariant->mNodeDef.mName);
    inVariant->mNodeDef.mHash = Hash(inVariant->mNodeDef.mName);

    inVariant->mInitialControlValues = (float32*)malloc(sizeof(float32) * inGraphDef->mNumControls);
    for (uint32 i = 0; i < inGraphDef->mNumControls; ++i) {
        inVariant->mInitialControlValues[i] = readFloat_be(buffer);
    }
}


typedef struct IndexMap {
    uint32 index;
    uint32 paramSpecIndex;
} IndexMap;

static inline bool sortIndexMaps(IndexMap map1, IndexMap map2) { return map1.paramSpecIndex < map2.paramSpecIndex; }

// ver 2
inline static void calcParamSpecs(GraphDef* graphDef, char*& buffer) {
    if (graphDef->mNumParamSpecs) {
        int hashTableSize = NEXTPOWEROFTWO(graphDef->mNumParamSpecs);
        graphDef->mParamSpecTable = new ParamSpecTable(&gMalloc, hashTableSize, false);
        uint32 nSpecs = graphDef->mNumParamSpecs;
        graphDef->mParamSpecs = (ParamSpec*)malloc(nSpecs * sizeof(ParamSpec));
        IndexMap* tempMaps = (IndexMap*)malloc(nSpecs * sizeof(IndexMap));

        for (uint32 i = 0; i < nSpecs; ++i) {
            ParamSpec* paramSpec = graphDef->mParamSpecs + i;
            ParamSpec_Read(paramSpec, buffer);
            graphDef->mParamSpecTable->Add(paramSpec);
            IndexMap* tempMap = tempMaps + i;
            tempMap->index = i;
            tempMap->paramSpecIndex = paramSpec->mIndex;
        }
        // calculate numChannels for each spec
        // printf("\n\n**************\n");
        std::sort(tempMaps, tempMaps + nSpecs, sortIndexMaps);
        for (uint32 i = 0; i < (nSpecs - 1); ++i) {
            IndexMap* tempMap = tempMaps + i;
            IndexMap* nextTempMap = tempMap + 1;
            ParamSpec* paramSpec = graphDef->mParamSpecs + tempMap->index;
            paramSpec->mNumChannels = nextTempMap->paramSpecIndex - tempMap->paramSpecIndex;
            // printf("%s: numChannels = %i\n", paramSpec->mName, paramSpec->mNumChannels);
        }

        IndexMap* tempMap = tempMaps + nSpecs - 1;
        ParamSpec* paramSpec = graphDef->mParamSpecs + tempMap->index;
        paramSpec->mNumChannels = graphDef->mNumControls - tempMap->paramSpecIndex;

        // printf("%s: numChannels = %i\n", paramSpec->mName, paramSpec->mNumChannels, paramSpec->mIndex);

        free(tempMaps);
    } else {
        // empty table to eliminate test in Graph_SetControl
        graphDef->mParamSpecTable = new ParamSpecTable(&gMalloc, 4, false);
        graphDef->mParamSpecs = 0;
    }
}

// ver 1

inline static void calcParamSpecs1(GraphDef* graphDef, char*& buffer) {
    if (graphDef->mNumParamSpecs) {
        int hashTableSize = NEXTPOWEROFTWO(graphDef->mNumParamSpecs);
        graphDef->mParamSpecTable = new ParamSpecTable(&gMalloc, hashTableSize, false);
        uint32 nSpecs = graphDef->mNumParamSpecs;
        graphDef->mParamSpecs = (ParamSpec*)malloc(nSpecs * sizeof(ParamSpec));
        IndexMap* tempMaps = (IndexMap*)malloc(nSpecs * sizeof(IndexMap));

        for (uint32 i = 0; i < nSpecs; ++i) {
            ParamSpec* paramSpec = graphDef->mParamSpecs + i;
            ParamSpec_ReadVer1(paramSpec, buffer); // read version 1 (the only difference to ver 2).
            graphDef->mParamSpecTable->Add(paramSpec);
            IndexMap* tempMap = tempMaps + i;
            tempMap->index = i;
            tempMap->paramSpecIndex = paramSpec->mIndex;
        }
        // calculate numChannels for each spec
        // printf("\n\n**************\n");
        std::sort(tempMaps, tempMaps + nSpecs, sortIndexMaps);
        for (uint32 i = 0; i < (nSpecs - 1); ++i) {
            IndexMap* tempMap = tempMaps + i;
            IndexMap* nextTempMap = tempMap + 1;
            ParamSpec* paramSpec = graphDef->mParamSpecs + tempMap->index;
            paramSpec->mNumChannels = nextTempMap->paramSpecIndex - tempMap->paramSpecIndex;
            // printf("%s: numChannels = %i\n", paramSpec->mName, paramSpec->mNumChannels);
        }

        IndexMap* tempMap = tempMaps + nSpecs - 1;
        ParamSpec* paramSpec = graphDef->mParamSpecs + tempMap->index;
        paramSpec->mNumChannels = graphDef->mNumControls - tempMap->paramSpecIndex;

        // printf("%s: numChannels = %i\n", paramSpec->mName, paramSpec->mNumChannels, paramSpec->mIndex);

        free(tempMaps);
    } else {
        // empty table to eliminate test in Graph_SetControl
        graphDef->mParamSpecTable = new ParamSpecTable(&gMalloc, 4, false);
        graphDef->mParamSpecs = 0;
    }
}

// Allocation code shared between v1 and v2.
// See https://github.com/supercollider/supercollider/issues/3266
// TODO: refactor GraphDef to be less brittle.
static void GraphDef_SetAllocSizes(GraphDef* graphDef) {
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

    graphDef->mMapControlRatesAllocSize = graphDef->mNumControls * sizeof(int);
    graphDef->mNodeDef.mAllocSize += graphDef->mMapControlRatesAllocSize;

    graphDef->mAudioMapBusOffsetSize = graphDef->mNumControls * sizeof(int32);
    graphDef->mNodeDef.mAllocSize += graphDef->mAudioMapBusOffsetSize;
}

// ver 2
/** \note Relevant supernova code: \c sc_synthdef::prepare()
 * \note Relevant v1 code: \c GraphDef_ReadVer1()
 */
GraphDef* GraphDef_Read(World* inWorld, char*& buffer, GraphDef* inList, int32 inVersion) {
    int32 name[kSCNodeDefNameLen];
    ReadNodeDefName(buffer, name);

    GraphDef* graphDef = (GraphDef*)calloc(1, sizeof(GraphDef));

    graphDef->mOriginal = graphDef;

    graphDef->mNodeDef.mAllocSize = sizeof(Graph);

    memcpy((char*)graphDef->mNodeDef.mName, (char*)name, kSCNodeDefNameByteLen);

    graphDef->mNodeDef.mHash = Hash(graphDef->mNodeDef.mName);

    graphDef->mNumConstants = readInt32_be(buffer);

    graphDef->mConstants = (float*)malloc(graphDef->mNumConstants * sizeof(float));
    for (uint32 i = 0; i < graphDef->mNumConstants; ++i) {
        graphDef->mConstants[i] = readFloat_be(buffer);
    }

    graphDef->mNumControls = readInt32_be(buffer);
    graphDef->mInitialControlValues = (float32*)malloc(sizeof(float32) * graphDef->mNumControls);
    for (uint32 i = 0; i < graphDef->mNumControls; ++i) {
        graphDef->mInitialControlValues[i] = readFloat_be(buffer);
    }

    graphDef->mNumParamSpecs = readInt32_be(buffer);

    calcParamSpecs(graphDef, buffer);

    graphDef->mNumWires = graphDef->mNumConstants;
    graphDef->mNumUnitSpecs = readInt32_be(buffer);
    graphDef->mUnitSpecs = (UnitSpec*)malloc(sizeof(UnitSpec) * graphDef->mNumUnitSpecs);
    graphDef->mNumCalcUnits = 0;
    for (uint32 i = 0; i < graphDef->mNumUnitSpecs; ++i) {
        UnitSpec* unitSpec = graphDef->mUnitSpecs + i;
        UnitSpec_Read(unitSpec, buffer);

        switch (unitSpec->mCalcRate) {
        case calc_ScalarRate:
            unitSpec->mRateInfo = &inWorld->mBufRate;
            break;
        case calc_BufRate:
            graphDef->mNumCalcUnits++;
            unitSpec->mRateInfo = &inWorld->mBufRate;
            break;
        case calc_FullRate:
            graphDef->mNumCalcUnits++;
            unitSpec->mRateInfo = &inWorld->mFullRate;
            break;
        case calc_DemandRate:
            unitSpec->mRateInfo = &inWorld->mBufRate;
            break;
        }

        graphDef->mNodeDef.mAllocSize += unitSpec->mAllocSize;
        graphDef->mNumWires += unitSpec->mNumOutputs;
    }

    DoBufferColoring(inWorld, graphDef);

    GraphDef_SetAllocSizes(graphDef);

    graphDef->mNext = inList;
    graphDef->mRefCount = 1;

    graphDef->mNumVariants = readInt16_be(buffer);
    if (graphDef->mNumVariants) {
        graphDef->mVariants = (GraphDef*)calloc(graphDef->mNumVariants, sizeof(GraphDef));
        for (uint32 i = 0; i < graphDef->mNumVariants; ++i) {
            GraphDef_ReadVariant(inWorld, buffer, graphDef, graphDef->mVariants + i);
        }
    }

    return graphDef;
}

// ver 0 or 1
GraphDef* GraphDef_ReadVer1(World* inWorld, char*& buffer, GraphDef* inList, int32 inVersion) {
    int32 name[kSCNodeDefNameLen];
    ReadNodeDefName(buffer, name);

    GraphDef* graphDef = (GraphDef*)calloc(1, sizeof(GraphDef));

    graphDef->mOriginal = graphDef;

    graphDef->mNodeDef.mAllocSize = sizeof(Graph);

    memcpy((char*)graphDef->mNodeDef.mName, (char*)name, kSCNodeDefNameByteLen);

    graphDef->mNodeDef.mHash = Hash(graphDef->mNodeDef.mName);

    graphDef->mNumConstants = readInt16_be(buffer);
    graphDef->mConstants = (float*)malloc(graphDef->mNumConstants * sizeof(float));
    for (uint32 i = 0; i < graphDef->mNumConstants; ++i) {
        graphDef->mConstants[i] = readFloat_be(buffer);
    }

    graphDef->mNumControls = readInt16_be(buffer);
    graphDef->mInitialControlValues = (float32*)malloc(sizeof(float32) * graphDef->mNumControls);
    for (uint32 i = 0; i < graphDef->mNumControls; ++i) {
        graphDef->mInitialControlValues[i] = readFloat_be(buffer);
    }

    graphDef->mNumParamSpecs = readInt16_be(buffer);
    calcParamSpecs1(graphDef, buffer);

    graphDef->mNumWires = graphDef->mNumConstants;
    graphDef->mNumUnitSpecs = readInt16_be(buffer);
    graphDef->mUnitSpecs = (UnitSpec*)malloc(sizeof(UnitSpec) * graphDef->mNumUnitSpecs);
    graphDef->mNumCalcUnits = 0;
    for (uint32 i = 0; i < graphDef->mNumUnitSpecs; ++i) {
        UnitSpec* unitSpec = graphDef->mUnitSpecs + i;
        UnitSpec_ReadVer1(unitSpec, buffer);

        switch (unitSpec->mCalcRate) {
        case calc_ScalarRate:
            unitSpec->mRateInfo = &inWorld->mBufRate;
            break;
        case calc_BufRate:
            graphDef->mNumCalcUnits++;
            unitSpec->mRateInfo = &inWorld->mBufRate;
            break;
        case calc_FullRate:
            graphDef->mNumCalcUnits++;
            unitSpec->mRateInfo = &inWorld->mFullRate;
            break;
        case calc_DemandRate:
            unitSpec->mRateInfo = &inWorld->mBufRate;
            break;
        }

        graphDef->mNodeDef.mAllocSize += unitSpec->mAllocSize;
        graphDef->mNumWires += unitSpec->mNumOutputs;
    }

    DoBufferColoring(inWorld, graphDef);

    GraphDef_SetAllocSizes(graphDef);

    graphDef->mNext = inList;
    graphDef->mRefCount = 1;

    if (inVersion >= 1) {
        graphDef->mNumVariants = readInt16_be(buffer);
        if (graphDef->mNumVariants) {
            graphDef->mVariants = (GraphDef*)calloc(graphDef->mNumVariants, sizeof(GraphDef));
            for (uint32 i = 0; i < graphDef->mNumVariants; ++i) {
                GraphDef_ReadVariant(inWorld, buffer, graphDef, graphDef->mVariants + i);
            }
        }
    }

    return graphDef;
}


void GraphDef_Define(World* inWorld, GraphDef* inList) {
    GraphDef* graphDef = inList;
    while (graphDef) {
        GraphDef* next = graphDef->mNext;

        GraphDef* previousDef = World_GetGraphDef(inWorld, graphDef->mNodeDef.mName);
        if (previousDef) {
            World_RemoveGraphDef(inWorld, previousDef);
            if (--previousDef->mRefCount == 0) {
                GraphDef_DeleteMsg(inWorld, previousDef);
            }
        }
        World_AddGraphDef(inWorld, graphDef);
        graphDef->mNext = 0;
        graphDef = next;
    }
}

SCErr GraphDef_Remove(World* inWorld, int32* inName) {
    GraphDef* graphDef = World_GetGraphDef(inWorld, inName);
    if (graphDef) {
        World_RemoveGraphDef(inWorld, graphDef);
        if (--graphDef->mRefCount == 0) {
            return GraphDef_DeleteMsg(inWorld, graphDef);
        }
    }
    return kSCErr_None;
}

SCErr SendReplyCmd_d_removed(World* inWorld, int inSize, char* inData, ReplyAddress* inReply) {
    void* space = World_Alloc(inWorld, sizeof(SendReplyCmd));
    SendReplyCmd* cmd = new (space) SendReplyCmd(inWorld, inReply);
    if (!cmd)
        return kSCErr_Failed;
    int err = cmd->Init(inData, inSize);
    if (err) {
        cmd->~SendReplyCmd();
        World_Free(inWorld, space);
        return err;
    }
    if (inWorld->mRealTime)
        cmd->CallNextStage();
    else
        cmd->CallEveryStage();
    return kSCErr_None;
}

SCErr GraphDef_DeleteMsg(World* inWorld, GraphDef* inDef) {
    DeleteGraphDefMsg msg;
    msg.mDef = inDef;
    inWorld->hw->mDeleteGraphDefs.Write(msg);

    small_scpacket packet;
    packet.adds("/d_removed");
    packet.maketags(2);
    packet.addtag(',');
    packet.addtag('s');
    packet.adds((char*)inDef->mNodeDef.mName);

    for (auto addr : *inWorld->hw->mUsers) {
        SCErr const err = SendReplyCmd_d_removed(inWorld, packet.size(), packet.data(), &addr);
        if (err != kSCErr_None)
            return err;
    }

    return kSCErr_None;
}

GraphDef* GraphDef_Recv(World* inWorld, char* buffer, GraphDef* inList) {
    try {
        inList = GraphDefLib_Read(inWorld, buffer, inList);
    } catch (std::exception& exc) {
        scprintf("exception in GraphDef_Recv: %s\n", exc.what());
    } catch (...) {
        scprintf("unknown exception in GraphDef_Recv\n");
    }

    return inList;
}

GraphDef* GraphDef_LoadGlob(World* inWorld, const char* pattern, GraphDef* inList) {
    SC_Filesystem::Glob* glob = SC_Filesystem::makeGlob(pattern);
    if (!glob)
        return inList;

    bfs::path path;
    while (!(path = SC_Filesystem::globNext(glob)).empty()) {
        if (path.extension() == ".scsyndef") {
            inList = GraphDef_Load(inWorld, path, inList);
        }
        // why? <sk>
        GraphDef_Load(inWorld, path, inList);
    }

    SC_Filesystem::freeGlob(glob);
    return inList;
}

GraphDef* GraphDef_Load(World* inWorld, const bfs::path& path, GraphDef* inList) {
    try {
        std::string file_contents;
        bfs::load_string_file(path, file_contents);
        inList = GraphDefLib_Read(inWorld, &file_contents[0], inList);
    } catch (const std::exception& e) {
        scprintf("exception in GraphDef_Load: %s\n", e.what());
        const std::string path_utf8_str = SC_Codecvt::path_to_utf8_str(path);
        scprintf("while reading file: '%s'", path_utf8_str.c_str());
    } catch (...) {
        scprintf("unknown exception in GrafDef_Load\n");
        const std::string path_utf8_str = SC_Codecvt::path_to_utf8_str(path);
        scprintf("while reading file '%s'\n", path_utf8_str.c_str());
    }

    return inList;
}

GraphDef* GraphDef_LoadDir(World* inWorld, const bfs::path& dirname, GraphDef* inList) {
    boost::system::error_code ec;
    bfs::recursive_directory_iterator rditer(dirname, bfs::symlink_option::recurse, ec);

    if (ec) {
        scprintf("*** ERROR: open directory failed '%s'\n", SC_Codecvt::path_to_utf8_str(dirname).c_str());
        return inList;
    }

    while (rditer != bfs::end(rditer)) {
        const bfs::path path = *rditer;

        if (bfs::is_directory(path)) {
            if (SC_Filesystem::instance().shouldNotCompileDirectory(path))
                rditer.no_push();
            else
                ; // do nothing; recursion will happen automatically
        } else if (path.extension() == ".scsyndef") { // ordinary file
            inList = GraphDef_Load(inWorld, path, inList);
        } else {
            // ignore file, wasn't a synth def
        }

        rditer.increment(ec);
        if (ec) {
            scprintf("*** ERROR: Could not iterate on '%s': %s\n", SC_Codecvt::path_to_utf8_str(path).c_str(),
                     ec.message().c_str());
            return inList;
        }
    }

    return inList;
}

void UnitSpec_Free(UnitSpec* inUnitSpec);
void UnitSpec_Free(UnitSpec* inUnitSpec) {
    free(inUnitSpec->mInputSpec);
    free(inUnitSpec->mOutputSpec);
}

void GraphDef_Free(GraphDef* inGraphDef) {
    if (inGraphDef != inGraphDef->mOriginal)
        return;

    for (uint32 i = 0; i < inGraphDef->mNumUnitSpecs; ++i) {
        UnitSpec_Free(inGraphDef->mUnitSpecs + i);
    }
    for (uint32 i = 0; i < inGraphDef->mNumVariants; ++i) {
        free(inGraphDef->mVariants[i].mInitialControlValues);
    }
    delete inGraphDef->mParamSpecTable;
    free(inGraphDef->mParamSpecs);
    free(inGraphDef->mInitialControlValues);
    free(inGraphDef->mConstants);
    free(inGraphDef->mUnitSpecs);
    free(inGraphDef->mVariants);
    free(inGraphDef);
}

void NodeDef_Dump(NodeDef* inNodeDef) {
    scprintf("mName '%s'\n", (char*)inNodeDef->mName);
    scprintf("mHash %d\n", inNodeDef->mHash);
    scprintf("mAllocSize %lu\n", inNodeDef->mAllocSize);
}

void GraphDef_Dump(GraphDef* inGraphDef) {
    NodeDef_Dump(&inGraphDef->mNodeDef);

    scprintf("mNumControls %d\n", inGraphDef->mNumControls);
    scprintf("mNumWires %d\n", inGraphDef->mNumWires);
    scprintf("mNumUnitSpecs %d\n", inGraphDef->mNumUnitSpecs);
    scprintf("mNumWireBufs %d\n", inGraphDef->mNumWireBufs);

    for (uint32 i = 0; i < inGraphDef->mNumControls; ++i) {
        scprintf("   %d mInitialControlValues %g\n", i, inGraphDef->mInitialControlValues[i]);
    }

    for (uint32 i = 0; i < inGraphDef->mNumWires; ++i) {
        // WireSpec_Dump(inGraphDef->mWireSpec + i);
    }
    for (uint32 i = 0; i < inGraphDef->mNumUnitSpecs; ++i) {
        // UnitSpec_Dump(inGraphDef->mUnitSpecs + i);
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

struct BufColorAllocator {
    int16* refs;
    int16* stack;
    int16 stackPtr;
    int16 nextIndex;
    int16 refsMaxSize;
    int16 stackMaxSize;

    BufColorAllocator();
    ~BufColorAllocator();

    uint32 alloc(uint32 count);
    bool release(int inIndex);
    int NumBufs() { return nextIndex; }
};

inline BufColorAllocator::BufColorAllocator() {
    refsMaxSize = 32;
    stackMaxSize = 32;
    refs = (int16*)calloc(refsMaxSize, sizeof(int16));
    stack = (int16*)calloc(stackMaxSize, sizeof(int16));
    stackPtr = 0;
    nextIndex = 0;
}

inline BufColorAllocator::~BufColorAllocator() {
    free(refs);
    free(stack);
}

inline uint32 BufColorAllocator::alloc(uint32 count) {
    uint32 outIndex;
    if (stackPtr) {
        outIndex = stack[--stackPtr];
    } else {
        outIndex = nextIndex++;
    }
    if (outIndex >= refsMaxSize) {
        int16* tmprefs = (int16*)realloc(refs, refsMaxSize * 2 * sizeof(int16));
        if (tmprefs == NULL) {
            free(refs);
            refs = NULL;
            throw std::runtime_error("buffer coloring error: reallocation failed.");
        } else {
            refs = tmprefs;
        }
        memset(refs + refsMaxSize, 0, refsMaxSize * sizeof(int16));
        refsMaxSize *= 2;
    }
    refs[outIndex] = count;
    return outIndex;
}

inline bool BufColorAllocator::release(int inIndex) {
    if (refs[inIndex] == 0)
        return false;
    if (--refs[inIndex] == 0) {
        if (stackPtr >= stackMaxSize) {
            int16* tmpstack = (int16*)realloc(stack, stackMaxSize * 2 * sizeof(int16));
            if (tmpstack == NULL) {
                free(stack);
                stack = NULL;
                throw std::runtime_error("buffer coloring error: reallocation during release failed.");
            } else {
                stack = tmpstack;
            }
            memset(stack + stackMaxSize, 0, stackMaxSize * sizeof(int16));
            stackMaxSize *= 2;
        }
        stack[stackPtr++] = inIndex;
    }
    return true;
}

static void ReleaseInputBuffers(GraphDef* inGraphDef, UnitSpec* unitSpec, BufColorAllocator& bufColor) {
    for (int64 i = (int64)(unitSpec->mNumInputs) - 1; i >= 0; --i) {
        InputSpec* inputSpec = unitSpec->mInputSpec + i;
        if (inputSpec->mFromUnitIndex >= 0) {
            UnitSpec* outUnit = inGraphDef->mUnitSpecs + inputSpec->mFromUnitIndex;
            OutputSpec* outputSpec = outUnit->mOutputSpec + inputSpec->mFromOutputIndex;
            inputSpec->mWireIndex = outputSpec->mWireIndex;
            if (outputSpec->mCalcRate == calc_FullRate) {
                if (unitSpec->mCalcRate == calc_DemandRate)
                    // we never release any input buffers of demand-rate ugens
                    continue;

                if (!bufColor.release(outputSpec->mBufferIndex)) {
                    scprintf("buffer coloring error: tried to release output with zero count\n");
                    scprintf("output: %d %s %d\n", inputSpec->mFromUnitIndex, outUnit->mUnitDef->mUnitDefName,
                             inputSpec->mFromOutputIndex);
                    scprintf("input: %s %d\n", unitSpec->mUnitDef->mUnitDefName, i);
                    throw std::runtime_error("buffer coloring error.");
                }
            }
        } else {
            inputSpec->mWireIndex = inputSpec->mFromOutputIndex;
        }
    }
}

static void AllocOutputBuffers(UnitSpec* unitSpec, BufColorAllocator& bufColor, int32& wireIndexCtr) {
    // scprintf("AllocOutputBuffers %s numoutputs %d\n", unitSpec->mUnitDef->mUnitDefName, unitSpec->mNumOutputs);
    for (uint32 i = 0; i < unitSpec->mNumOutputs; ++i) {
        OutputSpec* outputSpec = unitSpec->mOutputSpec + i;
        outputSpec->mWireIndex = wireIndexCtr++;
        if (outputSpec->mCalcRate == calc_FullRate) {
            uint32 bufIndex = bufColor.alloc(outputSpec->mNumConsumers);
            outputSpec->mBufferIndex = bufIndex;
        }
    }
}

void DoBufferColoring(World* inWorld, GraphDef* inGraphDef) {
    // count consumers of outputs
    for (uint32 j = 0; j < inGraphDef->mNumUnitSpecs; ++j) {
        UnitSpec* unitSpec = inGraphDef->mUnitSpecs + j;
        for (uint32 i = 0; i < unitSpec->mNumInputs; ++i) {
            InputSpec* inputSpec = unitSpec->mInputSpec + i;
            if (inputSpec->mFromUnitIndex >= 0) {
                UnitSpec* outUnit = inGraphDef->mUnitSpecs + inputSpec->mFromUnitIndex;
                OutputSpec* outputSpec = outUnit->mOutputSpec + inputSpec->mFromOutputIndex;
                outputSpec->mNumConsumers++;
            }
        }
    }

    // buffer coloring
    {
        BufColorAllocator bufColor;
        int32 wireIndexCtr = inGraphDef->mNumConstants; // mNumConstants is a uint32, but limited to int32 in OSC
        for (uint32 j = 0; j < inGraphDef->mNumUnitSpecs; ++j) {
            UnitSpec* unitSpec = inGraphDef->mUnitSpecs + j;
            if (unitSpec->mUnitDef->mFlags & kUnitDef_CantAliasInputsToOutputs) {
                // set wire index, alloc outputs
                AllocOutputBuffers(unitSpec, bufColor, wireIndexCtr);
                // set wire index, release inputs
                ReleaseInputBuffers(inGraphDef, unitSpec, bufColor);
            } else {
                // set wire index, release inputs
                ReleaseInputBuffers(inGraphDef, unitSpec, bufColor);
                // set wire index, alloc outputs
                AllocOutputBuffers(unitSpec, bufColor, wireIndexCtr);
            }
        }

        inGraphDef->mNumWireBufs = bufColor.NumBufs();
        if (inWorld->mRunning) {
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
    for (uint32 j = 0; j < inGraphDef->mNumUnitSpecs; ++j) {
        UnitSpec* unitSpec = inGraphDef->mUnitSpecs + j;
        for (uint32 i = 0; i < unitSpec->mNumOutputs; ++i) {
            OutputSpec* outputSpec = unitSpec->mOutputSpec + i;
            if (outputSpec->mCalcRate == calc_FullRate) {
                outputSpec->mBufferIndex *= bufLength;
            }
        }
    }
}
