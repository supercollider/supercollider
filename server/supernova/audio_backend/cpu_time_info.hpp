//  cpu time info
//  Copyright (C) 2011 Tim Blechmann
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

#ifndef AUDIO_BACKEND_CPU_TIME_INFO_HPP
#define AUDIO_BACKEND_CPU_TIME_INFO_HPP

#include <vector>
#include "utilities/aligned_class.hpp"
#include "nova-simd/simd_horizontal_functions.hpp"

namespace nova {

struct cpu_time_info
{
    static const size_t size = 512; // 700 ms at 128 samples 96kHz, 6.9 s at 512 samples 44.1kHz

    cpu_time_info(void):
        index(size - 1), buffer(size, 0.f)
    {}

    void update(float f)
    {
        ++index;
        if (index == size)
            index = 0;

        buffer[index] = f;
    }

    void get(float & peak, float & average) const
    {
        const float average_factor = 1.f/size;
        float sum;
#ifdef __PATHCC__
        horizontal_maxsum_vec_simd(peak, sum, &buffer.front(), size);
#else
        horizontal_maxsum_vec_simd(peak, sum, buffer.data(), size);
#endif
        average = sum * average_factor;
    }

    std::size_t index;
    std::vector<float, aligned_allocator<float> > buffer;
};

}

#endif /* AUDIO_BACKEND_CPU_TIME_INFO_HPP */
