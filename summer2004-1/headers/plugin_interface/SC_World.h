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


#ifndef _SC_World_
#define _SC_World_

#include "SC_Types.h"
#include "SC_Rate.h"
#include "SC_SndBuf.h"
#include "SC_RGen.h"
#include "SC_Lock.h"

struct World 
{
	// a pointer to private implementation, not available to plug-ins.
	struct HiddenWorld *hw;
	
	// a pointer to the table of function pointers that implement the plug-ins' 
	// interface to the server.
	struct InterfaceTable *ft;

	// data accessible to plug-ins :
	double mSampleRate;
	int mBufLength;
	int mBufCounter;
	
	uint32 mNumAudioBusChannels;
	uint32 mNumControlBusChannels;
	uint32 mNumInputs;
	uint32 mNumOutputs;
	
	// vector of samples for all audio busses
	float *mAudioBus;
	
	// vector of samples for all control busses
	float *mControlBus;
	
	// these tell if a buss has been written to during a control period
	// if the value is equal to mBufCounter then the buss has been touched 
	// this control period.
	int32 *mAudioBusTouched;
	int32 *mControlBusTouched;
	
	uint32 mNumSndBufs;
	SndBuf *mSndBufs;
	SndBuf *mSndBufsNonRealTimeMirror;
	SndBufUpdates *mSndBufUpdates;
	
	struct Group *mTopGroup;

	Rate mFullRate, mBufRate;
	
	uint32 mNumRGens;
	RGen *mRGen;

	uint32 mNumUnits, mNumGraphs, mNumGroups;
	int mSampleOffset; // offset in the buffer of current event time.
	
	SC_Lock* mNRTLock;
	
	uint32 mNumSharedControls;
	float *mSharedControls;	
	
	bool mRealTime;
	bool mRunning;
	int mDumpOSC;
};

extern "C" {
	int scprintf(const char *fmt, ...);
}

inline SndBuf* World_GetBuf(struct World *inWorld, uint32 index)
{
	if (index > inWorld->mNumSndBufs) index = 0;
	return inWorld->mSndBufs + index;
}

inline SndBuf* World_GetNRTBuf(struct World *inWorld, uint32 index)
{
	if (index > inWorld->mNumSndBufs) index = 0;
	return inWorld->mSndBufsNonRealTimeMirror + index;
}

typedef void (*LoadPlugInFunc)(struct InterfaceTable *);

#endif
