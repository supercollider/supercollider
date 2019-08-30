/*
    SuperCollider audio driver for Android
    Copyright (c) 2010 Dan Stowell. All rights reserved.
    Incorporating code from
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
#include "SC_Prototypes.h"
#include "SC_HiddenWorld.h"
#include <sys/time.h>

SC_AndroidJNIAudioDriver::SC_AndroidJNIAudioDriver(struct World* inWorld): SC_AudioDriver(inWorld) {}

SC_AndroidJNIAudioDriver::~SC_AndroidJNIAudioDriver() {}

bool SC_AndroidJNIAudioDriver::DriverSetup(int* outNumSamplesPerCallback, double* outSampleRate) {
    // Here we are setting these to the values that were originally passed in as options.
    // In current android impl, these values came directly from java as args to scsynth_android_start().
    *outNumSamplesPerCallback = mPreferredHardwareBufferFrameSize;
    *outSampleRate = mPreferredSampleRate;

    int audioDataSize = mPreferredHardwareBufferFrameSize * mWorld->mNumOutputs * sizeof(float);
#ifndef NDEBUG
    scprintf("SC_AndroidJNIAudioDriver::DriverSetup: allocating %i bytes for %i frames\n", audioDataSize,
             mPreferredHardwareBufferFrameSize);
#endif

    if (mWorld->mVerbosity >= 0) {
        scprintf("<-SC_AndroidJNIAudioDriver::Setup world %p, mPreferredHardwareBufferFrameSize %i, "
                 "mPreferredSampleRate %i, outNumSamplesPerCallback %i, outSampleRate %g\n",
                 mWorld, mPreferredHardwareBufferFrameSize, mPreferredSampleRate, *outNumSamplesPerCallback,
                 *outSampleRate);
    }
    return true;
}

bool SC_AndroidJNIAudioDriver::DriverStart() {
    if (mWorld->mVerbosity >= 0) {
        scprintf("SC_AndroidJNIAudioDriver::DriverStart\n");
    }
    // no-op, nothing to do here
    return true;
}

bool SC_AndroidJNIAudioDriver::DriverStop() {
    if (mWorld->mVerbosity >= 0) {
        scprintf("SC_AndroidJNIAudioDriver::DriverStop\n");
    }

    // TODO: send a message back to java to say stop the audio loop

    return true;
}

// NB numSamplesPassed genuinely is num samples (not num frames as sometimes in sc code)
void SC_AndroidJNIAudioDriver::genaudio(short* arri, int numSamplesPassed) {
    // scprintf("->SC_AndroidJNIAudioDriver::genaudio()\n");

    World* world = mWorld;

    int numFramesPerCallback = NumSamplesPerCallback();
    // mOSCbuftime = oscTime;   // TODO, how do we set this?
    mFromEngine.Free();
    mToEngine.Perform();
    mOscPacketsToEngine.Perform();

    int numInputs = world->mNumInputs;
    int numOutputs = world->mNumOutputs;

    int bufFrames = mWorld->mBufLength;
    int numBufs = numFramesPerCallback / bufFrames;

#ifndef NDEBUG
    if ((numFramesPerCallback * numOutputs) != numSamplesPassed)
        scprintf("(numFramesPerCallback * numOutputs) != numSamplesPassed, %i %i\n", numFramesPerCallback, numOutputs,
                 numSamplesPassed);
#endif

    float* inBuses = mWorld->mAudioBus + mWorld->mNumOutputs * bufFrames;
    float* outBuses = mWorld->mAudioBus;
    int32* inTouched = mWorld->mAudioBusTouched + mWorld->mNumOutputs;
    int32* outTouched = mWorld->mAudioBusTouched;

    int minInputs = std::min<size_t>(numInputs, mWorld->mNumInputs);
    int minOutputs = std::min<size_t>(numOutputs, mWorld->mNumOutputs);

    int bufFramePos = 0;

    int64 oscTime = mOSCbuftime;
    int64 oscInc = mOSCincrement;
    double oscToSamples = mOSCtoSamples;

    // main loop
    for (int i = 0; i < numBufs; ++i, mWorld->mBufCounter++, bufFramePos += bufFrames) {
        int32 bufCounter = mWorld->mBufCounter;
        int32* tch;

        // copy+touch inputs
        tch = inTouched;
        for (int k = 0; k < minInputs; ++k) {
            float* dst = inBuses + k * bufFrames;
            // OK, so source is an interleaved array of ints, target is noninterleaved floats
            for (int frame = 0; frame < bufFrames; ++frame)
                *dst++ = (1.f / 32767.f) * (float)arri[(bufFramePos + frame) * minInputs + k];
            *tch++ = bufCounter;
        }

        // run engine
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

        World_Run(world);

        // copy touched outputs
        tch = outTouched;
        for (int k = 0; k < minOutputs; ++k) {
            // OK, so the source is noninterleaved floats, target is an interleaved array of ints
            if (*tch++ == bufCounter) {
                float* src = outBuses + k * bufFrames;
                for (int frame = 0; frame < bufFrames; ++frame)
                    arri[(bufFramePos + frame) * minOutputs + k] = (short)((*src++) * 32767.f);
            } else {
                for (int frame = 0; frame < bufFrames; ++frame)
                    arri[(bufFramePos + frame) * minOutputs + k] = 0;
            }
        }

        // update buffer time
        oscTime = mOSCbuftime = nextTime;
    }

    mAudioSync.Signal();
}

int64 gOSCoffset = 0;

static inline int64 GetCurrentOSCTime() {
    struct timeval tv;
    uint64 s, f;
    gettimeofday(&tv, 0);
    s = (uint64)tv.tv_sec + (uint64)kSECONDS_FROM_1900_to_1970;
    f = (uint64)((double)tv.tv_usec * kMicrosToOSCunits);

    return (s << 32) + f;
}

int64 oscTimeNow() { return GetCurrentOSCTime(); }

int32 server_timeseed() {
    int64 time = GetCurrentOSCTime();
    return Hash((int32)(time >> 32) + Hash((int32)time));
}

void initializeScheduler() { gOSCoffset = GetCurrentOSCTime(); }
