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

#define NUMPRIMES 6542
long nthPrime(int n);
long findPrime(int n);
long prevPrime(int n);
long nextPrime(int n);


inline double linlin(double x, double a, double b, double c, double d)
{
    if (x <= a) return c;
    if (x >= b) return d;
    return (x-a)/(b-a) * (d-c) + c;
}

inline double explin(double x, double a, double b, double c, double d)
{
    if (x <= a) return c;
    if (x >= b) return d;
    return (log(x/a)) / (log(b/a)) * (d-c) + c;
}

inline double expexp(double x, double a, double b, double c, double d)
{
    if (x <= a) return c;
    if (x >= b) return d;
    return pow(d/c, log(x/a)) / (log(b/a)) * c;
}

inline double linexp(double x, double a, double b, double c, double d)
{
    if (x <= a) return c;
    if (x >= b) return d;
    return pow(d/c, (x-a)/(b-a)) * c;
}
