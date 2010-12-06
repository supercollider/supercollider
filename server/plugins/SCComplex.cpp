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


#include "SCComplex.h"
#include "SC_Types.h"
#include "SC_Constants.h"
#include "SC_InterfaceTable.h"
#include <math.h>

////////////////////////////////////////////////////////////////////////////////

const int32 kPolarLUTSize = 2049;
const int32 kPolarLUTSize2 = kPolarLUTSize >> 1;
float gMagLUT[kPolarLUTSize];
float gPhaseLUT[kPolarLUTSize];

static float gSinePhaseScale;
static int gSineSize;
static float *gSine;
static int gSineMask;

void init_SCComplex(InterfaceTable *ft);
void init_SCComplex(InterfaceTable *ft)
{
	gSineSize = ft->mSineSize;
	gSine = ft->mSine;
	gSinePhaseScale = gSineSize / twopi_f;
	gSineMask = gSineSize - 1;

	double rPolarLUTSize2 = 1. / kPolarLUTSize2;
	for (int i=0; i < kPolarLUTSize; ++i) {
		double slope = (i - kPolarLUTSize2) * rPolarLUTSize2;
		double angle = atan(slope);
		gPhaseLUT[i] = (float)angle;
		gMagLUT[i] = 1.f / (float)cos(angle);
	}
}

SCPolar SCComplex::ToPolar()
{
	return SCPolar(hypot(imag, real), atan2(imag, real));
}

SCComplex SCPolar::ToComplex()
{
	return SCComplex(mag * cos(phase), mag * sin(phase));
}

/*
float fhypotx(float real, float imag);
float fhypotx(float real, float imag)
{

	int32 index;
	float absreal = fabs(real);
	float absimag = fabs(imag);
	float slope;
	if (absreal > absimag) {
		slope = imag/real;
		index = kPolarLUTSize2 + kPolarLUTSize2 * slope;
		return gMagLUT[index] * absreal;
	} else {
		slope = real/imag;
		index = kPolarLUTSize2 + kPolarLUTSize2 * slope;
		return gMagLUT[index] * absimag;
	}
}
*/

SCPolar SCComplex::ToPolarApx()
{
	int32 index;
	float absreal = fabs(real);
	float absimag = fabs(imag);
	float slope;
	if (absreal > absimag) {
		slope = imag/real;
		index = kPolarLUTSize2 + (int32)(kPolarLUTSize2 * slope);
		if (real > 0) {
			return SCPolar(gMagLUT[index] * absreal, gPhaseLUT[index]);
		} else {
			return SCPolar(gMagLUT[index] * absreal, pi_f + gPhaseLUT[index]);
		}
	} else if(absimag==0.f) { // because of the above test, this means real is also zero
		return SCPolar(0.f, 0.f);
	} else {
		slope = real/imag;
		index = kPolarLUTSize2 + (int32)(kPolarLUTSize2 * slope);
		if (imag > 0) {
			return SCPolar(gMagLUT[index] * absimag, pi2_f - gPhaseLUT[index]);
		} else {
			return SCPolar(gMagLUT[index] * absimag, pi32_f - gPhaseLUT[index]);
		}
	}
}

void SCComplex::ToPolarInPlace()
{
	SCPolar polar = ToPolar();
	real = polar.mag;
	imag = polar.phase;
}

void SCPolar::ToComplexInPlace()
{
	SCComplex complx = ToComplex();
	mag = complx.real;
	phase = complx.imag;
}

SCComplex SCPolar::ToComplexApx()
{
	uint32 sinindex = (int32)((double)gSinePhaseScale * phase) & gSineMask;
	uint32 cosindex = (sinindex + (gSineSize>>2)) & gSineMask;
	return SCComplex(mag * gSine[cosindex], mag * gSine[sinindex]);
}

void SCComplex::ToPolarApxInPlace()
{
	SCPolar polar = ToPolarApx();
	real = polar.mag;
	imag = polar.phase;
}

void SCPolar::ToComplexApxInPlace()
{
	SCComplex complx = ToComplexApx();
	mag = complx.real;
	phase = complx.imag;
}

////////////////////////////////////////////////////////////////////////////////

