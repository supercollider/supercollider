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

#ifndef _SC_SynthInterfaceTable_
#define _SC_SynthInterfaceTable_

#include "SC_Types.h"
#include "SC_SndBuf.h"
#include "SC_Unit.h"
#include "SC_BufGen.h"
#include "SC_FifoMsg.h"
#include <sndfile.h>

struct World;

typedef bool (*AsyncStageFn)(World *inWorld, void* cmdData);
typedef void (*AsyncFreeFn)(World *inWorld, void* cmdData);

struct InterfaceTable 
{	
	unsigned int mSineSize;
	float32 *mSineWavetable;
	float32 *mSine;
	float32 *mCosecant;
	
	// call printf for debugging. should not use in finished code.
	int (*fPrint)(const char *fmt, ...);
	
	// get a seed for a random number generator
	int32 (*fRanSeed)();
	
	// define a unit def
	bool (*fDefineUnit)(char *inUnitClassName, size_t inAllocSize, 
			UnitCtorFunc inCtor, UnitDtorFunc inDtor, uint32 inFlags);

	// define a command  /cmd
	bool (*fDefinePlugInCmd)(char *inCmdName, PlugInCmdFunc inFunc, void* inUserData);

	// define a command for a unit generator  /u_cmd
	bool (*fDefineUnitCmd)(char *inUnitClassName, char *inCmdName, UnitCmdFunc inFunc);
	
	// define a buf gen
	bool (*fDefineBufGen)(char *inName, BufGenFunc inFunc);

	// clear all of the unit's outputs.
	void (*fClearUnitOutputs)(Unit *inUnit, int inNumSamples);
	
	// non real time memory allocation
	void* (*fNRTAlloc)(size_t inSize);
	void* (*fNRTRealloc)(void *inPtr, size_t inSize);
	void  (*fNRTFree)(void *inPtr);
	
	// real time memory allocation
	void* (*fRTAlloc)(World *inWorld, size_t inSize);
	void* (*fRTRealloc)(World *inWorld, void *inPtr, size_t inSize);
	void  (*fRTFree)(World *inWorld, void *inPtr);
	
	// call to set a Node to run or not.
	void (*fNodeRun)(struct Node* node, int run);
	
	// call to stop a Graph after the next buffer.
	void (*fNodeEnd)(struct Node* graph);
	
	// send a trigger from a Node to clients
	void (*fSendTrigger)(struct Node* inNode, int triggerID, float value);	
	
	// sending messages between real time and non real time levels.
	bool (*fSendMsgFromRT)(World *inWorld, struct FifoMsg& inMsg);
	bool (*fSendMsgToRT)(World *inWorld, struct FifoMsg& inMsg);
	
	// libsndfile support
	int (*fSndFileFormatInfoFromStrings)(SF_INFO *info, 
		const char *headerFormatString, const char *sampleFormatString);

	// get nodes by id
	struct Node* (*fGetNode)(World *inWorld, int inID);
	struct Graph* (*fGetGraph)(World *inWorld, int inID);
	
	void (*fNRTLock)(World *inWorld);
	void (*fNRTUnlock)(World *inWorld);
	
	bool mAltivecAvailable;

	void (*fGroup_DeleteAll)(struct Group* group);
	void (*fDoneAction)(int doneAction, struct Unit *unit);

	int (*fDoAsynchronousCommand)
		(
			World *inWorld,
			void* replyAddr,
			const char* cmdName,
			void *cmdData,
			AsyncStageFn stage2, // stage2 is non real time
			AsyncStageFn stage3, // stage3 is real time - completion msg performed if stage3 returns true
			AsyncStageFn stage4, // stage4 is non real time - sends done if stage4 returns true
			AsyncFreeFn cleanup,
			int completionMsgSize,
			void* completionMsgData
		);
	

	// fBufAlloc should only be called within a BufGenFunc
	int (*fBufAlloc)(SndBuf *inBuf, int inChannels, int inFrames, double inSampleRate);
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
#define SendMsgFromRT (*ft->fSendMsgFromRT)
#define SendMsgToRT (*ft->fSendMsgToRT)
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

#define DefineSimpleUnit(name) \
	(*ft->fDefineUnit)(#name, sizeof(name), (UnitCtorFunc)&name##_Ctor, 0, 0);

#define DefineDtorUnit(name) \
	(*ft->fDefineUnit)(#name, sizeof(name), (UnitCtorFunc)&name##_Ctor, \
	(UnitDtorFunc)&name##_Dtor, 0);

#define DefineSimpleCantAliasUnit(name) \
	(*ft->fDefineUnit)(#name, sizeof(name), (UnitCtorFunc)&name##_Ctor, 0, kUnitDef_CantAliasInputsToOutputs);

#define DefineDtorCantAliasUnit(name) \
	(*ft->fDefineUnit)(#name, sizeof(name), (UnitCtorFunc)&name##_Ctor, \
	(UnitDtorFunc)&name##_Dtor, kUnitDef_CantAliasInputsToOutputs);


#endif
