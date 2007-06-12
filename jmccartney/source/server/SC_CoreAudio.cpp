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

const double kMicrosToOSCunits = 4294.967296; // pow(2,32)/1e6
const double kNanosToOSCunits  = 4.294967296; // pow(2,32)/1e9
int64 gOSCoffset = 0; 

const int32 kSECONDS_FROM_1900_to_1970 = (int32)2208988800UL; /* 17 leap years */

int32 timeseed();
int32 timeseed()
{
	int64 time = AudioGetCurrentHostTime();
	return Hash((int32)(time >> 32) + Hash((int32)time));
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

const double kOSCtoSecs = 2.328306436538696e-10;


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
	//printf("gOSCoffset %016llX\n", gOSCoffset);
}

void* resyncThreadFunc(void* arg);
void* resyncThreadFunc(void* /*arg*/)
{
	while (true) {
		sleep(20);
		syncOSCOffsetWithTimeOfDay();
	}
}


void initializeScheduler();
void set_real_time_priority(pthread_t thread);

void initializeScheduler()
{
	//printf("->initializeScheduler\n");
	syncOSCOffsetWithTimeOfDay();
	
	pthread_t resyncThread;
	pthread_create (&resyncThread, NULL, resyncThreadFunc, (void*)0);
	set_real_time_priority(resyncThread);
}

double gSampleRate, gSampleDur;

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
	//printf("ProcessOSCPacket %d, '%s'\n", inPacket->mSize, inPacket->mData);
	
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
	//printf("->PerformOSCMessage '%s'\n", inData);
	SC_LibCmd *cmdObj = gCmdLib->Get((int32*)inData);
	//printf("cmdObj %08X\n", cmdObj);
	if (!cmdObj) {
		CallSendFailureCommand(inWorld, inData, "Command not found", inReply);
		printf("FAILURE %s Command not found\n", inData);
		return kSCErr_NoSuchCommand;
	}
	
	int cmdNameLen = OSCstrlen(inData);
	int err = cmdObj->Perform(inWorld, inSize - cmdNameLen, inData + cmdNameLen, inReply);
	//printf("<-PerformOSCMessage '%s'\n", inData);
	return err;
}

void PerformOSCBundle(World *inWorld, OSC_Packet *inPacket)
{
	//printf("->PerformOSCBundle %d\n", inPacket->mSize);
	char *data = inPacket->mData + 16;
	char* dataEnd = inPacket->mData + inPacket->mSize;
	while (data < dataEnd) {
		int32 msgSize = *(int32*)data;
		data += sizeof(int32);
		//printf("msgSize %d\n", msgSize);
		PerformOSCMessage(inWorld, msgSize, data, &inPacket->mReplyAddr);
		data += msgSize;
	}
	//printf("<-PerformOSCBundle %d\n", inPacket->mSize);
}

////////////////////////////////////////////////////////////////////////////

void Perform_ToEngine_Msg(FifoMsg *inMsg)
{
	//printf("Perform_ToEngine_Msg\n");
	World *world = inMsg->mWorld;
	OSC_Packet *packet = (OSC_Packet*)inMsg->mData;
	if (!packet) return;
	
	// in real time engine, schedule the packet
	int64 time = *(int64*)(packet->mData + 8);
	//printf("recvTime %lld\n", time);
	//printf("sched %.9f\n", time*kOSCtoSecs);
	
	SC_CoreAudioDriver *driver = inMsg->mWorld->hw->mAudioDriver;
	
	if (!packet->mIsBundle) {
		PerformOSCMessage(world, packet->mSize, packet->mData, &packet->mReplyAddr);
	} else if (time == 0 || time == 1) {
		PerformOSCBundle(world, packet);
	} else {
		if (time < driver->mOSCbuftime) {
			double seconds = (driver->mOSCbuftime - time)*kOSCtoSecs;
			printf("late %.9f\n", seconds);
			//FifoMsg outMsg;
			
			//ReportLateness(packet->mReply, seconds)
		}
		
		SC_ScheduledEvent event(world, time, packet);
		driver->AddEvent(event);
		inMsg->mFreeFunc = 0;
	}
}


void FreeOSCPacket(OSC_Packet *inPacket);
void FreeOSCPacket(OSC_Packet *inPacket)
{
	//printf("->FreeOSCPacket %08X\n", inPacket);
	if (inPacket) {
		free(inPacket->mData);
		free(inPacket);
	}
}

void Free_ToEngine_Msg(FifoMsg *inMsg)
{
	//printf("->Free_ToEngine_Msg %08X\n", inMsg);
	OSC_Packet *packet = (OSC_Packet*)inMsg->mData;
	FreeOSCPacket(packet);
	//printf("<-Free_ToEngine_Msg %08X\n", inMsg);
}

void Free_FromEngine_Msg(FifoMsg *inMsg);
void Free_FromEngine_Msg(FifoMsg *inMsg)
{
	//printf("->Free_FromEngine_Msg %08X\n", inMsg);
	World_Free(inMsg->mWorld, inMsg->mData);
	//printf("<-Free_FromEngine_Msg %08X\n", inMsg);
}

void DoneWithPacket(FifoMsg *inMsg);
void DoneWithPacket(FifoMsg *inMsg)
{
	OSC_Packet *packet = (OSC_Packet*)inMsg->mData;
	FreeOSCPacket(packet);
}

SC_CoreAudioDriver::SC_CoreAudioDriver(struct World *inWorld)
		: mWorld(inWorld), mActive(false), mNumSamples(0), mSampleTime(0)
{
	mProcessPacketLock = new SC_Lock();
}


void* core_audio_thread_func(void* arg);
void* core_audio_thread_func(void* arg)
{
	SC_CoreAudioDriver *ca = (SC_CoreAudioDriver*)arg;
	void* result = ca->RunThread();
	return result;
}


bool SC_CoreAudioDriver::Setup()
{
	OSStatus	err = kAudioHardwareNoError;
	UInt32	count;
	mOutputDevice = kAudioDeviceUnknown;
	mInputDevice = kAudioDeviceUnknown;

	//printf("SC_CoreAudioDriver::Setup world %08X\n", mWorld);

	////////////////////////////////////////////////////////////////////////////////////////////////

	/*{
		err = AudioHardwareGetPropertyInfo(kAudioHardwarePropertyDevices, &count, 0);

		AudioDeviceID *devices = (AudioDeviceID*)malloc(count);
		err = AudioHardwareGetProperty(kAudioHardwarePropertyDevices, &count, devices);
		if (err != kAudioHardwareNoError) {
			fprintf(stdout, "get kAudioHardwarePropertyDevices error %c%c%c%c\n", (err>>24)&255,(err>>16)&255,(err>>8)&255,(err)&255);
			return false;
		}
		int numdevices = count / sizeof(AudioDeviceID);
		fprintf(stdout, "numdevices %d\n", numdevices);
		for (int i = 0; i < numdevices; ++i) {
			err = AudioDeviceGetPropertyInfo(devices[i], 0, false, kAudioDevicePropertyDeviceName, &count, 0);

			char *name = (char*)malloc(count);
			err = AudioDeviceGetProperty(devices[i], 0, false, kAudioDevicePropertyDeviceName, &count, name);
			if (err != kAudioHardwareNoError) {
				fprintf(stdout, "get kAudioDevicePropertyDeviceName error %c%c%c%c\n", (err>>24)&255,(err>>16)&255,(err>>8)&255,(err)&255);
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
					fprintf(stdout, "get kAudioDevicePropertyStreamConfiguration error %c%c%c%c\n", (err>>24)&255,(err>>16)&255,(err>>8)&255,(err)&255);
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

	pthread_create (&mThread, NULL, core_audio_thread_func, (void*)this);
	set_real_time_priority(mThread);

	// get the default output device for the HAL
	count = sizeof(mOutputDevice);		// it is required to pass the size of the data to be returned
	//get the output device:
	err = AudioHardwareGetProperty(kAudioHardwarePropertyDefaultOutputDevice, &count, (void *) & mOutputDevice);
	//get the input device
	err = AudioHardwareGetProperty(kAudioHardwarePropertyDefaultInputDevice, &count, (void *) & mInputDevice);

	if (err != kAudioHardwareNoError) {
		fprintf(stdout, "get kAudioHardwarePropertyDefaultOutputDevice error %c%c%c%c\n", (err>>24)&255,(err>>16)&255,(err>>8)&255,(err)&255);
		return false;
	}

	// get the buffersize that the default device uses for IO
	count = sizeof(mHardwareBufferSize);	// it is required to pass the size of the data to be returned
	err = AudioDeviceGetProperty(mOutputDevice, 0, false, kAudioDevicePropertyBufferSize, &count, &mHardwareBufferSize);
	if (err != kAudioHardwareNoError) {
		fprintf(stdout, "get kAudioDevicePropertyBufferSize error %c%c%c%c\n", (err>>24)&255,(err>>16)&255,(err>>8)&255,(err)&255);
		return false;
	}
	//fprintf(stdout, "mHardwareBufferSize = %ld\n", mHardwareBufferSize);

	// get a description of the data format used by the default input device
	count = sizeof(AudioStreamBasicDescription);	// it is required to pass the size of the data to be returned
	err = AudioDeviceGetProperty(mInputDevice, 0, true, kAudioDevicePropertyStreamFormat, &count, &inputStreamDesc);
	if (err != kAudioHardwareNoError) {
		fprintf(stdout, "get kAudioDevicePropertyStreamFormat error %c%c%c%c\n", (err>>24)&255,(err>>16)&255,(err>>8)&255,(err)&255);
		return false;
	}

	count = sizeof(AudioStreamBasicDescription);	// it is required to pass the size of the data to be returned
	err = AudioDeviceGetProperty(mInputDevice, 0, true, kAudioDevicePropertyStreamFormat, &count, &inputStreamDesc);
	if (err != kAudioHardwareNoError) {
		fprintf(stdout, "get kAudioDevicePropertyStreamFormat error %c%c%c%c\n", (err>>24)&255,(err>>16)&255,(err>>8)&255,(err)&255);
		return false;
	}

	// get a description of the data format used by the default output device
	count = sizeof(AudioStreamBasicDescription);	// it is required to pass the size of the data to be returned
	err = AudioDeviceGetProperty(mOutputDevice, 0, false, kAudioDevicePropertyStreamFormat, &count, &outputStreamDesc);
	if (err != kAudioHardwareNoError) {
		fprintf(stdout, "get kAudioDevicePropertyStreamFormat error %c%c%c%c\n", (err>>24)&255,(err>>16)&255,(err>>8)&255,(err)&255);
		return false;
	}
	if (inputStreamDesc.mSampleRate != outputStreamDesc.mSampleRate) {
		fprintf(stdout, "input and output sample rates do not match. %g != %g\n", inputStreamDesc.mSampleRate, outputStreamDesc.mSampleRate);
		return false;
	}

	if (UseSeparateIO()) {
		count = sizeof(UInt32);	// it is required to pass the size of the data to be returned
		err = AudioDeviceGetProperty(mInputDevice, 0, true, kAudioDevicePropertySafetyOffset, &count, &mSafetyOffset);
		if (err != kAudioHardwareNoError) {
			fprintf(stdout, "get kAudioDevicePropertySafetyOffset error %c%c%c%c\n", (err>>24)&255,(err>>16)&255,(err>>8)&255,(err)&255);
			return false;
		}
		printf("mSafetyOffset %d\n", mSafetyOffset);
	
		Boolean writeable;
		err = AudioDeviceGetPropertyInfo(mInputDevice, 0, true, kAudioDevicePropertyStreamConfiguration, &count, &writeable);
		mInputBufList = (AudioBufferList*)malloc(count);
		err = AudioDeviceGetProperty(mInputDevice, 0, true, kAudioDevicePropertyStreamConfiguration, &count, mInputBufList);
		if (err != kAudioHardwareNoError) {
			fprintf(stdout, "get kAudioDevicePropertyStreamConfiguration error %c%c%c%c\n", (err>>24)&255,(err>>16)&255,(err>>8)&255,(err)&255);
			return false;
		}
	
		printf("mNumberBuffers %d\n", mInputBufList->mNumberBuffers);
		for (uint32 i=0; i<mInputBufList->mNumberBuffers; ++i) {
			printf("  mDataByteSize %d %d\n", i, mInputBufList->mBuffers[i].mDataByteSize);
			mInputBufList->mBuffers[i].mData = calloc(1, mInputBufList->mBuffers[i].mDataByteSize);
		}
	
	
		AudioTimeStamp	now;
		now.mFlags = kAudioTimeStampHostTimeValid;
		now.mHostTime = AudioGetCurrentHostTime();
		
		err = AudioDeviceSetProperty(mInputDevice, &now, 0, true, kAudioDevicePropertyRegisterBufferList, count, mInputBufList);
		if (err != kAudioHardwareNoError) {
			fprintf(stdout, "get kAudioDevicePropertyRegisterBufferList error %c%c%c%c\n", (err>>24)&255,(err>>16)&255,(err>>8)&255,(err)&255);
			return false;
		}
	}

	mNumSamples = mHardwareBufferSize / outputStreamDesc.mBytesPerFrame;
	printf("mNumSamples %d\n", mNumSamples);
	printf("mHardwareBufferSize %d\n", mHardwareBufferSize);
	  
	// compute a per sample increment to the OpenSoundControl Time
	mOSCincrement = (int64)(mWorld->mBufLength * pow(2.,32.)/outputStreamDesc.mSampleRate);
	mOSCtoSamples = outputStreamDesc.mSampleRate / pow(2.,32.);

	World_SetSampleRate(mWorld, outputStreamDesc.mSampleRate);
	return true;
}

void* SC_CoreAudioDriver::RunThread()
{
	MsgFifoNoFree<TriggerMsg, 1024> *trigfifo = &mWorld->hw->mTriggers;
	MsgFifoNoFree<NodeEndMsg, 1024> *nodeendfifo = &mWorld->hw->mNodeEnds;
	MsgFifoNoFree<DeleteGraphDefMsg, 512> *deletegraphfifo = &mWorld->hw->mDeleteGraphDefs;

	while (true) {
		// wait for sync
		mAudioSync.WaitNext();
		
		// send /tr messages
		trigfifo->Perform();		
		
		// send /n.end messages
		nodeendfifo->Perform();
		
		// free GraphDefs
		deletegraphfifo->Perform();
		
		// perform messages
		mFromEngine.Perform();
	}
	return 0;
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
	readTime.mSampleTime = inNow->mSampleTime - def->SafetyOffset() - def->NumSamples();
	readTime.mFlags = kAudioTimeStampSampleTimeValid;
	
	AudioDeviceRead(def->InputDevice(), &readTime, def->GetInputBufferList());
	
	def->Run(inInputData, outOutputData, oscTime);

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

	def->Run(inInputData, outOutputData, oscTime);

	return kAudioHardwareNoError;
}

void SC_CoreAudioDriver::Run(const AudioBufferList* inInputData, 
					AudioBufferList* outOutputData, int64 oscTime)
{
	try {
		int numSamples = NumSamples();
		mOSCbuftime = oscTime;
		
		mFromEngine.Free();
		/*if (mToEngine.HasData()) {
			printf("oscTime %.9f %.9f\n", oscTime*kOSCtoSecs, CoreAudioHostTimeToOSC(AudioGetCurrentHostTime())*kOSCtoSecs);
		}*/
		mToEngine.Perform();
		
		int bufFrames = mWorld->mBufLength;
		int numBufs = numSamples / bufFrames;
		
		int numInputBuses = mWorld->mNumInputs;
		int numOutputBuses = mWorld->mNumOutputs;
		float* inputBuses = mWorld->mAudioBus + mWorld->mNumOutputs * bufFrames;
		float* outputBuses = mWorld->mAudioBus;
		int32* inputTouched = mWorld->mAudioBusTouched + mWorld->mNumOutputs;
		int32* outputTouched = mWorld->mAudioBusTouched;
		int numInputStreams = inInputData->mNumberBuffers;
		int numOutputStreams = outOutputData->mNumberBuffers;
		
		//static int go = 0;
		
		int64 oscInc = mOSCincrement;
	
		int bufFramePos = 0;
		
		for (int i = 0; i < numBufs; ++i, mWorld->mBufCounter++, bufFramePos += bufFrames) {
			int32 bufCounter = mWorld->mBufCounter;
			
			// de-interleave input
			if (inInputData->mBuffers) {
				for (int s = 0, b = 0; b<numInputBuses && s < numInputStreams; s++) {
					const AudioBuffer* buf = inInputData->mBuffers + s;
					int nchan = buf->mNumberChannels;
					if (buf->mData) {
						float *busdata = inputBuses;
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
									//if (i==0&&k<4) printf("%d %d %g\n", j, k, bufdata[m]);
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
					
			/*if (mScheduler.Ready(mOSCbuftime)) {
				double diff = (mScheduler.NextTime() - mOSCbuftime)*kOSCtoSecs; 
				printf("rdy %.9f %.9f %.9f\n", mScheduler.NextTime() * kOSCtoSecs, mOSCbuftime*kOSCtoSecs, diff);
			}*/
			
			//mScheduler.Perform(mOSCbuftime);
			int64 schedTime;
			int64 nextTime = mOSCbuftime + oscInc;
			while ((schedTime = mScheduler.NextTime()) <= nextTime) {
				mSampleOffset = (int)((double)(schedTime - mOSCbuftime) * mOSCtoSamples);
				SC_ScheduledEvent event = mScheduler.Remove();
				event.Perform();
			}
			
			World_Run(mWorld);
	
			// interleave output
			if (outOutputData->mBuffers) {
				for (int s = 0, b = 0; b<numOutputBuses && s < numOutputStreams; s++) {
					AudioBuffer* buf = outOutputData->mBuffers + s;
					int nchan = buf->mNumberChannels;
					if (buf->mData) {
						float *busdata = outputBuses;
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
			}
			mOSCbuftime = nextTime;
		}
	} catch (std::exception& exc) {
		printf("exception in real time: %s\n", exc.what());
	} catch (...) {
		printf("unknown exception in real time\n");
	}
	//if (go == 1) go = 2;
	mAudioSync.Signal();
}

bool SC_CoreAudioDriver::Start()
{
	OSStatus	err = kAudioHardwareNoError;

	mActive = true;

	World_Start(mWorld);
	
	printf("start   UseSeparateIO?: %d\n", UseSeparateIO());
	
	if (UseSeparateIO()) {
		err = AudioDeviceAddIOProc(mOutputDevice, appIOProc2, (void *) this);	// setup our device with an IO proc
		if (err != kAudioHardwareNoError) {
			fprintf(stdout, "AudioDeviceAddIOProc failed %d\n", (int)err);
			return false;
		}
	
		err = AudioDeviceStart(mOutputDevice, appIOProc2);		// start playing sound through the device
		if (err != kAudioHardwareNoError) {
			fprintf(stdout, "AudioDeviceStart failed %d\n", (int)err);
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
	mSampleTime = 0;

	return true;
}


bool SC_CoreAudioDriver::Stop()
{
	printf("SC_CoreAudioDriver::Stop\n");
	OSStatus err = kAudioHardwareNoError;

	mActive = false;

	err = AudioDeviceStop(mOutputDevice, appIOProc);		// stop playing sound through the device
	if (err != kAudioHardwareNoError) return false;

	err = AudioDeviceRemoveIOProc(mOutputDevice, appIOProc);	// remove the IO proc from the device
	if (err != kAudioHardwareNoError) return false;
	
	World_Stop(mWorld);
	
	return true;
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
            if (err == noErr)
            {
                err = AudioHardwareGetProperty(kAudioHardwarePropertyDefaultOutputDevice, &outSize, &deviceID);
                if (err == noErr)
                {
                    err =  AudioDeviceGetPropertyInfo(deviceID, 0, false, kAudioDevicePropertyDeviceName,  &outSize, &outWritable);
                    if (err == noErr)
                    {
                        err = AudioDeviceGetProperty(deviceID, 0, false, kAudioDevicePropertyDeviceName, &outSize, cStr);
                        if (err == noErr)
                        {
							// do something
                        }
                    }
                }
            }
            
            break;
       
        case kAudioHardwarePropertyDefaultInputDevice:
            fprintf(stdout, "%s\n", "***** HARDWARE NOTIFICATION - kAudioHardwarePropertyDefaultInputDevice\r");
            err =  AudioHardwareGetPropertyInfo(kAudioHardwarePropertyDefaultInputDevice,  &outSize, &outWritable);
            if (err == noErr)
            {
                err = AudioHardwareGetProperty(kAudioHardwarePropertyDefaultInputDevice, &outSize, &deviceID);
                if (err == noErr)
                {
                    err =  AudioDeviceGetPropertyInfo(deviceID, 0, false, kAudioDevicePropertyDeviceName,  &outSize, &outWritable);
                    if (err == noErr)
                    {
                        err = AudioDeviceGetProperty(deviceID, 0, false, kAudioDevicePropertyDeviceName, &outSize, cStr);
                        if (err == noErr)
                        {
							// do something
                        }
                    }
                }
            }
           break;
            
        case kAudioHardwarePropertyDefaultSystemOutputDevice:
            fprintf(stdout, "%s\n", "***** HARDWARE NOTIFICATION - kAudioHardwarePropertyDefaultSystemOutputDevice\r");
            err =  AudioHardwareGetPropertyInfo(kAudioHardwarePropertyDefaultSystemOutputDevice,  &outSize, &outWritable);
            if (err == noErr)
            {
                err = AudioHardwareGetProperty(kAudioHardwarePropertyDefaultSystemOutputDevice, &outSize, &deviceID);
                if (err == noErr)
                {
                    err =  AudioDeviceGetPropertyInfo(deviceID, 0, false, kAudioDevicePropertyDeviceName,  &outSize, &outWritable);
                    if (err == noErr)
                    {
                        err = AudioDeviceGetProperty(deviceID, 0, false, kAudioDevicePropertyDeviceName, &outSize, cStr);
                        if (err == noErr)
                        {
							// do something
                        }
                    }
                }
            }

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


