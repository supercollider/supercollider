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


#ifndef _SC_Complex_
#define _SC_Complex_

#include "SC_Types.h"
#include "float.h"

////////////////////////////////////////////////////////////////////////////////

struct Polar;

struct Complex
{
	Complex() {}
	Complex(float r, float i) : real(r), imag(i) {}
	void Set(float r, float i) { real = r; imag = i; }

	Complex& operator=(Complex b) { real = b.real; imag = b.imag; return *this; }
	Complex& operator=(float b) { real = b; imag = 0.; return *this; }

	Polar ToPolar();
	Polar ToPolarApx();

	void ToPolarInPlace();
	void ToPolarApxInPlace();

	float real, imag;
};

struct Polar
{
	Polar() {}
	Polar(float m, float p) : mag(m), phase(p) {}
	void Set(float m, float p) { mag = m; phase = p; }

	Complex ToComplex();
	Complex ToComplexApx();

	void ToComplexInPlace();
	void ToComplexApxInPlace();

	float mag, phase;
};

struct ComplexFT
{
	float dc, nyq;
	Complex complex[1];
};

struct PolarFT
{
	float dc, nyq;
	Polar polar[1];
};

void ToComplex(Polar in, Complex& out);

inline Complex operator+(Complex a, Complex b) { return Complex(a.real + b.real, a.imag + b.imag); }
inline Complex operator+(Complex a, float b) { return Complex(a.real + b, a.imag); }
inline Complex operator+(float a, Complex b) { return Complex(a + b.real, b.imag); }

inline Complex& operator+=(Complex& a, const Complex& b) { a.real += b.real, a.imag += b.imag; return a; }
inline Complex& operator+=(Complex& a, float b) { a.real += b; return a; }

inline Complex operator-(Complex a, Complex b) { return Complex(a.real - b.real, a.imag - b.imag); }
inline Complex operator-(Complex a, float b) { return Complex(a.real - b, a.imag); }
inline Complex operator-(float a, Complex b) { return Complex(a - b.real, b.imag); }

inline Complex operator-=(Complex a, Complex b) { a.real -= b.real, a.imag -= b.imag; return a; }
inline Complex operator-=(Complex a, float b) { a.real -= b; return a; }

inline Complex operator*(Complex a, Complex b)
{
	return Complex(a.real * b.real - a.imag * b.imag, a.real * b.imag + a.imag * b.real);
}

inline Complex operator*(Complex a, float b)
{
	return Complex(a.real * b, a.imag * b);
}

inline Complex operator*(float a, Complex b)
{
	return Complex(b.real * a, b.imag * a);
}

inline Complex operator*=(Complex a, Complex b)
{
	a.Set(
		a.real * b.real - a.imag * b.imag,
		a.real * b.imag + a.imag * b.real
	);
	return a;
}

inline Complex operator*=(Complex a, float b)
{
	a.real *= b;
	a.imag *= b;
	return a;
}


inline Polar operator*(Polar a, float b)
{
	return Polar(a.mag * b, a.phase);
}

inline Polar operator*(float a, Polar b)
{
	return Polar(a * b.mag, b.phase);
}

inline Polar operator*=(Polar a, float b)
{
	a.mag *= b;
	return a;
}


#endif
