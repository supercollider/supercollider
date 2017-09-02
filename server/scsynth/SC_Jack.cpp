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
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
*/

#include "SC_CoreAudio.h"
#include "SC_HiddenWorld.h"
#include "SC_Prototypes.h"
#include "SC_StringParser.h"
#include "SC_WorldOptions.h"
#include "SC_TimeDLL.hpp"
#include "SC_Time.hpp"

#include <jack/jack.h>
#ifdef SC_JACK_USE_METADATA_API
#	include <jack/metadata.h>
#	include <jack/uuid.h>
#	include <jackey.h>
#endif
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

int32 server_timeseed()
{
	return timeSeed();
}

int64 oscTimeNow()
{
	return OSCTime(getTime());
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

	SC_JackPortList(jack_client_t *client, int orderOffset, int numPorts, int type);
	~SC_JackPortList();
};

class SC_JackDriver : public SC_AudioDriver
{
	jack_client_t		*mClient;
	SC_JackPortList		*mInputList;
	SC_JackPortList		*mOutputList;
	double				mMaxOutputLatency;
	SC_TimeDLL			mDLL;

protected:
	// driver interface methods
	virtual bool DriverSetup(int* outNumSamplesPerCallback, double* outSampleRate);
	virtual bool DriverStart();
	virtual bool DriverStop();

public:
	SC_JackDriver(struct World *inWorld);
	virtual ~SC_JackDriver();

	// process loop
	void Run();

	// reset state
	void Reset(double sampleRate, int bufferSize);

	// notifications
	bool BufferSizeChanged(int numSamples);
	bool SampleRateChanged(double sampleRate);
	bool GraphOrderChanged();
	bool XRun();

private:
	void ConnectPorts(const char * src, const char * dst)
	{
		int err = jack_connect(mClient, src, dst);
		scprintf("%s: %s %s to %s\n",
		         kJackDriverIdent,
		         err ? "couldn't connect " : "connected ",
		         src, dst);
	}

	void ConnectClientInputs(const char * pattern);
	void ConnectClientOutputs(const char * pattern);
};

SC_AudioDriver* SC_NewAudioDriver(struct World *inWorld)
{
	return new SC_JackDriver(inWorld);
}

// =====================================================================
// SC_JackPortList

SC_JackPortList::SC_JackPortList(jack_client_t *client, int orderOffset, int numPorts, int type)
	: mSize(numPorts), mPorts(0), mBuffers(0)
{
	const char *fmt = (type == JackPortIsInput ? "in_%d" : "out_%d");
#ifdef SC_JACK_USE_METADATA_API
	const char *prettyFmt= (type == JackPortIsInput ? "Input %d" : "Output %d");
#endif
	char tempStr[32];

	mPorts = new jack_port_t*[mSize];
	mBuffers = new float*[mSize];

	for (int i = 0; i < mSize; i++) {
		snprintf(tempStr, 32, fmt, i+1);
		mPorts[i] = jack_port_register(
					client, tempStr,
					JACK_DEFAULT_AUDIO_TYPE,
					type, 0);
		mBuffers[i] = 0;

#ifdef SC_JACK_USE_METADATA_API
		jack_uuid_t uuid = jack_port_uuid(mPorts[i]);
		if(!jack_uuid_empty(uuid)) {
			snprintf(tempStr, 32, prettyFmt, i+1);
			jack_set_property(client, uuid, JACK_METADATA_PRETTY_NAME,
				tempStr, "text/plain");

			snprintf(tempStr, 32, "%d", orderOffset + i);
			jack_set_property(client, uuid, JACKEY_ORDER,
				tempStr, "http://www.w3.org/2001/XMLSchema#integer");
		}
#endif
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
	World * world = (World*)arg;
	world->hw->mTerminating = true;
	world->hw->mQuitProgram->post();
}

// =====================================================================
// SC_JackDriver (JACK)

SC_JackDriver::SC_JackDriver(struct World *inWorld)
	: SC_AudioDriver(inWorld),
	mClient(0),
	mInputList(0),
	mOutputList(0),
	mMaxOutputLatency(0.)
{ }

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

	if (mWorld->hw->mInDeviceName && (strlen(mWorld->hw->mInDeviceName) > 0)) {
		// parse string <serverName>:<clientName>
		SC_StringParser sp(mWorld->hw->mInDeviceName, ':');
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
	if (serverName) free(serverName); if (clientName) free(clientName);
	if (mClient == 0) return false;

	scprintf("%s: client name is '%s'\n", kJackDriverIdent, jack_get_client_name(mClient));

	// create jack I/O ports
	mInputList = new SC_JackPortList(mClient, 0, mWorld->mNumInputs, JackPortIsInput);
	mOutputList = new SC_JackPortList(mClient, mWorld->mNumInputs, mWorld->mNumOutputs, JackPortIsOutput);

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

void SC_JackDriver::ConnectClientInputs(const char * pattern)
{
	const char **ports = jack_get_ports (mClient, pattern, NULL, JackPortIsOutput);
	jack_port_t ** ourports = mInputList->mPorts;

	if (!ports)
		return;

	int i = 0;
	while (ports[i]) {
		if (i == mInputList->mSize)
			break;

		const char * src = ports[i];
		const char * dst = jack_port_name(ourports[i]);

		ConnectPorts(src, dst);
		++i;
	}

	free(ports);
}

void SC_JackDriver::ConnectClientOutputs(const char * pattern)
{
	const char **ports = jack_get_ports (mClient, pattern, NULL, JackPortIsInput);
	jack_port_t ** ourports = mOutputList->mPorts;

	if (!ports)
		return;

	int i = 0;
	while (ports[i]) {
		if (i == mOutputList->mSize)
			break;

		const char * src = jack_port_name(ourports[i]);
		const char * dst = ports[i];

		ConnectPorts(src, dst);
		++i;
	}

	free(ports);
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
		const char *thatPortName = sp.NextToken();

		if (i == 0 && sp.AtEnd() && (strchr(thatPortName, ':') == 0)) {
			ConnectClientInputs(thatPortName);
			break;
		}

		const char *thisPortName = jack_port_name(ports[i]);
		if (thisPortName && thatPortName)
			ConnectPorts(thatPortName, thisPortName);
	}

	// connect default outputs
	sp = SC_StringParser(getenv("SC_JACK_DEFAULT_OUTPUTS"), ',');
	ports = mOutputList->mPorts;
	numPorts = mOutputList->mSize;
	for (int i = 0; !sp.AtEnd() && (i < numPorts); i++) {
		const char *thatPortName = sp.NextToken();

		if (i == 0 && sp.AtEnd() && (strchr(thatPortName, ':') == 0)) {
			ConnectClientOutputs(thatPortName);
			break;
		}

		const char *thisPortName = jack_port_name(ports[i]);
		if (thisPortName && thatPortName)
			ConnectPorts(thisPortName, thatPortName);
	}

	return true;
}

bool SC_JackDriver::DriverStop()
{
	int err = 0;
	if (mClient) err = jack_deactivate(mClient);
	return err == 0;
}
void sc_SetDenormalFlags();
void SC_JackDriver::Run()
{
	sc_SetDenormalFlags();
	jack_client_t* client = mClient;
	World* world = mWorld;

#ifdef SC_JACK_USE_DLL
	mDLL.Update(secondsSinceEpoch(getTime()));
#if SC_JACK_DEBUG_DLL
	static int tick = 0;
	if (++tick >= 10) {
		tick = 0;
		scprintf("DLL: t %.6f p %.9f sr %.6f e %.9f avg(e) %.9f inc %.9f\n",
		         mDLL.PeriodTime(), mDLL.Period(), mDLL.SampleRate(),
		         mDLL.Error(), mDLL.AvgError(), mOSCincrement * kOSCtoSecs);
	}
#endif
#else
	HostTime hostTime = getTime();

	double hostSecs = secondsSinceEpoch(hostTime);
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
#endif
	}

	mPrevHostSecs = hostSecs;
	mPrevSampleTime = sampleTime;
#endif

	try {
		mFromEngine.Free();
		mToEngine.Perform();
		mOscPacketsToEngine.Perform();

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
#ifdef SC_JACK_USE_DLL
		int64 oscTime = mOSCbuftime = (int64)((mDLL.PeriodTime() - mMaxOutputLatency) * kSecondsToOSCunits + .5);
// 		int64 oscInc = mOSCincrement = (int64)(mOSCincrementNumerator / mDLL.SampleRate());
		int64 oscInc = mOSCincrement = (int64)((mDLL.Period() / numBufs) * kSecondsToOSCunits + .5);
		mSmoothSampleRate = mDLL.SampleRate();
		double oscToSamples = mOSCtoSamples = mSmoothSampleRate * kOSCtoSecs /* 1/pow(2,32) */;
#else
		int64 oscTime = mOSCbuftime = OSCTime(hostTime) - (int64)(mMaxOutputLatency * kSecondsToOSCunits + .5);
		int64 oscInc = mOSCincrement;
		double oscToSamples = mOSCtoSamples;
#endif

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
			world->mSubsampleOffset = 0.f;
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
	mSampleRate = mSmoothSampleRate = sampleRate;
	mNumSamplesPerCallback = bufferSize;

	World_SetSampleRate(mWorld, mSampleRate);
	mBuffersPerSecond = mSampleRate / mNumSamplesPerCallback;
	mMaxPeakCounter = (int)mBuffersPerSecond;
	mOSCincrement = (int64)(mOSCincrementNumerator / mSampleRate);

#ifdef SC_JACK_USE_DLL
	mDLL.Reset(
		mSampleRate,
		mNumSamplesPerCallback,
		SC_TIME_DLL_BW,
		secondsSinceEpoch(getTime()));
#endif
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
		jack_latency_range_t range;
		jack_port_get_latency_range( outputs->mPorts[i], JackPlaybackLatency, &range );
		jack_nframes_t portLat = range.max;
		if (portLat > lat) lat = portLat;
	}

	double maxLat = (double)lat / mSampleRate;

	if (maxLat != mMaxOutputLatency) {
		mMaxOutputLatency = maxLat;
		scprintf("%s: max output latency %.1f ms\n", kJackDriverIdent, maxLat * 1e3);
	}

	return true;
}

bool SC_JackDriver::XRun()
{
	Reset(mSampleRate, mNumSamplesPerCallback);
	return true;
}

// EOF
