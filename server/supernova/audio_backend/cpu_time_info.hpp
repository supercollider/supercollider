//  cpu time info
//  Copyright (C) 2011-2015 Tim Blechmann
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

#pragma once

#include <vector>

#include <boost/align/aligned_allocator.hpp>

#include "nova-simd/simd_horizontal_functions.hpp"

namespace nova {

class cpu_time_info {
    typedef std::vector<float, boost::alignment::aligned_allocator<float, 64>> ringbuffer;

public:
    cpu_time_info() {}

    void resize(int sampleRate, int blockSize, int seconds = 1) {
        const size_t blocks = sampleRate * seconds / blockSize;
        size = std::max(size_t(1), blocks);
        buffer.resize(size, 0.f);
        index = size - 1;
    }

    void update(float f) {
        ++index;
        if (index == size)
            index = 0;

        buffer[index] = f;
    }

    void get(float& peak, float& average) const {
        const float average_factor = 1.f / size;
        float sum;
        horizontal_maxsum_vec_simd(peak, sum, buffer.data(), size);
        average = sum * average_factor;
    }

private:
    std::size_t size = 0;
    std::size_t index = 0;
    ringbuffer buffer;
};

}
