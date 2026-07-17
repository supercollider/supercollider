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

#pragma once

#include "PyrObject.h"
#include "GC.h"

#define UNROLL 1

PyrFloatArray* newPyrSignal(VMGlobals* g, std::int64_t size);

#define UNROLL8_CODE(size, var, stmt)                                                                                  \
    {                                                                                                                  \
        int tempi, tempend;                                                                                            \
        tempend = size >> 3;                                                                                           \
        for (tempi = 0; tempi < tempend; ++tempi) {                                                                    \
            stmt;                                                                                                      \
            stmt;                                                                                                      \
            stmt;                                                                                                      \
            stmt;                                                                                                      \
            stmt;                                                                                                      \
            stmt;                                                                                                      \
            stmt;                                                                                                      \
            stmt;                                                                                                      \
        }                                                                                                              \
        tempend = size & 7;                                                                                            \
        for (tempi = 0; tempi < tempend; ++tempi) {                                                                    \
            stmt;                                                                                                      \
        }                                                                                                              \
    }

#define UNROLL4_CODE(size, var, stmt)                                                                                  \
    {                                                                                                                  \
        int tempi, tempend;                                                                                            \
        tempend = size >> 2;                                                                                           \
        for (tempi = 0; tempi < tempend; ++tempi) {                                                                    \
            stmt;                                                                                                      \
            stmt;                                                                                                      \
            stmt;                                                                                                      \
            stmt;                                                                                                      \
        }                                                                                                              \
        tempend = size & 3;                                                                                            \
        for (tempi = 0; tempi < tempend; ++tempi) {                                                                    \
            stmt;                                                                                                      \
        }                                                                                                              \
    }

#define FILTER_LOOP(size, var, stmt, stmt2)                                                                            \
    {                                                                                                                  \
        int tempi, tempend;                                                                                            \
        tempend = size >> 2;                                                                                           \
        for (tempi = 0; tempi < tempend; ++tempi) {                                                                    \
            stmt;                                                                                                      \
            stmt;                                                                                                      \
            stmt;                                                                                                      \
            stmt;                                                                                                      \
            stmt2;                                                                                                     \
        }                                                                                                              \
        tempend = size & 3;                                                                                            \
        for (tempi = 0; tempi < tempend; ++tempi) {                                                                    \
            stmt;                                                                                                      \
        }                                                                                                              \
    }

#define UNROLL1_CODE(size, var, stmt)                                                                                  \
    {                                                                                                                  \
        int tempi, tempend;                                                                                            \
        tempend = size;                                                                                                \
        for (tempi = 0; tempi < tempend; ++tempi) {                                                                    \
            stmt;                                                                                                      \
        }                                                                                                              \
    }


#if UNROLL == 8
#    define UNROLL_CODE UNROLL8_CODE
#elif UNROLL == 4
#    define UNROLL_CODE UNROLL4_CODE
#else
#    define UNROLL_CODE UNROLL1_CODE
#endif

#if 0

#    define BINOP_LOOP1(OP)
#    define BINOP_LOOP2(STMT1, STMT2, STMT3)

#else

#    define BINOP_LOOP1(OP)                                                                                            \
        float *a, *b, *c, *endptr;                                                                                     \
        PyrFloatArray* outc;                                                                                           \
        std::int64_t size;                                                                                             \
        a = ina->f - 1;                                                                                                \
        b = inb->f - 1;                                                                                                \
        size = sc_min(ina->size, inb->size);                                                                           \
        outc = newPyrSignal(g, size);                                                                                  \
        c = outc->f - 1;                                                                                               \
        endptr = c + size;                                                                                             \
        switch (size & 3) {                                                                                            \
            while (c < endptr) {                                                                                       \
                *++c = *++a OP * ++b;                                                                                  \
            case 3:                                                                                                    \
                *++c = *++a OP * ++b;                                                                                  \
            case 2:                                                                                                    \
                *++c = *++a OP * ++b;                                                                                  \
            case 1:                                                                                                    \
                *++c = *++a OP * ++b;                                                                                  \
            case 0:;                                                                                                   \
            }                                                                                                          \
        }                                                                                                              \
        return outc;


#    define BINOP_LOOP2(STMT1)                                                                                         \
        float *a, *b, *c, *endptr;                                                                                     \
        PyrFloatArray* outc;                                                                                           \
        std::int64_t size;                                                                                             \
        a = ina->f - 1;                                                                                                \
        b = inb->f - 1;                                                                                                \
        size = sc_min(ina->size, inb->size);                                                                           \
        outc = newPyrSignal(g, size);                                                                                  \
        c = outc->f - 1;                                                                                               \
        endptr = c + size;                                                                                             \
        switch (size & 3) {                                                                                            \
            while (c < endptr) {                                                                                       \
                STMT1;                                                                                                 \
            case 3:                                                                                                    \
                STMT1;                                                                                                 \
            case 2:                                                                                                    \
                STMT1;                                                                                                 \
            case 1:                                                                                                    \
                STMT1;                                                                                                 \
            case 0:;                                                                                                   \
            }                                                                                                          \
        }                                                                                                              \
        return outc;

#endif

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

void signal_init_globs();
PyrFloatArray* signal_fill(PyrFloatArray* outSignal, float inValue);
PyrFloatArray* signal_scale(PyrFloatArray* outSignal, float inValue);
PyrFloatArray* signal_offset(PyrFloatArray* outSignal, float inValue);
PyrFloatArray* signal_scale_offset(PyrFloatArray* outSignal, float mul, float add);
PyrFloatArray* signal_mix(PyrFloatArray* ina, PyrFloatArray* inb, float start, float end, float slopeFactor);
PyrFloatArray* signal_add_xx(VMGlobals* g, PyrFloatArray* ina, PyrFloatArray* inb);
PyrFloatArray* signal_sub_xx(VMGlobals* g, PyrFloatArray* ina, PyrFloatArray* inb);
PyrFloatArray* signal_mul_xx(VMGlobals* g, PyrFloatArray* ina, PyrFloatArray* inb);
PyrFloatArray* signal_mul_ds_xx(VMGlobals* g, PyrFloatArray* ina, PyrFloatArray* inb);
PyrFloatArray* signal_add_ds_xx(VMGlobals* g, PyrFloatArray* ina, PyrFloatArray* inb);
PyrFloatArray* signal_sub_ds_xx(VMGlobals* g, PyrFloatArray* ina, PyrFloatArray* inb);
PyrFloatArray* signal_ring1_xx(VMGlobals* g, PyrFloatArray* ina, PyrFloatArray* inb);
PyrFloatArray* signal_ring2_xx(VMGlobals* g, PyrFloatArray* ina, PyrFloatArray* inb);
PyrFloatArray* signal_ring3_xx(VMGlobals* g, PyrFloatArray* ina, PyrFloatArray* inb);
PyrFloatArray* signal_ring4_xx(VMGlobals* g, PyrFloatArray* ina, PyrFloatArray* inb);
PyrFloatArray* signal_thresh_xx(VMGlobals* g, PyrFloatArray* ina, PyrFloatArray* inb);
PyrFloatArray* signal_amclip_xx(VMGlobals* g, PyrFloatArray* ina, PyrFloatArray* inb);
PyrFloatArray* signal_div_xx(VMGlobals* g, PyrFloatArray* ina, PyrFloatArray* inb);
PyrFloatArray* signal_difsqr_xx(VMGlobals* g, PyrFloatArray* ina, PyrFloatArray* inb);
PyrFloatArray* signal_sumsqr_xx(VMGlobals* g, PyrFloatArray* ina, PyrFloatArray* inb);
PyrFloatArray* signal_sqrsum_xx(VMGlobals* g, PyrFloatArray* ina, PyrFloatArray* inb);
PyrFloatArray* signal_sqrdif_xx(VMGlobals* g, PyrFloatArray* ina, PyrFloatArray* inb);
PyrFloatArray* signal_add_xf(VMGlobals* g, PyrFloatArray* ina, float inb);
PyrFloatArray* signal_sub_xf(VMGlobals* g, PyrFloatArray* ina, float inb);
PyrFloatArray* signal_mul_xf(VMGlobals* g, PyrFloatArray* ina, float inb);
PyrFloatArray* signal_ring1_xf(VMGlobals* g, PyrFloatArray* ina, float inb);
PyrFloatArray* signal_pow_xf(VMGlobals* g, PyrFloatArray* ina, double inb);
PyrFloatArray* signal_pow_xx(VMGlobals* g, PyrFloatArray* ina, PyrFloatArray* inb);
PyrFloatArray* signal_pow_fx(VMGlobals* g, double ina, PyrFloatArray* inb);
PyrFloatArray* signal_ring2_xf(VMGlobals* g, PyrFloatArray* ina, float inb);
PyrFloatArray* signal_ring3_xf(VMGlobals* g, PyrFloatArray* ina, float inb);
PyrFloatArray* signal_ring4_xf(VMGlobals* g, PyrFloatArray* ina, float inb);
PyrFloatArray* signal_thresh_xf(VMGlobals* g, PyrFloatArray* ina, float inb);
PyrFloatArray* signal_amclip_xf(VMGlobals* g, PyrFloatArray* ina, float inb);
PyrFloatArray* signal_div_xf(VMGlobals* g, PyrFloatArray* ina, float inb);
PyrFloatArray* signal_difsqr_xf(VMGlobals* g, PyrFloatArray* ina, float inb);
PyrFloatArray* signal_sumsqr_xf(VMGlobals* g, PyrFloatArray* ina, float inb);
PyrFloatArray* signal_sqrsum_xf(VMGlobals* g, PyrFloatArray* ina, float inb);
PyrFloatArray* signal_sqrdif_xf(VMGlobals* g, PyrFloatArray* ina, float inb);
PyrFloatArray* signal_ring1_fx(VMGlobals* g, float ina, PyrFloatArray* inb);
PyrFloatArray* signal_ring2_fx(VMGlobals* g, float ina, PyrFloatArray* inb);
PyrFloatArray* signal_ring3_fx(VMGlobals* g, float ina, PyrFloatArray* inb);
PyrFloatArray* signal_ring4_fx(VMGlobals* g, float ina, PyrFloatArray* inb);
PyrFloatArray* signal_thresh_fx(VMGlobals* g, float ina, PyrFloatArray* inb);
PyrFloatArray* signal_amclip_fx(VMGlobals* g, float ina, PyrFloatArray* inb);
PyrFloatArray* signal_sub_fx(VMGlobals* g, float ina, PyrFloatArray* inb);
PyrFloatArray* signal_div_fx(VMGlobals* g, float ina, PyrFloatArray* inb);
PyrFloatArray* signal_difsqr_fx(VMGlobals* g, float ina, PyrFloatArray* inb);
PyrFloatArray* signal_sumsqr_fx(VMGlobals* g, float ina, PyrFloatArray* inb);
PyrFloatArray* signal_sqrsum_fx(VMGlobals* g, float ina, PyrFloatArray* inb);
PyrFloatArray* signal_sqrdif_fx(VMGlobals* g, float ina, PyrFloatArray* inb);
PyrFloatArray* signal_min_xx(VMGlobals* g, PyrFloatArray* ina, PyrFloatArray* inb);
PyrFloatArray* signal_max_xx(VMGlobals* g, PyrFloatArray* ina, PyrFloatArray* inb);
PyrFloatArray* signal_min_xf(VMGlobals* g, PyrFloatArray* ina, float inb);
PyrFloatArray* signal_max_xf(VMGlobals* g, PyrFloatArray* ina, float inb);
PyrFloatArray* signal_invert(VMGlobals* g, PyrFloatArray* inPyrSignal);
PyrFloatArray* signal_recip(VMGlobals* g, PyrFloatArray* inPyrSignal);
PyrFloatArray* signal_squared(VMGlobals* g, PyrFloatArray* inPyrSignal);
PyrFloatArray* signal_cubed(VMGlobals* g, PyrFloatArray* inPyrSignal);
PyrFloatArray* signal_abs(VMGlobals* g, PyrFloatArray* inPyrSignal);
PyrFloatArray* signal_sign(VMGlobals* g, PyrFloatArray* inPyrSignal);
PyrFloatArray* signal_negative(VMGlobals* g, PyrFloatArray* inPyrSignal);
PyrFloatArray* signal_positive(VMGlobals* g, PyrFloatArray* inPyrSignal);
PyrFloatArray* signal_strictly_positive(VMGlobals* g, PyrFloatArray* inPyrSignal);
PyrFloatArray* signal_nyqring(VMGlobals* g, PyrFloatArray* inPyrSignal);
PyrFloatArray* signal_clip_f(VMGlobals* g, PyrFloatArray* inPyrSignal, float lo, float hi);
PyrFloatArray* signal_clip_f_ds(PyrFloatArray* inPyrSignal, float lo, float hi);
PyrFloatArray* signal_clip_x(VMGlobals* g, PyrFloatArray* ina, PyrFloatArray* inb, PyrFloatArray* inc);
PyrFloatArray* signal_wrap_f(VMGlobals* g, PyrFloatArray* inPyrSignal, float lo, float hi);
PyrFloatArray* signal_wrap_x(VMGlobals* g, PyrFloatArray* ina, PyrFloatArray* inb, PyrFloatArray* inc);
PyrFloatArray* signal_fold_f(VMGlobals* g, PyrFloatArray* inPyrSignal, float lo, float hi);
PyrFloatArray* signal_fold_x(VMGlobals* g, PyrFloatArray* ina, PyrFloatArray* inb, PyrFloatArray* inc);
PyrFloatArray* signal_log(VMGlobals* g, PyrFloatArray* inPyrSignal);
PyrFloatArray* signal_log2(VMGlobals* g, PyrFloatArray* inPyrSignal);
PyrFloatArray* signal_log10(VMGlobals* g, PyrFloatArray* inPyrSignal);
PyrFloatArray* signal_sin(VMGlobals* g, PyrFloatArray* inPyrSignal);
PyrFloatArray* signal_cos(VMGlobals* g, PyrFloatArray* inPyrSignal);
PyrFloatArray* signal_tan(VMGlobals* g, PyrFloatArray* inPyrSignal);
PyrFloatArray* signal_sinh(VMGlobals* g, PyrFloatArray* inPyrSignal);
PyrFloatArray* signal_cosh(VMGlobals* g, PyrFloatArray* inPyrSignal);
PyrFloatArray* signal_tanh(VMGlobals* g, PyrFloatArray* inPyrSignal);
PyrFloatArray* signal_asin(VMGlobals* g, PyrFloatArray* inPyrSignal);
PyrFloatArray* signal_acos(VMGlobals* g, PyrFloatArray* inPyrSignal);
PyrFloatArray* signal_atan(VMGlobals* g, PyrFloatArray* inPyrSignal);
PyrFloatArray* signal_exp(VMGlobals* g, PyrFloatArray* inPyrSignal);
PyrFloatArray* signal_sqrt(VMGlobals* g, PyrFloatArray* inPyrSignal);
PyrFloatArray* signal_distort(VMGlobals* g, PyrFloatArray* inPyrSignal);
PyrFloatArray* signal_distortneg(VMGlobals* g, PyrFloatArray* inPyrSignal);
PyrFloatArray* signal_softclip(VMGlobals* g, PyrFloatArray* inPyrSignal);
PyrFloatArray* signal_softclipneg(VMGlobals* g, PyrFloatArray* inPyrSignal);
PyrFloatArray* signal_fsin(VMGlobals* g, PyrFloatArray* inPyrSignal);
PyrFloatArray* signal_poly3(VMGlobals* g, PyrFloatArray* inPyrSignal, float a, float b, float c);
PyrFloatArray* signal_poly3r(VMGlobals* g, PyrFloatArray* inPyrSignal, float a1, float a2, float b1, float b2, float c1,
                             float c2, float slopeFactor);
PyrFloatArray* signal_integrate(VMGlobals* g, PyrFloatArray* inPyrSignal, float* ioSum);
PyrFloatArray* signal_leakdc(VMGlobals* g, PyrFloatArray* inPyrSignal, float* ioDC, float leakFactor);
PyrFloatArray* signal_ampflw1(VMGlobals* g, PyrFloatArray* inPyrSignal, float* ioAmp, float leak1);
PyrFloatArray* signal_ampflw2(VMGlobals* g, PyrFloatArray* inPyrSignal, float* ioAmp, float leak1);
PyrFloatArray* signal_differentiate(VMGlobals* g, PyrFloatArray* inPyrSignal, float* ioPrev);
PyrFloatArray* signal_rotate(VMGlobals* g, PyrFloatArray* ina, int rot);
PyrFloatArray* signal_reverse_ds(PyrFloatArray* ina);
PyrFloatArray* signal_cat(VMGlobals* g, PyrFloatArray* ina, PyrFloatArray* inb);
PyrFloatArray* signal_insert(VMGlobals* g, PyrFloatArray* ina, PyrFloatArray* inb, std::int64_t index);
PyrFloatArray* signal_overdub(VMGlobals* g, PyrFloatArray* ina, PyrFloatArray* inb, std::int64_t index);
PyrFloatArray* signal_overwrite(VMGlobals* g, PyrFloatArray* ina, PyrFloatArray* inb, std::int64_t index);
PyrFloatArray* signal_cat3(VMGlobals* g, PyrFloatArray* ina, PyrFloatArray* inb, PyrFloatArray* inc);
PyrFloatArray* signal_linen(VMGlobals* g, PyrFloatArray* ina, std::int64_t atk, std::int64_t dcy, float amp);
PyrFloatArray* signal_linen2(VMGlobals* g, PyrFloatArray* ina, std::int64_t atk, std::int64_t dcy, float amp,
                             float midamp);
PyrFloatArray* signal_writesplice(VMGlobals* g, PyrFloatArray* outc, PyrFloatArray* ina, PyrFloatArray* inb,
                                  std::int64_t indexc, std::int64_t indexa, std::int64_t indexb, std::int64_t fadelen,
                                  float midamp);
PyrFloatArray* signal_splice(VMGlobals* g, PyrFloatArray* ina, PyrFloatArray* inb, std::int64_t indexa,
                             std::int64_t indexb, std::int64_t fadelen, float midamp);
PyrFloatArray* signal_mod_fx(VMGlobals* g, double ina, PyrFloatArray* inb);
PyrFloatArray* signal_mod_xf(VMGlobals* g, PyrFloatArray* ina, double inb);
PyrFloatArray* signal_mod_xx(VMGlobals* g, PyrFloatArray* ina, PyrFloatArray* inb);

PyrFloatArray* signal_invert_ds(PyrFloatArray* inPyrSignal);
PyrFloatArray* signal_recip_ds(PyrFloatArray* inPyrSignal);
PyrFloatArray* signal_squared_ds(PyrFloatArray* inPyrSignal);
PyrFloatArray* signal_cubed_ds(PyrFloatArray* inPyrSignal);
PyrFloatArray* signal_abs_ds(PyrFloatArray* inPyrSignal);
PyrFloatArray* signal_sign_ds(PyrFloatArray* inPyrSignal);
PyrFloatArray* signal_negative_ds(PyrFloatArray* inPyrSignal);
PyrFloatArray* signal_positive_ds(PyrFloatArray* inPyrSignal);
PyrFloatArray* signal_strictly_positive_ds(PyrFloatArray* inPyrSignal);
PyrFloatArray* signal_nyqring_ds(PyrFloatArray* inPyrSignal);

PyrFloatArray* signal_clipneg_ds(PyrFloatArray* inPyrSignal);
PyrFloatArray* signal_distort_ds(PyrFloatArray* inPyrSignal);
PyrFloatArray* signal_distortneg_ds(PyrFloatArray* inPyrSignal);
PyrFloatArray* signal_softclip_ds(PyrFloatArray* inPyrSignal);
PyrFloatArray* signal_softclipneg_ds(PyrFloatArray* inPyrSignal);
PyrFloatArray* signal_fsin_ds(PyrFloatArray* inPyrSignal);

PyrFloatArray* signal_log_ds(PyrFloatArray* inPyrSignal);
PyrFloatArray* signal_log2_ds(PyrFloatArray* inPyrSignal);
PyrFloatArray* signal_log10_ds(PyrFloatArray* inPyrSignal);
PyrFloatArray* signal_sin_ds(PyrFloatArray* inPyrSignal);
PyrFloatArray* signal_cos_ds(PyrFloatArray* inPyrSignal);
PyrFloatArray* signal_tan_ds(PyrFloatArray* inPyrSignal);
PyrFloatArray* signal_sinh_ds(PyrFloatArray* inPyrSignal);
PyrFloatArray* signal_cosh_ds(PyrFloatArray* inPyrSignal);
PyrFloatArray* signal_tanh_ds(PyrFloatArray* inPyrSignal);
PyrFloatArray* signal_asin_ds(PyrFloatArray* inPyrSignal);
PyrFloatArray* signal_acos_ds(PyrFloatArray* inPyrSignal);
PyrFloatArray* signal_atan_ds(PyrFloatArray* inPyrSignal);
PyrFloatArray* signal_exp_ds(PyrFloatArray* inPyrSignal);
PyrFloatArray* signal_sqrt_ds(PyrFloatArray* inPyrSignal);

float signal_findpeak(PyrFloatArray* inPyrSignal);
PyrFloatArray* signal_normalize(PyrFloatArray* inPyrSignal);
PyrFloatArray* signal_normalize_transfer_fn(PyrFloatArray* inPyrSignal);
float signal_integral(PyrFloatArray* inPyrSignal);
PyrFloatArray* signal_combself(VMGlobals* g, PyrFloatArray* ina, std::int64_t rot);
PyrFloatArray* signal_bilinen(VMGlobals* g, PyrFloatArray* ina, std::int64_t atk, std::int64_t dcy, float amp,
                              float midamp);
PyrFloatArray* signal_lace2(VMGlobals* g, PyrFloatArray* ina, PyrFloatArray* inb);
void signal_unlace2(VMGlobals* g, PyrFloatArray* ina, PyrFloatArray** outb, PyrFloatArray** outc);
void signal_convolve(VMGlobals* g, PyrFloatArray* ina, PyrFloatArray* ir, PyrFloatArray* previn, std::int64_t* ppos);
PyrFloatArray* signal_thumbnail(VMGlobals* g, PyrFloatArray* ina, std::int64_t startpos, std::int64_t length,
                                int binsize);

PyrFloatArray* signal_scaleneg_xx(VMGlobals* g, PyrFloatArray* ina, PyrFloatArray* inb);
PyrFloatArray* signal_scaleneg_xf(VMGlobals* g, PyrFloatArray* ina, float inb);
PyrFloatArray* signal_scaleneg_fx(VMGlobals* g, float ina, PyrFloatArray* inb);

PyrFloatArray* signal_clip2_xx(VMGlobals* g, PyrFloatArray* ina, PyrFloatArray* inb);
PyrFloatArray* signal_clip2_xf(VMGlobals* g, PyrFloatArray* ina, float inb);
PyrFloatArray* signal_clip2_fx(VMGlobals* g, float ina, PyrFloatArray* inb);

PyrFloatArray* signal_fold2_xx(VMGlobals* g, PyrFloatArray* ina, PyrFloatArray* inb);
PyrFloatArray* signal_fold2_xf(VMGlobals* g, PyrFloatArray* ina, float inb);
PyrFloatArray* signal_fold2_fx(VMGlobals* g, float ina, PyrFloatArray* inb);

PyrFloatArray* signal_wrap2_xx(VMGlobals* g, PyrFloatArray* ina, PyrFloatArray* inb);
PyrFloatArray* signal_wrap2_xf(VMGlobals* g, PyrFloatArray* ina, float inb);
PyrFloatArray* signal_wrap2_fx(VMGlobals* g, float ina, PyrFloatArray* inb);

PyrFloatArray* signal_excess_xx(VMGlobals* g, PyrFloatArray* ina, PyrFloatArray* inb);
PyrFloatArray* signal_excess_xf(VMGlobals* g, PyrFloatArray* ina, float inb);
PyrFloatArray* signal_excess_fx(VMGlobals* g, float ina, PyrFloatArray* inb);

PyrFloatArray* signal_absdif_fx(VMGlobals* g, float ina, PyrFloatArray* inb);
PyrFloatArray* signal_absdif_xf(VMGlobals* g, PyrFloatArray* ina, float inb);
PyrFloatArray* signal_absdif_xx(VMGlobals* g, PyrFloatArray* ina, PyrFloatArray* inb);

bool signal_equal_xf(VMGlobals* g, PyrFloatArray* ina, float inb);
bool signal_equal_xx(VMGlobals* g, PyrFloatArray* ina, PyrFloatArray* inb);

void signal_get_bounds(PyrFloatArray* ina, float* ominval, float* omaxval);

void signal_smooth_ds(PyrFloatArray* inPyrSignal);
void signal_hanning_ds(PyrFloatArray* inPyrSignal);
void signal_welch_ds(PyrFloatArray* inPyrSignal);
void signal_parzen_ds(PyrFloatArray* inPyrSignal);

PyrFloatArray* signal_normalize_range(PyrFloatArray* ina, std::int64_t start, std::int64_t end);
PyrFloatArray* signal_zero_range(PyrFloatArray* ina, std::int64_t start, std::int64_t end);
PyrFloatArray* signal_invert_range(PyrFloatArray* ina, std::int64_t start, std::int64_t end);
PyrFloatArray* signal_reverse_range(PyrFloatArray* ina, std::int64_t start, std::int64_t end);
PyrFloatArray* signal_fade_in(PyrFloatArray* ina, std::int64_t start, std::int64_t end);
PyrFloatArray* signal_fade_out(PyrFloatArray* ina, std::int64_t start, std::int64_t end);
PyrFloatArray* signal_abs_range(PyrFloatArray* ina, std::int64_t start, std::int64_t end);
PyrFloatArray* signal_squared_range(PyrFloatArray* ina, std::int64_t start, std::int64_t end);
PyrFloatArray* signal_cubed_range(PyrFloatArray* ina, std::int64_t start, std::int64_t end);
PyrFloatArray* signal_distort_range(PyrFloatArray* ina, std::int64_t start, std::int64_t end);

PyrFloatArray* signal_fade_range(PyrFloatArray* ina, std::int64_t start, std::int64_t end, float lvl0, float lvl1);
