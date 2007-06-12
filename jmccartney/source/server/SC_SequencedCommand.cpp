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


#include "SC_SequencedCommand.h"
#include "SC_CoreAudio.h"
#include "SC_Errors.h"
#include "scsynthsend.h"
#include "SC_Prototypes.h"
#include "SC_HiddenWorld.h"
#include "SC_Sem.h"

void SndBuf_Init(SndBuf *buf);
void SndBuf_Init(SndBuf *buf)
{
	buf->data = 0;
	buf->channels = 0;
	buf->frames = 0;
	buf->samples = 0;
	buf->mask = 0;
	buf->mask1 = 0;
	buf->coord = 0;
	buf->shmid = 0;
	buf->shmkey = 0;
}

SC_SequencedCommand::SC_SequencedCommand(World *inWorld, ReplyAddress *inReplyAddress)
	: mNextStage(1), mReplyAddress(*inReplyAddress), mWorld(inWorld)
{
	//printf("SC_SequencedCommand::SC_SequencedCommand\n");
}

SC_SequencedCommand::~SC_SequencedCommand()
{
}

int SC_SequencedCommand::Init(char* /*inData*/, int /*inSize*/)
{
	return kSCErr_None;
}

void SC_SequencedCommand::SendDone(char *inCommandName)
{
	::SendDone(&mReplyAddress, inCommandName);
};

void SC_SequencedCommand::DoCommand()
{
	//printf("DoCommand\n");
	CallNextStage();
}

void SC_SequencedCommand::CallEveryStage()
{
	//printf("CallEveryStage\n");
	switch (mNextStage) {
		case 1 : if (!Stage1()) break; mNextStage++; 
		case 2 : if (!Stage2()) break; mNextStage++; 
		case 3 : if (!Stage3()) break; mNextStage++; 
		case 4 : Stage4(); break;
	}
	Delete();
}

void DoSequencedCommand(FifoMsg *inMsg);
void DoSequencedCommand(FifoMsg *inMsg)
{
	SC_SequencedCommand *cmd = (SC_SequencedCommand*)inMsg->mData;
	cmd->DoCommand();
}

void FreeSequencedCommand(FifoMsg *inMsg);
void FreeSequencedCommand(FifoMsg *inMsg)
{
	SC_SequencedCommand *cmd = (SC_SequencedCommand*)inMsg->mData;
	cmd->Delete();
}


void SC_SequencedCommand::CallNextStage()
{
	//printf("CallNextStage %d\n", mNextStage);
	bool sendAgain = false;
	FifoMsg msg;
	
	int isRealTime = mNextStage & 1;
	switch (mNextStage) {
		case 1 : 
			sendAgain = Stage1(); // RT
			break;
		case 2 : 
			sendAgain = Stage2(); // NRT
			break;
		case 3 : 
			sendAgain = Stage3(); // RT
			break;
		case 4 : 
			Stage4(); 			// NRT
			break;
	} 
	mNextStage++;
	SC_CoreAudioDriver *driver = AudioDriver(mWorld);
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

void SC_SequencedCommand::Delete()
{
	CallDestructor();
	World_Free(mWorld, this);
}

bool SC_SequencedCommand::Stage1()
{
	return true;
}

bool SC_SequencedCommand::Stage2()
{
	return false;
}

bool SC_SequencedCommand::Stage3()
{
	return false;
}

void SC_SequencedCommand::Stage4()
{
}


///////////////////////////////////////////////////////////////////////////

#include "sc_msg_iter.h"
#include <string.h>

BufAllocCmd::BufAllocCmd(World *inWorld, ReplyAddress *inReplyAddress)
	: SC_SequencedCommand(inWorld, inReplyAddress)
{
}

int BufAllocCmd::Init(char *inData, int inSize)
{
	//printf("data %08X size %d\n", inData, inSize);
	sc_msg_iter msg(inSize, inData);
	mBufIndex = msg.geti();
	mNumFrames = msg.geti();
	mNumChannels = msg.geti(1);
	//printf("BufAllocCmd::Init %d %d %d\n", mBufIndex, mNumFrames, mNumChannels);
	return kSCErr_None;
}

void BufAllocCmd::CallDestructor() 
{
	this->~BufAllocCmd();
}

bool BufAllocCmd::Stage2()
{
	SndBuf *buf = World_GetNRTBuf(mWorld, mBufIndex);
	mFreeData = buf->data;
	bufAlloc(buf, mNumChannels, mNumFrames);
	mSndBuf = *buf;
	//printf("BufAllocCmd::Stage2 %08X\n", buf);
	//printf("buf %d %d %d\n", buf->frames, buf->channels, buf->samples);
	return true;
}

bool BufAllocCmd::Stage3()
{
	SndBuf* buf = World_GetBuf(mWorld, mBufIndex);
	*buf = mSndBuf;
	//printf("BufAllocCmd::Stage3 %08X\n", buf);
	return true;
}

void BufAllocCmd::Stage4()
{
	free(mFreeData);
	SendDone("/b_alloc");
}

///////////////////////////////////////////////////////////////////////////

#include "sc_msg_iter.h"
#include <string.h>

BufGenCmd::BufGenCmd(World *inWorld, ReplyAddress *inReplyAddress)
	: SC_SequencedCommand(inWorld, inReplyAddress)
{
}

BufGenCmd::~BufGenCmd()
{
	World_Free(mWorld, mData);
}
	
int BufGenCmd::Init(char *inData, int inSize)
{
	sc_msg_iter msg(inSize, inData);
	mBufIndex = msg.geti();
	
	int32 *genName = msg.gets4();
	mBufGen = GetBufGen(genName);
	if (!mBufGen) return kSCErr_Failed;

	mSize = msg.size;
	mData = (char*)World_Alloc(mWorld, mSize);
	memcpy(mData, msg.data, mSize);
	mMsg = msg;
	
	return kSCErr_None;
}

void BufGenCmd::CallDestructor() 
{
	this->~BufGenCmd();
}

bool BufGenCmd::Stage2()
{
	SndBuf *buf = World_GetNRTBuf(mWorld, mBufIndex);
	mFreeData = buf->data;
	
	(*mBufGen->mBufGenFunc)(buf, &mMsg);
	mSndBuf = *buf;
	return true;
}

bool BufGenCmd::Stage3()
{
	SndBuf* buf = World_GetBuf(mWorld, mBufIndex);
	*buf = mSndBuf;
	return true;
}

void BufGenCmd::Stage4()
{
	free(mFreeData);
	SendDone("/b_gen");
}


///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////


BufFreeCmd::BufFreeCmd(World *inWorld, ReplyAddress *inReplyAddress)
	: SC_SequencedCommand(inWorld, inReplyAddress)
{
}

int BufFreeCmd::Init(char *inData, int inSize)
{
	sc_msg_iter msg(inSize, inData);
	mBufIndex = msg.geti();

	return kSCErr_None;
}

void BufFreeCmd::CallDestructor() 
{
	this->~BufFreeCmd();
}

bool BufFreeCmd::Stage2()
{
	SndBuf *buf = World_GetNRTBuf(mWorld, mBufIndex);
	mFreeData = buf->data;

	SndBuf_Init(buf);
	return true;
}

bool BufFreeCmd::Stage3()
{
	SndBuf *buf = World_GetBuf(mWorld, mBufIndex);
	
	SndBuf_Init(buf);
	
	return true;
}

void BufFreeCmd::Stage4()
{
	free(mFreeData);
	SendDone("/b_free");
}

///////////////////////////////////////////////////////////////////////////

BufZeroCmd::BufZeroCmd(World *inWorld, ReplyAddress *inReplyAddress)
	: SC_SequencedCommand(inWorld, inReplyAddress)
{
}

int BufZeroCmd::Init(char *inData, int inSize)
{
	sc_msg_iter msg(inSize, inData);
	mBufIndex = msg.geti();
	return kSCErr_None;
}

void BufZeroCmd::CallDestructor() 
{
	this->~BufZeroCmd();
}

bool BufZeroCmd::Stage2()
{
	SndBuf *buf = World_GetNRTBuf(mWorld, mBufIndex);
	memset(buf->data, 0, buf->samples * sizeof(float));
	return true;
}

bool BufZeroCmd::Stage3()
{
	return true;
}

void BufZeroCmd::Stage4()
{
	SendDone("/b_zero");
}

///////////////////////////////////////////////////////////////////////////

BufAllocReadCmd::BufAllocReadCmd(World *inWorld, ReplyAddress *inReplyAddress)
	: SC_SequencedCommand(inWorld, inReplyAddress)
{
}

int BufAllocReadCmd::Init(char *inData, int inSize)
{
	sc_msg_iter msg(inSize, inData);
	mBufIndex = msg.geti();
	
	char *filename = msg.gets();
	//printf("filename '%s'\n", filename);
	mFilename = (char*)World_Alloc(mWorld, strlen(filename)+1);
	strcpy(mFilename, filename);
	
	mFileOffset = msg.geti();
	//printf("mFileOffset %d\n", mFileOffset);
	mNumFrames = msg.geti();
	//printf("mNumFrames %d\n", mNumFrames);
	return kSCErr_None;
}

BufAllocReadCmd::~BufAllocReadCmd()
{
	World_Free(mWorld, mFilename);
}

void BufAllocReadCmd::CallDestructor() 
{
	this->~BufAllocReadCmd();
}

bool BufAllocReadCmd::Stage2()
{
	//printf("Stage2 **\n");
	SndBuf *buf = World_GetNRTBuf(mWorld, mBufIndex);
	
	SF_INFO fileinfo;
	memset(&fileinfo, 0, sizeof(fileinfo));
	SNDFILE* sf = sf_open(mFilename, SFM_READ, &fileinfo);
	//printf("sf %08X\n", sf);
	//printf("mFilename '%s'\n", mFilename);
	if (!sf) {
		printf("File '%s' could not be opened.\n", mFilename);
		return true;
	}
	//printf("mNumFrames B %d\n", mNumFrames);
	if (mNumFrames <= 0 || mNumFrames > fileinfo.frames) mNumFrames = fileinfo.frames;
	//printf("mNumFrames C %d %d\n", mNumFrames, fileinfo.frames);
	
	// alloc data size
	mFreeData = buf->data;
	SCErr err = bufAlloc(buf, fileinfo.channels, mNumFrames);
	//printf("bufAlloc err %d\n", err);
	if (err) goto leave;
	//printf("chan %d\n", buf->channels);
	
	sf_seek(sf, mFileOffset, SEEK_SET);
	sf_readf_float(sf, buf->data, mNumFrames);
	
leave:
	mSndBuf = *buf;
	//printf("schanA %d\n", mSndBuf.channels);
	//sf_close(sf);
	
	return true;
}

bool BufAllocReadCmd::Stage3()
{
	SndBuf* buf = World_GetBuf(mWorld, mBufIndex);	
	//printf("schanA %d\n", mSndBuf.channels);
	*buf = mSndBuf;
	//printf("buf %08X %d\n", buf, buf->channels);
	
	return true;
}

void BufAllocReadCmd::Stage4()
{
	//printf("Stage4\n");
	free(mFreeData);
	SendDone("/b_allocRead");
}

///////////////////////////////////////////////////////////////////////////

BufReadCmd::BufReadCmd(World *inWorld, ReplyAddress *inReplyAddress)
	: SC_SequencedCommand(inWorld, inReplyAddress)
{
}

int BufReadCmd::Init(char *inData, int inSize)
{
	sc_msg_iter msg(inSize, inData);
	mBufIndex = msg.geti();
	
	char *filename = msg.gets();
	mFilename = (char*)World_Alloc(mWorld, strlen(filename)+1);
	strcpy(mFilename, filename);
	
	mFileOffset = msg.geti();
	mNumFrames = msg.geti();
	mBufOffset = msg.geti();
	return kSCErr_None;
}

BufReadCmd::~BufReadCmd()
{
	World_Free(mWorld, mFilename);
}

void BufReadCmd::CallDestructor() 
{
	this->~BufReadCmd();
}

bool BufReadCmd::Stage2()
{
	SF_INFO fileinfo;

	SndBuf *buf = World_GetNRTBuf(mWorld, mBufIndex);

	SNDFILE* sf = sf_open(mFilename, SFM_READ, &fileinfo);
	if (!sf) {
		printf("File '%s' could not be opened.\n", mFilename);
		return true;
	}
	if (mNumFrames <= 0 || mNumFrames > fileinfo.frames) mNumFrames = fileinfo.frames;
	int samplesToEnd = buf->frames - mBufOffset;
	
	if (samplesToEnd <= 0) goto leave;
	if (mNumFrames > samplesToEnd) mNumFrames = samplesToEnd;

	sf_seek(sf, mFileOffset, SEEK_SET);
	sf_readf_float(sf, buf->data + (mBufOffset * buf->channels), mNumFrames);
leave:
	sf_close(sf);
	
	return true;
}

bool BufReadCmd::Stage3()
{
	return true;
}

void BufReadCmd::Stage4()
{
	SendDone("/b_read");
}

///////////////////////////////////////////////////////////////////////////

BufWriteCmd::BufWriteCmd(World *inWorld, ReplyAddress *inReplyAddress)
	: SC_SequencedCommand(inWorld, inReplyAddress)
{
}

int sndfileFormatInfoFromStrings(SF_INFO *info, const char *headerFormatString, const char *sampleFormatString);

int BufWriteCmd::Init(char *inData, int inSize)
{
	sc_msg_iter msg(inSize, inData);
	mBufIndex = msg.geti();
	
	char *filename = msg.gets();
	mFilename = (char*)World_Alloc(mWorld, strlen(filename)+1);
	strcpy(mFilename, filename);

	char *headerFormatString = msg.gets();
	char *sampleFormatString = msg.gets();

	return sndfileFormatInfoFromStrings(&mFileInfo, headerFormatString, sampleFormatString);
}

BufWriteCmd::~BufWriteCmd()
{
	World_Free(mWorld, mFilename);
}

void BufWriteCmd::CallDestructor() 
{
	this->~BufWriteCmd();
}

bool BufWriteCmd::Stage2()
{
	SndBuf *buf = World_GetNRTBuf(mWorld, mBufIndex);

	SNDFILE* sf = sf_open(mFilename, SFM_WRITE, &mFileInfo);
	if (!sf) {
		printf("File '%s' could not be opened.\n", mFilename);
		return true;
	}
	sf_writef_float(sf, buf->data, buf->frames);
	sf_close(sf);
	return true;
}

bool BufWriteCmd::Stage3()
{
	return true;
}

void BufWriteCmd::Stage4()
{
	SendDone("/b_write");
}

///////////////////////////////////////////////////////////////////////////

AudioQuitCmd::AudioQuitCmd(World *inWorld, ReplyAddress *inReplyAddress)
	: SC_SequencedCommand(inWorld, inReplyAddress)
{
}

void AudioQuitCmd::CallDestructor() 
{
	this->~AudioQuitCmd();
}

bool AudioQuitCmd::Stage2()
{
	SendDone("/quit");
	mWorld->hw->mQuitProgram->Release();
	return false;
}

///////////////////////////////////////////////////////////////////////////

AudioStatusCmd::AudioStatusCmd(World *inWorld, ReplyAddress *inReplyAddress)
	: SC_SequencedCommand(inWorld, inReplyAddress)
{
	//printf("AudioStatusCmd::AudioStatusCmd\n");
}

void AudioStatusCmd::CallDestructor() 
{
	//printf("AudioStatusCmd::CallDestructor\n");
	this->~AudioStatusCmd();
}

int64 oscTimeNow();

bool AudioStatusCmd::Stage2()
{
	//printf("->AudioStatusCmd::Stage2\n");
	scpacket packet;
	packet.adds("status.reply");
	packet.maketags(5);
	packet.addtag(',');
	packet.addtag('i');
	packet.addtag('i');
	packet.addtag('i');
	packet.addtag('i');
	packet.addtag('i');

	packet.addi(1); // audio is always active now.

	packet.addi(mWorld->mNumUnits);
	packet.addi(mWorld->mNumGraphs);
	packet.addi(mWorld->mNumGroups);
	packet.addi(mWorld->hw->mGraphDefLib->NumItems());

	SendReply(&mReplyAddress, packet.data(), packet.size());

	//printf("<-AudioStatusCmd::Stage2\n");
	return false;
}

///////////////////////////////////////////////////////////////////////////

NotifyCmd::NotifyCmd(World *inWorld, ReplyAddress *inReplyAddress)
	: SC_SequencedCommand(inWorld, inReplyAddress)
{
}

int NotifyCmd::Init(char *inData, int inSize)
{
	sc_msg_iter msg(inSize, inData);
	mOnOff = msg.geti();
	return kSCErr_None;
}

void NotifyCmd::CallDestructor() 
{
	this->~NotifyCmd();
}

bool NotifyCmd::Stage2()
{
	HiddenWorld *hw = mWorld->hw;
	
	if (mOnOff) {
		for (int i=0; i<hw->mNumUsers; ++i) {
			if (mReplyAddress == hw->mUsers[i]) {
				// already in table - don't fail though..
				SendFailure(&mReplyAddress, "/notify", "already registered\n");
				return false;
			}
		}
		
		// add reply address to user table
		if (hw->mNumUsers >= hw->mMaxUsers) {
			SendFailure(&mReplyAddress, "/notify", "too many users\n");
			return false;
		}
			
		hw->mUsers[hw->mNumUsers++] = mReplyAddress;
		
		SendDone("/notify");
	} else {
		for (int i=0; i<hw->mNumUsers; ++i) {
			if (mReplyAddress == hw->mUsers[i]) {
				// remove from list
				hw->mUsers[i] = hw->mUsers[--hw->mNumUsers];
				SendDone("/notify");
				return false;
			}
		}
		
		SendFailure(&mReplyAddress, "/notify", "not registered\n");
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////

SendFailureCmd::SendFailureCmd(World *inWorld, ReplyAddress *inReplyAddress)
	: SC_SequencedCommand(inWorld, inReplyAddress)
{
}

SendFailureCmd::~SendFailureCmd()
{
	World_Free(mWorld, mCmdName);
	World_Free(mWorld, mErrString);
}


void SendFailureCmd::InitSendFailureCmd(char *inCmdName, char* inErrString)
{
	mCmdName = (char*)World_Alloc(mWorld, strlen(inCmdName)+1);
	strcpy(mCmdName, inCmdName);
	
	mErrString = (char*)World_Alloc(mWorld, strlen(inErrString)+1);
	strcpy(mErrString, inErrString);
}

void SendFailureCmd::CallDestructor() 
{
	this->~SendFailureCmd();
}

bool SendFailureCmd::Stage2()
{
	SendFailure(&mReplyAddress, mCmdName, mErrString);
	return false;
}

///////////////////////////////////////////////////////////////////////////

LoadSynthDefCmd::LoadSynthDefCmd(World *inWorld, ReplyAddress *inReplyAddress)
	: SC_SequencedCommand(inWorld, inReplyAddress)
{
}

int LoadSynthDefCmd::Init(char *inData, int inSize)
{
	sc_msg_iter msg(inSize, inData);
	
	char *filename = msg.gets();
	mFilename = (char*)World_Alloc(mWorld, strlen(filename)+1);
	strcpy(mFilename, filename);
	
	mDefs = 0;
	return kSCErr_None;
}

LoadSynthDefCmd::~LoadSynthDefCmd()
{
	World_Free(mWorld, mFilename);
}

void LoadSynthDefCmd::CallDestructor() 
{
	this->~LoadSynthDefCmd();
}

bool LoadSynthDefCmd::Stage2()
{
	mDefs = GraphDef_Load(mWorld, mFilename, mDefs);
	
	return true;
}

bool LoadSynthDefCmd::Stage3()
{
	GraphDef_Define(mWorld, mDefs);
	return true;
}

void LoadSynthDefCmd::Stage4()
{
	SendDone("/d_load");
}

///////////////////////////////////////////////////////////////////////////

LoadSynthDefDirCmd::LoadSynthDefDirCmd(World *inWorld, ReplyAddress *inReplyAddress)
	: SC_SequencedCommand(inWorld, inReplyAddress)
{
}

int LoadSynthDefDirCmd::Init(char *inData, int inSize)
{
	sc_msg_iter msg(inSize, inData);
	
	char *filename = msg.gets();
	mFilename = (char*)World_Alloc(mWorld, strlen(filename)+1);
	strcpy(mFilename, filename);
	
	mDefs = 0;
	return kSCErr_None;
}

LoadSynthDefDirCmd::~LoadSynthDefDirCmd()
{
	World_Free(mWorld, mFilename);
}

void LoadSynthDefDirCmd::CallDestructor() 
{
	this->~LoadSynthDefDirCmd();
}

bool LoadSynthDefDirCmd::Stage2()
{
	mDefs = GraphDef_LoadDir(mWorld, mFilename, mDefs);
	
	return true;
}

bool LoadSynthDefDirCmd::Stage3()
{
	GraphDef_Define(mWorld, mDefs);
	return true;
}

void LoadSynthDefDirCmd::Stage4()
{
	SendDone("/d_loadDir");
}

///////////////////////////////////////////////////////////////////////////


