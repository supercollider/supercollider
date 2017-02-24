/*
	Alsa MIDI/Sequencer support.
	Copyright (c) 2004 stefan kersten.

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

#include "SCBase.h"
#include "VMGlobals.h"
#include "PyrSymbolTable.h"
#include "PyrInterpreter.h"
#include "PyrKernel.h"

#include "PyrPrimitive.h"
#include "PyrObjectProto.h"
#include "PyrPrimitiveProto.h"
#include "PyrKernelProto.h"
#include "SC_InlineUnaryOp.h"
#include "SC_InlineBinaryOp.h"
#include "PyrSched.h"
#include "GC.h"
#include "SC_LanguageClient.h"

#include <atomic>
#include <thread>

PyrSymbol* s_midiin;
PyrSymbol* s_domidiaction;
PyrSymbol* s_midiNoteOnAction;
PyrSymbol* s_midiNoteOffAction;
PyrSymbol* s_midiTouchAction;
PyrSymbol* s_midiControlAction;
PyrSymbol* s_midiPolyTouchAction;
PyrSymbol* s_midiProgramAction;
PyrSymbol* s_midiBendAction;
PyrSymbol* s_midiSysexAction;
PyrSymbol* s_midiSysrtAction;
PyrSymbol* s_midiSMPTEAction;

const int kMaxMidiPorts = 128;
bool gMIDIInitialized = false;

extern bool compiledOK;

// =====================================================================
// Platform declarations (interface routines)
// =====================================================================

static int initClient();
static int initMIDI(int numIn, int numOut);
static int disposeMIDI();
static int restartMIDI();
static void cleanUpMIDI();

static int listMIDIEndpoints(struct VMGlobals *g, PyrSlot *a);
static int connectMIDIIn(int inputIndex, int uid);
static int disconnectMIDIIn(int inputIndex, int uid);
static int connectMIDIOut(int outputIndex, int uid);
static int disconnectMIDIOut(int outputIndex, int uid);

static int sendMIDI(int port, int destId, int length, int hiStatus, int loStatus, int aval, int bval, float late);
static int sendMIDISysex(int port, int destId, int length, uint8* data);

// =====================================================================
// Platform declarations (ALSA)
// =====================================================================

#include <alsa/asoundlib.h>
#include <vector>
#include <string.h>

static const size_t kAlsaMaxPacketSize = 3;
static const size_t kAlsaMaxPortNameLen = 256;

// MIDI packet
struct SC_AlsaMidiPacket
{
	uint8 data[kAlsaMaxPacketSize];
};

// MIDI client state
struct SC_AlsaMidiClient
{
	snd_seq_t*			mHandle;
	int					mQueue;
	int					mNumInPorts;
	int					mInPorts[kMaxMidiPorts];
	int					mNumOutPorts;
	int					mOutPorts[kMaxMidiPorts];
	snd_midi_event_t*	mEventToMidi;
	snd_midi_event_t*	mMidiToEvent;
	std::thread			mInputThread;
	std::atomic_bool	mShouldBeRunning { false };

	int startInputThread()
	{
		mShouldBeRunning = true;
		try {
			std::thread inputThread ( [this] {
				inputThreadFunc();
			});
			mInputThread = std::move( inputThread );
			return errNone;
		} catch (...) {
			post("MIDI (ALSA): could not start input thread\n");
			return errFailed;
		}
	}

	void joinInputThread()
	{
		mShouldBeRunning = false;
		if( mInputThread.joinable() )
			mInputThread.join();
	}

	void inputThreadFunc();
	void processEvent(snd_seq_event_t* evt);

	int connectInput(int inputIndex, int uid, int (*action)(snd_seq_t*, snd_seq_port_subscribe_t*), const char* actionName);
	int connectOutput(int outputIndex, int uid, int (*action)(snd_seq_t*, snd_seq_port_subscribe_t*), const char* actionName);
	int sendEvent(int outputIndex, int uid, snd_seq_event_t* evt, float late=0.f);

	int mClientID;
};

static SC_AlsaMidiClient gMIDIClient;

// Port description
struct SC_AlsaMidiPort
{
	SC_AlsaMidiPort()
		: uid(0)
	{ *name = 0; *device = 0; }

	char		name[kAlsaMaxPortNameLen];
	char		device[kAlsaMaxPortNameLen];
	int32		uid;
};

// =====================================================================
// Platform implementation (ALSA)
// =====================================================================

static inline int SC_AlsaMakeUID(int clientID, int portID)
{
	return (clientID << 16) | (portID & 0xFFFF);
}

static inline void SC_AlsaParseUID(int uid, int& clientID, int& portID)
{
	clientID = uid >> 16;
	portID = uid & 0xFFFF;
}

void SC_AlsaMidiClient::processEvent(snd_seq_event_t* evt)
{
	int status = lockLanguageOrQuit(mShouldBeRunning);
	if (status == EINTR)
		return;
	if (status) {
		postfl("error when locking language (%d)\n", status);
		return;
	}

	if (compiledOK) {
		VMGlobals* g = gMainVMGlobals;
		PyrInt8Array* sysexArray;

		SC_AlsaMidiPacket pkt;

		g->canCallOS = false; // cannot call the OS

		// class MIDIIn
		++g->sp; SetObject(g->sp, s_midiin->u.classobj);
		// source
		++g->sp; SetInt(g->sp, SC_AlsaMakeUID(evt->source.client, evt->source.port));

		switch (evt->type) {
		// midi events
		case SND_SEQ_EVENT_NOTEOFF:			// noteOff
			++g->sp; SetInt(g->sp, evt->data.note.channel);
			++g->sp; SetInt(g->sp, evt->data.note.note);
			++g->sp; SetInt(g->sp, evt->data.note.velocity);
			runInterpreter(g, s_midiNoteOffAction, 5);
			break;
		case SND_SEQ_EVENT_NOTEON:			// noteOn
			++g->sp; SetInt(g->sp, evt->data.note.channel);
			++g->sp; SetInt(g->sp, evt->data.note.note);
			++g->sp; SetInt(g->sp, evt->data.note.velocity);
			// 				runInterpreter(g, evt->data.note.velocity ? s_midiNoteOnAction : s_midiNoteOffAction, 5);
			runInterpreter(g, s_midiNoteOnAction, 5);
			break;
		case SND_SEQ_EVENT_KEYPRESS:		// polytouch
			++g->sp; SetInt(g->sp, evt->data.note.channel);
			++g->sp; SetInt(g->sp, evt->data.note.note);
			++g->sp; SetInt(g->sp, evt->data.note.velocity);
			runInterpreter(g, s_midiPolyTouchAction, 5);
			break;
		case SND_SEQ_EVENT_CONTROLLER:		// control
			++g->sp; SetInt(g->sp, evt->data.control.channel);
			++g->sp; SetInt(g->sp, evt->data.control.param);
			++g->sp; SetInt(g->sp, evt->data.control.value);
			runInterpreter(g, s_midiControlAction, 5);
			break;
		case SND_SEQ_EVENT_PGMCHANGE:		// program
			++g->sp; SetInt(g->sp, evt->data.control.channel);
			++g->sp; SetInt(g->sp, evt->data.control.value);
			runInterpreter(g, s_midiProgramAction, 4);
			break;
		case SND_SEQ_EVENT_CHANPRESS:		// touch
			++g->sp; SetInt(g->sp, evt->data.control.channel);
			++g->sp; SetInt(g->sp, evt->data.control.value);
			runInterpreter(g, s_midiTouchAction, 4);
			break;
		case SND_SEQ_EVENT_PITCHBEND:		// bend
			++g->sp; SetInt(g->sp, evt->data.control.channel);
			++g->sp; SetInt(g->sp, evt->data.control.value + 8192);
			runInterpreter(g, s_midiBendAction, 4);
			break;
			// system common events
		case SND_SEQ_EVENT_QFRAME:			// mtc quarter frame
		{
			int index = evt->data.control.value >> 4;
			int data = evt->data.control.value & 0xf;

#if 0
			post(
						"mtc qframe: byte 0x%x index 0x%x data 0x%x\n",
						evt->data.control.value,
						index, data
						);
#endif

			switch (index) {
			case 1: case 3:
			case 5:	case 7:
				data = data << 4;
			}

			++g->sp; SetInt(g->sp, index);
			++g->sp; SetInt(g->sp, data);
		}
			runInterpreter(g, s_midiSMPTEAction, 4);
			break;
		case SND_SEQ_EVENT_SONGPOS:			// song ptr
			++g->sp; SetInt(g->sp, evt->data.control.channel);
			++g->sp; SetInt(g->sp, (evt->data.control.value << 7) | evt->data.control.param);
			runInterpreter(g, s_midiSysrtAction, 4);
			break;
		case SND_SEQ_EVENT_SONGSEL:			// song sel
			++g->sp; SetInt(g->sp, evt->data.control.channel);
			++g->sp; SetInt(g->sp, evt->data.control.param);
			runInterpreter(g, s_midiSysrtAction, 4);
			break;
			// system realtime events
		case SND_SEQ_EVENT_TUNE_REQUEST:	// tuning request
			++g->sp; SetInt(g->sp, 0x6);
			++g->sp; SetInt(g->sp, 0);
			runInterpreter(g, s_midiSysrtAction, 4);
			break;
		case SND_SEQ_EVENT_CLOCK:			// clock
			++g->sp; SetInt(g->sp, 0x8);
			++g->sp; SetInt(g->sp, 0);
			runInterpreter(g, s_midiSysrtAction, 4);
			break;
		case SND_SEQ_EVENT_TICK:			// tick
			++g->sp; SetInt(g->sp, 0x9);
			++g->sp; SetInt(g->sp, 0);
			runInterpreter(g, s_midiSysrtAction, 4);
			break;
		case SND_SEQ_EVENT_START:			// start
			++g->sp; SetInt(g->sp, 0xA);
			++g->sp; SetInt(g->sp, 0);
			runInterpreter(g, s_midiSysrtAction, 4);
			break;
		case SND_SEQ_EVENT_CONTINUE:		// continue
			++g->sp; SetInt(g->sp, 0xB);
			++g->sp; SetInt(g->sp, 0);
			runInterpreter(g, s_midiSysrtAction, 4);
			break;
		case SND_SEQ_EVENT_STOP:			// stop
			++g->sp; SetInt(g->sp, 0xC);
			++g->sp; SetInt(g->sp, 0);
			runInterpreter(g, s_midiSysrtAction, 4);
			break;
		case SND_SEQ_EVENT_SENSING:			// active sensing
			++g->sp; SetInt(g->sp, 0xE);
			++g->sp; SetInt(g->sp, 0);
			runInterpreter(g, s_midiSysrtAction, 4);
			break;
		case SND_SEQ_EVENT_RESET:			// system reset
			++g->sp; SetInt(g->sp, 0xF);
			++g->sp; SetInt(g->sp, 0);
			runInterpreter(g, s_midiSysrtAction, 4);
			break;
			// sysex events
		case SND_SEQ_EVENT_SYSEX:			// sysex
			sysexArray = newPyrInt8Array(g->gc, evt->data.ext.len, 0, true);
			memcpy(sysexArray->b, evt->data.ext.ptr, evt->data.ext.len);
			sysexArray->size = evt->data.ext.len;
			++g->sp; SetObject(g->sp, (PyrObject*)sysexArray);
			runInterpreter(g, s_midiSysexAction, 3);
			break;
		default:
			// unknown: convert to midi packet
			snd_midi_event_reset_decode(mEventToMidi);
			memset(pkt.data, 0, kAlsaMaxPacketSize);
			if (snd_midi_event_decode(mEventToMidi, pkt.data, kAlsaMaxPacketSize, evt) > 0) {
				for (size_t i=0; i < kAlsaMaxPacketSize; i++) {
					++g->sp; SetInt(g->sp, pkt.data[i]);
				}
				runInterpreter(g, s_domidiaction, 2+kAlsaMaxPacketSize);
			} else {
				g->sp -= 2;
			}
		}
		g->canCallOS = false;
	}
	gLangMutex.unlock();
}

void SC_AlsaMidiClient::inputThreadFunc()
{
	int npfd = snd_seq_poll_descriptors_count(mHandle, POLLIN);
	struct pollfd pfd[npfd];

	snd_seq_poll_descriptors(mHandle, pfd, npfd, POLLIN);

	while (mShouldBeRunning.load( std::memory_order_relaxed )) {
		if (poll(pfd, npfd, 2000) > 0) { // 2s timeout
			for (int i=0; i < npfd; i++) {
				if (pfd[i].revents > 0) {
					do {
						snd_seq_event_t* evt = nullptr;
						int status = snd_seq_event_input(mHandle, &evt);
						if( status > 0 ) {
							assert( evt );
							processEvent(evt);
							snd_seq_free_event(evt);
						}
					} while (snd_seq_event_input_pending(mHandle, 0) > 0);
				}
			}
		}
	}
}

int SC_AlsaMidiClient::connectInput(int inputIndex, int uid, int (*action)(snd_seq_t*, snd_seq_port_subscribe_t*), const char* actionName)
{
	snd_seq_t* seq = mHandle;
	snd_seq_client_info_t* cinfo;
	snd_seq_port_subscribe_t* subs;
	snd_seq_addr_t src, dst;
	int cid, pid;

	if ((inputIndex < 0) || (inputIndex >= mNumInPorts)) return errIndexOutOfRange;

	snd_seq_client_info_alloca(&cinfo);
	if (snd_seq_get_client_info(seq, cinfo) < 0) {
		post("MIDI (ALSA): could not get client info: %s\n", snd_strerror(errno));
		return errFailed;
	}

	dst.client = snd_seq_client_info_get_client(cinfo);
	dst.port = mInPorts[inputIndex];
	SC_AlsaParseUID(uid, cid, pid);
	src.client = cid;
	src.port = pid;

	//post("MIDI (ALSA): connect ndx %d uid %u dst %d:%d src %d:%d\n", inputIndex, uid, dst.client, dst.port, src.client, src.port);

	snd_seq_port_subscribe_alloca(&subs);
	snd_seq_port_subscribe_set_sender(subs, &src);
	snd_seq_port_subscribe_set_dest(subs, &dst);

	if ((*action)(seq, subs) < 0) {
		post("MIDI (ALSA): %s failed (%s)\n", actionName, snd_strerror(errno));
		return errFailed;
	}

	return errNone;
}

int SC_AlsaMidiClient::connectOutput(int outputIndex, int uid, int (*action)(snd_seq_t*, snd_seq_port_subscribe_t*), const char* actionName)
{
	snd_seq_t* seq = mHandle;
	snd_seq_client_info_t* cinfo;
	snd_seq_port_subscribe_t* subs;
	snd_seq_addr_t src, dst;
	int cid, pid;

	if ((outputIndex < 0) || (outputIndex >= mNumOutPorts)) return errIndexOutOfRange;

	snd_seq_client_info_alloca(&cinfo);
	if (snd_seq_get_client_info(seq, cinfo) < 0) {
		post("MIDI (ALSA): could not get client info: %s\n", snd_strerror(errno));
		return errFailed;
	}

	src.client = snd_seq_client_info_get_client(cinfo);
	src.port = mOutPorts[outputIndex];
	SC_AlsaParseUID(uid, cid, pid);
	dst.client = cid;
	dst.port = pid;

// 	post("MIDI (ALSA): connect ndx %d uid %u dst %d:%d src %d:%d\n", outputIndex, uid, dst.client, dst.port, src.client, src.port);

	snd_seq_port_subscribe_alloca(&subs);
	snd_seq_port_subscribe_set_sender(subs, &src);
	snd_seq_port_subscribe_set_dest(subs, &dst);

	if ((*action)(seq, subs) < 0) {
		post("MIDI (ALSA): %s failed (%s)\n", actionName, snd_strerror(errno));
		return errFailed;
	}

	return errNone;
}

int SC_AlsaMidiClient::sendEvent(int outputIndex, int uid, snd_seq_event_t* evt, float late)
{
	snd_seq_real_time time;

	if ((outputIndex < 0) || (outputIndex >= mNumOutPorts)) return errIndexOutOfRange;

	snd_seq_ev_set_source(evt, mOutPorts[outputIndex]);
	if (uid == 0) {
		// send to all subscribed ports
		snd_seq_ev_set_subs(evt);
	} else {
		// send to specific port
		int cid, pid;
		SC_AlsaParseUID(uid, cid, pid);
		snd_seq_ev_set_dest(evt, cid, pid);
	}

// 	long latelong;
	if (late > 0.f) {
// 		latelong = (long) (late * 1000000000);
// new time calculation. The old one was not correct
// 		time.tv_sec = (long)(latelong / 1000000000); // seconds
// 		time.tv_nsec = (long)(latelong % 1000000000); // nanoseconds
		time.tv_sec = (long)(floorf (late));
		time.tv_nsec = (long)((late - time.tv_sec) * 1e9f);
	} else {
		time.tv_sec = time.tv_nsec = 0;
	}

// 	evt->flags = evt->flags | SND_SEQ_TIME_STAMP_REAL;

// 	post("MIDI (ALSA): sending event, time %i, %i, late %f, latelong %i\n", time.tv_sec, time.tv_nsec, late, latelong);

	snd_seq_ev_schedule_real(evt, mQueue, 1, &time);
	snd_seq_event_output_direct(mHandle, evt);
//  	snd_seq_event_output(mHandle, evt);

// 	snd_seq_continue_queue(mHandle, mQueue, 0);
// 	snd_seq_drain_output(mHandle);

	return errNone;
}

int initMIDI(int numIn, int numOut)
{
	SC_AlsaMidiClient* client = &gMIDIClient;
	int i;

	if (client->mHandle) cleanUpMIDI();

	if(numIn > kMaxMidiPorts) { printf("MIDI: note that maximum midi in ports is limited to %d.\n", kMaxMidiPorts); }
	if(numOut > kMaxMidiPorts) { printf("MIDI: note that maximum midi out ports is limited to %d.\n", kMaxMidiPorts); }
	numIn = sc_clip(numIn, 1, kMaxMidiPorts);
	numOut = sc_clip(numOut, 1, kMaxMidiPorts);

	// initialize client handle
	if (snd_seq_open(&client->mHandle, "default", SND_SEQ_OPEN_DUPLEX, 0) < 0) {
		client->mHandle = 0;
		post("MIDI (ALSA): could not open ALSA sequencer: %s\n", snd_strerror(errno));
		return errFailed;
	}

	snd_seq_set_client_name(client->mHandle, "SuperCollider");

	// allocate i/o ports
	for (i=0; i < numIn; i++) {
		char str[32];
		int port;

		sprintf(str, "in%d", i);

		port = snd_seq_create_simple_port(
					client->mHandle, str,
					SND_SEQ_PORT_CAP_WRITE|SND_SEQ_PORT_CAP_SUBS_WRITE,
					SND_SEQ_PORT_TYPE_APPLICATION);

		if (port < 0) {
			post("MIDI (ALSA): could not create MIDI in port %d: %s\n", i, snd_strerror(errno));
			break;
		}

		client->mInPorts[i] = port;
	}

	client->mNumInPorts = i;

	for (i=0; i < numOut; i++) {
		char str[32];
		int port;

		sprintf(str, "out%d", i);

		port = snd_seq_create_simple_port(
					client->mHandle, str,
					SND_SEQ_PORT_CAP_READ|SND_SEQ_PORT_CAP_SUBS_READ,
					SND_SEQ_PORT_TYPE_APPLICATION);

		if (port < 0) {
			post("MIDI (ALSA): could not create MIDI out port %d: %s\n", i, snd_strerror(errno));
			break;
		}

		client->mOutPorts[i] = port;
	}

	client->mNumOutPorts = i;

	// initialize queue
	client->mQueue = snd_seq_alloc_queue(client->mHandle);
	snd_seq_start_queue(client->mHandle, client->mQueue, 0);
	snd_seq_drain_output(client->mHandle);
	// snd_seq_set_client_pool_output(seqHandle, ??);

	// initialize event en-/decoders
	if (snd_midi_event_new(32, &client->mEventToMidi) < 0) {
		client->mEventToMidi = 0;
		post("MIDI (ALSA): could not create MIDI decoder\n");
		return errFailed;
	}

	if (snd_midi_event_new(32, &client->mMidiToEvent) < 0) {
		client->mMidiToEvent = 0;
		post("MIDI (ALSA): could not create MIDI encoder\n");
		return errFailed;
	}

	snd_midi_event_no_status(client->mEventToMidi, 1);
	snd_midi_event_no_status(client->mMidiToEvent, 1);

	// start input thread
	return client->startInputThread();
}

int initMIDIClient()
{
	SC_AlsaMidiClient* client = &gMIDIClient;

	if (client->mHandle) return errNone;

	// initialize client handle
	if (snd_seq_open(&client->mHandle, "default", SND_SEQ_OPEN_DUPLEX, 0) < 0) {
		client->mHandle = 0;
		post("MIDI (ALSA): could not open ALSA sequencer: %s\n", snd_strerror(errno));
		return errFailed;
	}

	snd_seq_set_client_name(client->mHandle, "SuperCollider");
	client->mClientID = snd_seq_client_id( client->mHandle );


	// initialize queue
	client->mQueue = snd_seq_alloc_queue(client->mHandle);
	snd_seq_start_queue(client->mHandle, client->mQueue, 0);
	snd_seq_drain_output(client->mHandle);
	// snd_seq_set_client_pool_output(seqHandle, ??);

	// initialize event en-/decoders
	if (snd_midi_event_new(32, &client->mEventToMidi) < 0) {
		client->mEventToMidi = 0;
		post("MIDI (ALSA): could not create MIDI decoder\n");
		return errFailed;
	}

	if (snd_midi_event_new(32, &client->mMidiToEvent) < 0) {
		client->mMidiToEvent = 0;
		post("MIDI (ALSA): could not create MIDI encoder\n");
		return errFailed;
	}

	snd_midi_event_no_status(client->mEventToMidi, 1);
	snd_midi_event_no_status(client->mMidiToEvent, 1);

	return client->startInputThread();
}

int disposeMIDI()
{
	cleanUpMIDI();
	return errNone;
}

int restartMIDI()
{
	return errNone;
}

void cleanUpMIDI()
{
	SC_AlsaMidiClient* client = &gMIDIClient;

	if (client->mHandle) {
		client->joinInputThread();

		snd_seq_remove_events_t *revt;
		snd_seq_remove_events_malloc(&revt);
		snd_seq_remove_events_set_queue(revt, client->mQueue);
		snd_seq_remove_events_set_condition(revt, SND_SEQ_REMOVE_OUTPUT|SND_SEQ_REMOVE_IGNORE_OFF);
		snd_seq_remove_events(client->mHandle, revt);
		snd_seq_remove_events_free(revt);

		snd_seq_stop_queue(client->mHandle, client->mQueue, 0);
		snd_seq_free_queue(client->mHandle, client->mQueue);

		if (client->mEventToMidi) {
			snd_midi_event_free(client->mEventToMidi);
		}

		if (client->mMidiToEvent) {
			snd_midi_event_free(client->mMidiToEvent);
		}

		snd_seq_close(client->mHandle);
		client->mHandle = 0;
	}
}

inline static bool SC_AlsaCheckPerm(snd_seq_port_info_t* pinfo, int bits)
{
	int cap = snd_seq_port_info_get_capability(pinfo);
	return ((cap & bits) == bits) && !(cap & SND_SEQ_PORT_CAP_NO_EXPORT);
}

int listMIDIEndpoints(struct VMGlobals *g, PyrSlot* a)
{
	snd_seq_t* seq;
	snd_seq_client_info_t* cinfo;
	snd_seq_port_info_t* pinfo;

	if (!gMIDIClient.mHandle) return errFailed;

	seq = gMIDIClient.mHandle;

	snd_seq_client_info_alloca(&cinfo);
	snd_seq_port_info_alloca(&pinfo);
	snd_seq_client_info_set_client(cinfo, -1);

	std::vector<SC_AlsaMidiPort> srcPorts;
	std::vector<SC_AlsaMidiPort> dstPorts;

	while (snd_seq_query_next_client(seq, cinfo) >= 0) {
		int cid = snd_seq_client_info_get_client(cinfo);
		const char* cname = snd_seq_client_info_get_name(cinfo);

		if ((cid < 0) || (cid > 0xffff)) {
			post("MIDI (ALSA): client ID out of range.\n");
			return errFailed;
		}

		snd_seq_port_info_set_client(pinfo, cid);
		snd_seq_port_info_set_port(pinfo, -1);

		while (snd_seq_query_next_port(seq, pinfo) >= 0) {
			int pid = snd_seq_port_info_get_port(pinfo);
			const char* pname = snd_seq_port_info_get_name(pinfo);

			if ((pid < 0) || (pid > 0xffff)) {
				post("MIDI (ALSA): port ID out of range.\n");
				return errFailed;
			}

			if (SC_AlsaCheckPerm(pinfo, SND_SEQ_PORT_CAP_READ|SND_SEQ_PORT_CAP_SUBS_READ)) {
				// src port
				srcPorts.push_back(SC_AlsaMidiPort());
				snprintf(srcPorts.back().name, kAlsaMaxPortNameLen, "%s", pname);
				snprintf(srcPorts.back().device, kAlsaMaxPortNameLen, "%s", cname);
				srcPorts.back().uid = SC_AlsaMakeUID(cid, pid);
				//post("MIDI (ALSA): src %s-%s %d:%d %u\n", cname, pname, cid, pid, srcPorts.back().uid);
			}

			if (SC_AlsaCheckPerm(pinfo, SND_SEQ_PORT_CAP_WRITE|SND_SEQ_PORT_CAP_SUBS_WRITE)) {
				// dst port
				dstPorts.push_back(SC_AlsaMidiPort());
				snprintf(dstPorts.back().name, kAlsaMaxPortNameLen, "%s", pname);
				snprintf(dstPorts.back().device, kAlsaMaxPortNameLen, "%s", cname);
				dstPorts.back().uid = SC_AlsaMakeUID(cid, pid);
				//post("MIDI (ALSA): dst %s-%s %d:%d %u\n", cname, pname, cid, pid, srcPorts.back().uid);
			}
		}
	}

	int numSrc = srcPorts.size();
	int numDst = dstPorts.size();

	PyrObject* idarray = newPyrArray(g->gc, 6 * sizeof(PyrObject), 0 , true);
	SetObject(a, idarray);

	PyrObject* idarraySo = newPyrArray(g->gc, numSrc * sizeof(int32), 0 , true);
	SetObject(idarray->slots+idarray->size++, idarraySo);
	g->gc->GCWrite(idarray, idarraySo);

	PyrObject* devarraySo = newPyrArray(g->gc, numSrc * sizeof(PyrObject), 0 , true);
	SetObject(idarray->slots+idarray->size++, devarraySo);
	g->gc->GCWrite(idarray, devarraySo);

	PyrObject* namearraySo = newPyrArray(g->gc, numSrc * sizeof(PyrObject), 0 , true);
	SetObject(idarray->slots+idarray->size++, namearraySo);
	g->gc->GCWrite(idarray, namearraySo);

	PyrObject* idarrayDe = newPyrArray(g->gc, numDst * sizeof(int32), 0 , true);
	SetObject(idarray->slots+idarray->size++, idarrayDe);
	g->gc->GCWrite(idarray, idarrayDe);

	PyrObject* namearrayDe = newPyrArray(g->gc, numDst * sizeof(PyrObject), 0 , true);
	SetObject(idarray->slots+idarray->size++, namearrayDe);
	g->gc->GCWrite(idarray, namearrayDe);

	PyrObject* devarrayDe = newPyrArray(g->gc, numDst * sizeof(PyrObject), 0 , true);
	SetObject(idarray->slots+idarray->size++, devarrayDe);
	g->gc->GCWrite(idarray, devarrayDe);


	for (int i=0; i<numSrc; ++i) {
		char* name = srcPorts[i].name;
		char* devicename = srcPorts[i].device;

		PyrString *string = newPyrString(g->gc, name, 0, true);
		SetObject(namearraySo->slots+i, string);
		namearraySo->size++;
		g->gc->GCWrite(namearraySo, (PyrObject*)string);

		PyrString *devstring = newPyrString(g->gc, devicename, 0, true);
		SetObject(devarraySo->slots+i, devstring);
		devarraySo->size++;
		g->gc->GCWrite(devarraySo, (PyrObject*)devstring);

		SetInt(idarraySo->slots+i, srcPorts[i].uid);
		idarraySo->size++;
	}

	for (int i=0; i<numDst; ++i) {
		char* name = dstPorts[i].name;
		char* devicename = dstPorts[i].device;

		PyrString *string = newPyrString(g->gc, name, 0, true);
		SetObject(namearrayDe->slots+namearrayDe->size++, string);
		g->gc->GCWrite(namearrayDe, (PyrObject*)string);

		PyrString *devstring = newPyrString(g->gc, devicename, 0, true);
		SetObject(devarrayDe->slots+i, devstring);
		devarrayDe->size++;
		g->gc->GCWrite(devarrayDe, (PyrObject*)devstring);

		SetInt(idarrayDe->slots+i, dstPorts[i].uid);
		idarrayDe->size++;
	}

	return errNone;
}

int connectMIDIIn(int inputIndex, int uid)
{
	if (!gMIDIClient.mHandle) return errFailed;
	return gMIDIClient.connectInput(inputIndex, uid, &snd_seq_subscribe_port, "connect");
}

int disconnectMIDIIn(int inputIndex, int uid)
{
	if (!gMIDIClient.mHandle) return errFailed;
	return gMIDIClient.connectInput(inputIndex, uid, &snd_seq_unsubscribe_port, "disconnect");
}

int connectMIDIOut(int outputIndex, int uid)
{
	if (!gMIDIClient.mHandle) return errFailed;
	return gMIDIClient.connectOutput(outputIndex, uid, &snd_seq_subscribe_port, "connect");
}

int disconnectMIDIOut(int outputIndex, int uid)
{
	if (!gMIDIClient.mHandle) return errFailed;
	return gMIDIClient.connectOutput(outputIndex, uid, &snd_seq_unsubscribe_port, "disconnect");
}

int sendMIDI(int port, int uid, int length, int hiStatus, int loStatus, int aval, int bval, float late)
{
	if (!gMIDIClient.mHandle) return errFailed;

// 	post("MIDI (ALSA): send %x %x %d %d %i\n", hiStatus>>4, loStatus, aval, bval, gMIDIClient.mMidiToEvent);

	snd_seq_event_t evt;
	SC_AlsaMidiPacket pkt;

	snd_seq_ev_clear(&evt);
	pkt.data[0] = (hiStatus & 0xF0) | (loStatus & 0x0F);
	pkt.data[1] = (uint8)aval;
	pkt.data[2] = (uint8)bval;

	snd_midi_event_reset_encode(gMIDIClient.mMidiToEvent);

	if (snd_midi_event_encode(gMIDIClient.mMidiToEvent, pkt.data, length, &evt) < 0) {
		post("MIDI (ALSA): could not encode midi data: %s\n", snd_strerror(errno));
		return errFailed;
	}

	return gMIDIClient.sendEvent(port, uid, &evt, late);
}

int sendMIDISysex(int port, int uid, int length, uint8* data)
{
	if (!gMIDIClient.mHandle) return errFailed;
	snd_seq_event_t evt;
	evt.type = SND_SEQ_EVENT_SYSEX; // MIDIOut.sysex patch 2007-01-16
	snd_seq_ev_set_variable(&evt, length, data);
	return gMIDIClient.sendEvent(port, uid, &evt, 0.f);
}

// =====================================================================
// Primitives
// =====================================================================

int prInitMIDI(struct VMGlobals *g, int numArgsPushed);
int prInitMIDI(struct VMGlobals *g, int numArgsPushed)
{
	//PyrSlot *a = g->sp - 2;
	PyrSlot *b = g->sp - 1;
	PyrSlot *c = g->sp;

	int err, numIn, numOut;
	err = slotIntVal(b, &numIn);
	if (err) return errWrongType;

	err = slotIntVal(c, &numOut);
	if (err) return errWrongType;

	return initMIDI(numIn, numOut);
}

int prInitMIDIClient(struct VMGlobals *g, int numArgsPushed);
int prInitMIDIClient(struct VMGlobals *g, int numArgsPushed)
{
	return initMIDIClient();
}

int prDisposeMIDIClient(VMGlobals *g, int numArgsPushed);
int prDisposeMIDIClient(VMGlobals *g, int numArgsPushed)
{
	return disposeMIDI();
}

int prRestartMIDI(VMGlobals *g, int numArgsPushed);
int prRestartMIDI(VMGlobals *g, int numArgsPushed)
{
	return restartMIDI();
}

int prListMIDIEndpoints(struct VMGlobals *g, int numArgsPushed);
int prListMIDIEndpoints(struct VMGlobals *g, int numArgsPushed)
{
	return listMIDIEndpoints(g, g->sp);
}

int prConnectMIDIIn(struct VMGlobals *g, int numArgsPushed);
int prConnectMIDIIn(struct VMGlobals *g, int numArgsPushed)
{
	//PyrSlot *a = g->sp - 2;
	PyrSlot *b = g->sp - 1;
	PyrSlot *c = g->sp;

	int err, inputIndex, uid;
	err = slotIntVal(b, &inputIndex);
	if (err) return err;

	err = slotIntVal(c, &uid);
	if (err) return err;

	return connectMIDIIn(inputIndex, uid);
}

int prDisconnectMIDIIn(struct VMGlobals *g, int numArgsPushed);
int prDisconnectMIDIIn(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *b = g->sp - 1;
	PyrSlot *c = g->sp;

	int err, inputIndex, uid;
	err = slotIntVal(b, &inputIndex);
	if (err) return err;

	err = slotIntVal(c, &uid);
	if (err) return err;

	return disconnectMIDIIn(inputIndex, uid);
}

int prConnectMIDIOut(struct VMGlobals *g, int numArgsPushed);
int prConnectMIDIOut(struct VMGlobals *g, int numArgsPushed)
{
	//PyrSlot *a = g->sp - 2;
	PyrSlot *b = g->sp - 1;
	PyrSlot *c = g->sp;

	int err, inputIndex, uid;
	err = slotIntVal(b, &inputIndex);
	if (err) return err;

	err = slotIntVal(c, &uid);
	if (err) return err;

	return connectMIDIOut(inputIndex, uid);
}

int prDisconnectMIDIOut(struct VMGlobals *g, int numArgsPushed);
int prDisconnectMIDIOut(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *b = g->sp - 1;
	PyrSlot *c = g->sp;

	int err, inputIndex, uid;
	err = slotIntVal(b, &inputIndex);
	if (err) return err;

	err = slotIntVal(c, &uid);
	if (err) return err;

	return disconnectMIDIOut(inputIndex, uid);
}

int prSendMIDIOut(struct VMGlobals *g, int numArgsPushed);
int prSendMIDIOut(struct VMGlobals *g, int numArgsPushed)
{
	//port, uid, len, hiStatus, loStatus, a, b, latency
	//PyrSlot *m = g->sp - 8;
	PyrSlot *p = g->sp - 7;

	PyrSlot *u = g->sp - 6;
	PyrSlot *l = g->sp - 5;

	PyrSlot *his = g->sp - 4;
	PyrSlot *los = g->sp - 3;

	PyrSlot *a = g->sp - 2;
	PyrSlot *b = g->sp - 1;
	PyrSlot *plat = g->sp;

	int err, outputIndex, uid, length, hiStatus, loStatus, aval, bval;
	float late;
	err = slotIntVal(p, &outputIndex);
	if (err) return err;

	err = slotIntVal(u, &uid);
	if (err) return err;

	err = slotIntVal(l, &length);
	if (err) return err;

	err = slotIntVal(his, &hiStatus);
	if (err) return err;

	err = slotIntVal(los, &loStatus);
	if (err) return err;

	err = slotIntVal(a, &aval);
	if (err) return err;

	err = slotIntVal(b, &bval);
	if (err) return err;

	err = slotFloatVal(plat, &late);
	if (err) return err;

	return sendMIDI(outputIndex, uid, length, hiStatus, loStatus, aval, bval, late);
}

int prSendSysex(VMGlobals *g, int numArgsPushed)
{
	int err, uid, outputIndex;
	PyrInt8Array* packet;

	// rcvr, uid, packet
	PyrSlot* args = g->sp - 2;

	int MIDIOut_port_index = instVarOffset("MIDIOut", "port");

	err = slotIntVal(slotRawObject(args)->slots + MIDIOut_port_index, &outputIndex);
	if (err) return err;

	err = slotIntVal(args+1, &uid);
	if (err) return err;

	if( !isKindOfSlot(args+2, s_int8array->u.classobj) )
		return errWrongType;

	packet = slotRawInt8Array(&args[2]);

	return sendMIDISysex(outputIndex, uid, packet->size, packet->b);
}

int prGetMIDIClientID(VMGlobals *g, int numArgsPushed)
{

	PyrSlot* args = g->sp;
	if (!gMIDIClient.mHandle) return errFailed;


	SetInt(args, gMIDIClient.mClientID );

	return errNone;
}

void initMIDIPrimitives()
{
	int base, index;

	base = nextPrimitiveIndex();
	index = 0;

	s_midiin = getsym("MIDIIn");

	s_domidiaction = getsym("doAction");
	s_midiNoteOnAction = getsym("doNoteOnAction");
	s_midiNoteOffAction = getsym("doNoteOffAction");
	s_midiTouchAction = getsym("doTouchAction");
	s_midiControlAction = getsym("doControlAction");
	s_midiPolyTouchAction = getsym("doPolyTouchAction");
	s_midiProgramAction = getsym("doProgramAction");
	s_midiBendAction = getsym("doBendAction");
	s_midiSysexAction = getsym("doSysexAction");
	s_midiSysrtAction = getsym("doSysrtAction");
	s_midiSMPTEAction = getsym("doSMPTEaction");

	definePrimitive(base, index++, "_InitMIDI", prInitMIDI, 3, 0);
	definePrimitive(base, index++, "_InitMIDIClient", prInitMIDIClient, 1, 0);
	definePrimitive(base, index++, "_RestartMIDI", prRestartMIDI, 1, 0);
	definePrimitive(base, index++, "_DisposeMIDIClient", prDisposeMIDIClient, 1, 0);

	definePrimitive(base, index++, "_ListMIDIEndpoints", prListMIDIEndpoints, 1, 0);
	definePrimitive(base, index++, "_ConnectMIDIIn", prConnectMIDIIn, 3, 0);
	definePrimitive(base, index++, "_DisconnectMIDIIn", prDisconnectMIDIIn, 3, 0);
	definePrimitive(base, index++, "_ConnectMIDIOut", prConnectMIDIOut, 3, 0);
	definePrimitive(base, index++, "_DisconnectMIDIOut", prDisconnectMIDIOut, 3, 0);

	definePrimitive(base, index++, "_SendMIDIOut", prSendMIDIOut, 9, 0);
	definePrimitive(base, index++, "_SendSysex", prSendSysex, 3, 0); // MIDIOut.sysex patch 2007-01-16
	
	definePrimitive(base, index++, "_GetMIDIClientID", prGetMIDIClientID, 1, 0);

	cleanUpMIDI();
}

void deinitMIDIPrimitives()
{
	cleanUpMIDI();
}
// EOF
