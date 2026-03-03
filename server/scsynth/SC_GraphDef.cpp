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

#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

extern Malloc gMalloc;

const size_t ERR_BUF_SIZE(256);

int32 GetHash(ParamSpec* inParamSpec) { return inParamSpec->mHash; }

int32* GetKey(ParamSpec* inParamSpec) { return inParamSpec->mName; }

// this is used for reading count fields that have been changed from int16 to int32 in SynthDef v2.
inline int32 readCount(const char*& buffer, const char* end, int version) {
    if (version >= 2)
        return readInt32_be(buffer, end);
    else
        return readInt16_be(buffer, end);
}

void ReadName(const char*& buffer, const char* end, int32* name) {
    uint32 namelen = readUInt8(buffer, end);
    if (namelen >= kSCNameByteLen) {
        std::ostringstream os;
        os << "name too long (> " << kSCNameByteLen - 1 << " chars): " << std::string(buffer, namelen);
        throw std::runtime_error(os.str());
    }
    memset(name, 0, kSCNameByteLen);
    readData(buffer, end, (char*)name, namelen);
}

void ReadNodeDefName(const char*& buffer, const char* end, int32* name) {
    uint32 namelen = readUInt8(buffer, end);
    if (namelen >= kSCNodeDefNameByteLen) {
        std::ostringstream os;
        os << "node definition name too long (> " << kSCNodeDefNameByteLen - 1
           << " chars): " << std::string(buffer, namelen);
        throw std::runtime_error(os.str());
    }
    memset(name, 0, kSCNodeDefNameByteLen);
    readData(buffer, end, (char*)name, namelen);
}

void ParamSpec_Read(ParamSpec* inParamSpec, const char*& buffer, const char* end, int version) {
    ReadName(buffer, end, inParamSpec->mName);
    inParamSpec->mIndex = readCount(buffer, end, version);
    inParamSpec->mHash = Hash(inParamSpec->mName);
}

void InputSpec_Read(InputSpec* inInputSpec, const char*& buffer, const char* end, int version) {
    inInputSpec->mFromUnitIndex = readCount(buffer, end, version);
    inInputSpec->mFromOutputIndex = readCount(buffer, end, version);

    inInputSpec->mWireIndex = -1;
}

void OutputSpec_Read(OutputSpec* inOutputSpec, const char*& buffer, const char* end) {
    inOutputSpec->mCalcRate = readInt8(buffer, end);
    inOutputSpec->mWireIndex = -1;
    inOutputSpec->mBufferIndex = -1;
    inOutputSpec->mNumConsumers = 0;
}

void UnitSpec_Read(UnitSpec* inUnitSpec, const char*& buffer, const char* end, int version) {
    int32 name[kSCNameLen];
    ReadName(buffer, end, name);

    inUnitSpec->mUnitDef = GetUnitDef(name);
    if (!inUnitSpec->mUnitDef) {
        char str[ERR_BUF_SIZE];
        snprintf(str, ERR_BUF_SIZE, "UGen '%s' not installed.", (char*)name);
        throw std::runtime_error(str);
    }
    inUnitSpec->mCalcRate = readInt8(buffer, end);

    inUnitSpec->mNumInputs = readCount(buffer, end, version);
    inUnitSpec->mNumOutputs = readCount(buffer, end, version);
    inUnitSpec->mSpecialIndex = readInt16_be(buffer, end);
    inUnitSpec->mInputSpec = new InputSpec[inUnitSpec->mNumInputs];
    inUnitSpec->mOutputSpec = new OutputSpec[inUnitSpec->mNumOutputs];
    for (uint32 i = 0; i < inUnitSpec->mNumInputs; ++i) {
        InputSpec_Read(inUnitSpec->mInputSpec + i, buffer, end, version);
    }
    for (uint32 i = 0; i < inUnitSpec->mNumOutputs; ++i) {
        OutputSpec_Read(inUnitSpec->mOutputSpec + i, buffer, end);
    }
    uint64 numPorts = inUnitSpec->mNumInputs + inUnitSpec->mNumOutputs;
    inUnitSpec->mAllocSize = inUnitSpec->mUnitDef->mAllocSize + numPorts * (sizeof(Wire*) + sizeof(float*));
}

GraphDef* GraphDef_Read(World* inWorld, const char*& buffer, const char* end, GraphDef* inList, int32 inVersion);

GraphDef* GraphDefLib_Read(World* inWorld, const char* buffer, size_t size, GraphDef* inList) {
    const char* end = buffer + size;

    // check header ('SCgf')
    int32 magic = readInt32_be(buffer, end);
    if (magic != (('S' << 24) | ('C' << 16) | ('g' << 8) | 'f'))
        throw std::runtime_error("not a synthdef");

    int32 version = readInt32_be(buffer, end);
    if (version > 3)
        throw std::runtime_error("version " + std::to_string(version) + " not supported");

    uint32 numDefs = readInt16_be(buffer, end);
    if (version > 2) {
        // in version 3, every synth definition starts with a size field (int32) that tells
        // the size of the entire definition in bytes (including the size field itself).
        // NB: GraphDef_Read() might not read all the fields so we must explicitly set
        // the begin and end on each iteration!
        for (int i = 0; i < numDefs; ++i) {
            size_t synthDefSize = readInt32_be(buffer, end);
            const char* synthDefEnd = buffer + synthDefSize - 4;
            if (synthDefEnd > end)
                throw std::runtime_error("wrong synthdef size");

            inList = GraphDef_Read(inWorld, buffer, synthDefEnd, inList, version);

            buffer = synthDefEnd;
        }
    } else {
        for (int i = 0; i < numDefs; ++i) {
            inList = GraphDef_Read(inWorld, buffer, end, inList, version);
        }
    }

    return inList;
}


void ChooseMulAddFunc(GraphDef* graphDef, UnitSpec* unitSpec);
void DoBufferColoring(World* inWorld, GraphDef* inGraphDef);

void GraphDef_ReadVariant(World* inWorld, const char*& buffer, const char* end, GraphDef* inGraphDef,
                          GraphDef* inVariant) {
    memcpy(inVariant, inGraphDef, sizeof(GraphDef));

    inVariant->mNumVariants = 0;
    inVariant->mVariants = nullptr;

    ReadName(buffer, end, inVariant->mNodeDef.mName);
    inVariant->mNodeDef.mHash = Hash(inVariant->mNodeDef.mName);

    inVariant->mInitialControlValues = new float32[inGraphDef->mNumControls];
    for (uint32 i = 0; i < inGraphDef->mNumControls; ++i) {
        inVariant->mInitialControlValues[i] = readFloat_be(buffer, end);
    }
}


typedef struct IndexMap {
    uint32 index;
    uint32 paramSpecIndex;
} IndexMap;

inline static void calcParamSpecs(GraphDef* graphDef, const char*& buffer, const char* end, int version) {
    uint32 numSpecs = graphDef->mNumParamSpecs;
    if (numSpecs > 0) {
        int hashTableSize = NEXTPOWEROFTWO(numSpecs);
        graphDef->mParamSpecTable = new ParamSpecTable(&gMalloc, hashTableSize, false);
        graphDef->mParamSpecs = new ParamSpec[numSpecs];
        std::vector<IndexMap> tempMaps(numSpecs);

        for (uint32 i = 0; i < numSpecs; ++i) {
            ParamSpec* paramSpec = &graphDef->mParamSpecs[i];
            ParamSpec_Read(paramSpec, buffer, end, version);
            graphDef->mParamSpecTable->Add(paramSpec);
            tempMaps[i].index = i;
            tempMaps[i].paramSpecIndex = paramSpec->mIndex;
        }
        // calculate numChannels for each spec
        // printf("\n\n**************\n");
        std::sort(tempMaps.begin(), tempMaps.end(),
                  [](const auto& a, const auto& b) { return a.paramSpecIndex < b.paramSpecIndex; });
        for (uint32 i = 0; i < (numSpecs - 1); ++i) {
            const auto& tempMap = tempMaps[i];
            const auto& nextTempMap = tempMaps[i + 1];
            auto& paramSpec = graphDef->mParamSpecs[tempMap.index];
            paramSpec.mNumChannels = nextTempMap.paramSpecIndex - tempMap.paramSpecIndex;
            // printf("%s: numChannels = %i\n", paramSpec.mName, paramSpec.mNumChannels);
        }

        // last spec
        const auto& tempMap = tempMaps.back();
        auto& paramSpec = graphDef->mParamSpecs[tempMap.index];
        paramSpec.mNumChannels = graphDef->mNumControls - tempMap.paramSpecIndex;

        // printf("%s: numChannels = %i\n", paramSpec.mName, paramSpec.mNumChannels, paramSpec.mIndex);
    } else {
        // empty table to eliminate test in Graph_SetControl
        graphDef->mParamSpecTable = new ParamSpecTable(&gMalloc, 4, false);
        graphDef->mParamSpecs = nullptr;
    }
}

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


/** \note Relevant supernova code: \c sc_synthdef::sc_synthdef() */
GraphDef* GraphDef_Read(World* inWorld, const char*& buffer, const char* end, GraphDef* inList, int32 inVersion) {
    int32 name[kSCNodeDefNameLen];
    ReadNodeDefName(buffer, end, name);

    // Use custom deleter for automatic cleanup in case an exception is thrown.
    auto deleter = [](GraphDef* def) { GraphDef_Free(def); };
    // value-initialize so that all members of GraphDef are initially set to zero!
    std::unique_ptr<GraphDef, decltype(deleter)> graphDef(new GraphDef {}, deleter);

    graphDef->mOriginal = graphDef.get();

    graphDef->mNodeDef.mAllocSize = sizeof(Graph);

    memcpy(graphDef->mNodeDef.mName, name, kSCNodeDefNameByteLen);

    graphDef->mNodeDef.mHash = Hash(graphDef->mNodeDef.mName);

    graphDef->mNumConstants = readCount(buffer, end, inVersion);
    graphDef->mConstants = new float32[graphDef->mNumConstants];
    for (uint32 i = 0; i < graphDef->mNumConstants; ++i) {
        graphDef->mConstants[i] = readFloat_be(buffer, end);
    }
    graphDef->mNumWires = graphDef->mNumConstants;

    graphDef->mNumControls = readCount(buffer, end, inVersion);
    graphDef->mInitialControlValues = new float32[graphDef->mNumControls];
    for (uint32 i = 0; i < graphDef->mNumControls; ++i) {
        graphDef->mInitialControlValues[i] = readFloat_be(buffer, end);
    }

    graphDef->mNumParamSpecs = readCount(buffer, end, inVersion);
    calcParamSpecs(graphDef.get(), buffer, end, inVersion);

    uint32 numUnitSpecs = readCount(buffer, end, inVersion);
    graphDef->mUnitSpecs = new UnitSpec[numUnitSpecs];
    graphDef->mNumCalcUnits = 0;
    graphDef->mNumUnitSpecs = 0;
    for (uint32 i = 0; i < numUnitSpecs; ++i) {
        UnitSpec* unitSpec = &graphDef->mUnitSpecs[i];
        UnitSpec_Read(unitSpec, buffer, end, inVersion);

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

        // increment mNumUnitSpecs after every successful call to UnitSpec_Read() so that we
        // can safely call GraphDef_Free() when we catch an exception.
        graphDef->mNumUnitSpecs++;
        graphDef->mNodeDef.mAllocSize += unitSpec->mAllocSize;
        graphDef->mNumWires += unitSpec->mNumOutputs;
    }

    DoBufferColoring(inWorld, graphDef.get());

    GraphDef_SetAllocSizes(graphDef.get());

    if (inVersion >= 1) {
        uint16 numVariants = readInt16_be(buffer, end);
        if (numVariants > 0) {
            graphDef->mVariants = new GraphDef[numVariants];
            for (uint32 i = 0; i < numVariants; ++i) {
                GraphDef_ReadVariant(inWorld, buffer, end, graphDef.get(), graphDef->mVariants + i);
                // increment mNumVariants after every successful call to GraphDef_ReadVariant()
                // so that we can safely call GraphDef_Free() when we catch an exception.
                graphDef->mNumVariants++;
            }
        }
    }

    // finally add to list
    graphDef->mNext = inList;
    graphDef->mRefCount = 1;

    return graphDef.release();
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
        graphDef->mNext = nullptr;
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
    if (!space)
        return kSCErr_OutOfRealTimeMemory;
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

GraphDef* GraphDef_Recv(World* inWorld, const char* buffer, size_t size, GraphDef* inList) {
    try {
        inList = GraphDefLib_Read(inWorld, buffer, size, inList);
    } catch (std::exception& exc) { scprintf("exception in GraphDef_Recv: %s\n", exc.what()); } catch (...) {
        scprintf("unknown exception in GraphDef_Recv\n");
    }

    return inList;
}

GraphDef* GraphDef_LoadGlob(World* inWorld, const char* pattern, GraphDef* inList) {
    SC_Filesystem::Glob* glob = SC_Filesystem::makeGlob(pattern);
    if (!glob)
        return inList;

    fs::path path;
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

std::string load_file(const std::filesystem::path& file_path) {
    std::ifstream file(file_path, std::ifstream::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + file_path.string());
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

GraphDef* GraphDef_Load(World* inWorld, const fs::path& path, GraphDef* inList) {
    try {
        std::string file_contents = load_file(path);
        inList = GraphDefLib_Read(inWorld, file_contents.data(), file_contents.size(), inList);
    } catch (const std::exception& e) {
        scprintf("exception in GraphDef_Load: %s\n", e.what());
        const std::string path_utf8_str = SC_Codecvt::path_to_utf8_str(path);
        scprintf("while reading file: '%s'\n", path_utf8_str.c_str());
    } catch (...) {
        scprintf("unknown exception in GrafDef_Load\n");
        const std::string path_utf8_str = SC_Codecvt::path_to_utf8_str(path);
        scprintf("while reading file '%s'\n", path_utf8_str.c_str());
    }

    return inList;
}

GraphDef* GraphDef_LoadDir(World* inWorld, const fs::path& dirname, GraphDef* inList) {
    std::error_code ec;
    fs::recursive_directory_iterator rditer(dirname, fs::directory_options::follow_directory_symlink, ec);

    if (ec) {
        scprintf("*** ERROR: open directory failed '%s'\n", SC_Codecvt::path_to_utf8_str(dirname).c_str());
        return inList;
    }

    while (rditer != fs::end(rditer)) {
        const fs::path path = *rditer;

        if (fs::is_directory(path)) {
            if (SC_Filesystem::instance().shouldNotCompileDirectory(path))
                rditer.disable_recursion_pending();
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
    delete[] inUnitSpec->mInputSpec;
    delete[] inUnitSpec->mOutputSpec;
}

void GraphDef_Free(GraphDef* inGraphDef) {
    if (inGraphDef != inGraphDef->mOriginal)
        return;

    for (uint32 i = 0; i < inGraphDef->mNumUnitSpecs; ++i) {
        UnitSpec_Free(inGraphDef->mUnitSpecs + i);
    }
    for (uint32 i = 0; i < inGraphDef->mNumVariants; ++i) {
        delete[] inGraphDef->mVariants[i].mInitialControlValues;
    }
    delete inGraphDef->mParamSpecTable;
    delete[] inGraphDef->mParamSpecs;
    delete[] inGraphDef->mInitialControlValues;
    delete[] inGraphDef->mConstants;
    delete[] inGraphDef->mUnitSpecs;
    delete[] inGraphDef->mVariants;
    delete inGraphDef;
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

class BufColorAllocator {
public:
    BufColorAllocator();

    uint32 alloc(uint32 count);
    bool release(int inIndex);
    int NumBufs() { return mRefs.size(); }

private:
    std::vector<int16> mRefs;
    std::vector<int16> mStack;
};

inline BufColorAllocator::BufColorAllocator() {
    mRefs.reserve(32);
    mStack.reserve(32);
}

inline uint32 BufColorAllocator::alloc(uint32 count) {
    uint32 outIndex;
    if (!mStack.empty()) {
        // pop index from stack
        outIndex = mStack.back();
        mStack.pop_back();
        mRefs[outIndex] = count;
    } else {
        // make new index
        outIndex = mRefs.size();
        mRefs.push_back(count);
    }
    return outIndex;
}

inline bool BufColorAllocator::release(int inIndex) {
    if (mRefs[inIndex] == 0)
        return false;
    if (--mRefs[inIndex] == 0) {
        mStack.push_back(inIndex);
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
