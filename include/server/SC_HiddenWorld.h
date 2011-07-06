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


#ifndef _SC_HiddenWorld_
#define _SC_HiddenWorld_

#include "SC_Types.h"
#include "SC_Sem.h"
#include "SC_Rate.h"
#include "SC_SndBuf.h"
#include "SC_RGen.h"
#include "HashTable.h"
#include "SC_World.h"
#include "SC_Reply.h"
#include "MsgFifo.h"

extern HashTable<struct UnitDef, Malloc> *gUnitDefLib;


struct TriggerMsg {
	World *mWorld;
	int32 mNodeID;
	int32 mTriggerID;
	float mValue;

	void Perform();
};


struct NodeReplyMsg {
	World *mWorld;
	int32 mNodeID;
	int32 mID;
	int32 mNumArgs;
	float *mValues;
	int32 mCmdNameSize;
	char *mCmdName;
	void *mRTMemory;
	void Perform();
};


struct NodeEndMsg {
	World *mWorld;
	int32 mNodeID;
	int32 mGroupID;
	int32 mPrevNodeID;
	int32 mNextNodeID;
	int32 mIsGroup;
	int32 mHeadID;
	int32 mTailID;
	int32 mState;

	void Perform();
};

struct DeleteGraphDefMsg {
	struct GraphDef* mDef;

	void Perform();
};


typedef MsgFifoNoFree<TriggerMsg, 1024> TriggersFifo;
typedef MsgFifoNoFree<NodeReplyMsg, 1024> NodeReplyFifo;
typedef MsgFifoNoFree<NodeEndMsg, 1024> NodeEndsFifo;
typedef MsgFifoNoFree<DeleteGraphDefMsg, 512> DeleteGraphDefsFifo;
typedef HashTable<struct GraphDef, Malloc> GrafDefTable;

struct HiddenWorld
{
	class AllocPool *mAllocPool;
	IntHashTable<struct Node, AllocPool> *mNodeLib;
	GrafDefTable *mGraphDefLib;
	uint32 mNumUsers, mMaxUsers;
	ReplyAddress *mUsers;

	class SC_AudioDriver *mAudioDriver;
	char mPassword[32];

	uint32 mMaxWireBufs;
	float *mWireBufSpace;

	TriggersFifo mTriggers;
	NodeReplyFifo mNodeMsgs;
	NodeEndsFifo mNodeEnds;
	DeleteGraphDefsFifo mDeleteGraphDefs;

	SC_Semaphore* mQuitProgram;
	bool mTerminating;

#ifndef NO_LIBSNDFILE
	SNDFILE *mNRTInputFile;
	SNDFILE *mNRTOutputFile;
	FILE *mNRTCmdFile;
#endif

	int32 mHiddenID;
	int32 mRecentID;

#ifdef __APPLE__
	const char* mInputStreamsEnabled;
	const char* mOutputStreamsEnabled;
#endif
	const char *mInDeviceName;
	const char *mOutDeviceName;
};

typedef struct HiddenWorld HiddenWorld;

inline SC_AudioDriver *AudioDriver(World *inWorld)
{
	return inWorld->hw->mAudioDriver;
}

#endif
