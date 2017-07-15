/*
	SuperCollider real time audio synthesis system
	Copyright (c) 2002 James McCartney. All rights reserved.
	Copyright (c) 2009 Marije Baalman
	Copyright (c) 2008 Scott Wilson
	Copyright (c) 2012 Tim Blechmann
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

#include "SC_SyncCondition.h"
#include "SC_PlugIn.h"

#include <sndfile.h>

#include <atomic>
#include <new>
#include <functional>
#include <SC_Lock.h>

#include <boost/lockfree/queue.hpp>
#include <boost/lockfree/spsc_queue.hpp>

static InterfaceTable *ft;

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
	uint32 m_framepos;
};

struct DiskOut : public Unit
{
	float m_fbufnum;
	SndBuf *m_buf;
	uint32 m_framepos;
	uint32 m_framewritten;
};

struct VDiskIn : public Unit
{
	float m_fbufnum, m_pchRatio, m_rBufSize;
	double m_framePos, m_bufPos;
	uint32 m_count;
	SndBuf *m_buf;
	uint32 m_iFramePos, m_iBufPos;
};

//////////////////////////////////////////////////////////////////////////////////////////////////

extern "C"
{

	void DiskIn_next(DiskIn *unit, int inNumSamples);
	void DiskIn_Ctor(DiskIn* unit);

	void DiskOut_next(DiskOut *unit, int inNumSamples);
	void DiskOut_Ctor(DiskOut* unit);
	void DiskOut_Dtor(DiskOut* unit);

	void VDiskIn_next(VDiskIn *unit, int inNumSamples);
	void VDiskIn_first(VDiskIn *unit, int inNumSamples);
	void VDiskIn_next_rate1(VDiskIn *unit, int inNumSamples);
	void VDiskIn_Ctor(VDiskIn* unit);
}

//////////////////////////////////////////////////////////////////////////////////////////////////


enum {
	kDiskCmd_Read,
	kDiskCmd_Write,
	kDiskCmd_ReadLoop,
};

namespace {

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
				memset(buf->data + (mPos + count) * buf->channels, 0, (mFrames - count) * buf->channels * sizeof(float));
				World_GetBuf(mWorld, mBufNum)->mask = mPos + count;
				// NOTE: Possible race condition above: The disk IO thread may write to the rt SndBuf
				// while the stage3 of the sequenced commands copies the non-rt SndBuf struct to the rt buf.
				// This only happens if the buffer is modified via an osc command.
				// We can't use the non-rt SndBuf above since buf->mask won't be reflected to the rt buf.
			}
		break;
		case kDiskCmd_ReadLoop :
			if (!buf->sndfile) {
				memset(buf->data + mPos * buf->channels, 0, mFrames * buf->channels * sizeof(float));
				goto leave;
			}
			count = sf_readf_float(buf->sndfile, buf->data + mPos * buf->channels, mFrames);
			while (mFrames -= count) {
				sf_seek(buf->sndfile, 0, SEEK_SET);
				count = sf_readf_float(buf->sndfile, buf->data + (mPos + count) * buf->channels, mFrames);
			}
		break;
		case kDiskCmd_Write :
			//printf("kDiskCmd_Write %d %p\n", mBufNum, buf->sndfile);
			if (!buf->sndfile) goto leave;
			count = sf_writef_float(buf->sndfile, buf->data + mPos * buf->channels, mFrames);
		break;
	}

leave:
	NRTUnlock(mWorld);
}

struct DiskIOThread
{
	SC_SyncCondition mDiskFifoHasData;

#ifdef SUPERNOVA
	boost::lockfree::queue<DiskIOMsg, boost::lockfree::capacity<256> > mDiskFifo;
#else
	boost::lockfree::spsc_queue<DiskIOMsg, boost::lockfree::capacity<256> > mDiskFifo;
#endif

	std::atomic<bool> mRunning;
	SC_Thread mThread;

	DiskIOThread():
		mRunning(false)
	{}

	~DiskIOThread()
	{
		if (mRunning) {
			mRunning.store(false);
			mDiskFifoHasData.Signal();
			mThread.join();
		}
	}

	void launchThread()
	{
		using namespace std;
		mRunning.store(true);

		mThread = move( thread( bind(&DiskIOThread::ioThreadFunc, this) ) );
	}

	bool Write(DiskIOMsg& data)
	{
		bool pushSucceeded = mDiskFifo.push(data);
		if (pushSucceeded)
			mDiskFifoHasData.Signal();
		return pushSucceeded;
	}

	void ioThreadFunc()
	{
		while (mRunning.load()) {
			mDiskFifoHasData.WaitEach();

			DiskIOMsg msg;
			bool popSucceeded = mDiskFifo.pop(msg);

			if (popSucceeded)
				msg.Perform();
		}
	}
};

DiskIOThread *gDiskIO;

}

//////////////////////////////////////////////////////////////////////////////////////////////////

#define SETUP_OUT(offset) \
	if (unit->mNumOutputs != bufChannels) { \
		ClearUnitOutputs(unit, inNumSamples); \
		return; \
	} \
	float *const * const out = &OUT(offset);

#define SETUP_IN(offset) \
	if (unit->mNumInputs - (uint32)offset != bufChannels) { \
		ClearUnitOutputs(unit, inNumSamples); \
		return; \
	} \
	const float *const * const in = &IN(offset);

void DiskIn_Ctor(DiskIn* unit)
{
	unit->m_fbufnum = -1.f;
	unit->m_buf = unit->mWorld->mSndBufs;
	unit->m_framepos = 0;
	SETCALC(DiskIn_next);
	ClearUnitOutputs(unit, 1);
}

void DiskIn_next(DiskIn *unit, int inNumSamples)
{
	GET_BUF_SHARED
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
			for (uint32 i=0; i<bufChannels; ++i)
				out[i][j] = *bufData++;
		}
	} else if (bufChannels == 2) {
		float *out0 = out[0];
		float *out1 = out[1];
		for (int j=0; j<inNumSamples; ++j) {
			out0[j] = *bufData++;
			out1[j] = *bufData++;
		}
	} else {
		float *out0 = out[0];
		for (int j=0; j<inNumSamples; ++j) {
			out0[j] = *bufData++;
		}
	}
	if(unit->m_buf->mask1>=0 && unit->m_framepos>=unit->m_buf->mask1) unit->mDone = true;
	unit->m_framepos += inNumSamples;
	uint32 bufFrames2 = bufFrames >> 1;
	if (unit->m_framepos == bufFrames) {
		unit->m_framepos = 0;
		goto sendMessage;
	} else if (unit->m_framepos == bufFrames2) {
sendMessage:
		if (unit->m_buf->mask>=0) unit->m_buf->mask1 = unit->m_buf->mask;
		if(unit->mWorld->mRealTime){
			// send a message to read
			DiskIOMsg msg;
			msg.mWorld = unit->mWorld;
			msg.mCommand = (int)ZIN0(1) ? kDiskCmd_ReadLoop : kDiskCmd_Read;
			msg.mBufNum = (int)fbufnum;
			msg.mPos = bufFrames2 - unit->m_framepos;
			msg.mFrames = bufFrames2;
			msg.mChannels = bufChannels;
			gDiskIO->Write(msg);
		} else {
			SndBuf *bufr = World_GetNRTBuf(unit->mWorld, (int) fbufnum);
			uint32 mPos = bufFrames2 - unit->m_framepos;
			if (mPos > (uint32)bufr->frames || mPos + bufFrames2 > (uint32)bufr->frames || (uint32) bufr->channels != bufChannels) return;
			sf_count_t count;

			if ((int)ZIN0(1)) { // loop
				if (!bufr->sndfile) memset(bufr->data + mPos * bufr->channels, 0, bufFrames2 * bufr->channels * sizeof(float));
				count = sf_readf_float(bufr->sndfile, bufr->data + mPos * bufr->channels, bufFrames2);
				while (bufFrames2 -= count) {
					sf_seek(bufr->sndfile, 0, SEEK_SET);
					count = sf_readf_float(bufr->sndfile, bufr->data + (mPos + count) * bufr->channels, bufFrames2);
				}
			} else { // non-loop
				count = bufr->sndfile ? sf_readf_float(bufr->sndfile, bufr->data + mPos * bufr->channels, bufFrames2) : 0;
				if (count < bufFrames2) {
					memset(bufr->data + (mPos + count) * bufr->channels, 0, (bufFrames2 - count) * bufr->channels * sizeof(float));
					unit->m_buf->mask = mPos + count;
				}
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void DiskOut_Ctor(DiskOut* unit)
{
	unit->m_fbufnum = -1.f;
	unit->m_buf = unit->mWorld->mSndBufs;
	unit->m_framepos = 0;
	unit->m_framewritten = 0;
	SETCALC(DiskOut_next);
	ClearUnitOutputs(unit, 1);
}


void DiskOut_next(DiskOut *unit, int inNumSamples)
{
	GET_BUF

	if (!bufData || ((bufFrames & ((unit->mWorld->mBufLength<<1) - 1)) != 0)) {
		unit->m_framepos = 0;
//		unit->m_framewritten = 0;
		return;
	}
	SETUP_IN(1)

	float *out = OUT(0);
	uint32 framew = unit->m_framewritten;

//	printf("Start frames %i %i\n", unit->m_framewritten, framew );

	if (unit->m_framepos >= bufFrames)
		unit->m_framepos = 0;

	bufData += unit->m_framepos * bufChannels;

	if (bufChannels > 2) {
		for (int j=0; j<inNumSamples; ++j) {
			for (uint32 i=0; i<bufChannels; ++i)
				*bufData++ = in[i][j];

			out[j] = framew++;
		}
	} else if (bufChannels == 2) {
		const float *in0 = in[0];
		const float *in1 = in[1];
		for (int j=0; j<inNumSamples; ++j) {
			*bufData++ = in0[j];
			*bufData++ = in1[j];
			out[j] = framew++;
		}
	} else {
		const float *in0 = in[0];
		for (int j=0; j<inNumSamples; ++j) {
			*bufData++ = in0[j];
			out[j] = framew++;
		}
	}

	unit->m_framepos += inNumSamples;
//	unit->m_framewritten += inNumSamples;
	unit->m_framewritten = framew;

//	printf("frames %i %i\n", unit->m_framewritten, framew );

	uint32 bufFrames2 = bufFrames >> 1;
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
		gDiskIO->Write(msg);
	}

}

void DiskOut_Dtor(DiskOut* unit)
{
	GET_BUF

	uint32 framepos = unit->m_framepos;
	uint32 bufFrames2 = bufFrames >> 1;
	// check that we didn't just write
	if (framepos != 0 && framepos != bufFrames2) {
		// if not write the last chunk of samples
		uint32 writeStart;
		if (framepos > bufFrames2) {
		    writeStart = bufFrames2;
		} else {
		    writeStart = 0;
		}
		DiskIOMsg msg;
		msg.mWorld = unit->mWorld;
		msg.mCommand = kDiskCmd_Write;
		msg.mBufNum = (int)fbufnum;
		msg.mPos = writeStart;
		msg.mFrames = framepos - writeStart;
		msg.mChannels = bufChannels;
		//printf("sendMessage %d  %d %d %d\n", msg.mBufNum, msg.mPos, msg.mFrames, msg.mChannels);
		gDiskIO->Write(msg);
	}
}


void VDiskIn_Ctor(VDiskIn* unit)
{
	unit->m_fbufnum = -1.f;
	unit->m_buf = unit->mWorld->mSndBufs;
	unit->m_framePos = 0.;
	unit->m_bufPos = 0.;
	unit->m_pchRatio = sc_max(IN0(1), 0.f);
	unit->m_count = 0;
	unit->m_iFramePos = 0;
	unit->m_iBufPos = 0;

	if (INRATE(1) == calc_ScalarRate && (unit->m_pchRatio == 1))
		SETCALC(VDiskIn_next_rate1);
	else
		SETCALC(VDiskIn_first);

	ClearUnitOutputs(unit, 1);
}

static void VDiskIn_request_buffer(VDiskIn * unit, float fbufnum, uint32 bufFrames2,
								   uint32 bufChannels, double bufPos)
{
	if (unit->m_buf->mask>=0) unit->m_buf->mask1 = unit->m_buf->mask;
	unit->m_count++;
	if(unit->mWorld->mRealTime) {
		DiskIOMsg msg;
		msg.mWorld = unit->mWorld;
		msg.mCommand = (int)ZIN0(2) ? kDiskCmd_ReadLoop : kDiskCmd_Read;
		msg.mBufNum = (int)fbufnum;
		uint32 thisPos;
		if((uint32)bufPos >= bufFrames2) thisPos = 0; else thisPos = bufFrames2;
		msg.mPos = thisPos;
		msg.mFrames = bufFrames2;
		msg.mChannels = bufChannels;
		gDiskIO->Write(msg);

		if((int)ZIN0(3)) {
			//	float outval = bufPos + sc_mod((float)(unit->m_count * bufFrames2), (float)buf->fileinfo.frames);
			float outval = bufPos + (float)(unit->m_count * bufFrames2);
			SendNodeReply(&unit->mParent->mNode, (int)ZIN0(3), "/diskin", 1, &outval);
		}
	} else {
		SndBuf *bufr = World_GetNRTBuf(unit->mWorld, (int) fbufnum);
		uint32 mPos;
		if((uint32)bufPos >= bufFrames2) mPos = 0; else mPos = bufFrames2;
		if (mPos > (uint32)bufr->frames || mPos + bufFrames2 > (uint32)bufr->frames || (uint32) bufr->channels != bufChannels) return;
		sf_count_t count;

		if ((int)ZIN0(2)) { // loop
			if (!bufr->sndfile) memset(bufr->data + mPos * bufr->channels, 0, bufFrames2 * bufr->channels * sizeof(float));
			count = sf_readf_float(bufr->sndfile, bufr->data + mPos * bufr->channels, bufFrames2);
			while (bufFrames2 -= count) {
				sf_seek(bufr->sndfile, 0, SEEK_SET);
				count = sf_readf_float(bufr->sndfile, bufr->data + (mPos + count) * bufr->channels, bufFrames2);
			}
		} else { // non-loop
			count = bufr->sndfile ? sf_readf_float(bufr->sndfile, bufr->data + mPos * bufr->channels, bufFrames2) : 0;
			if (count < bufFrames2) {
				memset(bufr->data + (mPos + count) * bufr->channels, 0, (bufFrames2 - count) * bufr->channels * sizeof(float));
				unit->m_buf->mask = mPos + count;
			}
		}
	}
}

// first time through, the FIRST sample doesn't need the interpolation... the buffer won't be filled 'correctly' for
// interpolation, so use the _first function to make this exception
template <bool First>
static inline void VDiskIn_next_(VDiskIn *unit, int inNumSamples)
{
	bool test = false;

	GET_BUF_SHARED
	if (!bufData || ((bufFrames & ((unit->mWorld->mBufLength<<1) - 1)) != 0)) {
		unit->m_framePos = 0.;
		unit->m_count = 0;
		ClearUnitOutputs(unit, inNumSamples);
		return;
	}

	SETUP_OUT(0);
	if (First)
		unit->m_rBufSize = 1. / bufFrames;

	double framePos = unit->m_framePos;
	double bufPos = unit->m_bufPos; // where we are in the DiskIn buffer
	float newPchRatio = sc_max(IN0(1), 0.f);
	if ((newPchRatio * inNumSamples * unit->m_rBufSize) >= 0.5) {
		printf("pitch ratio is greater then max allowed (see VDiskIn help)\n");
		ClearUnitOutputs(unit, inNumSamples);
		return;
	}

	float pchRatio = unit->m_pchRatio;
	float pchSlope = CALCSLOPE(newPchRatio, pchRatio);
	uint32 bufFrames2 = bufFrames >> 1;
	double fbufFrames2 = (double)bufFrames2;
	double fbufFrames = (double)bufFrames;

	if (First) {
		for (uint32 i = 0; i < bufChannels; i++)
			out[i][0] = bufData[0 + i];
	}

	const int firstLoopSample = First ? 1 : 0;

	for (int j = firstLoopSample; j < inNumSamples; ++j){
		int32 iBufPos = (int32)bufPos;
		double frac = bufPos - (double)iBufPos;
		int table1 = iBufPos * bufChannels;
		int table0 = table1 - bufChannels;
		int table2 = table1 + bufChannels;
		int table3 = table2 + bufChannels;
		while(table1 >= bufSamples) table1 -= bufSamples;
		while(table0 < 0) table0 += bufSamples;
		while(table2 >= bufSamples) table2 -= bufSamples;
		while(table3 >= bufSamples) table3 -= bufSamples;
		for (uint32 i = 0; i < bufChannels; i++){
			float a, b, c, d;
			a = bufData[table0 + i];
			b = bufData[table1 + i];
			c = bufData[table2 + i];
			d = bufData[table3 + i];
			out[i][j] = cubicinterp(frac, a, b, c, d);
		};
		pchRatio += pchSlope;
		framePos += pchRatio;
		const double oldBufPos = bufPos;
		bufPos += pchRatio;

		// the +1 is needed for the cubic interpolation... make SURE the old data isn't needed any more before
		// setting up the new buffer
		if ((oldBufPos < (fbufFrames2 + 1)) && ((bufPos >= (fbufFrames2 + 1)))){
			test = true;
		}
		if (bufPos >= (fbufFrames + 1)){
			test = true;
			bufPos -= fbufFrames;
		}
	}
	if (unit->m_buf->mask1>=0 && bufPos>=unit->m_buf->mask1) unit->mDone = true;
	if ( test )
		VDiskIn_request_buffer(unit, fbufnum, bufFrames2, bufChannels, bufPos);

	unit->m_framePos = framePos;
	unit->m_pchRatio = pchRatio;
	unit->m_bufPos = bufPos;

	if (First)
		SETCALC(VDiskIn_next);
}

void VDiskIn_first(VDiskIn *unit, int inNumSamples)
{
	VDiskIn_next_<true>(unit, inNumSamples);
}

void VDiskIn_next(VDiskIn *unit, int inNumSamples)
{
	VDiskIn_next_<false>(unit, inNumSamples);
}

void VDiskIn_next_rate1(VDiskIn *unit, int inNumSamples)
{
	bool test = false;

	GET_BUF_SHARED
	if (!bufData || ((bufFrames & ((unit->mWorld->mBufLength<<1) - 1)) != 0)) {
		unit->m_iFramePos = 0.;
		unit->m_count = 0;
		ClearUnitOutputs(unit, inNumSamples);
		return;
	}

	SETUP_OUT(0)

	uint32 framePos = unit->m_iFramePos;
	uint32 bufPos = unit->m_iBufPos;

	uint32 bufFrames2 = bufFrames >> 1;

	for (int sample = 0; sample < inNumSamples; ++sample) {
		int bufferIndex = bufPos * bufChannels;

		for (uint32 channel = 0; channel < bufChannels; channel++)
			out[channel][sample] = bufData[bufferIndex + channel];

		const uint32 oldBufPos = bufPos;
		bufPos += 1;
		framePos += 1;

		if ((oldBufPos < bufFrames2) && (bufPos >= bufFrames2))
			test = true;

		if (bufPos >= bufFrames) {
			test = true;
			bufPos -= bufFrames;
		}
	}
	if (unit->m_buf->mask1>=0 && bufPos>=unit->m_buf->mask1) unit->mDone = true;
	if ( test )
		VDiskIn_request_buffer(unit, fbufnum, bufFrames2, bufChannels, bufPos);

	unit->m_iFramePos = framePos;
	unit->m_iBufPos   = bufPos;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////
C_LINKAGE SC_API_EXPORT void unload(InterfaceTable *inTable)
{
	delete gDiskIO;
}

PluginLoad(DiskIO)
{
	ft = inTable;
	gDiskIO = new DiskIOThread();
	gDiskIO->launchThread();

	DefineSimpleUnit(DiskIn);
	DefineDtorUnit(DiskOut);
	DefineSimpleUnit(VDiskIn);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
