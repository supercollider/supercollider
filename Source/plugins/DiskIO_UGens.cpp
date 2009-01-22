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
	uint32 m_framepos;
};

struct DiskOut : public Unit
{
	float m_fbufnum;
	SndBuf *m_buf;
	uint32 m_framepos;
};

struct VDiskIn : public Unit
{
	float m_fbufnum, m_pchRatio, m_framePos, m_bufPos, m_rBufSize;
	uint32 m_count;
	SndBuf *m_buf;
};

//////////////////////////////////////////////////////////////////////////////////////////////////

extern "C"
{
	void load(InterfaceTable *inTable);

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

#ifndef SC_WIN32
MsgFifoNoFree<DiskIOMsg, 256> gDiskFifo;
SC_SyncCondition gDiskFifoHasData;
#else // #ifndef SC_WIN32
MsgFifoNoFree<DiskIOMsg, 256>* pgDiskFifo;
SC_SyncCondition* pgDiskFifoHasData;
#endif // #ifndef SC_WIN32

void* disk_io_thread_func(void* arg);
void* disk_io_thread_func(void* arg)
{
	while (true) {
#ifndef SC_WIN32
    gDiskFifoHasData.WaitEach();
		gDiskFifo.Perform();
#else //#ifndef SC_WIN32
    pgDiskFifoHasData->WaitEach();
		pgDiskFifo->Perform();
#endif //#ifndef SC_WIN32
	}
	return 0;
}

static float cubicinterp(float x, float y0, float y1, float y2, float y3)
{
	// 4-point, 3rd-order Hermite (x-form)
	float c0 = y1;
	float c1 = 0.5f * (y2 - y0);
	float c2 = y0 - 2.5f * y1 + 2.f * y2 - 0.5f * y3;
	float c3 = 0.5f * (y3 - y0) + 1.5f * (y1 - y2);

	return ((c3 * x + c2) * x + c1) * x + c0;
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
	
	unit->m_framepos += inNumSamples;
	uint32 bufFrames2 = bufFrames >> 1;
	if (unit->m_framepos == bufFrames) {
		unit->m_framepos = 0;
		goto sendMessage;
	} else if (unit->m_framepos == bufFrames2) {
sendMessage:
	if(unit->mWorld->mRealTime){
		// send a message to read
		DiskIOMsg msg;
		msg.mWorld = unit->mWorld;
		msg.mCommand = (int)ZIN0(1) ? kDiskCmd_ReadLoop : kDiskCmd_Read;
		msg.mBufNum = (int)fbufnum;
		msg.mPos = bufFrames2 - unit->m_framepos;
		msg.mFrames = bufFrames2;
		msg.mChannels = bufChannels;
#ifndef SC_WIN32
		gDiskFifo.Write(msg);
		gDiskFifoHasData.Signal();
#else //#ifndef SC_WIN32
		pgDiskFifo->Write(msg);
		pgDiskFifoHasData->Signal();
#endif //#ifndef SC_WIN32
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
			for (uint32 i=0; i<bufChannels; ++i) {
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
#ifndef SC_WIN32
		gDiskFifo.Write(msg);
		gDiskFifoHasData.Signal();
#else //#ifndef SC_WIN32
		pgDiskFifo->Write(msg);
		pgDiskFifoHasData->Signal();
#endif //#ifndef SC_WIN32
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
				memset(buf->data + (mPos + count) * buf->channels, 0, (mFrames - count) * buf->channels * sizeof(float));
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
			//printf("kDiskCmd_Write %d %08X\n", mBufNum, buf->sndfile);
			if (!buf->sndfile) goto leave;
			count = sf_writef_float(buf->sndfile, buf->data + mPos * buf->channels, mFrames);
		break;
	}
	
leave:
	NRTUnlock(mWorld);
}


void VDiskIn_Ctor(VDiskIn* unit)
{
	unit->m_fbufnum = -1.f;
	unit->m_buf = unit->mWorld->mSndBufs;
	unit->m_framePos = 0.;
	unit->m_bufPos = 0.;
	unit->m_pchRatio = IN0(1);
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

	GET_BUF
	
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
	float newPchRatio = IN0(1);

	if ((newPchRatio * inNumSamples * unit->m_rBufSize) >= 0.5) {
	    printf("pitch ratio is greater then max allowed (see VDiskIn help)\n");
	    ClearUnitOutputs(unit, inNumSamples);
	    SETCALC(VDiskIn_first);
	    return;
	    }
	    
	float pchRatio = unit->m_pchRatio;
	float pchSlope = CALCSLOPE(newPchRatio, pchRatio);

	float* tableInit = bufData;

	for (uint32 i = 0; i < bufChannels; i++){
	    out[i][0] = bufData[0 + i];
	    };
	    
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
		};
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
	if ( test ){
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
			#ifndef SC_WIN32
				gDiskFifo.Write(msg);
				gDiskFifoHasData.Signal();
			#else //#ifndef SC_WIN32
				pgDiskFifo->Write(msg);
				pgDiskFifoHasData->Signal();
			#endif //#ifndef SC_WIN32
				

				if((int)ZIN0(3)) {
				//	float outval = bufPos + sc_mod((float)(unit->m_count * bufFrames2), (float)buf->fileinfo.frames);
					float outval = bufPos + (float)(unit->m_count * bufFrames2);
					SendNodeReply(&unit->mParent->mNode, (int)ZIN0(3), 1, &outval, "/diskin");
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
	float a, b, c, d, oldBufPos;
	bool test = false;
	
	GET_BUF
	if (!bufData || ((bufFrames & ((unit->mWorld->mBufLength<<1) - 1)) != 0)) {
		unit->m_framePos = 0.;
		unit->m_count = 0;
		ClearUnitOutputs(unit, inNumSamples);
		return;
	}
	
	SETUP_OUT(0)
	
	float framePos = unit->m_framePos;
	float bufPos = unit->m_bufPos; // where we are in the DiskIn buffer
	float newPchRatio = IN0(1);
	if ((newPchRatio * inNumSamples * unit->m_rBufSize) >= 0.5) {
	    printf("pitch ratio is greater then max allowed (see VDiskIn help)\n");
	    ClearUnitOutputs(unit, inNumSamples);
	    return;
	    }
	    
	float pchRatio = unit->m_pchRatio;
	float pchSlope = CALCSLOPE(newPchRatio, pchRatio);
	uint32 bufFrames2 = bufFrames >> 1;
	float fbufFrames2 = (float)bufFrames2;
	float fbufFrames = (float)bufFrames;
	
	for (int j = 0; j < inNumSamples; ++j){
	    int iBufPos = (int)bufPos;   
	    float frac = bufPos - (float)iBufPos;
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
	if ( test ){
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
			#ifndef SC_WIN32
				gDiskFifo.Write(msg);
				gDiskFifoHasData.Signal();
			#else //#ifndef SC_WIN32
				pgDiskFifo->Write(msg);
				pgDiskFifoHasData->Signal();
			#endif //#ifndef SC_WIN32

				if((int)ZIN0(3)) {
					
					// float outval = bufPos + sc_mod((float)(unit->m_count * bufFrames2), (float)buf->fileinfo.frames);
					float outval = bufPos + (float)(unit->m_count * bufFrames2);
					SendNodeReply(&unit->mParent->mNode, (int)ZIN0(3), 1, &outval, "/diskin");
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
			    }
		    }
			
	    }
	}
	
	
	
	unit->m_framePos = framePos;
	unit->m_pchRatio = pchRatio;
	unit->m_bufPos = bufPos;

}


////////////////////////////////////////////////////////////////////////////////////////////////////////

void load(InterfaceTable *inTable)
{
	ft = inTable;
	
#ifdef SC_WIN32
  pgDiskFifo = new MsgFifoNoFree<DiskIOMsg, 256>;
  pgDiskFifoHasData = new SC_SyncCondition;
  //$$$todo FIXME free those objects . (use a global std::auto_ptr)
#endif //SC_WIN32

  pthread_t diskioThread;
	pthread_create (&diskioThread, NULL, disk_io_thread_func, (void*)0);
	
	DefineSimpleUnit(DiskIn);
	DefineSimpleUnit(DiskOut);
	DefineSimpleUnit(VDiskIn);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
