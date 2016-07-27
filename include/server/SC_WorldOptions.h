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


#ifndef _SC_WorldOptions_
#define _SC_WorldOptions_

#include <stdarg.h>
#include "SC_Reply.h"
#include "SC_Types.h"
#include "SC_Export.h"

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
	bool mMemoryLocking;

	const char *mNonRealTimeCmdFilename;
	const char *mNonRealTimeInputFilename;
	const char *mNonRealTimeOutputFilename;
	const char *mNonRealTimeOutputHeaderFormat;
	const char *mNonRealTimeOutputSampleFormat;

	uint32 mPreferredSampleRate;
	uint32 mNumRGens;

	uint32 mPreferredHardwareBufferFrameSize;

	uint32 mLoadGraphDefs;

	const char *mInputStreamsEnabled;
	const char *mOutputStreamsEnabled;
	const char *mInDeviceName;

	int mVerbosity;

	bool mRendezvous;

	const char *mUGensPluginPath;

	const char *mOutDeviceName;

	const char *mRestrictedPath;

	int mSharedMemoryID;
};

const struct WorldOptions kDefaultWorldOptions =
{
	0,1024,64,1024,1024,64,1024,8,8,16384,64,8192, 0,0, 1, 0, 0,0,0,0,0
#if defined(_WIN32)
	,44100
#else
	,0
#endif
	,64, 0, 1
	,0, 0, 0
	,0
	,1
	,0
	,0
	,0
	,0
};

struct SndBuf;

SCSYNTH_DLLEXPORT_C void SetPrintFunc(PrintFunc func);
SCSYNTH_DLLEXPORT_C struct World* World_New(struct WorldOptions *inOptions);
SCSYNTH_DLLEXPORT_C void World_Cleanup(struct World *inWorld, bool unload_plugins = false);
SCSYNTH_DLLEXPORT_C void World_NonRealTimeSynthesis(struct World *inWorld, struct WorldOptions *inOptions);
SCSYNTH_DLLEXPORT_C int World_OpenUDP(struct World *inWorld, const char *bindTo, int inPort);
SCSYNTH_DLLEXPORT_C int World_OpenTCP(struct World *inWorld, const char *bindTo, int inPort, int inMaxConnections, int inBacklog);
SCSYNTH_DLLEXPORT_C void World_WaitForQuit(struct World *inWorld, bool unload_plugins = false);
SCSYNTH_DLLEXPORT_C bool World_SendPacket(struct World *inWorld, int inSize, char *inData, ReplyFunc inFunc);
SCSYNTH_DLLEXPORT_C bool World_SendPacketWithContext(struct World *inWorld, int inSize, char *inData, ReplyFunc inFunc, void *inContext);
SCSYNTH_DLLEXPORT_C int World_CopySndBuf(struct World *world, uint32 index, struct SndBuf *outBuf, bool onlyIfChanged, bool *didChange);
SCSYNTH_DLLEXPORT_C int scprintf(const char *fmt, ...);

#endif // _SC_WorldOptions_
