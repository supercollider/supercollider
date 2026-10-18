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

#pragma once

#include "SC_World.h"
#include "SC_Command.h"
#include "sc_msg_iter.h"
#include "SC_SndFileHelpers.hpp"
#include "SC_ReplyImpl.hpp"

struct BufGen;
struct GraphDef;
struct Unit;

#define CallSequencedCommand(T, inWorld, inSize, inData, inReply)                                                      \
    void* space = World_Alloc(inWorld, sizeof(T));                                                                     \
    if (!space)                                                                                                        \
        return kSCErr_OutOfRealTimeMemory;                                                                             \
    T* cmd = new (space) T(inWorld, inReply);                                                                          \
    if (!cmd)                                                                                                          \
        return kSCErr_Failed;                                                                                          \
    int err = cmd->Init(inData, inSize);                                                                               \
    if (err) {                                                                                                         \
        cmd->Delete();                                                                                                 \
        return err;                                                                                                    \
    }                                                                                                                  \
    if (inWorld->mRealTime)                                                                                            \
        cmd->CallNextStage();                                                                                          \
    else                                                                                                               \
        cmd->CallEveryStage();


class SC_SequencedCommand {
public:
    SC_SequencedCommand(World* inWorld, const ReplyAddress* inReplyAddress);

    /** @brief virtual destructor.
     *  Frees any resources that have been allocated from the RT memory pool.
     *  The base implementation only frees the completion message (if provided).
     *  If a child class manages additional RT sources, it should free them
     *  in its own (virtual) destructor.
     */
    virtual ~SC_SequencedCommand();

    /** @brief Destroy the command object.
     *  Calls the (virtual) destructor and frees the underlying RT memory.
     */
    void Delete();

    void CallEveryStage();
    void CallNextStage();

    virtual int Init(char* inData, int inSize);

    virtual bool Stage1(); //     real time
    virtual bool Stage2(); // non real time
    virtual bool Stage3(); //     real time
    virtual void Stage4(); // non real time

    void SendDone(const char* inCommandName);
    void SendDoneWithIntValue(const char* inCommandName, int value);

protected:
    int mNextStage;
    ReplyAddress mReplyAddress;
    World* mWorld;

    // optional completion message data
    int mMsgSize;
    char* mMsgData;
};

///////////////////////////////////////////////////////////////////////////

class SyncCmd : public SC_SequencedCommand {
public:
    SyncCmd(World* inWorld, ReplyAddress* inReplyAddress);

    int Init(char* inData, int inSize) override;

    bool Stage2() override; // non real time
    bool Stage3() override; //     real time
    void Stage4() override; // non real time

protected:
    int mID;
};

///////////////////////////////////////////////////////////////////////////

class BufGenCmd : public SC_SequencedCommand {
public:
    BufGenCmd(World* inWorld, ReplyAddress* inReplyAddress);
    virtual ~BufGenCmd();

    int Init(char* inData, int inSize) override;

    bool Stage2() override; // non real time
    bool Stage3() override; //     real time
    void Stage4() override; // non real time

protected:
    int mBufIndex;
    BufGen* mBufGen;
    sc_msg_iter mMsg;
    char* mData;
    int mSize;
    SndBuf mSndBuf;
    float* mFreeData;
};

///////////////////////////////////////////////////////////////////////////

class BufAllocCmd : public SC_SequencedCommand {
public:
    BufAllocCmd(World* inWorld, ReplyAddress* inReplyAddress);

    int Init(char* inData, int inSize) override;

    bool Stage2() override; // non real time
    bool Stage3() override; //     real time
    void Stage4() override; // non real time

protected:
    int mBufIndex;
    SndBuf mSndBuf;
    int mNumChannels, mNumFrames;
    float mSampleRate;
    float* mFreeData;
};

///////////////////////////////////////////////////////////////////////////


class BufFreeCmd : public SC_SequencedCommand {
public:
    BufFreeCmd(World* inWorld, ReplyAddress* inReplyAddress);

    int Init(char* inData, int inSize) override;

    bool Stage2() override; // non real time
    bool Stage3() override; //     real time
    void Stage4() override; // non real time

protected:
    int mBufIndex;
    float* mFreeData;
};


///////////////////////////////////////////////////////////////////////////


class BufCloseCmd : public SC_SequencedCommand {
public:
    BufCloseCmd(World* inWorld, ReplyAddress* inReplyAddress);

    int Init(char* inData, int inSize) override;

    bool Stage2() override; // non real time
    bool Stage3() override; //     real time
    void Stage4() override; // non real time

protected:
    int mBufIndex;
};


///////////////////////////////////////////////////////////////////////////


class BufZeroCmd : public SC_SequencedCommand {
public:
    BufZeroCmd(World* inWorld, ReplyAddress* inReplyAddress);

    int Init(char* inData, int inSize) override;

    bool Stage2() override; // non real time
    bool Stage3() override; //     real time
    void Stage4() override; // non real time

protected:
    int mBufIndex;
};

///////////////////////////////////////////////////////////////////////////

class BufAllocReadCmd : public SC_SequencedCommand {
public:
    BufAllocReadCmd(World* inWorld, ReplyAddress* inReplyAddress);
    virtual ~BufAllocReadCmd();

    int Init(char* inData, int inSize) override;

    bool Stage2() override; // non real time
    bool Stage3() override; //     real time
    void Stage4() override; // non real time

protected:
    int mBufIndex;
    float* mFreeData;
    SndBuf mSndBuf;
    char* mFilename;
    int mFileOffset, mNumFrames;
};

///////////////////////////////////////////////////////////////////////////

class BufReadCmd : public SC_SequencedCommand {
public:
    BufReadCmd(World* inWorld, ReplyAddress* inReplyAddress);
    virtual ~BufReadCmd();

    int Init(char* inData, int inSize) override;

    bool Stage2() override; // non real time
    bool Stage3() override; //     real time
    void Stage4() override; // non real time

protected:
    int mBufIndex;
    char* mFilename;
    int mFileOffset, mNumFrames, mBufOffset;
    bool mLeaveFileOpen;
    double mSampleRate;
};

///////////////////////////////////////////////////////////////////////////

class SC_BufReadCommand : public SC_SequencedCommand {
public:
    enum { kMaxNumChannels = 32 };

    SC_BufReadCommand(World* inWorld, ReplyAddress* inReplyAddress);

protected:
    void InitChannels(sc_msg_iter& msg);
    bool CheckChannels(int inNumChannels);
    void CopyChannels(float* dst, float* src, size_t srcChannels, size_t numFrames);

protected:
    int mNumChannels;
    int mChannels[kMaxNumChannels];
};

///////////////////////////////////////////////////////////////////////////

class BufAllocReadChannelCmd : public SC_BufReadCommand {
public:
    BufAllocReadChannelCmd(World* inWorld, ReplyAddress* inReplyAddress);
    virtual ~BufAllocReadChannelCmd();

    int Init(char* inData, int inSize) override;

    bool Stage2() override; // non real time
    bool Stage3() override; //     real time
    void Stage4() override; // non real time

protected:
    int mBufIndex;
    float* mFreeData;
    SndBuf mSndBuf;
    char* mFilename;
    int mFileOffset, mNumFrames;
};

///////////////////////////////////////////////////////////////////////////

class BufReadChannelCmd : public SC_BufReadCommand {
public:
    BufReadChannelCmd(World* inWorld, ReplyAddress* inReplyAddress);
    virtual ~BufReadChannelCmd();

    int Init(char* inData, int inSize) override;

    bool Stage2() override; // non real time
    bool Stage3() override; //     real time
    void Stage4() override; // non real time

protected:
    int mBufIndex;
    char* mFilename;
    int mFileOffset, mNumFrames, mBufOffset;
    bool mLeaveFileOpen;
    double mSampleRate;
};

///////////////////////////////////////////////////////////////////////////

class BufWriteCmd : public SC_SequencedCommand {
public:
    BufWriteCmd(World* inWorld, ReplyAddress* inReplyAddress);
    virtual ~BufWriteCmd();

    int Init(char* inData, int inSize) override;

    bool Stage2() override; // non real time
    bool Stage3() override; //     real time
    void Stage4() override; // non real time

protected:
    int mBufIndex;
    char* mFilename;
#ifndef NO_LIBSNDFILE
    SF_INFO mFileInfo;
#endif
    int mNumFrames, mBufOffset;
    bool mLeaveFileOpen;
};

///////////////////////////////////////////////////////////////////////////

class AudioQuitCmd : public SC_SequencedCommand {
public:
    AudioQuitCmd(World* inWorld, ReplyAddress* inReplyAddress);

    bool Stage2() override; // non real time
    bool Stage3() override; //     real time
    void Stage4() override; // non real time
};

///////////////////////////////////////////////////////////////////////////

class AudioStatusCmd : public SC_SequencedCommand {
public:
    AudioStatusCmd(World* inWorld, ReplyAddress* inReplyAddress);

    bool Stage2() override; // non real time
};

///////////////////////////////////////////////////////////////////////////

class RTMemStatusCmd : public SC_SequencedCommand {
public:
    RTMemStatusCmd(World* inWorld, ReplyAddress* inReplyAddress);

    bool Stage2() override; // non real time
};
///////////////////////////////////////////////////////////////////////////

class NotifyCmd : public SC_SequencedCommand {
public:
    NotifyCmd(World* inWorld, ReplyAddress* inReplyAddress);

    int Init(char* inData, int inSize) override;

    bool Stage2() override; // non real time

protected:
    int mOnOff;
    int mID;
};


///////////////////////////////////////////////////////////////////////////

#define CallSendFailureCommand(inWorld, inCmdName, inErrString, inReply)                                               \
    void* space = World_Alloc(inWorld, sizeof(SendFailureCmd));                                                        \
    if (!space)                                                                                                        \
        return kSCErr_OutOfRealTimeMemory;                                                                             \
    SendFailureCmd* cmd = new (space) SendFailureCmd(inWorld, inReply);                                                \
    if (!cmd)                                                                                                          \
        return kSCErr_Failed;                                                                                          \
    cmd->InitSendFailureCmd(inCmdName, inErrString);                                                                   \
    if (inWorld->mRealTime)                                                                                            \
        cmd->CallNextStage();                                                                                          \
    else                                                                                                               \
        cmd->CallEveryStage();

class SendFailureCmd : public SC_SequencedCommand {
public:
    SendFailureCmd(World* inWorld, ReplyAddress* inReplyAddress);
    virtual ~SendFailureCmd();

    void InitSendFailureCmd(const char* inCmdName, const char* inErrString);

    bool Stage2() override; // non real time

protected:
    char *mCmdName, *mErrString;
};

///////////////////////////////////////////////////////////////////////////

class LoadSynthDefCmd : public SC_SequencedCommand {
public:
    LoadSynthDefCmd(World* inWorld, ReplyAddress* inReplyAddress);
    virtual ~LoadSynthDefCmd();

    int Init(char* inData, int inSize) override;

    bool Stage2() override; // non real time
    bool Stage3() override; //     real time
    void Stage4() override; // non real time

protected:
    char* mFilename;
    GraphDef* mDefs;
};

///////////////////////////////////////////////////////////////////////////

class RecvSynthDefCmd : public SC_SequencedCommand {
public:
    RecvSynthDefCmd(World* inWorld, ReplyAddress* inReplyAddress);
    virtual ~RecvSynthDefCmd();

    int Init(char* inData, int inSize) override;

    bool Stage2() override; // non real time
    bool Stage3() override; //     real time
    void Stage4() override; // non real time

protected:
    char* mBuffer;
    size_t mSize;
    GraphDef* mDefs;
};

///////////////////////////////////////////////////////////////////////////

class LoadSynthDefDirCmd : public SC_SequencedCommand {
public:
    LoadSynthDefDirCmd(World* inWorld, ReplyAddress* inReplyAddress);
    virtual ~LoadSynthDefDirCmd();

    int Init(char* inData, int inSize) override;

    bool Stage2() override; // non real time
    bool Stage3() override; //     real time
    void Stage4() override; // non real time

protected:
    char* mFilename;
    GraphDef* mDefs;
};

///////////////////////////////////////////////////////////////////////////

class SendReplyCmd : public SC_SequencedCommand {
public:
    SendReplyCmd(World* inWorld, ReplyAddress* inReplyAddress);

    int Init(char* inData, int inSize) override;

    bool Stage2() override; // non real time
};

///////////////////////////////////////////////////////////////////////////

// AsyncPlugInCmd and AsyncPlugInCmdEx only differ in the type of stage function.
template <typename StageFn> class AsyncPlugInCmd_ : public SC_SequencedCommand {
public:
    AsyncPlugInCmd_(World* inWorld, ReplyAddress* inReplyAddress, const char* cmdName, void* cmdData,
                    StageFn stage2, // stage2 is non real time
                    StageFn stage3, // stage3 is real time - completion msg performed if stage3 returns true
                    StageFn stage4, // stage4 is non real time - sends done if stage4 returns true
                    AsyncFreeFn cleanup, int completionMsgSize, const void* completionMsgData);

    virtual ~AsyncPlugInCmd_();

    bool Stage2() override; // non real time
    bool Stage3() override; //     real time
    void Stage4() override; // non real time

protected:
    const char* mCmdName;
    void* mCmdData;
    StageFn mStage2, mStage3, mStage4;
    AsyncFreeFn mCleanup;
};

using AsyncPlugInCmd = AsyncPlugInCmd_<AsyncStageFn>;
using AsyncPlugInCmdEx = AsyncPlugInCmd_<AsyncStageFnEx>;

///////////////////////////////////////////////////////////////////////////

class AsyncUnitCmd : public SC_SequencedCommand {
public:
    AsyncUnitCmd(Unit* inUnit, ReplyAddress* inReplyAddress, const char* cmdName, void* cmdData,
                 AsyncUnitStageFn stage2, // stage2 is non real time
                 AsyncUnitStageFn stage3, // stage3 is real time - completion msg performed if stage3 returns true
                 AsyncUnitStageFn stage4, // stage4 is non real time - sends done if stage4 returns true
                 AsyncFreeFn cleanup, int completionMsgSize, const void* completionMsgData);

    virtual ~AsyncUnitCmd();

    bool Stage2() override; // non real time
    bool Stage3() override; //     real time
    void Stage4() override; // non real time

protected:
    Unit* mUnit;
    const char* mCmdName;
    void* mCmdData;
    AsyncUnitStageFn mStage2, mStage3, mStage4;
    AsyncFreeFn mCleanup;
    bool mAlive;
};
