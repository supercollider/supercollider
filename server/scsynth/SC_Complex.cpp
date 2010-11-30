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


#include "SC_Complex.h"
#include "SC_Constants.h"
#include "SC_Samp.h"
#include <math.h>

////////////////////////////////////////////////////////////////////////////////

const int32 kPolarLUTSize = 2049;
const int32 kPolarLUTSize2 = kPolarLUTSize >> 1;
const double kSinePhaseScale = (double)kSineSize / twopi;
float gMagLUT[kPolarLUTSize];
float gPhaseLUT[kPolarLUTSize];

void BuildPolarLUT();
void BuildPolarLUT()
{
	double rPolarLUTSize2 = 1. / kPolarLUTSize2;
	for (int i=0; i < kPolarLUTSize; ++i) {
		double slope = (i - kPolarLUTSize2) * rPolarLUTSize2;
		double angle = atan(slope);
		gPhaseLUT[i] = (float)angle;
		gMagLUT[i] = 1.f / (float)cos(angle);
	}
}

Polar Complex::ToPolar()
{
	return Polar(hypot(imag, real), atan2(imag, real));
}

Complex Polar::ToComplex()
{
	return Complex(mag * cos(phase), mag * sin(phase));
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

/**
* Converts cartesian to polar representation, using lookup tables.
* Note: in this implementation the phase values returned lie in the range [-pi/4, 7pi/4]
* rather than the more conventional [0, 2pi] or [-pi, pi].
*/
Polar Complex::ToPolarApx()
{
	int32 index;
	float absreal = fabs(real);
	float absimag = fabs(imag);
	float mag, phase, slope;
	if (absreal > absimag) {
		slope = imag/real;
		index = (int32)(kPolarLUTSize2 + kPolarLUTSize2 * slope);
		mag = gMagLUT[index] * absreal;
		phase = gPhaseLUT[index];
		if (real > 0) {
			return Polar(mag, phase);
		} else {
			return Polar(mag, (float)(pi + phase));
		}
	} else {
		slope = real/imag;
		index = (int32)(kPolarLUTSize2 + kPolarLUTSize2 * slope);
		mag = gMagLUT[index] * absimag;
		phase = gPhaseLUT[index];
		if (imag > 0) {
			return Polar(mag, (float)(pi2 - phase));
		} else {
			return Polar(mag, (float)(pi32 - phase));
		}
	}
}

void Complex::ToPolarInPlace()
{
	Polar polar = ToPolar();
	real = polar.mag;
	imag = polar.phase;
}

void Polar::ToComplexInPlace()
{
	Complex complx = ToComplex();
	mag = complx.real;
	phase = complx.imag;
}

Complex Polar::ToComplexApx()
{
	uint32 sinindex = (int32)(kSinePhaseScale * phase) & kSineMask;
	uint32 cosindex = (sinindex + (kSineSize>>2)) & kSineMask;
	return Complex(mag * gSine[cosindex], mag * gSine[sinindex]);
}

void Complex::ToPolarApxInPlace()
{
	Polar polar = ToPolarApx();
	real = polar.mag;
	imag = polar.phase;
}

void Polar::ToComplexApxInPlace()
{
	Complex complx = ToComplexApx();
	mag = complx.real;
	phase = complx.imag;
}

////////////////////////////////////////////////////////////////////////////////

