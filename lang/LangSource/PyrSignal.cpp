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


// Note: For some reason Signals are PyrFloatArrays, this seems incorrect and they should be PyrFloatArrays instead.

/*
    compound formulas :
    amclip	out = B<=0 ? 0 : A*B;		// two quadrant amplitude modulation
    ring1	out = A*(B+1) = A*B + A;	// amplitude modulation of a by b.
    ring2	out = A*B + A + B;			// ring modulation plus both original signals
    ring3	out = A*A*B;				// ring modulation variant
    ring4	out = A*A*B - A*B*B;		// ring modulation variant
    difsqr	out = A*A - B*B;			// difference of squares
    sumsqr	out = A*A + B*B;			// sum of squares
    sqrdif	out = (A - B)^2				// square of the difference = a^2 + b^2 - 2ab
    sqrsum	out = (A + B)^2				// square of the sum        = a^2 + b^2 + 2ab
*/

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "PyrSignal.h"
#include "PyrKernel.h"
#include "SC_InlineUnaryOp.h"
#include "SC_InlineBinaryOp.h"
#include "SCBase.h"
#include "InitAlloc.h"

PyrFloatArray* newPyrSignal(VMGlobals* g, std::int64_t size) {
    PyrFloatArray* signal;
    std::int64_t numbytes = size * sizeof(float);
    signal = (PyrFloatArray*)g->gc->New(numbytes, 0, obj_float, true);
    if (signal) {
        signal->classptr = class_signal;
        signal->size = size;
    }
    // note: signal is filled with garbage
    return signal;
}

/*PyrFloatArray* newPyrSignalFrom(VMGlobals *g, PyrFloatArray* inSignal, std::int64_t size)
{
    PyrFloatArray *signal;
    double *pslot, *qslot, *endptr;
    std::int64_t set, m, mmax;
    std::int64_t numbytes = size * sizeof(float);
    signal = (PyrFloatArray*)g->gc->New(numbytes, 0, obj_float, true);
    signal->classptr = inSignal->classptr;
    signal->size = size;
    return signal;
}
*/

PyrFloatArray* signal_fill(PyrFloatArray* outSignal, float inValue) {
    float* out = outSignal->f - 1;
    UNROLL_CODE(outSignal->size, out, *++out = inValue;);
    return outSignal;
}

PyrFloatArray* signal_scale(PyrFloatArray* outSignal, float inValue) {
    if (inValue != 1.f) {
        float* out = outSignal->f - 1;
        UNROLL_CODE(outSignal->size, out, *++out *= inValue;)
    }
    return outSignal;
}

PyrFloatArray* signal_offset(PyrFloatArray* outSignal, float inValue) {
    if (inValue != 0.f) {
        float* out = outSignal->f - 1;
        UNROLL_CODE(outSignal->size, out, *++out += inValue;)
    }
    return outSignal;
}

PyrFloatArray* signal_add_xx(VMGlobals* g, PyrFloatArray* ina, PyrFloatArray* inb) { BINOP_LOOP1(+); }
PyrFloatArray* signal_mul_xx(VMGlobals* g, PyrFloatArray* ina, PyrFloatArray* inb) { BINOP_LOOP1(*); }
PyrFloatArray* signal_sub_xx(VMGlobals* g, PyrFloatArray* ina, PyrFloatArray* inb) { BINOP_LOOP2(*++c = *++a - *++b;); }
PyrFloatArray* signal_mod_xx(VMGlobals* g, PyrFloatArray* ina, PyrFloatArray* inb) {
    BINOP_LOOP2(++a; *++c = sc_mod(*a, *++b));
}
PyrFloatArray* signal_pow_xx(VMGlobals* g, PyrFloatArray* ina, PyrFloatArray* inb) {
    BINOP_LOOP2(++a; *++c = sc_pow(*a, *++b););
}
PyrFloatArray* signal_ring1_xx(VMGlobals* g, PyrFloatArray* ina, PyrFloatArray* inb) {
    BINOP_LOOP2(++a; *++c = *a * *++b + *a;);
}

PyrFloatArray* signal_ring2_xx(VMGlobals* g, PyrFloatArray* ina, PyrFloatArray* inb) {
    BINOP_LOOP2(++a; ++b; *++c = *a * *b + *a + *b;);
}

PyrFloatArray* signal_ring3_xx(VMGlobals* g, PyrFloatArray* ina, PyrFloatArray* inb) {
    BINOP_LOOP2(++a; *++c = *a * *a * *++b;);
}

PyrFloatArray* signal_ring4_xx(VMGlobals* g, PyrFloatArray* ina, PyrFloatArray* inb) {
    BINOP_LOOP2(++a; ++b; *++c = *a * *a * *b - *a * *b * *b;);
}

PyrFloatArray* signal_thresh_xx(VMGlobals* g, PyrFloatArray* ina, PyrFloatArray* inb) {
    BINOP_LOOP2(++a; ++b; *++c = *a < *b ? 0.f : *a;);
}

PyrFloatArray* signal_amclip_xx(VMGlobals* g, PyrFloatArray* ina, PyrFloatArray* inb) {
    BINOP_LOOP2(++a; ++b; *++c = *b <= 0.f ? 0.f : *a * *b;);
}

PyrFloatArray* signal_div_xx(VMGlobals* g, PyrFloatArray* ina, PyrFloatArray* inb) { BINOP_LOOP2(*++c = *++a / *++b;); }

PyrFloatArray* signal_difsqr_xx(VMGlobals* g, PyrFloatArray* ina, PyrFloatArray* inb) {
    BINOP_LOOP2(++a; ++b; *c = *a * *a - *b * *b;);
}

PyrFloatArray* signal_sumsqr_xx(VMGlobals* g, PyrFloatArray* ina, PyrFloatArray* inb) {
    BINOP_LOOP2(++a; ++b; *c = *a * *a + *b * *b;);
}

PyrFloatArray* signal_sqrsum_xx(VMGlobals* g, PyrFloatArray* ina, PyrFloatArray* inb) {
    float z;
    BINOP_LOOP2(z = *++a + *++b; *++c = z * z;);
}

PyrFloatArray* signal_sqrdif_xx(VMGlobals* g, PyrFloatArray* ina, PyrFloatArray* inb) {
    float z;
    BINOP_LOOP2(z = *++a - *++b; *++c = z * z;);
}

PyrFloatArray* signal_absdif_xx(VMGlobals* g, PyrFloatArray* ina, PyrFloatArray* inb) {
    BINOP_LOOP2(*++c = fabs(*++a - *++b););
}

PyrFloatArray* signal_add_xf(VMGlobals* g, PyrFloatArray* ina, float inb) {
    PyrFloatArray* outc = newPyrSignal(g, ina->size);
    if (inb == 0.f) {
        float* a = ina->f;
        memcpy(outc->f, a, ina->size * sizeof(float));
    } else {
        float* a = ina->f - 1;
        float* c = outc->f - 1;
        UNROLL_CODE(outc->size, c, *++c = *++a + inb;)
    }
    return outc;
}

PyrFloatArray* signal_sub_xf(VMGlobals* g, PyrFloatArray* ina, float inb) {
    PyrFloatArray* outc = newPyrSignal(g, ina->size);
    if (inb == 0.f) {
        float* a = ina->f;
        memcpy(outc->f, a, ina->size * sizeof(float));
    } else {
        float* a = ina->f - 1;
        float* c = outc->f - 1;
        UNROLL_CODE(outc->size, c, *++c = *++a - inb;)
    }
    return outc;
}

PyrFloatArray* signal_mul_xf(VMGlobals* g, PyrFloatArray* ina, float inb) {
    PyrFloatArray* outc = newPyrSignal(g, ina->size);
    if (inb == 1.f) {
        float* a = ina->f;
        memcpy(outc->f, a, ina->size * sizeof(float));
    } else {
        float* a = ina->f - 1;
        float* c = outc->f - 1;
        UNROLL_CODE(outc->size, c, *++c = *++a * inb;)
    }
    return outc;
}

PyrFloatArray* signal_mod_xf(VMGlobals* g, PyrFloatArray* ina, double inb) {
    PyrFloatArray* outc = newPyrSignal(g, ina->size);
    float* a = ina->f - 1;
    float* c = outc->f - 1;
    UNROLL_CODE(outc->size, c, ++a; *++c = sc_mod(static_cast<double>(*a), inb);)
    return outc;
}

PyrFloatArray* signal_pow_xf(VMGlobals* g, PyrFloatArray* ina, double inb) {
    PyrFloatArray* outc = newPyrSignal(g, ina->size);
    float* a = ina->f - 1;
    float* c = outc->f - 1;
    UNROLL_CODE(outc->size, c, ++a; *++c = sc_pow(static_cast<double>(*a), inb);)
    return outc;
}

PyrFloatArray* signal_ring1_xf(VMGlobals* g, PyrFloatArray* ina, float inb) {
    PyrFloatArray* outc = newPyrSignal(g, ina->size);
    float* a = ina->f - 1;
    float* c = outc->f - 1;
    UNROLL_CODE(outc->size, c, ++a; *++c = *a * inb + *a;)
    return outc;
}

PyrFloatArray* signal_ring2_xf(VMGlobals* g, PyrFloatArray* ina, float inb) {
    PyrFloatArray* outc = newPyrSignal(g, ina->size);
    float* a = ina->f - 1;
    float* c = outc->f - 1;
    UNROLL_CODE(outc->size, c, ++a; *++c = *a * inb + *a + inb;)
    return outc;
}

PyrFloatArray* signal_ring3_xf(VMGlobals* g, PyrFloatArray* ina, float inb) {
    PyrFloatArray* outc = newPyrSignal(g, ina->size);
    float* a = ina->f - 1;
    float* c = outc->f - 1;
    UNROLL_CODE(outc->size, c, ++a; *++c = *a * *a * inb;)
    return outc;
}

PyrFloatArray* signal_ring4_xf(VMGlobals* g, PyrFloatArray* ina, float inb) {
    PyrFloatArray* outc = newPyrSignal(g, ina->size);
    float* a = ina->f - 1;
    float* c = outc->f - 1;
    float inb2 = inb * inb;
    UNROLL_CODE(outc->size, c, ++a; *++c = *a * *a * inb + *a * inb2;)
    return outc;
}


PyrFloatArray* signal_thresh_xf(VMGlobals* g, PyrFloatArray* ina, float inb) {
    PyrFloatArray* outc = newPyrSignal(g, ina->size);
    float* a = ina->f - 1;
    float* c = outc->f - 1;
    UNROLL_CODE(outc->size, c, ++a; *++c = *a < inb ? 0.f : *a;)
    return outc;
}

PyrFloatArray* signal_amclip_xf(VMGlobals* g, PyrFloatArray* ina, float inb) {
    PyrFloatArray* res;
    PyrFloatArray* outc = newPyrSignal(g, ina->size);
    if (inb <= 0.f)
        res = signal_fill(outc, 0.f);
    else
        res = signal_scale(outc, inb);
    return res;
}

PyrFloatArray* signal_div_xf(VMGlobals* g, PyrFloatArray* ina, float inb) {
    PyrFloatArray* outc = newPyrSignal(g, ina->size);
    if (inb == 1.f) {
        float* a = ina->f;
        memcpy(outc->f, a, ina->size * sizeof(float));
    } else {
        float* a = ina->f - 1;
        float* c = outc->f - 1;
        inb = 1.f / inb;
        UNROLL_CODE(outc->size, c, *++c = *++a * inb;)
    }
    return outc;
}

PyrFloatArray* signal_difsqr_xf(VMGlobals* g, PyrFloatArray* ina, float inb) {
    PyrFloatArray* outc = newPyrSignal(g, ina->size);
    float* a = ina->f - 1;
    float* c = outc->f - 1;
    inb = inb * inb;
    UNROLL_CODE(outc->size, c, ++a; *++c = *a * *a - inb;)
    return outc;
}

PyrFloatArray* signal_sumsqr_xf(VMGlobals* g, PyrFloatArray* ina, float inb) {
    PyrFloatArray* outc = newPyrSignal(g, ina->size);
    float* a = ina->f - 1;
    float* c = outc->f - 1;
    inb = inb * inb;
    UNROLL_CODE(outc->size, c, ++a; *++c = *a * *a + inb;)
    return outc;
}

PyrFloatArray* signal_sqrsum_xf(VMGlobals* g, PyrFloatArray* ina, float inb) {
    PyrFloatArray* outc = newPyrSignal(g, ina->size);
    float* a = ina->f - 1;
    float* c = outc->f - 1;
    float temp;
    UNROLL_CODE(outc->size, c, ++a; temp = *a + inb; *++c = temp * temp;)
    return outc;
}

PyrFloatArray* signal_sqrdif_xf(VMGlobals* g, PyrFloatArray* ina, float inb) {
    PyrFloatArray* outc = newPyrSignal(g, ina->size);
    float* a = ina->f - 1;
    float* c = outc->f - 1;
    float temp;
    UNROLL_CODE(outc->size, c, ++a; temp = *a - inb; *++c = temp * temp;)
    return outc;
}

PyrFloatArray* signal_absdif_xf(VMGlobals* g, PyrFloatArray* ina, float inb) {
    PyrFloatArray* outc = newPyrSignal(g, ina->size);
    float* a = ina->f - 1;
    float* c = outc->f - 1;
    UNROLL_CODE(outc->size, c, ++a; *++c = fabs(*a - inb);)
    return outc;
}

PyrFloatArray* signal_mod_fx(VMGlobals* g, double ina, PyrFloatArray* inb) {
    PyrFloatArray* outc = newPyrSignal(g, inb->size);
    float* b = inb->f - 1;
    float* c = outc->f - 1;
    UNROLL_CODE(outc->size, c, ++b; *++c = sc_mod(ina, static_cast<double>(*b)););
    return outc;
}

PyrFloatArray* signal_pow_fx(VMGlobals* g, double ina, PyrFloatArray* inb) {
    PyrFloatArray* outc = newPyrSignal(g, inb->size);
    float* b = inb->f - 1;
    float* c = outc->f - 1;
    UNROLL_CODE(outc->size, c, ++b; *++c = sc_pow(ina, static_cast<double>(*b));)
    return outc;
}

PyrFloatArray* signal_ring1_fx(VMGlobals* g, float ina, PyrFloatArray* inb) {
    PyrFloatArray* outc = newPyrSignal(g, inb->size);
    float* b = inb->f - 1;
    float* c = outc->f - 1;
    UNROLL_CODE(outc->size, c, *++c = ina * *++b + ina;)
    return outc;
}

PyrFloatArray* signal_ring2_fx(VMGlobals* g, float ina, PyrFloatArray* inb) {
    PyrFloatArray* outc = newPyrSignal(g, inb->size);
    float* b = inb->f - 1;
    float* c = outc->f - 1;
    UNROLL_CODE(outc->size, c, ++b; *++c = ina * *b + ina + *b;)
    return outc;
}

PyrFloatArray* signal_ring3_fx(VMGlobals* g, float ina, PyrFloatArray* inb) {
    PyrFloatArray* outc = newPyrSignal(g, inb->size);
    float* b = inb->f - 1;
    float* c = outc->f - 1;
    float ina2 = ina * ina;
    UNROLL_CODE(outc->size, c, *++c = ina2 * *++b;)
    return outc;
}

PyrFloatArray* signal_ring4_fx(VMGlobals* g, float ina, PyrFloatArray* inb) {
    PyrFloatArray* outc = newPyrSignal(g, inb->size);
    float* b = inb->f - 1;
    float* c = outc->f - 1;
    float ina2 = ina * ina;
    UNROLL_CODE(outc->size, c, ++b; *++c = ina2 * *b - ina * *b * *b;)
    return outc;
}

PyrFloatArray* signal_thresh_fx(VMGlobals* g, float ina, PyrFloatArray* inb) {
    PyrFloatArray* outc = newPyrSignal(g, inb->size);
    float* b = inb->f - 1;
    float* c = outc->f - 1;
    UNROLL1_CODE(outc->size, c, ++b; *++c = ina < *b ? 0.f : ina;)
    return outc;
}

PyrFloatArray* signal_amclip_fx(VMGlobals* g, float ina, PyrFloatArray* inb) {
    PyrFloatArray* outc = newPyrSignal(g, inb->size);
    float* b = inb->f - 1;
    float* c = outc->f - 1;
    UNROLL1_CODE(outc->size, c, ++b; *++c = *b <= 0.f ? 0.f : ina * *b;)
    return outc;
}

PyrFloatArray* signal_sub_fx(VMGlobals* g, float ina, PyrFloatArray* inb) {
    PyrFloatArray* outc = newPyrSignal(g, inb->size);
    float* b = inb->f - 1;
    float* c = outc->f - 1;
    UNROLL_CODE(outc->size, c, *++c = ina - *++b;)
    return outc;
}

PyrFloatArray* signal_div_fx(VMGlobals* g, float ina, PyrFloatArray* inb) {
    PyrFloatArray* outc = newPyrSignal(g, inb->size);
    float* b = inb->f - 1;
    float* c = outc->f - 1;
    UNROLL_CODE(outc->size, c, *++c = ina / *++b;)
    return outc;
}

PyrFloatArray* signal_difsqr_fx(VMGlobals* g, float ina, PyrFloatArray* inb) {
    PyrFloatArray* outc = newPyrSignal(g, inb->size);
    float* b = inb->f - 1;
    float* c = outc->f - 1;
    ina = ina * ina;
    UNROLL_CODE(outc->size, c, ++b; *++c = ina - *b * *b;)
    return outc;
}

PyrFloatArray* signal_sumsqr_fx(VMGlobals* g, float ina, PyrFloatArray* inb) {
    PyrFloatArray* outc = newPyrSignal(g, inb->size);
    float* b = inb->f - 1;
    float* c = outc->f - 1;
    ina = ina * ina;
    UNROLL_CODE(outc->size, c, ++b; *++c = ina + *b * *b;)
    return outc;
}

PyrFloatArray* signal_sqrsum_fx(VMGlobals* g, float ina, PyrFloatArray* inb) {
    PyrFloatArray* outc = newPyrSignal(g, inb->size);
    float temp;
    float* b = inb->f - 1;
    float* c = outc->f - 1;
    UNROLL_CODE(outc->size, c, ++b; temp = ina + *b; *++c = temp * temp;)
    return outc;
}

PyrFloatArray* signal_sqrdif_fx(VMGlobals* g, float ina, PyrFloatArray* inb) {
    PyrFloatArray* outc = newPyrSignal(g, inb->size);
    float temp;
    float* b = inb->f - 1;
    float* c = outc->f - 1;
    UNROLL_CODE(outc->size, c, ++b; temp = ina - *b; *++c = temp * temp;)
    return outc;
}

PyrFloatArray* signal_absdif_fx(VMGlobals* g, float ina, PyrFloatArray* inb) {
    PyrFloatArray* outc = newPyrSignal(g, inb->size);
    float* b = inb->f - 1;
    float* c = outc->f - 1;
    UNROLL_CODE(outc->size, c, ++b; *++c = fabs(ina - *b);)
    return outc;
}

PyrFloatArray* signal_min_xx(VMGlobals* g, PyrFloatArray* ina, PyrFloatArray* inb) {
    BINOP_LOOP2(++a; ++b; *++c = *a < *b ? *a : *b;);
}

PyrFloatArray* signal_max_xx(VMGlobals* g, PyrFloatArray* ina, PyrFloatArray* inb) {
    BINOP_LOOP2(++a; ++b; *++c = *a > *b ? *a : *b;);
}

PyrFloatArray* signal_scaleneg_xx(VMGlobals* g, PyrFloatArray* ina, PyrFloatArray* inb) {
    BINOP_LOOP2(++a; ++b; *++c = *a < 0.f ? *a * *b : *a;);
}

PyrFloatArray* signal_clip2_xx(VMGlobals* g, PyrFloatArray* ina, PyrFloatArray* inb) {
    BINOP_LOOP2(++a; ++b; *++c = *a > *b ? *b : (*a < -*b ? -*b : *a););
}

PyrFloatArray* signal_fold2_xx(VMGlobals* g, PyrFloatArray* ina, PyrFloatArray* inb) {
    BINOP_LOOP2(++a; ++b; *++c = sc_fold(*a, -*b, *b););
}

PyrFloatArray* signal_wrap2_xx(VMGlobals* g, PyrFloatArray* ina, PyrFloatArray* inb) {
    BINOP_LOOP2(++a; ++b; *++c = sc_wrap(*a, -*b, *b););
}

PyrFloatArray* signal_excess_xx(VMGlobals* g, PyrFloatArray* ina, PyrFloatArray* inb) {
    BINOP_LOOP2(++a; ++b; *++c = *a > *b ? *a - *b : (*a < -*b ? *a + *b : 0.f););
}

bool signal_equal_xx(VMGlobals* g, PyrFloatArray* ina, PyrFloatArray* inb) {
    if (ina->size != inb->size)
        return false;
    float* a = ina->f;
    float* b = inb->f;
    for (int i = 0; i < ina->size; i++) {
        if (a[i] != b[i]) {
            return false;
        }
    }
    return true;
}

PyrFloatArray* signal_min_xf(VMGlobals* g, PyrFloatArray* ina, float inb) {
    PyrFloatArray* outc = newPyrSignal(g, ina->size);
    float* a = ina->f - 1;
    float* c = outc->f - 1;
    UNROLL1_CODE(outc->size, c, ++a; *++c = *a < inb ? *a : inb;)
    return outc;
}

PyrFloatArray* signal_max_xf(VMGlobals* g, PyrFloatArray* ina, float inb) {
    PyrFloatArray* outc = newPyrSignal(g, ina->size);
    float* a = ina->f - 1;
    float* c = outc->f - 1;
    UNROLL1_CODE(outc->size, c, ++a; *++c = *a > inb ? *a : inb;)
    return outc;
}

PyrFloatArray* signal_scaleneg_xf(VMGlobals* g, PyrFloatArray* ina, float inb) {
    PyrFloatArray* outc = newPyrSignal(g, ina->size);
    float* a = ina->f - 1;
    float* c = outc->f - 1;
    UNROLL1_CODE(outc->size, c, ++a; *++c = *a < 0.f ? *a * inb : *a;)
    return outc;
}

PyrFloatArray* signal_clip2_xf(VMGlobals* g, PyrFloatArray* ina, float inb) {
    PyrFloatArray* outc = newPyrSignal(g, ina->size);
    float* a = ina->f - 1;
    float* c = outc->f - 1;
    UNROLL1_CODE(outc->size, c, ++a; *++c = *a > inb ? inb : (*a < -inb ? -inb : *a);)
    return outc;
}

PyrFloatArray* signal_fold2_xf(VMGlobals* g, PyrFloatArray* ina, float inb) {
    PyrFloatArray* outc = newPyrSignal(g, ina->size);
    float* a = ina->f - 1;
    float* c = outc->f - 1;
    UNROLL1_CODE(outc->size, c, ++a; *++c = sc_fold(*a, -inb, inb);)
    return outc;
}

PyrFloatArray* signal_wrap2_xf(VMGlobals* g, PyrFloatArray* ina, float inb) {
    PyrFloatArray* outc = newPyrSignal(g, ina->size);
    float* a = ina->f - 1;
    float* c = outc->f - 1;
    UNROLL1_CODE(outc->size, c, ++a; *++c = sc_wrap(*a, -inb, inb);)
    return outc;
}

PyrFloatArray* signal_excess_xf(VMGlobals* g, PyrFloatArray* ina, float inb) {
    PyrFloatArray* outc = newPyrSignal(g, ina->size);
    float* a = ina->f - 1;
    float* c = outc->f - 1;
    UNROLL1_CODE(outc->size, c, ++a; *++c = *a > inb ? *a - inb : (*a < -inb ? *a + inb : 0.f);)
    return outc;
}

PyrFloatArray* signal_scaleneg_fx(VMGlobals* g, float ina, PyrFloatArray* inb) {
    PyrFloatArray* outc = newPyrSignal(g, inb->size);
    float* b = inb->f - 1;
    float* c = outc->f - 1;
    if (ina < 0.f) {
        UNROLL1_CODE(outc->size, c, *++c = ina * *++b;);
    } else {
        UNROLL1_CODE(outc->size, c, *++c = ina;);
    }
    return outc;
}

PyrFloatArray* signal_clip2_fx(VMGlobals* g, float ina, PyrFloatArray* inb) {
    PyrFloatArray* outc = newPyrSignal(g, inb->size);
    float* b = inb->f - 1;
    float* c = outc->f - 1;
    UNROLL1_CODE(outc->size, c, ++b; *++c = ina > *b ? *b : (ina < -*b ? -*b : ina);)
    return outc;
}

PyrFloatArray* signal_fold2_fx(VMGlobals* g, float ina, PyrFloatArray* inb) {
    PyrFloatArray* outc = newPyrSignal(g, inb->size);
    float* b = inb->f - 1;
    float* c = outc->f - 1;
    UNROLL1_CODE(outc->size, c, ++b; *++c = sc_fold(ina, -*b, *b);)
    return outc;
}

PyrFloatArray* signal_wrap2_fx(VMGlobals* g, float ina, PyrFloatArray* inb) {
    PyrFloatArray* outc = newPyrSignal(g, inb->size);
    float* b = inb->f - 1;
    float* c = outc->f - 1;
    UNROLL1_CODE(outc->size, c, ++b; *++c = sc_wrap(ina, -*b, *b);)
    return outc;
}

PyrFloatArray* signal_excess_fx(VMGlobals* g, float ina, PyrFloatArray* inb) {
    PyrFloatArray* outc = newPyrSignal(g, inb->size);
    float* b = inb->f - 1;
    float* c = outc->f - 1;
    UNROLL1_CODE(outc->size, c, ++b; *++c = ina > *b ? ina - *b : (ina < -*b ? ina + *b : 0.f);)
    return outc;
}

PyrFloatArray* signal_invert(VMGlobals* g, PyrFloatArray* inPyrSignal) {
    float* in = inPyrSignal->f - 1;
    PyrFloatArray* outc = newPyrSignal(g, inPyrSignal->size);
    float* out = outc->f - 1;
    UNROLL_CODE(inPyrSignal->size, out, *++out = -*++in;)
    return outc;
}

PyrFloatArray* signal_recip(VMGlobals* g, PyrFloatArray* inPyrSignal) {
    float* in = inPyrSignal->f - 1;
    PyrFloatArray* outc = newPyrSignal(g, inPyrSignal->size);
    float* out = outc->f - 1;
    UNROLL_CODE(inPyrSignal->size, out, *++out = 1.f / *++in;)
    return outc;
}

PyrFloatArray* signal_squared(VMGlobals* g, PyrFloatArray* inPyrSignal) {
    float* in = inPyrSignal->f - 1;
    PyrFloatArray* outc = newPyrSignal(g, inPyrSignal->size);
    float* out = outc->f - 1;
    UNROLL_CODE(inPyrSignal->size, out, ++in; *++out = *in * *in;)
    return outc;
}

PyrFloatArray* signal_cubed(VMGlobals* g, PyrFloatArray* inPyrSignal) {
    float* in = inPyrSignal->f - 1;
    PyrFloatArray* outc = newPyrSignal(g, inPyrSignal->size);
    float* out = outc->f - 1;
    UNROLL_CODE(inPyrSignal->size, out, ++in; *++out = *in * *in * *in;)
    return outc;
}

// PowerPC has a fast fabs instruction
PyrFloatArray* signal_abs(VMGlobals* g, PyrFloatArray* inPyrSignal) {
    float* in = inPyrSignal->f - 1;
    PyrFloatArray* outc = newPyrSignal(g, inPyrSignal->size);
    float* out = outc->f - 1;
    UNROLL_CODE(inPyrSignal->size, out, *++out = fabs(*++in);)
    return outc;
}

float signal_findpeak(PyrFloatArray* inPyrSignal) {
    float* out = inPyrSignal->f - 1;
    float peak = 0.f;
    float z;
    UNROLL1_CODE(inPyrSignal->size, out, ++out; z = fabs(*out); peak = z > peak ? z : peak;)
    return peak;
}

PyrFloatArray* signal_normalize_transfer_fn(PyrFloatArray* inPyrSignal) {
    float *out, scale, offset, x, maxval;
    std::int64_t length, halflength;

    maxval = 0.0;
    out = inPyrSignal->f - 1;
    length = inPyrSignal->size;
    halflength = length >> 1;
    offset = (out[halflength - 1] + out[halflength]) * 0.5;

    UNROLL1_CODE(inPyrSignal->size, out, ++out; x = *out - offset; x = (x < 0.) ? -x : x; if (x > maxval) maxval = x;);
    if (maxval) {
        out = inPyrSignal->f - 1;
        scale = 1.0 / maxval;
        UNROLL1_CODE(inPyrSignal->size, out, ++out; *out = (*out - offset) * scale;);
    }
    return inPyrSignal;
}


float signal_integral(PyrFloatArray* inPyrSignal) {
    float* out = inPyrSignal->f - 1;
    float sum = 0.f;
    UNROLL1_CODE(inPyrSignal->size, out, sum += *++out;)
    return sum;
}

PyrFloatArray* signal_sign(VMGlobals* g, PyrFloatArray* inPyrSignal) {
    float* in = inPyrSignal->f - 1;
    PyrFloatArray* outc = newPyrSignal(g, inPyrSignal->size);
    float* out = outc->f - 1;
    UNROLL1_CODE(inPyrSignal->size, out, ++in; *++out = *in < 0.f ? -1.f : (*in > 0.f ? 1.f : 0.f);)
    return outc;
}

PyrFloatArray* signal_negative(VMGlobals* g, PyrFloatArray* inPyrSignal) {
    float* in = inPyrSignal->f - 1;
    PyrFloatArray* outc = newPyrSignal(g, inPyrSignal->size);
    float* out = outc->f - 1;
    UNROLL1_CODE(inPyrSignal->size, out, *++out = *++in < 0.f ? 1.f : 0.f;)
    return outc;
}

PyrFloatArray* signal_positive(VMGlobals* g, PyrFloatArray* inPyrSignal) {
    float* in = inPyrSignal->f - 1;
    PyrFloatArray* outc = newPyrSignal(g, inPyrSignal->size);
    float* out = outc->f - 1;
    UNROLL1_CODE(inPyrSignal->size, out, *++out = *++in >= 0.f ? 1.f : 0.f;)
    return outc;
}

PyrFloatArray* signal_strictly_positive(VMGlobals* g, PyrFloatArray* inPyrSignal) {
    float* in = inPyrSignal->f - 1;
    PyrFloatArray* outc = newPyrSignal(g, inPyrSignal->size);
    float* out = outc->f - 1;
    UNROLL1_CODE(inPyrSignal->size, out, *++out = *++in > 0.f ? 1.f : 0.f;)
    return outc;
}

PyrFloatArray* signal_nyqring(VMGlobals* g, PyrFloatArray* inPyrSignal) {
    float* in = inPyrSignal->f - 1;
    PyrFloatArray* outc = newPyrSignal(g, inPyrSignal->size);
    float* out = outc->f - 1;
    float* endptr = out + inPyrSignal->size;
    switch (inPyrSignal->size & 3) {
        while (out < endptr) {
            *++out = -*++in;
        case 3:
            *++out = *++in;
        case 2:
            *++out = -*++in;
        case 1:
            *++out = *++in;
        case 0:;
        }
    }
    return outc;
}

PyrFloatArray* signal_clip_f(VMGlobals* g, PyrFloatArray* inPyrSignal, float lo, float hi) {
    float* in = inPyrSignal->f - 1;
    PyrFloatArray* outc = newPyrSignal(g, inPyrSignal->size);
    float* out = outc->f - 1;
    float* endptr = out + inPyrSignal->size;
    while (out < endptr) {
        ++in;
        *++out = *in < lo ? lo : (*in > hi ? hi : *in);
    }
    return outc;
}

PyrFloatArray* signal_clip_x(VMGlobals* g, PyrFloatArray* ina, PyrFloatArray* inb, PyrFloatArray* inc) {
    PyrFloatArray* outd;
    float *a, *b, *c, *d, *endptr;
    std::int64_t minsize = sc_min(ina->size, inb->size);
    minsize = sc_min(minsize, inc->size);
    outd = newPyrSignal(g, minsize);
    a = ina->f - 1;
    b = inb->f - 1;
    c = inc->f - 1;
    d = outd->f - 1;
    endptr = a + minsize;
    UNROLL1_CODE(outd->size, d, ++a; ++b; ++c; *++d = *a < *b ? *b : (*a > *c ? *c : *a););
    return outd;
}


/// WRAP AND FOLD ARE STILL INCORRECT !

PyrFloatArray* signal_wrap_f(VMGlobals* g, PyrFloatArray* inPyrSignal, float lo, float hi) {
    float* out = inPyrSignal->f - 1;
    float* endptr = out + inPyrSignal->size;
    while (out < endptr) {
        ++out;
        *out = sc_wrap(*out, lo, hi);
    }
    return inPyrSignal;
}

PyrFloatArray* signal_wrap_x(VMGlobals* g, PyrFloatArray* ina, PyrFloatArray* inb, PyrFloatArray* inc) {
    PyrFloatArray* outd;
    float *a, *b, *c, *d, *endptr;
    std::int64_t minsize = sc_min(ina->size, inb->size);
    minsize = sc_min(minsize, inc->size);
    outd = newPyrSignal(g, minsize);
    a = ina->f - 1;
    b = inb->f - 1;
    c = inc->f - 1;
    d = outd->f - 1;
    endptr = d + outd->size;
    while (d < endptr) {
        a++;
        b++;
        c++;
        d++;
        ++d;
        *d = sc_wrap(*a, *b, *c);
    }
    return outd;
}

PyrFloatArray* signal_fold_f(VMGlobals* g, PyrFloatArray* inPyrSignal, float lo, float hi) {
    float* out = inPyrSignal->f - 1;
    float* endptr = out + inPyrSignal->size;
    while (out < endptr) {
        ++out;
        *out = sc_fold(*out, lo, hi);
    }
    return inPyrSignal;
}

PyrFloatArray* signal_fold_x(VMGlobals* g, PyrFloatArray* ina, PyrFloatArray* inb, PyrFloatArray* inc) {
    PyrFloatArray* outd;
    float *a, *b, *c, *d, *endptr;
    std::int64_t minsize = sc_min(ina->size, inb->size);
    minsize = sc_min(minsize, inc->size);
    outd = newPyrSignal(g, minsize);
    a = ina->f - 1;
    b = inb->f - 1;
    c = inc->f - 1;
    d = outd->f - 1;
    endptr = d + outd->size;
    while (d < endptr) {
        a++;
        b++;
        c++;
        d++;
        *d = sc_fold(*a, *b, *c);
    }
    return outd;
}

PyrFloatArray* signal_log(VMGlobals* g, PyrFloatArray* inPyrSignal) {
    float* in = inPyrSignal->f - 1;
    PyrFloatArray* outc = newPyrSignal(g, inPyrSignal->size);
    float* out = outc->f - 1;
    UNROLL1_CODE(inPyrSignal->size, out, *++out = log(*++in);)
    return outc;
}

PyrFloatArray* signal_log2(VMGlobals* g, PyrFloatArray* inPyrSignal) {
    float* in = inPyrSignal->f - 1;
    PyrFloatArray* outc = newPyrSignal(g, inPyrSignal->size);
    float* out = outc->f - 1;
    UNROLL1_CODE(inPyrSignal->size, out, *++out = sc_log2(*++in);)
    return outc;
}

PyrFloatArray* signal_log10(VMGlobals* g, PyrFloatArray* inPyrSignal) {
    float* in = inPyrSignal->f - 1;
    PyrFloatArray* outc = newPyrSignal(g, inPyrSignal->size);
    float* out = outc->f - 1;
    UNROLL1_CODE(inPyrSignal->size, out, *++out = log10(*++in);)
    return outc;
}


PyrFloatArray* signal_sin(VMGlobals* g, PyrFloatArray* inPyrSignal) {
    float* in = inPyrSignal->f - 1;
    PyrFloatArray* outc = newPyrSignal(g, inPyrSignal->size);
    float* out = outc->f - 1;
    UNROLL1_CODE(inPyrSignal->size, out, *++out = sin(*++in);)
    return outc;
}

PyrFloatArray* signal_cos(VMGlobals* g, PyrFloatArray* inPyrSignal) {
    float* in = inPyrSignal->f - 1;
    PyrFloatArray* outc = newPyrSignal(g, inPyrSignal->size);
    float* out = outc->f - 1;
    UNROLL1_CODE(inPyrSignal->size, out, *++out = cos(*++in);)
    return outc;
}

PyrFloatArray* signal_tan(VMGlobals* g, PyrFloatArray* inPyrSignal) {
    float* in = inPyrSignal->f - 1;
    PyrFloatArray* outc = newPyrSignal(g, inPyrSignal->size);
    float* out = outc->f - 1;
    UNROLL1_CODE(inPyrSignal->size, out, *++out = tan(*++in);)
    return outc;
}

PyrFloatArray* signal_sinh(VMGlobals* g, PyrFloatArray* inPyrSignal) {
    float* in = inPyrSignal->f - 1;
    PyrFloatArray* outc = newPyrSignal(g, inPyrSignal->size);
    float* out = outc->f - 1;
    UNROLL1_CODE(inPyrSignal->size, out, *++out = sinh(*++in);)
    return outc;
}

PyrFloatArray* signal_cosh(VMGlobals* g, PyrFloatArray* inPyrSignal) {
    float* in = inPyrSignal->f - 1;
    PyrFloatArray* outc = newPyrSignal(g, inPyrSignal->size);
    float* out = outc->f - 1;
    UNROLL1_CODE(inPyrSignal->size, out, *++out = cosh(*++in);)
    return outc;
}

PyrFloatArray* signal_tanh(VMGlobals* g, PyrFloatArray* inPyrSignal) {
    float* in = inPyrSignal->f - 1;
    PyrFloatArray* outc = newPyrSignal(g, inPyrSignal->size);
    float* out = outc->f - 1;
    UNROLL1_CODE(inPyrSignal->size, out, *++out = tanh(*++in);)
    return outc;
}

PyrFloatArray* signal_tanh_ds(PyrFloatArray* inPyrSignal) {
    float* out = inPyrSignal->f - 1;
    UNROLL1_CODE(inPyrSignal->size, out, ++out; *out = tanh(*out);)
    return inPyrSignal;
}

PyrFloatArray* signal_asin(VMGlobals* g, PyrFloatArray* inPyrSignal) {
    float* in = inPyrSignal->f - 1;
    PyrFloatArray* outc = newPyrSignal(g, inPyrSignal->size);
    float* out = outc->f - 1;
    UNROLL1_CODE(inPyrSignal->size, out, *++out = asin(*++in);)
    return outc;
}

PyrFloatArray* signal_acos(VMGlobals* g, PyrFloatArray* inPyrSignal) {
    float* in = inPyrSignal->f - 1;
    PyrFloatArray* outc = newPyrSignal(g, inPyrSignal->size);
    float* out = outc->f - 1;
    UNROLL1_CODE(inPyrSignal->size, out, *++out = acos(*++in);)
    return outc;
}

PyrFloatArray* signal_atan(VMGlobals* g, PyrFloatArray* inPyrSignal) {
    float* in = inPyrSignal->f - 1;
    PyrFloatArray* outc = newPyrSignal(g, inPyrSignal->size);
    float* out = outc->f - 1;
    UNROLL1_CODE(inPyrSignal->size, out, *++out = atan(*++in);)
    return outc;
}

PyrFloatArray* signal_exp(VMGlobals* g, PyrFloatArray* inPyrSignal) {
    float* in = inPyrSignal->f - 1;
    PyrFloatArray* outc = newPyrSignal(g, inPyrSignal->size);
    float* out = outc->f - 1;
    UNROLL1_CODE(inPyrSignal->size, out, *++out = exp(*++in);)
    return outc;
}

PyrFloatArray* signal_sqrt(VMGlobals* g, PyrFloatArray* inPyrSignal) {
    float* in = inPyrSignal->f - 1;
    PyrFloatArray* outc = newPyrSignal(g, inPyrSignal->size);
    float* out = outc->f - 1;
    UNROLL1_CODE(inPyrSignal->size, out, *++out = sqrt(*++in);)
    return outc;
}

PyrFloatArray* signal_distort(VMGlobals* g, PyrFloatArray* inPyrSignal) {
    float* in = inPyrSignal->f - 1;
    PyrFloatArray* outc = newPyrSignal(g, inPyrSignal->size);
    float* out = outc->f - 1;
    float* endptr = out + inPyrSignal->size;
    while (out < endptr) {
        float z = *++in;
        if (z < 0.f)
            *++out = z / (1.f - z);
        else
            *++out = z / (1.f + z);
    }
    return outc;
}

PyrFloatArray* signal_softclip(VMGlobals* g, PyrFloatArray* inPyrSignal) {
    float* in = inPyrSignal->f - 1;
    PyrFloatArray* outc = newPyrSignal(g, inPyrSignal->size);
    float* out = outc->f - 1;
    float* endptr = out + inPyrSignal->size;
    while (out < endptr) {
        float z = *++in;
        if (z < -0.5f)
            *++out = (-z - .25f) / z;
        else if (z > 0.5f)
            *++out = (z - .25f) / z;
        else
            *++out = z;
    }
    return outc;
}


PyrFloatArray* signal_rotate(VMGlobals* g, PyrFloatArray* ina, int rot) {
    std::int64_t i, j;
    PyrFloatArray* outc = newPyrSignal(g, ina->size);
    float* a0 = ina->f - 1;
    //	float *a = a0 + sc_mod(rot, ina->size);
    float* a = a0 + sc_mod(0 - rot, ina->size);
    float* aend = a0 + ina->size;
    float* c = outc->f - 1;
    std::int64_t nsmps = outc->size;
    for (i = 0, j = rot; i < nsmps; i++, j++) {
        *++c = *++a;
        if (a >= aend)
            a = a0;
    }
    return outc;
}

PyrFloatArray* signal_reverse_range(PyrFloatArray* ina, std::int64_t start, std::int64_t end) {
    std::int64_t size = ina->size;
    std::int64_t size2;
    std::int64_t i;
    start = sc_max(0, start);
    end = sc_min(end + 1, size);
    size = end - start;
    size2 = size >> 1;
    float* a = ina->f - 1 + start;
    //	float *b = ina->f - 1 + end;
    float* b = ina->f + end;
    float temp;
    for (i = 0; i < size2; ++i) {
        temp = *++a;
        *a = *--b;
        *b = temp;
    }
    return ina;
}

PyrFloatArray* signal_normalize_range(PyrFloatArray* ina, std::int64_t start, std::int64_t end) {
    std::int64_t size = ina->size;
    std::int64_t i;
    float z, scale, maxlevel;
    float *a0, *a;
    start = sc_max(0, start);
    end = sc_min(end + 1, size);
    size = end - start;
    a0 = ina->f - 1 + start;
    a = a0;
    maxlevel = 0.f;
    for (i = 0; i < size; ++i) {
        z = fabs(*++a);
        if (z > maxlevel)
            maxlevel = z;
    }
    a = a0;
    if (maxlevel != 0.f) {
        scale = 1. / maxlevel;
        for (i = 0; i < size; ++i) {
            z = *++a;
            *a = scale * z;
        }
    }
    return ina;
}

PyrFloatArray* signal_invert_range(PyrFloatArray* ina, std::int64_t start, std::int64_t end) {
    std::int64_t size = ina->size;
    std::int64_t i;
    float z;
    start = sc_max(0, start);
    end = sc_min(end, size);
    size = end - start + 1;
    float* a = ina->f - 1 + start;

    for (i = 0; i < size; ++i) {
        z = *++a;
        *a = -z;
    }
    return ina;
}


PyrFloatArray* signal_fade_range(PyrFloatArray* ina, std::int64_t start, std::int64_t end, float lvl0, float lvl1) {
    std::int64_t size = ina->size;
    std::int64_t i;
    float z, level, slope;
    start = sc_max(0, start);
    end = sc_min(end + 1, size);
    size = end - start;
    float* a = ina->f - 1 + start;
    slope = (lvl1 - lvl0) / size;
    level = lvl0;
    for (i = 0; i < size; ++i) {
        z = *++a;
        *a = z * level;
        level += slope;
    }
    return ina;
}


PyrFloatArray* signal_overdub(VMGlobals* g, PyrFloatArray* ina, PyrFloatArray* inb, std::int64_t index) {
    float *a, *b;
    std::int64_t len;

    if (index > 0) {
        a = ina->f + index - 1;
        b = inb->f - 1;
        len = sc_min(inb->size, ina->size - index);
    } else {
        a = ina->f - 1;
        b = inb->f - index - 1;
        len = sc_min(ina->size, inb->size + index);
    }

    UNROLL_CODE(len, a, *++a += *++b;);

    return ina;
}

PyrFloatArray* signal_overwrite(VMGlobals* g, PyrFloatArray* ina, PyrFloatArray* inb, std::int64_t index) {
    float *a, *b;
    std::int64_t len;
    if (index > 0) {
        a = ina->f + index - 1;
        b = inb->f - 1;
        len = sc_min(inb->size, ina->size - index);
    } else {
        a = ina->f - 1;
        b = inb->f - index - 1;
        len = sc_min(ina->size, inb->size + index);
    }

    UNROLL_CODE(len, a, *++a = *++b;);

    return ina;
}
