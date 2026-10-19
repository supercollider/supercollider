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

/** @brief base class for all sequenced commands
 *
 *  SC_SequencedCommand encapsulates an asynchronous command that may consist
 *  of up to four stages of execution:
 *
 *  - stage 1 (SC_SequencedCommand::Init): runs on the RT thread, right after the
 *    command has been created. The command itself is allocated from the RT memory
 *    pool. Any additional resources that are needed for the other stages must be
 *    allocated from the RT memory pool as well.
 *
 *  - stage 2 (SC_SequencedCommand::stage2): runs on the NRT thread.
 *    This is where  you do any non-realtime-safe work, such as file I/O, Buffer
 *    allocation, SynthDef reading, etc.
 *    In case of an error, you can send a \c /fail message and abort the command
 *    by returning \c false.
 *
 *  - stage 3 (SC_SequencedCommand::stage3): runs on the RT thread.
 *    This is where you apply the data that you created in stage 2. For example,
 *    Buffer commands such as \c /b_alloc update the SndBuf struct with the new
 *    soundfile data.
 *    If the command has been successful, you should send the provided completion
 *    message (if any). See PerformCompletionMsg().
 *
 *  - stage 4 (SC_SequencedCommand::stage4): runs on the NRT thread.
 *    This is where you can safely dispose old data that is no longer needed.
 *    For example, Buffer commands use this stage to free the old buffer data.
 *    We can't do this already in stage 2 because Synths might still use the data
 *    on the RT thread, which would create a data race. However, after we have
 *    updated the necessary structures in stage 3, we know that nothing is
 *    referencing the data anymore, so we can finally delete it.
 *    Finally, you can send a \c /done message to tell the Client that the
 *    command has been completed successfully.
 *
 *  - clean up: runs on the RT thread. This is where you clean up any resources
 *    that you have allocated in stage 1. This is also where the command object
 *    itself is destroyed.
 *
 *  In NRT synthesis, all stages are executed synchronously, see CallEveryStage().
 *
 *  @note Not every asynchronous command needs all four stages! For example,
 *  SendReplyCmd only needs the first two stages. Every stage function returns
 *  a boolean. \c true means that the Server should continue with the next stage.
 *  \c false means that the command is finished and that the Server should jump
 *  to the clean up stage.
 */
class SC_SequencedCommand {
public:
    /** @brief constructor.
     *  @param inWorld  the World instance
     *  @param inReplyAddress   (optional) ReplyAddress used for performing the
     *         completition message and sending \c /done resp. \c /fail replies.
     */
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

    /** @brief calls every stage in sequence (in non-realtime mode) */
    void CallEveryStage();
    /** @brief call the next stage (in realtime mode) */
    void CallNextStage();

    /** @brief initialize the command from the OSC message. */
    virtual int Init(const char* inData, int inSize);

    /** @brief stage 1: RT thread. This function is a no-op, the actual work is done in Init(). */
    virtual bool Stage1();
    /** @brief stage 2: NRT thread. */
    virtual bool Stage2();
    /** @brief stage 3: RT thread. */
    virtual bool Stage3();
    /** @brief stage 4: NRT thread. */
    virtual void Stage4();

protected:
    void SendDone(const char* inCommandName);
    void SendDoneWithIntValue(const char* inCommandName, int value);

    int mNextStage;
    ReplyAddress mReplyAddress;
    World* mWorld;

    // optional completion message data (allocated from the RT memory pool)
    int mMsgSize;
    char* mMsgData;
};

///////////////////////////////////////////////////////////////////////////

/** @brief implements the \c /sync command. */
class SyncCmd : public SC_SequencedCommand {
public:
    SyncCmd(World* inWorld, const ReplyAddress* inReplyAddress);

    int Init(const char* inData, int inSize) override;

    bool Stage2() override; // non real time
    bool Stage3() override; //     real time
    void Stage4() override; // non real time

protected:
    int mID;
};

///////////////////////////////////////////////////////////////////////////

/** @brief implements the \c /b_gen command. */
class BufGenCmd : public SC_SequencedCommand {
public:
    BufGenCmd(World* inWorld, const ReplyAddress* inReplyAddress);
    virtual ~BufGenCmd();

    int Init(const char* inData, int inSize) override;

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

/** @brief implements the \c /b_alloc command. */
class BufAllocCmd : public SC_SequencedCommand {
public:
    BufAllocCmd(World* inWorld, const ReplyAddress* inReplyAddress);

    int Init(const char* inData, int inSize) override;

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

/** @brief implements the \c /b_free command. */
class BufFreeCmd : public SC_SequencedCommand {
public:
    BufFreeCmd(World* inWorld, const ReplyAddress* inReplyAddress);

    int Init(const char* inData, int inSize) override;

    bool Stage2() override; // non real time
    bool Stage3() override; //     real time
    void Stage4() override; // non real time

protected:
    int mBufIndex;
    float* mFreeData;
};


///////////////////////////////////////////////////////////////////////////

/** @brief implements the \c /b_close command. */
class BufCloseCmd : public SC_SequencedCommand {
public:
    BufCloseCmd(World* inWorld, const ReplyAddress* inReplyAddress);

    int Init(const char* inData, int inSize) override;

    bool Stage2() override; // non real time
    bool Stage3() override; //     real time
    void Stage4() override; // non real time

protected:
    int mBufIndex;
};


///////////////////////////////////////////////////////////////////////////

/** @brief implements the \c /b_zero command. */
class BufZeroCmd : public SC_SequencedCommand {
public:
    BufZeroCmd(World* inWorld, const ReplyAddress* inReplyAddress);

    int Init(const char* inData, int inSize) override;

    bool Stage2() override; // non real time
    bool Stage3() override; //     real time
    void Stage4() override; // non real time

protected:
    int mBufIndex;
};

///////////////////////////////////////////////////////////////////////////

/** @brief implements the \c /b_allocRead command. */
class BufAllocReadCmd : public SC_SequencedCommand {
public:
    BufAllocReadCmd(World* inWorld, const ReplyAddress* inReplyAddress);
    virtual ~BufAllocReadCmd();

    int Init(const char* inData, int inSize) override;

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

/** @brief implements the \c /b_read command. */
class BufReadCmd : public SC_SequencedCommand {
public:
    BufReadCmd(World* inWorld, const ReplyAddress* inReplyAddress);
    virtual ~BufReadCmd();

    int Init(const char* inData, int inSize) override;

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

/** @brief common base clase for BufAllocReadChannelCmd and BufReadChannelCmd. */
class SC_BufReadCommand : public SC_SequencedCommand {
public:
    enum { kMaxNumChannels = 32 };

    SC_BufReadCommand(World* inWorld, const ReplyAddress* inReplyAddress);

protected:
    void InitChannels(sc_msg_iter& msg);
    bool CheckChannels(int inNumChannels);
    void CopyChannels(float* dst, float* src, size_t srcChannels, size_t numFrames);

    int mNumChannels;
    int mChannels[kMaxNumChannels];
};

///////////////////////////////////////////////////////////////////////////

/** @brief implements the \c /b_allocReadChannel command. */
class BufAllocReadChannelCmd : public SC_BufReadCommand {
public:
    BufAllocReadChannelCmd(World* inWorld, const ReplyAddress* inReplyAddress);
    virtual ~BufAllocReadChannelCmd();

    int Init(const char* inData, int inSize) override;

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

/** @brief implements the \c /b_readChannel command. */
class BufReadChannelCmd : public SC_BufReadCommand {
public:
    BufReadChannelCmd(World* inWorld, const ReplyAddress* inReplyAddress);
    virtual ~BufReadChannelCmd();

    int Init(const char* inData, int inSize) override;

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

/** @brief implements the \c /b_write command. */
class BufWriteCmd : public SC_SequencedCommand {
public:
    BufWriteCmd(World* inWorld, const ReplyAddress* inReplyAddress);
    virtual ~BufWriteCmd();

    int Init(const char* inData, int inSize) override;

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

/** @brief implements the \c /quit command. */
class AudioQuitCmd : public SC_SequencedCommand {
public:
    AudioQuitCmd(World* inWorld, const ReplyAddress* inReplyAddress);

    bool Stage2() override; // non real time
    bool Stage3() override; //     real time
    void Stage4() override; // non real time
};

///////////////////////////////////////////////////////////////////////////

class RTMemStatusCmd : public SC_SequencedCommand {
public:
    RTMemStatusCmd(World* inWorld, const ReplyAddress* inReplyAddress);

    bool Stage2() override; // non real time
};

///////////////////////////////////////////////////////////////////////////

/** @brief implements the \c /notify command. */
class NotifyCmd : public SC_SequencedCommand {
public:
    NotifyCmd(World* inWorld, const ReplyAddress* inReplyAddress);

    int Init(const char* inData, int inSize) override;

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

/** @brief used by the CallSendFailureCommand macro to send a \c /fail
 *  message from the RT thread. */
class SendFailureCmd : public SC_SequencedCommand {
public:
    SendFailureCmd(World* inWorld, const ReplyAddress* inReplyAddress);
    virtual ~SendFailureCmd();

    void InitSendFailureCmd(const char* inCmdName, const char* inErrString);

    bool Stage2() override; // non real time

protected:
    char *mCmdName, *mErrString;
};

///////////////////////////////////////////////////////////////////////////

/** @brief implements the \c /d_load command. */
class LoadSynthDefCmd : public SC_SequencedCommand {
public:
    LoadSynthDefCmd(World* inWorld, const ReplyAddress* inReplyAddress);
    virtual ~LoadSynthDefCmd();

    int Init(const char* inData, int inSize) override;

    bool Stage2() override; // non real time
    bool Stage3() override; //     real time
    void Stage4() override; // non real time

protected:
    char* mFilename;
    GraphDef* mDefs;
};

///////////////////////////////////////////////////////////////////////////

/** @brief implements the \c /d_recv command. */
class RecvSynthDefCmd : public SC_SequencedCommand {
public:
    RecvSynthDefCmd(World* inWorld, const ReplyAddress* inReplyAddress);
    virtual ~RecvSynthDefCmd();

    int Init(const char* inData, int inSize) override;

    bool Stage2() override; // non real time
    bool Stage3() override; //     real time
    void Stage4() override; // non real time

protected:
    char* mBuffer;
    size_t mSize;
    GraphDef* mDefs;
};

///////////////////////////////////////////////////////////////////////////

/** @brief implements the \c /d_loadDir command. */
class LoadSynthDefDirCmd : public SC_SequencedCommand {
public:
    LoadSynthDefDirCmd(World* inWorld, const ReplyAddress* inReplyAddress);
    virtual ~LoadSynthDefDirCmd();

    int Init(const char* inData, int inSize) override;

    bool Stage2() override; // non real time
    bool Stage3() override; //     real time
    void Stage4() override; // non real time

protected:
    char* mFilename;
    GraphDef* mDefs;
};

///////////////////////////////////////////////////////////////////////////

/** @brief send an OSC reply message from the RT thread. */
class SendReplyCmd : public SC_SequencedCommand {
public:
    SendReplyCmd(World* inWorld, const ReplyAddress* inReplyAddress);

    int Init(const char* inData, int inSize) override;

    bool Stage2() override; // non real time
};

///////////////////////////////////////////////////////////////////////////

/** @brief implements the PerformAsynchronousCommand(Ex) API function.
 *
 *  This class template is shared by AsyncPlugInCmd and AsyncPlugInCmdEx,
 *  which only differs in the type of the stage function.
 */
template <typename StageFn> class AsyncPlugInCmd_ : public SC_SequencedCommand {
public:
    /** @brief constructor
     *
     *  @param inWorld  the World* instance
     *
     *  @param inReplyAddress   (optional) ReplyAddress used for performing the completion message
     *                  and sending the \c /done message. Also passed to `AsyncStageFnEx`.
     *
     *  @param cmdName  (optional) command name. See `stage4`.
     *                  @attention This should be a string literal. No copy is made!
     *
     *  @param cmdData  (optional) user data that is passed to the stage functions.
     *
     *  @param stage2   (optional) stage 2 function (NRT thread). If the function has been omitted
     *                  or returns \c true, execution continues with `stage3`, otherwise it jumps
     *                  straight to `cleanup`.
     *
     *  @param stage3   (optional) stage 3 function (RT thread). If the function has been omitted
     *                  or returns \c true, the (optional) completition message is performed and
     *                  execution continues with `stage4`, otherwise it jumps straight to `cleanup`.
     *
     *  @param stage4   (optional) stage 4 function (NRT thread). A \c /done message is sent with
     *                  `cmdName` as the first argument if the following conditions are true:
     *                  - `stage4` is \c NULL or returns \c true
     *                  - `cmdName` is not \c NULL
     *
     *  @param cleanup  (optional) cleanup function for releasing resources on the RT thread.
     *
     *  @param completionMsgSize    size of the completion message in bytes. Pass 0 if you want to omit it.
     *
     *  @param completionMsgData    the completion message data. Pass \c NULL if you want to omit it.
     */
    AsyncPlugInCmd_(World* inWorld, const ReplyAddress* inReplyAddress, const char* cmdName, void* cmdData,
                    StageFn stage2, StageFn stage3, StageFn stage4, AsyncFreeFn cleanup, int completionMsgSize,
                    const void* completionMsgData);

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

/** @brief implements PerformAsyncUnitCommand API function */
class AsyncUnitCmd : public SC_SequencedCommand {
public:
    /** @brief constructor
     *
     *  @param inUnit   the owning Unit
     *
     *  @param inReplyAddress   (optional) ReplyAddress used for performing the completion message
     *                  and sending the \c /done message. Also passed to the stage functions.
     *
     *  @param cmdName  (optional) command name. See `stage4`.
     *                  @attention This should be a string literal. No copy is made!
     *
     *  @param cmdData  (optional) user data that is passed to the stage functions.
     *
     *  @param stage2   (optional) stage 2 function (NRT thread). If the function has been omitted
     *                  or returns \c true, execution continues with `stage3`, otherwise it jumps
     *                  straight to `cleanup`.
     *
     *  @param stage3   (optional) stage 3 function (RT thread). If the function has been omitted
     *                  or returns \c true, execution continues with `stage4`, otherwise it jumps
     *                  straight to `cleanup`. The (optional) completition message is performed
     *                  before continuing with `stage4`, but only if the Unit's Graph is still alive.
     *
     *  @param stage4   (optional) stage 4 function (NRT thread). A \c /done message is sent with
     *                  `cmdName` as the first argument if the following conditions are all true:
     *                  - `stage4` is \c NULL or returns \c true
     *                  - `cmdName` is not \c NULL
     *                  - the Unit's Graph is still alive.
     *
     *  @param cleanup  (optional) cleanup function for releasing resources on the RT thread.
     *
     *  @param completionMsgSize    size of the completion message in bytes. Pass 0 if you want to omit it.
     *
     *  @param completionMsgData    the completion message data. Pass \c NULL if you want to omit it.
     */
    AsyncUnitCmd(Unit* inUnit, const ReplyAddress* inReplyAddress, const char* cmdName, void* cmdData,
                 AsyncUnitStageFn stage2, AsyncUnitStageFn stage3, AsyncUnitStageFn stage4, AsyncFreeFn cleanup,
                 int completionMsgSize, const void* completionMsgData);

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
