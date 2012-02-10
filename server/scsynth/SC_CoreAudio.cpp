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

#include "SC_CoreAudio.h"
#include "SC_Sem.h"
#include <stdarg.h>
#include "SC_SequencedCommand.h"
#include "SC_Prototypes.h"
#include "SC_HiddenWorld.h"
#include "SC_Endian.h"
#include "SC_Lib_Cintf.h"
#include <stdlib.h>
#include <pthread.h>
#include <algorithm>

#ifdef _WIN32

#else
#include <sys/time.h>
#endif

#ifdef SC_IPHONE
#include "SC_VFP11.h"
#endif


#ifdef _WIN32
#include "SC_Win32Utils.h"
#endif

int64 gStartupOSCTime = -1;

void sc_SetDenormalFlags();

void set_real_time_priority(pthread_t thread);

double gSampleRate, gSampleDur;

// =====================================================================
// Timing (CoreAudio)

#if SC_AUDIO_API == SC_AUDIO_API_COREAUDIO || SC_AUDIO_API == SC_AUDIO_API_AUDIOUNITS

int64 gOSCoffset = 0;

int32 server_timeseed()
{
	static int32 count = 0;
	int64 time = AudioGetCurrentHostTime();
	return (int32)(time >> 32) ^ (int32)time ^ count--;
}

inline int64 CoreAudioHostTimeToOSC(int64 hostTime)
{
	return (int64)((double)AudioConvertHostTimeToNanos(hostTime) * kNanosToOSCunits) + gOSCoffset;
}

int64 oscTimeNow()
{
	return CoreAudioHostTimeToOSC(AudioGetCurrentHostTime());
}

static void syncOSCOffsetWithTimeOfDay()
{
	// generate a value gOSCoffset such that
	// (gOSCOffset + systemTimeInOSCunits)
	// is equal to gettimeofday time in OSCunits.
	// Then if this machine is synced via NTP, we are synced with the world.
	// more accurate way to do this??

	struct timeval tv;

	int64 systemTimeBefore, systemTimeAfter, diff;
	int64 minDiff = 0x7fffFFFFffffFFFFLL;

	// take best of several tries
	const int numberOfTries = 5;
	int64 newOffset = gOSCoffset;
	for (int i=0; i<numberOfTries; ++i) {
		systemTimeBefore = AudioGetCurrentHostTime();
		gettimeofday(&tv, 0);
		systemTimeAfter = AudioGetCurrentHostTime();

		diff = systemTimeAfter - systemTimeBefore;
		if (diff < minDiff) {
			minDiff = diff;
			// assume that gettimeofday happens halfway between AudioGetCurrentHostTime calls
			int64 systemTimeBetween = systemTimeBefore + diff/2;
			int64 systemTimeInOSCunits = (int64)((double)AudioConvertHostTimeToNanos(systemTimeBetween) * kNanosToOSCunits);
			int64 timeOfDayInOSCunits  = ((int64)(tv.tv_sec + kSECONDS_FROM_1900_to_1970) << 32)
								    + (int64)(tv.tv_usec * kMicrosToOSCunits);
			newOffset = timeOfDayInOSCunits - systemTimeInOSCunits;
		}
	}

	gOSCoffset = newOffset;
	//scprintf("gOSCoffset %016llX\n", gOSCoffset);
}

void* resyncThreadFunc(void* arg);
void* resyncThreadFunc(void* /*arg*/)
{
	while (true) {
		sleep(20);
		syncOSCOffsetWithTimeOfDay();
	}
	return 0;
}

void initializeScheduler()
{
	syncOSCOffsetWithTimeOfDay();

	pthread_t resyncThread;
	pthread_create (&resyncThread, NULL, resyncThreadFunc, (void*)0);
	set_real_time_priority(resyncThread);
}
#endif // SC_AUDIO_API_COREAUDIO


// =====================================================================
// Timing (CoreAudioIPHONE)

#if SC_AUDIO_API == SC_AUDIO_API_COREAUDIOIPHONE

int64 gOSCoffset = 0;

static inline int64 GetMicroseconds()
{
	struct timeval tv;
	gettimeofday(&tv, 0);
	return (int64) tv.tv_sec * 1000000 + tv.tv_usec;
}

static inline int64 GetCurrentOSCTime()
{
	struct timeval tv;
	uint64 s, f;
	gettimeofday(&tv, 0);
	s = (uint64)tv.tv_sec + (uint64)kSECONDS_FROM_1900_to_1970;
	f = (uint64)((double)tv.tv_usec * kMicrosToOSCunits);

	return (s << 32) + f;
}

int32 server_timeseed()
{
	int64 time = GetCurrentOSCTime();
	return Hash((int32)(time >> 32) + Hash((int32)time));
}

int64 oscTimeNow()
{
	return GetCurrentOSCTime();
}

void initializeScheduler()
{
	gOSCoffset = GetCurrentOSCTime();
}
#endif // SC_AUDIO_API_COREAUDIO




// =====================================================================
// Timing (PortAudio)

#if SC_AUDIO_API == SC_AUDIO_API_PORTAUDIO

int64 gOSCoffset = 0;

static inline int64 GetCurrentOSCTime()
{
	struct timeval tv;
	uint64 s, f;
	gettimeofday(&tv, 0);
	s = (uint64)tv.tv_sec + (uint64)kSECONDS_FROM_1900_to_1970;
	f = (uint64)((double)tv.tv_usec * kMicrosToOSCunits);

	return (s << 32) + f;
}

int32 server_timeseed()
{
	int64 time = GetCurrentOSCTime();
	return Hash((int32)(time >> 32) + Hash((int32)time));
}

int64 oscTimeNow()
{
	return GetCurrentOSCTime();
}

int64 PaStreamTimeToOSC(PaTime pa_time) {
	uint64 s, f;
	s = (uint64)pa_time;
	f = (uint64)((pa_time - s) * 1000000 * kMicrosToOSCunits);

	return (s << 32) + f;
}

void initializeScheduler()
{
	gOSCoffset = GetCurrentOSCTime();
}
#endif // SC_AUDIO_API_PORTAUDIO


// =====================================================================
// Packets (Common)

bool ProcessOSCPacket(World *inWorld, OSC_Packet *inPacket);
void PerformOSCBundle(World *inWorld, OSC_Packet *inPacket);
int PerformOSCMessage(World *inWorld, int inSize, char *inData, ReplyAddress *inReply);
PacketStatus PerformOSCPacket(World *world, OSC_Packet *packet, SC_ScheduledEvent::PacketFreeFunc);

void Perform_ToEngine_Msg(FifoMsg *inMsg);
void FreeOSCPacket(FifoMsg *inMsg);

struct IsBundle
{
	IsBundle() { str4cpy(s, "#bundle"); }
	bool checkIsBundle(int32 *in) { return in[0] == s[0] && in[1] == s[1]; }
	int32 s[2];
};
IsBundle gIsBundle;

bool ProcessOSCPacket(World *inWorld, OSC_Packet *inPacket)
{
	//scprintf("ProcessOSCPacket %d, '%s'\n", inPacket->mSize, inPacket->mData);
	if (!inPacket) return false;
	bool result;
	inWorld->mDriverLock->Lock();
		SC_AudioDriver *driver = AudioDriver(inWorld);
		if (!driver) {
			inWorld->mDriverLock->Unlock();
			return false;
		}
		inPacket->mIsBundle = gIsBundle.checkIsBundle((int32*)inPacket->mData);
		FifoMsg fifoMsg;
		fifoMsg.Set(inWorld, Perform_ToEngine_Msg, FreeOSCPacket, (void*)inPacket);
		result = driver->SendOscPacketMsgToEngine(fifoMsg);
	inWorld->mDriverLock->Unlock();
	return result;
}

int PerformOSCMessage(World *inWorld, int inSize, char *inData, ReplyAddress *inReply)
{
	// scprintf("->PerformOSCMessage %d\n", inData[0]);
	SC_LibCmd *cmdObj;
	int cmdNameLen;
	if (inData[0] == 0) {
		cmdNameLen = 4;
		uint32 index = inData[3];
		if (index >= NUMBER_OF_COMMANDS) cmdObj = 0;
		else cmdObj = gCmdArray[index];
	} else {
		cmdNameLen = OSCstrlen(inData);
		cmdObj = gCmdLib->Get((int32*)inData);
	}
	if (!cmdObj) {
		CallSendFailureCommand(inWorld, inData, "Command not found", inReply);
		scprintf("FAILURE IN SERVER: %s Command not found\n", inData);
		return kSCErr_NoSuchCommand;
	}

	int err = cmdObj->Perform(inWorld, inSize - cmdNameLen, inData + cmdNameLen, inReply);
	//scprintf("<-PerformOSCMessage %d\n", inData[0]);
	return err;
}

void PerformOSCBundle(World *inWorld, OSC_Packet *inPacket)
{
	//scprintf("->PerformOSCBundle %d\n", inPacket->mSize);
	char *data = inPacket->mData + 16;
	char* dataEnd = inPacket->mData + inPacket->mSize;

	while (data < dataEnd) {
		int32 msgSize = ntohl(*(int32*)data);
		data += sizeof(int32);
		//scprintf("msgSize %d\n", msgSize);
		PerformOSCMessage(inWorld, msgSize, data, &inPacket->mReplyAddr);
		data += msgSize;
	}

		// reset so next command uses permanent error notification status
	inWorld->mLocalErrorNotification = 0;

//		// 0 is a temporary change, so reset the local error flag
//	if(!inWorld->mLocalErrorNotification) {
//		inWorld->mLocalErrorNotification = inWorld->mErrorNotification;
//	};

	//scprintf("<-PerformOSCBundle %d\n", inPacket->mSize);
}

PacketStatus PerformOSCPacket(World *world, OSC_Packet *packet, SC_ScheduledEvent::PacketFreeFunc freeFunc)
{
	SC_AudioDriver *driver = world->hw->mAudioDriver;

	if (!packet->mIsBundle) {
		PerformOSCMessage(world, packet->mSize, packet->mData, &packet->mReplyAddr);
		world->mLocalErrorNotification = 0;
//		if(!world->mLocalErrorNotification) {
//			world->mLocalErrorNotification = world->mErrorNotification;
//		};
		return PacketPerformed;
	} else {
		// in real time engine, schedule the packet
		int64 time = OSCtime(packet->mData + 8);
		if (time == 0 || time == 1) {
			PerformOSCBundle(world, packet);
			return PacketPerformed;
		} else {
			if ((time < driver->mOSCbuftime) && (world->mVerbosity >= 0)) {
				double seconds = (driver->mOSCbuftime - time)*kOSCtoSecs;
				scprintf("late %.9f\n", seconds);
				//FifoMsg outMsg;

				//ReportLateness(packet->mReply, seconds)
			}
			// DEBUG
			// else
				//scprintf("scheduled in %.6f at time %.6f\n",
				//	(time-driver->mOSCbuftime)*kOSCtoSecs,
				//	(time-gStartupOSCTime)*kOSCtoSecs);

			SC_ScheduledEvent event(world, time, packet, freeFunc);
			driver->AddEvent(event);
			return PacketScheduled;
		}
	}
}

////////////////////////////////////////////////////////////////////////////

void Perform_ToEngine_Msg(FifoMsg *inMsg)
{
	World *world = inMsg->mWorld;
	OSC_Packet *packet = (OSC_Packet*)inMsg->mData;
	if (!packet) return;

	PacketStatus status = PerformOSCPacket(world, packet, SC_ScheduledEvent::FreeInNRT);
	if (status == PacketScheduled) {
		// Transfer ownership
		inMsg->mData = 0;
		inMsg->mFreeFunc = 0;
	}
}

PacketStatus PerformCompletionMsg(World *inWorld, const OSC_Packet& inPacket)
{
	OSC_Packet* packet = (OSC_Packet*)World_Alloc(inWorld, sizeof(OSC_Packet));
	*packet = inPacket;
	packet->mIsBundle = gIsBundle.checkIsBundle((int32*)packet->mData);
	PacketStatus status = PerformOSCPacket(inWorld, packet, SC_ScheduledEvent::FreeInRT);
	if (status == PacketPerformed) {
		World_Free(inWorld, packet);
	}
	return status;
}

void FreeOSCPacket(FifoMsg *inMsg)
{
	OSC_Packet *packet = (OSC_Packet*)inMsg->mData;
	if (packet) {
		inMsg->mData = 0;
#ifdef _WIN32
#pragma message("$$$todo fixme hack for the 'uninitialized packet->mData ptr when using MSVC 7.1 debug")
    if (packet->mData != reinterpret_cast<char*>(0xcdcdcdcd))
  		free(packet->mData);
#else //#ifdef _WIN32
    free(packet->mData);
#endif //#ifdef _WIN32
		free(packet);
	}
}

void Free_FromEngine_Msg(FifoMsg *inMsg);
void Free_FromEngine_Msg(FifoMsg *inMsg)
{
	World_Free(inMsg->mWorld, inMsg->mData);
}

// =====================================================================
// Audio driver (Common)

SC_AudioDriver::SC_AudioDriver(struct World *inWorld)
		: mWorld(inWorld)
        , mSampleTime(0)
        , mNumSamplesPerCallback(0)

{
}

SC_AudioDriver::~SC_AudioDriver()
{
    mRunThreadFlag = false;
	mAudioSync.Signal();
	pthread_join(mThread, 0);
}

void* audio_driver_thread_func(void* arg);
void* audio_driver_thread_func(void* arg)
{
	SC_AudioDriver *ca = (SC_AudioDriver*)arg;
	void* result = ca->RunThread();
	return result;
}

void* SC_AudioDriver::RunThread()
{
	TriggersFifo *trigfifo = &mWorld->hw->mTriggers;
	NodeReplyFifo *nodereplyfifo = &mWorld->hw->mNodeMsgs;
	NodeEndsFifo *nodeendfifo = &mWorld->hw->mNodeEnds;
	DeleteGraphDefsFifo *deletegraphfifo = &mWorld->hw->mDeleteGraphDefs;

	while (mRunThreadFlag) {
		// wait for sync
		mAudioSync.WaitNext();

		mWorld->mNRTLock->Lock();

		// send /tr messages
		trigfifo->Perform();

		// send node reply messages
		nodereplyfifo->Perform();

		// send node status messages
		nodeendfifo->Perform();

		// free GraphDefs
		deletegraphfifo->Perform();

		// perform messages
		mFromEngine.Perform();

		mWorld->mNRTLock->Unlock();
	}
	return 0;
}

bool SC_AudioDriver::SendMsgFromEngine(FifoMsg& inMsg)
{
	return mFromEngine.Write(inMsg);
}


bool SC_AudioDriver::SendMsgToEngine(FifoMsg& inMsg)
{
	mToEngine.Free();
	return mToEngine.Write(inMsg);
}

bool SC_AudioDriver::SendOscPacketMsgToEngine(FifoMsg& inMsg)
{
	mOscPacketsToEngine.Free();
	return mOscPacketsToEngine.Write(inMsg);
}

void SC_ScheduledEvent::FreeInRT(struct World* world, OSC_Packet* packet)
{
	World_Free(world, packet->mData);
	World_Free(world, packet);
}

void SC_ScheduledEvent::FreeInNRT(struct World* world, OSC_Packet* packet)
{
	FifoMsg msg;
	msg.Set(world, FreeOSCPacket, 0, (void*)packet);
	world->hw->mAudioDriver->SendMsgFromEngine(msg);
}

void SC_ScheduledEvent::Perform()
{
	PerformOSCBundle(mWorld, mPacket);
	(*mPacketFreeFunc)(mWorld, mPacket);
}

bool SC_AudioDriver::Setup()
{
	mRunThreadFlag = true;
	pthread_create (&mThread, NULL, audio_driver_thread_func, (void*)this);
	set_real_time_priority(mThread);

	int numSamples;
	double sampleRate;

	if (!DriverSetup(&numSamples, &sampleRate)) return false;

	mNumSamplesPerCallback = numSamples;
	//scprintf("mNumSamplesPerCallback %d\n", mNumSamplesPerCallback);
	//scprintf("mHardwareBufferSize %lu\n", mHardwareBufferSize);

	// compute a per sample increment to the OpenSoundControl Time
	mOSCincrementNumerator = (double)mWorld->mBufLength * pow(2.,32.);
	mOSCincrement = (int64)(mOSCincrementNumerator / sampleRate);
	mOSCtoSamples = sampleRate / pow(2.,32.);

	World_SetSampleRate(mWorld, sampleRate);
	mSampleRate = mSmoothSampleRate = sampleRate;
	mBuffersPerSecond = sampleRate / mNumSamplesPerCallback;
	mMaxPeakCounter = (int)mBuffersPerSecond;

	if(mWorld->mVerbosity >= 0){
		scprintf("SC_AudioDriver: sample rate = %f, driver's block size = %d\n", sampleRate, mNumSamplesPerCallback);
	}

	return true;
}

bool SC_AudioDriver::Start()
{
	mAvgCPU = 0.;
	mPeakCPU = 0.;
	mPeakCounter = 0;

	mStartHostSecs = 0.;
	mPrevHostSecs = 0.;
	mStartSampleTime = 0.;
	mPrevSampleTime = 0.;

	World_Start(mWorld);

	gStartupOSCTime = oscTimeNow();

	return DriverStart();
}

bool SC_AudioDriver::Stop()
{
	if (!DriverStop()) return false;
	return true;
}


// =====================================================================
// Audio driver (CoreAudio)
#if SC_AUDIO_API == SC_AUDIO_API_COREAUDIO

SC_AudioDriver* SC_NewAudioDriver(struct World *inWorld)
{
    return new SC_CoreAudioDriver(inWorld);
}

#endif

#if SC_AUDIO_API == SC_AUDIO_API_COREAUDIO ||  SC_AUDIO_API == SC_AUDIO_API_AUDIOUNITS

SC_CoreAudioDriver::SC_CoreAudioDriver(struct World *inWorld)
		: SC_AudioDriver(inWorld)
        , mInputBufList(0)
{
}

SC_CoreAudioDriver::~SC_CoreAudioDriver()
{
	if (mInputBufList)
	{
		int i;
		for (i=0; i<mInputBufList->mNumberBuffers; i++)
		{
			free(mInputBufList->mBuffers[i].mData);
		}
		free(mInputBufList);
	}
}

bool SC_CoreAudioDriver::DriverSetup(int* outNumSamplesPerCallback, double* outSampleRate)
{
	OSStatus	err = kAudioHardwareNoError;
	UInt32	count;
	mOutputDevice = kAudioDeviceUnknown;
	mInputDevice = kAudioDeviceUnknown;

	//scprintf("SC_CoreAudioDriver::Setup world %p\n", mWorld);

	////////////////////////////////////////////////////////////////////////////////////////////////

	do {
		err = AudioHardwareGetPropertyInfo(kAudioHardwarePropertyDevices, &count, 0);

		AudioDeviceID *devices = (AudioDeviceID*)malloc(count);
		err = AudioHardwareGetProperty(kAudioHardwarePropertyDevices, &count, devices);
		if (err != kAudioHardwareNoError) {
			scprintf("get kAudioHardwarePropertyDevices error %4.4s\n", (char*)&err);
			free(devices);
			break;
		}

		int numdevices = count / sizeof(AudioDeviceID);
		if(mWorld->mVerbosity >= 0){
			scprintf("Number of Devices: %d\n", numdevices);
		}
		for (int i = 0; i < numdevices; ++i) {
			err = AudioDeviceGetPropertyInfo(devices[i], 0, false, kAudioDevicePropertyDeviceName, &count, 0);
			if (err != kAudioHardwareNoError) {
				scprintf("info kAudioDevicePropertyDeviceName error %4.4s A %d %p\n", (char*)&err, i, devices[i]);
				break;
			}

			char *name = (char*)malloc(count);
			err = AudioDeviceGetProperty(devices[i], 0, false, kAudioDevicePropertyDeviceName, &count, name);
			if (err != kAudioHardwareNoError) {
				scprintf("get kAudioDevicePropertyDeviceName error %4.4s A %d %p\n", (char*)&err, i, devices[i]);
				free(name);
				break;
			}
			if(mWorld->mVerbosity >= 0){
				scprintf("   %d : \"%s\"\n", i, name);
			}
			free(name);
		}
		free(devices);
		if(mWorld->mVerbosity >= 0){
			scprintf("\n");
		}
	} while (false);

	if (mWorld->hw->mInDeviceName || mWorld->hw->mOutDeviceName) {
		err = AudioHardwareGetPropertyInfo(kAudioHardwarePropertyDevices, &count, 0);
		if (err != kAudioHardwareNoError) {
			scprintf("info kAudioHardwarePropertyDevices error %4.4s\n", (char*)&err);
			return false;
		}

		AudioDeviceID *devices = (AudioDeviceID*)malloc(count);
		err = AudioHardwareGetProperty(kAudioHardwarePropertyDevices, &count, devices);
		if (err != kAudioHardwareNoError) {
			scprintf("get kAudioHardwarePropertyDevices error %4.4s\n", (char*)&err);
			return false;
		}

		int numdevices = count / sizeof(AudioDeviceID);
		for (int i = 0; i < numdevices; ++i) {
			err = AudioDeviceGetPropertyInfo(devices[i], 0, false, kAudioDevicePropertyDeviceName, &count, 0);
			if (err != kAudioHardwareNoError) {
				scprintf("info kAudioDevicePropertyDeviceName error %4.4s B %d %p\n", (char*)&err, i, devices[i]);
				break;
			}

			char *name = (char*)malloc(count);
			err = AudioDeviceGetProperty(devices[i], 0, false, kAudioDevicePropertyDeviceName, &count, name);
			if (err != kAudioHardwareNoError) {
				scprintf("get kAudioDevicePropertyDeviceName error %4.4s B %d %p\n", (char*)&err, i, devices[i]);
				return false;
			}
			if (strcmp(name, mWorld->hw->mInDeviceName) == 0) {
				mInputDevice = devices[i];
			}
			if (strcmp(name, mWorld->hw->mOutDeviceName) == 0) {
				mOutputDevice = devices[i];
			}
			free(name);
			if (mInputDevice!=kAudioDeviceUnknown && mOutputDevice!=kAudioDeviceUnknown) break;
		}
		free(devices);
		if (mOutputDevice==kAudioDeviceUnknown || mInputDevice==kAudioDeviceUnknown) goto getDefault;
	} else {
		getDefault:

		// get the default output device for the HAL
		if (mOutputDevice==kAudioDeviceUnknown)
		{
			count = sizeof(mOutputDevice);
			//get the output device:
			err = AudioHardwareGetProperty(kAudioHardwarePropertyDefaultOutputDevice, &count, (void *) & mOutputDevice);
			if (err != kAudioHardwareNoError) {
				scprintf("get kAudioHardwarePropertyDefaultOutputDevice error %4.4s\n", (char*)&err);
				return false;
			}
		}

		//get the input device
		if (mInputDevice==kAudioDeviceUnknown)
		{
			count = sizeof(mInputDevice);
			err = AudioHardwareGetProperty(kAudioHardwarePropertyDefaultInputDevice, &count, (void *) & mInputDevice);
			//get the input device:
			if (err != kAudioHardwareNoError) {
				scprintf("get kAudioHardwarePropertyDefaultInputDevice error %4.4s\n", (char*)&err);
				return false;
			}
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	AudioTimeStamp	now;
	now.mFlags = kAudioTimeStampHostTimeValid;
	now.mHostTime = AudioGetCurrentHostTime();

	if (mPreferredHardwareBufferFrameSize)
	{

		count = sizeof(UInt32);
		err = AudioDeviceSetProperty(mOutputDevice, &now, 0, false, kAudioDevicePropertyBufferFrameSize, count, &mPreferredHardwareBufferFrameSize);
		if (err != kAudioHardwareNoError) {
			scprintf("set kAudioDevicePropertyBufferFrameSize error %4.4s\n", (char*)&err);
			//return false;
		}
		if (UseSeparateIO())
		{
			count = sizeof(UInt32);
			err = AudioDeviceSetProperty(mOutputDevice, &now, 0, false, kAudioDevicePropertyBufferFrameSize, count, &mPreferredHardwareBufferFrameSize);
			if (err != kAudioHardwareNoError) {
				scprintf("set kAudioDevicePropertyNominalSampleRate error %4.4s\n", (char*)&err);
				//return false;
			}
		}
	}

	if (mPreferredSampleRate)
	{
		Float64 sampleRate = mPreferredSampleRate;
		count = sizeof(Float64);
		err = AudioDeviceSetProperty(mOutputDevice, &now, 0, false, kAudioDevicePropertyNominalSampleRate, count, &sampleRate);
		if (err != kAudioHardwareNoError) {
			scprintf("set kAudioDevicePropertyNominalSampleRate error %4.4s\n", (char*)&err);
			//return false;
		}
		if (UseSeparateIO())
		{
			count = sizeof(Float64);
			err = AudioDeviceSetProperty(mInputDevice, &now, 0, false, kAudioDevicePropertyNominalSampleRate, count, &sampleRate);
			if (err != kAudioHardwareNoError) {
				scprintf("set kAudioDevicePropertyNominalSampleRate error %4.4s\n", (char*)&err);
				//return false;
			}
		}
	}

	// get the buffersize for the out device
	count = sizeof(mHardwareBufferSize);
	err = AudioDeviceGetProperty(mOutputDevice, 0, false, kAudioDevicePropertyBufferSize, &count, &mHardwareBufferSize);
	if (err != kAudioHardwareNoError) {
		scprintf("get kAudioDevicePropertyBufferSize error %4.4s\n", (char*)&err);
		return false;
	}
	//scprintf("mHardwareBufferSize = %ld\n", mHardwareBufferSize);

	// get a description of the data format used by the output device
	count = sizeof(AudioStreamBasicDescription);
	err = AudioDeviceGetProperty(mOutputDevice, 0, false, kAudioDevicePropertyStreamFormat, &count, &outputStreamDesc);
	if (err != kAudioHardwareNoError) {
		scprintf("get kAudioDevicePropertyStreamFormat error %4.4s\n", (char*)&err);
		return false;
	}

	if (mInputDevice != kAudioDeviceUnknown) {
		// get a description of the data format used by the input device
		count = sizeof(AudioStreamBasicDescription);
		err = AudioDeviceGetProperty(mInputDevice, 0, true, kAudioDevicePropertyStreamFormat, &count, &inputStreamDesc);
		if (err != kAudioHardwareNoError) {
			scprintf("get kAudioDevicePropertyStreamFormat error %4.4s\n", (char*)&err);
			return false;
		}

		if (inputStreamDesc.mSampleRate != outputStreamDesc.mSampleRate) {
			scprintf("input and output sample rates do not match. %g != %g\n", inputStreamDesc.mSampleRate, outputStreamDesc.mSampleRate);
			return false;
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	do {
		err = AudioDeviceGetPropertyInfo(mInputDevice, 0, false, kAudioDevicePropertyDeviceName, &count, 0);
		if (err != kAudioHardwareNoError) {
			scprintf("info kAudioDevicePropertyDeviceName error %4.4s C %p\n", (char*)&err, mInputDevice);
			break;
		}

		char *name = (char*)malloc(count);
		err = AudioDeviceGetProperty(mInputDevice, 0, false, kAudioDevicePropertyDeviceName, &count, name);
		if (err != kAudioHardwareNoError) {
			scprintf("get kAudioDevicePropertyDeviceName error %4.4s C %p\n", (char*)&err, mInputDevice);
			free(name);
			break;
		}

		if(mWorld->mVerbosity >= 0){
			scprintf("\"%s\" Input Device\n", name);
		}
		free(name);

		Boolean writeable;
		err = AudioDeviceGetPropertyInfo(mInputDevice, 0, 1, kAudioDevicePropertyStreamConfiguration,
										 &count, &writeable);
		if (err != kAudioHardwareNoError) {
			scprintf("info kAudioDevicePropertyStreamConfiguration error %4.4s\n", (char*)&err);
			break;
		}

		AudioBufferList *bufList = (AudioBufferList*)malloc(count);
		err = AudioDeviceGetProperty(mInputDevice, 0, 1, kAudioDevicePropertyStreamConfiguration,
									 &count, bufList);
		if (err != kAudioHardwareNoError) {
			scprintf("get kAudioDevicePropertyStreamConfiguration error %4.4s\n", (char*)&err);
			free(bufList);
			break;
		}

		if(mWorld->mVerbosity >= 0){
			scprintf("   Streams: %d\n", bufList->mNumberBuffers);
			for (unsigned int j = 0; j < bufList->mNumberBuffers; ++j) {
				scprintf("      %d  channels %d\n", j, bufList->mBuffers[j].mNumberChannels);
			}
		}

		free(bufList);
	} while (false);
	if(mWorld->mVerbosity >= 0){
		scprintf("\n");
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	do {
		err = AudioDeviceGetPropertyInfo(mOutputDevice, 0, false, kAudioDevicePropertyDeviceName, &count, 0);

		char *name = (char*)malloc(count);
		err = AudioDeviceGetProperty(mOutputDevice, 0, false, kAudioDevicePropertyDeviceName, &count, name);
		if (err != kAudioHardwareNoError) {
			scprintf("get kAudioDevicePropertyDeviceName error %4.4s\n", (char*)&err);
			free(name);
			break;
		}

		if(mWorld->mVerbosity >= 0){
			scprintf("\"%s\" Output Device\n", name);
		}
		free(name);

		Boolean writeable;
		err = AudioDeviceGetPropertyInfo(mOutputDevice, 0, 0, kAudioDevicePropertyStreamConfiguration,
										 &count, &writeable);
		if (err != kAudioHardwareNoError) {
			scprintf("info kAudioDevicePropertyStreamConfiguration error %4.4s\n", (char*)&err);
			break;
		}

		AudioBufferList *bufList = (AudioBufferList*)malloc(count);
		err = AudioDeviceGetProperty(mOutputDevice, 0, 0, kAudioDevicePropertyStreamConfiguration,
									 &count, bufList);
		if (err != kAudioHardwareNoError) {
			scprintf("get kAudioDevicePropertyStreamConfiguration error %4.4s\n", (char*)&err);
			free(bufList);
			break;
		}

		if(mWorld->mVerbosity >= 0){
			scprintf("   Streams: %d\n", bufList->mNumberBuffers);
			for (unsigned int j = 0; j < bufList->mNumberBuffers; ++j) {
				scprintf("      %d  channels %d\n", j, bufList->mBuffers[j].mNumberChannels);
			}
		}
		free(bufList);
	} while (false);
	if(mWorld->mVerbosity >= 0){
		scprintf("\n");
	}

	////////////////////////////////////////////////////////////////////////////////////////////////


	if (UseSeparateIO()) {
		count = sizeof(UInt32);
		err = AudioDeviceGetProperty(mInputDevice, 0, true, kAudioDevicePropertySafetyOffset, &count, &mSafetyOffset);
		if (err != kAudioHardwareNoError) {
			scprintf("get kAudioDevicePropertySafetyOffset error %4.4s\n", (char*)&err);
			return false;
		}
		if(mWorld->mVerbosity >= 1){
			scprintf("mSafetyOffset %lu\n", mSafetyOffset);
		}

		Boolean writeable;
		err = AudioDeviceGetPropertyInfo(mInputDevice, 0, true, kAudioDevicePropertyStreamConfiguration, &count, &writeable);
		mInputBufList = (AudioBufferList*)malloc(count);
		err = AudioDeviceGetProperty(mInputDevice, 0, true, kAudioDevicePropertyStreamConfiguration, &count, mInputBufList);
		if (err != kAudioHardwareNoError) {
			scprintf("get kAudioDevicePropertyStreamConfiguration error %4.4s\n", (char*)&err);
			return false;
		}

		if(mWorld->mVerbosity >= 1){
			scprintf("mNumberBuffers %lu\n", mInputBufList->mNumberBuffers);
		}
		for (uint32 i=0; i<mInputBufList->mNumberBuffers; ++i) {
			if(mWorld->mVerbosity >= 1){
				scprintf("  mDataByteSize %d %lu\n", i, mInputBufList->mBuffers[i].mDataByteSize);
			}
			mInputBufList->mBuffers[i].mData = zalloc(1, mInputBufList->mBuffers[i].mDataByteSize);
		}

		/*
		AudioTimeStamp	now;
		now.mFlags = kAudioTimeStampHostTimeValid;
		now.mHostTime = AudioGetCurrentHostTime();


		err = AudioDeviceSetProperty(mInputDevice, &now, 0, true, kAudioDevicePropertyRegisterBufferList, count, mInputBufList);
		if (err != kAudioHardwareNoError) {
			scprintf("get kAudioDevicePropertyRegisterBufferList error %4.4s\n", (char*)&err);
			return false;
		}
		*/
	}

	*outNumSamplesPerCallback = mHardwareBufferSize / outputStreamDesc.mBytesPerFrame;
	*outSampleRate = outputStreamDesc.mSampleRate;

	if(mWorld->mVerbosity >= 1){
		scprintf("<-SC_CoreAudioDriver::Setup world %p\n", mWorld);
	}



	//check if using built-in output, and thus whether there could be headphone plug/un-plug issues
	//our assumption otherwise is that we don't respond, and SC will stay with the pre-arranged or default device, and not restart just because headphones switched

	err = AudioDeviceGetPropertyInfo(mOutputDevice, 0, false, kAudioDevicePropertyDeviceName, &count, 0);
	if (err != kAudioHardwareNoError) {
		scprintf("info kAudioDevicePropertyDeviceName error %4.4s %p\n", (char*)&err, mOutputDevice);
		return false;
	}

	char *outputname = (char*)malloc(count);
	const char *testname = "Built-in Output";
	err = AudioDeviceGetProperty(mOutputDevice, 0, false, kAudioDevicePropertyDeviceName, &count, outputname);
	if (err != kAudioHardwareNoError) {
		scprintf("get kAudioDevicePropertyDeviceName error %4.4s %p\n", (char*)&err, mOutputDevice);
		return false;
	}
	builtinoutputflag_ = 0;

	if (strcmp(testname, outputname) == 0) {
		builtinoutputflag_ = 1;
	}
// else {
//
//		//check for an Aggregate Devices with a subdevice which is Built-in Output
//		//http://lists.apple.com/archives/coreaudio-api/2009/Oct/msg00182.html
//
//
//	}

	free(outputname);


	return true;
}

/*
OSStatus appIOProc2 (AudioDeviceID inDevice, const AudioTimeStamp* inNow,
					const AudioBufferList* inInputData,
					const AudioTimeStamp* inInputTime,
					AudioBufferList* outOutputData,
					const AudioTimeStamp* inOutputTime,
					void* defptr);
OSStatus appIOProc2 (AudioDeviceID inDevice, const AudioTimeStamp* inNow,
					const AudioBufferList* inInputData,
					const AudioTimeStamp* inInputTime,
					AudioBufferList* outOutputData,
					const AudioTimeStamp* inOutputTime,
					void* defptr)
{
	SC_CoreAudioDriver* def = (SC_CoreAudioDriver*)defptr;

	int64 oscTime = CoreAudioHostTimeToOSC(inOutputTime->mHostTime);

	AudioTimeStamp readTime;
	readTime.mSampleTime = inNow->mSampleTime - def->SafetyOffset() - def->NumSamplesPerCallback();
	readTime.mFlags = kAudioTimeStampSampleTimeValid;

	AudioDeviceRead(def->InputDevice(), &readTime, def->GetInputBufferList());

	def->Run(def->GetInputBufferList(), outOutputData, oscTime);

	return kAudioHardwareNoError;
}
*/


OSStatus appIOProcSeparateIn (AudioDeviceID device, const AudioTimeStamp* inNow,
					const AudioBufferList* inInputData,
					const AudioTimeStamp* inInputTime,
					AudioBufferList* outOutputData,
					const AudioTimeStamp* inOutputTime,
					void* defptr)
{
	SC_CoreAudioDriver* def = (SC_CoreAudioDriver*)defptr;

	// copy input data to driver's private buffer list
	int i;
	for (i=0; i<inInputData->mNumberBuffers; i++)
	{
		memcpy(def->mInputBufList->mBuffers[i].mData, inInputData->mBuffers[i].mData, inInputData->mBuffers[i].mDataByteSize);
	}

	return kAudioHardwareNoError;
}

OSStatus appIOProc (AudioDeviceID device, const AudioTimeStamp* inNow,
					const AudioBufferList* inInputData,
					const AudioTimeStamp* inInputTime,
					AudioBufferList* outOutputData,
					const AudioTimeStamp* inOutputTime,
					void* defptr)
{
	SC_CoreAudioDriver* def = (SC_CoreAudioDriver*)defptr;
	int64 oscTime = CoreAudioHostTimeToOSC(inOutputTime->mHostTime);

	double hostSecs = (double)AudioConvertHostTimeToNanos(inOutputTime->mHostTime) * 1e-9;
	double sampleTime = inOutputTime->mSampleTime;
	if (def->mStartHostSecs == 0) {
		def->mStartHostSecs = hostSecs;
		def->mStartSampleTime = sampleTime;
	} else {
		double instSampleRate = (sampleTime -  def->mPrevSampleTime)/(hostSecs -  def->mPrevHostSecs);
		double smoothSampleRate = def->mSmoothSampleRate;
		smoothSampleRate = smoothSampleRate + 0.002 * (instSampleRate - smoothSampleRate);
		def->mOSCincrement = (int64)(def->mOSCincrementNumerator / smoothSampleRate);
		def->mSmoothSampleRate = smoothSampleRate;

#if 0
		double avgSampleRate  = (sampleTime - def->mStartSampleTime)/(hostSecs - def->mStartHostSecs);
		double jitter = (smoothSampleRate * (hostSecs - def->mPrevHostSecs)) - (sampleTime - def->mPrevSampleTime);
		double drift = (smoothSampleRate - def->mSampleRate) * (hostSecs - def->mStartHostSecs);
		//if (fabs(jitter) > 0.01) {
			scprintf("avgSR %.6f   smoothSR %.6f   instSR %.6f   jitter %.6f   drift %.6f   inc %lld\n",
				avgSampleRate, smoothSampleRate, instSampleRate, jitter, drift, def->mOSCincrement);
		//}
#endif
	}
	def->mPrevHostSecs = hostSecs;
	def->mPrevSampleTime = sampleTime;

	if (!def->UseSeparateIO())
	{
		def->Run(inInputData, outOutputData, oscTime);
		return kAudioHardwareNoError;
	}


	def->Run(def->mInputBufList, outOutputData, oscTime);
	return kAudioHardwareNoError;
}

void SC_CoreAudioDriver::Run(const AudioBufferList* inInputData,
					AudioBufferList* outOutputData, int64 oscTime)
{
	int64 systemTimeBefore = AudioGetCurrentHostTime();
	World *world = mWorld;

	try {
		int numSamplesPerCallback = NumSamplesPerCallback();
		mOSCbuftime = oscTime;

#ifdef __APPLE__
		sc_SetDenormalFlags();
#endif

		mFromEngine.Free();
		/*if (mToEngine.HasData()) {
			scprintf("oscTime %.9f %.9f\n", oscTime*kOSCtoSecs, CoreAudioHostTimeToOSC(AudioGetCurrentHostTime())*kOSCtoSecs);
		}*/
		mToEngine.Perform();
		mOscPacketsToEngine.Perform();

		int bufFrames = world->mBufLength;
		int numBufs = numSamplesPerCallback / bufFrames;

		int numInputBuses    = world->mNumInputs;
		int numOutputBuses   = world->mNumOutputs;
		float* inputBuses    = world->mAudioBus + world->mNumOutputs * bufFrames;
		float* outputBuses   = world->mAudioBus;
		int32* inputTouched  = world->mAudioBusTouched + world->mNumOutputs;
		int32* outputTouched = world->mAudioBusTouched;
		int numInputStreams  = inInputData ? inInputData->mNumberBuffers : 0;
		int numOutputStreams = outOutputData ? outOutputData->mNumberBuffers : 0;

		//static int go = 0;

		int64 oscInc = mOSCincrement;
		double oscToSamples = mOSCtoSamples;

		int bufFramePos = 0;

		for (int i = 0; i < numBufs; ++i, world->mBufCounter++, bufFramePos += bufFrames) {
			int32 bufCounter = world->mBufCounter;

			// de-interleave input
			if (inInputData) {
				const AudioBuffer* inInputDataBuffers = inInputData->mBuffers;
				for (int s = 0, b = 0; b<numInputBuses && s < numInputStreams; s++) {
					const AudioBuffer* buf = inInputDataBuffers + s;
					int nchan = buf->mNumberChannels;
					if (buf->mData) {
						float *busdata = inputBuses + b * bufFrames;
						float *bufdata = (float*)buf->mData + bufFramePos * nchan;
						if (nchan == 1) {
							for (int k=0; k<bufFrames; ++k) {
								busdata[k] = bufdata[k];
							}
							inputTouched[b] = bufCounter;
						} else {
							int minchan = sc_min(nchan, numInputBuses - b);
							for (int j=0; j<minchan; ++j, busdata += bufFrames) {
								for (int k=0, m=j; k<bufFrames; ++k, m += nchan) {
									busdata[k] = bufdata[m];
								}
								inputTouched[b+j] = bufCounter;
							}
						}
						b += nchan;
					}
				}
			}
			//count++;

			int64 schedTime;
			int64 nextTime = oscTime + oscInc;

			/*if (mScheduler.Ready(nextTime)) {
				double diff = (mScheduler.NextTime() - mOSCbuftime)*kOSCtoSecs;
				scprintf("rdy %.9f %.9f %.9f\n", (mScheduler.NextTime()-gStartupOSCTime) * kOSCtoSecs, (mOSCbuftime-gStartupOSCTime)*kOSCtoSecs, diff);
			}*/

			while ((schedTime = mScheduler.NextTime()) <= nextTime) {
				float diffTime = (float)(schedTime - oscTime) * oscToSamples + 0.5;
				float diffTimeFloor = floor(diffTime);
				world->mSampleOffset = (int)diffTimeFloor;
				world->mSubsampleOffset = diffTime - diffTimeFloor;

				if (world->mSampleOffset < 0) world->mSampleOffset = 0;
				else if (world->mSampleOffset >= world->mBufLength) world->mSampleOffset = world->mBufLength-1;

				SC_ScheduledEvent event = mScheduler.Remove();
				event.Perform();
			}
			world->mSampleOffset = 0;
			world->mSubsampleOffset = 0.f;

			World_Run(world);

			// interleave output
			AudioBuffer* outOutputDataBuffers = outOutputData->mBuffers;
			for (int s = 0, b = 0; b<numOutputBuses && s < numOutputStreams; s++) {
				AudioBuffer* buf = outOutputDataBuffers + s;
				int nchan = buf->mNumberChannels;
				if (buf->mData) {
					float *busdata = outputBuses + b * bufFrames;
					float *bufdata = (float*)buf->mData + bufFramePos * nchan;
					if (nchan == 1) {
						if (outputTouched[b] == bufCounter) {
							for (int k=0; k<bufFrames; ++k) {
								bufdata[k] = busdata[k];
							}
						}
					} else {
						int minchan = sc_min(nchan, numOutputBuses - b);
						for (int j=0; j<minchan; ++j, busdata += bufFrames) {
							if (outputTouched[b+j] == bufCounter) {
								for (int k=0, m=j; k<bufFrames; ++k, m += nchan) {
									bufdata[m] = busdata[k];
								}
							}
						}
					}
					b += nchan;
				}
			}
			oscTime = mOSCbuftime = nextTime;
		}
	} catch (std::exception& exc) {
		scprintf("exception in real time: %s\n", exc.what());
	} catch (...) {
		scprintf("unknown exception in real time\n");
	}
	int64 systemTimeAfter = AudioGetCurrentHostTime();
	double calcTime = (double)AudioConvertHostTimeToNanos(systemTimeAfter - systemTimeBefore) * 1e-9;
	double cpuUsage = calcTime * mBuffersPerSecond * 100.;
	mAvgCPU = mAvgCPU + 0.1 * (cpuUsage - mAvgCPU);
	if (cpuUsage > mPeakCPU || --mPeakCounter <= 0)
	{
		mPeakCPU = cpuUsage;
		mPeakCounter = mMaxPeakCounter;
	}

	mAudioSync.Signal();
}




//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
// These are not linked in yet, but we'll need to listen for the properties and stop/restart synthesis
// if sample-rate, format, or device change.

OSStatus	hardwareListenerProc (	AudioHardwarePropertyID	inPropertyID,
								  void*					inClientData)
{
    OSStatus			err = noErr;
    char				cStr[255];
    UInt32				outSize;
    Boolean				outWritable;
    AudioDeviceID		deviceID;

    switch(inPropertyID)
    {
        case kAudioHardwarePropertyDefaultOutputDevice:
            scprintf("%s\n", "***** HARDWARE NOTIFICATION - kAudioHardwarePropertyDefaultOutputDevice\r");
            err =  AudioHardwareGetPropertyInfo(kAudioHardwarePropertyDefaultOutputDevice,  &outSize, &outWritable);
			if (err) break;
			err = AudioHardwareGetProperty(kAudioHardwarePropertyDefaultOutputDevice, &outSize, &deviceID);
			if (err) break;
			err =  AudioDeviceGetPropertyInfo(deviceID, 0, false, kAudioDevicePropertyDeviceName,  &outSize, &outWritable);
			if (err) break;
			err = AudioDeviceGetProperty(deviceID, 0, false, kAudioDevicePropertyDeviceName, &outSize, cStr);
			if (err) break;

			// do something

            break;

        case kAudioHardwarePropertyDefaultInputDevice:
            scprintf("%s\n", "***** HARDWARE NOTIFICATION - kAudioHardwarePropertyDefaultInputDevice\r");
            err =  AudioHardwareGetPropertyInfo(kAudioHardwarePropertyDefaultInputDevice,  &outSize, &outWritable);
			if (err) break;
			err = AudioHardwareGetProperty(kAudioHardwarePropertyDefaultInputDevice, &outSize, &deviceID);
			if (err) break;
			err =  AudioDeviceGetPropertyInfo(deviceID, 0, false, kAudioDevicePropertyDeviceName,  &outSize, &outWritable);
			if (err) break;
			err = AudioDeviceGetProperty(deviceID, 0, false, kAudioDevicePropertyDeviceName, &outSize, cStr);
			if (err) break;

			// do something

            break;

        case kAudioHardwarePropertyDefaultSystemOutputDevice:
            scprintf("%s\n", "***** HARDWARE NOTIFICATION - kAudioHardwarePropertyDefaultSystemOutputDevice\r");
            err =  AudioHardwareGetPropertyInfo(kAudioHardwarePropertyDefaultSystemOutputDevice,  &outSize, &outWritable);
			if (err) break;
			err = AudioHardwareGetProperty(kAudioHardwarePropertyDefaultSystemOutputDevice, &outSize, &deviceID);
			if (err) break;
			err =  AudioDeviceGetPropertyInfo(deviceID, 0, false, kAudioDevicePropertyDeviceName,  &outSize, &outWritable);
			if (err) break;
			err = AudioDeviceGetProperty(deviceID, 0, false, kAudioDevicePropertyDeviceName, &outSize, cStr);
			if (err) break;

			// do something

            break;

        case kAudioHardwarePropertyDevices:
        {
            scprintf("%s\n", "***** HARDWARE NOTIFICATION - kAudioHardwarePropertyDevices\r");
        }
            break;
		default:
			scprintf("%s\n", "***** HARDWARE NOTIFICATION - %4.4s\r", &inPropertyID);
	}

    fflush(stdout);
    return (noErr);
}




OSStatus AddDeviceListeners(AudioDeviceID inDevice, void *inClientData);

OSStatus AddHardwareListeners(void* inClientData);
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
OSStatus AddHardwareListeners(void* inClientData)
{
    OSStatus			err = noErr;

	//non deprecated but requires AudiObject, bleargh
	//err= AudioObjectAddPropertyListener(AudioObject, kAudioHardwarePropertyDefaultOutputDevice, hardwareListenerProc, inClientData);

    err = AudioHardwareAddPropertyListener(kAudioHardwarePropertyDefaultOutputDevice, hardwareListenerProc, inClientData);
    if (err) return err;

    err = AudioHardwareAddPropertyListener(kAudioHardwarePropertyDefaultInputDevice, hardwareListenerProc, inClientData);
    if (err) return err;

	//doesn't matter? Only default looked at by SC?
	err = AudioHardwareAddPropertyListener(kAudioHardwarePropertyDefaultSystemOutputDevice, hardwareListenerProc, inClientData);
    if (err) return err;

    err = AudioHardwareAddPropertyListener(kAudioHardwarePropertyDevices, hardwareListenerProc, inClientData);
    if (err) return err;

    return (err);
}

bool SC_CoreAudioDriver::DriverStart()
{
	if(mWorld->mVerbosity >= 1){
		scprintf("->SC_CoreAudioDriver::DriverStart\n");
	}
	OSStatus	err = kAudioHardwareNoError;
	AudioTimeStamp	now;
	UInt32 propertySize;
	Boolean writable;

	if(mWorld->mVerbosity >= 1){
		scprintf("start   UseSeparateIO?: %d\n", UseSeparateIO());
	}

	try {
	if (UseSeparateIO()) {
		err = AudioDeviceAddIOProc(mOutputDevice, appIOProc, (void *) this);	// setup Out device with an IO proc
		if (err != kAudioHardwareNoError) {
			scprintf("AudioDeviceAddIOProc failed %s %d\n", &err, (int)err);
			return false;
		}

		err = AudioDeviceAddIOProc(mInputDevice, appIOProcSeparateIn, (void *) this);		// setup In device with an IO proc
		if (err != kAudioHardwareNoError) {
			scprintf("AudioDeviceAddIOProc failed %s %d\n", &err, (int)err);
			return false;
		}


		if (mWorld->hw->mInputStreamsEnabled) {
			err = AudioDeviceGetPropertyInfo(mInputDevice, 0, true, kAudioDevicePropertyIOProcStreamUsage, &propertySize, &writable);
			AudioHardwareIOProcStreamUsage *su = (AudioHardwareIOProcStreamUsage*)malloc(propertySize);
			su->mIOProc = (void*)appIOProcSeparateIn;
			err = AudioDeviceGetProperty(mInputDevice, 0, true, kAudioDevicePropertyIOProcStreamUsage, &propertySize, su);
			int len = std::min(su->mNumberStreams, (UInt32)strlen(mWorld->hw->mInputStreamsEnabled));
			for (int i=0; i<len; ++i) {
				su->mStreamIsOn[i] = mWorld->hw->mInputStreamsEnabled[i] == '1';
			}
			err = AudioDeviceSetProperty(mInputDevice, &now, 0, true, kAudioDevicePropertyIOProcStreamUsage, propertySize, su);
		}

		if (mWorld->hw->mOutputStreamsEnabled) {
			err = AudioDeviceGetPropertyInfo(mOutputDevice, 0, false, kAudioDevicePropertyIOProcStreamUsage, &propertySize, &writable);
			AudioHardwareIOProcStreamUsage *su = (AudioHardwareIOProcStreamUsage*)malloc(propertySize);
			su->mIOProc = (void*)appIOProc;
			err = AudioDeviceGetProperty(mOutputDevice, 0, false, kAudioDevicePropertyIOProcStreamUsage, &propertySize, su);
			int len = std::min(su->mNumberStreams, (UInt32)strlen(mWorld->hw->mOutputStreamsEnabled));
			for (int i=0; i<len; ++i) {
				su->mStreamIsOn[i] = mWorld->hw->mOutputStreamsEnabled[i] == '1';
			}
			err = AudioDeviceSetProperty(mOutputDevice, &now, 0, false, kAudioDevicePropertyIOProcStreamUsage, propertySize, su);
		}

		err = AudioDeviceStart(mInputDevice, appIOProcSeparateIn);		// start playing sound through the device
		if (err != kAudioHardwareNoError) {
			scprintf("AudioDeviceStart failed %d\n", (int)err);
			return false;
		}

		err = AudioDeviceStart(mOutputDevice, appIOProc);		// start playing sound through the device
		if (err != kAudioHardwareNoError) {
			scprintf("AudioDeviceStart failed %d\n", (int)err);
			err = AudioDeviceStop(mInputDevice, appIOProcSeparateIn);			// stop playing sound through the device
			return false;
		}
	} else {
		err = AudioDeviceAddIOProc(mOutputDevice, appIOProc, (void *) this);	// setup our device with an IO proc
		if (err != kAudioHardwareNoError) {
			scprintf("AudioDeviceAddIOProc failed %d\n", (int)err);
			return false;
		}

		if (mWorld->hw->mInputStreamsEnabled) {
			err = AudioDeviceGetPropertyInfo(mOutputDevice, 0, true, kAudioDevicePropertyIOProcStreamUsage, &propertySize, &writable);
			AudioHardwareIOProcStreamUsage *su = (AudioHardwareIOProcStreamUsage*)malloc(propertySize);
			su->mIOProc = (void*)appIOProc;
			err = AudioDeviceGetProperty(mOutputDevice, 0, true, kAudioDevicePropertyIOProcStreamUsage, &propertySize, su);
			int len = std::min(su->mNumberStreams, (UInt32)strlen(mWorld->hw->mInputStreamsEnabled));
			for (int i=0; i<len; ++i) {
				su->mStreamIsOn[i] = mWorld->hw->mInputStreamsEnabled[i] == '1';
			}
			err = AudioDeviceSetProperty(mOutputDevice, &now, 0, true, kAudioDevicePropertyIOProcStreamUsage, propertySize, su);
		}

		if (mWorld->hw->mOutputStreamsEnabled) {
			err = AudioDeviceGetPropertyInfo(mOutputDevice, 0, false, kAudioDevicePropertyIOProcStreamUsage, &propertySize, &writable);
			AudioHardwareIOProcStreamUsage *su = (AudioHardwareIOProcStreamUsage*)malloc(propertySize);
			su->mIOProc = (void*)appIOProc;
			err = AudioDeviceGetProperty(mOutputDevice, 0, false, kAudioDevicePropertyIOProcStreamUsage, &propertySize, su);
			int len = std::min(su->mNumberStreams, (UInt32)strlen(mWorld->hw->mOutputStreamsEnabled));
			for (int i=0; i<len; ++i) {
				su->mStreamIsOn[i] = mWorld->hw->mOutputStreamsEnabled[i] == '1';
			}
			err = AudioDeviceSetProperty(mOutputDevice, &now, 0, false, kAudioDevicePropertyIOProcStreamUsage, propertySize, su);
		}

		err = AudioDeviceStart(mOutputDevice, appIOProc);		// start playing sound through the device
		if (err != kAudioHardwareNoError) {
			scprintf("AudioDeviceStart failed %d\n", (int)err);
			return false;
		}
	}
	} catch (...) {
	scprintf("exception in SC_CoreAudioDriver::DriverStart\n");
	}
	if(mWorld->mVerbosity >= 1){
		scprintf("<-SC_CoreAudioDriver::DriverStart\n");
	}


	//http://lists.apple.com/archives/coreaudio-api/2010/Aug/msg00114.html
	CFRunLoopRef theRunLoop =  NULL;
	AudioObjectPropertyAddress theAddress = { kAudioHardwarePropertyRunLoop, kAudioObjectPropertyScopeGlobal, kAudioObjectPropertyElementMaster };
	AudioObjectSetPropertyData(kAudioObjectSystemObject, &theAddress, 0, NULL, sizeof(CFRunLoopRef), &theRunLoop);

	//for now no spotting of hardware changes, assumption is that ServerOptions inviolate. However, if a device was unplugged, could react to loss of that device
	//by switching to system default?
	//AddHardwareListeners(NULL);
	//note that the number of listeners is stripped down to only one for now, to react to headphone swaps in the case of Built-in Output
	AddDeviceListeners(mOutputDevice, this);

	return true;
}


bool SC_CoreAudioDriver::StopStart() {

	bool test = DriverStop();

	bool test2 = DriverStart();
	return test && test2;
}


bool SC_CoreAudioDriver::DriverStop()
{
	if(mWorld->mVerbosity >= 1){
		scprintf("->SC_CoreAudioDriver::DriverStop\n");
	}
	OSStatus err = kAudioHardwareNoError;

	if (UseSeparateIO()) {
		err = AudioDeviceStop(mOutputDevice, appIOProc);
		if (err != kAudioHardwareNoError) {
			scprintf("AudioDeviceStop A failed %p\n", err);
			return false;
		}
		err = AudioDeviceRemoveIOProc(mOutputDevice, appIOProc);
		if (err != kAudioHardwareNoError) {
			scprintf("AudioDeviceRemoveIOProc A failed %p\n", err);
			return false;
		}

		err = AudioDeviceStop(mInputDevice, appIOProcSeparateIn);
		if (err != kAudioHardwareNoError) {
			scprintf("AudioDeviceStop A failed %p\n", err);
			return false;
		}

		err = AudioDeviceRemoveIOProc(mInputDevice, appIOProcSeparateIn);
		if (err != kAudioHardwareNoError) {
			scprintf("AudioDeviceRemoveIOProc A failed %p\n", err);
			return false;
		}
	} else {
		err = AudioDeviceStop(mOutputDevice, appIOProc);
		if (err != kAudioHardwareNoError) {
			scprintf("AudioDeviceStop B failed %p\n", err);
			return false;
		}

		err = AudioDeviceRemoveIOProc(mOutputDevice, appIOProc);
		if (err != kAudioHardwareNoError) {
			scprintf("AudioDeviceRemoveIOProc B failed %p\n", err);
			return false;
		}
	}
	if(mWorld->mVerbosity >= 1){
		scprintf("<-SC_CoreAudioDriver::DriverStop\n");
	}
	return true;
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Listen for Device Properties and update interface and globals
OSStatus	deviceListenerProc (	AudioDeviceID			inDevice,
									UInt32					inLine,
									Boolean					isInput,
									AudioDevicePropertyID	inPropertyID,
									void*					inClientData)
{
    OSStatus			err = noErr;
    UInt32 				outSize,
                        theUIntData,
						mute,
						playThru;
    UInt32				tLong;
	Float32				vol;


	SC_CoreAudioDriver* coredriver = (SC_CoreAudioDriver*) inClientData;

    switch(inPropertyID)
    {
 //       case kAudioDevicePropertyBufferSize:
//            scprintf("%s\n", "***** DEVICE NOTIFICATION - kAudioDevicePropertyBufferSize\r");
//            outSize = sizeof(UInt32);
//            err = AudioDeviceGetProperty(inDevice, 0, 0, kAudioDevicePropertyBufferSize, &outSize, &theUIntData);
//
//            break;
//
//        case kAudioDevicePropertyBufferFrameSize:
//            scprintf("%s\n", "***** DEVICE NOTIFICATION - kAudioDevicePropertyBufferFrameSize\r");
//            outSize = sizeof(UInt32);
//            err = AudioDeviceGetProperty(inDevice, 0, 0, kAudioDevicePropertyBufferFrameSize, &outSize, &theUIntData);
//
//            break;
//
//       case kAudioDevicePropertyBufferSizeRange:
//        {
//            AudioValueRange		range;
//
//            scprintf("%s\n", "***** DEVICE NOTIFICATION - kAudioDevicePropertyBufferSizeRange\r");
//            outSize = sizeof(AudioValueRange);
//            err = AudioDeviceGetProperty(inDevice, 0, isInput, kAudioDevicePropertyBufferSizeRange, &outSize, &range);
//		}
//			break;
//
//        case kAudioDevicePropertyStreamFormat:
//            scprintf("%s\n", "***** DEVICE NOTIFICATION - kAudioDevicePropertyStreamFormat\r");
//            break;
//
//        case kAudioDevicePropertyDeviceIsRunning:
//            scprintf("%s\n", "***** DEVICE NOTIFICATION - kAudioDevicePropertyDeviceIsRunning\r");
//            outSize = sizeof(UInt32);
//            err = AudioDeviceGetProperty(inDevice, inLine, isInput, kAudioDevicePropertyDeviceIsRunning, &outSize, &theUIntData);
//
//			//when change device get up to four messages:
//			//isInput ==NO or YES  theUIntData= 0 or 1 from old and possibly new device (ieheadphone swap)
//
//
//
//             break;
//
//        case kAudioDevicePropertyVolumeScalar:
//            scprintf("%s\n", "***** DEVICE NOTIFICATION - kAudioDevicePropertyVolumeScalar\r");
//            outSize = sizeof(Float32);
//            err = AudioDeviceGetProperty(inDevice, inLine, isInput, kAudioDevicePropertyVolumeScalar, &outSize, &vol);
//            break;
//
//        case kAudioDevicePropertyMute:
//            scprintf("%s\n", "***** DEVICE NOTIFICATION - kAudioDevicePropertyMute\r");
//            outSize = sizeof(UInt32);
//            err = AudioDeviceGetProperty(inDevice, inLine, isInput, kAudioDevicePropertyMute, &outSize, &mute);
//            break;
//
//        case kAudioDevicePropertyPlayThru:
//            scprintf("%s\n", "***** DEVICE NOTIFICATION - kAudioDevicePropertyPlayThru\r");
//            outSize = sizeof(UInt32);
//            err = AudioDeviceGetProperty(inDevice, inLine, isInput, kAudioDevicePropertyPlayThru, &outSize, &playThru);
//
//            break;
//
//        case kAudioDevicePropertyDeviceIsAlive:
//            scprintf("%s\n", "***** DEVICE NOTIFICATION - kAudioDevicePropertyDeviceIsAlive\r");
//            outSize = sizeof(UInt32);
//            err = AudioDeviceGetProperty(inDevice, 0, false, kAudioDevicePropertyDeviceIsAlive, &outSize, &tLong);
//
//            break;

        case kAudioDevicePropertyDataSource:
			//Don't print anything
            //scprintf("%s\n", "***** DEVICE NOTIFICATION - kAudioDevicePropertyDataSource\r");
            // get the source
            // match the source to one of the available sources and return the index of that source
            //SetControlValue(control, (chan->vol) * 100);

			//will get this message anyway even if don't have built-in output seleected.
			//so need to react based on whether current output IS built-in output. Annoyingly, headphone unplugging/plugging also sends default and system output + default input change hardware messages
			//swapping to new driver
			if (coredriver->builtinoutputflag_==1)
				coredriver->StopStart();


            break;

		//default:
           //  scprintf("%s\n", "***** DEVICE NOTIFICATION - %4.4s\r", &inPropertyID);
    }

	//fflush(stdout);
    return (err);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
OSStatus	streamListenerProc (	AudioStreamID			inStream,
									UInt32					inChannel,
									AudioDevicePropertyID	inPropertyID,
									void*					inClientData)
{
    OSStatus				err = noErr;

    switch(inPropertyID)
    {
        case kAudioStreamPropertyPhysicalFormat:
            scprintf("%s\n", "***** STREAM NOTIFICATION - kAudioStreamPropertyPhysicalFormat\r");
            break;

        case kAudioDevicePropertyStreamFormat:
            scprintf("%s\n", "***** STREAM NOTIFICATION - kAudioDevicePropertyStreamFormat\r");
            break;

        default:
            break;
    }

    fflush(stdout);

    return (err);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
OSStatus	AddStreamListeners (AudioDeviceID inDevice, AudioDevicePropertyID	inPropertyID, Boolean isInput, void *inClientData);

OSStatus    AddDeviceListeners(AudioDeviceID inDevice, void *inClientData)
{
    OSStatus		err = noErr;

//ONLY REACTING TO HEADPHONE SWAPS FOR NOW
//
//
//
//    // kAudioDevicePropertyBufferSize
//    err = AudioDeviceAddPropertyListener(inDevice, 0, false, kAudioDevicePropertyBufferSize, deviceListenerProc, inClientData);
//    if (err) return err;
//
//    err = AudioDeviceAddPropertyListener(inDevice, 0, true, kAudioDevicePropertyBufferSize, deviceListenerProc, inClientData);
//    if (err) return err;
//
//
//    // kAudioDevicePropertyBufferFrameSize
//    err = AudioDeviceAddPropertyListener(inDevice, 0, false, kAudioDevicePropertyBufferFrameSize, deviceListenerProc, inClientData);
//    if (err) return err;
//
//    err = AudioDeviceAddPropertyListener(inDevice, 0, true, kAudioDevicePropertyBufferFrameSize, deviceListenerProc, inClientData);
//    if (err) return err;
//
//    // kAudioDevicePropertyDeviceIsRunning
//    err = AudioDeviceAddPropertyListener(inDevice, 0, false, kAudioDevicePropertyDeviceIsRunning, deviceListenerProc, inClientData);
//    if (err) return err;
//
//    err = AudioDeviceAddPropertyListener(inDevice, 0, true, kAudioDevicePropertyDeviceIsRunning, deviceListenerProc, inClientData);
//    if (err) return err;
//
///*
//    for (i = 0; i <= deviceInfo->totalOutputChannels; i++)
//    {
//        // kAudioDevicePropertyVolumeScalar output
//        err = AudioDeviceAddPropertyListener(inDevice, i, false, kAudioDevicePropertyVolumeScalar, deviceListenerProc, inClientData);
//        if (err) return err;
//
//        // kAudioDevicePropertyVolumeMute output
//        err = AudioDeviceAddPropertyListener(inDevice, i, false, kAudioDevicePropertyMute, deviceListenerProc, inClientData);
//        if (err) return err;
//    }
//
//    for (i = 0; i <= deviceInfo->totalInputChannels; i++)
//    {
//        // kAudioDevicePropertyVolumeScalar input
//        err = AudioDeviceAddPropertyListener(inDevice, i, true, kAudioDevicePropertyVolumeScalar, deviceListenerProc, inClientData);
//        if (err) return err;
//
//        // kAudioDevicePropertyVolumeMute input
//        err = AudioDeviceAddPropertyListener(inDevice, i, true, kAudioDevicePropertyMute, deviceListenerProc, inClientData);
//        if (err) return err;
//
//        // kAudioDevicePropertyPlayThru input
//        err = AudioDeviceAddPropertyListener(inDevice, i, true, kAudioDevicePropertyPlayThru, deviceListenerProc, inClientData);
//        if (err) return err;
//    }
//*/
//
//    // kAudioDevicePropertyDeviceIsAlive
//    err = AudioDeviceAddPropertyListener(inDevice, 0, false, kAudioDevicePropertyDeviceIsAlive, deviceListenerProc, inClientData);
//    if (err) return err;
//
//    err = AudioDeviceAddPropertyListener(inDevice, 0, true, kAudioDevicePropertyDeviceIsAlive, deviceListenerProc, inClientData);
//    if (err) return err;
//
//
//    // kAudioDevicePropertyStreamFormat
//    err = AudioDeviceAddPropertyListener(inDevice, 0, false, kAudioDevicePropertyStreamFormat, deviceListenerProc, inClientData);
//    if (err) return err;
//
//    err = AudioDeviceAddPropertyListener(inDevice, 0, true, kAudioDevicePropertyStreamFormat, deviceListenerProc, inClientData);
//    if (err) return err;
//
//    // kAudioDevicePropertyBufferSizeRange
//    err = AudioDeviceAddPropertyListener(inDevice, 0, false, kAudioDevicePropertyBufferSizeRange, deviceListenerProc, inClientData);
//    if (err) return err;
//
//    err = AudioDeviceAddPropertyListener(inDevice, 0, true, kAudioDevicePropertyBufferSizeRange, deviceListenerProc, inClientData);
//    if (err) return err;

    //kAudioDevicePropertyDataSource
    err = AudioDeviceAddPropertyListener(inDevice, 0, false, kAudioDevicePropertyDataSource, deviceListenerProc, inClientData);
    if (err) return err;

  //  err = AudioDeviceAddPropertyListener(inDevice, 0, true, kAudioDevicePropertyDataSource, deviceListenerProc, inClientData);
   // if (err) return err;


    //AddStreamListeners (inDevice, kAudioStreamPropertyPhysicalFormat, false, inClientData);

    //AddStreamListeners (inDevice, kAudioStreamPropertyPhysicalFormat, true, inClientData);

    return (err);
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
OSStatus	AddStreamListeners (AudioDeviceID inDevice, AudioDevicePropertyID	inPropertyID, Boolean isInput, void *inClientData)
{
    OSStatus			err = noErr;
    UInt32				count;
    UInt32				outSize;
    Boolean				outWritable;
    AudioStreamID		*streamList = nil;

    err =  AudioDeviceGetPropertyInfo(inDevice, 0, isInput, kAudioDevicePropertyStreams,  &outSize, &outWritable);
    if (err == noErr)
    {
        streamList = (AudioStreamID*)malloc(outSize);
        err = AudioDeviceGetProperty(inDevice, 0, isInput, kAudioDevicePropertyStreams, &outSize, streamList);
        if (err == noErr)
        {
            for (count = 0; count < (outSize / sizeof(AudioStreamID)); count++)
            {
                err = AudioStreamAddPropertyListener(streamList[count], 0, inPropertyID, streamListenerProc, inClientData);
                if (err) return err;
            }
        }
        if (streamList != nil)
        {
            free(streamList);
            streamList = nil;
        }
    }


    return (noErr);
}
#endif // SC_AUDIO_API_COREAUDIO




// =====================================================================
// Audio driver (CoreAudioIPHONE)

#if SC_AUDIO_API == SC_AUDIO_API_COREAUDIOIPHONE
SC_iCoreAudioDriver::SC_iCoreAudioDriver(struct World *inWorld)
		: SC_AudioDriver(inWorld)
{
	receivedIn = 0;
}

SC_iCoreAudioDriver::~SC_iCoreAudioDriver()
{
}

/*
OSStatus appIOProc2 (AudioDeviceID inDevice, const AudioTimeStamp* inNow,
					const AudioBufferList* inInputData,
					const AudioTimeStamp* inInputTime,
					AudioBufferList* outOutputData,
					const AudioTimeStamp* inOutputTime,
					void* defptr);
OSStatus appIOProc2 (AudioDeviceID inDevice, const AudioTimeStamp* inNow,
					const AudioBufferList* inInputData,
					const AudioTimeStamp* inInputTime,
					AudioBufferList* outOutputData,
					const AudioTimeStamp* inOutputTime,
					void* defptr)
{
	SC_CoreAudioDriver* def = (SC_CoreAudioDriver*)defptr;

	int64 oscTime = CoreAudioHostTimeToOSC(inOutputTime->mHostTime);

	AudioTimeStamp readTime;
	readTime.mSampleTime = inNow->mSampleTime - def->SafetyOffset() - def->NumSamplesPerCallback();
	readTime.mFlags = kAudioTimeStampSampleTimeValid;

	AudioDeviceRead(def->InputDevice(), &readTime, def->GetInputBufferList());

	def->Run(def->GetInputBufferList(), outOutputData, oscTime);

	return kAudioHardwareNoError;
}
*/


OSStatus InputCallback(void *inRefCon, AudioUnitRenderActionFlags *ioActionFlags, const AudioTimeStamp *inTimeStamp, UInt32 inBusNumber, UInt32 inNumberFrames, AudioBufferList *ioData)
{
	SC_iCoreAudioDriver *driver = (SC_iCoreAudioDriver *) inRefCon;

	if (driver->receivedIn)
	{
		//printf("exit input with no data \n");
		return noErr;
	}

	OSStatus ret = AudioUnitRender(driver->inputUnit, ioActionFlags, inTimeStamp, inBusNumber, inNumberFrames, driver->buflist);
	if (ret)
	{
		//printf("no input !\n");
		return noErr;
	}
	driver->receivedIn = 1;

	return noErr;
}

OSStatus RenderCallback(void *inRefCon, AudioUnitRenderActionFlags *ioActionFlags, const AudioTimeStamp *inTimeStamp, UInt32 inBusNumber, UInt32 inNumberFrames, AudioBufferList *ioData)
{
	SC_iCoreAudioDriver *driver = (SC_iCoreAudioDriver *) inRefCon;

#ifndef SC_IPHONE
	if (!driver->receivedIn)
	{
		//printf("exit output with no data \n");
		return noErr;
	}
#endif

	//float *fbuffer = (float *) driver->converter_buffer;

	int i;
	for (i=0; i<inNumberFrames; i++)
	{
		signed short s = ((signed short *)driver->buflist->mBuffers[0].mData)[i];
		int k = s + 0x43c00000;
		float f = *((float *) &k);
		((float *)driver->floatInputList->mBuffers[0].mData)[i] = f - 384.0f;
	}

	int64 oscTime = GetCurrentOSCTime();
	//driver->Run(driver->floatInputList, driver->floatOutputList, oscTime);
	driver->Run(driver->floatInputList, ioData, oscTime);

	for (i=0; i<inNumberFrames; i++)
	{
		float f1 = ((float *)ioData->mBuffers[0].mData)[i];
		float f2 = ((float *)ioData->mBuffers[1].mData)[i];
		((int *) ioData->mBuffers[0].mData)[i] = (int) (f1*16777216);
		((int *) ioData->mBuffers[1].mData)[i] = (int) (f2*16777216);
	}


/*
	unsigned long size = 2*1024*sizeof(unsigned long);
	OSStatus ret = AudioConverterConvertBuffer(driver->converter_in_to_F32, driver->buflist->mBuffers[0].mDataByteSize, driver->buflist->mBuffers[0].mData, &size, driver->converter_buffer);
	if (ret)
	{
		printf("couldn't convert !\n");
		return noErr;
	}

	int i;
	for (i=0; i<inNumberFrames; i++)
	{
		((int *)ioData->mBuffers[0].mData)[i] = ((int *)ioData->mBuffers[1].mData)[i] = driver->converter_buffer[2*i];
	}
*/
	driver->receivedIn = 0;
	return noErr;
}



void SC_iCoreAudioDriver::Run(const AudioBufferList* inInputData,
					AudioBufferList* outOutputData, int64 oscTime)
{
	int64 systemTimeBefore = GetMicroseconds();

	World *world = mWorld;

	try {
		int numSamplesPerCallback = NumSamplesPerCallback();
		mOSCbuftime = oscTime;

#ifdef __APPLE__
		sc_SetDenormalFlags();
#endif

		mFromEngine.Free();

		mToEngine.Perform();
		mOscPacketsToEngine.Perform();
		//printf("mOscPacketsToEngine : %d micros\n", (int) (GetMicroseconds()-systemTimeBefore));

		int bufFrames = world->mBufLength;
		int numBufs = numSamplesPerCallback / bufFrames;

		int numInputBuses    = world->mNumInputs;
		int numOutputBuses   = world->mNumOutputs;
		float* inputBuses    = world->mAudioBus + world->mNumOutputs * bufFrames;
		float* outputBuses   = world->mAudioBus;
		int32* inputTouched  = world->mAudioBusTouched + world->mNumOutputs;
		int32* outputTouched = world->mAudioBusTouched;
		int numInputStreams  = inInputData ? inInputData->mNumberBuffers : 0;
		int numOutputStreams = outOutputData ? outOutputData->mNumberBuffers : 0;

		//static int go = 0;

		int64 oscInc = mOSCincrement;
		double oscToSamples = mOSCtoSamples;

		int bufFramePos = 0;

		for (int i = 0; i < numBufs; ++i, world->mBufCounter++, bufFramePos += bufFrames) {
			int32 bufCounter = world->mBufCounter;

			// de-interleave input
			if (inInputData) {
				const AudioBuffer* inInputDataBuffers = inInputData->mBuffers;
				for (int s = 0, b = 0; b<numInputBuses && s < numInputStreams; s++) {
					const AudioBuffer* buf = inInputDataBuffers + s;
					int nchan = buf->mNumberChannels;
					if (buf->mData) {
						float *busdata = inputBuses + b * bufFrames;
						float *bufdata = (float*)buf->mData + bufFramePos * nchan;
						if (nchan == 1)
						{
#ifdef IPHONE_VEC
							vcopy(busdata, bufdata, bufFrames);
#else
							for (int k=0; k<bufFrames; ++k)
							{
								busdata[k] = bufdata[k];
							}
#endif
							inputTouched[b] = bufCounter;
						} else {
							int minchan = sc_min(nchan, numInputBuses - b);
							for (int j=0; j<minchan; ++j, busdata += bufFrames) {
								for (int k=0, m=j; k<bufFrames; ++k, m += nchan) {
									busdata[k] = bufdata[m];
								}
								inputTouched[b+j] = bufCounter;
							}
						}
						b += nchan;
					}
				}
			}
			//count++;

			int64 schedTime;
			int64 nextTime = oscTime + oscInc;

			while ((schedTime = mScheduler.NextTime()) <= nextTime) {
				float diffTime = (float)(schedTime - oscTime) * oscToSamples + 0.5;
				float diffTimeFloor = floor(diffTime);
				world->mSampleOffset = (int)diffTimeFloor;
				world->mSubsampleOffset = diffTime - diffTimeFloor;

				if (world->mSampleOffset < 0) world->mSampleOffset = 0;
				else if (world->mSampleOffset >= world->mBufLength) world->mSampleOffset = world->mBufLength-1;

				SC_ScheduledEvent event = mScheduler.Remove();
				event.Perform();
			}
			world->mSampleOffset = 0;
			world->mSubsampleOffset = 0.f;

			//int64 now = GetMicroseconds();

			World_Run(world);

			//printf("world run : %fms\n", (float) (GetMicroseconds()-now)/1000);


			// interleave output
			AudioBuffer* outOutputDataBuffers = outOutputData->mBuffers;
			for (int s = 0, b = 0; b<numOutputBuses && s < numOutputStreams; s++) {
				AudioBuffer* buf = outOutputDataBuffers + s;
				int nchan = buf->mNumberChannels;
				if (buf->mData) {
					float *busdata = outputBuses + b * bufFrames;
					float *bufdata = (float*)buf->mData + bufFramePos * nchan;
					if (nchan == 1)
					{
						if (outputTouched[b] == bufCounter)
						{
#ifdef IPHONE_VEC
							vcopy(bufdata, busdata, bufFrames);
#else
							for (int k=0; k<bufFrames; ++k)
							{
								bufdata[k] = busdata[k];
							}
#endif
						}
					} else {
						int minchan = sc_min(nchan, numOutputBuses - b);
						for (int j=0; j<minchan; ++j, busdata += bufFrames) {
							if (outputTouched[b+j] == bufCounter) {
								for (int k=0, m=j; k<bufFrames; ++k, m += nchan) {
									bufdata[m] = busdata[k];
								}
							}
						}
					}
					b += nchan;
				}
			}
			oscTime = mOSCbuftime = nextTime;
		}
	} catch (std::exception& exc) {
		scprintf("exception in real time: %s\n", exc.what());
	} catch (...) {
		scprintf("unknown exception in real time\n");
	}

	int64 systemTimeAfter = GetMicroseconds();
	double calcTime = (double)(systemTimeAfter - systemTimeBefore) * 1e-6;
	double cpuUsage = calcTime * mBuffersPerSecond * 100.;
	mAvgCPU = mAvgCPU + 0.1 * (cpuUsage - mAvgCPU);
	if (cpuUsage > mPeakCPU || --mPeakCounter <= 0)
	{
		mPeakCPU = cpuUsage;
		mPeakCounter = mMaxPeakCounter;
	}

	mAudioSync.Signal();
}




/*
OSStatus appIOProc (AudioDeviceID device, const AudioTimeStamp* inNow,
					const AudioBufferList* inInputData,
					const AudioTimeStamp* inInputTime,
					AudioBufferList* outOutputData,
					const AudioTimeStamp* inOutputTime,
					void* defptr)
{
	SC_CoreAudioDriver* def = (SC_CoreAudioDriver*)defptr;
	int64 oscTime = CoreAudioHostTimeToOSC(inOutputTime->mHostTime);

	double hostSecs = (double)AudioConvertHostTimeToNanos(inOutputTime->mHostTime) * 1e-9;
	double sampleTime = inOutputTime->mSampleTime;
	if (def->mStartHostSecs == 0) {
		def->mStartHostSecs = hostSecs;
		def->mStartSampleTime = sampleTime;
	} else {
		double instSampleRate = (sampleTime -  def->mPrevSampleTime)/(hostSecs -  def->mPrevHostSecs);
		double smoothSampleRate = def->mSmoothSampleRate;
		smoothSampleRate = smoothSampleRate + 0.002 * (instSampleRate - smoothSampleRate);
		def->mOSCincrement = (int64)(def->mOSCincrementNumerator / smoothSampleRate);
		def->mSmoothSampleRate = smoothSampleRate;

#if 0
		double avgSampleRate  = (sampleTime - def->mStartSampleTime)/(hostSecs - def->mStartHostSecs);
		double jitter = (smoothSampleRate * (hostSecs - def->mPrevHostSecs)) - (sampleTime - def->mPrevSampleTime);
		double drift = (smoothSampleRate - def->mSampleRate) * (hostSecs - def->mStartHostSecs);
		//if (fabs(jitter) > 0.01) {
			scprintf("avgSR %.6f   smoothSR %.6f   instSR %.6f   jitter %.6f   drift %.6f   inc %lld\n",
				avgSampleRate, smoothSampleRate, instSampleRate, jitter, drift, def->mOSCincrement);
		//}
#endif
	}
	def->mPrevHostSecs = hostSecs;
	def->mPrevSampleTime = sampleTime;

	if (!def->UseSeparateIO())
	{
		def->Run(inInputData, outOutputData, oscTime);
		return kAudioHardwareNoError;
	}


	def->Run(lastInputData, outOutputData, oscTime);
	lastInputData = 0;

	return kAudioHardwareNoError;
}
*/


void AudioSessionInterruptionCbk(void *inClientData, UInt32 inInterruptionState)
{
}


bool SC_iCoreAudioDriver::DriverSetup(int* outNumSamplesPerCallback, double* outSampleRate)
{
	AudioSessionInitialize(0, 0, AudioSessionInterruptionCbk, 0);
	unsigned long category = kAudioSessionCategory_PlayAndRecord;
#ifdef SC_IPHONE
	UInt32 micInput, micInputSize = sizeof(&micInput);
	AudioSessionGetProperty(kAudioSessionProperty_AudioInputAvailable, &micInputSize, &micInput);
	if(!micInput) {
		category = kAudioSessionCategory_MediaPlayback;
		scprintf("SC_IPHONE: WARNING - no audio input available\n");
	}
#endif
	AudioSessionSetProperty(kAudioSessionProperty_AudioCategory, sizeof(category), &category);

	if (mPreferredHardwareBufferFrameSize)
	{
		Float32 preferredBufferSize = (float) mPreferredHardwareBufferFrameSize/44100.f;
		AudioSessionSetProperty(kAudioSessionProperty_PreferredHardwareIOBufferDuration, sizeof(preferredBufferSize), &preferredBufferSize);
	}

	AudioSessionSetActive(true);

	float actualBufferDuration;
	UInt32 size = sizeof(actualBufferDuration);
	AudioSessionGetProperty(kAudioSessionProperty_CurrentHardwareIOBufferDuration, &size, &actualBufferDuration);

	*outNumSamplesPerCallback = (int) (actualBufferDuration*44100.f+0.5f);
	*outSampleRate = 44100;

	AudioComponentDescription desc;
	desc.componentType = kAudioUnitType_Output;
	desc.componentSubType = kAudioUnitSubType_RemoteIO;
	desc.componentManufacturer = kAudioUnitManufacturer_Apple;

	NewAUGraph(&graph);
	AUNode node;
	AudioUnit unit;
	OSStatus ret = AUGraphAddNode(graph, &desc, &node);
	//printf("node : %d\n", node);
	AUGraphOpen(graph);

	ret = AUGraphNodeInfo(graph, node, &desc, &unit);
	//printf("%d\n", unit);



	AudioComponent remoteIOComp = AudioComponentFindNext(0, &desc);
	if (AudioComponentInstanceNew(remoteIOComp, &inputUnit)!=noErr)
	{
		//printf("error instantiating RemoteIO\n");
		return false;
	}
	//printf("instantiated : %d\n", inputUnit);

	int enableIO = 1;
	ret = AudioUnitSetProperty(inputUnit, kAudioOutputUnitProperty_EnableIO, kAudioUnitScope_Input, 1, &enableIO, sizeof(enableIO));
	if (ret!=noErr)
	{
		//printf("can't set input : %d\n", ret);
		return false;
	}
	enableIO = 0;
	ret = AudioUnitSetProperty(inputUnit, kAudioOutputUnitProperty_EnableIO, kAudioUnitScope_Output, 0, &enableIO, sizeof(enableIO));
	if (ret!=noErr)
	{
		//printf("can't set output : %d\n", ret);
		return false;
	}

	AudioUnitInitialize(inputUnit);

	unsigned long bufferSizeBytes = 1024 * sizeof(unsigned long);
	buflist = (AudioBufferList *) malloc(sizeof(AudioBufferList));
	buflist->mNumberBuffers = 1;
	buflist->mBuffers[0].mDataByteSize = bufferSizeBytes;
	buflist->mBuffers[0].mData = malloc(bufferSizeBytes);
	buflist->mBuffers[0].mNumberChannels = 1;

	floatInputList = (AudioBufferList *) malloc(sizeof(AudioBufferList));
	floatInputList->mNumberBuffers = 1;
	floatInputList->mBuffers[0].mDataByteSize = bufferSizeBytes;
	floatInputList->mBuffers[0].mData = malloc(bufferSizeBytes);
	floatInputList->mBuffers[0].mNumberChannels = 1;

/*
	floatOutputList = (AudioBufferList *) malloc(sizeof(AudioBufferList)+sizeof(AudioBuffer));
	floatOutputList->mNumberBuffers = 2;
	floatOutputList->mBuffers[0].mDataByteSize = bufferSizeBytes;
	floatOutputList->mBuffers[0].mData = malloc(bufferSizeBytes);
	floatOutputList->mBuffers[0].mNumberChannels = 1;
	floatOutputList->mBuffers[1].mDataByteSize = bufferSizeBytes;
	floatOutputList->mBuffers[1].mData = malloc(bufferSizeBytes);
	floatOutputList->mBuffers[1].mNumberChannels = 1;
*/

	AURenderCallbackStruct inputStruct;
	inputStruct.inputProc = InputCallback;
	inputStruct.inputProcRefCon = this;
	ret = AudioUnitSetProperty(inputUnit, kAudioOutputUnitProperty_SetInputCallback, kAudioUnitScope_Input, 0, &inputStruct, sizeof(inputStruct));

	AURenderCallbackStruct renderStruct;
	renderStruct.inputProc = RenderCallback;
	renderStruct.inputProcRefCon = this;
	ret = AudioUnitSetProperty(unit, kAudioUnitProperty_SetRenderCallback, kAudioUnitScope_Input, 0, &renderStruct, sizeof(renderStruct));

	AudioStreamBasicDescription streamFormat;
	streamFormat.mFormatID         = kAudioFormatLinearPCM;
	streamFormat.mFormatFlags      =
                  kAudioFormatFlagIsSignedInteger
                | kAudioFormatFlagsNativeEndian
                | kLinearPCMFormatFlagIsNonInterleaved
                | (24 << kLinearPCMFormatFlagsSampleFractionShift);
	streamFormat.mSampleRate       = 44100;
	streamFormat.mBitsPerChannel   = 32;
	streamFormat.mChannelsPerFrame = 2;
	streamFormat.mFramesPerPacket  = 1;
	streamFormat.mBytesPerFrame    = ( streamFormat.mBitsPerChannel / 8 );
	streamFormat.mBytesPerPacket   = streamFormat.mBytesPerFrame *
                                 streamFormat.mFramesPerPacket;

	ret = AudioUnitSetProperty(unit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Input, 0, &streamFormat, sizeof(streamFormat));


	AudioStreamBasicDescription audioFormat;
	audioFormat.mSampleRate = 44100.00;
	audioFormat.mFormatID = kAudioFormatLinearPCM;
	audioFormat.mFormatFlags = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked;
	audioFormat.mFramesPerPacket = 1;
	audioFormat.mChannelsPerFrame = 1;
	audioFormat.mBitsPerChannel	= 16;
	audioFormat.mBytesPerPacket	= 2;
	audioFormat.mBytesPerFrame = 2;
	ret = AudioUnitSetProperty(inputUnit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Output, 1, &audioFormat, sizeof(audioFormat));


/*
	AudioStreamBasicDescription d;
	size = sizeof(d);
	ret = AudioUnitGetProperty(unit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Input, 1, &d, &size);

	AudioStreamBasicDescription inputFormat;
	AudioStreamBasicDescription outputFormat;
	AudioStreamBasicDescription f32Format;

	size = sizeof(AudioStreamBasicDescription);
	ret = AudioUnitGetProperty(inputUnit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Input, 0, &inputFormat, &size);
	ret = AudioUnitGetProperty(inputUnit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Input, 1, &inputFormat, &size);
	ret = AudioUnitGetProperty(inputUnit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Output, 0, &outputFormat, &size);
	ret = AudioUnitGetProperty(inputUnit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Output, 1, &outputFormat, &size);

	outputFormat.mChannelsPerFrame = 1;

	f32Format.mSampleRate = inputFormat.mSampleRate;
	f32Format.mFormatID = inputFormat.mFormatID;
	f32Format.mFormatFlags = kAudioFormatFlagsNativeFloatPacked;
	f32Format.mBytesPerPacket = 4;
	f32Format.mFramesPerPacket = 1;
	f32Format.mBytesPerFrame = 4;
	f32Format.mChannelsPerFrame = 1;
	f32Format.mBitsPerChannel = 32;
	f32Format.mReserved = 0;

	ret = AudioConverterNew(&inputFormat, &f32Format, &converter_in_to_F32);
	ret = AudioConverterNew(&f32Format, &outputFormat, &converter_F32_to_out);
	ret = AudioConverterNew(&inputFormat, &outputFormat, &converter_in_to_out);
*/
	AUGraphInitialize(graph);

	if(mWorld->mVerbosity >= 0){
		scprintf("<-SC_CoreAudioDriver::Setup world %p\n", mWorld);
	}
	return true;
}

bool SC_iCoreAudioDriver::DriverStart()
{
	if(mWorld->mVerbosity >= 0){
		scprintf("->SC_CoreAudioDriver::DriverStart\n");
	}

	try
	{
		OSStatus ret = AUGraphStart(graph);
		AudioOutputUnitStart(inputUnit);
	} catch (...) {
	scprintf("exception in SC_CoreAudioDriver::DriverStart\n");
	}
	if(mWorld->mVerbosity >= 0){
		scprintf("<-SC_CoreAudioDriver::DriverStart\n");
	}
	return true;
}

bool SC_iCoreAudioDriver::DriverStop()
{
	if(mWorld->mVerbosity >= 0){
		scprintf("->SC_CoreAudioDriver::DriverStop\n");
	}

	AUGraphStop(graph);
	AudioOutputUnitStop(inputUnit);

	if(mWorld->mVerbosity >= 0){
		scprintf("<-SC_CoreAudioDriver::DriverStop\n");
	}
	return true;
}

#endif // SC_AUDIO_API_COREAUDIOIPHONE




// =====================================================================
// Audio driver (PortAudio)

#if SC_AUDIO_API == SC_AUDIO_API_PORTAUDIO

// =====================================================================
// SC_PortAudioDriver (PortAudio)

#define PRINT_PORTAUDIO_ERROR( function, errorcode )\
        scprintf( "SC_PortAudioDriver: PortAudio failed at %s with error: '%s'\n",\
                #function, Pa_GetErrorText( errorcode ) )

SC_PortAudioDriver::SC_PortAudioDriver(struct World *inWorld)
		: SC_AudioDriver(inWorld)
        , mStream(0)
{
    PaError paerror = Pa_Initialize();
    if( paerror != paNoError )
        PRINT_PORTAUDIO_ERROR( Pa_Initialize, paerror );
}

SC_PortAudioDriver::~SC_PortAudioDriver()
{
    if( mStream ){
        PaError paerror = Pa_CloseStream( mStream );
        if( paerror != paNoError )
            PRINT_PORTAUDIO_ERROR( Pa_CloseStream, paerror );
    }
    Pa_Terminate();
}

static int SC_PortAudioStreamCallback( const void *input, void *output,
    unsigned long frameCount, const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags statusFlags, void *userData )
{
    SC_PortAudioDriver *driver = (SC_PortAudioDriver*)userData;

    return driver->PortAudioCallback( input, output, frameCount, timeInfo, statusFlags );
}

int SC_PortAudioDriver::PortAudioCallback( const void *input, void *output,
            unsigned long frameCount, const PaStreamCallbackTimeInfo* timeInfo,
            PaStreamCallbackFlags statusFlags )
{
    World *world = mWorld;
    (void) frameCount, timeInfo, statusFlags; // suppress unused parameter warnings

	try {
		// synchronise against the output buffer - timeInfo->currentTime is 0.0 bug in PA?
		if (mPaStreamStartupTime==0 && mPaStreamStartupTimeOSC==0) {
			mPaStreamStartupTimeOSC = GetCurrentOSCTime();
			mPaStreamStartupTime = timeInfo->outputBufferDacTime;
		}
		mOSCbuftime = PaStreamTimeToOSC(timeInfo->outputBufferDacTime - mPaStreamStartupTime) + mPaStreamStartupTimeOSC;

		mFromEngine.Free();
		mToEngine.Perform();
		mOscPacketsToEngine.Perform();

		int numInputs = mInputChannelCount;
		int numOutputs = mOutputChannelCount;
		const float **inBuffers = (const float**)input;
		float **outBuffers = (float**)output;

		int numSamples = NumSamplesPerCallback();
		int bufFrames = mWorld->mBufLength;
		int numBufs = numSamples / bufFrames;

		float *inBuses = mWorld->mAudioBus + mWorld->mNumOutputs * bufFrames;
		float *outBuses = mWorld->mAudioBus;
		int32 *inTouched = mWorld->mAudioBusTouched + mWorld->mNumOutputs;
		int32 *outTouched = mWorld->mAudioBusTouched;

		int minInputs = std::min<size_t>(numInputs, mWorld->mNumInputs);
		int minOutputs = std::min<size_t>(numOutputs, mWorld->mNumOutputs);

		int bufFramePos = 0;

		int64 oscTime = mOSCbuftime;
		int64 oscInc = mOSCincrement;
		double oscToSamples = mOSCtoSamples;

		// main loop
		for (int i = 0; i < numBufs; ++i, mWorld->mBufCounter++, bufFramePos += bufFrames)
		{
			int32 bufCounter = mWorld->mBufCounter;
			int32 *tch;

			// copy+touch inputs
			tch = inTouched;
			for (int k = 0; k < minInputs; ++k)
			{
				const float *src = inBuffers[k] + bufFramePos;
				float *dst = inBuses + k * bufFrames;
				for (int n = 0; n < bufFrames; ++n) *dst++ = *src++;
				*tch++ = bufCounter;
			}

			// run engine
			int64 schedTime;
			int64 nextTime = oscTime + oscInc;
			// DEBUG
			/*
			if (mScheduler.Ready(nextTime)) {
				double diff = (mScheduler.NextTime() - mOSCbuftime)*kOSCtoSecs;
				scprintf("rdy %.6f %.6f %.6f %.6f \n", (mScheduler.NextTime()-gStartupOSCTime) * kOSCtoSecs, (mOSCbuftime-gStartupOSCTime)*kOSCtoSecs, diff, (nextTime-gStartupOSCTime)*kOSCtoSecs);
			}
			*/
			while ((schedTime = mScheduler.NextTime()) <= nextTime) {
				float diffTime = (float)(schedTime - oscTime) * oscToSamples + 0.5;
				float diffTimeFloor = floor(diffTime);
				world->mSampleOffset = (int)diffTimeFloor;
				world->mSubsampleOffset = diffTime - diffTimeFloor;

				if (world->mSampleOffset < 0) world->mSampleOffset = 0;
				else if (world->mSampleOffset >= world->mBufLength) world->mSampleOffset = world->mBufLength-1;

				SC_ScheduledEvent event = mScheduler.Remove();
				event.Perform();
			}
			world->mSampleOffset = 0;
			world->mSubsampleOffset = 0.f;

			World_Run(world);

			// copy touched outputs
			tch = outTouched;
			for (int k = 0; k < minOutputs; ++k) {
				float *dst = outBuffers[k] + bufFramePos;
				if (*tch++ == bufCounter) {
					float *src = outBuses + k * bufFrames;
					for (int n = 0; n < bufFrames; ++n) *dst++ = *src++;
				} else {
					for (int n = 0; n < bufFrames; ++n) *dst++ = 0.0f;
				}
			}

			// update buffer time
			oscTime = mOSCbuftime = nextTime;
		}
	} catch (std::exception& exc) {
		scprintf("SC_PortAudioDriver: exception in real time: %s\n", exc.what());
	} catch (...) {
		scprintf("SC_PortAudioDriver: unknown exception in real time\n");
	}

	double cpuUsage = (double)Pa_GetStreamCpuLoad(mStream);
	mAvgCPU = mAvgCPU + 0.1 * (cpuUsage - mAvgCPU);
	if (cpuUsage > mPeakCPU || --mPeakCounter <= 0)
	{
		mPeakCPU = cpuUsage;
		mPeakCounter = mMaxPeakCounter;
	}

	mAudioSync.Signal();

    return paContinue;
}

void SC_PortAudioDriver::GetPaDeviceFromName(const char* device, int* mInOut) {

	const PaDeviceInfo *pdi;
	const PaHostApiInfo *apiInfo;
	char devString[256];
	PaDeviceIndex numDevices = Pa_GetDeviceCount();
	mInOut[0] = paNoDevice;
	mInOut[1] = paNoDevice;

	// This tries to find one or two devices that match the given name (substring)
	// might cause problems for some names...
	for( int i=0; i<numDevices; i++ ) {
	    pdi = Pa_GetDeviceInfo( i );
		apiInfo = Pa_GetHostApiInfo(pdi->hostApi);
		strcpy(devString, apiInfo->name);
		strcat(devString, " : ");
		strcat(devString, pdi->name);
		if (strstr(devString, device)) {
			if (pdi->maxInputChannels > 0) mInOut[0] = i;
			if (pdi->maxOutputChannels > 0) mInOut[1] = i;
		}
	}
}

// ====================================================================
//
//
bool SC_PortAudioDriver::DriverSetup(int* outNumSamples, double* outSampleRate)
{
	int mDeviceInOut[2];
	PaError paerror;
	const PaDeviceInfo *pdi;
	const PaHostApiInfo *apiInfo;
	const PaStreamInfo *psi;
	PaTime suggestedLatencyIn, suggestedLatencyOut;
	PaDeviceIndex numDevices = Pa_GetDeviceCount();

	// print out all options:
	fprintf(stdout, "\nDevice options:\n");
	for( int i=0; i<numDevices; i++ ) {
	    pdi = Pa_GetDeviceInfo( i );
		apiInfo = Pa_GetHostApiInfo(pdi->hostApi);
		fprintf(stdout, "  - %s : %s   (device #%d with %d ins %d outs)\n",apiInfo->name,pdi->name, i, pdi->maxInputChannels, pdi->maxOutputChannels);
	}

	mDeviceInOut[0] = paNoDevice;
	mDeviceInOut[1] = paNoDevice;
	if (mWorld->hw->mInDeviceName)
		GetPaDeviceFromName(mWorld->hw->mInDeviceName, mDeviceInOut);
	if (mDeviceInOut[0] == paNoDevice) mDeviceInOut[0] = Pa_GetDefaultInputDevice();
	if (mDeviceInOut[1] == paNoDevice) mDeviceInOut[1] = Pa_GetDefaultOutputDevice();

	*outNumSamples = mWorld->mBufLength;
	if (mPreferredSampleRate)
		*outSampleRate = mPreferredSampleRate;
	else
		*outSampleRate = 44100.;


	if (mDeviceInOut[0]!=paNoDevice && mDeviceInOut[1]!=paNoDevice) {

		if (mPreferredHardwareBufferFrameSize)
			// controls the suggested latency by hardwareBufferSize switch -Z
			suggestedLatencyIn = suggestedLatencyOut = mPreferredHardwareBufferFrameSize / (*outSampleRate);
		else {
			suggestedLatencyIn = Pa_GetDeviceInfo( mDeviceInOut[0] )->defaultLowInputLatency;
			suggestedLatencyOut = Pa_GetDeviceInfo( mDeviceInOut[1] )->defaultLowOutputLatency;
		}

		PaSampleFormat fmt = paFloat32 | paNonInterleaved;
		mInputChannelCount = Pa_GetDeviceInfo( mDeviceInOut[0] )->maxInputChannels;
		mOutputChannelCount = Pa_GetDeviceInfo( mDeviceInOut[1] )->maxOutputChannels;
		fprintf(stdout, "\nBooting with:\n  In: %s : %s \n",
			Pa_GetHostApiInfo(Pa_GetDeviceInfo( mDeviceInOut[0] )->hostApi)->name,
			Pa_GetDeviceInfo( mDeviceInOut[0] )->name);
		fprintf(stdout, "  Out: %s : %s \n",
			Pa_GetHostApiInfo(Pa_GetDeviceInfo( mDeviceInOut[1] )->hostApi)->name,
			Pa_GetDeviceInfo( mDeviceInOut[1] )->name);

		PaStreamParameters inStreamParams;
		inStreamParams.device = mDeviceInOut[0];
		inStreamParams.channelCount = mInputChannelCount;
		inStreamParams.sampleFormat = fmt;
		inStreamParams.suggestedLatency = suggestedLatencyIn;
		inStreamParams.hostApiSpecificStreamInfo = NULL;

		PaStreamParameters outStreamParams;
		outStreamParams.device = mDeviceInOut[1];
		outStreamParams.channelCount = mOutputChannelCount;
		outStreamParams.sampleFormat = fmt;
		outStreamParams.suggestedLatency = suggestedLatencyOut;
		outStreamParams.hostApiSpecificStreamInfo = NULL;

		paerror = Pa_OpenStream(&mStream, &inStreamParams, &outStreamParams, *outSampleRate, *outNumSamples, paNoFlag, SC_PortAudioStreamCallback, this );
		if( paerror != paNoError )
			PRINT_PORTAUDIO_ERROR( Pa_OpenStream, paerror );
		else {
			psi = Pa_GetStreamInfo(mStream);
			if (!psi)
				fprintf(stdout,"  Could not obtain further info from portaudio stream\n");
			else {
				fprintf(stdout,"  Sample rate: %.3f\n", psi->sampleRate);
				fprintf(stdout,"  Latency (in/out): %.3f / %.3f sec\n", psi->inputLatency, psi->outputLatency);
			}
		}
		return paerror == paNoError;
	}

	// should not be necessary, but a last try with OpenDefaultStream...
	paerror = Pa_OpenDefaultStream( &mStream, 2, 2,
        paFloat32 | paNonInterleaved, *outSampleRate, *outNumSamples, SC_PortAudioStreamCallback, this );
	if( paerror != paNoError )
	    PRINT_PORTAUDIO_ERROR( Pa_OpenDefaultStream, paerror );
	return paerror == paNoError;
}

bool SC_PortAudioDriver::DriverStart()
{
	if (!mStream)
		return false;

    PaError paerror = Pa_StartStream( mStream );
    if( paerror != paNoError )
        PRINT_PORTAUDIO_ERROR( Pa_StartStream, paerror );

	// sync times
	mPaStreamStartupTimeOSC = 0;
	mPaStreamStartupTime = 0;
	// it would be better to do the sync here, but the timeInfo in the callback is incomplete
	//mPaStreamStartupTimeOSC = GetCurrentOSCTime();
	//mPaStreamStartupTime = Pa_GetStreamTime(mStream);

	return paerror == paNoError;
}

bool SC_PortAudioDriver::DriverStop()
{
    if (!mStream)
		return false;

    PaError paerror = Pa_StopStream(mStream);
    if( paerror != paNoError )
        PRINT_PORTAUDIO_ERROR( Pa_StopStream, paerror );

	return paerror == paNoError;
}

#endif // SC_AUDIO_API_PORTAUDIO
