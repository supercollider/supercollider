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
	
	int mNumAudioBusChannels;
	int mNumControlBusChannels;
	int mNumInputs;
	int mNumOutputs;
	
	// vector of samples for all audio busses
	float *mAudioBus;
	
	// vector of samples for all control busses
	float *mControlBus;
	
	// these tell if a buss has been written to during a control period
	// if the value is equal to mBufCounter then the buss has been touched 
	// this control period.
	int32 *mAudioBusTouched;
	int32 *mControlBusTouched;
	
	int mNumSndBufs;
	SndBuf *mSndBufs;
	SndBuf *mSndBufsNonRealTimeMirror;

	struct Group *mTopGroup;

	Rate mFullRate, mBufRate;
	
	RGen mRGen;

	int mNumUnits, mNumGraphs, mNumGroups;
		
};

inline SndBuf* World_GetBuf(struct World *inWorld, int index)
{
	if (index < 0 || index > inWorld->mNumSndBufs) index = 0;
	return inWorld->mSndBufs + index;
}

inline SndBuf* World_GetNRTBuf(struct World *inWorld, int index)
{
	if (index < 0 || index > inWorld->mNumSndBufs) index = 0;
	return inWorld->mSndBufsNonRealTimeMirror + index;
}

typedef void (*LoadPlugInFunc)(struct InterfaceTable *);

#endif
