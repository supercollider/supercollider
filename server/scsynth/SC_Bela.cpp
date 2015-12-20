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
#include "SC_Prototypes.h"
#include "SC_HiddenWorld.h"
#include "SC_WorldOptions.h"
#include "SC_Time.hpp"
#include <math.h>
#include <stdlib.h>
#include <iostream>

#include "BeagleRT.h"

using namespace std;

int32 server_timeseed()
{
	return timeSeed();
}

int64 gOSCoffset = 0;

static inline int64 GetCurrentOSCTime()
{
	return OSCTime(getTime());
}

int64 oscTimeNow()
{
	return GetCurrentOSCTime();
}

void initializeScheduler()
{
	gOSCoffset = GetCurrentOSCTime();
}



class SC_BelaDriver : public SC_AudioDriver
{

	int mInputChannelCount, mOutputChannelCount;
protected:
	// Driver interface methods
	virtual bool DriverSetup(int* outNumSamplesPerCallback, double* outSampleRate);
	virtual bool DriverStart();
	virtual bool DriverStop();

public:
	SC_BelaDriver(struct World *inWorld);
	virtual ~SC_BelaDriver();

	void BelaAudioCallback(BeagleRTContext *belaContext);
};

SC_AudioDriver* SC_NewAudioDriver(struct World *inWorld)
{
	return new SC_BelaDriver(inWorld);
}

SC_BelaDriver::SC_BelaDriver(struct World *inWorld)
		: SC_AudioDriver(inWorld)
{
	mStartHostSecs = 0;
}

SC_BelaDriver::~SC_BelaDriver()
{
	// Clean up any resources allocated for audio
	BeagleRT_cleanupAudio();
}

void render(BeagleRTContext *belaContext, void *userData)
{
	SC_BelaDriver *driver = (SC_BelaDriver*)userData;

	driver->BelaAudioCallback(belaContext);
}
void sc_SetDenormalFlags();
void SC_BelaDriver::BelaAudioCallback(BeagleRTContext *belaContext)
{
	sc_SetDenormalFlags();
	World *world = mWorld;

	// NOTE: code here is adapted from the SC_Jack.cpp, the version not using the DLL
	HostTime hostTime = getTime();
	double hostSecs = secondsSinceEpoch(hostTime);
	// NOTE: jack driver uses time in num frames; bela gives it in num samples, so here we divide to give in num frames. need to do this?
	double sampleTime = static_cast<double>(belaContext->audioSampleCount / belaContext->audioFrames);
	//double sampleTime = (double)(jack_frame_time(client) + jack_frames_since_cycle_start(client));

	if (mStartHostSecs == 0) {
		mStartHostSecs = hostSecs;
		mStartSampleTime = sampleTime;
	} else {
		double instSampleRate = (sampleTime - mPrevSampleTime) / (hostSecs - mPrevHostSecs);
		double smoothSampleRate = mSmoothSampleRate;
		smoothSampleRate = smoothSampleRate + 0.002 * (instSampleRate - smoothSampleRate);
		if (fabs(smoothSampleRate - mSampleRate) > 10.) {
			smoothSampleRate = mSampleRate;
		}
		mOSCincrement = (int64)(mOSCincrementNumerator / smoothSampleRate);
		mSmoothSampleRate = smoothSampleRate;
#if 0
		double avgSampleRate = (sampleTime - mStartSampleTime)/(hostSecs - mStartHostSecs);
		double jitter = (smoothSampleRate * (hostSecs - mPrevHostSecs)) - (sampleTime - mPrevSampleTime);
		double drift = (smoothSampleRate - mSampleRate) * (hostSecs - mStartHostSecs);
#endif
	}

	mPrevHostSecs = hostSecs;
	mPrevSampleTime = sampleTime;

	try {
		mFromEngine.Free();
		mToEngine.Perform();
		mOscPacketsToEngine.Perform();

		const uint32_t numInputs = belaContext->audioChannels;
		const uint32_t numOutputs = belaContext->audioChannels;

		int numSamples = NumSamplesPerCallback();
		int bufFrames = mWorld->mBufLength;
		int numBufs = numSamples / bufFrames;

		float *inBuses = mWorld->mAudioBus + mWorld->mNumOutputs * bufFrames;
		float *outBuses = mWorld->mAudioBus;
		int32 *inTouched = mWorld->mAudioBusTouched + mWorld->mNumOutputs;
		int32 *outTouched = mWorld->mAudioBusTouched;

		int minInputs = sc_min(numInputs, (int)mWorld->mNumInputs);
		int minOutputs = sc_min(numOutputs, (int)mWorld->mNumOutputs);

		int bufFramePos = 0;

		// THIS IS TO DO LATER -- LOOK AT CACHEING AND CONSTING TO IMPROVE EFFICIENCY
		// cache I/O buffers
		//for (int i = 0; i < minInputs; ++i) {
		//	inBuffers[i] = (sc_jack_sample_t*)jack_port_get_buffer(inPorts[i], numSamples);
		//}
		//
		//for (int i = 0; i < minOutputs; ++i) {
		//	outBuffers[i] = (sc_jack_sample_t*)jack_port_get_buffer(outPorts[i], numSamples);
		//}

		// main loop
		int64 oscTime = mOSCbuftime = OSCTime(hostTime); // TODO CHECK: - (int64)(mMaxOutputLatency * kSecondsToOSCunits + .5);
		int64 oscInc = mOSCincrement;
		double oscToSamples = mOSCtoSamples;

		for (int i = 0; i < numBufs; ++i, mWorld->mBufCounter++, bufFramePos += bufFrames) {
			int32 bufCounter = mWorld->mBufCounter;
			int32 *tch;

			// copy+touch inputs
			tch = inTouched;
			for (int k = 0; k < minInputs; ++k) {
				//sc_jack_sample_t *src = inBuffers[k] + bufFramePos;
				float *dst = inBuses + k * bufFrames;
				for (int n = 0; n < bufFrames; ++n) {
					//*dst++ = *src++;
					*dst++ = belaContext->audioIn[n * numInputs + k];
				}
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
				//sc_jack_sample_t *dst = outBuffers[k] + bufFramePos;
				if (*tch++ == bufCounter) {
					float *src = outBuses + k * bufFrames;
					for (int n = 0; n < bufFrames; ++n) {
						//*dst++ = *src++;
						belaContext->audioOut[n * numOutputs + k] = *src++;
					}
				} else {
					for (int n = 0; n < bufFrames; ++n) {
						//*dst++ = 0.0f;
						belaContext->audioOut[n * numOutputs + k] = 0.0f;
					}
				}
			}

			// advance OSC time
			mOSCbuftime = oscTime = nextTime;
		}
	} catch (std::exception& exc) {
		scprintf("SC_BelaDriver: exception in real time: %s\n", exc.what());
	} catch (...) {
		scprintf("SC_BelaDriver: unknown exception in real time\n");
	}

	mAudioSync.Signal();
}

// ====================================================================

typedef struct {
	int* outNumSamples;
	double* outSampleRate;
} DataSlotsToFill; // just a convenience for use during driver setup

bool SC_BelaDriver::DriverSetup(int* outNumSamples, double* outSampleRate)
{
	BeagleRTInitSettings settings;
	BeagleRT_defaultSettings(&settings);	// This function should be called in main() before parsing any command-line arguments. It
				// sets default values in the data structure which specifies the BeagleRT settings, including
				// frame sizes, numbers of channels, volume levels and other parameters.



	/*
	TODO DAN NOTE: we do NOT know the samplerate or blocksize in here. anything that DOES need to know that goes in the setup() callback.
	*/
	DataSlotsToFill dataSlotsToFill;
	dataSlotsToFill.outNumSamples = outNumSamples;
	dataSlotsToFill.outSampleRate = outSampleRate;

	//NO, stick with bela documented layout    settings.interleave = 0; // we prefer our io buffers non-interleaved thanks
	settings.periodSize = mPreferredHardwareBufferFrameSize / 2; // halved because "periodSize" in bela is for the analogue pins not the audio pins
	// note that Bela doesn't give us an option to choose samplerate, since it's baked-in.

	// Initialise the PRU audio device. This function prepares audio rendering in BeagleRT. It should be called from main() sometime
	// after command line option parsing has finished. It will initialise the rendering system, which
	// in the process will result in a call to the user-defined setup() function.
	if(BeagleRT_initAudio(&settings, &dataSlotsToFill) != 0) {
		cout << "Error in SC_BelaDriver::DriverSetup(): unable to initialise audio" << endl;
		return false;
	}
	return true;
}

// setup() is called once before the audio rendering starts.
// Use it to perform any initialisation and allocation which is dependent
// on the period size or sample rate.
//
// userData holds an opaque pointer to a data structure that was passed
// in from the call to initAudio().
//
// Return true on success; returning false halts the program.
bool setup(BeagleRTContext* belaContext, void* userData)
{
	if(userData != 0){
		DataSlotsToFill* dataSlotsToFill = (DataSlotsToFill*)userData;
		*(dataSlotsToFill->outNumSamples) = static_cast<int>(belaContext->audioFrames);
		*(dataSlotsToFill->outSampleRate) = static_cast<double>(belaContext->audioSampleRate);
	}
	return true;
}

// cleanup() is called once at the end, after the audio has stopped.
// Release any resources that were allocated in setup().
void cleanup(BeagleRTContext *belaContext, void *userData)
{
}

bool SC_BelaDriver::DriverStart()
{
	// Start the audio device running
	if(BeagleRT_startAudio()) {
		cout << "Error in SC_BelaDriver::DriverStart(): unable to start real-time audio" << endl;
		return false;
	}
	return true;
}

bool SC_BelaDriver::DriverStop()
{
	// Stop the audio device
	BeagleRT_stopAudio();
	return true;
}
