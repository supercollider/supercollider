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


#ifndef _SC_CoreAudio_
#define _SC_CoreAudio_

#include <CoreAudio/AudioHardware.h>
#include <CoreAudio/HostTime.h>
#include "MsgFifo.h"
#include "SC_FifoMsg.h"
#include "OSC_Packet.h"
#include "SC_SyncCondition.h"
#include "PriorityQueue.h"
#include "SC_Lock.h"

struct SC_ScheduledEvent 
{
	SC_ScheduledEvent() : mTime(0), mPacket(0) {}
	SC_ScheduledEvent(struct World *inWorld, int64 inTime, OSC_Packet *inPacket) 
		: mTime(inTime), mPacket(inPacket), mWorld(inWorld) {}
	
	int64 Time() { return mTime; }
	void Perform();
	
	int64 mTime;
	OSC_Packet *mPacket;
	struct World *mWorld;
};

typedef MsgFifo<FifoMsg, 1024> EngineFifo;

class SC_CoreAudioDriver
{
	struct World *mWorld;
	bool mActive;			// playing now?
	UInt32	mHardwareBufferSize;	// bufferSize returned by kAudioDevicePropertyBufferSize
	EngineFifo mFromEngine, mToEngine;
	SC_SyncCondition mAudioSync;
	pthread_t mThread;
	UInt32 mSafetyOffset;
	int mNumSamples;
	int mSampleTime;
	int mSampleOffset;
	int64 mOSCincrement;
	double mOSCtoSamples;
	PriorityQueueT<SC_ScheduledEvent, 1024> mScheduler;
	SC_Lock *mProcessPacketLock;
	
	AudioBufferList * mInputBufList;
	AudioDeviceID	mInputDevice;
	AudioDeviceID	mOutputDevice;

	AudioStreamBasicDescription	inputStreamDesc;	// info about the default device
	AudioStreamBasicDescription	outputStreamDesc;	// info about the default device

public:

	SC_CoreAudioDriver(struct World *inWorld);
	
	bool Setup();
	bool Start();
	bool Stop();

	void Lock() { mProcessPacketLock->Lock(); }
	void Unlock() { mProcessPacketLock->Unlock(); }
	
	void Run(const AudioBufferList* inInputData, AudioBufferList* outOutputData, int64 oscTime);
	void RunNonRealTime(float *in, float *out, int numSamples, int64 oscTime);

	void* RunThread();

	bool Active() const { return mActive; }
	int NumSamples() const { return mNumSamples; }
	int SafetyOffset() const { return mSafetyOffset; }
	
	int SampleOffset() const { return mSampleOffset; }

	bool SendMsgToEngine(FifoMsg& inMsg);
	bool SendMsgFromEngine(FifoMsg& inMsg);
	
	void AddEvent(SC_ScheduledEvent& event) { mScheduler.Add(event); }

	bool UseSeparateIO() { return mInputDevice != mOutputDevice; }
	AudioDeviceID InputDevice() { return mInputDevice; }
	AudioDeviceID OutputDevice() { return mOutputDevice; }
	
	void SetInputBufferList(AudioBufferList * inBufList) { mInputBufList = inBufList; }
	AudioBufferList* GetInputBufferList() const { return mInputBufList; }
	
	int64 mOSCbuftime;
};

#endif
