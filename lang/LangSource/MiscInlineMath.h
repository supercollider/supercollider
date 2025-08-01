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

#define NUMPRIMES 6542
std::int64_t nthPrime(int n);
std::int64_t findPrime(int n);
std::int64_t prevPrime(int n);
std::int64_t nextPrime(int n);


inline double linlin(double x, double a, double b, double c, double d) {
    if (x <= a)
        return c;
    if (x >= b)
        return d;
    return (x - a) / (b - a) * (d - c) + c;
}

inline double explin(double x, double a, double b, double c, double d) {
    if (x <= a)
        return c;
    if (x >= b)
        return d;
    return (log(x / a)) / (log(b / a)) * (d - c) + c;
}

inline double expexp(double x, double a, double b, double c, double d) {
    if (x <= a)
        return c;
    if (x >= b)
        return d;
    return pow(d / c, log(x / a) / log(b / a)) * c;
}

inline double linexp(double x, double a, double b, double c, double d) {
    if (x <= a)
        return c;
    if (x >= b)
        return d;
    return pow(d / c, (x - a) / (b - a)) * c;
}
