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
	SndBuf *m_prebuf;
	SndBuf *m_iobuf;

	//char m_filename[FILENAME_MAX];
	float *m_outputbufs[kMAXDISKCHANNELS];
	int m_bufpos;
	int m_state, m_loop;
	int m_framestoread, m_framesread, m_framesremain, m_fileoffset;
	int m_loopstart, m_loopend, m_filereadpos;
};

struct DiskOut : public Unit
{
	float m_fbufnum;
	SndBuf *m_buf;
	int m_state;
};


//////////////////////////////////////////////////////////////////////////////////////////////////

extern "C"
{
	void load(InterfaceTable *inTable);

	void DiskIn_next(DiskIn *unit, int inNumSamples);
	void DiskIn_Ctor(DiskIn* unit);

	void DiskOut_next(DiskOut *unit, int inNumSamples);
	void DiskOut_Ctor(DiskOut* unit);

	void DiskIn_Start(DiskIn *unit, sc_msg_iter *msg);
	void DiskIn_Setup(DiskIn *unit, sc_msg_iter *msg);
	void DiskOut_Setup(DiskOut *unit, sc_msg_iter *msg);
	void DiskOut_Start(DiskOut *unit, sc_msg_iter *msg);

}

//////////////////////////////////////////////////////////////////////////////////////////////////

//	void Set(struct World *inWorld, FifoMsgFunc inPerform, FifoMsgFunc inFree, void* inData);

struct diskin_setup {
	char *filename;
	int pos, length;
	int bufnum;
};

/*
void SFOpenReadAsync(filename, &fileptr, pos, length, bufptr);
void SFReadAsync(fileptr, pos, length, bufptr);
void SFCloseAsync(fileptr);
*/

void DiskIn_Start(DiskIn *unit, sc_msg_iter *msg)
{
}

void DiskIn_Setup(DiskIn *unit, sc_msg_iter *msg)
{
	char *filename = msg->gets();
	int ioBufNum = msg->geti(-1);
	int preloadBufNum = msg->geti(-1);
	int startPos = msg->geti(0);
	int length = msg->geti(-1);
	
	World *world = unit->mWorld;
	if (ioBufNum < 0 || ioBufNum >= world->mNumSndBufs) ioBufNum = 0;
	SndBuf *ioBuf = world->mSndBufs + ioBufNum;
	unit->m_iobuf = ioBuf;

	if (preloadBufNum < 0 || preloadBufNum >= world->mNumSndBufs) preloadBufNum = 0;
	SndBuf *preloadBuf = world->mSndBufs + preloadBufNum;
	unit->m_prebuf = preloadBuf;

/*
	int bufChannels = buf->channels;
	int bufSamples = buf->samples;
	int bufFrames = buf->frames;
	int mask = buf->mask;
	int guardFrame = bufFrames - 2;
*/

//	SFOpenReadAsync(filename, &fileptr, startPos, length, ioBuf->data);
}

void DiskOut_Setup(DiskOut *unit, sc_msg_iter *msg)
{
	char *filename = msg->gets();
	char *headerFormat = msg->gets();
	char *sampleFormat = msg->gets();

	int ioBuf = msg->geti(-1);
}

void DiskOut_Start(DiskOut *unit, sc_msg_iter *msg)
{
}


//////////////////////////////////////////////////////////////////////////////////////////////////


#define GET_BUF \
	float fbufnum  = ZIN0(0); \
	if (fbufnum != unit->m_fbufnum) { \
		int bufnum = (int)fbufnum; \
		World *world = unit->mWorld; \
		if (bufnum < 0 || bufnum >= world->mNumSndBufs) bufnum = 0; \
		unit->m_buf = world->mSndBufs + bufnum; \
	} \
	SndBuf *buf = unit->m_buf; \
	int bufChannels = buf->channels; \
	int bufSamples = buf->samples; \
	int bufFrames = buf->frames; \
	int mask = buf->mask; \
	int guardFrame = bufFrames - 2; \
	float *bufData = buf->data;

#define SETUP_OUT \
	if (unit->mNumOutputs != bufChannels) { \
		ClearUnitOutputs(unit, inNumSamples); \
		return; \
	} \
	float *out[16]; \
	for (int i=0; i<bufChannels; ++i) out[i] = OUT(i); 

#define SETUP_IN(offset) \
	if ((unit->mNumInputs - offset) != bufChannels) { \
		ClearUnitOutputs(unit, inNumSamples); \
		return; \
	} \
	float *in[16]; \
	for (int i=0; i<bufChannels; ++i) in[i] = IN(i+offset); 
		

void DiskIn_next(DiskIn *unit, int inNumSamples)
{
	//GET_BUF
	//SETUP_OUT
	int remain = inNumSamples;
	do {
		switch (unit->m_state) {
			case diskinIdle :
				break;
			case diskinStartingEmpty :
			case diskinStartingFull :
				break;
			case diskinNormal :
				break;
			case diskinLastBuffer :
				break;
			case diskinEndSilence :
				//for (int i=0; i<unit->mNumOutputs; ++i) {
				//	Fill(remain, out+i, 0.f);
				//}
				break;
		}
	} while (remain);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void load(InterfaceTable *inTable)
{
	ft = inTable;

	//DefineSimpleUnit(DiskIn);
	//DefineSimpleUnit(DiskOut);
	
	DefineUnitCmd("DiskIn", "setup", (UnitCmdFunc)&DiskIn_Setup);
	DefineUnitCmd("DiskIn", "start", (UnitCmdFunc)&DiskIn_Start);
	
	DefineUnitCmd("DiskOut", "setup", (UnitCmdFunc)&DiskOut_Setup);
	DefineUnitCmd("DiskOut", "start", (UnitCmdFunc)&DiskOut_Start);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
