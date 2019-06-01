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


#include "SC_SequencedCommand.h"
#include "SC_CoreAudio.h"
#include "SC_Errors.h"
#include "scsynthsend.h"
#include "SC_Prototypes.h"
#include "SC_HiddenWorld.h"
#include "SC_Filesystem.hpp"
#include "SC_StringParser.h"
#include "../../common/SC_SndFileHelpers.hpp"
#include "SC_WorldOptions.h"

const size_t ERR_BUF_SIZE(512);

#define GET_COMPLETION_MSG(msg)                                                                                        \
    mMsgSize = msg.getbsize();                                                                                         \
    if (mMsgSize) {                                                                                                    \
        mMsgData = (char*)World_Alloc(mWorld, mMsgSize);                                                               \
        msg.getb(mMsgData, mMsgSize);                                                                                  \
    }

void PerformCompletionMsg(World* inWorld, OSC_Packet* inPacket);

#define SEND_COMPLETION_MSG                                                                                            \
    if (mMsgSize) {                                                                                                    \
        OSC_Packet packet;                                                                                             \
        packet.mData = mMsgData;                                                                                       \
        packet.mSize = mMsgSize;                                                                                       \
        packet.mIsBundle = false;                                                                                      \
        packet.mReplyAddr = mReplyAddress;                                                                             \
        PacketStatus status = PerformCompletionMsg(mWorld, packet);                                                    \
        if (status == PacketScheduled) {                                                                               \
            mMsgSize = 0;                                                                                              \
            mMsgData = 0;                                                                                              \
        }                                                                                                              \
    }

void SndBuf_Init(SndBuf* buf);
void SndBuf_Init(SndBuf* buf) {
    buf->data = 0;
    buf->channels = 0;
    buf->samples = 0;
    buf->frames = 0;
    buf->mask = 0;
    buf->mask1 = 0;
    buf->coord = 0;
    buf->sndfile = 0;
}

char* allocAndRestrictPath(World* mWorld, const char* inPath, const char* restrictBase);
char* allocAndRestrictPath(World* mWorld, const char* inPath, const char* restrictBase) {
    char strbuf[PATH_MAX];
    int offset = 0;
    int remain = PATH_MAX;

    // Ensure begins with the base
    if (strncmp(inPath, restrictBase, strlen(restrictBase)) != 0) {
        strcpy(strbuf, restrictBase);
        offset = strlen(restrictBase);
        remain -= offset;
        if (inPath[0] != '/' && strbuf[strlen(strbuf) - 1] != '/') {
            strbuf[offset] = '/';
            ++offset;
            --remain;
        }
    }

    // Now copy string, but discard any ".." (which could be benign, but easy to abuse)
    SC_StringParser sp(inPath, '/');
    size_t tokenlen;
    while (!sp.AtEnd()) {
        const char* token = const_cast<char*>(sp.NextToken());
        tokenlen = strlen(token);
        // now add the new token, then a slash, as long as token is neither dodgy nor overflows
        if (strcmp(token, "..") != 0 && remain > tokenlen) {
            strcpy(strbuf + offset, token);
            offset += tokenlen;
            remain -= tokenlen;
            if (!sp.AtEnd()) {
                strbuf[offset] = '/';
                ++offset;
                --remain;
            }
        }
    }

    // Now we can make a long-term home for the string and return it
    char* saferPath = (char*)World_Alloc(mWorld, strlen(strbuf) + 1);
    strcpy(saferPath, strbuf);
    return saferPath;
}

SC_SequencedCommand::SC_SequencedCommand(World* inWorld, ReplyAddress* inReplyAddress):
    mNextStage(1),
    mWorld(inWorld),
    mMsgSize(0),
    mMsgData(0) {
    if (inReplyAddress)
        mReplyAddress = *inReplyAddress;
    else
        mReplyAddress.mReplyFunc = null_reply_func;
}

SC_SequencedCommand::~SC_SequencedCommand() {
    if (mMsgData)
        World_Free(mWorld, mMsgData);
}

int SC_SequencedCommand::Init(char* /*inData*/, int /*inSize*/) { return kSCErr_None; }

void SC_SequencedCommand::SendDone(const char* inCommandName) { ::SendDone(&mReplyAddress, inCommandName); };

void SC_SequencedCommand::SendDoneWithIntValue(const char* inCommandName, int value) {
    ::SendDoneWithIntValue(&mReplyAddress, inCommandName, value);
};

void SC_SequencedCommand::CallEveryStage() {
    switch (mNextStage) {
    case 1:
        if (!Stage1())
            break;
        mNextStage++;
    case 2:
        if (!Stage2())
            break;
        mNextStage++;
    case 3:
        if (!Stage3())
            break;
        mNextStage++;
    case 4:
        Stage4();
        break;
    }
    Delete();
}

void DoSequencedCommand(FifoMsg* inMsg);
void DoSequencedCommand(FifoMsg* inMsg) {
    SC_SequencedCommand* cmd = (SC_SequencedCommand*)inMsg->mData;
    cmd->CallNextStage();
}

void FreeSequencedCommand(FifoMsg* inMsg);
void FreeSequencedCommand(FifoMsg* inMsg) {
    SC_SequencedCommand* cmd = (SC_SequencedCommand*)inMsg->mData;
    cmd->Delete();
}


void SC_SequencedCommand::CallNextStage() {
    bool sendAgain = false;
    FifoMsg msg;

    int isRealTime = mNextStage & 1;
    switch (mNextStage) {
    case 1:
        sendAgain = Stage1(); // RT
        break;
    case 2:
        sendAgain = Stage2(); // NRT
        break;
    case 3:
        sendAgain = Stage3(); // RT
        break;
    case 4:
        Stage4(); // NRT
        break;
    }
    mNextStage++;
    SC_AudioDriver* driver = AudioDriver(mWorld);
    if (sendAgain) {
        msg.Set(mWorld, DoSequencedCommand, 0, (void*)this);
        // send this to next time.
        if (isRealTime) {
            // send to NRT
            driver->SendMsgFromEngine(msg);
        } else {
            // send to RT
            driver->SendMsgToEngine(msg);
        }
    } else {
        if (isRealTime) {
            Delete();
        } else {
            // can only be freed from RT.
            msg.Set(mWorld, FreeSequencedCommand, 0, (void*)this);
            driver->SendMsgToEngine(msg);
        }
    }
}

void SC_SequencedCommand::Delete() {
    CallDestructor();
    World_Free(mWorld, this);
}

bool SC_SequencedCommand::Stage1() { return true; }

bool SC_SequencedCommand::Stage2() { return false; }

bool SC_SequencedCommand::Stage3() { return false; }

void SC_SequencedCommand::Stage4() {}


///////////////////////////////////////////////////////////////////////////

#include "sc_msg_iter.h"
#include <string.h>

SyncCmd::SyncCmd(World* inWorld, ReplyAddress* inReplyAddress): SC_SequencedCommand(inWorld, inReplyAddress) {}

int SyncCmd::Init(char* inData, int inSize) {
    sc_msg_iter msg(inSize, inData);
    mID = msg.geti();
    return kSCErr_None;
}

void SyncCmd::CallDestructor() { this->~SyncCmd(); }

bool SyncCmd::Stage2() { return true; }

bool SyncCmd::Stage3() { return true; }

void SyncCmd::Stage4() {
    small_scpacket packet;
    packet.adds("/synced");
    packet.maketags(2);
    packet.addtag(',');
    packet.addtag('i');

    packet.addi(mID);

    SendReply(&mReplyAddress, packet.data(), packet.size());
}

///////////////////////////////////////////////////////////////////////////

BufAllocCmd::BufAllocCmd(World* inWorld, ReplyAddress* inReplyAddress): SC_SequencedCommand(inWorld, inReplyAddress) {}

int BufAllocCmd::Init(char* inData, int inSize) {
    sc_msg_iter msg(inSize, inData);
    mBufIndex = msg.geti();
    mNumFrames = msg.geti();
    mNumChannels = msg.geti(1);

    GET_COMPLETION_MSG(msg);

    return kSCErr_None;
}

void BufAllocCmd::CallDestructor() { this->~BufAllocCmd(); }

bool BufAllocCmd::Stage2() {
    SndBuf* buf = World_GetNRTBuf(mWorld, mBufIndex);
    mFreeData = buf->data;
    bufAlloc(buf, mNumChannels, mNumFrames, mWorld->mFullRate.mSampleRate);
    mSndBuf = *buf;
    return true;
}

bool BufAllocCmd::Stage3() {
    SndBuf* buf = World_GetBuf(mWorld, mBufIndex);
    *buf = mSndBuf;
    mWorld->mSndBufUpdates[mBufIndex].writes++;
    SEND_COMPLETION_MSG;
    return true;
}

void BufAllocCmd::Stage4() {
    zfree(mFreeData);
    SendDoneWithIntValue("/b_alloc", mBufIndex);
}

///////////////////////////////////////////////////////////////////////////

#include "sc_msg_iter.h"
#include <string.h>

BufGenCmd::BufGenCmd(World* inWorld, ReplyAddress* inReplyAddress):
    SC_SequencedCommand(inWorld, inReplyAddress),
    mData(0) {}

BufGenCmd::~BufGenCmd() { World_Free(mWorld, mData); }

int BufGenCmd::Init(char* inData, int inSize) {
    mSize = inSize;
    mData = (char*)World_Alloc(mWorld, mSize);
    memcpy(mData, inData, mSize);

    sc_msg_iter msg(mSize, mData);
    mBufIndex = msg.geti();

    int32* genName = msg.gets4();
    if (!genName)
        return kSCErr_WrongArgType;

    mBufGen = GetBufGen(genName);
    if (!mBufGen)
        return kSCErr_BufGenNotFound;

    mMsg = msg;

    return kSCErr_None;
}

void BufGenCmd::CallDestructor() { this->~BufGenCmd(); }

bool BufGenCmd::Stage2() {
    SndBuf* buf = World_GetNRTBuf(mWorld, mBufIndex);

    mFreeData = buf->data;
    (*mBufGen->mBufGenFunc)(mWorld, buf, &mMsg);
    if (buf->data == mFreeData)
        mFreeData = NULL;
    mSndBuf = *buf;
    return true;
}

bool BufGenCmd::Stage3() {
    SndBuf* buf = World_GetBuf(mWorld, mBufIndex);
    *buf = mSndBuf;
    mWorld->mSndBufUpdates[mBufIndex].writes++;
    return true;
}

void BufGenCmd::Stage4() {
    zfree(mFreeData);
    SendDoneWithIntValue("/b_gen", mBufIndex);
}


///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////


BufFreeCmd::BufFreeCmd(World* inWorld, ReplyAddress* inReplyAddress): SC_SequencedCommand(inWorld, inReplyAddress) {}

int BufFreeCmd::Init(char* inData, int inSize) {
    sc_msg_iter msg(inSize, inData);
    mBufIndex = msg.geti();

    GET_COMPLETION_MSG(msg);

    return kSCErr_None;
}

void BufFreeCmd::CallDestructor() { this->~BufFreeCmd(); }

bool BufFreeCmd::Stage2() {
    SndBuf* buf = World_GetNRTBuf(mWorld, mBufIndex);
    mFreeData = buf->data;
#ifndef NO_LIBSNDFILE
    if (buf->sndfile)
        sf_close(buf->sndfile);
#endif
    SndBuf_Init(buf);
    return true;
}

bool BufFreeCmd::Stage3() {
    SndBuf* buf = World_GetBuf(mWorld, mBufIndex);

    SndBuf_Init(buf);
    mWorld->mSndBufUpdates[mBufIndex].writes++;
    SEND_COMPLETION_MSG;

    return true;
}

void BufFreeCmd::Stage4() {
    zfree(mFreeData);
    SendDoneWithIntValue("/b_free", mBufIndex);
}

///////////////////////////////////////////////////////////////////////////

BufZeroCmd::BufZeroCmd(World* inWorld, ReplyAddress* inReplyAddress): SC_SequencedCommand(inWorld, inReplyAddress) {}

int BufZeroCmd::Init(char* inData, int inSize) {
    sc_msg_iter msg(inSize, inData);
    mBufIndex = msg.geti();

    GET_COMPLETION_MSG(msg);

    return kSCErr_None;
}

void BufZeroCmd::CallDestructor() { this->~BufZeroCmd(); }

bool BufZeroCmd::Stage2() {
    SndBuf* buf = World_GetNRTBuf(mWorld, mBufIndex);
    memset(buf->data, 0, buf->samples * sizeof(float));
    return true;
}

bool BufZeroCmd::Stage3() {
    mWorld->mSndBufUpdates[mBufIndex].writes++;
    SEND_COMPLETION_MSG;
    return true;
}

void BufZeroCmd::Stage4() { SendDoneWithIntValue("/b_zero", mBufIndex); }

///////////////////////////////////////////////////////////////////////////

BufAllocReadCmd::BufAllocReadCmd(World* inWorld, ReplyAddress* inReplyAddress):
    SC_SequencedCommand(inWorld, inReplyAddress),
    mFreeData(0),
    mFilename(0) {}

int BufAllocReadCmd::Init(char* inData, int inSize) {
    sc_msg_iter msg(inSize, inData);
    mBufIndex = msg.geti();

    const char* filename = msg.gets();
    if (!filename)
        return kSCErr_WrongArgType;

    if (mWorld->mRestrictedPath) {
        mFilename = allocAndRestrictPath(mWorld, filename, mWorld->mRestrictedPath);
    } else {
        mFilename = (char*)World_Alloc(mWorld, strlen(filename) + 1);
        strcpy(mFilename, filename);
    }

    mFileOffset = msg.geti();
    mNumFrames = msg.geti();

    GET_COMPLETION_MSG(msg);

    return kSCErr_None;
}

BufAllocReadCmd::~BufAllocReadCmd() { World_Free(mWorld, mFilename); }

void BufAllocReadCmd::CallDestructor() { this->~BufAllocReadCmd(); }

bool BufAllocReadCmd::Stage2() {
#ifdef NO_LIBSNDFILE
    SendFailure(&mReplyAddress, "/b_allocRead", "scsynth compiled without libsndfile\n");
    scprintf("scsynth compiled without libsndfile\n");
    return false;
#else
    SndBuf* buf = World_GetNRTBuf(mWorld, mBufIndex);
    SF_INFO fileinfo;
    memset(&fileinfo, 0, sizeof(fileinfo));
    SNDFILE* sf = sndfileOpenFromCStr(mFilename, SFM_READ, &fileinfo);
    if (!sf) {
        char str[ERR_BUF_SIZE];
        snprintf(str, ERR_BUF_SIZE, "File '%s' could not be opened: %s\n", mFilename, sf_strerror(NULL));
        SendFailureWithIntValue(&mReplyAddress, "/b_allocRead", str,
                                mBufIndex); // SendFailure(&mReplyAddress, "/b_allocRead", str);
        scprintf(str);
        return false;
    }
    if (mFileOffset < 0)
        mFileOffset = 0;
    else if (mFileOffset > fileinfo.frames)
        mFileOffset = fileinfo.frames;
    if (mNumFrames <= 0 || mNumFrames + mFileOffset > fileinfo.frames)
        mNumFrames = fileinfo.frames - mFileOffset;

    // alloc data size
    mFreeData = buf->data;
    SCErr err = bufAlloc(buf, fileinfo.channels, mNumFrames, fileinfo.samplerate);
    if (err)
        goto leave;

    sf_seek(sf, mFileOffset, SEEK_SET);
    sf_readf_float(sf, buf->data, mNumFrames);

leave:
    mSndBuf = *buf;
    sf_close(sf);

    return true;
#endif
}

bool BufAllocReadCmd::Stage3() {
    SndBuf* buf = World_GetBuf(mWorld, mBufIndex);
    *buf = mSndBuf;
    mWorld->mSndBufUpdates[mBufIndex].writes++;
    SEND_COMPLETION_MSG;

    return true;
}

void BufAllocReadCmd::Stage4() {
    zfree(mFreeData);
    SendDoneWithIntValue("/b_allocRead", mBufIndex);
}

///////////////////////////////////////////////////////////////////////////

BufReadCmd::BufReadCmd(World* inWorld, ReplyAddress* inReplyAddress):
    SC_SequencedCommand(inWorld, inReplyAddress),
    mFilename(0) {}

int BufReadCmd::Init(char* inData, int inSize) {
    sc_msg_iter msg(inSize, inData);
    mBufIndex = msg.geti();

    const char* filename = msg.gets();
    if (!filename)
        return kSCErr_WrongArgType;

    if (mWorld->mRestrictedPath) {
        mFilename = allocAndRestrictPath(mWorld, filename, mWorld->mRestrictedPath);
    } else {
        mFilename = (char*)World_Alloc(mWorld, strlen(filename) + 1);
        strcpy(mFilename, filename);
    }

    mFileOffset = msg.geti();
    mNumFrames = msg.geti(-1);
    mBufOffset = msg.geti();
    mLeaveFileOpen = msg.geti();

    GET_COMPLETION_MSG(msg);

    return kSCErr_None;
}

BufReadCmd::~BufReadCmd() { World_Free(mWorld, mFilename); }

void BufReadCmd::CallDestructor() { this->~BufReadCmd(); }

bool BufReadCmd::Stage2() {
#ifdef NO_LIBSNDFILE
    SendFailure(&mReplyAddress, "/b_read", "scsynth compiled without libsndfile\n");
    scprintf("scsynth compiled without libsndfile\n");
    return false;
#else
    SF_INFO fileinfo;
    memset(&fileinfo, 0, sizeof(fileinfo));

    SndBuf* buf = World_GetNRTBuf(mWorld, mBufIndex);
    int framesToEnd = buf->frames - mBufOffset;
    if (framesToEnd <= 0)
        return true;

    SNDFILE* sf = sndfileOpenFromCStr(mFilename, SFM_READ, &fileinfo);
    if (!sf) {
        char str[ERR_BUF_SIZE];
        snprintf(str, ERR_BUF_SIZE, "File '%s' could not be opened: %s\n", mFilename, sf_strerror(NULL));
        SendFailureWithIntValue(&mReplyAddress, "/b_read", str,
                                mBufIndex); // SendFailure(&mReplyAddress, "/b_read", str);
        scprintf(str);
        return false;
    }
    if (fileinfo.channels != buf->channels) {
        char str[ERR_BUF_SIZE];
        sf_close(sf);
        snprintf(str, ERR_BUF_SIZE, "Channel mismatch. File '%s' has %d channels. Buffer has %d channels.\n", mFilename,
                 fileinfo.channels, buf->channels);
        SendFailureWithIntValue(&mReplyAddress, "/b_read", str,
                                mBufIndex); // SendFailure(&mReplyAddress, "/b_read", str);
        scprintf(str);
        return false;
    }

    if (mFileOffset < 0)
        mFileOffset = 0;
    else if (mFileOffset > fileinfo.frames)
        mFileOffset = fileinfo.frames;
    if (mNumFrames < 0 || mNumFrames + mFileOffset > fileinfo.frames)
        mNumFrames = fileinfo.frames - mFileOffset;

    if (mNumFrames > framesToEnd)
        mNumFrames = framesToEnd;

    sf_seek(sf, mFileOffset, SEEK_SET);
    if (mNumFrames > 0) {
        sf_readf_float(sf, buf->data + (mBufOffset * buf->channels), mNumFrames);
    }

    if (buf->sndfile)
        sf_close(buf->sndfile);

    if (mLeaveFileOpen) {
        buf->sndfile = sf;
    } else {
        sf_close(sf);
        buf->sndfile = 0;
    }

    mSampleRate = (double)fileinfo.samplerate;

    return true;
#endif
}

bool BufReadCmd::Stage3() {
    SndBuf* buf = World_GetBuf(mWorld, mBufIndex);
    buf->samplerate = mSampleRate;
    if (mLeaveFileOpen)
        buf->mask = buf->mask1 = -1;

    mWorld->mSndBufUpdates[mBufIndex].writes++;
    SEND_COMPLETION_MSG;
    return true;
}

void BufReadCmd::Stage4() { SendDoneWithIntValue("/b_read", mBufIndex); }

///////////////////////////////////////////////////////////////////////////

SC_BufReadCommand::SC_BufReadCommand(World* inWorld, ReplyAddress* inReplyAddress):
    SC_SequencedCommand(inWorld, inReplyAddress),
    mNumChannels(0) {}

SC_BufReadCommand::~SC_BufReadCommand() {}

void SC_BufReadCommand::InitChannels(sc_msg_iter& msg) {
    mNumChannels = 0;
    while (msg.nextTag(0) == 'i') {
        int c = msg.geti();
        if (mNumChannels <= kMaxNumChannels) {
            mChannels[mNumChannels++] = c;
        }
    }
}

bool SC_BufReadCommand::CheckChannels(int inNumChannels) {
    for (int i = 0; i < mNumChannels; ++i) {
        if (mChannels[i] >= inNumChannels) {
            return false;
        }
    }
    return true;
}

void SC_BufReadCommand::CopyChannels(float* dst, float* src, size_t srcChannels, size_t numFrames) {
    for (int ci = 0; ci < mNumChannels; ++ci) {
        int c = mChannels[ci];
        if (c >= 0 && c < srcChannels) {
            for (size_t fi = 0; fi < numFrames; ++fi) {
                dst[fi * mNumChannels + ci] = src[fi * srcChannels + c];
            }
        } else {
            for (size_t fi = 0; fi < numFrames; ++fi) {
                dst[fi * mNumChannels + ci] = 0.f;
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////

BufAllocReadChannelCmd::BufAllocReadChannelCmd(World* inWorld, ReplyAddress* inReplyAddress):
    SC_BufReadCommand(inWorld, inReplyAddress),
    mFreeData(0),
    mFilename(0) {}

int BufAllocReadChannelCmd::Init(char* inData, int inSize) {
    sc_msg_iter msg(inSize, inData);
    mBufIndex = msg.geti();

    const char* filename = msg.gets();
    if (!filename)
        return kSCErr_WrongArgType;

    if (mWorld->mRestrictedPath) {
        mFilename = allocAndRestrictPath(mWorld, filename, mWorld->mRestrictedPath);
    } else {
        mFilename = (char*)World_Alloc(mWorld, strlen(filename) + 1);
        strcpy(mFilename, filename);
    }

    mFileOffset = msg.geti();
    mNumFrames = msg.geti();

    InitChannels(msg);

    GET_COMPLETION_MSG(msg);

    return kSCErr_None;
}

BufAllocReadChannelCmd::~BufAllocReadChannelCmd() { World_Free(mWorld, mFilename); }

void BufAllocReadChannelCmd::CallDestructor() { this->~BufAllocReadChannelCmd(); }

bool BufAllocReadChannelCmd::Stage2() {
#ifdef NO_LIBSNDFILE
    SendFailure(&mReplyAddress, "/b_allocReadChannel", "scsynth compiled without libsndfile\n");
    scprintf("scsynth compiled without libsndfile\n");
    return false;
#else
    SndBuf* buf = World_GetNRTBuf(mWorld, mBufIndex);

    SF_INFO fileinfo;
    memset(&fileinfo, 0, sizeof(fileinfo));
    SNDFILE* sf = sndfileOpenFromCStr(mFilename, SFM_READ, &fileinfo);
    if (!sf) {
        char str[ERR_BUF_SIZE];
        snprintf(str, ERR_BUF_SIZE, "File '%s' could not be opened: %s\n", mFilename, sf_strerror(NULL));
        SendFailureWithIntValue(&mReplyAddress, "/b_allocReadChannel", str,
                                mBufIndex); // SendFailure(&mReplyAddress, "/b_allocRead", str);
        scprintf(str);
        return false;
    }
    if (mFileOffset < 0)
        mFileOffset = 0;
    else if (mFileOffset > fileinfo.frames)
        mFileOffset = fileinfo.frames;
    if (mNumFrames <= 0 || mNumFrames + mFileOffset > fileinfo.frames)
        mNumFrames = fileinfo.frames - mFileOffset;

    if (mNumChannels == 0) {
        // alloc data size
        mFreeData = buf->data;
        SCErr err = bufAlloc(buf, fileinfo.channels, mNumFrames, fileinfo.samplerate);
        if (err)
            goto leave;
        // read all channels
        sf_seek(sf, mFileOffset, SEEK_SET);
        sf_readf_float(sf, buf->data, mNumFrames);
    } else {
        // verify channel indexes
        if (!CheckChannels(fileinfo.channels)) {
            const char* str = "Channel index out of range.\n";
            SendFailureWithIntValue(&mReplyAddress, "/b_allocReadChannel", str,
                                    mBufIndex); // SendFailure(&mReplyAddress, "/b_allocRead", str);
            scprintf(str);
            sf_close(sf);
            return false;
        }
        // alloc data size
        mFreeData = buf->data;
        SCErr err = bufAlloc(buf, mNumChannels, mNumFrames, fileinfo.samplerate);
        if (err)
            goto leave;
        // alloc temp buffer
        float* data = (float*)malloc(mNumFrames * fileinfo.channels * sizeof(float));
        if (data == 0)
            goto leave;
        // read some channels
        sf_seek(sf, mFileOffset, SEEK_SET);
        sf_readf_float(sf, data, mNumFrames);
        CopyChannels(buf->data, data, fileinfo.channels, mNumFrames);
        // free temp buffer
        free(data);
    }

leave:
    mSndBuf = *buf;
    sf_close(sf);

    return true;
#endif
}

bool BufAllocReadChannelCmd::Stage3() {
    SndBuf* buf = World_GetBuf(mWorld, mBufIndex);
    *buf = mSndBuf;
    mWorld->mSndBufUpdates[mBufIndex].writes++;
    SEND_COMPLETION_MSG;

    return true;
}

void BufAllocReadChannelCmd::Stage4() {
    zfree(mFreeData);
    SendDoneWithIntValue("/b_allocReadChannel", mBufIndex);
}

///////////////////////////////////////////////////////////////////////////

BufReadChannelCmd::BufReadChannelCmd(World* inWorld, ReplyAddress* inReplyAddress):
    SC_BufReadCommand(inWorld, inReplyAddress),
    mFilename(0) {}

int BufReadChannelCmd::Init(char* inData, int inSize) {
    sc_msg_iter msg(inSize, inData);
    mBufIndex = msg.geti();

    const char* filename = msg.gets();
    if (!filename)
        return kSCErr_WrongArgType;

    if (mWorld->mRestrictedPath) {
        mFilename = allocAndRestrictPath(mWorld, filename, mWorld->mRestrictedPath);
    } else {
        mFilename = (char*)World_Alloc(mWorld, strlen(filename) + 1);
        strcpy(mFilename, filename);
    }

    mFileOffset = msg.geti();
    mNumFrames = msg.geti(-1);
    mBufOffset = msg.geti();
    mLeaveFileOpen = msg.geti();

    InitChannels(msg);

    GET_COMPLETION_MSG(msg);

    return kSCErr_None;
}

BufReadChannelCmd::~BufReadChannelCmd() { World_Free(mWorld, mFilename); }

void BufReadChannelCmd::CallDestructor() { this->~BufReadChannelCmd(); }

bool BufReadChannelCmd::Stage2() {
#ifdef NO_LIBSNDFILE
    SendFailure(&mReplyAddress, "/b_readChannel", "scsynth compiled without libsndfile\n");
    scprintf("scsynth compiled without libsndfile\n");
    return false;
#else
    SF_INFO fileinfo;
    memset(&fileinfo, 0, sizeof(fileinfo));

    SndBuf* buf = World_GetNRTBuf(mWorld, mBufIndex);
    int framesToEnd = buf->frames - mBufOffset;
    if (framesToEnd <= 0)
        return true;

    SNDFILE* sf = sndfileOpenFromCStr(mFilename, SFM_READ, &fileinfo);
    if (!sf) {
        char str[ERR_BUF_SIZE];
        snprintf(str, ERR_BUF_SIZE, "File '%s' could not be opened: %s\n", mFilename, sf_strerror(NULL));
        SendFailureWithIntValue(&mReplyAddress, "/b_readChannel", str,
                                mBufIndex); // SendFailure(&mReplyAddress, "/b_read", str);
        scprintf(str);
        return false;
    }

    if (mNumChannels > 0) {
        // verify channel indexes
        if (!(CheckChannels(fileinfo.channels))) { // nescivi:  && CheckChannels(buf->channels) (should not check here
                                                   // for buf->channels)
            const char* str = "Channel index out of range.\n";
            SendFailureWithIntValue(&mReplyAddress, "/b_readChannel", str,
                                    mBufIndex); // SendFailure(&mReplyAddress, "/b_allocRead", str);
            scprintf(str);
            sf_close(sf);
            return false;
        }
        // nescivi: this also seems out of place: we want to read from a file with more channels than are in the buffer
        // 	} else if (fileinfo.channels != buf->channels) {
        // 		char str[256];
        // 		sf_close(sf);
        // 		sprintf(str, "Channel mismatch. File '%s' has %d channels. Buffer has %d channels.\n",
        // 				mFilename, fileinfo.channels, buf->channels);
        // 		SendFailure(&mReplyAddress, "/b_read", str);
        // 		scprintf(str);
        // 		return false;
    }

    if (mFileOffset < 0)
        mFileOffset = 0;
    else if (mFileOffset > fileinfo.frames)
        mFileOffset = fileinfo.frames;
    if (mNumFrames < 0 || mNumFrames + mFileOffset > fileinfo.frames)
        mNumFrames = fileinfo.frames - mFileOffset;
    if (mNumFrames > framesToEnd)
        mNumFrames = framesToEnd;

    sf_seek(sf, mFileOffset, SEEK_SET);
    if (mNumFrames > 0) {
        if (mNumChannels == 0) {
            // read all channels
            sf_readf_float(sf, buf->data + (mBufOffset * buf->channels), mNumFrames);
        } else {
            // alloc temp buffer
            float* data = (float*)malloc(mNumFrames * fileinfo.channels * sizeof(float));
            if (data == 0)
                goto leave;
            // read some channels
            sf_seek(sf, mFileOffset, SEEK_SET);
            sf_readf_float(sf, data, mNumFrames);
            CopyChannels(buf->data + (mBufOffset * mNumChannels), data, fileinfo.channels, mNumFrames);
            // free temp buffer
            free(data);
        }
    }

leave:
    if (buf->sndfile)
        sf_close(buf->sndfile);

    if (mLeaveFileOpen) {
        buf->sndfile = sf;
    } else {
        sf_close(sf);
        buf->sndfile = 0;
    }

    mSampleRate = (double)fileinfo.samplerate;

    return true;
#endif
}

bool BufReadChannelCmd::Stage3() {
    SndBuf* buf = World_GetBuf(mWorld, mBufIndex);
    buf->samplerate = mSampleRate;
    if (mLeaveFileOpen)
        buf->mask = buf->mask1 = -1;

    mWorld->mSndBufUpdates[mBufIndex].writes++;
    SEND_COMPLETION_MSG;
    return true;
}

void BufReadChannelCmd::Stage4() { SendDoneWithIntValue("/b_readChannel", mBufIndex); }

///////////////////////////////////////////////////////////////////////////

BufWriteCmd::BufWriteCmd(World* inWorld, ReplyAddress* inReplyAddress):
    SC_SequencedCommand(inWorld, inReplyAddress),
    mFilename(0) {}

#ifdef NO_LIBSNDFILE
struct SF_INFO {};
#endif

int BufWriteCmd::Init(char* inData, int inSize) {
#ifdef NO_LIBSNDFILE
    SendFailure(&mReplyAddress, "/b_write", "scsynth compiled without libsndfile\n");
    scprintf("scsynth compiled without libsndfile\n");
    return false;
#else
    sc_msg_iter msg(inSize, inData);
    mBufIndex = msg.geti();

    const char* filename = msg.gets();
    if (!filename)
        return kSCErr_WrongArgType;

    if (mWorld->mRestrictedPath) {
        mFilename = allocAndRestrictPath(mWorld, filename, mWorld->mRestrictedPath);
    } else {
        mFilename = (char*)World_Alloc(mWorld, strlen(filename) + 1);
        strcpy(mFilename, filename);
    }

    const char* headerFormatString = msg.gets("aiff");
    const char* sampleFormatString = msg.gets("int16");

    mNumFrames = msg.geti(-1);
    mBufOffset = msg.geti();
    mLeaveFileOpen = msg.geti();

    GET_COMPLETION_MSG(msg);

    memset(&mFileInfo, 0, sizeof(mFileInfo));
    return sndfileFormatInfoFromStrings(&mFileInfo, headerFormatString, sampleFormatString);
#endif
}

BufWriteCmd::~BufWriteCmd() { World_Free(mWorld, mFilename); }

void BufWriteCmd::CallDestructor() { this->~BufWriteCmd(); }

bool BufWriteCmd::Stage2() {
#ifdef NO_LIBSNDFILE
    return false;
#else
    SndBuf* buf = World_GetNRTBuf(mWorld, mBufIndex);
    int framesToEnd = buf->frames - mBufOffset;
    if (framesToEnd < 0)
        framesToEnd = 0;
    mFileInfo.samplerate = (int)buf->samplerate;
    mFileInfo.channels = buf->channels;

    SNDFILE* sf = sndfileOpenFromCStr(mFilename, SFM_WRITE, &mFileInfo);
    if (!sf) {
        char str[ERR_BUF_SIZE];
        snprintf(str, ERR_BUF_SIZE, "File '%s' could not be opened: %s\n", mFilename, sf_strerror(NULL));
        SendFailureWithIntValue(&mReplyAddress, "/b_write", str,
                                mBufIndex); // SendFailure(&mReplyAddress, "/b_write", str);
        scprintf(str);
        return false;
    }

    if (mNumFrames < 0 || mNumFrames > buf->frames)
        mNumFrames = buf->frames;

    if (mNumFrames > framesToEnd)
        mNumFrames = framesToEnd;

    sf_command(sf, SFC_SET_CLIPPING, NULL, SF_TRUE); // choose clipping rather than wraparound for integer-format files

    if (mNumFrames > 0) {
        sf_writef_float(sf, buf->data + (mBufOffset * buf->channels), mNumFrames);
    }

    if (buf->sndfile)
        sf_close(buf->sndfile);

    if (mLeaveFileOpen) {
        buf->sndfile = sf;
    } else {
        sf_close(sf);
        buf->sndfile = 0;
    }

    return true;
#endif
}

bool BufWriteCmd::Stage3() {
    SEND_COMPLETION_MSG;
    return true;
}

void BufWriteCmd::Stage4() { SendDoneWithIntValue("/b_write", mBufIndex); }

///////////////////////////////////////////////////////////////////////////

BufCloseCmd::BufCloseCmd(World* inWorld, ReplyAddress* inReplyAddress): SC_SequencedCommand(inWorld, inReplyAddress) {}

int BufCloseCmd::Init(char* inData, int inSize) {
    sc_msg_iter msg(inSize, inData);
    mBufIndex = msg.geti();

    GET_COMPLETION_MSG(msg);

    return kSCErr_None;
}

void BufCloseCmd::CallDestructor() { this->~BufCloseCmd(); }

bool BufCloseCmd::Stage2() {
#ifdef NO_LIBSNDFILE
    SendFailure(&mReplyAddress, "/b_close", "scsynth compiled without libsndfile\n");
    scprintf("scsynth compiled without libsndfile\n");
    return false;
#else
    SndBuf* buf = World_GetNRTBuf(mWorld, mBufIndex);
    if (buf->sndfile) {
        sf_close(buf->sndfile);
        buf->sndfile = 0;
    }
    return true;
#endif
}

bool BufCloseCmd::Stage3() {
    SEND_COMPLETION_MSG;
    return true;
}

void BufCloseCmd::Stage4() { SendDoneWithIntValue("/b_close", mBufIndex); }

///////////////////////////////////////////////////////////////////////////

AudioQuitCmd::AudioQuitCmd(World* inWorld, ReplyAddress* inReplyAddress):
    SC_SequencedCommand(inWorld, inReplyAddress) {}

void AudioQuitCmd::CallDestructor() { this->~AudioQuitCmd(); }

bool AudioQuitCmd::Stage2() {
    mWorld->hw->mTerminating = true;
    return true;
}

bool AudioQuitCmd::Stage3() {
#if SC_AUDIO_API == SC_AUDIO_API_AUDIOUNITS
    SendFailure(&mReplyAddress, "/quit", "not allowed in AU host\n");
    scprintf("/quit : quit not allowed in AU host\n");
    return false;
#else
    if (mWorld->hw->mShmem)
        mWorld->hw->mShmem->disconnect();
    return true;
#endif
}

void AudioQuitCmd::Stage4() {
    SendDone("/quit");
    mWorld->hw->mQuitProgram->post();
}

///////////////////////////////////////////////////////////////////////////

AudioStatusCmd::AudioStatusCmd(World* inWorld, ReplyAddress* inReplyAddress):
    SC_SequencedCommand(inWorld, inReplyAddress) {}

void AudioStatusCmd::CallDestructor() { this->~AudioStatusCmd(); }

bool AudioStatusCmd::Stage2() {
    // we stop replying to status requests after receiving /quit
    if (mWorld->hw->mTerminating == true)
        return false;

    small_scpacket packet;
    packet.adds("/status.reply");
    packet.maketags(10);
    packet.addtag(',');
    packet.addtag('i');
    packet.addtag('i');
    packet.addtag('i');
    packet.addtag('i');
    packet.addtag('i');
    packet.addtag('f');
    packet.addtag('f');
    packet.addtag('d');
    packet.addtag('d');

    packet.addi(1); // audio is always active now.
    packet.addi(mWorld->mNumUnits);
    packet.addi(mWorld->mNumGraphs);
    packet.addi(mWorld->mNumGroups);
    packet.addi(mWorld->hw->mGraphDefLib->NumItems());

    SC_AudioDriver* driver = mWorld->hw->mAudioDriver;
    packet.addf(driver->GetAvgCPU());
    packet.addf(driver->GetPeakCPU());
    packet.addd(driver->GetSampleRate());
    packet.addd(driver->GetActualSampleRate());

    SendReply(&mReplyAddress, packet.data(), packet.size());

    return false;
}

///////////////////////////////////////////////////////////////////////////

NotifyCmd::NotifyCmd(World* inWorld, ReplyAddress* inReplyAddress): SC_SequencedCommand(inWorld, inReplyAddress) {}

int NotifyCmd::Init(char* inData, int inSize) {
    sc_msg_iter msg(inSize, inData);
    mOnOff = msg.geti();
    mID = msg.geti(-1);

    return kSCErr_None;
}

void NotifyCmd::CallDestructor() { this->~NotifyCmd(); }

/** \brief Attempts to find and remove the requested \c id from \c availableIDs.
 *
 * If \c id is -1 or not in \c availableIDs, returns the first element in
 * \c availableIDs. Otherwise, returns \c id.
 */
int popAvailableClientID(int const id, ClientIDs& availableIDs) {
    int clientID = -1;
    if (id == -1) {
        // no requested clientID
        clientID = availableIDs.front(); // pop an ID
        availableIDs.pop_front();
    } else {
        // user ID requested
        auto it = std::find(availableIDs.begin(), availableIDs.end(), id);
        if (it != availableIDs.end()) { // return the requested ID if available
            clientID = id;
            availableIDs.erase(it);
        } else {
            // otherwise return the first free one
            clientID = availableIDs.front();
            availableIDs.pop_front();
        }
    }

    return clientID;
}

bool NotifyCmd::Stage2() {
    HiddenWorld* hw = mWorld->hw;

    if (mOnOff) {
        for (auto addr : *hw->mUsers) {
            if (mReplyAddress == addr) {
                // already in table - don't fail though..
                SendFailureWithIntValue(&mReplyAddress, "/notify", "notify: already registered\n",
                                        hw->mClientIDdict->at(mReplyAddress));
                scprintf("/notify : already registered\n");
                return false;
            }
        }

        if (hw->mUsers->size() >= hw->mMaxUsers) {
            SendFailure(&mReplyAddress, "/notify", "too many users\n");
            scprintf("too many users\n");
            return false;
        }

        int const clientID = popAvailableClientID(mID, *hw->mAvailableClientIDs);

        hw->mClientIDdict->insert(std::make_pair(mReplyAddress, clientID));
        hw->mUsers->insert(mReplyAddress);
        SendDoneWithVarArgs(&mReplyAddress, "/notify", "ii", clientID, (int)hw->mMaxUsers);

    } else {
        auto const it = std::find(hw->mUsers->begin(), hw->mUsers->end(), mReplyAddress);
        if (it != hw->mUsers->end()) {
            // remove from list
            hw->mAvailableClientIDs->push_back(hw->mClientIDdict->at(mReplyAddress)); // push the freed ID
            hw->mClientIDdict->erase(mReplyAddress);
            hw->mUsers->erase(it);
            SendDone("/notify");
            return false;
        }

        SendFailure(&mReplyAddress, "/notify", "not registered\n");
        scprintf("not registered\n");
    }
    return false;
}

///////////////////////////////////////////////////////////////////////////

SendFailureCmd::SendFailureCmd(World* inWorld, ReplyAddress* inReplyAddress):
    SC_SequencedCommand(inWorld, inReplyAddress),
    mCmdName(0),
    mErrString(0) {}

SendFailureCmd::~SendFailureCmd() {
    World_Free(mWorld, mCmdName);
    World_Free(mWorld, mErrString);
}


void SendFailureCmd::InitSendFailureCmd(const char* inCmdName, const char* inErrString) {
    mCmdName = (char*)World_Alloc(mWorld, strlen(inCmdName) + 1);
    strcpy(mCmdName, inCmdName);

    mErrString = (char*)World_Alloc(mWorld, strlen(inErrString) + 1);
    strcpy(mErrString, inErrString);
}

void SendFailureCmd::CallDestructor() { this->~SendFailureCmd(); }

bool SendFailureCmd::Stage2() {
    SendFailure(&mReplyAddress, mCmdName, mErrString);
    return false;
}

///////////////////////////////////////////////////////////////////////////

RecvSynthDefCmd::RecvSynthDefCmd(World* inWorld, ReplyAddress* inReplyAddress):
    SC_SequencedCommand(inWorld, inReplyAddress),
    mBuffer(0) {}

int RecvSynthDefCmd::Init(char* inData, int inSize) {
    sc_msg_iter msg(inSize, inData);

    int size = msg.getbsize();
    if (!size)
        throw kSCErr_WrongArgType;

    mBuffer = (char*)World_Alloc(mWorld, size);
    msg.getb(mBuffer, size);

    GET_COMPLETION_MSG(msg);

    mDefs = 0;
    return kSCErr_None;
}

RecvSynthDefCmd::~RecvSynthDefCmd() { World_Free(mWorld, mBuffer); }

void RecvSynthDefCmd::CallDestructor() { this->~RecvSynthDefCmd(); }

bool RecvSynthDefCmd::Stage2() {
    mDefs = GraphDef_Recv(mWorld, mBuffer, mDefs);

    return true;
}

bool RecvSynthDefCmd::Stage3() {
    GraphDef_Define(mWorld, mDefs);
    SEND_COMPLETION_MSG;
    return true;
}

void RecvSynthDefCmd::Stage4() { SendDone("/d_recv"); }

///////////////////////////////////////////////////////////////////////////

LoadSynthDefCmd::LoadSynthDefCmd(World* inWorld, ReplyAddress* inReplyAddress):
    SC_SequencedCommand(inWorld, inReplyAddress),
    mFilename(0) {}

int LoadSynthDefCmd::Init(char* inData, int inSize) {
    sc_msg_iter msg(inSize, inData);

    const char* filename = msg.gets();
    if (!filename)
        return kSCErr_WrongArgType;

    if (mWorld->mRestrictedPath) {
        mFilename = allocAndRestrictPath(mWorld, filename, mWorld->mRestrictedPath);
    } else {
        mFilename = (char*)World_Alloc(mWorld, strlen(filename) + 1);
        strcpy(mFilename, filename);
    }

    GET_COMPLETION_MSG(msg);

    mDefs = 0;
    return kSCErr_None;
}

LoadSynthDefCmd::~LoadSynthDefCmd() { World_Free(mWorld, mFilename); }

void LoadSynthDefCmd::CallDestructor() { this->~LoadSynthDefCmd(); }

bool LoadSynthDefCmd::Stage2() {
    char* fname = mFilename;
    mDefs = GraphDef_LoadGlob(mWorld, fname, mDefs);
    return true;
}

bool LoadSynthDefCmd::Stage3() {
    GraphDef_Define(mWorld, mDefs);
    SEND_COMPLETION_MSG;
    return true;
}

void LoadSynthDefCmd::Stage4() { SendDone("/d_load"); }

///////////////////////////////////////////////////////////////////////////

LoadSynthDefDirCmd::LoadSynthDefDirCmd(World* inWorld, ReplyAddress* inReplyAddress):
    SC_SequencedCommand(inWorld, inReplyAddress),
    mFilename(0) {}

int LoadSynthDefDirCmd::Init(char* inData, int inSize) {
    sc_msg_iter msg(inSize, inData);

    const char* filename = msg.gets();
    if (!filename)
        return kSCErr_WrongArgType;

    if (mWorld->mRestrictedPath) {
        mFilename = allocAndRestrictPath(mWorld, filename, mWorld->mRestrictedPath);
    } else {
        mFilename = (char*)World_Alloc(mWorld, strlen(filename) + 1);
        strcpy(mFilename, filename);
    }

    GET_COMPLETION_MSG(msg);

    mDefs = 0;
    return kSCErr_None;
}

LoadSynthDefDirCmd::~LoadSynthDefDirCmd() { World_Free(mWorld, mFilename); }

void LoadSynthDefDirCmd::CallDestructor() { this->~LoadSynthDefDirCmd(); }

bool LoadSynthDefDirCmd::Stage2() {
    mDefs = GraphDef_LoadDir(mWorld, mFilename, mDefs);

    return true;
}

bool LoadSynthDefDirCmd::Stage3() {
    GraphDef_Define(mWorld, mDefs);
    SEND_COMPLETION_MSG;
    return true;
}

void LoadSynthDefDirCmd::Stage4() { SendDone("/d_loadDir"); }

///////////////////////////////////////////////////////////////////////////

SendReplyCmd::SendReplyCmd(World* inWorld, ReplyAddress* inReplyAddress):
    SC_SequencedCommand(inWorld, inReplyAddress) {}

int SendReplyCmd::Init(char* inData, int inSize) {
    mMsgSize = inSize;
    mMsgData = (char*)World_Alloc(mWorld, mMsgSize);
    memcpy(mMsgData, inData, inSize);
    return kSCErr_None;
}

void SendReplyCmd::CallDestructor() { this->~SendReplyCmd(); }

bool SendReplyCmd::Stage2() {
    SendReply(&mReplyAddress, mMsgData, mMsgSize);
    return false;
}

///////////////////////////////////////////////////////////////////////////

int PerformAsynchronousCommand(
    World* inWorld, void* replyAddr, const char* cmdName, void* cmdData,
    AsyncStageFn stage2, // stage2 is non real time
    AsyncStageFn stage3, // stage3 is real time - completion msg performed if stage3 returns true
    AsyncStageFn stage4, // stage4 is non real time - sends done if stage4 returns true
    AsyncFreeFn cleanup, int completionMsgSize, void* completionMsgData) {
    void* space = World_Alloc(inWorld, sizeof(AsyncPlugInCmd));
    AsyncPlugInCmd* cmd = new (space) AsyncPlugInCmd(inWorld, (ReplyAddress*)replyAddr, cmdName, cmdData, stage2,
                                                     stage3, stage4, cleanup, completionMsgSize, completionMsgData);
    if (!cmd)
        return kSCErr_Failed;
    if (inWorld->mRealTime)
        cmd->CallNextStage();
    else
        cmd->CallEveryStage();
    return kSCErr_None;
}

///////////////////////////////////////////////////////////////////////////

AsyncPlugInCmd::AsyncPlugInCmd(
    World* inWorld, ReplyAddress* inReplyAddress, const char* cmdName, void* cmdData,
    AsyncStageFn stage2, // stage2 is non real time
    AsyncStageFn stage3, // stage3 is real time - completion msg performed if stage3 returns true
    AsyncStageFn stage4, // stage4 is non real time - sends done if stage4 returns true
    AsyncFreeFn cleanup, // cleanup is called in real time
    int completionMsgSize, void* completionMsgData):
    SC_SequencedCommand(inWorld, inReplyAddress),
    mCmdName(cmdName),
    mCmdData(cmdData),
    mStage2(stage2),
    mStage3(stage3),
    mStage4(stage4),
    mCleanup(cleanup) {
    if (completionMsgSize && completionMsgData) {
        mMsgSize = completionMsgSize;
        mMsgData = (char*)World_Alloc(mWorld, mMsgSize);
        memcpy(mMsgData, completionMsgData, mMsgSize);
    }
}

AsyncPlugInCmd::~AsyncPlugInCmd() {
    (mCleanup)(mWorld, mCmdData);
    if (mMsgData)
        World_Free(mWorld, mMsgData);
}

void AsyncPlugInCmd::CallDestructor() { this->~AsyncPlugInCmd(); }

bool AsyncPlugInCmd::Stage2() {
    bool result = !mStage2 || (mStage2)(mWorld, mCmdData);
    return result;
}

bool AsyncPlugInCmd::Stage3() {
    bool result = !mStage3 || (mStage3)(mWorld, mCmdData);
    if (result)
        SEND_COMPLETION_MSG;
    return result;
}

void AsyncPlugInCmd::Stage4() {
    bool result = !mStage4 || (mStage4)(mWorld, mCmdData);
    if (result && mCmdName && mReplyAddress.mReplyFunc != null_reply_func)
        SendDone((char*)mCmdName);
}
