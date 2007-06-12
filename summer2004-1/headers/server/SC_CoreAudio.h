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
#define SC_AUDIO_API_PORTAUDIO  3

#ifdef SC_WIN32
# define SC_AUDIO_API SC_AUDIO_API_PORTAUDIO
#endif

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

#if SC_AUDIO_API == SC_AUDIO_API_PORTAUDIO
#include "portaudio.h"
#endif


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


class SC_AudioDriver
{
protected:
    int64 mOSCincrement;
	struct World *mWorld;
	double mOSCtoSamples;
	int mSampleTime;

	// Common members
	uint32	mHardwareBufferSize;	// bufferSize returned by kAudioDevicePropertyBufferSize
	EngineFifo mFromEngine, mToEngine;
	SC_SyncCondition mAudioSync;
	pthread_t mThread;
	bool mRunThreadFlag;
	uint32 mSafetyOffset;
	PriorityQueueT<SC_ScheduledEvent, 2048> mScheduler;
	SC_Lock *mProcessPacketLock;
	int mNumSamplesPerCallback;
	uint32 mPreferredHardwareBufferFrameSize;
	uint32 mPreferredSampleRate;
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

    // Driver interface methods, implemented by subclasses
	virtual bool DriverSetup(int* outNumSamplesPerCallback, double* outSampleRate) = 0;
	virtual bool DriverStart() = 0;
	virtual bool DriverStop() = 0;
    
public:
	// Common methods
	SC_AudioDriver(struct World *inWorld);
	virtual ~SC_AudioDriver();

    int64 mOSCbuftime;
    
	bool Setup();
	bool Start();
	bool Stop();
	
	void ClearSched() { mScheduler.Empty(); }

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
	void SetPreferredSampleRate(int inRate)
	{
		mPreferredSampleRate = inRate;
	}

	bool SendMsgToEngine(FifoMsg& inMsg);
	bool SendMsgFromEngine(FifoMsg& inMsg);
	
	void AddEvent(SC_ScheduledEvent& event) { mScheduler.Add(event); }

	double GetAvgCPU() const { return mAvgCPU; }
	double GetPeakCPU() const { return mPeakCPU; }
	double GetSampleRate() const { return mSampleRate; }
	double GetActualSampleRate() const { return mSmoothSampleRate; }	
};


// the following classes should be split out into separate source files.
#if SC_AUDIO_API == SC_AUDIO_API_COREAUDIO
class SC_CoreAudioDriver : public SC_AudioDriver
{

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

protected:
    // Driver interface methods
	virtual bool DriverSetup(int* outNumSamplesPerCallback, double* outSampleRate);
	virtual bool DriverStart();
	virtual bool DriverStop();
    
public:
    SC_CoreAudioDriver(struct World *inWorld);
	virtual ~SC_CoreAudioDriver();

    void Run(const AudioBufferList* inInputData, AudioBufferList* outOutputData, int64 oscTime);

	bool UseInput() { return mInputDevice != kAudioDeviceUnknown; }
	bool UseSeparateIO() { return UseInput() && mInputDevice != mOutputDevice; }
	AudioDeviceID InputDevice() { return mInputDevice; }
	AudioDeviceID OutputDevice() { return mOutputDevice; }
	
	void SetInputBufferList(AudioBufferList * inBufList) { mInputBufList = inBufList; }
	AudioBufferList* GetInputBufferList() const { return mInputBufList; }	
};

inline SC_AudioDriver* SC_NewAudioDriver(struct World *inWorld)
{
    return new SC_CoreAudioDriver(inWorld);
}
#endif // SC_AUDIO_API_COREAUDIO


#if SC_AUDIO_API == SC_AUDIO_API_JACK
class SC_JackDriver : public SC_AudioDriver
{
    jack_client_t		*mClient;
	SC_JackPortList		*mInputList;
	SC_JackPortList		*mOutputList;
	int64				mMaxOutputLatency;

protected:
    // Driver interface methods
	virtual bool DriverSetup(int* outNumSamplesPerCallback, double* outSampleRate);
	virtual bool DriverStart();
	virtual bool DriverStop();
    
public:
    SC_JackDriver(struct World *inWorld);
	virtual ~SC_JackDriver();

    void Run();
	void BufferSizeChanged(int numSamples);
	void SampleRateChanged(double sampleRate);
	void GraphOrderChanged();
};

inline SC_AudioDriver* SC_NewAudioDriver(struct World *inWorld)
{
    return new SC_JackDriver(inWorld);
}
#endif // SC_AUDIO_API_JACK


#if SC_AUDIO_API == SC_AUDIO_API_PORTAUDIO
class SC_PortAudioDriver : public SC_AudioDriver
{

    int mInputChannelCount, mOutputChannelCount;
    PaStream *mStream;

protected:
    // Driver interface methods
	virtual bool DriverSetup(int* outNumSamplesPerCallback, double* outSampleRate);
	virtual bool DriverStart();
	virtual bool DriverStop();
    
public:
    SC_PortAudioDriver(struct World *inWorld);
	virtual ~SC_PortAudioDriver();

    int PortAudioCallback( const void *input, void *output,
            unsigned long frameCount, const PaStreamCallbackTimeInfo* timeInfo,
            PaStreamCallbackFlags statusFlags );
};

inline SC_AudioDriver* SC_NewAudioDriver(struct World *inWorld)
{
    return new SC_PortAudioDriver(inWorld);
}
#endif // SC_AUDIO_API_PORTAUDIO


#endif
