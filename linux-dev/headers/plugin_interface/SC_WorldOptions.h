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


#ifndef _SC_WorldOptions_
#define _SC_WorldOptions_

#include <stdarg.h>

typedef int (*PrintFunc)(const char *format, va_list ap);

struct WorldOptions
{
	const char* mPassword;
	int mNumBuffers;
	int mMaxLogins;
	int mMaxNodes;
	int mMaxGraphDefs;
	int mMaxWireBufs;
	int mNumAudioBusChannels;
	int mNumInputBusChannels;
	int mNumOutputBusChannels;
	int mNumControlBusChannels;
	int mBufLength;
	int mRealTimeMemorySize;
	
	int mNumSharedSndBufs;
	struct SndBuf *mSharedSndBufs;

	int mNumSharedControls;
	float *mSharedControls;
	
	bool mRealTime;
	
	char *mNonRealTimeCmdFilename;
	char *mNonRealTimeInputFilename;
	char *mNonRealTimeOutputFilename;
	char *mNonRealTimeOutputHeaderFormat;
	char *mNonRealTimeOutputSampleFormat;
	
	int mNonRealTimeSampleRate;
	int mNumRGens;
	int mPreferredHardwareBufferFrameSize;
};

const WorldOptions kDefaultWorldOptions = 
{
	0,1024,64,1024,1024,64,128,2,2,4096,64,2048, 0,0,0,0, 1, 0,0,0,0,0, 0, 64, 0
};

#include "SC_Reply.h"

extern "C" {
	void SetPrintFunc(PrintFunc func);
	struct World* World_New(WorldOptions *inOptions);
	void World_OpenUDP(struct World *inWorld, int inPort);
	void World_OpenTCP(struct World *inWorld, int inPort, int inMaxConnections, int inBacklog);
	void World_SendPacket(struct World *inWorld, int inSize, char *inData, ReplyFunc inFunc);
	void World_WaitForQuit(struct World *inWorld);
	int scprintf(const char *fmt, ...);
}

#endif

