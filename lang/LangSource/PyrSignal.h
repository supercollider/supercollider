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

#ifndef PYR_SIGNAL_H
#define PYR_SIGNAL_H

#include "PyrObject.h"
#include "GC.h"

#define UNROLL 1

enum {
	kSignalRate = 0, // index of rate slot
	kSignalNextNode
};

PyrObject* newPyrSignal(VMGlobals *g, long size);

#define UNROLL8_CODE(size,var,stmt)		\
	{	int tempi, tempend;						\
		tempend = size>>3;							\
		for (tempi=0; tempi<tempend; ++tempi) {	\
			stmt;stmt;stmt;stmt;				\
			stmt;stmt;stmt;stmt;				\
		}										\
		tempend = size&7;							\
		for (tempi=0; tempi<tempend; ++tempi) {	\
			stmt;								\
		}										\
	}

#define UNROLL4_CODE(size,var,stmt)		\
	{	int tempi, tempend;						\
		tempend = size>>2;							\
		for (tempi=0; tempi<tempend; ++tempi) {	\
			stmt;stmt;stmt;stmt;				\
		}										\
		tempend = size&3;							\
		for (tempi=0; tempi<tempend; ++tempi) {	\
			stmt;								\
		}										\
	}

#define FILTER_LOOP(size,var,stmt,stmt2)		\
	{	int tempi, tempend;						\
		tempend = size>>2;						\
		for (tempi=0; tempi<tempend; ++tempi) {	\
			stmt;stmt;stmt;stmt;				\
			stmt2;								\
		}										\
		tempend = size&3;						\
		for (tempi=0; tempi<tempend; ++tempi) {	\
			stmt;								\
		}										\
	}

#define UNROLL1_CODE(size,var,stmt)				\
	{	int tempi, tempend;						\
		tempend = size;							\
		for (tempi=0; tempi<tempend; ++tempi) {	\
			stmt;								\
		}										\
	}


#if UNROLL == 8
#define UNROLL_CODE UNROLL8_CODE
#elif UNROLL == 4
#define UNROLL_CODE UNROLL4_CODE
#else
#define UNROLL_CODE UNROLL1_CODE
#endif

#if 0

#define BINOP_LOOP1(OP)
#define BINOP_LOOP2(STMT1, STMT2, STMT3)

#else

#define BINOP_LOOP1(OP)	\
	float *a, *b, *c, *endptr;	\
	PyrObject *outc;	\
	long size;	\
	a = (float*)(ina->slots) - 1;	\
	b = (float*)(inb->slots) - 1;	\
	size = sc_min(ina->size, inb->size);	\
	outc = newPyrSignal(g, size);	\
	c = (float*)(outc->slots) - 1;	\
	endptr = c + size;	\
	switch (size & 3) {	\
		while (c < endptr) {	\
			         *++c = *++a OP *++b;	\
			case 3 : *++c = *++a OP *++b;		\
			case 2 : *++c = *++a OP *++b;		\
			case 1 : *++c = *++a OP *++b;		\
			case 0 : ;			\
		}						\
	}	\
	return outc;	\


#define BINOP_LOOP2(STMT1)	\
	float *a, *b, *c, *endptr;	\
	PyrObject *outc;	\
	long size;	\
	a = (float*)(ina->slots) - 1;	\
	b = (float*)(inb->slots) - 1;	\
	size = sc_min(ina->size, inb->size);	\
	outc = newPyrSignal(g, size);	\
	c = (float*)(outc->slots) - 1;	\
	endptr = c + size;	\
	switch (size & 3) {	\
		while (c < endptr) {	\
			        STMT1;		\
			case 3 :STMT1;		\
			case 2 :STMT1;		\
			case 1 :STMT1;		\
			case 0 : ;			\
		}						\
	}	\
	return outc;	\

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
PyrObject* signal_fill(PyrObject *outSignal, float inValue);
PyrObject* signal_scale(PyrObject *outSignal, float inValue);
PyrObject* signal_offset(PyrObject *outSignal, float inValue);
PyrObject* signal_scale_offset(PyrObject *outSignal, float mul, float add);
PyrObject* signal_mix(PyrObject* ina, PyrObject* inb, float start, float end, float slopeFactor);
PyrObject* signal_add_xx(VMGlobals *g, PyrObject* ina, PyrObject* inb);
PyrObject* signal_sub_xx(VMGlobals *g, PyrObject* ina, PyrObject* inb);
PyrObject* signal_mul_xx(VMGlobals *g, PyrObject* ina, PyrObject* inb);
PyrObject* signal_mul_ds_xx(VMGlobals *g, PyrObject* ina, PyrObject* inb);
PyrObject* signal_add_ds_xx(VMGlobals *g, PyrObject* ina, PyrObject* inb);
PyrObject* signal_sub_ds_xx(VMGlobals *g, PyrObject* ina, PyrObject* inb);
PyrObject* signal_ring1_xx(VMGlobals *g, PyrObject* ina, PyrObject* inb);
PyrObject* signal_ring2_xx(VMGlobals *g, PyrObject* ina, PyrObject* inb);
PyrObject* signal_ring3_xx(VMGlobals *g, PyrObject* ina, PyrObject* inb);
PyrObject* signal_ring4_xx(VMGlobals *g, PyrObject* ina, PyrObject* inb);
PyrObject* signal_thresh_xx(VMGlobals *g, PyrObject* ina, PyrObject* inb);
PyrObject* signal_amclip_xx(VMGlobals *g, PyrObject* ina, PyrObject* inb);
PyrObject* signal_div_xx(VMGlobals *g, PyrObject* ina, PyrObject* inb);
PyrObject* signal_difsqr_xx(VMGlobals *g, PyrObject* ina, PyrObject* inb);
PyrObject* signal_sumsqr_xx(VMGlobals *g, PyrObject* ina, PyrObject* inb);
PyrObject* signal_sqrsum_xx(VMGlobals *g, PyrObject* ina, PyrObject* inb);
PyrObject* signal_sqrdif_xx(VMGlobals *g, PyrObject* ina, PyrObject* inb);
PyrObject* signal_add_xf(VMGlobals *g, PyrObject* ina, float inb);
PyrObject* signal_sub_xf(VMGlobals *g, PyrObject* ina, float inb);
PyrObject* signal_mul_xf(VMGlobals *g, PyrObject* ina, float inb);
PyrObject* signal_ring1_xf(VMGlobals *g, PyrObject* ina, float inb);
PyrObject* signal_ring2_xf(VMGlobals *g, PyrObject* ina, float inb);
PyrObject* signal_ring3_xf(VMGlobals *g, PyrObject* ina, float inb);
PyrObject* signal_ring4_xf(VMGlobals *g, PyrObject* ina, float inb);
PyrObject* signal_thresh_xf(VMGlobals *g, PyrObject* ina, float inb);
PyrObject* signal_amclip_xf(VMGlobals *g, PyrObject* ina, float inb);
PyrObject* signal_div_xf(VMGlobals *g, PyrObject* ina, float inb);
PyrObject* signal_difsqr_xf(VMGlobals *g, PyrObject* ina, float inb);
PyrObject* signal_sumsqr_xf(VMGlobals *g, PyrObject* ina, float inb);
PyrObject* signal_sqrsum_xf(VMGlobals *g, PyrObject* ina, float inb);
PyrObject* signal_sqrdif_xf(VMGlobals *g, PyrObject* ina, float inb);
PyrObject* signal_ring1_fx(VMGlobals *g, float ina, PyrObject* inb);
PyrObject* signal_ring2_fx(VMGlobals *g, float ina, PyrObject* inb);
PyrObject* signal_ring3_fx(VMGlobals *g, float ina, PyrObject* inb);
PyrObject* signal_ring4_fx(VMGlobals *g, float ina, PyrObject* inb);
PyrObject* signal_thresh_fx(VMGlobals *g, float ina, PyrObject* inb);
PyrObject* signal_amclip_fx(VMGlobals *g, float ina, PyrObject* inb);
PyrObject* signal_sub_fx(VMGlobals *g, float ina, PyrObject* inb);
PyrObject* signal_div_fx(VMGlobals *g, float ina, PyrObject* inb);
PyrObject* signal_difsqr_fx(VMGlobals *g, float ina, PyrObject* inb);
PyrObject* signal_sumsqr_fx(VMGlobals *g, float ina, PyrObject* inb);
PyrObject* signal_sqrsum_fx(VMGlobals *g, float ina, PyrObject* inb);
PyrObject* signal_sqrdif_fx(VMGlobals *g, float ina, PyrObject* inb);
PyrObject* signal_min_xx(VMGlobals *g, PyrObject* ina, PyrObject* inb);
PyrObject* signal_max_xx(VMGlobals *g, PyrObject* ina, PyrObject* inb);
PyrObject* signal_min_xf(VMGlobals *g, PyrObject* ina, float inb);
PyrObject* signal_max_xf(VMGlobals *g, PyrObject* ina, float inb);
PyrObject* signal_invert(VMGlobals *g, PyrObject *inPyrSignal);
PyrObject* signal_recip(VMGlobals *g, PyrObject *inPyrSignal);
PyrObject* signal_squared(VMGlobals *g, PyrObject *inPyrSignal);
PyrObject* signal_cubed(VMGlobals *g, PyrObject *inPyrSignal);
PyrObject* signal_abs(VMGlobals *g, PyrObject *inPyrSignal);
PyrObject* signal_sign(VMGlobals *g, PyrObject *inPyrSignal);
PyrObject* signal_negative(VMGlobals *g, PyrObject *inPyrSignal);
PyrObject* signal_positive(VMGlobals *g, PyrObject *inPyrSignal);
PyrObject* signal_strictly_positive(VMGlobals *g, PyrObject *inPyrSignal);
PyrObject* signal_nyqring(VMGlobals *g, PyrObject *inPyrSignal);
PyrObject* signal_clip_f(VMGlobals *g, PyrObject *inPyrSignal, float lo, float hi);
PyrObject* signal_clip_f_ds(PyrObject *inPyrSignal, float lo, float hi);
PyrObject* signal_clip_x(VMGlobals *g, PyrObject *ina, PyrObject *inb, PyrObject *inc);
PyrObject* signal_wrap_f(VMGlobals *g, PyrObject *inPyrSignal, float lo, float hi);
PyrObject* signal_wrap_x(VMGlobals *g, PyrObject *ina, PyrObject *inb, PyrObject *inc);
PyrObject* signal_fold_f(VMGlobals *g, PyrObject *inPyrSignal, float lo, float hi);
PyrObject* signal_fold_x(VMGlobals *g, PyrObject *ina, PyrObject *inb, PyrObject *inc);
PyrObject* signal_log(VMGlobals *g, PyrObject *inPyrSignal);
PyrObject* signal_log2(VMGlobals *g, PyrObject *inPyrSignal);
PyrObject* signal_log10(VMGlobals *g, PyrObject *inPyrSignal);
PyrObject* signal_sin(VMGlobals *g, PyrObject *inPyrSignal);
PyrObject* signal_cos(VMGlobals *g, PyrObject *inPyrSignal);
PyrObject* signal_tan(VMGlobals *g, PyrObject *inPyrSignal);
PyrObject* signal_sinh(VMGlobals *g, PyrObject *inPyrSignal);
PyrObject* signal_cosh(VMGlobals *g, PyrObject *inPyrSignal);
PyrObject* signal_tanh(VMGlobals *g, PyrObject *inPyrSignal);
PyrObject* signal_asin(VMGlobals *g, PyrObject *inPyrSignal);
PyrObject* signal_acos(VMGlobals *g, PyrObject *inPyrSignal);
PyrObject* signal_atan(VMGlobals *g, PyrObject *inPyrSignal);
PyrObject* signal_exp(VMGlobals *g, PyrObject *inPyrSignal);
PyrObject* signal_sqrt(VMGlobals *g, PyrObject *inPyrSignal);
PyrObject* signal_distort(VMGlobals *g, PyrObject *inPyrSignal);
PyrObject* signal_distortneg(VMGlobals *g, PyrObject *inPyrSignal);
PyrObject* signal_softclip(VMGlobals *g, PyrObject *inPyrSignal);
PyrObject* signal_softclipneg(VMGlobals *g, PyrObject *inPyrSignal);
PyrObject* signal_fsin(VMGlobals *g, PyrObject *inPyrSignal);
PyrObject* signal_poly3(VMGlobals *g, PyrObject *inPyrSignal, float a, float b, float c);
PyrObject* signal_poly3r(VMGlobals *g, PyrObject *inPyrSignal,
	float a1, float a2, float b1, float b2, float c1, float c2, float slopeFactor);
PyrObject* signal_integrate(VMGlobals *g, PyrObject *inPyrSignal, float *ioSum);
PyrObject* signal_leakdc(VMGlobals *g, PyrObject *inPyrSignal, float *ioDC, float leakFactor);
PyrObject* signal_ampflw1(VMGlobals *g, PyrObject *inPyrSignal, float *ioAmp, float leak1);
PyrObject* signal_ampflw2(VMGlobals *g, PyrObject *inPyrSignal, float *ioAmp, float leak1);
PyrObject* signal_differentiate(VMGlobals *g, PyrObject *inPyrSignal, float *ioPrev);
PyrObject* signal_rotate(VMGlobals *g, PyrObject* ina, int rot);
PyrObject* signal_reverse_ds(PyrObject* ina);
PyrObject* signal_cat(VMGlobals *g, PyrObject* ina, PyrObject* inb);
PyrObject* signal_insert(VMGlobals *g, PyrObject* ina, PyrObject* inb, long index);
PyrObject* signal_overdub(VMGlobals *g, PyrObject* ina, PyrObject* inb, long index);
PyrObject* signal_overwrite(VMGlobals *g, PyrObject* ina, PyrObject* inb, long index);
PyrObject* signal_cat3(VMGlobals *g, PyrObject* ina, PyrObject* inb, PyrObject* inc);
PyrObject* signal_linen(VMGlobals *g, PyrObject* ina, long atk, long dcy, float amp);
PyrObject* signal_linen2(VMGlobals *g, PyrObject* ina, long atk, long dcy, float amp, float midamp);
PyrObject* signal_writesplice(VMGlobals *g, PyrObject* outc, PyrObject* ina, PyrObject* inb,
	long indexc, long indexa, long indexb, long fadelen, float midamp);
PyrObject* signal_splice(VMGlobals *g, PyrObject* ina, PyrObject* inb,
	long indexa, long indexb, long fadelen, float midamp);

PyrObject* signal_invert_ds(PyrObject *inPyrSignal);
PyrObject* signal_recip_ds(PyrObject *inPyrSignal);
PyrObject* signal_squared_ds(PyrObject *inPyrSignal);
PyrObject* signal_cubed_ds(PyrObject *inPyrSignal);
PyrObject* signal_abs_ds(PyrObject *inPyrSignal);
PyrObject* signal_sign_ds(PyrObject *inPyrSignal);
PyrObject* signal_negative_ds(PyrObject *inPyrSignal);
PyrObject* signal_positive_ds(PyrObject *inPyrSignal);
PyrObject* signal_strictly_positive_ds(PyrObject *inPyrSignal);
PyrObject* signal_nyqring_ds(PyrObject *inPyrSignal);

PyrObject* signal_clipneg_ds(PyrObject *inPyrSignal);
PyrObject* signal_distort_ds(PyrObject *inPyrSignal);
PyrObject* signal_distortneg_ds(PyrObject *inPyrSignal);
PyrObject* signal_softclip_ds(PyrObject *inPyrSignal);
PyrObject* signal_softclipneg_ds(PyrObject *inPyrSignal);
PyrObject* signal_fsin_ds(PyrObject *inPyrSignal);

PyrObject* signal_log_ds(PyrObject *inPyrSignal);
PyrObject* signal_log2_ds(PyrObject *inPyrSignal);
PyrObject* signal_log10_ds(PyrObject *inPyrSignal);
PyrObject* signal_sin_ds(PyrObject *inPyrSignal);
PyrObject* signal_cos_ds(PyrObject *inPyrSignal);
PyrObject* signal_tan_ds(PyrObject *inPyrSignal);
PyrObject* signal_sinh_ds(PyrObject *inPyrSignal);
PyrObject* signal_cosh_ds(PyrObject *inPyrSignal);
PyrObject* signal_tanh_ds(PyrObject *inPyrSignal);
PyrObject* signal_asin_ds(PyrObject *inPyrSignal);
PyrObject* signal_acos_ds(PyrObject *inPyrSignal);
PyrObject* signal_atan_ds(PyrObject *inPyrSignal);
PyrObject* signal_exp_ds(PyrObject *inPyrSignal);
PyrObject* signal_sqrt_ds(PyrObject *inPyrSignal);

float signal_findpeak(PyrObject *inPyrSignal);
PyrObject* signal_normalize(PyrObject *inPyrSignal);
PyrObject* signal_normalize_transfer_fn(PyrObject *inPyrSignal);
float signal_integral(PyrObject *inPyrSignal);
PyrObject* signal_combself(VMGlobals *g, PyrObject* ina, long rot);
PyrObject* signal_bilinen(VMGlobals *g, PyrObject* ina, long atk, long dcy, float amp, float midamp);
PyrObject* signal_lace2(VMGlobals *g, PyrObject* ina, PyrObject* inb);
void signal_unlace2(VMGlobals *g, PyrObject* ina, PyrObject** outb, PyrObject** outc);
void signal_convolve(VMGlobals *g, PyrObject* ina, PyrObject* ir, PyrObject* previn, long *ppos);
PyrObject* signal_thumbnail(VMGlobals *g, PyrObject* ina, long startpos, long length, int binsize);

PyrObject* signal_scaleneg_xx(VMGlobals *g, PyrObject* ina, PyrObject* inb);
PyrObject* signal_scaleneg_xf(VMGlobals *g, PyrObject* ina, float inb);
PyrObject* signal_scaleneg_fx(VMGlobals *g, float ina, PyrObject* inb);

PyrObject* signal_clip2_xx(VMGlobals *g, PyrObject* ina, PyrObject* inb);
PyrObject* signal_clip2_xf(VMGlobals *g, PyrObject* ina, float inb);
PyrObject* signal_clip2_fx(VMGlobals *g, float ina, PyrObject* inb);

PyrObject* signal_fold2_xx(VMGlobals *g, PyrObject* ina, PyrObject* inb);
PyrObject* signal_fold2_xf(VMGlobals *g, PyrObject* ina, float inb);
PyrObject* signal_fold2_fx(VMGlobals *g, float ina, PyrObject* inb);

PyrObject* signal_wrap2_xx(VMGlobals *g, PyrObject* ina, PyrObject* inb);
PyrObject* signal_wrap2_xf(VMGlobals *g, PyrObject* ina, float inb);
PyrObject* signal_wrap2_fx(VMGlobals *g, float ina, PyrObject* inb);

PyrObject* signal_excess_xx(VMGlobals *g, PyrObject* ina, PyrObject* inb);
PyrObject* signal_excess_xf(VMGlobals *g, PyrObject* ina, float inb);
PyrObject* signal_excess_fx(VMGlobals *g, float ina, PyrObject* inb);

PyrObject* signal_absdif_fx(VMGlobals *g, float ina, PyrObject* inb);
PyrObject* signal_absdif_xf(VMGlobals *g, PyrObject* ina, float inb);
PyrObject* signal_absdif_xx(VMGlobals *g, PyrObject* ina, PyrObject* inb);

bool signal_equal_xf(VMGlobals *g, PyrObject* ina, float inb);
bool signal_equal_xx(VMGlobals *g, PyrObject* ina, PyrObject* inb);

void signal_get_bounds(PyrObject* ina, float *ominval, float *omaxval);

void signal_smooth_ds(PyrObject* inPyrSignal);
void signal_hanning_ds(PyrObject* inPyrSignal);
void signal_welch_ds(PyrObject* inPyrSignal);
void signal_parzen_ds(PyrObject* inPyrSignal);

PyrObject* signal_normalize_range(PyrObject* ina, long start, long end);
PyrObject* signal_zero_range(PyrObject* ina, long start, long end);
PyrObject* signal_invert_range(PyrObject* ina, long start, long end);
PyrObject* signal_reverse_range(PyrObject* ina, long start, long end);
PyrObject* signal_fade_in(PyrObject* ina, long start, long end);
PyrObject* signal_fade_out(PyrObject* ina, long start, long end);
PyrObject* signal_abs_range(PyrObject* ina, long start, long end);
PyrObject* signal_squared_range(PyrObject* ina, long start, long end);
PyrObject* signal_cubed_range(PyrObject* ina, long start, long end);
PyrObject* signal_distort_range(PyrObject* ina, long start, long end);

PyrObject* signal_fade_range(PyrObject* ina, long start, long end, float lvl0, float lvl1);

#endif // PYR_SIGNAL_H
