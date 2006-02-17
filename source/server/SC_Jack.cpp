/*
	Jack audio driver interface.
	Copyright (c) 2003-2006 stefan kersten.

	====================================================================

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
#include "SC_HiddenWorld.h"
#include "SC_Prototypes.h"
#include "SC_StringParser.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

// =====================================================================
// Constants

static const char* kJackDriverIdent = "JackDriver";
static const char* kJackDefaultClientName = "SuperCollider";

// =====================================================================
// Timing

static inline int64 SC_JackOSCTime(const struct timeval& tv)
{
	return ((int64)(tv.tv_sec + kSECONDS_FROM_1900_to_1970) << 32)
		+ (int64)(tv.tv_usec * kMicrosToOSCunits);
}

static inline int64 SC_JackOSCTime()
{
	struct timeval tv;
	gettimeofday(&tv, 0);
	return SC_JackOSCTime(tv);
}

int32 server_timeseed()
{
	static int32 count = 0;
	struct timeval tv;
	gettimeofday(&tv, 0);
	return (int32)tv.tv_sec ^ (int32)tv.tv_usec ^ count--;
}

int64 oscTimeNow()
{
	return SC_JackOSCTime();
}

void initializeScheduler()
{
}

// =====================================================================
// Audio driver

static int sc_jack_process_cb(jack_nframes_t numFrames, void *arg);
static int sc_jack_bufsize_cb(jack_nframes_t numSamples, void *arg);
static int sc_jack_srate_cb(jack_nframes_t sampleRate, void *arg);
static int sc_jack_graph_order_cb(void *arg);
static void sc_jack_shutdown_cb(void *arg);

typedef jack_default_audio_sample_t sc_jack_sample_t;

struct SC_JackPortList
{
	int					mSize;
	jack_port_t**		mPorts;
	sc_jack_sample_t**	mBuffers;

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
		snprintf(portname, 32, fmt, i+1);
		mPorts[i] = jack_port_register(
					client, portname,
					JACK_DEFAULT_AUDIO_TYPE,
					type, 0);
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
	((SC_JackDriver*)arg)->Run();
	return 0;
}

int sc_jack_bufsize_cb(jack_nframes_t numSamples, void *arg)
{
	return !((SC_JackDriver*)arg)->BufferSizeChanged((int)numSamples);
}

int sc_jack_srate_cb(jack_nframes_t sampleRate, void *arg)
{
	return !((SC_JackDriver*)arg)->SampleRateChanged((double)sampleRate);
}

int sc_jack_graph_order_cb(void* arg)
{
	return !((SC_JackDriver*)arg)->GraphOrderChanged();
}

int sc_jack_xrun_cb(void* arg)
{
	return !((SC_JackDriver*)arg)->XRun();
}

void sc_jack_shutdown_cb(void* arg)
{
	scprintf("%s: killed by jack\n", kJackDriverIdent);
	((World*)arg)->hw->mQuitProgram->Release();
}

// =====================================================================
// SC_JackDriver (JACK)

SC_JackDriver::SC_JackDriver(struct World *inWorld)
	: SC_AudioDriver(inWorld),
	  mClient(0),
	  mInputList(0),
	  mOutputList(0),
	  mMaxOutputLatency(0)
{
}

SC_JackDriver::~SC_JackDriver()
{
	if (mClient) {
		jack_client_close(mClient);
	}

	delete mInputList;
	delete mOutputList;
}

// ====================================================================
// NOTE: for now, in lieu of a mechanism that passes generic options to
// the platform driver, we rely on environment variables:
//
// 	SC_JACK_DEFAULT_INPUTS:		which outports to connect to
// 	SC_JACK_DEFAULT_OUTPUTS:	which inports to connect to
// ====================================================================

bool SC_JackDriver::DriverSetup(int* outNumSamples, double* outSampleRate)
{
	char* clientName = 0;
	char* serverName = 0;

	if (mWorld->hw->mDeviceName && (strlen(mWorld->hw->mDeviceName) > 0)) {
		// parse string <serverName>:<clientName>
		SC_StringParser sp(mWorld->hw->mDeviceName, ':');
		if (!sp.AtEnd()) serverName = strdup(sp.NextToken());
		if (!sp.AtEnd()) clientName = strdup(sp.NextToken());
		if (clientName == 0) {
			// no semicolon found
			clientName = serverName;
			serverName = 0;
		} else if (strlen(clientName) == 0) {
			free(clientName);
			clientName = 0;
		}
	}

	mClient = jack_client_open(
		clientName ? clientName : kJackDefaultClientName,
		serverName ? JackServerName : JackNullOption,
		NULL, serverName);
	free(serverName); free(clientName);
	if (mClient == 0) return false;

	scprintf("%s: client name is '%s'\n", kJackDriverIdent, jack_get_client_name(mClient));

	// create jack I/O ports
	mInputList = new SC_JackPortList(mClient, mWorld->mNumInputs, JackPortIsInput);
	mOutputList = new SC_JackPortList(mClient, mWorld->mNumOutputs, JackPortIsOutput);

	// register callbacks
	jack_set_process_callback(mClient, sc_jack_process_cb, this);
	jack_set_buffer_size_callback(mClient, sc_jack_bufsize_cb, this);
	jack_set_sample_rate_callback(mClient, sc_jack_srate_cb, this);
	jack_set_graph_order_callback(mClient, sc_jack_graph_order_cb, this);
	jack_set_xrun_callback(mClient, sc_jack_xrun_cb, this);
	jack_on_shutdown(mClient, sc_jack_shutdown_cb, mWorld);

	*outNumSamples = (int)jack_get_buffer_size(mClient);
	*outSampleRate = (double)jack_get_sample_rate(mClient);

	return true;
}

bool SC_JackDriver::DriverStart()
{
	if (!mClient) return false;

	int err;
	SC_StringParser sp;
	jack_port_t **ports;
	int numPorts;

	// activate client
	err = jack_activate(mClient);
	if (err) {
		scprintf("%s: couldn't activate jack client\n", kJackDriverIdent);
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
			scprintf("%s: %s %s to %s\n",
					 kJackDriverIdent,
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
			scprintf("%s: %s %s to %s\n",
					 kJackDriverIdent,
					 err ? "couldn't connect " : "connected ",
					 thisPortName, thatPortName);
		}
	}

	return true;
}

bool SC_JackDriver::DriverStop()
{
	int err = 0;
	if (mClient) err = jack_deactivate(mClient);
	return err == 0;
}

void SC_JackDriver::Run()
{
	jack_client_t* client = mClient;
	World* world = mWorld;

	struct timeval hostTime;
	gettimeofday(&hostTime, 0);

	double hostSecs = (double)hostTime.tv_sec + (double)hostTime.tv_usec * 1e-6;
	double sampleTime = (double)(jack_frame_time(client) + jack_frames_since_cycle_start(client));

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
		static int tick = 0;
		if (++tick > 10) {
			tick = 0;
			scprintf(
				"%.6f %.6f %.6f\n", 
				mSampleRate, mSmoothSampleRate, fabs(mSampleRate - mSmoothSampleRate)
				);
		}
#endif
	}

	mPrevHostSecs = hostSecs;
	mPrevSampleTime = sampleTime;

	try {
		mFromEngine.Free();
		mToEngine.Perform();
		
		int numInputs = mInputList->mSize;
		int numOutputs = mOutputList->mSize;
		jack_port_t **inPorts = mInputList->mPorts;
		jack_port_t **outPorts = mOutputList->mPorts;
		sc_jack_sample_t **inBuffers = mInputList->mBuffers;
		sc_jack_sample_t **outBuffers = mOutputList->mBuffers;

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

		// cache I/O buffers
		for (int i = 0; i < minInputs; ++i) {
			inBuffers[i] = (sc_jack_sample_t*)jack_port_get_buffer(inPorts[i], numSamples);
		}

		for (int i = 0; i < minOutputs; ++i) {
			outBuffers[i] = (sc_jack_sample_t*)jack_port_get_buffer(outPorts[i], numSamples);
		}

		// main loop
		int64 oscTime = mOSCbuftime = SC_JackOSCTime(hostTime) + mMaxOutputLatency;
		int64 oscInc = mOSCincrement;
		double oscToSamples = mOSCtoSamples;

		for (int i = 0; i < numBufs; ++i, mWorld->mBufCounter++, bufFramePos += bufFrames) {
			int32 bufCounter = mWorld->mBufCounter;
			int32 *tch;
			
			// copy+touch inputs
			tch = inTouched;
			for (int k = 0; k < minInputs; ++k) {
				sc_jack_sample_t *src = inBuffers[k] + bufFramePos;
				float *dst = inBuses + k * bufFrames;
				for (int n = 0; n < bufFrames; ++n) {
					*dst++ = *src++;
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
			World_Run(world);

			// copy touched outputs
			tch = outTouched;
			for (int k = 0; k < minOutputs; ++k) {
				sc_jack_sample_t *dst = outBuffers[k] + bufFramePos;
				if (*tch++ == bufCounter) {
					float *src = outBuses + k * bufFrames;
					for (int n = 0; n < bufFrames; ++n) {
						*dst++ = *src++;
					}
				} else {
					for (int n = 0; n < bufFrames; ++n) {
						*dst++ = 0.0f;
					}
				}
			}

			// advance OSC time
			mOSCbuftime = oscTime = nextTime;
		}
	} catch (std::exception& exc) {
		scprintf("%s: exception in real time: %s\n", kJackDriverIdent, exc.what());
	} catch (...) {
		scprintf("%s: unknown exception in real time\n", kJackDriverIdent);
	}

	double cpuUsage = (double)jack_cpu_load(mClient);
	mAvgCPU = mAvgCPU + 0.1 * (cpuUsage - mAvgCPU);
	if (cpuUsage > mPeakCPU || --mPeakCounter <= 0) {
		mPeakCPU = cpuUsage;
		mPeakCounter = mMaxPeakCounter;
	}

	mAudioSync.Signal();
}

void SC_JackDriver::Reset(double sampleRate, int bufferSize)
{
	scprintf("Reset %f %d\n", sampleRate, bufferSize);

	mSampleRate = mSmoothSampleRate = jack_get_sample_rate(mClient);
	mNumSamplesPerCallback = bufferSize;

	World_SetSampleRate(mWorld, mSampleRate);
	mBuffersPerSecond = mSampleRate / mNumSamplesPerCallback;
	mMaxPeakCounter = (int)mBuffersPerSecond;
	mOSCincrement = (int64)(mOSCincrementNumerator / mSampleRate);
}

bool SC_JackDriver::BufferSizeChanged(int numSamples)
{
	Reset(jack_get_sample_rate(mClient), numSamples);
	return true;
}

bool SC_JackDriver::SampleRateChanged(double sampleRate)
{
	Reset(sampleRate, jack_get_buffer_size(mClient));
	return true;
}

bool SC_JackDriver::GraphOrderChanged()
{
	SC_JackPortList* outputs = mOutputList;
	jack_nframes_t lat = 0;

	for (int i=0; i < outputs->mSize; ++i) {
		jack_nframes_t portLat = jack_port_get_total_latency(mClient, outputs->mPorts[i]);
		if (portLat > lat) lat = portLat;
	}

	int64 maxLat = (int64)((double)lat / mSampleRate * kSecondsToOSCunits);

	if (maxLat != mMaxOutputLatency) {
		mMaxOutputLatency = maxLat;
		scprintf("%s: max output latency %f\n", kJackDriverIdent, maxLat * kOSCtoSecs);
	}

	return true;
}

bool SC_JackDriver::XRun()
{
// 	scprintf("%s: xrun\n", kJackDriverIdent);
	return true;
}

// EOF
