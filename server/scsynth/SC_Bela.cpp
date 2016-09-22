/*
	Bela (BeagleRT) audio driver for SuperCollider.
	Copyright (c) 2015 Dan Stowell. All rights reserved.
	Copyright (c) 2016 Marije Baalman. All rights reserved.

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

	This file contains elements from SC_PortAudio.cpp and SC_Jack.cpp,
	copyright their authors, and published under the same licence.
*/
#include "SC_CoreAudio.h"
#include <stdarg.h>
#include "SC_Prototypes.h"
#include "SC_HiddenWorld.h"
#include "SC_WorldOptions.h"
#include "SC_Time.hpp"
#include <math.h>
#include <stdlib.h>
#include <posix/time.h>		// For Xenomai clock_gettime()

#include "Bela.h"
// Xenomai-specific includes
#include <sys/mman.h>
#include <native/task.h>
#include <native/timer.h>
#include <native/intr.h>
#include <rtdk.h>

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
	int mAudioFramesPerAnalogFrame;
	
protected:
	// Driver interface methods
	virtual bool DriverSetup(int* outNumSamplesPerCallback, double* outSampleRate);
	virtual bool DriverStart();
	virtual bool DriverStop();

public:
	SC_BelaDriver(struct World *inWorld);
	virtual ~SC_BelaDriver();

	void setAudioFramesPerAnalogFrame( int afpaf );
	
	void BelaAudioCallback(BelaContext *belaContext);
	static void staticMAudioSyncSignal();
	static AuxiliaryTask mAudioSyncSignalTask;
	static int countInstances;
	static SC_SyncCondition* staticMAudioSync;
private:
	uint32 mSCBufLength;
};

AuxiliaryTask SC_BelaDriver::mAudioSyncSignalTask;
int SC_BelaDriver::countInstances;
SC_SyncCondition* SC_BelaDriver::staticMAudioSync;

SC_AudioDriver* SC_NewAudioDriver(struct World *inWorld)
{
	return new SC_BelaDriver(inWorld);
}

SC_BelaDriver::SC_BelaDriver(struct World *inWorld)
		: SC_AudioDriver(inWorld)
{
	mStartHostSecs = 0;
	mSCBufLength = inWorld->mBufLength;
	mAudioSyncSignalTask = Bela_createAuxiliaryTask(staticMAudioSyncSignal, 90, "mAudioSyncSignalTask");
	staticMAudioSync = &mAudioSync;
	++countInstances;
	if(countInstances != 1){
		printf("Error: there are %d instances of SC_BelaDriver running at the same time. Exiting\n", countInstances);
		exit(1);
	}
}

SC_BelaDriver::~SC_BelaDriver()
{
	// Clean up any resources allocated for audio
	Bela_cleanupAudio();
	--countInstances;
}

void render(BelaContext *belaContext, void *userData)
{
	SC_BelaDriver *driver = (SC_BelaDriver*)userData;

	driver->BelaAudioCallback(belaContext);
}

void sc_SetDenormalFlags();

void SC_BelaDriver::BelaAudioCallback(BelaContext *belaContext)
{
	struct timespec tspec;

	sc_SetDenormalFlags();
	World *world = mWorld;
	// add a pointer to belaWorld
// 	mWorld->mBelaContext = belaContext;
	world->mBelaContext = belaContext;
	
	// NOTE: code here is adapted from the SC_Jack.cpp, the version not using the DLL

	// Use Xenomai-friendly clock_gettime() -- note that this requires a -wrap argument to build
	clock_gettime(CLOCK_HOST_REALTIME, &tspec);

	double hostSecs = (double)tspec.tv_sec + (double)tspec.tv_nsec * 1.0e-9;
	double sampleTime = static_cast<double>(belaContext->audioFramesElapsed);

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
	}

	mPrevHostSecs = hostSecs;
	mPrevSampleTime = sampleTime;

	try {
		mFromEngine.Free();
		mToEngine.Perform();
		mOscPacketsToEngine.Perform();

		const uint32_t numInputs = belaContext->audioInChannels;
		const uint32_t numOutputs = belaContext->audioOutChannels;

		int numSamples = NumSamplesPerCallback();
		int bufFrames = mWorld->mBufLength;
		int numBufs = numSamples / bufFrames;

		float *inBuses = mWorld->mAudioBus + mWorld->mNumOutputs * bufFrames;
		float *outBuses = mWorld->mAudioBus;
		int32 *inTouched = mWorld->mAudioBusTouched + mWorld->mNumOutputs;
		int32 *outTouched = mWorld->mAudioBusTouched;

		int minInputs = sc_min(numInputs, mWorld->mNumInputs);
		int minOutputs = sc_min(numOutputs, mWorld->mNumOutputs);
		
		int anaInputs = 0;
		if ( numInputs < (int)mWorld->mNumInputs ){
		  anaInputs = sc_min( belaContext->analogInChannels, (int) (mWorld->mNumInputs - numInputs) );
		}
		int anaOutputs = 0;
		if ( numOutputs < (int)mWorld->mNumOutputs ){
		  anaOutputs = sc_min( belaContext->analogOutChannels, (int) (mWorld->mNumOutputs - numOutputs) );
		}

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
		int64 oscTime = mOSCbuftime = ((int64)(tspec.tv_sec + kSECONDS_FROM_1900_to_1970) << 32)
									  + (int64)(tspec.tv_nsec * kNanosToOSCunits);

		int64 oscInc = mOSCincrement;
		double oscToSamples = mOSCtoSamples;

		// clear out anything left over in audioOut buffer
		for (int i = 0; i < belaContext->audioFrames * belaContext->audioOutChannels; i++) {
			belaContext->audioOut[i] = 0;
		}

		for (int i = 0; i < numBufs; ++i, mWorld->mBufCounter++, bufFramePos += bufFrames) {
			int32 bufCounter = mWorld->mBufCounter;
			int32 *tch;

			// copy+touch inputs
			tch = inTouched;
			float *dst;
			for (int k = 0; k < minInputs; ++k) {
				dst = inBuses + k * bufFrames;
				for (int n = 0; n < bufFrames; ++n) {
					*dst++ = belaContext->audioIn[n * numInputs + k];
				}
				*tch++ = bufCounter;
			}
			for ( int k = minInputs; k < ( minInputs + anaInputs ); ++k ) {
				dst = inBuses + k * bufFrames;
				int analogPin = k - minInputs; // starting at 0
				float analogValue; // placeholder for analogvalue
				for (int n = 0; n < bufFrames; ++n) {
				  if(!(n % mAudioFramesPerAnalogFrame)) {
				    analogValue = analogRead(belaContext, n / mAudioFramesPerAnalogFrame, analogPin);
				  }
				  *dst++ = analogValue;
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
				if (*tch++ == bufCounter) {
					float *src = outBuses + k * bufFrames;
					for (int n = 0; n < bufFrames; ++n) {
						belaContext->audioOut[n * numOutputs + k] = *src++;
					}
				} else {
					for (int n = 0; n < bufFrames; ++n) {
						belaContext->audioOut[n * numOutputs + k] = 0.0f;
					}
				}
			}
			for (int k = minOutputs; k < ( minOutputs + anaOutputs ); ++k) {
				int analogPin = k - minOutputs; // starting at 0
				if (*tch++ == bufCounter) {
					float *src = outBuses + k * bufFrames;
					for (int n = 0; n < bufFrames; ++n) {
						if(!(n % mAudioFramesPerAnalogFrame)){
 							analogWriteOnce( belaContext, n / mAudioFramesPerAnalogFrame, analogPin, *src * 0.5 + 0.5 );
					  }
					  ++src;
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

	// this avoids Xenomai mode switches in the audio thread ...
	Bela_scheduleAuxiliaryTask(mAudioSyncSignalTask);
}

void SC_BelaDriver::staticMAudioSyncSignal(){
	// ... but mode switches are still happening here, in a lower priority thread.
	// FIXME: this triggers a mode switch in Xenomai.
	staticMAudioSync->Signal();
	rt_task_suspend(rt_task_self());
}
// ====================================================================

bool SC_BelaDriver::DriverSetup(int* outNumSamples, double* outSampleRate)
{
	scprintf("SC_BelaDriver: >>DriverSetup\n");
	BelaInitSettings settings;
	Bela_defaultSettings(&settings);	// This function should be called in main() before parsing any command-line arguments. It
				// sets default values in the data structure which specifies the BeagleRT settings, including
				// frame sizes, numbers of channels, volume levels and other parameters.

	if(mPreferredHardwareBufferFrameSize){
		settings.periodSize = mPreferredHardwareBufferFrameSize;
	}
	if(settings.periodSize < mSCBufLength) {
		scprintf("Error in SC_BelaDriver::DriverSetup(): hardware buffer size (%i) smaller than SC audio buffer size (%i). It is recommended to have them set to the same value, using both the '-Z' and '-z' command-line options respectively.\n",
				settings.periodSize, mSCBufLength);
		return false;
	}
	// note that Bela doesn't give us an option to choose samplerate, since it's baked-in.
	
	// configure the number of analog channels - this will determine their samplerate
	settings.useAnalog = 0;
	
	if ( mWorld->mBelaAnalogInputChannels > 0 ){
	  settings.useAnalog = 1;
	  if ( mWorld->mBelaAnalogInputChannels < 5 ){ // always use a minimum of 4 analog channels, as we cannot read analog I/O faster than audio rate	    
	    settings.numAnalogInChannels = 4; // analog rate == audio rate
	  } else {
	    settings.numAnalogInChannels = 8; // analog rate == audie rate / 2
	  }
	} else {
	  settings.numAnalogInChannels = 0;
	}
	
	if ( mWorld->mBelaAnalogOutputChannels > 0 ){
	  if ( mWorld->mBelaAnalogOutputChannels < 5 ){ // always use a minimum of 4 analog channels, as we cannot read analog I/O faster than audio rate	    
	    settings.numAnalogOutChannels = 4; // analog rate == audio rate
	  } else {
	    settings.numAnalogOutChannels = 8; // analog rate == audie rate / 2
	  }
	} else {
	  settings.numAnalogOutChannels = 0;
	}
	
	// configure the number of digital channels
	settings.useDigital = 0;
	
	if ( mWorld->mBelaDigitalChannels > 0 ){
	  settings.numDigitalChannels = mWorld->mBelaDigitalChannels;
	  settings.useDigital = 1;
	}
	if ( (mWorld->mBelaHeadphoneLevel >= -63.5) && ( mWorld->mBelaHeadphoneLevel <= 0. )) {             //headphone output level (0dB max; -63.5dB min)
	  settings.headphoneLevel = mWorld->mBelaHeadphoneLevel;
	}
	if ( mWorld->mBelaPGAGainLeft > 0 ){
	  settings.pgaGain[0] = mWorld->mBelaPGAGainLeft;
	}
	if ( mWorld->mBelaPGAGainRight > 0 ){
	  settings.pgaGain[1] = mWorld->mBelaPGAGainRight;
	}


	scprintf("SC_BelaDriver: >>DriverSetup - configured with (%i) analog input and (%i) analog output channels, and (%i) digital channels, with headphoneLevel (%f), pga_gain_left (%f) and pga_gain_right (%f)\n", settings.numAnalogInChannels, settings.numAnalogOutChannels, settings.numDigitalChannels, settings.headphoneLevel, settings.pgaGain[0],settings.pgaGain[1] );

	// Initialise the PRU audio device. This function prepares audio rendering in BeagleRT. It should be called from main() sometime
	// after command line option parsing has finished. It will initialise the rendering system, which
	// in the process will result in a call to the user-defined setup() function.
	if(Bela_initAudio(&settings, this) != 0) {
		scprintf("Error in SC_BelaDriver::DriverSetup(): unable to initialise audio\n");
		return false;
	}

	*outNumSamples = settings.periodSize;
	*outSampleRate = 44100.0;		// This is fixed in Bela at the moment

	return true;
}


void SC_BelaDriver::setAudioFramesPerAnalogFrame( int afpaf ){
  mAudioFramesPerAnalogFrame = afpaf;
}


// setup() is called once before the audio rendering starts.
// Use it to perform any initialisation and allocation which is dependent
// on the period size or sample rate.
//
// userData holds an opaque pointer to a data structure that was passed
// in from the call to initAudio().
//
// Return true on success; returning false halts the program.
bool setup(BelaContext* belaContext, void* userData)
{
	if(userData == 0){
		scprintf("SC_BelaDriver: error, setup() got no user data\n");
		return false;
	}
	
	// cast void pointer
	SC_BelaDriver *belaDriver = (SC_BelaDriver*) userData;
	if ( (belaContext->analogInChannels > 0) || (belaContext->analogOutChannels > 0) ){
	  belaDriver->setAudioFramesPerAnalogFrame( belaContext->audioFrames / belaContext->analogFrames );
	}

	return true;
}

// cleanup() is called once at the end, after the audio has stopped.
// Release any resources that were allocated in setup().
void cleanup(BelaContext *belaContext, void *userData)
{
}

bool SC_BelaDriver::DriverStart()
{
	scprintf("SC_BelaDriver: >>DriverStart\n");
	if(Bela_startAudio()) {
		scprintf("Error in SC_BelaDriver::DriverStart(): unable to start real-time audio\n");
		return false;
	}
	return true;
}

bool SC_BelaDriver::DriverStop()
{
	scprintf("SC_BelaDriver: >>DriverStop\n");
	Bela_stopAudio();
	return true;
}
