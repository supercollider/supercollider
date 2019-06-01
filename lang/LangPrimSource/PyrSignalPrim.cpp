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

#include "PyrSymbol.h"
#include "PyrKernel.h"
#include "PyrObject.h"
#include "PyrPrimitive.h"
#include "PyrSignal.h"
#include "PyrSignalPrim.h"
#include "PyrMessage.h"
#include "SC_Constants.h"
#include "SCBase.h"
#include "clz.h"
extern "C" {
#include "fftlib.h"
}
#include <stdlib.h>
#include <string.h>
#include <math.h>


int prSignalFill(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *b;
    int err;
    float value;

    a = g->sp - 1;
    b = g->sp;

    err = slotFloatVal(b, &value);
    if (err != errNone)
        return err;
    signal_fill(slotRawObject(a), value);
    return errNone;
}

int prSignalScale(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *b;
    int err;
    float value;

    a = g->sp - 1;
    b = g->sp;

    err = slotFloatVal(b, &value);
    if (err != errNone)
        return err;
    signal_scale(slotRawObject(a), value);
    return errNone;
}

int prSignalOffset(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *b;
    int err;
    float value;

    a = g->sp - 1;
    b = g->sp;

    err = slotFloatVal(b, &value);
    if (err != errNone)
        return err;
    signal_offset(slotRawObject(a), value);
    return errNone;
}

int prSignalString(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* a;
    PyrString* string;
    PyrObject* signal;
    float* x;
    char str[128];

    a = g->sp;
    slotString(a, str);

    signal = slotRawObject(a);
    if (signal->size) {
        x = (float*)(signal->slots);
        sprintf(str, "%s[%g .. %g]", slotRawSymbol(&signal->classptr->name)->name, x[0], x[signal->size - 1]);
    } else {
        sprintf(str, "%s[none]", slotRawSymbol(&signal->classptr->name)->name);
    }
    string = newPyrString(g->gc, str, 0, true);
    SetObject(a, string);
    return errNone;
}

int prSignalPeak(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* a;

    a = g->sp;

    SetFloat(a, signal_findpeak(slotRawObject(a)));
    return errNone;
}

int prSignalNormalize(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *b, *c;
    int err, start, end;

    a = g->sp - 2;
    b = g->sp - 1;
    c = g->sp;

    err = slotIntVal(b, &start);
    if (err) {
        if (IsNil(c))
            start = 0;
        else
            return err;
    }

    err = slotIntVal(c, &end);
    if (err) {
        if (IsNil(c))
            end = slotRawObject(a)->size;
        else
            return err;
    }

    signal_normalize_range(slotRawObject(a), start, end);
    return errNone;
}

int prSignalNormalizeTransferFn(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* a;

    a = g->sp;

    signal_normalize_transfer_fn(slotRawObject(a));
    return errNone;
}

int prSignalIntegral(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* a;

    a = g->sp;

    SetFloat(a, signal_integral(slotRawObject(a)));
    return errNone;
}

int prSignalInvert(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *b, *c;
    int err, start, end;

    a = g->sp - 2;
    b = g->sp - 1;
    c = g->sp;

    err = slotIntVal(b, &start);
    if (err) {
        if (IsNil(c))
            start = 0;
        else
            return err;
    }

    err = slotIntVal(c, &end);
    if (err) {
        if (IsNil(c))
            end = slotRawObject(a)->size;
        else
            return err;
    }

    signal_invert_range(slotRawObject(a), start, end);
    return errNone;
}

int prSignalReverse(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *b, *c;
    int err, start, end;

    a = g->sp - 2;
    b = g->sp - 1;
    c = g->sp;

    err = slotIntVal(b, &start);
    if (err) {
        if (IsNil(b))
            start = 0;
        else
            return err;
    }

    err = slotIntVal(c, &end);
    if (err) {
        if (IsNil(c))
            end = slotRawObject(a)->size;
        else
            return err;
    }

    signal_reverse_range(slotRawObject(a), start, end);
    return errNone;
}

int prSignalRotate(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *b;
    int err, rot;

    a = g->sp - 1;
    b = g->sp;

    err = slotIntVal(b, &rot);
    if (err)
        return err;

    SetRaw(a, signal_rotate(g, slotRawObject(a), rot));
    return errNone;
}

int prSignalOverDub(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *b, *c;
    int err;
    int index;

    a = g->sp - 2;
    b = g->sp - 1;
    c = g->sp;

    if (NotObj(b) || !isKindOf(slotRawObject(b), class_signal))
        return errWrongType;
    err = slotIntVal(c, &index);
    if (err)
        return errWrongType;

    signal_overdub(g, slotRawObject(a), slotRawObject(b), index);
    return errNone;
}

int prSignalOverWrite(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *b, *c;
    int err;
    int index;

    a = g->sp - 2;
    b = g->sp - 1;
    c = g->sp;

    if (NotObj(b) || !isKindOf(slotRawObject(b), class_signal))
        return errWrongType;
    err = slotIntVal(c, &index);
    if (err)
        return errWrongType;

    signal_overwrite(g, slotRawObject(a), slotRawObject(b), index);
    return errNone;
}

int prSignalFade(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *b, *c, *d, *e;
    int err;
    int start, end;
    float lvl0, lvl1;

    a = g->sp - 4;
    b = g->sp - 3;
    c = g->sp - 2;
    d = g->sp - 1;
    e = g->sp;

    err = slotIntVal(b, &start);
    if (err) {
        if (IsNil(b))
            start = 0;
        else
            return err;
    }

    err = slotIntVal(c, &end);
    if (err) {
        if (IsNil(c))
            end = slotRawObject(a)->size;
        else
            return err;
    }

    err = slotFloatVal(d, &lvl0);
    if (err)
        return err;

    err = slotFloatVal(e, &lvl1);
    if (err)
        return err;

    signal_fade_range(slotRawObject(a), start, end, lvl0, lvl1);
    return errNone;
}

int prSignalAddHarmonic(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *b, *c, *d;
    int err;
    float harmonic, amp;
    double phase, step;
    PyrObject* signal;
    float* out;

    a = g->sp - 3;
    b = g->sp - 2;
    c = g->sp - 1;
    d = g->sp;

    err = slotFloatVal(b, &harmonic);
    if (err)
        return errWrongType;
    err = slotFloatVal(c, &amp);
    if (err)
        return errWrongType;
    err = slotDoubleVal(d, &phase);
    if (err)
        return errWrongType;

    signal = slotRawObject(a);
    out = (float*)(signal->slots) - 1;
    step = twopi * harmonic / signal->size;
    UNROLL_CODE(signal->size, out, *++out += sin(phase) * amp; phase += step;);

    return errNone;
}

int prSignalAddChebyshev(struct VMGlobals* g, int numArgsPushed);
int prSignalAddChebyshev(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *b, *c;
    int err;
    float harmonic, amp;
    double x, step;
    PyrObject* signal;
    float* out;

    a = g->sp - 2;
    b = g->sp - 1;
    c = g->sp;

    err = slotFloatVal(b, &harmonic);
    if (err)
        return errWrongType;
    err = slotFloatVal(c, &amp);
    if (err)
        return errWrongType;

    signal = slotRawObject(a);
    out = (float*)(signal->slots) - 1;
    x = -1.0;
    step = 2.0 / (signal->size - 1);
    UNROLL_CODE(signal->size, out, *++out += cos(harmonic * acos(sc_min(x, 1.0))) * amp; x += step;);

    return errNone;
}

//////////////////////////////////////////////////////////////////////////////

void signalAsWavetable(float* signal, float* wavetable, int size) {
    int i, imax;
    float *in, *out;
    float val1, val2;

    in = signal;
    out = wavetable - 1;
    imax = size - 1;
    for (i = 0; i < imax; ++i) {
        val1 = in[i];
        val2 = in[i + 1];
        *++out = 2.f * val1 - val2;
        *++out = val2 - val1;
    }
    val1 = in[imax];
    val2 = in[0];
    *++out = 2.f * val1 - val2;
    *++out = val2 - val1;
}

void wavetableAsSignal(float* wavetable, float* signal, int size) {
    float *in, *out;
    in = wavetable;
    out = signal - 1;
    for (int i = 0; i < size; ++i) {
        *++out = in[0] + in[1];
        in += 2;
    }
}


//////////////////////////////////////////////////////////////////////////////

int prSignalAsWavetable(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* a;
    int size;
    PyrObject *signal, *wavetable;

    a = g->sp;

    signal = slotRawObject(a);

    size = signal->size;
    if ((size & (size - 1)) != 0) {
        error("Signal size not a power of two.\n");
        return errFailed;
    }
    wavetable = newPyrSignal(g, signal->size * 2);
    wavetable->classptr = class_wavetable;
    signalAsWavetable((float*)signal->slots, (float*)wavetable->slots, signal->size);

    SetObject(a, wavetable);
    return errNone;
}


int prWavetableAsSignal(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* a;
    PyrObject *signal, *wavetable;

    a = g->sp;

    wavetable = slotRawObject(a);
    signal = newPyrSignal(g, wavetable->size / 2);
    wavetableAsSignal((float*)wavetable->slots, (float*)signal->slots, signal->size);
    SetObject(a, signal);
    return errNone;
}
// class_signal

#if 1
int prSignal_FFT(struct VMGlobals* g, int numArgsPushed);
int prSignal_FFT(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *b, *c;

    int i, M, fftsize, fftbufsize, asize;
    float *inreal, *inimag, *fftbuf, *costable, *outreal, *outimag;
    PyrObject *fftoutobj, *complexobj, *realobj, *imagobj;

    a = g->sp - 2;
    b = g->sp - 1;
    c = g->sp;

    asize = slotRawObject(a)->size;
    if (NotNil(b) && !(isKindOfSlot(b, class_signal) && slotRawObject(b)->size == asize)) {
        error("Signal::fft imaginary part wrong type or length.\n");
        return errFailed;
    }

    M = LOG2CEIL(asize);
    fftsize = 1L << M;

    if (!(isKindOfSlot(c, class_floatarray))) {
        error("Signal::fft must be provided a table containing 1/4 cycle of a cosine.\n");
        return errFailed;
    }
    if (slotRawObject(c)->size != fftsize / 4 + 1) {
        error("Signal::fft cosine table wrong size (%d), expected %d.\n", slotRawObject(c)->size, fftsize / 4 + 1);
        return errFailed;
    }
    costable = (float*)slotRawObject(c)->slots;

    fftbufsize = fftsize * 2;
    fftoutobj = newPyrSignal(g, fftbufsize);
    fftoutobj->size = fftbufsize;
    ++g->sp;
    SetObject(g->sp, fftoutobj);

    complexobj = instantiateObject(g->gc, s_complex->u.classobj, 0, false, true);
    ++g->sp;
    SetObject(g->sp, complexobj);

    realobj = newPyrSignal(g, fftbufsize);
    SetObject(complexobj->slots + 0, realobj);
    g->gc->GCWriteNew(complexobj, realobj);

    imagobj = newPyrSignal(g, fftbufsize);
    SetObject(complexobj->slots + 1, imagobj);
    g->gc->GCWriteNew(complexobj, imagobj);

    inreal = (float*)slotRawObject(a)->slots - 1;
    if (IsNil(b)) {
        fftbuf = (float*)fftoutobj->slots - 1;
        for (i = 0; i < asize; ++i) {
            *++fftbuf = *++inreal;
        }
        for (i = asize; i < fftsize; ++i) {
            *++fftbuf = 0.f;
        }

        fftbuf = (float*)fftoutobj->slots;
        rffts(fftbuf, M, 1, costable);
    } else {
        inimag = (float*)slotRawObject(b)->slots - 1;

        fftbuf = (float*)fftoutobj->slots - 1;
        for (i = 0; i < asize; ++i) {
            *++fftbuf = *++inreal;
            *++fftbuf = *++inimag;
        }
        for (i = asize; i < fftsize; ++i) {
            *++fftbuf = 0.f;
            *++fftbuf = 0.f;
        }

        fftbuf = (float*)fftoutobj->slots;
        ffts(fftbuf, M, 1, costable);
    }
    outreal = (float*)realobj->slots - 1;
    outimag = (float*)imagobj->slots - 1;
    fftbuf = (float*)fftoutobj->slots - 1;
    for (i = 0; i < fftsize; ++i) {
        *++outreal = *++fftbuf;
        *++outimag = *++fftbuf;
    }
    realobj->size = fftsize;
    imagobj->size = fftsize;

    g->sp -= 2;
    SetRaw(a, complexobj);

    return errNone;
}

int prSignal_IFFT(struct VMGlobals* g, int numArgsPushed);
int prSignal_IFFT(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot *a, *b, *c;

    int i, M, fftsize, fftbufsize, asize;
    float *inreal, *inimag, *fftbuf, *costable, *outreal, *outimag;
    PyrObject *fftoutobj, *complexobj, *realobj, *imagobj;

    a = g->sp - 2;
    b = g->sp - 1;
    c = g->sp;

    asize = slotRawObject(a)->size;
    if (!(isKindOfSlot(b, class_signal) && slotRawObject(b)->size == asize)) {
        error("Signal::ifft imaginary part wrong type or length.\n");
        return errFailed;
    }

    M = LOG2CEIL(asize);
    fftsize = 1L << M;

    if (!(isKindOfSlot(c, class_floatarray))) {
        error("Signal::ifft must be provided a table containing 1/4 cycle of a cosine.\n");
        return errFailed;
    }
    if (slotRawObject(c)->size != fftsize / 4 + 1) {
        error("Signal::ifft cosine table wrong size (%d), expected %d.\n", slotRawObject(c)->size, fftsize / 4 + 1);
        return errFailed;
    }
    costable = (float*)slotRawObject(c)->slots;

    fftbufsize = fftsize * 2;
    fftoutobj = newPyrSignal(g, fftbufsize);
    fftoutobj->size = fftbufsize;
    ++g->sp;
    SetObject(g->sp, fftoutobj);

    complexobj = instantiateObject(g->gc, s_complex->u.classobj, 0, false, true);
    ++g->sp;
    SetObject(g->sp, complexobj);

    realobj = newPyrSignal(g, fftbufsize);
    SetObject(complexobj->slots + 0, realobj);
    g->gc->GCWriteNew(complexobj, realobj);

    imagobj = newPyrSignal(g, fftbufsize);
    SetObject(complexobj->slots + 1, imagobj);
    g->gc->GCWriteNew(complexobj, imagobj);

    inreal = (float*)slotRawObject(a)->slots - 1;
    inimag = (float*)slotRawObject(b)->slots - 1;

    fftbuf = (float*)fftoutobj->slots - 1;
    for (i = 0; i < asize; ++i) {
        *++fftbuf = *++inreal;
        *++fftbuf = *++inimag;
    }
    for (i = asize; i < fftsize; ++i) {
        *++fftbuf = 0.f;
        *++fftbuf = 0.f;
    }

    fftbuf = (float*)fftoutobj->slots;
    iffts(fftbuf, M, 1, costable);

    outreal = (float*)realobj->slots - 1;
    outimag = (float*)imagobj->slots - 1;
    fftbuf = (float*)fftoutobj->slots - 1;
    for (i = 0; i < fftsize; ++i) {
        *++outreal = *++fftbuf;
        *++outimag = *++fftbuf;
    }
    realobj->size = fftsize;
    imagobj->size = fftsize;

    g->sp -= 2;
    SetRaw(a, complexobj);

    return errNone;
}
#endif

void initSignalPrimitives() {
    int base, index;

    base = nextPrimitiveIndex();
    index = 0;
    definePrimitive(base, index++, "_SignalPeak", prSignalPeak, 1, 0);
    definePrimitive(base, index++, "_SignalNormalize", prSignalNormalize, 3, 0);
    definePrimitive(base, index++, "_SignalNormalizeTransferFn", prSignalNormalizeTransferFn, 1, 0);
    definePrimitive(base, index++, "_SignalIntegral", prSignalIntegral, 1, 0);
    definePrimitive(base, index++, "_SignalReverse", prSignalReverse, 3, 0);
    definePrimitive(base, index++, "_SignalInvert", prSignalInvert, 3, 0);
    definePrimitive(base, index++, "_SignalRotate", prSignalRotate, 2, 0);

    definePrimitive(base, index++, "_SignalFill", prSignalFill, 2, 0);
    definePrimitive(base, index++, "_SignalScale", prSignalScale, 2, 0);
    definePrimitive(base, index++, "_SignalOffset", prSignalOffset, 2, 0);
    definePrimitive(base, index++, "_SignalOverDub", prSignalOverDub, 3, 0);
    definePrimitive(base, index++, "_SignalOverWrite", prSignalOverWrite, 3, 0);
    definePrimitive(base, index++, "_SignalFade", prSignalFade, 5, 0);
    definePrimitive(base, index++, "_SignalAddHarmonic", prSignalAddHarmonic, 4, 0);
    definePrimitive(base, index++, "_SignalAddChebyshev", prSignalAddChebyshev, 3, 0);
    definePrimitive(base, index++, "_SignalString", prSignalString, 1, 0);
    definePrimitive(base, index++, "_SignalAsWavetable", prSignalAsWavetable, 1, 0);
    definePrimitive(base, index++, "_WavetableAsSignal", prWavetableAsSignal, 1, 0);

    definePrimitive(base, index++, "_Signal_FFT", prSignal_FFT, 3, 0);
    definePrimitive(base, index++, "_Signal_IFFT", prSignal_IFFT, 3, 0);
}
