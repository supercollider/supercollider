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


#include "SC_CoreAudio.h"
#include "SC_Sem.h"
#include "SC_ComPort.h"
#include <stdarg.h>
#include "SC_SequencedCommand.h"
#include "SC_Prototypes.h"
#include "SC_HiddenWorld.h"
#include "SC_Endian.h"

const double kMicrosToOSCunits = 4294.967296; // pow(2,32)/1e6
const double kNanosToOSCunits  = 4.294967296; // pow(2,32)/1e9
int64 gOSCoffset = 0; 
int64 gStartupOSCTime;

const int32 kSECONDS_FROM_1900_to_1970 = (int32)2208988800UL; /* 17 leap years */
const double kOSCtoSecs = 2.328306436538696e-10;

void set_real_time_priority(pthread_t thread);

// =====================================================================
// Timing (CoreAudio)

#if SC_AUDIO_API == SC_AUDIO_API_COREAUDIO
int32 timeseed()
{
	static int32 count = 0;
	int64 time = AudioGetCurrentHostTime();
	return (int32)(time >> 32) ^ (int32)time ^ count--;
}

inline int64 CoreAudioHostTimeToOSC(int64 hostTime)
{
	return (int64)((double)AudioConvertHostTimeToNanos(hostTime) * kNanosToOSCunits) + gOSCoffset;
}

int64 oscTimeNow();
int64 oscTimeNow()
{
	return CoreAudioHostTimeToOSC(AudioGetCurrentHostTime());
}

void syncOSCOffsetWithTimeOfDay();
void syncOSCOffsetWithTimeOfDay()
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


void initializeScheduler();
void initializeScheduler()
{
	syncOSCOffsetWithTimeOfDay();
	
	pthread_t resyncThread;
	pthread_create (&resyncThread, NULL, resyncThreadFunc, (void*)0);
	set_real_time_priority(resyncThread);
}
#endif // SC_AUDIO_API_COREAUDIO


// =====================================================================
// Timing (JACK)

#if SC_AUDIO_API == SC_AUDIO_API_JACK
#include <sys/time.h>

static inline int64 GetCurrentOSCTime()
{
	struct timeval tv;
	uint64 s, f;
	gettimeofday(&tv, 0); 

	s = (uint64)tv.tv_sec + (uint64)kSECONDS_FROM_1900_to_1970;
	f = (uint64)((double)tv.tv_usec * kMicrosToOSCunits);

	return (s << 32) + f;
}

int32 timeseed()
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
}
#endif // SC_AUDIO_API_JACK

double gSampleRate, gSampleDur;


// =====================================================================
// Packets (Common)

#include "SC_Lib_Cintf.h"
#include <stdlib.h>

////////////////////////////////////////////////////////////////////////////


void ProcessOSCPacket(World *inWorld, OSC_Packet *inPacket);
void PerformOSCBundle(World *inWorld, OSC_Packet *inPacket);
int PerformOSCMessage(World *inWorld, int inSize, char *inData, ReplyAddress *inReply);

void Perform_ToEngine_Msg(FifoMsg *inMsg);
void Free_ToEngine_Msg(FifoMsg *inMsg);

struct IsBundle
{
	IsBundle() { str4cpy(s, "#bundle"); }
	bool check(int32 *in) { return in[0] == s[0] && in[1] == s[1]; }
	int32 s[2];
};

IsBundle gIsBundle;

void ProcessOSCPacket(World *inWorld, OSC_Packet *inPacket)
{
	//scprintf("ProcessOSCPacket %d, '%s'\n", inPacket->mSize, inPacket->mData);
	
	if (inPacket) {	
		SC_CoreAudioDriver *driver = AudioDriver(inWorld);	
		inPacket->mIsBundle = gIsBundle.check((int32*)inPacket->mData);
		driver->Lock();
			FifoMsg fifoMsg;
			fifoMsg.Set(inWorld, Perform_ToEngine_Msg, Free_ToEngine_Msg, (void*)inPacket);
			driver->SendMsgToEngine(fifoMsg);
		driver->Unlock();
	}
}

int PerformOSCMessage(World *inWorld, int inSize, char *inData, ReplyAddress *inReply)
{
	//scprintf("->PerformOSCMessage %d\n", inData[0]);
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
		scprintf("FAILURE %s Command not found\n", inData);
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
		int32 msgSize = NTOHL(*(int32*)data);
		data += sizeof(int32);
		//scprintf("msgSize %d\n", msgSize);
		PerformOSCMessage(inWorld, msgSize, data, &inPacket->mReplyAddr);
		data += msgSize;
	}
	//scprintf("<-PerformOSCBundle %d\n", inPacket->mSize);
}

////////////////////////////////////////////////////////////////////////////

void Perform_ToEngine_Msg(FifoMsg *inMsg)
{
	World *world = inMsg->mWorld;
	OSC_Packet *packet = (OSC_Packet*)inMsg->mData;
	if (!packet) return;
	
	SC_CoreAudioDriver *driver = inMsg->mWorld->hw->mAudioDriver;
	
	if (!packet->mIsBundle) {
		PerformOSCMessage(world, packet->mSize, packet->mData, &packet->mReplyAddr);
	} else {
	
		// in real time engine, schedule the packet
#if BYTE_ORDER == BIG_ENDIAN
		int64 time = *(int64*)(packet->mData + 8);
#else
		char* data = packet->mData + 8;
		int64 time = ((int64)NTOHL(*(uint32*)data) << 32) + (NTOHL(*(uint32*)(data + 4)));
#endif // BYTE_ORDER

		if (time == 0 || time == 1) {
			PerformOSCBundle(world, packet);
		} else {
			if (time < driver->mOSCbuftime) {
				double seconds = (driver->mOSCbuftime - time)*kOSCtoSecs;
				scprintf("late %.9f\n", seconds);
				//FifoMsg outMsg;
				
				//ReportLateness(packet->mReply, seconds)
			}
			
			SC_ScheduledEvent event(world, time, packet);
			driver->AddEvent(event);
			inMsg->mFreeFunc = 0;
		}
	}
}


void PerformCompletionMsg(World *inWorld, OSC_Packet *inPacket);
void PerformCompletionMsg(World *inWorld, OSC_Packet *inPacket)
{
	bool isBundle = gIsBundle.check((int32*)inPacket->mData);
	
	if (isBundle) {
		PerformOSCBundle(inWorld, inPacket);
	} else {
		PerformOSCMessage(inWorld, inPacket->mSize, inPacket->mData, &inPacket->mReplyAddr);
	}
}


void FreeOSCPacket(OSC_Packet *inPacket);
void FreeOSCPacket(OSC_Packet *inPacket)
{
	if (inPacket) {
		free(inPacket->mData);
		free(inPacket);
	}
}

void Free_ToEngine_Msg(FifoMsg *inMsg)
{
	OSC_Packet *packet = (OSC_Packet*)inMsg->mData;
	FreeOSCPacket(packet);
}

void Free_FromEngine_Msg(FifoMsg *inMsg);
void Free_FromEngine_Msg(FifoMsg *inMsg)
{
	World_Free(inMsg->mWorld, inMsg->mData);
}

void DoneWithPacket(FifoMsg *inMsg);
void DoneWithPacket(FifoMsg *inMsg)
{
	OSC_Packet *packet = (OSC_Packet*)inMsg->mData;
	FreeOSCPacket(packet);
}


// =====================================================================
// Audio driver (Common)

SC_AudioDriver::SC_AudioDriver(struct World *inWorld)
		: mWorld(inWorld), mSampleTime(0)
{
}

SC_CoreAudioDriver::SC_CoreAudioDriver(struct World *inWorld)
		: SC_AudioDriver(inWorld), mNumSamplesPerCallback(0)
{
	mProcessPacketLock = new SC_Lock();
	DriverInitialize();
}

SC_CoreAudioDriver::~SC_CoreAudioDriver()
{
	mRunThreadFlag = false;
	mAudioSync.Signal();
	pthread_join(mThread, 0);
	
	delete mProcessPacketLock;
	DriverRelease();
}

void* core_audio_thread_func(void* arg);
void* core_audio_thread_func(void* arg)
{
	SC_CoreAudioDriver *ca = (SC_CoreAudioDriver*)arg;
	void* result = ca->RunThread();
	return result;
}

void* SC_CoreAudioDriver::RunThread()
{
	MsgFifoNoFree<TriggerMsg, 1024> *trigfifo = &mWorld->hw->mTriggers;
	MsgFifoNoFree<NodeEndMsg, 1024> *nodeendfifo = &mWorld->hw->mNodeEnds;
	MsgFifoNoFree<DeleteGraphDefMsg, 512> *deletegraphfifo = &mWorld->hw->mDeleteGraphDefs;

	while (mRunThreadFlag) {
		// wait for sync
		mAudioSync.WaitNext();
		
		mWorld->mNRTLock->Lock();
		
		// send /tr messages
		trigfifo->Perform();		
		
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

bool SC_CoreAudioDriver::SendMsgFromEngine(FifoMsg& inMsg)
{
	return mFromEngine.Write(inMsg);
}


bool SC_CoreAudioDriver::SendMsgToEngine(FifoMsg& inMsg)
{
	mToEngine.Free();
	return mToEngine.Write(inMsg);
}

void SC_ScheduledEvent::Perform()
{
	PerformOSCBundle(mWorld, mPacket); 
	FifoMsg msg;
	msg.Set(mWorld, DoneWithPacket, 0, (void*)mPacket);
	mWorld->hw->mAudioDriver->SendMsgFromEngine(msg);
}

bool SC_CoreAudioDriver::Setup()
{
	mRunThreadFlag = true;
	pthread_create (&mThread, NULL, core_audio_thread_func, (void*)this);
	set_real_time_priority(mThread);

	int numSamples;
	double sampleRate;

	if (!DriverSetup(&numSamples, &sampleRate)) return false;

	mNumSamplesPerCallback = numSamples;
	scprintf("mNumSamplesPerCallback %d\n", mNumSamplesPerCallback);
	scprintf("mHardwareBufferSize %lu\n", mHardwareBufferSize);

	// compute a per sample increment to the OpenSoundControl Time
	mOSCincrementNumerator = (double)mWorld->mBufLength * pow(2.,32.);
	mOSCincrement = (int64)(mOSCincrementNumerator / sampleRate);
	mOSCtoSamples = sampleRate / pow(2.,32.);

	World_SetSampleRate(mWorld, sampleRate);
	mSampleRate = mSmoothSampleRate = sampleRate;
	mBuffersPerSecond = sampleRate / mNumSamplesPerCallback;
	mMaxPeakCounter = (int)mBuffersPerSecond;
	
#ifndef NDEBUG
	scprintf("SC_CoreAudioDriver: numSamples=%d, sampleRate=%f\n", mNumSamplesPerCallback, sampleRate);
#endif

	return true;
}

bool SC_CoreAudioDriver::Start()
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

bool SC_CoreAudioDriver::Stop()
{
	printf("SC_CoreAudioDriver::Stop\n");
	if (!DriverStop()) return false;
	return true;
}


// =====================================================================
// Audio driver (CoreAudio)

#if SC_AUDIO_API == SC_AUDIO_API_COREAUDIO
void SC_CoreAudioDriver::DriverInitialize()
{
	mInputBufList = 0;
}

void SC_CoreAudioDriver::DriverRelease()
{
	delete mInputBufList;
}

bool SC_CoreAudioDriver::DriverSetup(int* outNumSamplesPerCallback, double* outSampleRate)
{
	OSStatus	err = kAudioHardwareNoError;
	UInt32	count;
	mOutputDevice = kAudioDeviceUnknown;
	mInputDevice = kAudioDeviceUnknown;

	//scprintf("SC_CoreAudioDriver::Setup world %08X\n", mWorld);

	////////////////////////////////////////////////////////////////////////////////////////////////

	/*{
		err = AudioHardwareGetPropertyInfo(kAudioHardwarePropertyDevices, &count, 0);

		AudioDeviceID *devices = (AudioDeviceID*)malloc(count);
		err = AudioHardwareGetProperty(kAudioHardwarePropertyDevices, &count, devices);
		if (err != kAudioHardwareNoError) {
			fprintf(stdout, "get kAudioHardwarePropertyDevices error %4.4s\n", (char*)&err);
			return false;
		}
		int numdevices = count / sizeof(AudioDeviceID);
		fprintf(stdout, "numdevices %d\n", numdevices);
		for (int i = 0; i < numdevices; ++i) {
			err = AudioDeviceGetPropertyInfo(devices[i], 0, false, kAudioDevicePropertyDeviceName, &count, 0);

			char *name = (char*)malloc(count);
			err = AudioDeviceGetProperty(devices[i], 0, false, kAudioDevicePropertyDeviceName, &count, name);
			if (err != kAudioHardwareNoError) {
				fprintf(stdout, "get kAudioDevicePropertyDeviceName error %4.4s\n", (char*)&err);
				return false;
			}
			fprintf(stdout, "DEVICE %d %u '%s'\n", i, devices[i], name);
			free(name);
			for (int b=0; b<2; ++b) { // b is first false, then true
				fprintf(stdout, b ? "INPUT:\n" : "OUTPUT:\n");
				Boolean writeable;
				err = AudioDeviceGetPropertyInfo(devices[i], 0, b, kAudioDevicePropertyStreamConfiguration,
												 &count, &writeable);

				AudioBufferList *bufList = (AudioBufferList*)malloc(count);
				err = AudioDeviceGetProperty(devices[i], 0, b, kAudioDevicePropertyStreamConfiguration,
											 &count, bufList);
				if (err != kAudioHardwareNoError) {
					fprintf(stdout, "get kAudioDevicePropertyStreamConfiguration error %4.4s\n", (char*)&err);
					return false;
				}

				fprintf(stdout, "mNumberBuffers %d\n", bufList->mNumberBuffers);
				for (unsigned int j = 0; j < bufList->mNumberBuffers; ++j) {
					fprintf(stdout, "buffer %d  mNumberChannels %d mDataByteSize %d\n", j,
							bufList->mBuffers[j].mNumberChannels, bufList->mBuffers[j].mDataByteSize);
				}
				free(bufList);
			}
		}
		free(devices);
	}*/

	////////////////////////////////////////////////////////////////////////////////////////////////

	// get the default output device for the HAL
	count = sizeof(mOutputDevice);		// it is required to pass the size of the data to be returned
	//get the output device:
	err = AudioHardwareGetProperty(kAudioHardwarePropertyDefaultOutputDevice, &count, (void *) & mOutputDevice);
	if (err != kAudioHardwareNoError) {
		fprintf(stdout, "get kAudioHardwarePropertyDefaultOutputDevice error %4.4s\n", (char*)&err);
		return false;
	}

	//get the input device
	err = AudioHardwareGetProperty(kAudioHardwarePropertyDefaultInputDevice, &count, (void *) & mInputDevice);

	if (err != kAudioHardwareNoError) {
		fprintf(stdout, "get kAudioHardwarePropertyDefaultInputDevice error %4.4s\n", (char*)&err);
		return false;
	}
	
	if (mPreferredHardwareBufferFrameSize)
	{
		AudioTimeStamp	now;
		now.mFlags = kAudioTimeStampHostTimeValid;
		now.mHostTime = AudioGetCurrentHostTime();

		count = sizeof(UInt32);	// it is required to pass the size of the data to be returned
		err = AudioDeviceSetProperty(mOutputDevice, &now, 0, false, kAudioDevicePropertyBufferFrameSize, count, &mPreferredHardwareBufferFrameSize);
		if (err != kAudioHardwareNoError) {
			fprintf(stdout, "set kAudioDevicePropertyBufferFrameSize error %4.4s\n", (char*)&err);
			//return false;
		}
	}
	// get the buffersize that the default device uses for IO
	count = sizeof(mHardwareBufferSize);	// it is required to pass the size of the data to be returned
	err = AudioDeviceGetProperty(mOutputDevice, 0, false, kAudioDevicePropertyBufferSize, &count, &mHardwareBufferSize);
	if (err != kAudioHardwareNoError) {
		fprintf(stdout, "get kAudioDevicePropertyBufferSize error %4.4s\n", (char*)&err);
		return false;
	}
	//fprintf(stdout, "mHardwareBufferSize = %ld\n", mHardwareBufferSize);
	
	// get a description of the data format used by the default output device
	count = sizeof(AudioStreamBasicDescription);	// it is required to pass the size of the data to be returned
	err = AudioDeviceGetProperty(mOutputDevice, 0, false, kAudioDevicePropertyStreamFormat, &count, &outputStreamDesc);
	if (err != kAudioHardwareNoError) {
		fprintf(stdout, "get kAudioDevicePropertyStreamFormat error %4.4s\n", (char*)&err);
		return false;
	}

	if (mInputDevice != kAudioDeviceUnknown) {
		// get a description of the data format used by the default input device
		count = sizeof(AudioStreamBasicDescription);	// it is required to pass the size of the data to be returned
		err = AudioDeviceGetProperty(mInputDevice, 0, true, kAudioDevicePropertyStreamFormat, &count, &inputStreamDesc);
		if (err != kAudioHardwareNoError) {
			fprintf(stdout, "get kAudioDevicePropertyStreamFormat error %4.4s\n", (char*)&err);
			return false;
		}
	
		count = sizeof(AudioStreamBasicDescription);	// it is required to pass the size of the data to be returned
		err = AudioDeviceGetProperty(mInputDevice, 0, true, kAudioDevicePropertyStreamFormat, &count, &inputStreamDesc);
		if (err != kAudioHardwareNoError) {
			fprintf(stdout, "get kAudioDevicePropertyStreamFormat error %4.4s\n", (char*)&err);
			return false;
		}
		if (inputStreamDesc.mSampleRate != outputStreamDesc.mSampleRate) {
			fprintf(stdout, "input and output sample rates do not match. %g != %g\n", inputStreamDesc.mSampleRate, outputStreamDesc.mSampleRate);
			return false;
		}
	}

	if (UseSeparateIO()) {
		count = sizeof(UInt32);	// it is required to pass the size of the data to be returned
		err = AudioDeviceGetProperty(mInputDevice, 0, true, kAudioDevicePropertySafetyOffset, &count, &mSafetyOffset);
		if (err != kAudioHardwareNoError) {
			fprintf(stdout, "get kAudioDevicePropertySafetyOffset error %4.4s\n", (char*)&err);
			return false;
		}
		scprintf("mSafetyOffset %lu\n", mSafetyOffset);
	
		Boolean writeable;
		err = AudioDeviceGetPropertyInfo(mInputDevice, 0, true, kAudioDevicePropertyStreamConfiguration, &count, &writeable);
		mInputBufList = (AudioBufferList*)malloc(count);
		err = AudioDeviceGetProperty(mInputDevice, 0, true, kAudioDevicePropertyStreamConfiguration, &count, mInputBufList);
		if (err != kAudioHardwareNoError) {
			fprintf(stdout, "get kAudioDevicePropertyStreamConfiguration error %4.4s\n", (char*)&err);
			return false;
		}
	
		scprintf("mNumberBuffers %lu\n", mInputBufList->mNumberBuffers);
		for (uint32 i=0; i<mInputBufList->mNumberBuffers; ++i) {
			scprintf("  mDataByteSize %d %lu\n", i, mInputBufList->mBuffers[i].mDataByteSize);
			mInputBufList->mBuffers[i].mData = zalloc(1, mInputBufList->mBuffers[i].mDataByteSize);
		}
	
	
		AudioTimeStamp	now;
		now.mFlags = kAudioTimeStampHostTimeValid;
		now.mHostTime = AudioGetCurrentHostTime();
		
		err = AudioDeviceSetProperty(mInputDevice, &now, 0, true, kAudioDevicePropertyRegisterBufferList, count, mInputBufList);
		if (err != kAudioHardwareNoError) {
			fprintf(stdout, "get kAudioDevicePropertyRegisterBufferList error %4.4s\n", (char*)&err);
			return false;
		}
	}

	*outNumSamplesPerCallback = mHardwareBufferSize / outputStreamDesc.mBytesPerFrame;
	*outSampleRate = outputStreamDesc.mSampleRate;

	return true;
}

// this is the audio processing callback for two separate devices.
OSStatus appIOProc2 (AudioDeviceID inDevice, const AudioTimeStamp* inNow, 
					const AudioBufferList* inInputData, 
					const AudioTimeStamp* inInputTime, 
					AudioBufferList* outOutputData, 
					const AudioTimeStamp* inOutputTime,
					void* defptr);
OSStatus appIOProc2 (AudioDeviceID /*inDevice*/, const AudioTimeStamp* inNow, 
					const AudioBufferList* inInputData,
					const AudioTimeStamp* /*inInputTime*/, 
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


// this is the audio processing callback for one device.
OSStatus appIOProc (AudioDeviceID inDevice, const AudioTimeStamp* inNow, 
					const AudioBufferList* inInputData, 
					const AudioTimeStamp* inInputTime, 
					AudioBufferList* outOutputData, 
					const AudioTimeStamp* inOutputTime,
					void* defptr);
OSStatus appIOProc (AudioDeviceID /*inDevice*/, const AudioTimeStamp* inNow, 
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

	def->Run(inInputData, outOutputData, oscTime);

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
		
		mFromEngine.Free();
		/*if (mToEngine.HasData()) {
			scprintf("oscTime %.9f %.9f\n", oscTime*kOSCtoSecs, CoreAudioHostTimeToOSC(AudioGetCurrentHostTime())*kOSCtoSecs);
		}*/
		mToEngine.Perform();
		
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
					}
					b += nchan;
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
			
				world->mSampleOffset = (int)((double)(schedTime - oscTime) * oscToSamples);
				if (world->mSampleOffset < 0) world->mSampleOffset = 0;
				else if (world->mSampleOffset >= world->mBufLength) world->mSampleOffset = world->mBufLength-1;

				SC_ScheduledEvent event = mScheduler.Remove();
				event.Perform();
			}
			world->mSampleOffset = 0;
			
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
				}
				b += nchan;
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

bool SC_CoreAudioDriver::DriverStart()
{
	OSStatus	err = kAudioHardwareNoError;

	scprintf("start   UseSeparateIO?: %d\n", UseSeparateIO());
	
	if (UseSeparateIO()) {
		err = AudioDeviceAddIOProc(mOutputDevice, appIOProc2, (void *) this);	// setup our device with an IO proc
		if (err != kAudioHardwareNoError) {
			fprintf(stdout, "AudioDeviceAddIOProc failed %d\n", (int)err);
			return false;
		}
		
		err = AudioDeviceStart(mInputDevice, NULL);		// start playing sound through the device
		if (err != kAudioHardwareNoError) {
			fprintf(stdout, "AudioDeviceStart failed %d\n", (int)err);
			return false;
		}
		
		err = AudioDeviceStart(mOutputDevice, appIOProc2);		// start playing sound through the device
		if (err != kAudioHardwareNoError) {
			fprintf(stdout, "AudioDeviceStart failed %d\n", (int)err);
			err = AudioDeviceStop(mInputDevice, NULL);		// stop playing sound through the device
			return false;
		}
	} else {
		err = AudioDeviceAddIOProc(mOutputDevice, appIOProc, (void *) this);	// setup our device with an IO proc
		if (err != kAudioHardwareNoError) {
			fprintf(stdout, "AudioDeviceAddIOProc failed %d\n", (int)err);
			return false;
		}
	
		err = AudioDeviceStart(mOutputDevice, appIOProc);		// start playing sound through the device
		if (err != kAudioHardwareNoError) {
			fprintf(stdout, "AudioDeviceStart failed %d\n", (int)err);
			return false;
		}
	}

	return true;
}

bool SC_CoreAudioDriver::DriverStop()
{
	OSStatus err = kAudioHardwareNoError;

	if (UseSeparateIO()) {
		err = AudioDeviceStop(mOutputDevice, appIOProc2);		
		if (err != kAudioHardwareNoError) return false;
	
		err = AudioDeviceRemoveIOProc(mOutputDevice, appIOProc2);	
		if (err != kAudioHardwareNoError) return false;
	} else {
		err = AudioDeviceStop(mOutputDevice, appIOProc);		
		if (err != kAudioHardwareNoError) return false;
	
		err = AudioDeviceRemoveIOProc(mOutputDevice, appIOProc);	
		if (err != kAudioHardwareNoError) return false;
	}
	return true;
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
            fprintf(stdout, "%s\n", "***** HARDWARE NOTIFICATION - kAudioHardwarePropertyDefaultOutputDevice\r");
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
            fprintf(stdout, "%s\n", "***** HARDWARE NOTIFICATION - kAudioHardwarePropertyDefaultInputDevice\r");
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
            fprintf(stdout, "%s\n", "***** HARDWARE NOTIFICATION - kAudioHardwarePropertyDefaultSystemOutputDevice\r");
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
            fprintf(stdout, "%s\n", "***** HARDWARE NOTIFICATION - kAudioHardwarePropertyDevices\r");
        }
            break;
    }
    
    fflush(stdout);
    return (noErr);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Listen for Device Properties and update interface abd globals
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
	
    switch(inPropertyID)
    {
        case kAudioDevicePropertyBufferSize:
            fprintf(stdout, "%s\n", "***** DEVICE NOTIFICATION - kAudioDevicePropertyBufferSize\r");
            outSize = sizeof(UInt32);
            err = AudioDeviceGetProperty(inDevice, 0, 0, kAudioDevicePropertyBufferSize, &outSize, &theUIntData);
		
            break;
        
        case kAudioDevicePropertyBufferFrameSize:
            fprintf(stdout, "%s\n", "***** DEVICE NOTIFICATION - kAudioDevicePropertyBufferFrameSize\r");
            outSize = sizeof(UInt32);
            err = AudioDeviceGetProperty(inDevice, 0, 0, kAudioDevicePropertyBufferFrameSize, &outSize, &theUIntData);

            break;

       case kAudioDevicePropertyBufferSizeRange: 
        {
            AudioValueRange		range;
            
            fprintf(stdout, "%s\n", "***** DEVICE NOTIFICATION - kAudioDevicePropertyBufferSizeRange\r");
            outSize = sizeof(AudioValueRange);
            err = AudioDeviceGetProperty(inDevice, 0, isInput, kAudioDevicePropertyBufferSizeRange, &outSize, &range);
		}
			break;

        case kAudioDevicePropertyStreamFormat:
            fprintf(stdout, "%s\n", "***** DEVICE NOTIFICATION - kAudioDevicePropertyStreamFormat\r");
            break;

        case kAudioDevicePropertyDeviceIsRunning:
            fprintf(stdout, "%s\n", "***** DEVICE NOTIFICATION - kAudioDevicePropertyDeviceIsRunning\r");
            outSize = sizeof(UInt32);
            err = AudioDeviceGetProperty(inDevice, inLine, isInput, kAudioDevicePropertyDeviceIsRunning, &outSize, &theUIntData);

             break;

        case kAudioDevicePropertyVolumeScalar:
            fprintf(stdout, "%s\n", "***** DEVICE NOTIFICATION - kAudioDevicePropertyVolumeScalar\r");
            outSize = sizeof(Float32);
            err = AudioDeviceGetProperty(inDevice, inLine, isInput, kAudioDevicePropertyVolumeScalar, &outSize, &vol);
            break;

        case kAudioDevicePropertyMute:
            fprintf(stdout, "%s\n", "***** DEVICE NOTIFICATION - kAudioDevicePropertyMute\r");
            outSize = sizeof(UInt32);
            err = AudioDeviceGetProperty(inDevice, inLine, isInput, kAudioDevicePropertyMute, &outSize, &mute);
            break;

        case kAudioDevicePropertyPlayThru:
            fprintf(stdout, "%s\n", "***** DEVICE NOTIFICATION - kAudioDevicePropertyPlayThru\r");
            outSize = sizeof(UInt32);
            err = AudioDeviceGetProperty(inDevice, inLine, isInput, kAudioDevicePropertyPlayThru, &outSize, &playThru);

            break;

        case kAudioDevicePropertyDeviceIsAlive:
            fprintf(stdout, "%s\n", "***** DEVICE NOTIFICATION - kAudioDevicePropertyDeviceIsAlive\r");
            outSize = sizeof(UInt32);
            err = AudioDeviceGetProperty(inDevice, 0, false, kAudioDevicePropertyDeviceIsAlive, &outSize, &tLong);

            break;

        case kAudioDevicePropertyDataSource:
            fprintf(stdout, "%s\n", "***** DEVICE NOTIFICATION - kAudioDevicePropertyDataSource\r");
            // get the source
            // match the source to one of the available sources and return the index of that source
            //SetControlValue(control, (chan->vol) * 100);
            break;
    }
        
    fflush(stdout);
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
            fprintf(stdout, "%s\n", "***** STREAM NOTIFICATION - kAudioStreamPropertyPhysicalFormat\r");
            break;

        case kAudioDevicePropertyStreamFormat:
            fprintf(stdout, "%s\n", "***** STREAM NOTIFICATION - kAudioDevicePropertyStreamFormat\r");
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
                
    // kAudioDevicePropertyBufferSize
    err = AudioDeviceAddPropertyListener(inDevice, 0, false, kAudioDevicePropertyBufferSize, deviceListenerProc, inClientData);
    if (err) return err;

    err = AudioDeviceAddPropertyListener(inDevice, 0, true, kAudioDevicePropertyBufferSize, deviceListenerProc, inClientData);
    if (err) return err;


    // kAudioDevicePropertyBufferFrameSize
    err = AudioDeviceAddPropertyListener(inDevice, 0, false, kAudioDevicePropertyBufferFrameSize, deviceListenerProc, inClientData);
    if (err) return err;

    err = AudioDeviceAddPropertyListener(inDevice, 0, true, kAudioDevicePropertyBufferFrameSize, deviceListenerProc, inClientData);
    if (err) return err;

    // kAudioDevicePropertyDeviceIsRunning
    err = AudioDeviceAddPropertyListener(inDevice, 0, false, kAudioDevicePropertyDeviceIsRunning, deviceListenerProc, inClientData);
    if (err) return err;

    err = AudioDeviceAddPropertyListener(inDevice, 0, true, kAudioDevicePropertyDeviceIsRunning, deviceListenerProc, inClientData);
    if (err) return err;

/*
    for (i = 0; i <= deviceInfo->totalOutputChannels; i++)
    {
        // kAudioDevicePropertyVolumeScalar output
        err = AudioDeviceAddPropertyListener(inDevice, i, false, kAudioDevicePropertyVolumeScalar, deviceListenerProc, inClientData);
        if (err) return err;

        // kAudioDevicePropertyVolumeMute output
        err = AudioDeviceAddPropertyListener(inDevice, i, false, kAudioDevicePropertyMute, deviceListenerProc, inClientData);
        if (err) return err;
    }
    
    for (i = 0; i <= deviceInfo->totalInputChannels; i++)
    {
        // kAudioDevicePropertyVolumeScalar input
        err = AudioDeviceAddPropertyListener(inDevice, i, true, kAudioDevicePropertyVolumeScalar, deviceListenerProc, inClientData);
        if (err) return err;

        // kAudioDevicePropertyVolumeMute input
        err = AudioDeviceAddPropertyListener(inDevice, i, true, kAudioDevicePropertyMute, deviceListenerProc, inClientData);
        if (err) return err;

        // kAudioDevicePropertyPlayThru input
        err = AudioDeviceAddPropertyListener(inDevice, i, true, kAudioDevicePropertyPlayThru, deviceListenerProc, inClientData);
        if (err) return err;
    }
*/
    
    // kAudioDevicePropertyDeviceIsAlive
    err = AudioDeviceAddPropertyListener(inDevice, 0, false, kAudioDevicePropertyDeviceIsAlive, deviceListenerProc, inClientData);
    if (err) return err;

    err = AudioDeviceAddPropertyListener(inDevice, 0, true, kAudioDevicePropertyDeviceIsAlive, deviceListenerProc, inClientData);
    if (err) return err;


    // kAudioDevicePropertyStreamFormat
    err = AudioDeviceAddPropertyListener(inDevice, 0, false, kAudioDevicePropertyStreamFormat, deviceListenerProc, inClientData);
    if (err) return err;
    
    err = AudioDeviceAddPropertyListener(inDevice, 0, true, kAudioDevicePropertyStreamFormat, deviceListenerProc, inClientData);
    if (err) return err;

    // kAudioDevicePropertyBufferSizeRange
    err = AudioDeviceAddPropertyListener(inDevice, 0, false, kAudioDevicePropertyBufferSizeRange, deviceListenerProc, inClientData);
    if (err) return err;
    
    err = AudioDeviceAddPropertyListener(inDevice, 0, true, kAudioDevicePropertyBufferSizeRange, deviceListenerProc, inClientData);
    if (err) return err;
        
    //kAudioDevicePropertyDataSource
    err = AudioDeviceAddPropertyListener(inDevice, 0, false, kAudioDevicePropertyDataSource, deviceListenerProc, inClientData);
    if (err) return err;
    
    err = AudioDeviceAddPropertyListener(inDevice, 0, true, kAudioDevicePropertyDataSource, deviceListenerProc, inClientData);
    if (err) return err;

    
    AddStreamListeners (inDevice, kAudioStreamPropertyPhysicalFormat, false, inClientData);
    
    AddStreamListeners (inDevice, kAudioStreamPropertyPhysicalFormat, true, inClientData);
    
    return (err);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
OSStatus AddHardwareListeners (void* inClientData)
{
    OSStatus			err = noErr;
    
    err = AudioHardwareAddPropertyListener(kAudioHardwarePropertyDefaultOutputDevice, hardwareListenerProc, inClientData);
    if (err) return err;

    err = AudioHardwareAddPropertyListener(kAudioHardwarePropertyDefaultInputDevice, hardwareListenerProc, inClientData);
    if (err) return err;

    err = AudioHardwareAddPropertyListener(kAudioHardwarePropertyDefaultSystemOutputDevice, hardwareListenerProc, inClientData);
    if (err) return err;

    err = AudioHardwareAddPropertyListener(kAudioHardwarePropertyDevices, hardwareListenerProc, inClientData);
    if (err) return err;

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
// Audio driver (JACK)

#if SC_AUDIO_API == SC_AUDIO_API_JACK
#include "SC_StringParser.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

static int sc_jack_process_cb(jack_nframes_t numFrames, void *arg);
static int sc_jack_bufsize_cb(jack_nframes_t numSamples, void *arg);
static int sc_jack_srate_cb(jack_nframes_t sampleRate, void *arg);

struct SC_JackPortList
{
	int		mSize;
	jack_port_t	**mPorts;
	float		**mBuffers;

	SC_JackPortList(jack_client_t *client, int numPorts, int type);
	~SC_JackPortList();
};

// =====================================================================
// SC_JackPortList

SC_JackPortList::SC_JackPortList(jack_client_t *client, int numPorts, int type)
	: mSize(numPorts), mPorts(0), mBuffers(0)
{
	int i;
	char *fmt, portname[32];

	mPorts = new jack_port_t*[mSize];
	mBuffers = new float*[mSize];

	if (type == JackPortIsInput) {
		fmt = "in_%d";
	} else {
		fmt = "out_%d";
	}

	for (i = 0; i < mSize; i++) {
		sprintf(portname, fmt, i+1);
		mPorts[i] = jack_port_register(
					client, portname,
					JACK_DEFAULT_AUDIO_TYPE,
					type, 0
				);
		mBuffers[i] = 0;
	}
}

SC_JackPortList::~SC_JackPortList()
{
	delete [] mPorts;
	delete [] mBuffers;
}

// =====================================================================
// JACK callbacks

int sc_jack_process_cb(jack_nframes_t numFrames, void *arg)
{
	((SC_CoreAudioDriver*)arg)->JackRun(GetCurrentOSCTime());
	return 0;
}

int sc_jack_bufsize_cb(jack_nframes_t numSamples, void *arg)
{
	((SC_CoreAudioDriver*)arg)->JackBufferSizeChanged((int)numSamples);
	return 0;
}

int sc_jack_srate_cb(jack_nframes_t sampleRate, void *arg)
{
	((SC_CoreAudioDriver*)arg)->JackSampleRateChanged((double)sampleRate);
	return 0;
}

// =====================================================================
// SC_CoreAudioDriver (JACK)

void SC_CoreAudioDriver::DriverInitialize()
{
	mClient = 0;
	mInputList = mOutputList = 0;
}

void SC_CoreAudioDriver::DriverRelease()
{
	if (mClient) jack_client_close(mClient);
	delete mInputList;
	delete mOutputList;
}

// ====================================================================
// NOTE: for now, in lieu of a mechanism that passes generic options to
// the platform driver, we rely on environment variables:
//
// 	SC_JACK_INPUTS:			number of jack inputs created default=2
// 	SC_JACK_OUTPUTS:		number of jack outputs created default=2
// 	SC_JACK_DEFAULT_INPUTS:		which outports to connect to
// 	SC_JACK_DEFAULT_OUTPUTS:	which inports to connect to
//
// e.g.
// 	SC_JACK_INPUTS=4
// 	SC_JACK_OUTPUTS=4
// 	SC_JACK_DEFAULT_INPUTS=alsa:in_1,alsa:in_2
// 	SC_JACK_DEFAULT_OUTPUTS=alsa:out_1,alsa:out_2
//
// will create four jack in/out ports and connect the first two in/out
// ports to the respective ports of the alsa driver client, up to the
// number of ports actually available.

bool SC_CoreAudioDriver::DriverSetup(int* outNumSamples, double* outSampleRate)
{
	char *env;
	int n;

	// create jack client
	mClient = jack_client_new("SuperCollider");
	if (mClient == 0) return false;

	// create inputs according to SC_JACK_INPUTS (default 2)
	env = getenv("SC_JACK_INPUTS");
	if (env == 0) env = "2";
	n = std::max(2, atoi(env));
	mInputList = new SC_JackPortList(mClient, n, JackPortIsInput);

	// create outputs according to SC_JACK_OUTPUTS (default 2)
	env = getenv("SC_JACK_OUTPUTS");
	if (env == 0) env = "2";
	n = std::max(2, atoi(env));
	mOutputList = new SC_JackPortList(mClient, n, JackPortIsOutput);

	// register callbacks
	jack_set_process_callback(mClient, sc_jack_process_cb, this);
	jack_set_buffer_size_callback(mClient, sc_jack_bufsize_cb, this);
	jack_set_sample_rate_callback(mClient, sc_jack_srate_cb, this);

	*outNumSamples = (int)jack_get_buffer_size(mClient);
	*outSampleRate = (double)jack_get_sample_rate(mClient);

	return true;
}

bool SC_CoreAudioDriver::DriverStart()
{
	assert(mClient);

	int err;
	SC_StringParser sp;
	jack_port_t **ports;
	int numPorts;

	// activate client
	err = jack_activate(mClient);
	if (err) {
		scprintf("SC_CoreAudioDriver: couldn't activate jack client\n");
		return false;
	}

	// connect default inputs
	sp = SC_StringParser(getenv("SC_JACK_DEFAULT_INPUTS"), ',');
	ports = mInputList->mPorts;
	numPorts = mInputList->mSize;
	for (int i = 0; !sp.AtEnd() && (i < numPorts); i++) {
		const char *thisPortName = jack_port_name(ports[i]);
		const char *thatPortName = sp.NextToken();
		if (thisPortName && thatPortName) {
			err = jack_connect(mClient, thatPortName, thisPortName);
			scprintf("SC_CoreAudioDriver: %s %s to %s\n",
				 err ? "couldn't connect " : "connected ",
				 thatPortName, thisPortName);
		}
	}

	// connect default outputs
	sp = SC_StringParser(getenv("SC_JACK_DEFAULT_OUTPUTS"), ',');
	ports = mOutputList->mPorts;
	numPorts = mOutputList->mSize;
	for (int i = 0; !sp.AtEnd() && (i < numPorts); i++) {
		const char *thisPortName = jack_port_name(ports[i]);
		const char *thatPortName = sp.NextToken();
		if (thisPortName && thatPortName) {
			err = jack_connect(mClient, thisPortName, thatPortName);
			scprintf("SC_CoreAudioDriver: %s %s to %s\n",
				 err ? "couldn't connect " : "connected ",
				 thisPortName, thatPortName);
		}
	}

	return true;
}

bool SC_CoreAudioDriver::DriverStop()
{
	int err = 0;
	if (mClient) err = jack_deactivate(mClient);
	return err == 0;
}

void SC_CoreAudioDriver::JackRun(int64 oscTime)
{
	World *world = mWorld;
	
	try {
		mOSCbuftime = oscTime;
		
		mFromEngine.Free();
		mToEngine.Perform();
		
		int numInputs = mInputList->mSize;
		int numOutputs = mOutputList->mSize;
		jack_port_t **inPorts = mInputList->mPorts;
		jack_port_t **outPorts = mOutputList->mPorts;
		float **inBuffers = mInputList->mBuffers;
		float **outBuffers = mOutputList->mBuffers;

		int numSamples = NumSamplesPerCallback();
		int bufFrames = mWorld->mBufLength;
		int numBufs = numSamples / bufFrames;

		float *inBuses = mWorld->mAudioBus + mWorld->mNumOutputs * bufFrames;
		float *outBuses = mWorld->mAudioBus;
		int32 *inTouched = mWorld->mAudioBusTouched + mWorld->mNumOutputs;
		int32 *outTouched = mWorld->mAudioBusTouched;

		int minInputs = std::min(numInputs, mWorld->mNumInputs);
		int minOutputs = std::min(numOutputs, mWorld->mNumOutputs);

		int bufFramePos = 0;

		// cache I/O buffers, blindly assuming
		// jack_default_audio_sample_t is float

		for (int i = 0; i < minInputs; ++i) {
			inBuffers[i] = (float *)jack_port_get_buffer(inPorts[i], numSamples);
		}

		for (int i = 0; i < minOutputs; ++i) {
			outBuffers[i] = (float *)jack_port_get_buffer(outPorts[i], numSamples);
		}

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
				float *src = inBuffers[k] + bufFramePos;
				float *dst = inBuses + k * bufFrames;
				for (int n = 0; n < bufFrames; ++n) *dst++ = *src++;
				*tch++ = bufCounter;
			}

			// run engine
			int64 schedTime;
			int64 nextTime = oscTime + oscInc;
			while ((schedTime = mScheduler.NextTime()) <= nextTime) {
				world->mSampleOffset = (int)((double)(schedTime - oscTime) * oscToSamples);
				SC_ScheduledEvent event = mScheduler.Remove();
				event.Perform();
				world->mSampleOffset = 0;
			}
			
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
			mOSCbuftime = nextTime;
		}
	} catch (std::exception& exc) {
		scprintf("SC_CoreAudioDriver: exception in real time: %s\n", exc.what());
	} catch (...) {
		scprintf("SC_CoreAudioDriver: unknown exception in real time\n");
	}
	mAudioSync.Signal();
}

// ====================================================================
// TODO: do something more appropriate later

void SC_CoreAudioDriver::JackBufferSizeChanged(int numSamples)
{
#ifndef NDEBUG
	std::cerr << "SC_CoreAudioDriver::JackBufferSizeChanged " << numSamples << '\n';
#endif
}

void SC_CoreAudioDriver::JackSampleRateChanged(double sampleRate)
{
#ifndef NDEBUG
	std::cerr << "SC_CoreAudioDriver::JackSampleRateChanged " << sampleRate << '\n';
#endif
}
#endif // SC_AUDIO_API_JACK
