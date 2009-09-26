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


#ifndef _SCComplex_
#define _SCComplex_

////////////////////////////////////////////////////////////////////////////////

struct SCPolar;

struct SCComplex
{
	SCComplex(float r, float i) : real(r), imag(i) {}
	void Set(float r, float i) { real = r; imag = i; }

	SCComplex& operator=(SCComplex b) { real = b.real; imag = b.imag; return *this; }
	SCComplex& operator=(float b) { real = b; imag = 0.; return *this; }

	SCPolar ToPolar();
	SCPolar ToPolarApx();

	void ToPolarInPlace();
	void ToPolarApxInPlace();

	float real, imag;
};

struct SCPolar
{
	SCPolar(float m, float p) : mag(m), phase(p) {}
	void Set(float m, float p) { mag = m; phase = p; }

	SCPolar& operator=(SCPolar b) { mag = b.mag; phase = b.phase; return *this; }
	SCPolar& operator=(float b) { mag = b; phase = 0.; return *this; }

	SCComplex ToComplex();
	SCComplex ToComplexApx();

	void ToComplexInPlace();
	void ToComplexApxInPlace();

	float mag, phase;
};

void ToComplex(SCPolar in, SCComplex& out);

inline SCComplex operator+(SCComplex a, SCComplex b) { return SCComplex(a.real + b.real, a.imag + b.imag); }
inline SCComplex operator+(SCComplex a, float b) { return SCComplex(a.real + b, a.imag); }
inline SCComplex operator+(float a, SCComplex b) { return SCComplex(a + b.real, b.imag); }

inline SCComplex& operator+=(SCComplex& a, const SCComplex& b) { a.real += b.real, a.imag += b.imag; return a; }
inline SCComplex& operator+=(SCComplex& a, float b) { a.real += b; return a; }

inline SCComplex operator-(SCComplex a, SCComplex b) { return SCComplex(a.real - b.real, a.imag - b.imag); }
inline SCComplex operator-(SCComplex a, float b) { return SCComplex(a.real - b, a.imag); }
inline SCComplex operator-(float a, SCComplex b) { return SCComplex(a - b.real, b.imag); }

inline SCComplex operator-=(SCComplex a, SCComplex b) { a.real -= b.real, a.imag -= b.imag; return a; }
inline SCComplex operator-=(SCComplex a, float b) { a.real -= b; return a; }

inline SCComplex operator*(SCComplex a, SCComplex b)
{
	return SCComplex(a.real * b.real - a.imag * b.imag, a.real * b.imag + a.imag * b.real);
}

inline SCComplex operator*(SCComplex a, float b)
{
	return SCComplex(a.real * b, a.imag * b);
}

inline SCComplex operator*(float a, SCComplex b)
{
	return SCComplex(b.real * a, b.imag * a);
}

inline SCComplex operator*=(SCComplex a, SCComplex b)
{
	a.Set(
		a.real * b.real - a.imag * b.imag,
		a.real * b.imag + a.imag * b.real
	);
	return a;
}

inline SCComplex operator*=(SCComplex a, float b)
{
	a.real *= b;
	a.imag *= b;
	return a;
}


inline SCPolar operator*(SCPolar a, float b)
{
	return SCPolar(a.mag * b, a.phase);
}

inline SCPolar operator*(float a, SCPolar b)
{
	return SCPolar(a * b.mag, b.phase);
}

inline SCPolar operator*=(SCPolar a, float b)
{
	a.mag *= b;
	return a;
}


#endif
