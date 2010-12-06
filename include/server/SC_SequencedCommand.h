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

/*
 * Having SequencedCommands allows performing actions that might otherwise require
 * taking a mutex, which is undesirable in a real time thread.
 * Some commands require several stages of processing at both the real time
 * and non real time levels. This class does the messaging between levels for you
 * so that you only need to write the functions.
 */

#ifndef _SC_SequencedCommand_
#define _SC_SequencedCommand_

#include "OSC_Packet.h"
#include "SC_World.h"
#include "SC_BufGen.h"
#include "sc_msg_iter.h"
#ifndef NO_LIBSNDFILE
	#ifdef _WIN32
		#include <sndfile-win.h>
	#else
		#include <sndfile.h>
	#endif
#endif
#include <new>

#define CallSequencedCommand(T, inWorld, inSize, inData, inReply)	\
	void* space = World_Alloc(inWorld, sizeof(T)); \
	T *cmd = new (space) T(inWorld, inReply); \
	if (!cmd) return kSCErr_Failed; \
	int err = cmd->Init(inData, inSize); \
	if (err) { \
		cmd->~T(); \
		World_Free(inWorld, space); \
		return err; \
	} \
	if (inWorld->mRealTime) cmd->CallNextStage(); \
	else cmd->CallEveryStage();


class SC_SequencedCommand
{
public:
	SC_SequencedCommand(World *inWorld, ReplyAddress *inReplyAddress);
	virtual ~SC_SequencedCommand();

	void Delete();

	void CallEveryStage();
	void CallNextStage();

	virtual int Init(char *inData, int inSize);

	virtual bool Stage1();	//     real time
	virtual bool Stage2();	// non real time
	virtual bool Stage3();	//     real time
	virtual void Stage4();	// non real time

	void SendDone(const char *inCommandName);
	void SendDoneWithIntValue(const char *inCommandName, int value);


protected:
	int mNextStage;
	ReplyAddress mReplyAddress;
	World *mWorld;

	int mMsgSize;
	char *mMsgData;

	virtual void CallDestructor()=0;
};

///////////////////////////////////////////////////////////////////////////

class SyncCmd : public SC_SequencedCommand
{
public:
	SyncCmd(World *inWorld, ReplyAddress *inReplyAddress);

	virtual int Init(char *inData, int inSize);

	virtual bool Stage2();	// non real time
	virtual bool Stage3();	//     real time
	virtual void Stage4();	// non real time

protected:
	virtual void CallDestructor();
	int mID;
};

///////////////////////////////////////////////////////////////////////////

class BufGenCmd : public SC_SequencedCommand
{
public:
	BufGenCmd(World *inWorld, ReplyAddress *inReplyAddress);
	virtual ~BufGenCmd();

	virtual int Init(char *inData, int inSize);

	virtual bool Stage2();	// non real time
	virtual bool Stage3();	//     real time
	virtual void Stage4();	// non real time

protected:
	int mBufIndex;
	BufGen *mBufGen;
	sc_msg_iter mMsg;
	char *mData;
	int mSize;
	SndBuf mSndBuf;
	float *mFreeData;

	virtual void CallDestructor();

};

///////////////////////////////////////////////////////////////////////////

class BufAllocCmd : public SC_SequencedCommand
{
public:
	BufAllocCmd(World *inWorld, ReplyAddress *inReplyAddress);

	virtual int Init(char *inData, int inSize);

	virtual bool Stage2();	// non real time
	virtual bool Stage3();	//     real time
	virtual void Stage4();	// non real time

protected:
	int mBufIndex;
	SndBuf mSndBuf;
	int mNumChannels, mNumFrames;
	float *mFreeData;

	virtual void CallDestructor();

};

///////////////////////////////////////////////////////////////////////////


class BufFreeCmd : public SC_SequencedCommand
{
public:
	BufFreeCmd(World *inWorld, ReplyAddress *inReplyAddress);

	virtual int Init(char *inData, int inSize);

	virtual bool Stage2();	// non real time
	virtual bool Stage3();	//     real time
	virtual void Stage4();	// non real time

protected:
	int mBufIndex;
	float *mFreeData;

	virtual void CallDestructor();
};


///////////////////////////////////////////////////////////////////////////


class BufCloseCmd : public SC_SequencedCommand
{
public:
	BufCloseCmd(World *inWorld, ReplyAddress *inReplyAddress);

	virtual int Init(char *inData, int inSize);

	virtual bool Stage2();	// non real time
	virtual bool Stage3();	//     real time
	virtual void Stage4();	// non real time

protected:
	int mBufIndex;

	virtual void CallDestructor();
};


///////////////////////////////////////////////////////////////////////////


class BufZeroCmd : public SC_SequencedCommand
{
public:
	BufZeroCmd(World *inWorld, ReplyAddress *inReplyAddress);

	virtual int Init(char *inData, int inSize);

	virtual bool Stage2();	// non real time
	virtual bool Stage3();	//     real time
	virtual void Stage4();	// non real time

protected:
	int mBufIndex;

	virtual void CallDestructor();
};

///////////////////////////////////////////////////////////////////////////

class BufAllocReadCmd : public SC_SequencedCommand
{
public:
	BufAllocReadCmd(World *inWorld, ReplyAddress *inReplyAddress);
	virtual ~BufAllocReadCmd();

	virtual int Init(char *inData, int inSize);

	virtual bool Stage2();	// non real time
	virtual bool Stage3();	//     real time
	virtual void Stage4();	// non real time

protected:
	int mBufIndex;
	float *mFreeData;
	SndBuf mSndBuf;
	char *mFilename;
	int mFileOffset, mNumFrames;

	virtual void CallDestructor();
};

///////////////////////////////////////////////////////////////////////////

class BufReadCmd : public SC_SequencedCommand
{
public:
	BufReadCmd(World *inWorld, ReplyAddress *inReplyAddress);
	virtual ~BufReadCmd();

	virtual int Init(char *inData, int inSize);

	virtual bool Stage2();	// non real time
	virtual bool Stage3();	//     real time
	virtual void Stage4();	// non real time

protected:
	int mBufIndex;
	char *mFilename;
	int mFileOffset, mNumFrames, mBufOffset;
	bool mLeaveFileOpen;
	double mSampleRate;
	virtual void CallDestructor();
};

///////////////////////////////////////////////////////////////////////////

class SC_BufReadCommand : public SC_SequencedCommand
{
public:
	enum {
		kMaxNumChannels = 32
	};

	SC_BufReadCommand(World* inWorld, ReplyAddress* inReplyAddress);
	virtual ~SC_BufReadCommand();

protected:
	void InitChannels(sc_msg_iter& msg);
	bool CheckChannels(int inNumChannels);
	void CopyChannels(float* dst, float* src, size_t srcChannels, size_t numFrames);

protected:
	int mNumChannels;
	int mChannels[kMaxNumChannels];
};

///////////////////////////////////////////////////////////////////////////

class BufAllocReadChannelCmd : public SC_BufReadCommand
{
public:
	BufAllocReadChannelCmd(World *inWorld, ReplyAddress *inReplyAddress);
	virtual ~BufAllocReadChannelCmd();

	virtual int Init(char *inData, int inSize);

	virtual bool Stage2();	// non real time
	virtual bool Stage3();	//     real time
	virtual void Stage4();	// non real time

protected:
	int mBufIndex;
	float *mFreeData;
	SndBuf mSndBuf;
	char *mFilename;
	int mFileOffset, mNumFrames;
	virtual void CallDestructor();
};

///////////////////////////////////////////////////////////////////////////

class BufReadChannelCmd : public SC_BufReadCommand
{
public:
	BufReadChannelCmd(World *inWorld, ReplyAddress *inReplyAddress);
	virtual ~BufReadChannelCmd();

	virtual int Init(char *inData, int inSize);

	virtual bool Stage2();	// non real time
	virtual bool Stage3();	//     real time
	virtual void Stage4();	// non real time

protected:
	int mBufIndex;
	char *mFilename;
	int mFileOffset, mNumFrames, mBufOffset;
	bool mLeaveFileOpen;
	double mSampleRate;
	virtual void CallDestructor();
};

///////////////////////////////////////////////////////////////////////////

class BufWriteCmd : public SC_SequencedCommand
{
public:
	BufWriteCmd(World *inWorld, ReplyAddress *inReplyAddress);
	virtual ~BufWriteCmd();

	virtual int Init(char *inData, int inSize);

	virtual bool Stage2();	// non real time
	virtual bool Stage3();	//     real time
	virtual void Stage4();	// non real time

protected:
	int mBufIndex;
	char *mFilename;
#ifndef NO_LIBSNDFILE
	SF_INFO mFileInfo;
#endif
	int mNumFrames, mBufOffset;
	bool mLeaveFileOpen;

	virtual void CallDestructor();
};

///////////////////////////////////////////////////////////////////////////

class AudioQuitCmd : public SC_SequencedCommand
{
public:
	AudioQuitCmd(World *inWorld, ReplyAddress *inReplyAddress);

	virtual bool Stage2();	// non real time
	virtual bool Stage3();	//     real time
	virtual void Stage4();	// non real time

protected:

	virtual void CallDestructor();
};

///////////////////////////////////////////////////////////////////////////

class AudioStatusCmd : public SC_SequencedCommand
{
public:
	AudioStatusCmd(World *inWorld, ReplyAddress *inReplyAddress);

	virtual bool Stage2();	// non real time

protected:

	virtual void CallDestructor();
};

///////////////////////////////////////////////////////////////////////////

class NotifyCmd : public SC_SequencedCommand
{
public:
	NotifyCmd(World *inWorld, ReplyAddress *inReplyAddress);

	virtual int Init(char *inData, int inSize);

	virtual bool Stage2();	// non real time

protected:

	virtual void CallDestructor();

	int mOnOff;
	int mID;
};


///////////////////////////////////////////////////////////////////////////

#define CallSendFailureCommand(inWorld, inCmdName, inErrString, inReply)	\
	void* space = World_Alloc(inWorld, sizeof(SendFailureCmd)); \
	SendFailureCmd *cmd = new (space) SendFailureCmd(inWorld, inReply); \
	if (!cmd) return kSCErr_Failed; \
	cmd->InitSendFailureCmd(inCmdName, inErrString); \
	if (inWorld->mRealTime) cmd->CallNextStage(); \
	else cmd->CallEveryStage(); \

class SendFailureCmd : public SC_SequencedCommand
{
public:
	SendFailureCmd(World *inWorld, ReplyAddress *inReplyAddress);
	virtual ~SendFailureCmd();

	virtual void InitSendFailureCmd(const char *inCmdName, const char* inErrString);

	virtual bool Stage2();	// non real time

protected:
	char *mCmdName, *mErrString;

	virtual void CallDestructor();
};

///////////////////////////////////////////////////////////////////////////

#include "SC_GraphDef.h"

class LoadSynthDefCmd : public SC_SequencedCommand
{
public:
	LoadSynthDefCmd(World *inWorld, ReplyAddress *inReplyAddress);
	virtual ~LoadSynthDefCmd();

	virtual int Init(char *inData, int inSize);

	virtual bool Stage2();	// non real time
	virtual bool Stage3();	//     real time
	virtual void Stage4();	// non real time

protected:
	char *mFilename;
	GraphDef *mDefs;

	virtual void CallDestructor();
};

///////////////////////////////////////////////////////////////////////////

#include "SC_GraphDef.h"

class RecvSynthDefCmd : public SC_SequencedCommand
{
public:
	RecvSynthDefCmd(World *inWorld, ReplyAddress *inReplyAddress);
	virtual ~RecvSynthDefCmd();

	virtual int Init(char *inData, int inSize);

	virtual bool Stage2();	// non real time
	virtual bool Stage3();	//     real time
	virtual void Stage4();	// non real time

protected:
	char *mBuffer;
	GraphDef *mDefs;

	virtual void CallDestructor();
};

///////////////////////////////////////////////////////////////////////////

class LoadSynthDefDirCmd : public SC_SequencedCommand
{
public:
	LoadSynthDefDirCmd(World *inWorld, ReplyAddress *inReplyAddress);
	virtual ~LoadSynthDefDirCmd();

	virtual int Init(char *inData, int inSize);

	virtual bool Stage2();	// non real time
	virtual bool Stage3();	//     real time
	virtual void Stage4();	// non real time

protected:
	char *mFilename;
	GraphDef *mDefs;

	virtual void CallDestructor();
};

///////////////////////////////////////////////////////////////////////////

class SendReplyCmd : public SC_SequencedCommand
{
public:
	SendReplyCmd(World *inWorld, ReplyAddress *inReplyAddress);

	virtual int Init(char *inData, int inSize);

	virtual bool Stage2();	// non real time

protected:

	virtual void CallDestructor();
};

///////////////////////////////////////////////////////////////////////////


typedef bool (*AsyncStageFn)(World *inWorld, void* cmdData);
typedef void (*AsyncFreeFn)(World *inWorld, void* cmdData);

class AsyncPlugInCmd : public SC_SequencedCommand
{
public:
	AsyncPlugInCmd(World *inWorld, ReplyAddress *inReplyAddress,
			const char* cmdName,
			void *cmdData,
			AsyncStageFn stage2, // stage2 is non real time
			AsyncStageFn stage3, // stage3 is real time - completion msg performed if stage3 returns true
			AsyncStageFn stage4, // stage4 is non real time - sends done if stage4 returns true
			AsyncFreeFn cleanup,
			int completionMsgSize,
			void* completionMsgData);

	virtual ~AsyncPlugInCmd();

	virtual bool Stage2();	// non real time
	virtual bool Stage3();	//     real time
	virtual void Stage4();	// non real time

protected:
	const char *mCmdName;
	void *mCmdData;
	AsyncStageFn mStage2, mStage3, mStage4;
	AsyncFreeFn mCleanup;

	virtual void CallDestructor();
};

///////////////////////////////////////////////////////////////////////////

#endif

