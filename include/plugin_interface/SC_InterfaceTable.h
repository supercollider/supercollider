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

#pragma once

// TODO next time this is updated, change SC_PlugIn.hpp `in`, `zin`, etc. to take uint32s
// TODO next time this is updated, change SC_PlugIn.hpp `numInputs`, `numOutputs` to have correct
// return type
static const int sc_api_version = 4;

#include "SC_Types.h"
#include "SC_World.h"
#include "SC_SndBuf.h"
#include "SC_Unit.h"
#include "SC_BufGen.h"
#include "SC_FifoMsg.h"
#include "SC_fftlib.h"
#include "SC_Export.h"

typedef struct SF_INFO SF_INFO;

typedef SCBool (*AsyncStageFn)(World* inWorld, void* cmdData);
typedef void (*AsyncFreeFn)(World* inWorld, void* cmdData);

struct ScopeBufferHnd {
    void* internalData;
    float* data;
    uint32 channels;
    uint32 maxFrames;

#ifdef __cplusplus
    float* channel_data(uint32 channel) { return data + (channel * maxFrames); }

    operator bool() { return internalData != 0; }
#endif
};

struct InterfaceTable {
    uint32 mSineSize;
    float32* mSineWavetable;
    float32* mSine;
    float32* mCosecant;

    // call printf for debugging. should not use in finished code.
    int32 (*fPrint)(const char* fmt, ...);

    // get a seed for a random number generator
    int32 (*fRanSeed)();

    // define a unit def
    SCBool (*fDefineUnit)(const char* inUnitClassName, size_t inAllocSize, UnitCtorFunc inCtor, UnitDtorFunc inDtor,
                          uint32 inFlags);

    // define a command  /cmd
    SCBool (*fDefinePlugInCmd)(const char* inCmdName, PlugInCmdFunc inFunc, void* inUserData);

    // define a command for a unit generator  /u_cmd
    SCBool (*fDefineUnitCmd)(const char* inUnitClassName, const char* inCmdName, UnitCmdFunc inFunc);

    // define a buf gen
    SCBool (*fDefineBufGen)(const char* inName, BufGenFunc inFunc);

    // clear all of the unit's outputs.
    void (*fClearUnitOutputs)(Unit* inUnit, int32 inNumSamples);

    // non real time memory allocation
    void* (*fNRTAlloc)(size_t inSize);
    void* (*fNRTRealloc)(void* inPtr, size_t inSize);
    void (*fNRTFree)(void* inPtr);

    // real time memory allocation
    void* (*fRTAlloc)(World* inWorld, size_t inSize);
    void* (*fRTRealloc)(World* inWorld, void* inPtr, size_t inSize);
    void (*fRTFree)(World* inWorld, void* inPtr);

    // call to set a Node to run or not.
    void (*fNodeRun)(struct Node* node, int32 run);

    // call to stop a Graph after the next buffer.
    void (*fNodeEnd)(struct Node* graph);

    // send a trigger from a Node to clients
    void (*fSendTrigger)(struct Node* inNode, int32 triggerID, float value);

    // send a reply message from a Node to clients
    void (*fSendNodeReply)(struct Node* inNode, int32 replyID, const char* cmdName, int numArgs, const float* values);

    // sending messages between real time and non real time levels.
    SCBool (*fSendMsgFromRT)(World* inWorld, struct FifoMsg* inMsg);
    SCBool (*fSendMsgToRT)(World* inWorld, struct FifoMsg* inMsg);

    // libsndfile support
    int (*fSndFileFormatInfoFromStrings)(SF_INFO* info, const char* headerFormatString, const char* sampleFormatString);

    // get nodes by id
    struct Node* (*fGetNode)(World* inWorld, int32 inID);
    struct Graph* (*fGetGraph)(World* inWorld, int32 inID);

    void (*fNRTLock)(World* inWorld);
    void (*fNRTUnlock)(World* inWorld);

    void (*fGroup_DeleteAll)(struct Group* group);
    void (*fDoneAction)(int32 doneAction, struct Unit* unit);

    SCErr (*fDoAsynchronousCommand)(
        World* inWorld, void* replyAddr, const char* cmdName, void* cmdData,
        AsyncStageFn stage2, // stage2 is non real time
        AsyncStageFn stage3, // stage3 is real time - completion msg performed if stage3 returns true
        AsyncStageFn stage4, // stage4 is non real time - sends done if stage4 returns true
        AsyncFreeFn cleanup, int32 completionMsgSize, const void* completionMsgData);


    // fBufAlloc should only be called within a BufGenFunc
    SCErr (*fBufAlloc)(SndBuf* inBuf, int32 inChannels, int32 inFrames, double inSampleRate);

    // To initialise a specific FFT, ensure your input and output buffers exist. Internal data structures
    // will be allocated using the alloc object,
    // Both "fullsize" and "winsize" should be powers of two (this is not checked internally).
    // "wintype" must be a SCFFT_WindowFunction, "direction" must be kForward or kBackward. See SC_fftlib.h.
    struct scfft* (*fSCfftCreate)(size_t fullsize, size_t winsize, int32 wintype, float* indata, float* outdata,
                                  int32 direction, struct SCFFT_Allocator* alloc);

    void (*fSCfftDoFFT)(struct scfft* f);
    void (*fSCfftDoIFFT)(struct scfft* f);

    // destroy any resources held internally.
    void (*fSCfftDestroy)(struct scfft* f, struct SCFFT_Allocator* alloc);

    // Get scope buffer. Returns the maximum number of possile frames.
    SCBool (*fGetScopeBuffer)(World* inWorld, int32 index, int32 channels, int32 maxFrames, struct ScopeBufferHnd*);
    void (*fPushScopeBuffer)(World* inWorld, struct ScopeBufferHnd*, int frames);
    void (*fReleaseScopeBuffer)(World* inWorld, struct ScopeBufferHnd*);
};

typedef struct InterfaceTable InterfaceTable;

#define Print (*ft->fPrint)
#define RanSeed (*ft->fRanSeed)
#define NodeEnd (*ft->fNodeEnd)
#define NodeRun (*ft->fNodeRun)
#define DefineUnit (*ft->fDefineUnit)
#define DefinePlugInCmd (*ft->fDefinePlugInCmd)
#define DefineUnitCmd (*ft->fDefineUnitCmd)
#define DefineBufGen (*ft->fDefineBufGen)
#define ClearUnitOutputs (*ft->fClearUnitOutputs)
#define SendTrigger (*ft->fSendTrigger)
#define SendNodeReply (*ft->fSendNodeReply)
#ifdef __cplusplus
#    define SendMsgFromRT(world, msg) (*ft->fSendMsgFromRT)(world, &msg)
#    define SendMsgToRT(world, msg) (*ft->fSendMsgToRT)(world, &msg)
#else
#    define SendMsgFromRT (*ft->fSendMsgFromRT)
#    define SendMsgToRT (*ft->fSendMsgToRT)
#endif
#define DoneAction (*ft->fDoneAction)

#define NRTAlloc (*ft->fNRTAlloc)
#define NRTRealloc (*ft->fNRTRealloc)
#define NRTFree (*ft->fNRTFree)

#define RTAlloc (*ft->fRTAlloc)
#define RTRealloc (*ft->fRTRealloc)
#define RTFree (*ft->fRTFree)

#define SC_GetNode (*ft->fGetNode)
#define SC_GetGraph (*ft->fGetGraph)

#define NRTLock (*ft->fNRTLock)
#define NRTUnlock (*ft->fNRTUnlock)

#define BufAlloc (*ft->fBufAlloc)

#define GroupDeleteAll (*ft->fGroup_DeleteAll)

#define SndFileFormatInfoFromStrings (*ft->fSndFileFormatInfoFromStrings)

#define DoAsynchronousCommand (*ft->fDoAsynchronousCommand)

#define DefineSimpleUnit(name) (*ft->fDefineUnit)(#name, sizeof(name), (UnitCtorFunc)&name##_Ctor, 0, 0);

#define DefineDtorUnit(name)                                                                                           \
    (*ft->fDefineUnit)(#name, sizeof(name), (UnitCtorFunc)&name##_Ctor, (UnitDtorFunc)&name##_Dtor, 0);

#define DefineSimpleCantAliasUnit(name)                                                                                \
    (*ft->fDefineUnit)(#name, sizeof(name), (UnitCtorFunc)&name##_Ctor, 0, kUnitDef_CantAliasInputsToOutputs);

#define DefineDtorCantAliasUnit(name)                                                                                  \
    (*ft->fDefineUnit)(#name, sizeof(name), (UnitCtorFunc)&name##_Ctor, (UnitDtorFunc)&name##_Dtor,                    \
                       kUnitDef_CantAliasInputsToOutputs);

typedef enum { sc_server_scsynth = 0, sc_server_supernova = 1 } SC_ServerType;

#ifdef STATIC_PLUGINS
#    define PluginLoad(name) void name##_Load(InterfaceTable* inTable)
#    define PluginUnload(name) void name##_Unload(void)
#else
#    ifdef SUPERNOVA
#        define SUPERNOVA_CHECK                                                                                        \
            C_LINKAGE SC_API_EXPORT int server_type(void) { return sc_server_supernova; }
#    else
#        define SUPERNOVA_CHECK                                                                                        \
            C_LINKAGE SC_API_EXPORT int server_type(void) { return sc_server_scsynth; }
#    endif

#    define PluginLoad(name)                                                                                           \
        C_LINKAGE SC_API_EXPORT int api_version(void) { return sc_api_version; }                                       \
        SUPERNOVA_CHECK                                                                                                \
        C_LINKAGE SC_API_EXPORT void load(InterfaceTable* inTable)

#    define PluginUnload(name) C_LINKAGE SC_API_EXPORT void unload(void)
#endif

#ifdef __cplusplus
#    define scfft_create(fullsize, winsize, wintype, indata, outdata, direction, alloc)                                \
        (*ft->fSCfftCreate)(fullsize, winsize, wintype, indata, outdata, direction, &alloc)

#    define scfft_destroy(fft, alloc) (*ft->fSCfftDestroy)(fft, &alloc)
#else
#    define scfft_create (*ft->fSCfftCreate)
#    define scfft_destroy (*ft->fSCfftDestroy)
#endif

#define scfft_dofft (*ft->fSCfftDoFFT)
#define scfft_doifft (*ft->fSCfftDoIFFT)

SC_INLINE void* SCWorld_Allocator_alloc(void* user, size_t size) {
    World* world = (World*)user;
    return world->ft->fRTAlloc(world, size);
}

SC_INLINE void SCWorld_Allocator_free(void* user, void* ptr) {
    World* world = (World*)user;
    world->ft->fRTFree(world, ptr);
}

#ifdef __cplusplus
struct SCWorld_Allocator : SCFFT_Allocator {
    // Note: this constructor ensures source compatibility with plugins written before SC 3.15
    SCWorld_Allocator(InterfaceTable* ft, World* world): SCWorld_Allocator(world) {}

    SCWorld_Allocator(World* world) {
        mAlloc = SCWorld_Allocator_alloc;
        mFree = SCWorld_Allocator_free;
        mUser = world;
    }
};
#endif
