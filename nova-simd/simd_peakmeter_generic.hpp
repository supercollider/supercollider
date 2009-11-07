//  simd functions for peak metering, generic implementation
//  Copyright (C) 2009 Tim Blechmann
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; see the file COPYING.  If not, write to
//  the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
//  Boston, MA 02111-1307, USA.


#ifndef SIMD_PEAKMETER_GENERIC_HPP
#define SIMD_PEAKMETER_GENERIC_HPP

#include <cmath>                /* for abs */
#include <algorithm>            /* for max */

namespace nova
{

/* updates peak, returns last abs(in[n-1]) */
template <typename F>
F peak_vec(const F * in, F * peak, unsigned int n)
{
    F last;
    F local_peak = *peak;
    using namespace std;

    do {
        last = abs(*in++);
        local_peak = max(local_peak, last);
    } while(--n);

    *peak = local_peak;
    return last;
}

template <typename F>
F peak_vec_simd(const F * in, F * peak, unsigned int n)
{
    n /= 8;
    F last;
    F local_peak = *peak;

    using namespace std;
    do {
        last = abs(in[0]); local_peak = max(local_peak, last);
        last = abs(in[1]); local_peak = max(local_peak, last);
        last = abs(in[2]); local_peak = max(local_peak, last);
        last = abs(in[3]); local_peak = max(local_peak, last);
        last = abs(in[4]); local_peak = max(local_peak, last);
        last = abs(in[5]); local_peak = max(local_peak, last);
        last = abs(in[6]); local_peak = max(local_peak, last);
        last = abs(in[7]); local_peak = max(local_peak, last);
        in += 8;
    } while(--n);

    *peak = local_peak;
    return last;
}

} /* namespace nova */


#endif /* SIMD_PEAKMETER_GENERIC_HPP */
