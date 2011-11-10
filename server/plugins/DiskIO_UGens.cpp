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
  Output of frame index for DiskOut by Marije Baalman (nescivi), 2009
  VDiskIn by Scott Wilson, 2008
*/

#include "MsgFifo.h"
#include "SC_SyncCondition.h"
#include "SC_PlugIn.h"
#ifdef _WIN32
	#include <sndfile-win.h>
#else
	#include <sndfile.h>
#endif

#include <new>

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
};

//////////////////////////////////////////////////////////////////////////////////////////////////

extern "C"
{

	void DiskIn_next(DiskIn *unit, int inNumSamples);
	void DiskIn_Ctor(DiskIn* unit);

	void DiskOut_next(DiskOut *unit, int inNumSamples);
	void DiskOut_Ctor(DiskOut* unit);

	void VDiskIn_next(VDiskIn *unit, int inNumSamples);
	void VDiskIn_first(VDiskIn *unit, int inNumSamples);
	void VDiskIn_Ctor(VDiskIn* unit);
}

//////////////////////////////////////////////////////////////////////////////////////////////////


enum {
	kDiskCmd_Read,
	kDiskCmd_Write,
	kDiskCmd_ReadLoop,
};

namespace
{

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

MsgFifoNoFree<DiskIOMsg, 256> gDiskFifo;
SC_SyncCondition gDiskFifoHasData;

void* disk_io_thread_func(void* arg)
{
	while (true) {
		gDiskFifoHasData.WaitEach();
		gDiskFifo.Perform();
	}
	return 0;
}

}

//////////////////////////////////////////////////////////////////////////////////////////////////

#define MAXCHANNELS 32

#define SETUP_OUT(offset) \
	if (unit->mNumOutputs != bufChannels) { \
		ClearUnitOutputs(unit, inNumSamples); \
		return; \
	} \
	float *out[MAXCHANNELS]; \
	for (uint32 i=0; i<bufChannels; ++i) out[i] = OUT(i+offset);

#define SETUP_IN(offset) \
	if (unit->mNumInputs - (uint32)offset != bufChannels) { \
		ClearUnitOutputs(unit, inNumSamples); \
		return; \
	} \
	float *in[MAXCHANNELS]; \
	for (uint32 i=0; i<bufChannels; ++i) in[i] = IN(i+offset);

void DiskIn_Ctor(DiskIn* unit)
{
	unit->m_fbufnum = -1.f;
	unit->m_buf = unit->mWorld->mSndBufs;
	unit->m_framepos = 0;
	SETCALC(DiskIn_next);
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
			for (uint32 i=0; i<bufChannels; ++i) {
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
			gDiskFifo.Write(msg);
			gDiskFifoHasData.Signal();
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

	if (unit->m_framepos >= bufFrames) {
		unit->m_framepos = 0;
	}

	bufData += unit->m_framepos * bufChannels;

	if (bufChannels > 2) {
		for (int j=0; j<inNumSamples; ++j) {
			for (uint32 i=0; i<bufChannels; ++i) {
				*bufData++ = *in[i]++;
			}
			out[j] = framew++;
		}
	} else if (bufChannels == 2) {
		float *in0 = in[0];
		float *in1 = in[1];
		for (int j=0; j<inNumSamples; ++j) {
			*bufData++ = *in0++;
			*bufData++ = *in1++;
			out[j] = framew++;
		}
	} else {
		float *in0 = in[0];
		for (int j=0; j<inNumSamples; ++j) {
			*bufData++ = *in0++;
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
		gDiskFifo.Write(msg);
		gDiskFifoHasData.Signal();
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

	SETCALC(VDiskIn_first); // should be first
}

// first time through, the FIRST sample doesn't need the interpolation... the buffer won't be filled 'correctly' for
// interpolation, so use the _first function to make this exception

void VDiskIn_first(VDiskIn *unit, int inNumSamples)
{

	SETCALC(VDiskIn_next);

	float a, b, c, d, oldBufPos;
	bool test = false;

	GET_BUF_SHARED

	if (!bufData || ((bufFrames & ((unit->mWorld->mBufLength<<1) - 1)) != 0)) {
		unit->m_framePos = 0.;
		unit->m_count = 0;
		ClearUnitOutputs(unit, inNumSamples);
		return;
	}

	uint32 bufFrames2 = bufFrames >> 1;
	float fbufFrames2 = (float)bufFrames2;
	float fbufFrames = (float)bufFrames;
	unit->m_rBufSize = 1. / bufFrames;

	SETUP_OUT(0)

	float framePos = unit->m_framePos;
	float bufPos = unit->m_bufPos; // where we are in the DiskIn buffer
	float newPchRatio = sc_max(IN0(1), 0.f);

	if ((newPchRatio * inNumSamples * unit->m_rBufSize) >= 0.5) {
		printf("pitch ratio is greater then max allowed (see VDiskIn help)\n");
		ClearUnitOutputs(unit, inNumSamples);
		SETCALC(VDiskIn_first);
		return;
	}

	float pchRatio = unit->m_pchRatio;
	float pchSlope = CALCSLOPE(newPchRatio, pchRatio);

	const float* tableInit = bufData;

	for (uint32 i = 0; i < bufChannels; i++){
	    out[i][0] = bufData[0 + i];
	}

	pchRatio += pchSlope;
	framePos += pchRatio;
	bufPos += pchRatio;

	for (int j = 1; j < inNumSamples; j++){
		uint32 iBufPos = (uint32)bufPos;
		float frac = bufPos - (float)iBufPos;
		int table1 = iBufPos * bufChannels;
		int table0 = table1 - bufChannels;
		int table2 = table1 + bufChannels;
		int table3 = table2 + bufChannels;
		while(table1 >= bufSamples) table1 -= bufSamples;
		while(table0 < 0) table0 += bufSamples;
		while(table2 >= bufSamples) table2 -= bufSamples;
		while(table3 >= bufSamples) table3 -= bufSamples;
		for (int i = 0; i < bufChannels; i++){
			a = bufData[table0 + i];
			b = bufData[table1 + i];
			c = bufData[table2 + i];
			d = bufData[table3 + i];
			out[i][j] = cubicinterp(frac, a, b, c, d);
		}
		pchRatio += pchSlope;
		framePos += pchRatio;
		oldBufPos = bufPos;
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
	if ( test ){
		if (unit->m_buf->mask>=0) unit->m_buf->mask1 = unit->m_buf->mask;
		unit->m_count++;
		if(unit->mWorld->mRealTime) {
			test = false;
			DiskIOMsg msg;
			msg.mWorld = unit->mWorld;
			msg.mCommand = (int)ZIN0(2) ? kDiskCmd_ReadLoop : kDiskCmd_Read;
			msg.mBufNum = (int)fbufnum;
			uint32 thisPos;
			if((uint32)bufPos >= bufFrames2) thisPos = 0; else thisPos = bufFrames2;
			msg.mPos = thisPos;
			msg.mFrames = bufFrames2;
			msg.mChannels = bufChannels;
			gDiskFifo.Write(msg);
			gDiskFifoHasData.Signal();


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

	unit->m_framePos = framePos;
	unit->m_pchRatio = pchRatio;
	unit->m_bufPos = bufPos;
}

void VDiskIn_next(VDiskIn *unit, int inNumSamples)
{
	float a, b, c, d;
	bool test = false;
	double oldBufPos;

	GET_BUF_SHARED
	if (!bufData || ((bufFrames & ((unit->mWorld->mBufLength<<1) - 1)) != 0)) {
		unit->m_framePos = 0.;
		unit->m_count = 0;
		ClearUnitOutputs(unit, inNumSamples);
		return;
	}

	SETUP_OUT(0)

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

	for (int j = 0; j < inNumSamples; ++j){
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
			a = bufData[table0 + i];
			b = bufData[table1 + i];
			c = bufData[table2 + i];
			d = bufData[table3 + i];
			out[i][j] = cubicinterp(frac, a, b, c, d);
		};
		pchRatio += pchSlope;
		framePos += pchRatio;
		oldBufPos = bufPos;
		bufPos += pchRatio;

		if ((oldBufPos < (fbufFrames2 + 1)) && ((bufPos >= (fbufFrames2 + 1)))){
			test = true;
		}
		if (bufPos >= (fbufFrames + 1)){
			test = true;
			bufPos -= fbufFrames;
		}
	}
	if (unit->m_buf->mask1>=0 && bufPos>=unit->m_buf->mask1) unit->mDone = true;
	if ( test ){
		if (unit->m_buf->mask>=0) unit->m_buf->mask1 = unit->m_buf->mask;
		unit->m_count++;
		if(unit->mWorld->mRealTime){
				test = false;
				DiskIOMsg msg;
				msg.mWorld = unit->mWorld;
				msg.mCommand = (int)ZIN0(2) ? kDiskCmd_ReadLoop : kDiskCmd_Read;
				msg.mBufNum = (int)fbufnum;
				uint32 thisPos;
				if((uint32)bufPos >= bufFrames2) thisPos = 0; else thisPos = bufFrames2;
				msg.mPos = thisPos;
				msg.mFrames = bufFrames2;
				msg.mChannels = bufChannels;
				gDiskFifo.Write(msg);
				gDiskFifoHasData.Signal();

				if((int)ZIN0(3)) {

					// float outval = bufPos + sc_mod((float)(unit->m_count * bufFrames2), (float)buf->fileinfo.frames);
					float outval = bufPos + (float)(unit->m_count * bufFrames2);
					SendNodeReply(&unit->mParent->mNode, (int)ZIN0(3), "/diskin", 1, &outval);
				}

		} else {
			SndBuf *bufr = World_GetNRTBuf(unit->mWorld, (int)fbufnum);
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



	unit->m_framePos = framePos;
	unit->m_pchRatio = pchRatio;
	unit->m_bufPos = bufPos;

}


////////////////////////////////////////////////////////////////////////////////////////////////////////

PluginLoad(DiskIO)
{
	ft = inTable;

#ifdef _WIN32
	new(&gDiskFifo) MsgFifoNoFree<DiskIOMsg, 256>();
	new(&gDiskFifoHasData)  SC_SyncCondition();
#endif

	pthread_t diskioThread;
	pthread_create (&diskioThread, NULL, disk_io_thread_func, (void*)0);

	DefineSimpleUnit(DiskIn);
	DefineSimpleUnit(DiskOut);
	DefineSimpleUnit(VDiskIn);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
