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

/*
changes by jan trutzschler v. f. 9/9/2002
the midiReadProc calls doAction in the class MIDIIn.
with the arguments: inUid, status, chan, val1, val2
added prDisposeMIDIClient
added prRestartMIDI
19/9 call different actions,disconnect midiInPort, midiout: sendmidi
04/feb/03 prListMIDIEndpoints modification by Ron Kuivila added jt.
*/

#include "PortMIDI.h"
#include "PortTime.h"

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
#include "SC_Lock.h"
#include <map>
#include <cstring>
#include <vector>

using std::memset;

// symbols to call back into lang
PyrSymbol* s_domidiaction;
PyrSymbol* s_midiNoteOnAction;
PyrSymbol* s_midiNoteOffAction;
PyrSymbol* s_midiTouchAction;
PyrSymbol* s_midiControlAction;
PyrSymbol* s_midiPolyTouchAction;
PyrSymbol* s_midiProgramAction;
PyrSymbol* s_midiBendAction;
PyrSymbol* s_midiSysexAction;
PyrSymbol* s_midiInvalidSysexAction;
PyrSymbol* s_midiSysrtAction;
PyrSymbol* s_midiSMPTEAction;
PyrSymbol* s_midiin;
PyrSymbol* s_numMIDIDev;
PyrSymbol* s_midiclient;

const int kMaxMidiPorts = 128;
int gNumMIDIInPorts = 0, gNumMIDIOutPorts = 0;
bool gMIDIInitialized = false;

static std::vector<uint8_t> gSysexData;
static bool gSysexFlag = false;
static uint8_t gRunningStatus = 0;


PmStream* gMIDIInStreams[kMaxMidiPorts];
bool gMIDIInStreamUsed[kMaxMidiPorts];
PmStream* gMIDIOutStreams[kMaxMidiPorts];

std::map<int, int> gMidiInputIndexToPmDevIndex;
std::map<int, int> gMidiOutputIndexToPmDevIndex;
SC_Lock gPmStreamMutex;

/* if INPUT_BUFFER_SIZE is 0, PortMidi uses a default value */
#define PMSTREAM_INPUT_BUFFER_SIZE 0
#define PMSTREAM_OUTPUT_BUFFER_SIZE 100
#define PMSTREAM_DRIVER_INFO NULL
#define PMSTREAM_TIME_PROC NULL
#define PMSTREAM_TIME_INFO NULL

extern bool compiledOK;


static void sysexBegin() {
    gRunningStatus = 0; // clear running status
    gSysexData.clear();
    gSysexFlag = true;
}

static void scCallSysexAction(PyrSymbol* action) {
    VMGlobals* g = gMainVMGlobals;
    uint8_t* pSysexData = &gSysexData[0]; // Convert to array access
    PyrInt8Array* sysexArray = newPyrInt8Array(g->gc, gSysexData.size(), 0, true);
    sysexArray->size = gSysexData.size();
    memcpy(sysexArray->b, pSysexData, gSysexData.size());
    ++g->sp;
    SetObject(g->sp, (PyrObject*)sysexArray); // chan argument unneeded as there
    runInterpreter(g, action, 3); // special sysex action in the lang
}

static void sysexEnd() {
    gSysexFlag = false;
    scCallSysexAction(s_midiSysexAction);
}

static void sysexEndInvalid() {
    gSysexFlag = false;
    scCallSysexAction(s_midiInvalidSysexAction);
}


static int midiProcessPartialSystemPacket(uint32_t msg) {
    int index, data;
    VMGlobals* g = gMainVMGlobals;
    // to iterate over the contents of the message byte by byte
    uint8_t* p = reinterpret_cast<uint8_t*>(&msg);

    int i = 0;
    while (i < 4) {
        switch (p[i]) {
        /* SysEx start */
        case 0xF0:
            if (gSysexFlag) {
                sysexEndInvalid();
            }
            sysexBegin(); // new sysex in
            gSysexData.push_back(p[i]); // add SOX
            i++;
            break;


        /* SysEx end */
        case 0xF7:
            gSysexData.push_back(p[i]); // add EOX
            if (gSysexFlag)
                sysexEnd(); // if last_uid != 0 rebuild the VM.
            else
                sysexEndInvalid(); // invalid 1 byte with only EOX can happen
            return 0;

        /* MIDI clock. Can only be received as the first byte */
        case 0xF1:
            index = p[i + 1] >> 4;
            data = p[i + 1] & 0xf;
            if (index % 2) {
                data = data << 4;
            }
            SetInt(g->sp, index); // chan unneeded
            ++g->sp;
            SetInt(g->sp, data); // special smpte action in the lang
            runInterpreter(g, s_midiSMPTEAction, 4);
            return 0;

        /* Song pointer. Can only be received as the first byte */
        case 0xF2:
            ++g->sp;
            SetInt(g->sp, (p[i + 2] << 7) | p[i + 1]);
            runInterpreter(g, s_midiSysrtAction, 4);
            return 0;

        /* Song select. Can only be received as the first byte */
        case 0xF3:
            ++g->sp;
            SetInt(g->sp, p[i + 1]);
            runInterpreter(g, s_midiSysrtAction, 4);
            return 0;

        /* Realtime messages. Can be anywhere within a message */
        case 0xF6:
        case 0xF8:
        case 0xF9:
        case 0xFA:
        case 0xFB:
        case 0xFC:
        case 0xFE:
            gRunningStatus = 0; // clear running status
            ++g->sp;
            SetInt(g->sp, p[i] & 0xF);
            ++g->sp;
            SetInt(g->sp, 0);
            runInterpreter(g, s_midiSysrtAction, 4);
            i++;
            break;

        default: // This should be data
            if (p[i] & 0x80) { // if it a command byte, it is an error
                gSysexData.push_back(p[i]); // add it as an abort message
                sysexEndInvalid(); // flush invalid
                return 0;
            } else if (gSysexFlag) {
                gSysexData.push_back(p[i]); // add Byte
                i++;
            } else { // garbage - handle in case - discard it
                i++;
                break;
            }
        }
    }

    return 0;
}


/* timer "interrupt" for processing midi data */
static void PMProcessMidi(PtTimestamp timestamp, void* userData) {
    PmEvent buffer; /* just one message at a time */

    for (int i = 0; i < gNumMIDIInPorts; ++i) {
        for (;;) {
            long Tstatus, data1, data2;
            // Only lock the PM mutex while accessing the PortMidi functionality. It is very important to not acquire
            // the lang mutex while holding the PM mutex to avoid a deadlock, since the laguage may try to acquire the
            // PM mutex for a MIDIOut operation
            {
                lock_guard<SC_Lock> mulo(gPmStreamMutex);
                PmStream* midi_in = gMIDIInStreams[i];
                if (!midi_in) {
                    break;
                }

                const PmError result = Pm_Poll(midi_in);
                if (result == pmNoData) {
                    break;
                } else if (result != pmGotData) {
                    std::printf("errrr %s\n", Pm_GetErrorText(result));
                    continue;
                }

                if (Pm_Read(midi_in, &buffer, 1) == pmBufferOverflow)
                    continue;
                // unless there was overflow, we should have a message now

                Tstatus = Pm_MessageStatus(buffer.message);
                // The PM mutex is released here
            }
            // +---------------------------------------------+
            // | Lock the interp. mutex and dispatch message |
            // +---------------------------------------------+
            gLangMutex.lock();
            if (compiledOK) {
                VMGlobals* g = gMainVMGlobals;
                uint8 status = static_cast<uint8>(Tstatus & 0xF0);
                uint8 chan = static_cast<uint8>(Tstatus & 0x0F);

                g->canCallOS = false; // cannot call the OS

                // Do not put the data in the stack again if we are in the middle of processing a System Message,
                if (!gSysexFlag) {
                    ++g->sp;
                    SetObject(g->sp, s_midiin->u.classobj); // Set the class MIDIIn
                    ++g->sp;
                    SetInt(g->sp, i); // src
                }

                if (status & 0x80) // set the running status for voice messages
                    gRunningStatus = ((status >> 4) == 0xF) ? 0 : Tstatus; // keep also additional info

            L:
                switch (status) {
                case 0x80: // noteOff
                    ++g->sp;
                    SetInt(g->sp, chan); // chan
                    ++g->sp;
                    SetInt(g->sp, Pm_MessageData1(buffer.message));
                    ++g->sp;
                    SetInt(g->sp, Pm_MessageData2(buffer.message));
                    runInterpreter(g, s_midiNoteOffAction, 5);
                    break;
                case 0x90: // noteOn
                    ++g->sp;
                    SetInt(g->sp, chan); // chan
                    ++g->sp;
                    SetInt(g->sp, Pm_MessageData1(buffer.message));
                    ++g->sp;
                    SetInt(g->sp, Pm_MessageData2(buffer.message));
                    // 						runInterpreter(g, data2 ? s_midiNoteOnAction : s_midiNoteOffAction, 5);
                    runInterpreter(g, s_midiNoteOnAction, 5);
                    break;
                case 0xA0: // polytouch
                    ++g->sp;
                    SetInt(g->sp, chan); // chan
                    ++g->sp;
                    SetInt(g->sp, Pm_MessageData1(buffer.message));
                    ++g->sp;
                    SetInt(g->sp, Pm_MessageData2(buffer.message));
                    runInterpreter(g, s_midiPolyTouchAction, 5);
                    break;
                case 0xB0: // control
                    ++g->sp;
                    SetInt(g->sp, chan); // chan
                    ++g->sp;
                    SetInt(g->sp, Pm_MessageData1(buffer.message));
                    ++g->sp;
                    SetInt(g->sp, Pm_MessageData2(buffer.message));
                    runInterpreter(g, s_midiControlAction, 5);
                    break;
                case 0xC0: // program
                    ++g->sp;
                    SetInt(g->sp, chan); // chan
                    ++g->sp;
                    SetInt(g->sp, Pm_MessageData1(buffer.message));
                    runInterpreter(g, s_midiProgramAction, 4);
                    break;
                case 0xD0: // touch
                    ++g->sp;
                    SetInt(g->sp, chan); // chan
                    ++g->sp;
                    SetInt(g->sp, Pm_MessageData1(buffer.message));
                    runInterpreter(g, s_midiTouchAction, 4);
                    break;
                case 0xE0: // bend
                    ++g->sp;
                    SetInt(g->sp, chan); // chan
                    ++g->sp;
                    SetInt(g->sp, (Pm_MessageData2(buffer.message) << 7) | Pm_MessageData1(buffer.message));
                    runInterpreter(g, s_midiBendAction, 4);
                    break;
                case 0xF0:
                    // only the first Pm_Event will carry the 0xF0 byte
                    // we pass the messages to the processing function, which will take care of assembling the,
                    midiProcessPartialSystemPacket(buffer.message);
                    break;

                default: // data byte => continuing sysex message ir running status
                    if (gRunningStatus && !gSysexFlag) { // handle running status
                        status = gRunningStatus & 0xF0; // accept running status only inside a packet beginning
                        chan = gRunningStatus & 0x0F; // with a valid status byte
                        SetInt(g->sp, chan);
                        goto L; // parse again with running status set // mv - get next byte
                    }
                    if (gSysexFlag) {
                        midiProcessPartialSystemPacket(buffer.message); // process sysex packet
                    }
                    break;
                }
                g->canCallOS = false;
            }
            gLangMutex.unlock();
        } // if (midi_in)
    } // for loop until numMIDIInPorts
}

/*
-------------------------------------------------------------
*/
static void midiCleanUp();
static int initMIDI(int numIn, int numOut) {
    midiCleanUp();

    lock_guard<SC_Lock> mulo(gPmStreamMutex);
    const PmError initializationError = Pm_Initialize();
    if (initializationError) {
        std::printf("MIDI: cannot open midi backend %s\n", Pm_GetErrorText(initializationError));
        return errFailed;
    }

    /* Here, numIn and numOut are 0, even if the inputs to lang (MIDIClient init) were nil, but according to the
       documentation, it should be the number of inputs or outputs. That matches what I see in MIDIOut.sc -> MIDIClient
       *init, in which it is setting that to sources.size, and destinations.size, so I guess that the problem is that
       this information is not known at this point, or there is something missing. */
    if (numIn > kMaxMidiPorts) {
        std::printf("MIDI: note that maximum midi in ports is limited to %i.\n", kMaxMidiPorts);
    }
    if (numOut > kMaxMidiPorts) {
        std::printf("MIDI: note that maximum midi out ports is limited to %i.\n", kMaxMidiPorts);
    }
    numIn = sc_clip(numIn, 1, kMaxMidiPorts);
    numOut = sc_clip(numOut, 1, kMaxMidiPorts);

    int inIndex = 0;
    int outIndex = 0;

    for (int i = 0; i < Pm_CountDevices(); ++i) {
        const PmDeviceInfo* devInfo = Pm_GetDeviceInfo(i);
        if (devInfo->input) {
            gNumMIDIInPorts++;
            gMidiInputIndexToPmDevIndex[inIndex++] = i;
        }

        /* Ignore the MIDI Mapper. Otherwise, if it is allocated, we get "already allocated" errors */
        if ((devInfo->output) && (strnicmp(devInfo->name, "Microsoft MIDI Mapper", 50) != 0)) {
            gNumMIDIOutPorts++;
            gMidiOutputIndexToPmDevIndex[outIndex++] = i;
        }
    }

    if (gNumMIDIInPorts > numIn)
        gNumMIDIInPorts = numIn;

    if (gNumMIDIOutPorts > numOut)
        gNumMIDIOutPorts = numOut;

    for (int i = 0; i < gNumMIDIOutPorts; i++) {
        const int pmdid = gMidiOutputIndexToPmDevIndex[i];
        const PmDeviceInfo* devInfo = Pm_GetDeviceInfo(pmdid);
        const PmError error = Pm_OpenOutput(&gMIDIOutStreams[i], pmdid, NULL, 512L, NULL, NULL, 0);

        std::printf("MIDI: device %d %d %d %s (%s)\n", i, pmdid, &gMIDIOutStreams[i], Pm_GetErrorText(error),
                    devInfo->name);

        if (error) {
            std::printf("MIDI: cannot open device %d %d %s (%s)\n", i, pmdid, Pm_GetErrorText(error), devInfo->name);

            int hostError;
            if ((hostError = Pm_HasHostError(nullptr))) {
                char hostErrorString[PM_HOST_ERROR_MSG_LEN];
                Pm_GetHostErrorText(hostErrorString, PM_HOST_ERROR_MSG_LEN);
                std::printf("MIDI: Host error %s\n", hostErrorString);
            }
        }
    }

    /* will call our function, PMProcessMidi() every millisecond */
    Pt_Start(1, &PMProcessMidi, 0); /* start a timer with millisecond accuracy */

    gMIDIInitialized = true;
    return errNone;
}


static int getNumSources() {
    int n = 0;
    for (int i = 0; i < Pm_CountDevices(); ++i) {
        const PmDeviceInfo* devInfo = Pm_GetDeviceInfo(i);
        if (devInfo->input) {
            n++;
        }
    }
    return n;
}

static int getNumDestinations() {
    int n = 0;
    for (int i = 0; i < Pm_CountDevices(); ++i) {
        const PmDeviceInfo* devInfo = Pm_GetDeviceInfo(i);
        if ((devInfo->output) && (strnicmp(devInfo->name, "Microsoft MIDI Mapper", 50) != 0)) {
            n++;
        }
    }
    return n;
}


/*
-------------------------------------------------------------
*/
static void midiCleanUp() {
    lock_guard<SC_Lock> mulo(gPmStreamMutex);

    if (gMIDIInitialized) {
        for (int i = 0; i < gNumMIDIOutPorts; ++i) {
            Pm_Abort(gMIDIOutStreams[i]);
            Pm_Close(gMIDIOutStreams[i]);
        }
        for (int i = 0; i < gNumMIDIInPorts; ++i) {
            if (gMIDIInStreamUsed[i]) {
                Pm_Abort(gMIDIInStreams[i]);
                Pm_Close(gMIDIInStreams[i]);
            }
        }

        gNumMIDIOutPorts = 0;
        gNumMIDIInPorts = 0;
    }

    // set the stream pointers to NULL
    memset(gMIDIInStreams, 0, kMaxMidiPorts * sizeof(PmStream*));
    memset(gMIDIOutStreams, 0, kMaxMidiPorts * sizeof(PmStream*));

    // Mark the MIDI ins as not used
    for (int i = 0; i < kMaxMidiPorts; i++) {
        gMIDIInStreamUsed[i] = false;
    }


    // delete the objects that map in/out indices to Pm dev indices
    gMidiInputIndexToPmDevIndex.clear();
    gMidiOutputIndexToPmDevIndex.clear();

    gMIDIInitialized = false;
}
/*
-------------------------------------------------------------
*/
void midiListEndpoints() {}
/*
-------------------------------------------------------------
*/
int prListMIDIEndpoints(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* a = g->sp;
    int numSrc = getNumSources();
    int numDst = getNumDestinations();

    PyrObject* idarray = newPyrArray(g->gc, 6 * sizeof(PyrObject), 0, true);
    SetObject(a, idarray);

    // 0
    PyrObject* idarraySo = newPyrArray(g->gc, numSrc * sizeof(__int32), 0, true);
    SetObject(idarray->slots + idarray->size++, idarraySo);
    g->gc->GCWrite(idarray, idarraySo);

    // 1
    PyrObject* devarraySo = newPyrArray(g->gc, numSrc * sizeof(PyrObject), 0, true);
    SetObject(idarray->slots + idarray->size++, devarraySo);
    g->gc->GCWrite(idarray, devarraySo);

    // 2
    PyrObject* namearraySo = newPyrArray(g->gc, numSrc * sizeof(PyrObject), 0, true);
    SetObject(idarray->slots + idarray->size++, namearraySo);
    g->gc->GCWrite(idarray, namearraySo);

    // 3
    PyrObject* idarrayDe = newPyrArray(g->gc, numDst * sizeof(__int32), 0, true);
    SetObject(idarray->slots + idarray->size++, idarrayDe);
    g->gc->GCWrite(idarray, idarrayDe);

    // 4
    PyrObject* namearrayDe = newPyrArray(g->gc, numDst * sizeof(PyrObject), 0, true);
    SetObject(idarray->slots + idarray->size++, namearrayDe);
    g->gc->GCWrite(idarray, namearrayDe);

    // 5
    PyrObject* devarrayDe = newPyrArray(g->gc, numDst * sizeof(PyrObject), 0, true);
    SetObject(idarray->slots + idarray->size++, devarrayDe);
    g->gc->GCWrite(idarray, devarrayDe);

    for (int i = 0; i < numSrc; ++i) {
        const PmDeviceInfo* devInfo = Pm_GetDeviceInfo(gMidiInputIndexToPmDevIndex[i]);

        char cendname[1024], cdevname[1024];

        // currently, copy both name strings in endpoint name and dev name
        strncpy(cendname, devInfo->name, 1023);
        cendname[1023] = 0;
        strncpy(cdevname, devInfo->name, 1023);
        cdevname[1023] = 0;

        PyrString* string = newPyrString(g->gc, cendname, 0, true);
        SetObject(namearraySo->slots + i, string);
        namearraySo->size++;
        g->gc->GCWrite(namearraySo, (PyrObject*)string);

        PyrString* devstring = newPyrString(g->gc, cdevname, 0, true);
        SetObject(devarraySo->slots + i, devstring);
        devarraySo->size++;
        g->gc->GCWrite(devarraySo, (PyrObject*)devstring);

        SetInt(idarraySo->slots + i, i);
        idarraySo->size++;
    }

    //      post("numDst %d\n",  numDst);
    for (int i = 0; i < numDst; ++i) {
        const PmDeviceInfo* devInfo = Pm_GetDeviceInfo(gMidiOutputIndexToPmDevIndex[i]);
        char cendname[1024], cdevname[1024];

        // currently, copy both name strings in endpoint name and dev name
        strncpy(cendname, devInfo->name, 1023);
        cendname[1023] = 0;
        strncpy(cdevname, devInfo->name, 1023);
        cdevname[1023] = 0;

        PyrString* string = newPyrString(g->gc, cendname, 0, true);
        SetObject(namearrayDe->slots + namearrayDe->size++, string);
        g->gc->GCWrite(namearrayDe, (PyrObject*)string);

        PyrString* devstring = newPyrString(g->gc, cdevname, 0, true);

        SetObject(devarrayDe->slots + devarrayDe->size++, devstring);
        g->gc->GCWrite(devarrayDe, (PyrObject*)devstring);

        SetInt(idarrayDe->slots + idarrayDe->size++, i);
    }
    return errNone;
}

/*
-------------------------------------------------------------
*/

int prConnectMIDIIn(struct VMGlobals* g, int numArgsPushed) {
    lock_guard<SC_Lock> mulo(gPmStreamMutex);

    // PyrSlot *a = g->sp - 2;
    PyrSlot* b = g->sp - 1;
    PyrSlot* c = g->sp;

    int err, inputIndex, uid;
    err = slotIntVal(b, &inputIndex);
    if (err)
        return errWrongType;
    if (inputIndex < 0 || inputIndex >= gNumMIDIInPorts)
        return errIndexOutOfRange;

    err = slotIntVal(c, &uid);
    if (err)
        return errWrongType;

    PmStream* inStream = NULL;
    int pmdid = gMidiInputIndexToPmDevIndex[uid];

    const PmError error = Pm_OpenInput(&inStream, pmdid, PMSTREAM_DRIVER_INFO, PMSTREAM_INPUT_BUFFER_SIZE,
                                       PMSTREAM_TIME_PROC, PMSTREAM_TIME_INFO);

    if (error) {
        std::printf("cannot open MIDI input: %s\n", Pm_GetErrorText(error));

        int hostError;
        if ((hostError = Pm_HasHostError(nullptr))) {
            char hostErrorString[PM_HOST_ERROR_MSG_LEN];
            Pm_GetHostErrorText(hostErrorString, PM_HOST_ERROR_MSG_LEN);
            std::printf("MIDI: Host error %s\n", hostErrorString);
        }

        return errFailed;
    }

    gMIDIInStreams[uid] = inStream;
    gMIDIInStreamUsed[uid] = true;

    return errNone;
}
/*
-------------------------------------------------------------
*/
int prDisconnectMIDIIn(struct VMGlobals* g, int numArgsPushed) {
    lock_guard<SC_Lock> mulo(gPmStreamMutex);

    PyrSlot* b = g->sp - 1;
    PyrSlot* c = g->sp;

    int err, inputIndex, uid;
    err = slotIntVal(b, &inputIndex);
    if (err)
        return err;
    if (inputIndex < 0 || inputIndex >= gNumMIDIInPorts)
        return errIndexOutOfRange;
    err = slotIntVal(c, &uid);
    if (err)
        return err;

    if (gMIDIInStreamUsed[uid]) {
        const PmError error = Pm_Close(gMIDIInStreams[uid]);

        if (error) {
            std::printf("cannot close MIDI device: %s\n", Pm_GetErrorText(error));
            return errFailed;
        }

        gMIDIInStreamUsed[uid] = false;
        gMIDIInStreams[uid] = NULL;
    }
    return errNone;
}
/*
-------------------------------------------------------------
*/
int prInitMIDI(struct VMGlobals* g, int numArgsPushed) {
    // PyrSlot *a = g->sp - 2;
    PyrSlot* b = g->sp - 1;
    PyrSlot* c = g->sp;

    int err, numIn, numOut;
    err = slotIntVal(b, &numIn);
    if (err)
        return errWrongType;

    err = slotIntVal(c, &numOut);
    if (err)
        return errWrongType;

    return initMIDI(numIn, numOut);
}

int prDisposeMIDIClient(VMGlobals* g, int numArgsPushed) {
    midiCleanUp();
    return errNone;
}

int prRestartMIDI(VMGlobals* g, int numArgsPushed) {
    /* Do nothing */
    return errNone;
}

/*
void freeSysex(MIDISysexSendRequest* pk)
{
free(pk->data);
free(pk);
}
*/

int prSendSysex(VMGlobals* g, int numArgsPushed) {
    lock_guard<SC_Lock> mulo(gPmStreamMutex);
    int err, uid;

    PyrSlot* args = g->sp - 2;

    // args[1] contains the uid and args[2] contains the array
    err = slotIntVal(&args[1], &uid);
    if (err)
        return err;

    if (!isKindOfSlot(&args[2], s_int8array->u.classobj))
        return errWrongType;

    PyrInt8Array* packet = slotRawInt8Array(&args[2]);

    if (gMIDIOutStreams[uid]) {
        PmError result = Pm_WriteSysEx(gMIDIOutStreams[uid], 0, packet->b);
        if (result != pmNoError)
            return errFailed;
        else
            return errNone;
    }
    return errFailed;
}
/*
-------------------------------------------------------------
*/
int prSendMIDIOut(struct VMGlobals* g, int numArgsPushed) {
    lock_guard<SC_Lock> mulo(gPmStreamMutex);
    // port, uid, len, hiStatus, loStatus, a, b, latency
    // PyrSlot *m = g->sp - 8;
    PyrSlot* p = g->sp - 7;

    PyrSlot* u = g->sp - 6;
    PyrSlot* l = g->sp - 5;

    PyrSlot* his = g->sp - 4;
    PyrSlot* los = g->sp - 3;

    PyrSlot* a = g->sp - 2;
    PyrSlot* b = g->sp - 1;
    PyrSlot* plat = g->sp;

    int err, outputIndex, uid, length, hiStatus, loStatus, aval, bval;
    float late;
    err = slotIntVal(p, &outputIndex);
    if (err)
        return err;
    if (outputIndex < 0 || outputIndex >= gNumMIDIOutPorts)
        return errIndexOutOfRange;

    err = slotIntVal(u, &uid);
    if (err)
        return err;
    err = slotIntVal(l, &length);
    if (err)
        return err;
    err = slotIntVal(his, &hiStatus);
    if (err)
        return err;
    err = slotIntVal(los, &loStatus);
    if (err)
        return err;
    err = slotIntVal(a, &aval);
    if (err)
        return err;
    err = slotIntVal(b, &bval);
    if (err)
        return err;
    err = slotFloatVal(plat, &late);
    if (err)
        return err;

    Pm_WriteShort(gMIDIOutStreams[uid], 0, Pm_Message((hiStatus & 0xF0) | (loStatus & 0x0F), aval, bval));

    return errNone;
}

// not needed in PortMIDI:
int initMIDIClient() { return errNone; }

int prInitMIDIClient(struct VMGlobals* g, int numArgsPushed) { return initMIDIClient(); }

void initMIDIPrimitives() {
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
    s_midiInvalidSysexAction = getsym("doInvalidSysexAction"); // client can handle incorrect case
    s_midiSysrtAction = getsym("doSysrtAction");
    s_midiSMPTEAction = getsym("doSMPTEaction");
    s_numMIDIDev = getsym("prSetNumberOfDevices");
    s_midiclient = getsym("MIDIClient");
    definePrimitive(base, index++, "_ListMIDIEndpoints", prListMIDIEndpoints, 1, 0);
    definePrimitive(base, index++, "_InitMIDI", prInitMIDI, 3, 0);
    definePrimitive(base, index++, "_InitMIDIClient", prInitMIDIClient, 1, 0);
    definePrimitive(base, index++, "_ConnectMIDIIn", prConnectMIDIIn, 3, 0);
    definePrimitive(base, index++, "_DisconnectMIDIIn", prDisconnectMIDIIn, 3, 0);
    definePrimitive(base, index++, "_DisposeMIDIClient", prDisposeMIDIClient, 1, 0);
    definePrimitive(base, index++, "_RestartMIDI", prRestartMIDI, 1, 0);
    definePrimitive(base, index++, "_SendMIDIOut", prSendMIDIOut, 9, 0);
    definePrimitive(base, index++, "_SendSysex", prSendSysex, 3, 0);
    midiCleanUp();
}

void deinitMIDIPrimitives() { midiCleanUp(); }
