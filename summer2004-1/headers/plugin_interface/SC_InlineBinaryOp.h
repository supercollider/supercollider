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
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef _BinaryOpUGen_
#define _BinaryOpUGen_

#include "SC_BoundsMacros.h"
#include <math.h>

inline float sc_mod(float in, float hi) 
{ 
	// avoid the divide if possible
	const float lo = (float)0.;
	if (in >= hi) {
		in -= hi;
		if (in < hi) return in;
	} else if (in < lo) {
		in += hi;
		if (in >= lo) return in;
	} else return in;
	
	if (hi == lo) return lo;
	return in - hi*floor(in/hi); 
}

inline double sc_mod(double in, double hi) 
{ 
	// avoid the divide if possible
	const double lo = (double)0.;
	if (in >= hi) {
		in -= hi;
		if (in < hi) return in;
	} else if (in < lo) {
		in += hi;
		if (in >= lo) return in;
	} else return in;
	
	if (hi == lo) return lo;
	return in - hi*floor(in/hi); 
}

inline float sc_wrap(float in, float lo, float hi) 
{
	float range;
	// avoid the divide if possible
	if (in >= hi) {
		range = hi - lo;
		in -= range;
		if (in < hi) return in;
	} else if (in < lo) {
		range = hi - lo;
		in += range;
		if (in >= lo) return in;
	} else return in;
	
	if (hi == lo) return lo;
	return in - range * floor((in - lo)/range); 
}

inline double sc_wrap(double in, double lo, double hi) 
{
	double range;
	// avoid the divide if possible
	if (in >= hi) {
		range = hi - lo;
		in -= range;
		if (in < hi) return in;
	} else if (in < lo) {
		range = hi - lo;
		in += range;
		if (in >= lo) return in;
	} else return in;
	
	if (hi == lo) return lo;
	return in - range * floor((in - lo)/range); 
}

inline double sc_wrap(double in, double lo, double hi, double range) 
{
	// avoid the divide if possible
	if (in >= hi) {
		in -= range;
		if (in < hi) return in;
	} else if (in < lo) {
		in += range;
		if (in >= lo) return in;
	} else return in;
	
	if (hi == lo) return lo;
	return in - range * floor((in - lo)/range); 
}

inline double sc_wrap(float in, float lo, float hi, float range) 
{
	// avoid the divide if possible
	if (in >= hi) {
		in -= range;
		if (in < hi) return in;
	} else if (in < lo) {
		in += range;
		if (in >= lo) return in;
	} else return in;
	
	if (hi == lo) return lo;
	return in - range * floor((in - lo)/range); 
}

inline float sc_fold(float in, float lo, float hi) 
{
	float x, c, range, range2;
	x = in - lo;
	
	// avoid the divide if possible
	if (in >= hi) {
		in = hi + hi - in;
		if (in >= lo) return in;
	} else if (in < lo) {
		in = lo + lo - in;
		if (in < hi) return in;
	} else return in;
	
	if (hi == lo) return lo;
	// ok do the divide
	range = hi - lo;
	range2 = range + range;
	c = x - range2 * floor(x / range2);
	if (c>=range) c = range2 - c;
	return c + lo;
}

inline double sc_fold(double in, double lo, double hi) 
{
	double x, c, range, range2;
	x = in - lo;
	
	// avoid the divide if possible
	if (in >= hi) {
		in = hi + hi - in;
		if (in >= lo) return in;
	} else if (in < lo) {
		in = lo + lo - in;
		if (in < hi) return in;
	} else return in;
	
	if (hi == lo) return lo;
	// ok do the divide
	range = hi - lo;
	range2 = range + range;
	c = x - range2 * floor(x / range2);
	if (c>=range) c = range2 - c;
	return c + lo;
}

inline double sc_fold(float in, float lo, float hi, float range, float range2) 
{
	float x, c;
	x = in - lo;
	
	// avoid the divide if possible
	if (in >= hi) {
		in = hi + hi - in;
		if (in >= lo) return in;
	} else if (in < lo) {
		in = lo + lo - in;
		if (in < hi) return in;
	} else return in;
	
	if (hi == lo) return lo;
	// ok do the divide
	c = x - range2 * floor(x / range2);
	if (c>=range) c = range2 - c;
	return c + lo;
}

inline double sc_fold(double in, double lo, double hi, double range, double range2) 
{
	double x, c;
	x = in - lo;
	
	// avoid the divide if possible
	if (in >= hi) {
		in = hi + hi - in;
		if (in >= lo) return in;
	} else if (in < lo) {
		in = lo + lo - in;
		if (in < hi) return in;
	} else return in;
	
	if (hi == lo) return lo;
	// ok do the divide
	c = x - range2 * floor(x / range2);
	if (c>=range) c = range2 - c;
	return c + lo;
}

inline float sc_pow(float a, float b)
{
	return a >= 0.f ? pow(a, b) : -pow(-a, b);
}

inline double sc_pow(double a, double b)
{
	return a >= 0.f ? pow(a, b) : -pow(-a, b);
}

template <class T>
inline float sc_thresh(T a, T b)
{
	return a < b ? (T)0 : a;
}

inline float sc_clip2(float a, float b)
{
	return sc_clip(a, -b, b);
}

inline float sc_wrap2(float a, float b)
{
	return sc_wrap(a, -b, b);
}

inline float sc_fold2(float a, float b)
{
	return sc_fold(a, -b, b);
}

inline float sc_excess(float a, float b)
{
	return a - sc_clip(a, -b, b);
}

inline float sc_round(float x, float quant)
{
	return quant==0. ? x : floor(x/quant + .5f) * quant;
}

inline double sc_round(double x, double quant)
{
	return quant==0. ? x : floor(x/quant + .5) * quant;
}

inline float sc_roundUp(float x, float quant)
{
	return quant==0. ? x : ceil(x/quant) * quant;
}

inline double sc_roundUp(double x, double quant)
{
	return quant==0. ? x : ceil(x/quant) * quant;
}

inline float sc_trunc(float x, float quant)
{
	return quant==0. ? x : floor(x/quant) * quant;
}

inline double sc_trunc(double x, double quant)
{
	return quant==0. ? x : floor(x/quant) * quant;
}

inline float sc_atan2(float a, float b)
{
	return atan2(a, b);
}


inline float sc_scaleneg(float a, float b)
{
	b = 0.5f * b + 0.5f;
	return (fabs(a) - a) * b + a;
}

inline float sc_amclip(float a, float b)
{
	return a * 0.5f * (b + fabs(b));
}

inline double sc_amclip(double a, double b)
{
	return a * 0.5 * (b + fabs(b));
}

const float kFSQRT2M1 = sqrt(2.) - 1.;
const double kDSQRT2M1 = sqrt(2.) - 1.;

inline float sc_hypotx(float x, float y)
{
	float minxy;
	
	x = fabs(x);
	y = fabs(y);
	
	minxy = sc_min(x,y);
	
	return x + y - kFSQRT2M1 * minxy;
}

inline double sc_hypotx(double x, double y)
{
	double minxy;
	
	x = fabs(x);
	y = fabs(y);
	
	minxy = sc_min(x,y);
	
	return x + y - kDSQRT2M1 * minxy;
}

#pragma mark -


inline int sc_div(int a, int b) 
{
	int c;
	if (b) {
		if (a<0) c = (a+1)/b - 1;
		else c = a/b;
	} else c = a;
	return c;
}

/*
inline int sc_mod(int a, int b) 
{
	long c;
	c = a % b;
	if (c<0) c += b;
	return c;
}
*/

inline int sc_mod(int in, int hi) 
{ 
	// avoid the divide if possible
	const int lo = 0;
	if (in >= hi) {
		in -= hi;
		if (in < hi) return in;
	} else if (in < lo) {
		in += hi;
		if (in >= lo) return in;
	} else return in;
	
	if (hi == lo) return lo;
	
	int c;
	c = in % hi;
	if (c<0) c += hi;
	return c;
}

inline int sc_wrap(int in, int lo, int hi) 
{
	return sc_mod(in - lo, hi - lo + 1) + lo;
}

inline int sc_clip2(int a, int b)
{
	return sc_clip(a, -b, b);
}

inline int sc_wrap2(int a, int b)
{
	return sc_wrap(a, -b, b);
}

inline int sc_fold(int in, int lo, int hi) 
{
	int b = hi - lo;
	int b2 = b+b-2;
	int c = sc_mod(in - lo, b2);
	if (c>=b) c = b2-c;
	return c + lo;	
}

inline int sc_fold2(int a, int b)
{
	return sc_fold(a, -b, b);
}

inline int sc_excess(int a, int b)
{
	return a - sc_clip(a, -b, b);
}

inline int sc_gcd(int u, int v) 
{
	int t;
	u = sc_abs(u);
	v = sc_abs(v);
	if (u <= 1 || v <= 1) return 1;
	while (u>0) {
		if (u<v) { t=u; u=v; v=t; }
		u = u % v;
	}
	return v;
}

inline int sc_lcm(int u, int v) 
{
	return (u * v)/sc_gcd(u,v);
}

inline int sc_thresh(int a, int b)
{
	return a < b ? 0 : a;
}

inline int sc_bitAnd(int a, int b)
{
	return a & b;
}

inline int sc_bitOr(int a, int b)
{
	return a | b;
}

inline int sc_leftShift(int a, int b)
{
	return a << b;
}

inline int sc_rightShift(int a, int b)
{
	return a >> b;
}

inline int sc_unsignedRightShift(int a, int b)
{
	return (uint32)a >> b;
}

inline int sc_round(int x, int quant)
{
	return quant==0 ? x : sc_div(x + quant/2, quant) * quant;
}


inline int sc_roundUp(int x, int quant)
{
	return quant==0 ? x : sc_div(x + quant - 1, quant) * quant;
}

inline int sc_trunc(int x, int quant)
{
	return quant==0 ? x : sc_div(x, quant) * quant;
}

#pragma mark -

#if 0

inline long sc_div(long a, long b) 
{
	int c;
	if (b) {
		if (a<0) c = (a+1)/b - 1;
		else c = a/b;
	} else c = a;
	return c;
}


inline long sc_clip2(long a, long b)
{
	return sc_clip(a, -b, b);
}

inline long sc_wrap(long in, long lo, long hi) 
{
	return sc_mod(in - lo, hi - lo + 1) + lo;
}

inline long sc_wrap2(long a, long b)
{
	return sc_wrap(a, -b, b);
}

inline long sc_fold(long in, long lo, long hi) 
{
	long b = hi - lo;
	long b2 = b+b-2;
	long c = sc_mod(in - lo, b2);
	if (c>=b) c = b2-c;
	return c + lo;	
}

inline long sc_fold2(long a, long b)
{
	return sc_fold(a, -b, b);
}

inline long sc_thresh(long a, long b)
{
	return a < b ? 0 : a;
}

inline long sc_bitAnd(long a, long b)
{
	return a & b;
}

inline long sc_bitOr(long a, long b)
{
	return a | b;
}

inline long sc_leftShift(long a, long b)
{
	return a << b;
}

inline long sc_rightShift(long a, long b)
{
	return a >> b;
}

inline long sc_unsignedRightShift(long a, long b)
{
	return (unsigned long)a >> b;
}

inline long sc_gcd(long u, long v) 
{
	long t;
	u = sc_abs(u);
	v = sc_abs(v);
	if (u <= 1 || v <= 1) return 1;
	while (u>0) {
		if (u<v) { t=u; u=v; v=t; }
		u = u % v;
	}
	return v;
}

inline long sc_lcm(long u, long v) 
{
	return (u * v)/sc_gcd(u,v);
}

inline long sc_excess(long a, long b)
{
	return a - sc_clip(a, -b, b);
}

inline long sc_round(long x, long quant)
{
	return quant==0 ? x : sc_div(x + quant/2, quant) * quant;
}

#endif

#endif

