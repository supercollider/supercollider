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


#include "MsgFifo.h"
#include "SC_SyncCondition.h"
#include "SC_PlugIn.h"
#include <sndfile.h>

static InterfaceTable *ft;

const int kMAXDISKCHANNELS = 32;

enum {
	diskinIdle,
	diskinStartingEmpty,
	diskinStartingFull,
	diskinNormal,
	diskinLastBuffer,
	diskinEndSilence
};

enum {
	diskoutIdle,
	diskoutNormal
};


struct DiskIn : public Unit
{
	float m_fbufnum;
	SndBuf *m_buf;
	int m_framepos;
};

struct DiskOut : public Unit
{
	float m_fbufnum;
	SndBuf *m_buf;
	int m_framepos;
};


//////////////////////////////////////////////////////////////////////////////////////////////////

extern "C"
{
	void load(InterfaceTable *inTable);

	void DiskIn_next(DiskIn *unit, int inNumSamples);
	void DiskIn_Ctor(DiskIn* unit);

	void DiskOut_next(DiskOut *unit, int inNumSamples);
	void DiskOut_Ctor(DiskOut* unit);
}

//////////////////////////////////////////////////////////////////////////////////////////////////


enum {
	kDiskCmd_Read,
	kDiskCmd_Write,
};

struct DiskIOMsg
{
	World *mWorld;
	int16 mCommand;
	int16 mChannels;
	int32 mBufNum;
	int32 mPos;
	int32 mFrames;
	
	void Perform();
};

MsgFifoNoFree<DiskIOMsg, 256> gDiskFifo;
SC_SyncCondition gDiskFifoHasData;

void* disk_io_thread_func(void* arg);
void* disk_io_thread_func(void* arg)
{
	while (true) {
		gDiskFifoHasData.WaitEach();
		gDiskFifo.Perform();
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////

#define MAXCHANNELS 32

#define GET_BUF \
	float fbufnum  = ZIN0(0); \
	if (fbufnum != unit->m_fbufnum) { \
		int bufnum = (int)fbufnum; \
		World *world = unit->mWorld; \
		if (bufnum < 0 || bufnum >= world->mNumSndBufs) bufnum = 0; \
		unit->m_fbufnum = fbufnum; \
		unit->m_buf = world->mSndBufs + bufnum; \
	} \
	SndBuf *buf = unit->m_buf; \
	int bufChannels = buf->channels; \
	int bufFrames = buf->frames; \
	float *bufData = buf->data;

#define SETUP_OUT(offset) \
	if (unit->mNumOutputs != bufChannels) { \
		ClearUnitOutputs(unit, inNumSamples); \
		return; \
	} \
	float *out[MAXCHANNELS]; \
	for (int i=0; i<bufChannels; ++i) out[i] = OUT(i+offset); 

#define SETUP_IN(offset) \
	if (unit->mNumInputs - offset != bufChannels) { \
		ClearUnitOutputs(unit, inNumSamples); \
		return; \
	} \
	float *in[MAXCHANNELS]; \
	for (int i=0; i<bufChannels; ++i) in[i] = IN(i+offset); 
		
void DiskIn_Ctor(DiskIn* unit)
{
	unit->m_fbufnum = -1.f;
	unit->m_buf = unit->mWorld->mSndBufs;
	unit->m_framepos = 0;
	SETCALC(DiskIn_next);
}

void DiskIn_next(DiskIn *unit, int inNumSamples)
{
	GET_BUF
	if (!bufData || ((bufFrames & ((unit->mWorld->mBufLength<<1) - 1)) != 0)) {
		unit->m_framepos = 0;
		ClearUnitOutputs(unit, inNumSamples);
		return;
	}
	SETUP_OUT(0)
	
	if (unit->m_framepos >= bufFrames) {
		unit->m_framepos = 0;
	}

	bufData += unit->m_framepos * bufChannels;
	
	// buffer must be allocated as a multiple of 2*blocksize.
	if (bufChannels > 2) {
		for (int j=0; j<inNumSamples; ++j) {
			for (int i=0; i<bufChannels; ++i) {
				*out[i]++ = *bufData++;
			}
		}
	} else if (bufChannels == 2) {
		float *out0 = out[0];
		float *out1 = out[1];
		for (int j=0; j<inNumSamples; ++j) {
			*out0++ = *bufData++;
			*out1++ = *bufData++;
		}
	} else {
		float *out0 = out[0];
		for (int j=0; j<inNumSamples; ++j) {
			*out0++ = *bufData++;
		}
	}
	
	unit->m_framepos += inNumSamples;
	int32 bufFrames2 = bufFrames >> 1;
	if (unit->m_framepos == bufFrames) {
		unit->m_framepos = 0;
		goto sendMessage;
	} else if (unit->m_framepos == bufFrames2) {
sendMessage:
		// send a message to read
		DiskIOMsg msg;
		msg.mWorld = unit->mWorld;
		msg.mCommand = kDiskCmd_Read;
		msg.mBufNum = (int)fbufnum;
		msg.mPos = bufFrames2 - unit->m_framepos;
		msg.mFrames = bufFrames2;
		msg.mChannels = bufChannels;
		gDiskFifo.Write(msg);
		gDiskFifoHasData.Signal();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void DiskOut_Ctor(DiskOut* unit)
{
	unit->m_fbufnum = -1.f;
	unit->m_buf = unit->mWorld->mSndBufs;
	unit->m_framepos = 0;
	SETCALC(DiskOut_next);
}


void DiskOut_next(DiskOut *unit, int inNumSamples)
{
	GET_BUF

	if (!bufData || ((bufFrames & ((unit->mWorld->mBufLength<<1) - 1)) != 0)) {
		unit->m_framepos = 0;
		return;
	}
	SETUP_IN(1)
	
	if (unit->m_framepos >= bufFrames) {
		unit->m_framepos = 0;
	}
	
	bufData += unit->m_framepos * bufChannels;

	if (bufChannels > 2) {
		for (int j=0; j<inNumSamples; ++j) {
			for (int i=0; i<bufChannels; ++i) {
				*bufData++ = *in[i]++;
			}
		}
	} else if (bufChannels == 2) {
		float *in0 = in[0];
		float *in1 = in[1];
		for (int j=0; j<inNumSamples; ++j) {
			*bufData++ = *in0++;
			*bufData++ = *in1++;
		}
	} else {
		float *in0 = in[0];
		for (int j=0; j<inNumSamples; ++j) {
			*bufData++ = *in0++;
		}
	}
	
	unit->m_framepos += inNumSamples;
	int32 bufFrames2 = bufFrames >> 1;
	//printf("pos %d  %d %d\n", unit->m_framepos, bufFrames, bufFrames2);
	if (unit->m_framepos == bufFrames) {
		unit->m_framepos = 0;
		goto sendMessage;
	} else if (unit->m_framepos == bufFrames2) {
sendMessage:
		// send a message to write
		DiskIOMsg msg;
		msg.mWorld = unit->mWorld;
		msg.mCommand = kDiskCmd_Write;
		msg.mBufNum = (int)fbufnum;
		msg.mPos = bufFrames2 - unit->m_framepos;
		msg.mFrames = bufFrames2;
		msg.mChannels = bufChannels;
		//printf("sendMessage %d  %d %d %d\n", msg.mBufNum, msg.mPos, msg.mFrames, msg.mChannels);
		gDiskFifo.Write(msg);
		gDiskFifoHasData.Signal();
	}
}

void DiskIOMsg::Perform()
{
	NRTLock(mWorld);

	SndBuf *buf = World_GetNRTBuf(mWorld, mBufNum);
	if (mPos > buf->frames || mPos + mFrames > buf->frames || buf->channels != mChannels) goto leave;

	sf_count_t count;
	switch (mCommand) {
		case kDiskCmd_Read :
			count = buf->sndfile ? sf_readf_float(buf->sndfile, buf->data + mPos * buf->channels, mFrames) : 0;
			if (count < mFrames) {
				memset(buf->data + (mPos + count) * buf->channels, 0, (mFrames - count) * buf->channels);
			}
		break;
		case kDiskCmd_Write :
			//printf("kDiskCmd_Write %d %08X\n", mBufNum, buf->sndfile);
			if (!buf->sndfile) goto leave;
			count = sf_writef_float(buf->sndfile, buf->data + mPos * buf->channels, mFrames);
		break;
	}
	
leave:
	NRTUnlock(mWorld);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void load(InterfaceTable *inTable)
{
	ft = inTable;
	
	pthread_t diskioThread;
	pthread_create (&diskioThread, NULL, disk_io_thread_func, (void*)0);
	
	DefineSimpleUnit(DiskIn);
	DefineSimpleUnit(DiskOut);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
