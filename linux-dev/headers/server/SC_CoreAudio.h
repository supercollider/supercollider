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

#include "MsgFifo.h"
#include "SC_FifoMsg.h"
#include "OSC_Packet.h"
#include "SC_SyncCondition.h"
#include "PriorityQueue.h"
#include "SC_Lock.h"

#define SC_AUDIO_API_COREAUDIO	1
#define SC_AUDIO_API_JACK		2

#ifndef SC_AUDIO_API
# define SC_AUDIO_API SC_AUDIO_API_COREAUDIO
#endif // SC_AUDIO_API

#if SC_AUDIO_API == SC_AUDIO_API_COREAUDIO
# include <CoreAudio/AudioHardware.h>
# include <CoreAudio/HostTime.h>
#endif

#if SC_AUDIO_API == SC_AUDIO_API_JACK
# include <jack/jack.h>
class SC_JackPortList;
#endif

class SC_AudioDriver
{
public:

	SC_AudioDriver(struct World *inWorld);

	int64 mOSCbuftime;

protected:
	int64 mOSCincrement;
	struct World *mWorld;
	double mOSCtoSamples;
	int mSampleTime;
};

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

class SC_CoreAudioDriver : public SC_AudioDriver
{
	// Common members
	uint32	mHardwareBufferSize;	// bufferSize returned by kAudioDevicePropertyBufferSize
	EngineFifo mFromEngine, mToEngine;
	SC_SyncCondition mAudioSync;
	pthread_t mThread;
	bool mRunThreadFlag;
	uint32 mSafetyOffset;
	PriorityQueueT<SC_ScheduledEvent, 1024> mScheduler;
	SC_Lock *mProcessPacketLock;
	int mNumSamplesPerCallback;
	uint32 mPreferredHardwareBufferFrameSize;
	double mBuffersPerSecond;
	double mAvgCPU, mPeakCPU;
	int mPeakCounter, mMaxPeakCounter;
	double mOSCincrementNumerator;
	
	double mStartHostSecs;
	double mPrevHostSecs;
	double mStartSampleTime;
	double mPrevSampleTime;
	double mSmoothSampleRate;
	double mSampleRate;
	
	// Driver members
#if SC_AUDIO_API == SC_AUDIO_API_COREAUDIO
	AudioBufferList * mInputBufList;
	AudioDeviceID	mInputDevice;
	AudioDeviceID	mOutputDevice;

	AudioStreamBasicDescription	inputStreamDesc;	// info about the default device
	AudioStreamBasicDescription	outputStreamDesc;	// info about the default device

	friend OSStatus appIOProc (		AudioDeviceID inDevice, 
									const AudioTimeStamp* inNow, 
									const AudioBufferList* inInputData,
									const AudioTimeStamp* inInputTime, 
									AudioBufferList* outOutputData, 
									const AudioTimeStamp* inOutputTime,
									void* defptr);
#endif // SC_AUDIO_API_COREAUDIO

#if SC_AUDIO_API == SC_AUDIO_API_JACK
	jack_client_t		*mClient;
	SC_JackPortList		*mInputList;
	SC_JackPortList		*mOutputList;
#endif // SC_AUDIO_API_JACK

	// Driver interface methods
	void DriverInitialize();
	void DriverRelease();
	bool DriverSetup(int* outNumSamplesPerCallback, double* outSampleRate);
	bool DriverStart();
	bool DriverStop();

public:
	// Common methods
	SC_CoreAudioDriver(struct World *inWorld);
	~SC_CoreAudioDriver();
	
	bool Setup();
	bool Start();
	bool Stop();

	void Lock() { mProcessPacketLock->Lock(); }
	void Unlock() { mProcessPacketLock->Unlock(); }
	
	void RunNonRealTime(float *in, float *out, int numSamples, int64 oscTime);
	void* RunThread();

	int SafetyOffset() const { return mSafetyOffset; }
	int NumSamplesPerCallback() const { return mNumSamplesPerCallback; }
	void SetPreferredHardwareBufferFrameSize(int inSize) 
	{ 
		mPreferredHardwareBufferFrameSize = inSize;
	}

	bool SendMsgToEngine(FifoMsg& inMsg);
	bool SendMsgFromEngine(FifoMsg& inMsg);
	
	void AddEvent(SC_ScheduledEvent& event) { mScheduler.Add(event); }

	double GetAvgCPU() const { return mAvgCPU; }
	double GetPeakCPU() const { return mPeakCPU; }

	// Driver methods	
#if SC_AUDIO_API == SC_AUDIO_API_COREAUDIO
	void Run(const AudioBufferList* inInputData, AudioBufferList* outOutputData, int64 oscTime);

	bool UseInput() { return mInputDevice != kAudioDeviceUnknown; }
	bool UseSeparateIO() { return UseInput() && mInputDevice != mOutputDevice; }
	AudioDeviceID InputDevice() { return mInputDevice; }
	AudioDeviceID OutputDevice() { return mOutputDevice; }
	
	void SetInputBufferList(AudioBufferList * inBufList) { mInputBufList = inBufList; }
	AudioBufferList* GetInputBufferList() const { return mInputBufList; }	
#endif // SC_AUDIO_API_COREAUDIO

#if SC_AUDIO_API == SC_AUDIO_API_JACK
	void JackRun(int64 oscTime);
	void JackBufferSizeChanged(int numSamples);
	void JackSampleRateChanged(double sampleRate);
#endif // SC_AUDIO_API_JACK
};

#endif
