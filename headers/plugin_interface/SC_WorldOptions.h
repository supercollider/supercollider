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
#include "SC_Types.h"

typedef int (*PrintFunc)(const char *format, va_list ap);

struct WorldOptions
{
	const char* mPassword;
	uint32 mNumBuffers;
	uint32 mMaxLogins;
	uint32 mMaxNodes;
	uint32 mMaxGraphDefs;
	uint32 mMaxWireBufs;
	uint32 mNumAudioBusChannels;
	uint32 mNumInputBusChannels;
	uint32 mNumOutputBusChannels;
	uint32 mNumControlBusChannels;
	uint32 mBufLength;
	uint32 mRealTimeMemorySize;

	int mNumSharedControls;
	float *mSharedControls;
	
	bool mRealTime;
	
	const char *mNonRealTimeCmdFilename;
	const char *mNonRealTimeInputFilename;
	const char *mNonRealTimeOutputFilename;
	const char *mNonRealTimeOutputHeaderFormat;
	const char *mNonRealTimeOutputSampleFormat;
	
	uint32 mPreferredSampleRate;
	uint32 mNumRGens;
	
	uint32 mPreferredHardwareBufferFrameSize;
	
	uint32 mLoadGraphDefs;
	
#ifdef SC_DARWIN
	const char *mInputStreamsEnabled;
	const char *mOutputStreamsEnabled;
#endif
};

const WorldOptions kDefaultWorldOptions = 
{
	0,1024,64,1024,1024,64,128,8,8,4096,64,8192, 0,0, 1, 0,0,0,0,0, 0, 64, 0, 1
#ifdef SC_DARWIN
	,0,0
#endif
};

#include "SC_Reply.h"

extern "C" {
	void SetPrintFunc(PrintFunc func);
	struct World* World_New(WorldOptions *inOptions);
	void World_OpenUDP(struct World *inWorld, int inPort);
	void World_OpenTCP(struct World *inWorld, int inPort, int inMaxConnections, int inBacklog);
	void World_WaitForQuit(struct World *inWorld);
	bool World_SendPacket(struct World *inWorld, int inSize, char *inData, ReplyFunc inFunc);
	int World_CopySndBuf(World *world, uint32 index, struct SndBuf *outBuf, bool onlyIfChanged, bool &didChange);
	int scprintf(const char *fmt, ...);
}

#endif

