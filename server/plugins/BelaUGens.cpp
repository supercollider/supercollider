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
 *  BELA I/O UGens created by nescivi, (c) 2016
 *  https://www.nescivi.eu
 */

#include "Bela.h"
#include "cobalt/stdio.h"

#include "SC_PlugIn.h"

class AccessBuffer {
protected:
    AccessBuffer(float* buffer, unsigned int count): m_buffer(buffer), m_count(count), m_last(buffer[count - 1]) {}
    const float& at(unsigned int n) const {
        if (n < m_count)
            return m_buffer[n];
        else
            return m_last;
    }
    float& at(unsigned int n) {
        if (n < m_count)
            return m_buffer[n];
        else
            return m_last;
    }
    void updateBufferWithLast() { m_buffer[m_count - 1] = m_last; }
    unsigned int count() const { return m_count; }

private:
    float* m_buffer;
    float m_last;
    const unsigned int m_count;
};

// Two buffer views which on which you can call [] for arbitrarily large numbers, but
// if it exceeds count, you will get back the value it had at [count-1] at
// initialisation.
// Additionally, upon destruction of an AccessBufferWriter object, the
// [count-1] element wil be replaced with the cached value, which may have been
// overwritten when assigning the returned value. This is done heuristically.
// These classes are useful when dealing with Sc buffers transparently without
// worrying whether they are at audio or control rate.
// It is also safe against overlapping buffers, as long as:
// - only one object writes to the buffer
// - you write to elements incrementally (starting from 0)
// - no one tries to access the buffer if not with one of the Writer or Reader
// objects, as long as they are alive
// - for each element, you write after reading (note: the way this provides safety is
// that you can keep calling [n] with n >= count and you will still access the
// valid cached value and not whathever the writer may have written to it
// since)

class AccessBufferWriter : public AccessBuffer {
public:
    AccessBufferWriter(float* buffer, unsigned int count): AccessBuffer(buffer, count), m_lastTouched(false) {};
    ~AccessBufferWriter() {
        // calling [], you may have been passed a reference to last instead of
        // a pointer into the buffer itself. Here, we ensure we put it back into the buffer
        if (m_lastTouched)
            updateBufferWithLast();
    }
    float& operator[](unsigned int n) {
        if (n >= count())
            m_lastTouched = true;
        return at(n);
    }

private:
    bool m_lastTouched;
};

class AccessBufferReader : public AccessBuffer {
public:
    AccessBufferReader(float* buffer, unsigned int count): AccessBuffer(buffer, count) {};
    const float& operator[](unsigned int n) const { return at(n); }
};

static InterfaceTable* ft;

static inline void belaUGenInitOutput(Unit* unit) { (unit->mCalcFunc)(unit, 1); }

static inline void belaUGenDisable(Unit* unit) {
    SETCALC(ClearUnitOutputs);
    belaUGenInitOutput(unit);
}

struct MultiplexAnalogIn : public Unit {};


struct AnalogIn : public Unit {
    int mAnalogPin;
};

struct AnalogOut : public Unit {
    int mAnalogPin;
};

// static digital pin, static function (in)
struct DigitalIn : public Unit {
    int mDigitalPin;
};

// static digital pin, static function (out) - uses DigitalWrite and a check whether value changed
struct DigitalOut : public Unit {
    int mDigitalPin;
    int mLastOut;
};

// flexible digital pin, flexible function (in or out)
struct DigitalIO : public Unit {
    int mDigitalPin;
    int mLastDigitalIn;
    int mLastDigitalOut;
};

//////////////////////////////////////////////////////////////////////////////////////////////////

void MultiplexAnalogIn_next_aaa(MultiplexAnalogIn* unit, int inNumSamples) {
    auto* context = unit->mWorld->mBelaContext;

    float* fin = IN(0); // analog in pin, can be modulated
    float* fmux = IN(1); // mux channel, can be modulated
    float* out = OUT(0);
    float analogValue = 0;

    // context->audioFrames should be equal to inNumSamples
    for (unsigned int n = 0; n < inNumSamples; n++) {
        int analogPin = static_cast<int>(fin[n]);
        int muxChannel = static_cast<int>(fmux[n]);
        if ((analogPin < 0) || (analogPin >= context->analogInChannels) || (muxChannel < 0)
            || (muxChannel >= context->multiplexerChannels)) {
            rt_fprintf(stderr, "MultiplexAnalogIn warning: analog pin must be between %i and %i, it is %i\n", 0,
                       context->analogInChannels, analogPin);
            rt_fprintf(stderr, "MultiplexAnalogIn warning: muxChannel must be between %i and %i, it is %i\n", 0,
                       context->multiplexerChannels, muxChannel);
        } else {
            // is there something like NI? analogReadNI(context, 0, analogPin);
            analogValue = multiplexerAnalogRead(context, analogPin, muxChannel);
        }
        out[n] = analogValue;
    }
}

void MultiplexAnalogIn_next_aak(MultiplexAnalogIn* unit, int inNumSamples) {
    auto* context = unit->mWorld->mBelaContext;

    float* fin = IN(0); // analog in pin, can be modulated
    float* out = OUT(0);
    float analogValue = 0;
    int muxChannel = static_cast<int>(IN0(1));

    // context->audioFrames should be equal to inNumSamples
    for (unsigned int n = 0; n < inNumSamples; n++) {
        int analogPin = static_cast<int>(fin[n]);
        if ((analogPin < 0) || (analogPin >= context->analogInChannels) || (muxChannel < 0)
            || (muxChannel >= context->multiplexerChannels)) {
            rt_fprintf(stderr, "MultiplexAnalogIn warning: analog pin must be between %i and %i, it is %i\n", 0,
                       context->analogInChannels, analogPin);
            rt_fprintf(stderr, "MultiplexAnalogIn warning: muxChannel must be between %i and %i, it is %i\n", 0,
                       context->multiplexerChannels, muxChannel);
        } else {
            // is there something like NI? analogReadNI(context, 0, analogPin);
            analogValue = multiplexerAnalogRead(context, analogPin, muxChannel);
        }
        out[n] = analogValue;
    }
}

void MultiplexAnalogIn_next_aka(MultiplexAnalogIn* unit, int inNumSamples) {
    auto* context = unit->mWorld->mBelaContext;

    float* fmux = IN(1); // mux channel, can be modulated
    float* out = OUT(0);
    int analogPin = static_cast<int>(IN0(0));
    float analogValue = 0;

    // context->audioFrames should be equal to inNumSamples
    for (unsigned int n = 0; n < inNumSamples; n++) {
        int muxChannel = static_cast<int>(fmux[n]);
        if ((analogPin < 0) || (analogPin >= context->analogInChannels) || (muxChannel < 0)
            || (muxChannel >= context->multiplexerChannels)) {
            rt_fprintf(stderr, "MultiplexAnalogIn warning: analog pin must be between %i and %i, it is %i\n", 0,
                       context->analogInChannels, analogPin);
            rt_fprintf(stderr, "MultiplexAnalogIn warning: muxChannel must be between %i and %i, it is %i\n", 0,
                       context->multiplexerChannels, muxChannel);
        } else {
            // is there something like NI? analogReadNI(context, 0, analogPin);
            analogValue = multiplexerAnalogRead(context, analogPin, muxChannel);
        }
        out[n] = analogValue;
    }
}

void MultiplexAnalogIn_next_akk(MultiplexAnalogIn* unit, int inNumSamples) {
    auto* context = unit->mWorld->mBelaContext;

    int analogPin = static_cast<float>(IN0(0));
    int muxChannel = static_cast<float>(IN0(1));
    float* out = OUT(0);

    if ((analogPin < 0) || (analogPin >= context->analogInChannels) || (muxChannel < 0)
        || (muxChannel >= context->multiplexerChannels)) {
        rt_fprintf(stderr, "MultiplexAnalogIn warning: analog pin must be between %i and %i, it is %i\n", 0,
                   context->analogInChannels, analogPin);
        rt_fprintf(stderr, "MultiplexAnalogIn warning: muxChannel must be between %i and %i, it is %i\n", 0,
                   context->multiplexerChannels, muxChannel);
        for (unsigned int n = 0; n < inNumSamples; n++) {
            out[n] = 0;
        }
    } else {
        for (unsigned int n = 0; n < inNumSamples; n++) {
            // is there something like NI? analogReadNI(context, 0, analogPin);
            out[n] = multiplexerAnalogRead(context, analogPin, muxChannel);
        }
    }
}

void MultiplexAnalogIn_next_kkk(MultiplexAnalogIn* unit, int inNumSamples) {
    auto* context = unit->mWorld->mBelaContext;

    int analogPin = static_cast<float>(IN0(0));
    int muxChannel = static_cast<float>(IN0(1));

    if ((analogPin < 0) || (analogPin >= context->analogInChannels)) {
        rt_fprintf(stderr, "MultiplexAnalogIn warning: analog pin must be between %i and %i, it is %i\n", 0,
                   context->analogInChannels, analogPin);
        OUT0(0) = 0.0;
    } else if ((muxChannel < 0) || (muxChannel >= context->multiplexerChannels)) {
        rt_fprintf(stderr, "MultiplexAnalogIn warning: muxChannel must be between %i and %i, it is %i\n", 0,
                   context->multiplexerChannels, muxChannel);
        OUT0(0) = 0.0;
    } else {
        OUT0(0) = multiplexerAnalogRead(context, analogPin, muxChannel);
    }
}

void MultiplexAnalogIn_Ctor(MultiplexAnalogIn* unit) {
    BelaContext* context = unit->mWorld->mBelaContext;

    if (!context->multiplexerChannels) {
        belaUGenDisable(unit);
        rt_fprintf(stderr, "MultiplexAnalogIn Error: the UGen needs BELA Multiplexer Capelet enabled\n");
        return;
    }

    // set calculation method
    if (unit->mCalcRate == calc_FullRate) {
        if (INRATE(0) == calc_FullRate) {
            if (INRATE(1) == calc_FullRate) {
                SETCALC(MultiplexAnalogIn_next_aaa);
            } else {
                SETCALC(MultiplexAnalogIn_next_aak);
            }
        } else {
            if (INRATE(1) == calc_FullRate) {
                SETCALC(MultiplexAnalogIn_next_aka);
            } else {
                SETCALC(MultiplexAnalogIn_next_akk);
            }
        }
    } else {
        if ((INRATE(0) == calc_FullRate) || (INRATE(1) == calc_FullRate)) {
            rt_fprintf(stderr,
                       "MultiplexAnalogIn warning: output rate is control rate, so cannot change analog pin or "
                       "multiplex channel at audio rate\n");
        }
        SETCALC(MultiplexAnalogIn_next_kkk);
    }
    belaUGenInitOutput(unit);
}

//////////////////////////////////////////////////////////////////////////////////////////////////

static bool updatePin(unsigned int numChannels, unsigned int newPin, int* oldPin, const char* label) {
    bool isValid = newPin < numChannels;
    if (newPin != *oldPin) {
        *oldPin = newPin;
        if (!isValid) {
            rt_fprintf(stderr, "%s warning: pin must be 0 <= pin <= %i, it is %i\n", label, numChannels - 1, newPin);
        }
    }
    return isValid;
}

// returns false if pin is out of range, so that _next functions should avoid using it
bool AnalogIn_updatePin(AnalogIn* unit, int newPin) {
    BelaContext* context = unit->mWorld->mBelaContext;
    return updatePin(context->analogInChannels, newPin, &unit->mAnalogPin, "AnalogIn");
}

void AnalogIn_next_aa(AnalogIn* unit, int inNumSamples) {
    auto* context = unit->mWorld->mBelaContext;

    float* fin = IN(0); // analog in pin, can be modulated
    float* out = OUT(0);
    float analogValue = 0;

    for (unsigned int n = 0; n < inNumSamples; n++) {
        int analogPin = static_cast<int>(fin[n]);
        if (AnalogIn_updatePin(unit, analogPin)) {
            analogValue = analogReadNI(context, n, analogPin);
        }
        out[n] = analogValue;
    }
}

void AnalogIn_next_ak(AnalogIn* unit, int inNumSamples) {
    auto* context = unit->mWorld->mBelaContext;

    int analogPin = static_cast<int>(IN0(0));
    float* out = OUT(0);

    if (AnalogIn_updatePin(unit, analogPin)) {
        for (unsigned int n = 0; n < inNumSamples; n++) {
            out[n] = analogReadNI(context, n, analogPin);
        }
    } else {
        for (unsigned int n = 0; n < inNumSamples; n++) {
            out[n] = 0;
        }
    }
}


void AnalogIn_next_kk(AnalogIn* unit, int inNumSamples) {
    auto* context = unit->mWorld->mBelaContext;

    int analogPin = static_cast<int>(IN0(0));

    if (AnalogIn_updatePin(unit, analogPin)) {
        OUT0(0) = analogReadNI(context, 0, analogPin);
    } else {
        OUT0(0) = 0.0;
    }
}

void AnalogIn_Ctor(AnalogIn* unit) {
    BelaContext* context = unit->mWorld->mBelaContext;

    if (!context->analogInChannels) {
        belaUGenDisable(unit);
        rt_fprintf(stderr, "AnalogIn Error: the UGen needs BELA analog inputs enabled\n");
        return;
    }

    unit->mAnalogPin = -1;

    // set calculation method
    if (unit->mCalcRate == calc_FullRate) {
        if (INRATE(0) == calc_FullRate) {
            SETCALC(AnalogIn_next_aa);
        } else {
            SETCALC(AnalogIn_next_ak);
        }
    } else {
        if (INRATE(0) == calc_FullRate) {
            rt_fprintf(stderr,
                       "AnalogIn warning: output rate is control rate, so cannot change analog pin at audio rate\n");
        }
        SETCALC(AnalogIn_next_kk);
    }
    belaUGenInitOutput(unit);
}

//////////////////////////////////////////////////////////////////////////////////////////////////

// returns false if pin is out of range, so that _next functions should avoid using it
bool AnalogOut_updatePin(AnalogOut* unit, int newPin) {
    BelaContext* context = unit->mWorld->mBelaContext;
    return updatePin(context->analogOutChannels, newPin, &unit->mAnalogPin, "AnalogOut");
}

void AnalogOut_next_aaa(AnalogOut* unit, int inNumSamples) {
    auto* context = unit->mWorld->mBelaContext;

    float* fin = IN(0); // analog in pin, can be modulated
    float* in = IN(1);

    for (unsigned int n = 0; n < inNumSamples; n++) {
        // read input
        int analogPin = static_cast<int>(fin[n]);
        if (AnalogOut_updatePin(unit, analogPin)) {
            analogWriteOnceNI(context, n, unit->mAnalogPin, in[n]);
        }
    }
}

void AnalogOut_next_aka(AnalogOut* unit, int inNumSamples) {
    auto* context = unit->mWorld->mBelaContext;

    int analogPin = static_cast<int>(IN0(0)); // analog in pin, can be modulated
    float* in = IN(1);

    if (AnalogOut_updatePin(unit, analogPin)) {
        for (unsigned int n = 0; n < inNumSamples; n++) {
            analogWriteOnceNI(context, n, unit->mAnalogPin, in[n]);
        }
    }
}

void AnalogOut_next_aak(AnalogOut* unit, int inNumSamples) {
    auto* context = unit->mWorld->mBelaContext;

    float* fin = IN(0); // analog in pin, can be modulated
    float in = IN0(1);

    for (unsigned int n = 0; n < inNumSamples; n++) {
        // read input
        int analogPin = static_cast<int>(fin[n]);
        if (AnalogOut_updatePin(unit, analogPin)) {
            analogWriteOnceNI(context, n, unit->mAnalogPin, in);
        }
    }
}

void AnalogOut_next_kk(AnalogOut* unit, int inNumSamples) {
    auto* context = unit->mWorld->mBelaContext;

    int analogPin = static_cast<int>(IN0(0)); // analog in pin, can be modulated
    float in = IN0(1);

    if (AnalogOut_updatePin(unit, analogPin)) {
        analogWriteNI(context, 0, unit->mAnalogPin, in);
    }
}

void AnalogOut_Ctor(AnalogOut* unit) {
    BelaContext* context = unit->mWorld->mBelaContext;

    if (!context->analogOutChannels) {
        belaUGenDisable(unit);
        rt_fprintf(stderr, "AnalogOut Error: the UGen needs BELA analog outputs enabled\n");
        return;
    }

    unit->mAnalogPin = -1;

    if (unit->mCalcRate == calc_FullRate) { // ugen running at audio rate;
        if (INRATE(0) == calc_FullRate) { // pin changed at audio rate
            if (INRATE(1) == calc_FullRate) { // output changed at audio rate
                SETCALC(AnalogOut_next_aaa);
            } else {
                SETCALC(AnalogOut_next_aak);
            }
        } else { // pin changed at control rate
            if (INRATE(1) == calc_FullRate) { // output changed at audio rate
                SETCALC(AnalogOut_next_aka);
            } else { // analog output only changes at control rate anyways
                rt_fprintf(
                    stderr,
                    "AnalogOut warning: inputs are control rate, so AnalogOut is also running at control rate\n");
                SETCALC(AnalogOut_next_kk);
            }
        }
    } else { // ugen at control rate
        if ((INRATE(0) == calc_FullRate) || (INRATE(1) == calc_FullRate)) {
            rt_fprintf(stderr,
                       "AnalogOut warning: output rate is control rate, so cannot change inputs at audio rate\n");
        }
        SETCALC(AnalogOut_next_kk);
    }
    belaUGenInitOutput(unit);
}

//////////////////////////////////////////////////////////////////////////////////////////////////

void DigitalIn_next_a(DigitalIn* unit, int inNumSamples) {
    auto* context = unit->mWorld->mBelaContext;

    int pinid = unit->mDigitalPin;
    float* out = OUT(0);

    for (unsigned int n = 0; n < inNumSamples; n++) {
        out[n] = static_cast<float>(digitalRead(context, n, pinid));
    }
}

void DigitalIn_next_k(DigitalIn* unit, int inNumSamples) {
    auto* context = unit->mWorld->mBelaContext;

    int pinid = unit->mDigitalPin;
    OUT0(0) = static_cast<float>(digitalRead(context, 0, pinid));
}

void DigitalIn_Ctor(DigitalIn* unit) {
    BelaContext* context = unit->mWorld->mBelaContext;

    unit->mDigitalPin = static_cast<int>(IN0(0)); // digital in pin -- cannot change after construction
    if ((unit->mDigitalPin < 0) || (unit->mDigitalPin >= context->digitalChannels)) {
        rt_fprintf(stderr, "DigitalIn error: digital pin must be between %i and %i, it is %i\n", 0,
                   context->digitalChannels, unit->mDigitalPin);
        belaUGenDisable(unit);
        return;
    }
    pinMode(context, 0, unit->mDigitalPin, INPUT);
    // set calculation method
    if (unit->mCalcRate == calc_FullRate) { // ugen running at audio rate;
        SETCALC(DigitalIn_next_a);
    } else {
        SETCALC(DigitalIn_next_k);
    }
    belaUGenInitOutput(unit);
}

//////////////////////////////////////////////////////////////////////////////////////////////////

void DigitalOut_next_a(DigitalOut* unit, int inNumSamples) {
    auto* context = unit->mWorld->mBelaContext;

    int pinid = unit->mDigitalPin;
    float* in = IN(1);
    int lastOut = unit->mLastOut;

    for (unsigned int n = 0; n < inNumSamples; n++) {
        // read input
        float newinput = in[n];
        if (newinput > 0.5) {
            if (lastOut == 0) {
                lastOut = 1;
                digitalWrite(context, n, pinid, 1);
            }
        } else if (lastOut == 1) {
            lastOut = 0;
            digitalWrite(context, n, pinid, 0);
        }
    }
    unit->mLastOut = lastOut;
}

void DigitalOut_next_k(DigitalOut* unit, int inNumSamples) {
    auto* context = unit->mWorld->mBelaContext;

    int pinid = unit->mDigitalPin;
    float in = IN0(1);
    int lastOut = unit->mLastOut;

    if (in > 0.5) {
        if (lastOut == 0) {
            lastOut = 1;
            digitalWrite(context, 0, pinid, 1);
        }
    } else if (lastOut == 1) {
        lastOut = 0;
        digitalWrite(context, 0, pinid, 0);
    }
    unit->mLastOut = lastOut;
}

void DigitalOut_Ctor(DigitalOut* unit) {
    BelaContext* context = unit->mWorld->mBelaContext;

    unit->mDigitalPin = static_cast<int>(IN0(0)); // digital in pin -- cannot change after construction
    unit->mLastOut = 0;

    if ((unit->mDigitalPin < 0) || (unit->mDigitalPin >= context->digitalChannels)) {
        rt_fprintf(stderr, "DigitalOut error: digital pin must be between %i and %i, it is %i\n", 0,
                   context->digitalChannels, unit->mDigitalPin);
        belaUGenDisable(unit);
    }
    // initialize first buffer
    pinMode(context, 0, unit->mDigitalPin, OUTPUT);
    digitalWrite(context, 0, unit->mDigitalPin, unit->mLastOut);

    if (unit->mCalcRate == calc_FullRate) { // ugen running at audio rate;
        if (INRATE(1) == calc_FullRate) { // output changed at audio rate
            SETCALC(DigitalOut_next_a);
        } else { // not much reason to actually do audiorate output
            rt_fprintf(stderr, "DigitalOut warning: inputs are control rate, so DigitalOut will run at control rate\n");
            SETCALC(DigitalOut_next_k);
        }
    } else { // ugen at control rate
        if (INRATE(1) == calc_FullRate) {
            rt_fprintf(stderr,
                       "DigitalOut warning: UGen rate is control rate, so cannot change inputs at audio rate\n");
        }
        SETCALC(DigitalOut_next_k);
    }
    belaUGenInitOutput(unit);
}

//////////////////////////////////////////////////////////////////////////////////////////////////

// returns false if pin is out of range, so that _next functions should avoid using it
bool DigitalIO_updatePin(DigitalIO* unit, int newPin) {
    BelaContext* context = unit->mWorld->mBelaContext;
    return updatePin(context->digitalChannels, newPin, &unit->mDigitalPin, "DigitalIO");
}

static int parseDigitalValue(float value) { return value > 0.5; }

static int parseDigitalMode(float mode) { return mode < 0.5 ? INPUT : OUTPUT; }

void DigitalIO_next_universal(DigitalIO* unit, int inNumSamples) {
    auto* context = unit->mWorld->mBelaContext;
    const bool ugenAudioRate = (calc_FullRate == unit->mCalcRate);
    const bool pinAudioRate = (calc_FullRate == INRATE(0));
    const bool inputAudioRate = (calc_FullRate == INRATE(1));
    const bool modeAudioRate = (calc_FullRate == INRATE(2));

    unsigned int outsCount = ugenAudioRate ? inNumSamples : 1;
    unsigned int pinsCount = pinAudioRate ? inNumSamples : 1;
    unsigned int insCount = inputAudioRate ? inNumSamples : 1;
    unsigned int modesCount = modeAudioRate ? inNumSamples : 1;
    AccessBufferWriter outs(OUT(0), outsCount); // may be the same as pins
    const AccessBufferReader pins(IN(0), pinsCount);
    const AccessBufferReader ins(IN(1), insCount);
    const AccessBufferReader modes(IN(2), modesCount);

    bool lastDigIn = unit->mLastDigitalIn;
    // with properly initialised AccessBuffers, we can use [n] below regardless
    // of the K/A rate of each buffer
    for (unsigned int n = 0; n < inNumSamples; ++n) {
        unsigned int pin = static_cast<int>(pins[n]);
        if (DigitalIO_updatePin(unit, pin)) {
            int mode = parseDigitalMode(modes[n]);
            if (1 == inNumSamples) {
                // we are only ever going to go up to 1 (i.e.: processed at
                // control rate). So fill up the rest of the buffer.
                pinMode(context, 0, unit->mDigitalPin, mode);
            } else {
                pinModeOnce(context, n, unit->mDigitalPin, mode);
            }
            if (INPUT == mode) {
                lastDigIn = digitalRead(context, n, unit->mDigitalPin);
            } else {
                bool digOut = parseDigitalValue(ins[n]);
                if (1 == inNumSamples) {
                    // we are only ever going to go up to 1 (i.e.: processed at
                    // control rate). So fill up the rest of the buffer.
                    digitalWrite(context, 0, unit->mDigitalPin, digOut);
                } else {
                    digitalWriteOnce(context, n, unit->mDigitalPin, digOut);
                }
            }
        }
        outs[n] = lastDigIn;
    }
    unit->mLastDigitalIn = lastDigIn;
}

void DigitalIO_Ctor(DigitalIO* unit) {
    BelaContext* context = unit->mWorld->mBelaContext;

    unit->mDigitalPin = 0;
    unit->mLastDigitalIn = 0;
    unit->mLastDigitalOut = 0;
    SETCALC(DigitalIO_next_universal);
    belaUGenInitOutput(unit);
}

//////////////////////////////////////////////////////////////////////////////////////////////////

struct BelaScopeOut : public Unit {
    unsigned int numScopeChannels;
    unsigned int offset;
};

void BelaScopeOut_next(BelaScopeOut* unit, unsigned int numSamples) {
    float* scopeBuffer = unit->mWorld->mBelaScope->buffer;
    if (!scopeBuffer)
        return;
    unsigned int numChannels = unit->numScopeChannels;
    unsigned int maxChannels = unit->mWorld->mBelaMaxScopeChannels;
    unsigned int scopeBufferSamples = unit->mWorld->mBelaScope->bufferSamples;
    float* inputPointers[numChannels];
    // input 0: channelOffset
    // inputs 1 to numInputs-1 : signal inputs
    for (unsigned int ch = 0; ch < numChannels; ++ch)
        inputPointers[ch] = IN(ch + 1); // skip IN(0)

    for (unsigned int frame = unit->offset; frame < scopeBufferSamples; frame += maxChannels)
        for (unsigned int ch = 0; ch < numChannels; ++ch)
            scopeBuffer[frame + ch] += *inputPointers[ch]++;
    unit->mWorld->mBelaScope->touched = true;
}

void BelaScopeOut_Ctor(BelaScopeOut* unit) {
    BelaScope* scope = unit->mWorld->mBelaScope;
    if (!scope || !scope->buffer) {
        rt_fprintf(stderr, "BelaScopeOut error: Scope not initialized on server\n");
        belaUGenDisable(unit);
        return;
    };

    int offset = static_cast<int>(IN0(0));
    unit->offset = static_cast<unsigned int>(offset < 0 ? 0 : offset);
    uint32 maxScopeChannels = unit->mWorld->mBelaMaxScopeChannels;
    uint32 numInputSignals = unit->mNumInputs - 1;
    if (numInputSignals > maxScopeChannels - unit->offset) {
        rt_fprintf(
            stderr,
            "BelaScopeOut warning: can't scope %i channels starting from %i, maxBelaScopeChannels is set to %i\n",
            numInputSignals, offset, maxScopeChannels);
    }
    unit->numScopeChannels = sc_min(numInputSignals, maxScopeChannels - unit->offset);
    if (unit->numScopeChannels <= 0) {
        belaUGenDisable(unit);
    } else {
        BelaScopeOut_next(unit, 1);
        SETCALC(BelaScopeOut_next);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////

PluginLoad(BELA) {
    ft = inTable;

    DefineSimpleUnit(MultiplexAnalogIn);
    DefineSimpleUnit(AnalogIn);
    DefineSimpleUnit(AnalogOut);
    DefineSimpleUnit(DigitalIn);
    DefineSimpleUnit(DigitalOut);
    DefineSimpleUnit(DigitalIO);
    DefineSimpleUnit(BelaScopeOut);
}
