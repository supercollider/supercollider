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
#include <stdarg.h>
#include "SC_SequencedCommand.h"
#include "SC_Prototypes.h"
#include "SC_HiddenWorld.h"
#include "SC_WorldOptions.h"
#include "SC_Endian.h"
#include "SC_Lib_Cintf.h"
#include "SC_Lock.h"
#include "SC_Time.hpp"
#include <stdlib.h>
#include <algorithm>

#ifndef _WIN32
#    include <sys/time.h>
#endif

#ifdef SC_IPHONE
#    include "SC_VFP11.h"
#endif

#include "nova-tt/thread_priority.hpp"


int64 gStartupOSCTime = -1;

double gSampleRate, gSampleDur;

void sc_SetDenormalFlags();


// =====================================================================
// Timing (CoreAudio)

#if SC_AUDIO_API == SC_AUDIO_API_COREAUDIO || SC_AUDIO_API == SC_AUDIO_API_AUDIOUNITS

int64 gOSCoffset = 0;

int32 server_timeseed() {
    static int32 count = 0;
    int64 time = AudioGetCurrentHostTime();
    return (int32)(time >> 32) ^ (int32)time ^ count--;
}

inline int64 CoreAudioHostTimeToOSC(int64 hostTime) {
    return (int64)((double)AudioConvertHostTimeToNanos(hostTime) * kNanosToOSCunits) + gOSCoffset;
}

int64 oscTimeNow() { return CoreAudioHostTimeToOSC(AudioGetCurrentHostTime()); }

static void syncOSCOffsetWithTimeOfDay() {
    // generate a value gOSCoffset such that
    // (gOSCOffset + systemTimeInOSCunits)
    // is equal to gettimeofday time in OSCunits.
    // Then if this machine is synced via NTP, we are synced with the world.
    // more accurate way to do this??

    using namespace std::chrono;
    struct timeval tv;

    nanoseconds systemTimeBefore, systemTimeAfter;
    int64 diff, minDiff = 0x7fffFFFFffffFFFFLL;

    // take best of several tries
    const int numberOfTries = 5;
    int64 newOffset = gOSCoffset;
    for (int i = 0; i < numberOfTries; ++i) {
        systemTimeBefore = high_resolution_clock::now().time_since_epoch();
        gettimeofday(&tv, 0);
        systemTimeAfter = high_resolution_clock::now().time_since_epoch();

        diff = (systemTimeAfter - systemTimeBefore).count();
        if (diff < minDiff) {
            minDiff = diff;
            // assume that gettimeofday happens halfway between high_resolution_clock::now() calls
            int64 systemTimeBetween = systemTimeBefore.count() + diff / 2;
            int64 systemTimeInOSCunits = (int64)((double)systemTimeBetween * kNanosToOSCunits);
            int64 timeOfDayInOSCunits =
                ((int64)(tv.tv_sec + kSECONDS_FROM_1900_to_1970) << 32) + (int64)(tv.tv_usec * kMicrosToOSCunits);
            newOffset = timeOfDayInOSCunits - systemTimeInOSCunits;
        }
    }

    gOSCoffset = newOffset;
    // scprintf("gOSCoffset %016llX\n", gOSCoffset);
}

static void resyncThreadFunc() {
#    ifdef NOVA_TT_PRIORITY_RT
    std::pair<int, int> priorities = nova::thread_priority_interval_rt();
    nova::thread_set_priority_rt((priorities.first + priorities.second) / 2);
#    else
    std::pair<int, int> priorities = nova::thread_priority_interval();
    nova::thread_set_priority(priorities.second);
#    endif

    while (true) {
        sleep(20);
        syncOSCOffsetWithTimeOfDay();
    }
}

void initializeScheduler() {
    syncOSCOffsetWithTimeOfDay();

    SC_Thread resyncThread(resyncThreadFunc);
    resyncThread.detach();
}
#endif // SC_AUDIO_API_COREAUDIO


// =====================================================================
// Timing (CoreAudioIPHONE)

#if SC_AUDIO_API == SC_AUDIO_API_COREAUDIOIPHONE

int64 gOSCoffset = 0;

static inline int64 GetMicroseconds() {
    struct timeval tv;
    gettimeofday(&tv, 0);
    return (int64)tv.tv_sec * 1000000 + tv.tv_usec;
}

static inline int64 GetCurrentOSCTime() {
    struct timeval tv;
    uint64 s, f;
    gettimeofday(&tv, 0);
    s = (uint64)tv.tv_sec + (uint64)kSECONDS_FROM_1900_to_1970;
    f = (uint64)((double)tv.tv_usec * kMicrosToOSCunits);

    return (s << 32) + f;
}

int32 server_timeseed() {
    int64 time = GetCurrentOSCTime();
    return Hash((int32)(time >> 32) + Hash((int32)time));
}

int64 oscTimeNow() { return GetCurrentOSCTime(); }

void initializeScheduler() { gOSCoffset = GetCurrentOSCTime(); }
#endif // SC_AUDIO_API_COREAUDIO


// =====================================================================
// Packets (Common)

bool ProcessOSCPacket(World* inWorld, OSC_Packet* inPacket);
void PerformOSCBundle(World* inWorld, OSC_Packet* inPacket);
int PerformOSCMessage(World* inWorld, int inSize, char* inData, ReplyAddress* inReply);
PacketStatus PerformOSCPacket(World* world, OSC_Packet* packet, SC_ScheduledEvent::PacketFreeFunc);

void Perform_ToEngine_Msg(FifoMsg* inMsg);
void FreeOSCPacket(FifoMsg* inMsg);

struct IsBundle {
    IsBundle() { str4cpy(s, "#bundle"); }
    bool checkIsBundle(int32* in) { return in[0] == s[0] && in[1] == s[1]; }
    int32 s[2];
};
IsBundle gIsBundle;

bool ProcessOSCPacket(World* inWorld, OSC_Packet* inPacket) {
    // scprintf("ProcessOSCPacket %d, '%s'\n", inPacket->mSize, inPacket->mData);
    if (!inPacket)
        return false;
    bool result;
    static_cast<SC_Lock*>(inWorld->mDriverLock)->lock();
    SC_AudioDriver* driver = AudioDriver(inWorld);
    if (!driver) {
        static_cast<SC_Lock*>(inWorld->mDriverLock)->unlock();
        return false;
    }
    inPacket->mIsBundle = gIsBundle.checkIsBundle((int32*)inPacket->mData);
    FifoMsg fifoMsg;
    fifoMsg.Set(inWorld, Perform_ToEngine_Msg, FreeOSCPacket, (void*)inPacket);
    result = driver->SendOscPacketMsgToEngine(fifoMsg);
    static_cast<SC_Lock*>(inWorld->mDriverLock)->unlock();
    if (!result)
        scprintf("command FIFO full\n");
    return result;
}

int PerformOSCMessage(World* inWorld, int inSize, char* inData, ReplyAddress* inReply) {
    // scprintf("->PerformOSCMessage %d\n", inData[0]);
    SC_LibCmd* cmdObj;
    int cmdNameLen;
    if (inData[0] == 0) {
        cmdNameLen = 4;
        uint32 index = inData[3];
        if (index >= NUMBER_OF_COMMANDS)
            cmdObj = 0;
        else
            cmdObj = gCmdArray[index];
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
    // scprintf("<-PerformOSCMessage %d\n", inData[0]);
    return err;
}

void PerformOSCBundle(World* inWorld, OSC_Packet* inPacket) {
    // scprintf("->PerformOSCBundle %d\n", inPacket->mSize);
    char* data = inPacket->mData + 16;
    char* dataEnd = inPacket->mData + inPacket->mSize;

    while (data < dataEnd) {
        int32 msgSize = sc_ntohl(*(int32*)data);
        data += sizeof(int32);
        // scprintf("msgSize %d\n", msgSize);
        PerformOSCMessage(inWorld, msgSize, data, &inPacket->mReplyAddr);
        data += msgSize;
    }

    // reset so next command uses permanent error notification status
    inWorld->mLocalErrorNotification = 0;

    // scprintf("<-PerformOSCBundle %d\n", inPacket->mSize);
}

PacketStatus PerformOSCPacket(World* world, OSC_Packet* packet, SC_ScheduledEvent::PacketFreeFunc freeFunc) {
    SC_AudioDriver* driver = world->hw->mAudioDriver;

    if (!packet->mIsBundle) {
        PerformOSCMessage(world, packet->mSize, packet->mData, &packet->mReplyAddr);
        world->mLocalErrorNotification = 0;
        return PacketPerformed;
    } else {
        // in real time engine, schedule the packet
        int64 time = OSCtime(packet->mData + 8);
        if (time == 0 || time == 1) {
            PerformOSCBundle(world, packet);
            return PacketPerformed;
        } else {
            if ((time < driver->mOSCbuftime) && (world->mVerbosity >= 0)) {
                double seconds = (driver->mOSCbuftime - time) * kOSCtoSecs;
                scprintf("late %.9f\n", seconds);
                // FifoMsg outMsg;

                // ReportLateness(packet->mReply, seconds)
            }
            // DEBUG
            // else
            // scprintf("scheduled in %.6f at time %.6f\n",
            //	(time-driver->mOSCbuftime)*kOSCtoSecs,
            //	(time-gStartupOSCTime)*kOSCtoSecs);

            SC_ScheduledEvent event(world, time, packet, freeFunc);
            driver->AddEvent(event);
            return PacketScheduled;
        }
    }
}

////////////////////////////////////////////////////////////////////////////

void Perform_ToEngine_Msg(FifoMsg* inMsg) {
    World* world = inMsg->mWorld;
    OSC_Packet* packet = (OSC_Packet*)inMsg->mData;
    if (!packet)
        return;

    PacketStatus status = PerformOSCPacket(world, packet, SC_ScheduledEvent::FreeInNRT);
    if (status == PacketScheduled) {
        // Transfer ownership
        inMsg->mData = 0;
        inMsg->mFreeFunc = 0;
    }
}

PacketStatus PerformCompletionMsg(World* inWorld, const OSC_Packet& inPacket) {
    OSC_Packet* packet = (OSC_Packet*)World_Alloc(inWorld, sizeof(OSC_Packet));
    *packet = inPacket;
    packet->mIsBundle = gIsBundle.checkIsBundle((int32*)packet->mData);
    PacketStatus status = PerformOSCPacket(inWorld, packet, SC_ScheduledEvent::FreeInRT);
    if (status == PacketPerformed) {
        World_Free(inWorld, packet);
    }
    return status;
}

void FreeOSCPacket(FifoMsg* inMsg) {
    OSC_Packet* packet = (OSC_Packet*)inMsg->mData;
    if (packet) {
        inMsg->mData = 0;
#if _MSC_VER == 1310
#    pragma message("$$$todo fixme hack for the 'uninitialized packet->mData ptr when using MSVC 7.1 debug")
        if (packet->mData != reinterpret_cast<char*>(0xcdcdcdcd))
            free(packet->mData);
#else //#ifdef _MSC_VER
        free(packet->mData);
#endif //#ifdef _MSC_VER
        free(packet);
    }
}

void Free_FromEngine_Msg(FifoMsg* inMsg);
void Free_FromEngine_Msg(FifoMsg* inMsg) { World_Free(inMsg->mWorld, inMsg->mData); }

// =====================================================================
// Audio driver (Common)

SC_AudioDriver::SC_AudioDriver(struct World* inWorld):
    mWorld(inWorld),
    mSampleTime(0),
    mNumSamplesPerCallback(0)

{}

SC_AudioDriver::~SC_AudioDriver() {
    mRunThreadFlag = false;
    mAudioSync.Signal();
    mThread.join();
}

void SC_AudioDriver::RunThread() {
#ifdef NOVA_TT_PRIORITY_RT
    std::pair<int, int> priorities = nova::thread_priority_interval_rt();
    nova::thread_set_priority_rt((priorities.first + priorities.second) / 2);
#else
    std::pair<int, int> priorities = nova::thread_priority_interval();
    nova::thread_set_priority(priorities.second);
#endif

    TriggersFifo* trigfifo = &mWorld->hw->mTriggers;
    NodeReplyFifo* nodereplyfifo = &mWorld->hw->mNodeMsgs;
    NodeEndsFifo* nodeendfifo = &mWorld->hw->mNodeEnds;
    DeleteGraphDefsFifo* deletegraphfifo = &mWorld->hw->mDeleteGraphDefs;

    while (mRunThreadFlag) {
        // wait for sync
        mAudioSync.WaitNext();

        reinterpret_cast<SC_Lock*>(mWorld->mNRTLock)->lock();

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

        reinterpret_cast<SC_Lock*>(mWorld->mNRTLock)->unlock();
    }
}

bool SC_AudioDriver::SendMsgFromEngine(FifoMsg& inMsg) { return mFromEngine.Write(inMsg); }


bool SC_AudioDriver::SendMsgToEngine(FifoMsg& inMsg) {
    mToEngine.Free();
    return mToEngine.Write(inMsg);
}

bool SC_AudioDriver::SendOscPacketMsgToEngine(FifoMsg& inMsg) {
    mOscPacketsToEngine.Free();
    return mOscPacketsToEngine.Write(inMsg);
}

void SC_ScheduledEvent::FreeInRT(struct World* world, OSC_Packet* packet) {
    World_Free(world, packet->mData);
    World_Free(world, packet);
}

void SC_ScheduledEvent::FreeInNRT(struct World* world, OSC_Packet* packet) {
    FifoMsg msg;
    msg.Set(world, FreeOSCPacket, 0, (void*)packet);
    world->hw->mAudioDriver->SendMsgFromEngine(msg);
}

void SC_ScheduledEvent::Perform() {
    PerformOSCBundle(mWorld, mPacket);
    (*mPacketFreeFunc)(mWorld, mPacket);
}

bool SC_AudioDriver::Setup() {
    mRunThreadFlag = true;
    SC_Thread thread(std::bind(&SC_AudioDriver::RunThread, this));
    mThread = std::move(thread);

    int numSamples;
    double sampleRate;

    if (!DriverSetup(&numSamples, &sampleRate))
        return false;

    mNumSamplesPerCallback = numSamples;
    // scprintf("mNumSamplesPerCallback %d\n", mNumSamplesPerCallback);
    // scprintf("mHardwareBufferSize %lu\n", mHardwareBufferSize);

    // compute a per sample increment to the OpenSoundControl Time
    mOSCincrementNumerator = (double)mWorld->mBufLength * pow(2., 32.);
    mOSCincrement = (int64)(mOSCincrementNumerator / sampleRate);
    mOSCtoSamples = sampleRate / pow(2., 32.);

    World_SetSampleRate(mWorld, sampleRate);
    mSampleRate = mSmoothSampleRate = sampleRate;
    mBuffersPerSecond = sampleRate / mNumSamplesPerCallback;
    mMaxPeakCounter = (int)mBuffersPerSecond;

    if (mWorld->mVerbosity >= 0) {
        scprintf("SC_AudioDriver: sample rate = %f, driver's block size = %d\n", sampleRate, mNumSamplesPerCallback);
    }

    return true;
}

bool SC_AudioDriver::Start() {
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

bool SC_AudioDriver::Stop() {
    if (!DriverStop())
        return false;
    return true;
}


// =====================================================================
// Audio driver (CoreAudio)
#if SC_AUDIO_API == SC_AUDIO_API_COREAUDIO

SC_AudioDriver* SC_NewAudioDriver(struct World* inWorld) { return new SC_CoreAudioDriver(inWorld); }

#endif

#if SC_AUDIO_API == SC_AUDIO_API_COREAUDIO || SC_AUDIO_API == SC_AUDIO_API_AUDIOUNITS

SC_CoreAudioDriver::SC_CoreAudioDriver(struct World* inWorld): SC_AudioDriver(inWorld), mInputBufList(0) {}

SC_CoreAudioDriver::~SC_CoreAudioDriver() {
    if (mInputBufList) {
        int i;
        for (i = 0; i < mInputBufList->mNumberBuffers; i++) {
            free(mInputBufList->mBuffers[i].mData);
        }
        free(mInputBufList);
    }
}

std::vector<AudioValueRange> GetAvailableNominalSampleRates(const AudioDeviceID& device) {
    std::vector<AudioValueRange> result;
    OSStatus err;
    UInt32 size;
    UInt32 count;
    std::unique_ptr<AudioValueRange[]> validSampleRateRanges;

    AudioObjectPropertyAddress addr = { kAudioDevicePropertyAvailableNominalSampleRates,
                                        kAudioObjectPropertyScopeGlobal, kAudioObjectPropertyElementMaster };

    err = AudioObjectGetPropertyDataSize(device, &addr, 0, NULL, &size);

    if (err != kAudioHardwareNoError) {
        scprintf("get kAudioDevicePropertyAvailableNominalSampleRates data size error %4.4s\n", (char*)&err);
    } else {
        if (size > 0) {
            count = size / sizeof(AudioValueRange);
            validSampleRateRanges = std::unique_ptr<AudioValueRange[]>(new AudioValueRange[count]);

            err = AudioObjectGetPropertyData(device, &addr, 0, NULL, &size, validSampleRateRanges.get());
            if (err != kAudioHardwareNoError) {
                scprintf("get kAudioDevicePropertyAvailableNominalSampleRates error %4.4s\n", (char*)&err);
            } else {
                for (int i = 0; i < count; i++) {
                    result.push_back(validSampleRateRanges[i]);
                }
            }
        }
    }

    return result;
}

bool SC_CoreAudioDriver::DriverSetup(int* outNumSamplesPerCallback, double* outSampleRate) {
    OSStatus err = kAudioHardwareNoError;
    UInt32 count;
    mOutputDevice = kAudioDeviceUnknown;
    mInputDevice = kAudioDeviceUnknown;
    AudioObjectPropertyAddress propertyAddress;

    propertyAddress.mSelector = kAudioHardwarePropertyDevices;
    propertyAddress.mScope = kAudioObjectPropertyScopeGlobal;
    propertyAddress.mElement = kAudioObjectPropertyElementMaster;

    // scprintf("SC_CoreAudioDriver::Setup world %p\n", mWorld);

    ////////////////////////////////////////////////////////////////////////////////////////////////

    do {
        // err = AudioHardwareGetPropertyInfo( kAudioHardwarePropertyDevices, &count, 0);

        propertyAddress.mSelector = kAudioHardwarePropertyDevices;

        err = AudioObjectGetPropertyDataSize(kAudioObjectSystemObject, &propertyAddress, 0, NULL, &count);

        if (err != noErr) {
            scprintf("get kAudioHardwarePropertyDevices error %4.4s\n", (char*)&err);
            break;
        }

        AudioDeviceID* devices = (AudioDeviceID*)malloc(count);

        // err = AudioHardwareGetProperty(kAudioHardwarePropertyDevices, &count, devices);

        err = AudioObjectGetPropertyData(kAudioObjectSystemObject, &propertyAddress, 0, NULL, &count, devices);

        if (err != kAudioHardwareNoError) {
            scprintf("get kAudioHardwarePropertyDevices error %4.4s\n", (char*)&err);
            free(devices);
            break;
        }

        int numdevices = count / sizeof(AudioDeviceID);
        if (mWorld->mVerbosity >= 0) {
            scprintf("Number of Devices: %d\n", numdevices);
        }
        for (int i = 0; i < numdevices; ++i) {
            propertyAddress.mSelector = kAudioDevicePropertyDeviceName;

            // err = AudioDeviceGetPropertyInfo(devices[i], 0, false, kAudioDevicePropertyDeviceName, &count, 0);

            err = AudioObjectGetPropertyDataSize(devices[i], &propertyAddress, 0, NULL, &count);

            if (err != kAudioHardwareNoError) {
                scprintf("info kAudioDevicePropertyDeviceName error %4.4s A %d %p\n", (char*)&err, i, devices[i]);
                break;
            }

            char* name = (char*)malloc(count);
            // err = AudioDeviceGetProperty(devices[i], 0, false, kAudioDevicePropertyDeviceName, &count, name);

            err = AudioObjectGetPropertyData(devices[i], &propertyAddress, 0, NULL, &count, name);

            if (err != kAudioHardwareNoError) {
                scprintf("get kAudioDevicePropertyDeviceName error %4.4s A %d %p\n", (char*)&err, i, devices[i]);
                free(name);
                break;
            }
            if (mWorld->mVerbosity >= 0) {
                scprintf("   %d : \"%s\"\n", i, name);
            }
            free(name);
        }
        free(devices);
        if (mWorld->mVerbosity >= 0) {
            scprintf("\n");
        }
    } while (false);

    if (mWorld->hw->mInDeviceName || mWorld->hw->mOutDeviceName) {
        propertyAddress.mSelector = kAudioHardwarePropertyDevices;

        // err = AudioHardwareGetPropertyInfo(kAudioHardwarePropertyDevices, &count, 0);

        err = AudioObjectGetPropertyDataSize(kAudioObjectSystemObject, &propertyAddress, 0, NULL, &count);

        if (err != kAudioHardwareNoError) {
            scprintf("info kAudioHardwarePropertyDevices error %4.4s\n", (char*)&err);
            return false;
        }

        AudioDeviceID* devices = (AudioDeviceID*)malloc(count);

        // err = AudioHardwareGetProperty(kAudioHardwarePropertyDevices, &count, devices);
        err = AudioObjectGetPropertyData(kAudioObjectSystemObject, &propertyAddress, 0, NULL, &count, devices);

        if (err != kAudioHardwareNoError) {
            scprintf("get kAudioHardwarePropertyDevices error %4.4s\n", (char*)&err);
            return false;
        }

        int numdevices = count / sizeof(AudioDeviceID);
        for (int i = 0; i < numdevices; ++i) {
            // err = AudioDeviceGetPropertyInfo(devices[i], 0, false, kAudioDevicePropertyDeviceName, &count, 0);
            propertyAddress.mSelector = kAudioDevicePropertyDeviceName;
            err = AudioObjectGetPropertyDataSize(devices[i], &propertyAddress, 0, NULL, &count);

            if (err != kAudioHardwareNoError) {
                scprintf("info kAudioDevicePropertyDeviceName error %4.4s B %d %p\n", (char*)&err, i, devices[i]);
                break;
            }

            char* name = (char*)malloc(count);

            // err = AudioDeviceGetProperty(devices[i], 0, false, kAudioDevicePropertyDeviceName, &count, name);

            err = AudioObjectGetPropertyData(devices[i], &propertyAddress, 0, NULL, &count, name);

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
            if (mInputDevice != kAudioDeviceUnknown && mOutputDevice != kAudioDeviceUnknown)
                break;
        }
        free(devices);
        if (mOutputDevice == kAudioDeviceUnknown || mInputDevice == kAudioDeviceUnknown)
            goto getDefault;
    } else {
    getDefault:

        // get the default output device for the HAL
        if (mOutputDevice == kAudioDeviceUnknown) {
            count = sizeof(mOutputDevice);
            // get the output device:
            //			err = AudioHardwareGetProperty(kAudioHardwarePropertyDefaultOutputDevice, &count, (void *) &
            // mOutputDevice);

            propertyAddress.mSelector = kAudioHardwarePropertyDefaultOutputDevice;

            err = AudioObjectGetPropertyData(kAudioObjectSystemObject, &propertyAddress, 0, NULL, &count,
                                             (void*)&mOutputDevice);

            if (err != kAudioHardwareNoError) {
                scprintf("get kAudioHardwarePropertyDefaultOutputDevice error %4.4s\n", (char*)&err);
                return false;
            }
        }

        // get the input device
        if (mInputDevice == kAudioDeviceUnknown) {
            count = sizeof(mInputDevice);
            //			err = AudioHardwareGetProperty(kAudioHardwarePropertyDefaultInputDevice, &count, (void *) &
            // mInputDevice);

            propertyAddress.mSelector = kAudioHardwarePropertyDefaultInputDevice;

            err = AudioObjectGetPropertyData(kAudioObjectSystemObject, &propertyAddress, 0, NULL, &count,
                                             (void*)&mInputDevice);


            // get the input device:
            if (err != kAudioHardwareNoError) {
                scprintf("get kAudioHardwarePropertyDefaultInputDevice error %4.4s\n", (char*)&err);
                return false;
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////

    AudioTimeStamp now;
    now.mFlags = kAudioTimeStampHostTimeValid;
    now.mHostTime = AudioGetCurrentHostTime();
    if (mPreferredHardwareBufferFrameSize) {
        count = sizeof(UInt32);
        //		err = AudioDeviceSetProperty(mOutputDevice, &now, 0, false, kAudioDevicePropertyBufferFrameSize, count,
        //&mPreferredHardwareBufferFrameSize);

        propertyAddress.mSelector = kAudioDevicePropertyBufferFrameSize;
        propertyAddress.mScope = kAudioDevicePropertyScopeOutput;

        err = AudioObjectSetPropertyData(mOutputDevice, &propertyAddress, 0, NULL, count,
                                         &mPreferredHardwareBufferFrameSize);

        if (err != kAudioHardwareNoError) {
            scprintf("set kAudioDevicePropertyBufferFrameSize error %4.4s\n", (char*)&err);
            // return false;
        }
        if (UseSeparateIO()) {
            count = sizeof(UInt32);
            // err = AudioDeviceSetProperty(mOutputDevice, &now, 0, false, kAudioDevicePropertyBufferFrameSize, count,
            // &mPreferredHardwareBufferFrameSize);

            propertyAddress.mSelector = kAudioDevicePropertyBufferFrameSize;
            propertyAddress.mScope = kAudioDevicePropertyScopeOutput;

            err = AudioObjectSetPropertyData(mInputDevice, &propertyAddress, 0, NULL, count,
                                             &mPreferredHardwareBufferFrameSize);

            if (err != kAudioHardwareNoError) {
                scprintf("set kAudioDevicePropertyNominalSampleRate error %4.4s\n", (char*)&err);
                // return false;
            }
        }
    }
    if (mPreferredSampleRate) {
        Float64 sampleRate = mPreferredSampleRate;
        count = sizeof(Float64);

        bool sampleRateSupported = false;
        auto availableSampleRates = GetAvailableNominalSampleRates(mOutputDevice);

        // If we've got two devices, we need to use a sample rate list from both
        //  This won't account for cases where there are overlapping ranges, but
        //  that seems sufficiently edge-case to ignore for now.
        if (UseSeparateIO()) {
            auto inputSampleRates = GetAvailableNominalSampleRates(mInputDevice);

            std::vector<AudioValueRange> availableSampleRatesInputOutput(
                std::min(availableSampleRates.size(), inputSampleRates.size()));
            auto rateListIter =
                std::set_union(availableSampleRates.begin(), availableSampleRates.end(), inputSampleRates.begin(),
                               inputSampleRates.end(), availableSampleRatesInputOutput.begin(),
                               [](const AudioValueRange& lhs, const AudioValueRange& rhs) {
                                   return (lhs.mMaximum != rhs.mMaximum) ? (lhs.mMaximum < rhs.mMaximum)
                                                                         : (lhs.mMinimum < rhs.mMinimum);
                               });

            availableSampleRatesInputOutput.resize(rateListIter - availableSampleRatesInputOutput.begin());
            availableSampleRates = availableSampleRatesInputOutput;
        }

        for (const AudioValueRange& range : availableSampleRates) {
            if (mPreferredSampleRate >= range.mMinimum && mPreferredSampleRate <= range.mMaximum) {
                sampleRateSupported = true;
                break;
            }
        }

        if (!sampleRateSupported) {
#    define SR_MAX_VALUE 9999999
#    define SR_MIN_VALUE -9999999

            Float64 nextHighestMatch = SR_MAX_VALUE;
            Float64 nextLowestMatch = SR_MIN_VALUE;

            for (const AudioValueRange& range : availableSampleRates) {
                if (range.mMinimum > mPreferredSampleRate && range.mMinimum < nextHighestMatch) {
                    nextHighestMatch = range.mMinimum;
                }
                if (range.mMaximum < mPreferredSampleRate && range.mMaximum > nextLowestMatch) {
                    nextLowestMatch = range.mMaximum;
                }
            }

            if (nextHighestMatch != SR_MAX_VALUE) {
                sampleRate = nextHighestMatch;
                sampleRateSupported = true;
            } else if (nextLowestMatch != SR_MIN_VALUE) {
                sampleRate = nextLowestMatch;
                sampleRateSupported = true;
            } else {
                sampleRateSupported = false;
            }

            if (sampleRateSupported) {
                scprintf("Requested sample rate %f was not available - attempting to use sample rate of %f\n",
                         (double)mPreferredSampleRate, (double)sampleRate);
            } else {
                scprintf("Could not set requested sample rate of %f\n", (double)mPreferredSampleRate);
                scprintf("Available sample rates:\n");
                for (const AudioValueRange& range : availableSampleRates) {
                    if (range.mMaximum == range.mMinimum) {
                        scprintf("\t%f\n", range.mMaximum);
                    } else {
                        scprintf("\t%f - %f\n", range.mMinimum, range.mMaximum);
                    }
                }
            }
        }

        // Requested sample rate is available - we're going to set and, if we don't hit an error,
        // assume that our set was successful.
        if (sampleRateSupported) {
            bool sampleRateSetSuccess = true;

            propertyAddress.mSelector = kAudioDevicePropertyNominalSampleRate;
            propertyAddress.mScope = kAudioDevicePropertyScopeOutput;

            err = AudioObjectSetPropertyData(mOutputDevice, &propertyAddress, 0, NULL, count, &sampleRate);

            if (err != kAudioHardwareNoError) {
                sampleRateSetSuccess = false;
                scprintf("set kAudioDevicePropertyNominalSampleRate error %4.4s\n", (char*)&err);
            }

            if (UseSeparateIO()) {
                count = sizeof(Float64);

                propertyAddress.mSelector = kAudioDevicePropertyNominalSampleRate;
                propertyAddress.mScope = kAudioDevicePropertyScopeInput;

                err = AudioObjectSetPropertyData(mInputDevice, &propertyAddress, 0, NULL, count, &sampleRate);

                if (err != kAudioHardwareNoError) {
                    sampleRateSetSuccess = false;
                    scprintf("set kAudioDevicePropertyNominalSampleRate error %4.4s\n", (char*)&err);
                }
            }

            if (sampleRateSetSuccess) {
                mExplicitSampleRate = sampleRate;
            } else {
                return false;
            }
        }
    }

    // get the buffersize for the out device
    count = sizeof(mHardwareBufferSize);
    // err = AudioDeviceGetProperty(mOutputDevice, 0, false, kAudioDevicePropertyBufferSize, &count,
    // &mHardwareBufferSize);
    propertyAddress.mSelector = kAudioDevicePropertyBufferSize;
    propertyAddress.mScope = kAudioDevicePropertyScopeOutput;

    err = AudioObjectGetPropertyData(mOutputDevice, &propertyAddress, 0, NULL, &count, &mHardwareBufferSize);

    if (err != kAudioHardwareNoError) {
        scprintf("get kAudioDevicePropertyBufferSize error %4.4s\n", (char*)&err);
        return false;
    }
    // scprintf("mHardwareBufferSize = %ld\n", mHardwareBufferSize);

    // get a description of the data format used by the output device
    count = sizeof(AudioStreamBasicDescription);
    // err = AudioDeviceGetProperty(mOutputDevice, 0, false, kAudioDevicePropertyStreamFormat, &count,
    // &outputStreamDesc);
    propertyAddress.mSelector = kAudioDevicePropertyStreamFormat;
    propertyAddress.mScope = kAudioDevicePropertyScopeOutput;

    err = AudioObjectGetPropertyData(mOutputDevice, &propertyAddress, 0, NULL, &count, &outputStreamDesc);

    if (err != kAudioHardwareNoError) {
        scprintf("get kAudioDevicePropertyStreamFormat error %4.4s\n", (char*)&err);
        return false;
    }

    if (mInputDevice != kAudioDeviceUnknown) {
        // get a description of the data format used by the input device
        count = sizeof(AudioStreamBasicDescription);
        // err = AudioDeviceGetProperty(mInputDevice, 0, true, kAudioDevicePropertyStreamFormat, &count,
        // &inputStreamDesc);
        propertyAddress.mSelector = kAudioDevicePropertyStreamFormat;
        propertyAddress.mScope = kAudioDevicePropertyScopeInput;

        err = AudioObjectGetPropertyData(mInputDevice, &propertyAddress, 0, NULL, &count, &inputStreamDesc);

        if (err != kAudioHardwareNoError) {
            scprintf("get kAudioDevicePropertyStreamFormat error on input %4.4s\n", (char*)&err);
            return false;
        }

        if (
            // We do not support mismatched input and output sample rates, but there's no harm in skipping
            // this check if numInputBusChannels == 0. This allows the user to disable input entirely as
            // a workaround for a sample rate mismatch.
            mWorld->mNumInputs > 0 && !mExplicitSampleRate
            && inputStreamDesc.mSampleRate != outputStreamDesc.mSampleRate) {
            scprintf("ERROR: Input sample rate is %g, but output is %g. "
                     "Mismatched sample rates are not supported. "
                     "To disable input, set the number of input channels to 0.\n",
                     inputStreamDesc.mSampleRate, outputStreamDesc.mSampleRate);
            return false;
        }
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////

    do {
        // err = AudioDeviceGetPropertyInfo(mInputDevice, 0, false, kAudioDevicePropertyDeviceName, &count, 0);
        propertyAddress.mSelector = kAudioDevicePropertyDeviceName;
        propertyAddress.mScope = kAudioDevicePropertyScopeInput;

        err = AudioObjectGetPropertyDataSize(mInputDevice, &propertyAddress, 0, NULL, &count);

        if (err != kAudioHardwareNoError) {
            scprintf("info kAudioDevicePropertyDeviceName error %4.4s C %p\n", (char*)&err, mInputDevice);
            break;
        }

        char* name = (char*)malloc(count);
        // err = AudioDeviceGetProperty(mInputDevice, 0, false, kAudioDevicePropertyDeviceName, &count, name);

        err = AudioObjectGetPropertyData(mInputDevice, &propertyAddress, 0, NULL, &count, name);

        if (err != kAudioHardwareNoError) {
            scprintf("get kAudioDevicePropertyDeviceName error %4.4s C %p\n", (char*)&err, mInputDevice);
            free(name);
            break;
        }

        if (mWorld->mVerbosity >= 0) {
            scprintf("\"%s\" Input Device\n", name);
        }
        free(name);

        Boolean writeable;

        // err = AudioDeviceGetPropertyInfo(mInputDevice, 0, 1, kAudioDevicePropertyStreamConfiguration, &count,
        // &writeable);
        propertyAddress.mSelector = kAudioDevicePropertyStreamConfiguration;
        propertyAddress.mScope = kAudioDevicePropertyScopeInput;

        err = AudioObjectIsPropertySettable(mInputDevice, &propertyAddress, &writeable);

        if (err != kAudioHardwareNoError) {
            scprintf("info kAudioDevicePropertyStreamConfiguration settable error %4.4s\n", (char*)&err);
            break;
        }

        err = AudioObjectGetPropertyDataSize(mInputDevice, &propertyAddress, 0, NULL, &count);

        if (err != kAudioHardwareNoError) {
            scprintf("info kAudioDevicePropertyStreamConfiguration size error %4.4s\n", (char*)&err);
            break;
        }

        AudioBufferList* bufList = (AudioBufferList*)malloc(count);
        // err = AudioDeviceGetProperty(mInputDevice, 0, 1, kAudioDevicePropertyStreamConfiguration, &count, bufList);
        propertyAddress.mSelector = kAudioDevicePropertyStreamConfiguration;
        propertyAddress.mScope = kAudioDevicePropertyScopeInput;

        err = AudioObjectGetPropertyData(mInputDevice, &propertyAddress, 0, NULL, &count, bufList);

        if (err != kAudioHardwareNoError) {
            scprintf("get kAudioDevicePropertyStreamConfiguration error %4.4s\n", (char*)&err);
            free(bufList);
            break;
        }

        if (mWorld->mVerbosity >= 0) {
            scprintf("   Streams: %d\n", bufList->mNumberBuffers);
            for (unsigned int j = 0; j < bufList->mNumberBuffers; ++j) {
                scprintf("      %d  channels %d\n", j, bufList->mBuffers[j].mNumberChannels);
            }
        }

        free(bufList);
    } while (false);
    if (mWorld->mVerbosity >= 0) {
        scprintf("\n");
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////

    do {
        // err = AudioDeviceGetPropertyInfo(mOutputDevice, 0, false, kAudioDevicePropertyDeviceName, &count, 0);
        propertyAddress.mSelector = kAudioDevicePropertyDeviceName;
        propertyAddress.mScope = kAudioDevicePropertyScopeOutput;

        err = AudioObjectGetPropertyDataSize(mOutputDevice, &propertyAddress, 0, NULL, &count);

        char* name = (char*)malloc(count);
        // err = AudioDeviceGetProperty(mOutputDevice, 0, false, kAudioDevicePropertyDeviceName, &count, name);
        err = AudioObjectGetPropertyData(mOutputDevice, &propertyAddress, 0, NULL, &count, name);
        if (err != kAudioHardwareNoError) {
            scprintf("get kAudioDevicePropertyDeviceName error %4.4s\n", (char*)&err);
            free(name);
            break;
        }

        if (mWorld->mVerbosity >= 0) {
            scprintf("\"%s\" Output Device\n", name);
        }
        free(name);

        Boolean writeable;
        // err = AudioDeviceGetPropertyInfo(mOutputDevice, 0, 0, kAudioDevicePropertyStreamConfiguration, &count,
        // &writeable);

        propertyAddress.mSelector = kAudioDevicePropertyStreamConfiguration;
        propertyAddress.mScope = kAudioDevicePropertyScopeOutput;

        err = AudioObjectIsPropertySettable(mOutputDevice, &propertyAddress, &writeable);

        if (err != kAudioHardwareNoError) {
            scprintf("info kAudioDevicePropertyStreamConfiguration settable error %4.4s\n", (char*)&err);
            break;
        }

        err = AudioObjectGetPropertyDataSize(mOutputDevice, &propertyAddress, 0, NULL, &count);

        if (err != kAudioHardwareNoError) {
            scprintf("info kAudioDevicePropertyStreamConfiguration size error %4.4s\n", (char*)&err);
            break;
        }

        AudioBufferList* bufList = (AudioBufferList*)malloc(count);
        // err = AudioDeviceGetProperty(mOutputDevice, 0, 0, kAudioDevicePropertyStreamConfiguration, &count, bufList);

        propertyAddress.mSelector = kAudioDevicePropertyStreamConfiguration;
        propertyAddress.mScope = kAudioDevicePropertyScopeOutput;

        err = AudioObjectGetPropertyData(mOutputDevice, &propertyAddress, 0, NULL, &count, bufList);
        if (err != kAudioHardwareNoError) {
            scprintf("get kAudioDevicePropertyStreamConfiguration error %4.4s\n", (char*)&err);
            free(bufList);
            break;
        }
        if (mWorld->mVerbosity >= 0) {
            scprintf("   Streams: %d\n", bufList->mNumberBuffers);
            for (unsigned int j = 0; j < bufList->mNumberBuffers; ++j) {
                scprintf("      %d  channels %d\n", j, bufList->mBuffers[j].mNumberChannels);
            }
        }
        free(bufList);
    } while (false);
    if (mWorld->mVerbosity >= 0) {
        scprintf("\n");
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////


    if (UseSeparateIO()) {
        count = sizeof(UInt32);
        // err = AudioDeviceGetProperty(mInputDevice, 0, true, kAudioDevicePropertySafetyOffset, &count,
        // &mSafetyOffset);
        propertyAddress.mSelector = kAudioDevicePropertySafetyOffset;
        propertyAddress.mScope = kAudioDevicePropertyScopeInput;

        err = AudioObjectGetPropertyData(mInputDevice, &propertyAddress, 0, NULL, &count, &mSafetyOffset);

        if (err != kAudioHardwareNoError) {
            scprintf("get kAudioDevicePropertySafetyOffset error %4.4s\n", (char*)&err);
            return false;
        }
        if (mWorld->mVerbosity >= 1) {
            scprintf("mSafetyOffset %lu\n", mSafetyOffset);
        }

        Boolean writeable;
        //		err = AudioDeviceGetPropertyInfo(mInputDevice, 0, true, kAudioDevicePropertyStreamConfiguration, &count,
        //&writeable);

        propertyAddress.mSelector = kAudioDevicePropertyStreamConfiguration;
        propertyAddress.mScope = kAudioDevicePropertyScopeInput;

        err = AudioObjectIsPropertySettable(mInputDevice, &propertyAddress, &writeable);

        if (err != kAudioHardwareNoError) {
            scprintf("get kAudioDevicePropertyStreamConfiguration writeable error %4.4s\n", (char*)&err);
            return false;
        }

        err = AudioObjectGetPropertyDataSize(mInputDevice, &propertyAddress, 0, NULL, &count);

        if (err != kAudioHardwareNoError) {
            scprintf("get kAudioDevicePropertyStreamConfiguration size error %4.4s\n", (char*)&err);
            return false;
        }

        mInputBufList = (AudioBufferList*)malloc(count);

        // err = AudioDeviceGetProperty(mInputDevice, 0, true, kAudioDevicePropertyStreamConfiguration, &count,
        // mInputBufList);

        err = AudioObjectGetPropertyData(mInputDevice, &propertyAddress, 0, NULL, &count, mInputBufList);

        if (err != kAudioHardwareNoError) {
            scprintf("get kAudioDevicePropertyStreamConfiguration error %4.4s\n", (char*)&err);
            return false;
        }

        if (mWorld->mVerbosity >= 1) {
            scprintf("mNumberBuffers %lu\n", mInputBufList->mNumberBuffers);
        }
        for (uint32 i = 0; i < mInputBufList->mNumberBuffers; ++i) {
            if (mWorld->mVerbosity >= 1) {
                scprintf("  mDataByteSize %d %lu\n", i, mInputBufList->mBuffers[i].mDataByteSize);
            }
            mInputBufList->mBuffers[i].mData = zalloc(1, mInputBufList->mBuffers[i].mDataByteSize);
        }

        /*
        AudioTimeStamp	now;
        now.mFlags = kAudioTimeStampHostTimeValid;
        now.mHostTime = AudioGetCurrentHostTime();


        err = AudioDeviceSetProperty(mInputDevice, &now, 0, true, kAudioDevicePropertyRegisterBufferList, count,
        mInputBufList); if (err != kAudioHardwareNoError) { scprintf("get kAudioDevicePropertyRegisterBufferList error
        %4.4s\n", (char*)&err); return false;
        }
        */
    }

    *outNumSamplesPerCallback = mHardwareBufferSize / outputStreamDesc.mBytesPerFrame;
    if (mExplicitSampleRate) {
        *outSampleRate = mExplicitSampleRate.get();
    } else {
        *outSampleRate = outputStreamDesc.mSampleRate;
    }

    if (mWorld->mVerbosity >= 1) {
        scprintf("<-SC_CoreAudioDriver::Setup world %p\n", mWorld);
    }


    // check if using built-in output, and thus whether there could be headphone plug/un-plug issues
    // our assumption otherwise is that we don't respond, and SC will stay with the pre-arranged or default device, and
    // not restart just because headphones switched

    // err = AudioDeviceGetPropertyInfo(mOutputDevice, 0, false, kAudioDevicePropertyDeviceName, &count, 0);

    propertyAddress.mSelector = kAudioDevicePropertyDeviceName;
    propertyAddress.mScope = kAudioDevicePropertyScopeOutput;

    err = AudioObjectGetPropertyDataSize(mOutputDevice, &propertyAddress, 0, NULL, &count);

    if (err != kAudioHardwareNoError) {
        scprintf("info kAudioDevicePropertyDeviceName error %4.4s %p\n", (char*)&err, mOutputDevice);
        return false;
    }

    char* outputname = (char*)malloc(count);
    const char* testname = "Built-in Output";

    // err = AudioDeviceGetProperty(mOutputDevice, 0, false, kAudioDevicePropertyDeviceName, &count, outputname);
    err = AudioObjectGetPropertyData(mOutputDevice, &propertyAddress, 0, NULL, &count, outputname);

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


OSStatus appIOProcSeparateIn(AudioDeviceID device, const AudioTimeStamp* inNow, const AudioBufferList* inInputData,
                             const AudioTimeStamp* inInputTime, AudioBufferList* outOutputData,
                             const AudioTimeStamp* inOutputTime, void* defptr) {
    SC_CoreAudioDriver* def = (SC_CoreAudioDriver*)defptr;

    // copy input data to driver's private buffer list
    int i;
    for (i = 0; i < inInputData->mNumberBuffers; i++) {
        memcpy(def->mInputBufList->mBuffers[i].mData, inInputData->mBuffers[i].mData,
               inInputData->mBuffers[i].mDataByteSize);
    }

    return kAudioHardwareNoError;
}

OSStatus appIOProc(AudioDeviceID device, const AudioTimeStamp* inNow, const AudioBufferList* inInputData,
                   const AudioTimeStamp* inInputTime, AudioBufferList* outOutputData,
                   const AudioTimeStamp* inOutputTime, void* defptr) {
    SC_CoreAudioDriver* def = (SC_CoreAudioDriver*)defptr;
    int64 oscTime = CoreAudioHostTimeToOSC(inOutputTime->mHostTime);

    double hostSecs = (double)AudioConvertHostTimeToNanos(inOutputTime->mHostTime) * 1e-9;
    double sampleTime = inOutputTime->mSampleTime;
    if (def->mStartHostSecs == 0) {
        def->mStartHostSecs = hostSecs;
        def->mStartSampleTime = sampleTime;
    } else {
        double instSampleRate = (sampleTime - def->mPrevSampleTime) / (hostSecs - def->mPrevHostSecs);
        double smoothSampleRate = def->mSmoothSampleRate;
        smoothSampleRate = smoothSampleRate + 0.002 * (instSampleRate - smoothSampleRate);
        def->mOSCincrement = (int64)(def->mOSCincrementNumerator / smoothSampleRate);
        def->mSmoothSampleRate = smoothSampleRate;

#    if 0
		double avgSampleRate  = (sampleTime - def->mStartSampleTime)/(hostSecs - def->mStartHostSecs);
		double jitter = (smoothSampleRate * (hostSecs - def->mPrevHostSecs)) - (sampleTime - def->mPrevSampleTime);
		double drift = (smoothSampleRate - def->mSampleRate) * (hostSecs - def->mStartHostSecs);
		//if (fabs(jitter) > 0.01) {
			scprintf("avgSR %.6f   smoothSR %.6f   instSR %.6f   jitter %.6f   drift %.6f   inc %lld\n",
				avgSampleRate, smoothSampleRate, instSampleRate, jitter, drift, def->mOSCincrement);
		//}
#    endif
    }
    def->mPrevHostSecs = hostSecs;
    def->mPrevSampleTime = sampleTime;

    if (!def->UseSeparateIO()) {
        def->Run(inInputData, outOutputData, oscTime);
        return kAudioHardwareNoError;
    }


    def->Run(def->mInputBufList, outOutputData, oscTime);
    return kAudioHardwareNoError;
}

void SC_CoreAudioDriver::Run(const AudioBufferList* inInputData, AudioBufferList* outOutputData, int64 oscTime) {
    int64 systemTimeBefore = AudioGetCurrentHostTime();
    World* world = mWorld;

    try {
        int numSamplesPerCallback = NumSamplesPerCallback();
        mOSCbuftime = oscTime;

#    ifdef __APPLE__
        // FIXME: aren't denormal flags set per-process?
        sc_SetDenormalFlags();
#    endif

        mFromEngine.Free();
        /*if (mToEngine.HasData()) {
            scprintf("oscTime %.9f %.9f\n", oscTime*kOSCtoSecs,
        CoreAudioHostTimeToOSC(AudioGetCurrentHostTime())*kOSCtoSecs);
        }*/
        mToEngine.Perform();
        mOscPacketsToEngine.Perform();

        int bufFrames = world->mBufLength;
        int numBufs = numSamplesPerCallback / bufFrames;

        int numInputBuses = world->mNumInputs;
        int numOutputBuses = world->mNumOutputs;
        float* inputBuses = world->mAudioBus + world->mNumOutputs * bufFrames;
        float* outputBuses = world->mAudioBus;
        int32* inputTouched = world->mAudioBusTouched + world->mNumOutputs;
        int32* outputTouched = world->mAudioBusTouched;
        int numInputStreams = inInputData ? inInputData->mNumberBuffers : 0;
        int numOutputStreams = outOutputData ? outOutputData->mNumberBuffers : 0;

        // static int go = 0;

        int64 oscInc = mOSCincrement;
        double oscToSamples = mOSCtoSamples;

        int bufFramePos = 0;

        for (int i = 0; i < numBufs; ++i, world->mBufCounter++, bufFramePos += bufFrames) {
            int32 bufCounter = world->mBufCounter;

            // de-interleave input
            if (inInputData) {
                const AudioBuffer* inInputDataBuffers = inInputData->mBuffers;
                for (int s = 0, b = 0; b < numInputBuses && s < numInputStreams; s++) {
                    const AudioBuffer* buf = inInputDataBuffers + s;
                    int nchan = buf->mNumberChannels;
                    if (buf->mData) {
                        float* busdata = inputBuses + b * bufFrames;
                        float* bufdata = (float*)buf->mData + bufFramePos * nchan;
                        if (nchan == 1) {
                            for (int k = 0; k < bufFrames; ++k) {
                                busdata[k] = bufdata[k];
                            }
                            inputTouched[b] = bufCounter;
                        } else {
                            int minchan = sc_min(nchan, numInputBuses - b);
                            for (int j = 0; j < minchan; ++j, busdata += bufFrames) {
                                for (int k = 0, m = j; k < bufFrames; ++k, m += nchan) {
                                    busdata[k] = bufdata[m];
                                }
                                inputTouched[b + j] = bufCounter;
                            }
                        }
                        b += nchan;
                    }
                }
            }
            // count++;

            int64 schedTime;
            int64 nextTime = oscTime + oscInc;

            /*if (mScheduler.Ready(nextTime)) {
                double diff = (mScheduler.NextTime() - mOSCbuftime)*kOSCtoSecs;
                scprintf("rdy %.9f %.9f %.9f\n", (mScheduler.NextTime()-gStartupOSCTime) * kOSCtoSecs,
            (mOSCbuftime-gStartupOSCTime)*kOSCtoSecs, diff);
            }*/

            while ((schedTime = mScheduler.NextTime()) <= nextTime) {
                float diffTime = (float)(schedTime - oscTime) * oscToSamples + 0.5;
                float diffTimeFloor = floor(diffTime);
                world->mSampleOffset = (int)diffTimeFloor;
                world->mSubsampleOffset = diffTime - diffTimeFloor;

                if (world->mSampleOffset < 0)
                    world->mSampleOffset = 0;
                else if (world->mSampleOffset >= world->mBufLength)
                    world->mSampleOffset = world->mBufLength - 1;

                SC_ScheduledEvent event = mScheduler.Remove();
                event.Perform();
            }
            world->mSampleOffset = 0;
            world->mSubsampleOffset = 0.f;

            World_Run(world);

            // interleave output
            AudioBuffer* outOutputDataBuffers = outOutputData->mBuffers;
            for (int s = 0, b = 0; b < numOutputBuses && s < numOutputStreams; s++) {
                AudioBuffer* buf = outOutputDataBuffers + s;
                int nchan = buf->mNumberChannels;
                if (buf->mData) {
                    float* busdata = outputBuses + b * bufFrames;
                    float* bufdata = (float*)buf->mData + bufFramePos * nchan;
                    if (nchan == 1) {
                        if (outputTouched[b] == bufCounter) {
                            for (int k = 0; k < bufFrames; ++k) {
                                bufdata[k] = busdata[k];
                            }
                        }
                    } else {
                        int minchan = sc_min(nchan, numOutputBuses - b);
                        for (int j = 0; j < minchan; ++j, busdata += bufFrames) {
                            if (outputTouched[b + j] == bufCounter) {
                                for (int k = 0, m = j; k < bufFrames; ++k, m += nchan) {
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
    if (cpuUsage > mPeakCPU || --mPeakCounter <= 0) {
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

OSStatus hardwareListenerProc(AudioHardwarePropertyID inPropertyID, void* inClientData) {
    OSStatus err = noErr;
    char cStr[255];
    UInt32 outSize;
    Boolean outWritable;
    AudioDeviceID deviceID;

    AudioObjectPropertyAddress propertyAddress;

    propertyAddress.mSelector = kAudioHardwarePropertyDevices;
    propertyAddress.mScope = kAudioObjectPropertyScopeGlobal;
    propertyAddress.mElement = kAudioObjectPropertyElementMaster;

    switch (inPropertyID) {
    case kAudioHardwarePropertyDefaultOutputDevice:
        scprintf("%s\n", "***** HARDWARE NOTIFICATION - kAudioHardwarePropertyDefaultOutputDevice\r");

        // err =  AudioHardwareGetPropertyInfo(kAudioHardwarePropertyDefaultOutputDevice,  &outSize, &outWritable);

        propertyAddress.mSelector = kAudioHardwarePropertyDefaultOutputDevice;

        err = AudioObjectGetPropertyDataSize(kAudioObjectSystemObject, &propertyAddress, 0, NULL, &outSize);

        if (err)
            break;

        err = AudioObjectIsPropertySettable(kAudioObjectSystemObject, &propertyAddress, &outWritable);

        if (err)
            break;

        // err = AudioHardwareGetProperty(kAudioHardwarePropertyDefaultOutputDevice, &outSize, &deviceID);

        err = AudioObjectGetPropertyData(kAudioObjectSystemObject, &propertyAddress, 0, NULL, &outSize, &deviceID);

        if (err)
            break;
        //			err =  AudioDeviceGetPropertyInfo(deviceID, 0, false, kAudioDevicePropertyDeviceName,  &outSize,
        //&outWritable);

        propertyAddress.mSelector = kAudioDevicePropertyDeviceName;

        err = AudioObjectGetPropertyDataSize(deviceID, &propertyAddress, 0, NULL, &outSize);

        if (err)
            break;

        err = AudioObjectIsPropertySettable(deviceID, &propertyAddress, &outWritable);

        if (err)
            break;

        // err = AudioDeviceGetProperty(deviceID, 0, false, kAudioDevicePropertyDeviceName, &outSize, cStr);

        err = AudioObjectGetPropertyData(deviceID, &propertyAddress, 0, NULL, &outSize, cStr);

        if (err)
            break;

        // do something

        break;

    case kAudioHardwarePropertyDefaultInputDevice:
        scprintf("%s\n", "***** HARDWARE NOTIFICATION - kAudioHardwarePropertyDefaultInputDevice\r");
        // err =  AudioHardwareGetPropertyInfo(kAudioHardwarePropertyDefaultInputDevice,  &outSize, &outWritable);

        propertyAddress.mSelector = kAudioHardwarePropertyDefaultInputDevice;

        err = AudioObjectGetPropertyDataSize(kAudioObjectSystemObject, &propertyAddress, 0, NULL, &outSize);

        if (err)
            break;

        err = AudioObjectIsPropertySettable(kAudioObjectSystemObject, &propertyAddress, &outWritable);

        if (err)
            break;

        // err = AudioHardwareGetProperty(kAudioHardwarePropertyDefaultInputDevice, &outSize, &deviceID);

        err = AudioObjectGetPropertyData(kAudioObjectSystemObject, &propertyAddress, 0, NULL, &outSize, &deviceID);

        if (err)
            break;

        // err =  AudioDeviceGetPropertyInfo(deviceID, 0, false, kAudioDevicePropertyDeviceName,  &outSize,
        // &outWritable);


        propertyAddress.mSelector = kAudioDevicePropertyDeviceName;

        err = AudioObjectGetPropertyDataSize(deviceID, &propertyAddress, 0, NULL, &outSize);

        if (err)
            break;

        err = AudioObjectIsPropertySettable(deviceID, &propertyAddress, &outWritable);

        if (err)
            break;

        // err = AudioDeviceGetProperty(deviceID, 0, false, kAudioDevicePropertyDeviceName, &outSize, cStr);


        err = AudioObjectGetPropertyData(deviceID, &propertyAddress, 0, NULL, &outSize, cStr);

        if (err)
            break;

        // do something

        break;

    case kAudioHardwarePropertyDefaultSystemOutputDevice:
        scprintf("%s\n", "***** HARDWARE NOTIFICATION - kAudioHardwarePropertyDefaultSystemOutputDevice\r");
        // err =  AudioHardwareGetPropertyInfo(kAudioHardwarePropertyDefaultSystemOutputDevice,  &outSize,
        // &outWritable);

        propertyAddress.mSelector = kAudioHardwarePropertyDefaultSystemOutputDevice;

        err = AudioObjectGetPropertyDataSize(kAudioObjectSystemObject, &propertyAddress, 0, NULL, &outSize);

        if (err)
            break;

        err = AudioObjectIsPropertySettable(kAudioObjectSystemObject, &propertyAddress, &outWritable);

        if (err)
            break;

        // err = AudioHardwareGetProperty(kAudioHardwarePropertyDefaultSystemOutputDevice, &outSize, &deviceID);

        err = AudioObjectGetPropertyData(kAudioObjectSystemObject, &propertyAddress, 0, NULL, &outSize, &deviceID);

        if (err)
            break;

        // err =  AudioDeviceGetPropertyInfo(deviceID, 0, false, kAudioDevicePropertyDeviceName,  &outSize,
        // &outWritable);

        propertyAddress.mSelector = kAudioDevicePropertyDeviceName;

        err = AudioObjectGetPropertyDataSize(deviceID, &propertyAddress, 0, NULL, &outSize);

        if (err)
            break;

        err = AudioObjectIsPropertySettable(deviceID, &propertyAddress, &outWritable);

        if (err)
            break;

        // err = AudioDeviceGetProperty(deviceID, 0, false, kAudioDevicePropertyDeviceName, &outSize, cStr);

        err = AudioObjectGetPropertyData(deviceID, &propertyAddress, 0, NULL, &outSize, cStr);

        if (err)
            break;

        // do something

        break;

    case kAudioHardwarePropertyDevices: {
        scprintf("%s\n", "***** HARDWARE NOTIFICATION - kAudioHardwarePropertyDevices\r");
    } break;
    default:
        scprintf("%s\n", "***** HARDWARE NOTIFICATION - %4.4s\r", &inPropertyID);
    }

    fflush(stdout);
    return (noErr);
}

OSStatus AddDeviceListeners(AudioDeviceID inDevice, void* inClientData);

bool SC_CoreAudioDriver::DriverStart() {
    if (mWorld->mVerbosity >= 1) {
        scprintf("->SC_CoreAudioDriver::DriverStart\n");
    }
    OSStatus err = kAudioHardwareNoError;
    // AudioTimeStamp	now;
    UInt32 propertySize;
    Boolean writable;
    AudioObjectPropertyAddress propertyAddress;

    propertyAddress.mSelector = kAudioHardwarePropertyDevices;
    propertyAddress.mScope = kAudioObjectPropertyScopeGlobal;
    propertyAddress.mElement = kAudioObjectPropertyElementMaster;

    if (mWorld->mVerbosity >= 1) {
        scprintf("start   UseSeparateIO?: %d\n", UseSeparateIO());
    }

    try {
        if (UseSeparateIO()) {
            //		err = AudioDeviceAddIOProc(mOutputDevice, appIOProc, (void *) this);	// setup Out device with an
            // IO proc

            err = AudioDeviceCreateIOProcID(mOutputDevice, appIOProc, (void*)this, &mOutputID);
            if (err != kAudioHardwareNoError) {
                scprintf("AudioDeviceAddIOProc failed %s %d\n", &err, (int)err);
                return false;
            }
            //		err = AudioDeviceAddIOProc(mInputDevice, appIOProcSeparateIn, (void *) this);		// setup In
            // device with an IO proc
            err = AudioDeviceCreateIOProcID(mInputDevice, appIOProcSeparateIn, (void*)this, &mInputID);

            if (err != kAudioHardwareNoError) {
                scprintf("AudioDeviceAddIOProc failed %s %d\n", &err, (int)err);
                return false;
            }


            if (mWorld->hw->mInputStreamsEnabled) {
                propertyAddress.mSelector = kAudioDevicePropertyIOProcStreamUsage;
                propertyAddress.mScope = kAudioDevicePropertyScopeInput;
                propertyAddress.mElement = 0;

                err = AudioObjectGetPropertyDataSize(mInputDevice, &propertyAddress, 0, NULL, &propertySize);
                err = AudioObjectIsPropertySettable(mInputDevice, &propertyAddress, &writable);

                AudioHardwareIOProcStreamUsage* su = (AudioHardwareIOProcStreamUsage*)malloc(propertySize);
                su->mIOProc = (void*)appIOProcSeparateIn;

                err = AudioObjectGetPropertyData(mInputDevice, &propertyAddress, 0, NULL, &propertySize, su);

                int len = std::min(su->mNumberStreams, (UInt32)strlen(mWorld->hw->mInputStreamsEnabled));

                for (int i = 0; i < len; ++i) {
                    su->mStreamIsOn[i] = mWorld->hw->mInputStreamsEnabled[i] == '1';
                }

                err = AudioObjectSetPropertyData(mInputDevice, &propertyAddress, 0, NULL, propertySize, su);
            }

            if (mWorld->hw->mOutputStreamsEnabled) {
                propertyAddress.mSelector = kAudioDevicePropertyIOProcStreamUsage;
                propertyAddress.mScope = kAudioDevicePropertyScopeOutput;
                propertyAddress.mElement = 0;

                err = AudioObjectGetPropertyDataSize(mOutputDevice, &propertyAddress, 0, NULL, &propertySize);
                err = AudioObjectIsPropertySettable(mOutputDevice, &propertyAddress, &writable);

                AudioHardwareIOProcStreamUsage* su = (AudioHardwareIOProcStreamUsage*)malloc(propertySize);
                su->mIOProc = (void*)appIOProc;

                err = AudioObjectGetPropertyData(mOutputDevice, &propertyAddress, 0, NULL, &propertySize, su);

                int len = std::min(su->mNumberStreams, (UInt32)strlen(mWorld->hw->mOutputStreamsEnabled));

                for (int i = 0; i < len; ++i) {
                    su->mStreamIsOn[i] = mWorld->hw->mOutputStreamsEnabled[i] == '1';
                }

                err = AudioObjectSetPropertyData(mOutputDevice, &propertyAddress, 0, NULL, propertySize, su);
            }

            err = AudioDeviceStart(mInputDevice, appIOProcSeparateIn); // start playing sound through the device
            if (err != kAudioHardwareNoError) {
                scprintf("AudioDeviceStart failed %d\n", (int)err);
                return false;
            }

            err = AudioDeviceStart(mOutputDevice, appIOProc); // start playing sound through the device
            if (err != kAudioHardwareNoError) {
                scprintf("AudioDeviceStart failed %d\n", (int)err);
                err = AudioDeviceStop(mInputDevice, appIOProcSeparateIn); // stop playing sound through the device
                return false;
            }
        } else {
            // err = AudioDeviceAddIOProc(mOutputDevice, appIOProc, (void *) this);	// setup our device with an IO proc
            err = AudioDeviceCreateIOProcID(mOutputDevice, appIOProc, (void*)this, &mOutputID);

            if (err != kAudioHardwareNoError) {
                scprintf("AudioDeviceAddIOProc failed %d\n", (int)err);
                return false;
            }

            if (mWorld->hw->mInputStreamsEnabled) {
                propertyAddress.mSelector = kAudioDevicePropertyIOProcStreamUsage;
                propertyAddress.mScope = kAudioDevicePropertyScopeOutput;
                propertyAddress.mElement = 0;

                err = AudioObjectGetPropertyDataSize(mOutputDevice, &propertyAddress, 0, NULL, &propertySize);
                err = AudioObjectIsPropertySettable(mOutputDevice, &propertyAddress, &writable);

                AudioHardwareIOProcStreamUsage* su = (AudioHardwareIOProcStreamUsage*)malloc(propertySize);
                su->mIOProc = (void*)appIOProc;

                err = AudioObjectGetPropertyData(mOutputDevice, &propertyAddress, 0, NULL, &propertySize, su);

                int len = std::min(su->mNumberStreams, (UInt32)strlen(mWorld->hw->mInputStreamsEnabled));

                for (int i = 0; i < len; ++i) {
                    su->mStreamIsOn[i] = mWorld->hw->mInputStreamsEnabled[i] == '1';
                }

                err = AudioObjectSetPropertyData(mOutputDevice, &propertyAddress, 0, NULL, propertySize, su);
            }

            if (mWorld->hw->mOutputStreamsEnabled) {
                propertyAddress.mSelector = kAudioDevicePropertyIOProcStreamUsage;
                propertyAddress.mScope = kAudioDevicePropertyScopeOutput;
                propertyAddress.mElement = 0;

                err = AudioObjectGetPropertyDataSize(mOutputDevice, &propertyAddress, 0, NULL, &propertySize);
                err = AudioObjectIsPropertySettable(mOutputDevice, &propertyAddress, &writable);

                AudioHardwareIOProcStreamUsage* su = (AudioHardwareIOProcStreamUsage*)malloc(propertySize);
                su->mIOProc = (void*)appIOProc;

                err = AudioObjectGetPropertyData(mOutputDevice, &propertyAddress, 0, NULL, &propertySize, su);

                int len = std::min(su->mNumberStreams, (UInt32)strlen(mWorld->hw->mOutputStreamsEnabled));

                for (int i = 0; i < len; ++i) {
                    su->mStreamIsOn[i] = mWorld->hw->mOutputStreamsEnabled[i] == '1';
                }

                err = AudioObjectSetPropertyData(mOutputDevice, &propertyAddress, 0, NULL, propertySize, su);
            }

            err = AudioDeviceStart(mOutputDevice, appIOProc); // start playing sound through the device
            if (err != kAudioHardwareNoError) {
                scprintf("AudioDeviceStart failed %d\n", (int)err);
                return false;
            }
        }
    } catch (...) {
        scprintf("exception in SC_CoreAudioDriver::DriverStart\n");
    }
    if (mWorld->mVerbosity >= 1) {
        scprintf("<-SC_CoreAudioDriver::DriverStart\n");
    }


    // http://lists.apple.com/archives/coreaudio-api/2010/Aug/msg00114.html
    CFRunLoopRef theRunLoop = NULL;
    AudioObjectPropertyAddress theAddress = { kAudioHardwarePropertyRunLoop, kAudioObjectPropertyScopeGlobal,
                                              kAudioObjectPropertyElementMaster };
    AudioObjectSetPropertyData(kAudioObjectSystemObject, &theAddress, 0, NULL, sizeof(CFRunLoopRef), &theRunLoop);

    // for now no spotting of hardware changes, assumption is that ServerOptions inviolate. However, if a device was
    // unplugged, could react to loss of that device by switching to system default? note that the number of listeners
    // is stripped down to only one for now, to react to headphone swaps in the case of Built-in Output
    AddDeviceListeners(mOutputDevice, this);

    return true;
}


bool SC_CoreAudioDriver::StopStart() {
    bool test = DriverStop();

    bool test2 = DriverStart();
    return test && test2;
}


bool SC_CoreAudioDriver::DriverStop() {
    if (mWorld->mVerbosity >= 1) {
        scprintf("->SC_CoreAudioDriver::DriverStop\n");
    }
    OSStatus err = kAudioHardwareNoError;

    if (UseSeparateIO()) {
        err = AudioDeviceStop(mOutputDevice, appIOProc);
        if (err != kAudioHardwareNoError) {
            scprintf("Output AudioDeviceStop failed %p\n", err);
            return false;
        }

        err = AudioDeviceDestroyIOProcID(mOutputDevice, mOutputID);
        if (err != kAudioHardwareNoError) {
            scprintf("Output AudioDeviceDestroyIOProcID failed %p\n", err);
            return false;
        }

        err = AudioDeviceStop(mInputDevice, appIOProcSeparateIn);
        if (err != kAudioHardwareNoError) {
            scprintf("Input AudioDeviceStop failed %p\n", err);
            return false;
        }

        err = AudioDeviceDestroyIOProcID(mInputDevice, mInputID);
        if (err != kAudioHardwareNoError) {
            scprintf("Input AudioDeviceDestroyIOProcID failed %p\n", err);
            return false;
        }
    } else {
        err = AudioDeviceStop(mOutputDevice, appIOProc);
        if (err != kAudioHardwareNoError) {
            scprintf("AudioDeviceStop B failed %p\n", err);
            return false;
        }

        err = AudioDeviceDestroyIOProcID(mOutputDevice, mOutputID);
        if (err != kAudioHardwareNoError) {
            scprintf("AudioDeviceDestroyIOProcID B failed %p\n", err);
            return false;
        }
    }
    if (mWorld->mVerbosity >= 1) {
        scprintf("<-SC_CoreAudioDriver::DriverStop\n");
    }
    return true;
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Listen for Device Properties and update interface and globals
OSStatus deviceListenerProc(AudioObjectID inObjectID, UInt32 inNumberAddresses,
                            const AudioObjectPropertyAddress inAddresses[], void* inClientData) {
    OSStatus err = noErr;

    SC_CoreAudioDriver* coredriver = (SC_CoreAudioDriver*)inClientData;

    for (int i = 0; i < inNumberAddresses; i++) {
        AudioObjectPropertyAddress propAddress = inAddresses[i];
        if (inObjectID == kAudioObjectSystemObject
            && propAddress.mSelector == kAudioHardwarePropertyDefaultOutputDevice) {
            coredriver->StopStart();
        }
    }

    return (err);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
OSStatus AddDeviceListeners(AudioDeviceID inDevice, void* inClientData) {
    OSStatus err = noErr;
    AudioObjectPropertyAddress propertyAddress;

    // ONLY REACTING TO HEADPHONE SWAPS FOR NOW - see version control history for removed dead code
    propertyAddress.mSelector = kAudioHardwarePropertyDevices;
    propertyAddress.mScope = kAudioObjectPropertyScopeGlobal;
    propertyAddress.mElement = kAudioObjectPropertyElementMaster;

    err = AudioObjectAddPropertyListener(inDevice, &propertyAddress, deviceListenerProc, inClientData);
    if (err)
        return err;

    return (err);
}

#endif // SC_AUDIO_API_COREAUDIO


// =====================================================================
// Audio driver (CoreAudioIPHONE)

#if SC_AUDIO_API == SC_AUDIO_API_COREAUDIOIPHONE
SC_iCoreAudioDriver::SC_iCoreAudioDriver(struct World* inWorld): SC_AudioDriver(inWorld) { receivedIn = 0; }

SC_iCoreAudioDriver::~SC_iCoreAudioDriver() {}

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


OSStatus InputCallback(void* inRefCon, AudioUnitRenderActionFlags* ioActionFlags, const AudioTimeStamp* inTimeStamp,
                       UInt32 inBusNumber, UInt32 inNumberFrames, AudioBufferList* ioData) {
    SC_iCoreAudioDriver* driver = (SC_iCoreAudioDriver*)inRefCon;

    if (driver->receivedIn) {
        // printf("exit input with no data \n");
        return noErr;
    }

    OSStatus ret =
        AudioUnitRender(driver->inputUnit, ioActionFlags, inTimeStamp, inBusNumber, inNumberFrames, driver->buflist);
    if (ret) {
        // printf("no input !\n");
        return noErr;
    }
    driver->receivedIn = 1;

    return noErr;
}

OSStatus RenderCallback(void* inRefCon, AudioUnitRenderActionFlags* ioActionFlags, const AudioTimeStamp* inTimeStamp,
                        UInt32 inBusNumber, UInt32 inNumberFrames, AudioBufferList* ioData) {
    SC_iCoreAudioDriver* driver = (SC_iCoreAudioDriver*)inRefCon;

#    ifndef SC_IPHONE
    if (!driver->receivedIn) {
        // printf("exit output with no data \n");
        return noErr;
    }
#    endif

    // float *fbuffer = (float *) driver->converter_buffer;

    int i;
    for (i = 0; i < inNumberFrames; i++) {
        signed short s = ((signed short*)driver->buflist->mBuffers[0].mData)[i];
        int k = s + 0x43c00000;
        float f = *((float*)&k);
        ((float*)driver->floatInputList->mBuffers[0].mData)[i] = f - 384.0f;
    }

    int64 oscTime = GetCurrentOSCTime();
    // driver->Run(driver->floatInputList, driver->floatOutputList, oscTime);
    driver->Run(driver->floatInputList, ioData, oscTime);

    for (i = 0; i < inNumberFrames; i++) {
        float f1 = ((float*)ioData->mBuffers[0].mData)[i];
        float f2 = ((float*)ioData->mBuffers[1].mData)[i];
        ((int*)ioData->mBuffers[0].mData)[i] = (int)(f1 * 16777216);
        ((int*)ioData->mBuffers[1].mData)[i] = (int)(f2 * 16777216);
    }


    /*
        unsigned long size = 2*1024*sizeof(unsigned long);
        OSStatus ret = AudioConverterConvertBuffer(driver->converter_in_to_F32,
       driver->buflist->mBuffers[0].mDataByteSize, driver->buflist->mBuffers[0].mData, &size, driver->converter_buffer);
        if (ret)
        {
            printf("couldn't convert !\n");
            return noErr;
        }

        int i;
        for (i=0; i<inNumberFrames; i++)
        {
            ((int *)ioData->mBuffers[0].mData)[i] = ((int *)ioData->mBuffers[1].mData)[i] =
       driver->converter_buffer[2*i];
        }
    */
    driver->receivedIn = 0;
    return noErr;
}


void SC_iCoreAudioDriver::Run(const AudioBufferList* inInputData, AudioBufferList* outOutputData, int64 oscTime) {
    int64 systemTimeBefore = GetMicroseconds();

    World* world = mWorld;

    try {
        int numSamplesPerCallback = NumSamplesPerCallback();
        mOSCbuftime = oscTime;

#    ifdef __APPLE__
        // FIXME: does iOS support x86?
        sc_SetDenormalFlags();
#    endif

        mFromEngine.Free();

        mToEngine.Perform();
        mOscPacketsToEngine.Perform();
        // printf("mOscPacketsToEngine : %d micros\n", (int) (GetMicroseconds()-systemTimeBefore));

        int bufFrames = world->mBufLength;
        int numBufs = numSamplesPerCallback / bufFrames;

        int numInputBuses = world->mNumInputs;
        int numOutputBuses = world->mNumOutputs;
        float* inputBuses = world->mAudioBus + world->mNumOutputs * bufFrames;
        float* outputBuses = world->mAudioBus;
        int32* inputTouched = world->mAudioBusTouched + world->mNumOutputs;
        int32* outputTouched = world->mAudioBusTouched;
        int numInputStreams = inInputData ? inInputData->mNumberBuffers : 0;
        int numOutputStreams = outOutputData ? outOutputData->mNumberBuffers : 0;

        // static int go = 0;

        int64 oscInc = mOSCincrement;
        double oscToSamples = mOSCtoSamples;

        int bufFramePos = 0;

        for (int i = 0; i < numBufs; ++i, world->mBufCounter++, bufFramePos += bufFrames) {
            int32 bufCounter = world->mBufCounter;

            // de-interleave input
            if (inInputData) {
                const AudioBuffer* inInputDataBuffers = inInputData->mBuffers;
                for (int s = 0, b = 0; b < numInputBuses && s < numInputStreams; s++) {
                    const AudioBuffer* buf = inInputDataBuffers + s;
                    int nchan = buf->mNumberChannels;
                    if (buf->mData) {
                        float* busdata = inputBuses + b * bufFrames;
                        float* bufdata = (float*)buf->mData + bufFramePos * nchan;
                        if (nchan == 1) {
#    ifdef IPHONE_VEC
                            vcopy(busdata, bufdata, bufFrames);
#    else
                            for (int k = 0; k < bufFrames; ++k) {
                                busdata[k] = bufdata[k];
                            }
#    endif
                            inputTouched[b] = bufCounter;
                        } else {
                            int minchan = sc_min(nchan, numInputBuses - b);
                            for (int j = 0; j < minchan; ++j, busdata += bufFrames) {
                                for (int k = 0, m = j; k < bufFrames; ++k, m += nchan) {
                                    busdata[k] = bufdata[m];
                                }
                                inputTouched[b + j] = bufCounter;
                            }
                        }
                        b += nchan;
                    }
                }
            }
            // count++;

            int64 schedTime;
            int64 nextTime = oscTime + oscInc;

            while ((schedTime = mScheduler.NextTime()) <= nextTime) {
                float diffTime = (float)(schedTime - oscTime) * oscToSamples + 0.5;
                float diffTimeFloor = floor(diffTime);
                world->mSampleOffset = (int)diffTimeFloor;
                world->mSubsampleOffset = diffTime - diffTimeFloor;

                if (world->mSampleOffset < 0)
                    world->mSampleOffset = 0;
                else if (world->mSampleOffset >= world->mBufLength)
                    world->mSampleOffset = world->mBufLength - 1;

                SC_ScheduledEvent event = mScheduler.Remove();
                event.Perform();
            }
            world->mSampleOffset = 0;
            world->mSubsampleOffset = 0.f;

            // int64 now = GetMicroseconds();

            World_Run(world);

            // printf("world run : %fms\n", (float) (GetMicroseconds()-now)/1000);


            // interleave output
            AudioBuffer* outOutputDataBuffers = outOutputData->mBuffers;
            for (int s = 0, b = 0; b < numOutputBuses && s < numOutputStreams; s++) {
                AudioBuffer* buf = outOutputDataBuffers + s;
                int nchan = buf->mNumberChannels;
                if (buf->mData) {
                    float* busdata = outputBuses + b * bufFrames;
                    float* bufdata = (float*)buf->mData + bufFramePos * nchan;
                    if (nchan == 1) {
                        if (outputTouched[b] == bufCounter) {
#    ifdef IPHONE_VEC
                            vcopy(bufdata, busdata, bufFrames);
#    else
                            for (int k = 0; k < bufFrames; ++k) {
                                bufdata[k] = busdata[k];
                            }
#    endif
                        }
                    } else {
                        int minchan = sc_min(nchan, numOutputBuses - b);
                        for (int j = 0; j < minchan; ++j, busdata += bufFrames) {
                            if (outputTouched[b + j] == bufCounter) {
                                for (int k = 0, m = j; k < bufFrames; ++k, m += nchan) {
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
    if (cpuUsage > mPeakCPU || --mPeakCounter <= 0) {
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


void AudioSessionInterruptionCbk(void* inClientData, UInt32 inInterruptionState) {}


bool SC_iCoreAudioDriver::DriverSetup(int* outNumSamplesPerCallback, double* outSampleRate) {
    AudioSessionInitialize(0, 0, AudioSessionInterruptionCbk, 0);
    unsigned long category = kAudioSessionCategory_PlayAndRecord;
#    ifdef SC_IPHONE
    UInt32 micInput, micInputSize = sizeof(&micInput);
    AudioSessionGetProperty(kAudioSessionProperty_AudioInputAvailable, &micInputSize, &micInput);
    if (!micInput) {
        category = kAudioSessionCategory_MediaPlayback;
        scprintf("SC_IPHONE: WARNING - no audio input available\n");
    }
#    endif
    AudioSessionSetProperty(kAudioSessionProperty_AudioCategory, sizeof(category), &category);

    if (mPreferredHardwareBufferFrameSize) {
        Float32 preferredBufferSize = (float)mPreferredHardwareBufferFrameSize / 44100.f;
        AudioSessionSetProperty(kAudioSessionProperty_PreferredHardwareIOBufferDuration, sizeof(preferredBufferSize),
                                &preferredBufferSize);
    }

    AudioSessionSetActive(true);

    float actualBufferDuration;
    UInt32 size = sizeof(actualBufferDuration);
    AudioSessionGetProperty(kAudioSessionProperty_CurrentHardwareIOBufferDuration, &size, &actualBufferDuration);

    *outNumSamplesPerCallback = (int)(actualBufferDuration * 44100.f + 0.5f);
    *outSampleRate = 44100;

    AudioComponentDescription desc;
    desc.componentType = kAudioUnitType_Output;
    desc.componentSubType = kAudioUnitSubType_RemoteIO;
    desc.componentManufacturer = kAudioUnitManufacturer_Apple;

    NewAUGraph(&graph);
    AUNode node;
    AudioUnit unit;
    OSStatus ret = AUGraphAddNode(graph, &desc, &node);
    // printf("node : %d\n", node);
    AUGraphOpen(graph);

    ret = AUGraphNodeInfo(graph, node, &desc, &unit);
    // printf("%d\n", unit);


    AudioComponent remoteIOComp = AudioComponentFindNext(0, &desc);
    if (AudioComponentInstanceNew(remoteIOComp, &inputUnit) != noErr) {
        // printf("error instantiating RemoteIO\n");
        return false;
    }
    // printf("instantiated : %d\n", inputUnit);

    int enableIO = 1;
    ret = AudioUnitSetProperty(inputUnit, kAudioOutputUnitProperty_EnableIO, kAudioUnitScope_Input, 1, &enableIO,
                               sizeof(enableIO));
    if (ret != noErr) {
        // printf("can't set input : %d\n", ret);
        return false;
    }
    enableIO = 0;
    ret = AudioUnitSetProperty(inputUnit, kAudioOutputUnitProperty_EnableIO, kAudioUnitScope_Output, 0, &enableIO,
                               sizeof(enableIO));
    if (ret != noErr) {
        // printf("can't set output : %d\n", ret);
        return false;
    }

    AudioUnitInitialize(inputUnit);

    unsigned long bufferSizeBytes = 1024 * sizeof(unsigned long);
    buflist = (AudioBufferList*)malloc(sizeof(AudioBufferList));
    buflist->mNumberBuffers = 1;
    buflist->mBuffers[0].mDataByteSize = bufferSizeBytes;
    buflist->mBuffers[0].mData = malloc(bufferSizeBytes);
    buflist->mBuffers[0].mNumberChannels = 1;

    floatInputList = (AudioBufferList*)malloc(sizeof(AudioBufferList));
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
    ret = AudioUnitSetProperty(inputUnit, kAudioOutputUnitProperty_SetInputCallback, kAudioUnitScope_Input, 0,
                               &inputStruct, sizeof(inputStruct));

    AURenderCallbackStruct renderStruct;
    renderStruct.inputProc = RenderCallback;
    renderStruct.inputProcRefCon = this;
    ret = AudioUnitSetProperty(unit, kAudioUnitProperty_SetRenderCallback, kAudioUnitScope_Input, 0, &renderStruct,
                               sizeof(renderStruct));

    AudioStreamBasicDescription streamFormat;
    streamFormat.mFormatID = kAudioFormatLinearPCM;
    streamFormat.mFormatFlags = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagsNativeEndian
        | kLinearPCMFormatFlagIsNonInterleaved | (24 << kLinearPCMFormatFlagsSampleFractionShift);
    streamFormat.mSampleRate = 44100;
    streamFormat.mBitsPerChannel = 32;
    streamFormat.mChannelsPerFrame = 2;
    streamFormat.mFramesPerPacket = 1;
    streamFormat.mBytesPerFrame = (streamFormat.mBitsPerChannel / 8);
    streamFormat.mBytesPerPacket = streamFormat.mBytesPerFrame * streamFormat.mFramesPerPacket;

    ret = AudioUnitSetProperty(unit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Input, 0, &streamFormat,
                               sizeof(streamFormat));


    AudioStreamBasicDescription audioFormat;
    audioFormat.mSampleRate = 44100.00;
    audioFormat.mFormatID = kAudioFormatLinearPCM;
    audioFormat.mFormatFlags = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked;
    audioFormat.mFramesPerPacket = 1;
    audioFormat.mChannelsPerFrame = 1;
    audioFormat.mBitsPerChannel = 16;
    audioFormat.mBytesPerPacket = 2;
    audioFormat.mBytesPerFrame = 2;
    ret = AudioUnitSetProperty(inputUnit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Output, 1, &audioFormat,
                               sizeof(audioFormat));


    /*
        AudioStreamBasicDescription d;
        size = sizeof(d);
        ret = AudioUnitGetProperty(unit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Input, 1, &d, &size);

        AudioStreamBasicDescription inputFormat;
        AudioStreamBasicDescription outputFormat;
        AudioStreamBasicDescription f32Format;

        size = sizeof(AudioStreamBasicDescription);
        ret = AudioUnitGetProperty(inputUnit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Input, 0, &inputFormat,
       &size); ret = AudioUnitGetProperty(inputUnit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Input, 1,
       &inputFormat, &size); ret = AudioUnitGetProperty(inputUnit, kAudioUnitProperty_StreamFormat,
       kAudioUnitScope_Output, 0, &outputFormat, &size); ret = AudioUnitGetProperty(inputUnit,
       kAudioUnitProperty_StreamFormat, kAudioUnitScope_Output, 1, &outputFormat, &size);

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

    if (mWorld->mVerbosity >= 0) {
        scprintf("<-SC_CoreAudioDriver::Setup world %p\n", mWorld);
    }
    return true;
}

bool SC_iCoreAudioDriver::DriverStart() {
    if (mWorld->mVerbosity >= 0) {
        scprintf("->SC_CoreAudioDriver::DriverStart\n");
    }

    try {
        OSStatus ret = AUGraphStart(graph);
        AudioOutputUnitStart(inputUnit);
    } catch (...) {
        scprintf("exception in SC_CoreAudioDriver::DriverStart\n");
    }
    if (mWorld->mVerbosity >= 0) {
        scprintf("<-SC_CoreAudioDriver::DriverStart\n");
    }
    return true;
}

bool SC_iCoreAudioDriver::DriverStop() {
    if (mWorld->mVerbosity >= 0) {
        scprintf("->SC_CoreAudioDriver::DriverStop\n");
    }

    AUGraphStop(graph);
    AudioOutputUnitStop(inputUnit);

    if (mWorld->mVerbosity >= 0) {
        scprintf("<-SC_CoreAudioDriver::DriverStop\n");
    }
    return true;
}

#endif // SC_AUDIO_API_COREAUDIOIPHONE
