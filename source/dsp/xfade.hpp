//  crossfade class
//  Copyright (C) 2008, 2009 Tim Blechmann
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


#ifndef DSP_XFADE_HPP
#define DSP_XFADE_HPP

#include <cmath>

#include "../utilities/branch_hints.hpp"

#include "nova-simd/simd_memory.hpp"
#include "nova-simd/simd_mix.hpp"
#include "muladd_helpers.hpp"
#include "sample_extractor.hpp"
#include "shared_function_tables.hpp"

namespace nova
{

template <typename sample_type>
class xfade
{
public:
    xfade(void)
    {}

    void init(sample_type const & fade)
    {
        sample_type pos = fade * sample_type(sine_table_size/4) + sample_type(sine_table_size/4);

        gain0 = shared_half_sin_table<sample_type, sine_table_size, 0>::get(int(pos));
        gain1 = shared_half_sin_table<sample_type, sine_table_size, 0>::get(sine_table_size/4 - int(pos));
    }

    static const int sine_table_size = 4092;

    void set_fade(sample_type const & fade, unsigned int fade_length)
    {
        sample_type pos = fade * sample_type(sine_table_size/4) + sample_type(sine_table_size/4);

        sample_type new_gain0 = shared_half_sin_table<sample_type, sine_table_size, 0>::get(int(pos));
        sample_type new_gain1 = shared_half_sin_table<sample_type, sine_table_size, 0>::get(sine_table_size/2 - int(pos));

        gain0_slope = (new_gain0 - gain0) / fade_length;
        gain1_slope = (new_gain1 - gain1) / fade_length;

        remaining = fade_length;
    }

    template <typename input_buffer_type, typename output_buffer_type, typename sample_count_t,
              typename muladd_helper>
    inline void perform(input_buffer_type const & in0, input_buffer_type const & in1,
                        output_buffer_type & out, sample_count_t n, muladd_helper & ma)
    {
        assert(n > 0);
        sample_type * out_sample = get_samples(out);
        const sample_type * in_sample0 = get_samples(in0);
        const sample_type * in_sample1 = get_samples(in1);

        if (remaining)
        {
            for (sample_count_t i = 0; i != n; ++i)
            {
                out_sample[i] = ma(in_sample0[i] * gain0 + in_sample1[i] * gain1);
                gain0 += gain0_slope;
                gain1 += gain1_slope;
            }

            remaining -= n;
            if (remaining < 0)
                remaining = 0;
        }
        else
        {
            for (sample_count_t i = 0; i != n; ++i)
                out_sample[i] = ma(in_sample0[i] * gain0 + in_sample1[i] * gain1);
        }
    }

    template <typename input_buffer_type, typename output_buffer_type, typename sample_count_t>
    inline void perform(input_buffer_type const & in0, input_buffer_type const & in1,
                          output_buffer_type & out, sample_count_t n)
    {
        assert(n > 0);
        sample_type * out_sample = get_samples(out);
        const sample_type * in_sample0 = get_samples(in0);
        const sample_type * in_sample1 = get_samples(in1);

        if (remaining)
        {
            for (sample_count_t i = 0; i != n; ++i)
            {
                out_sample[i] = in_sample0[i] * gain0 + in_sample1[i] * gain1;
                gain0 += gain0_slope;
                gain1 += gain1_slope;
            }

            remaining -= n;
            if (remaining < 0)
                remaining = 0;
        }
        else
        {
            mix_vec(out_sample, in_sample0, gain0, in_sample1, gain1, n);
        }
    }

    template <typename input_buffer_type, typename output_buffer_type, typename sample_count_t>
    inline void perform_8(input_buffer_type const & in0, input_buffer_type const & in1,
                        output_buffer_type & out, sample_count_t n)
    {
        assert(n > 0);
        sample_type * out_sample = get_samples(out);
        const sample_type * in_sample0 = get_samples(in0);
        const sample_type * in_sample1 = get_samples(in1);

        if (remaining)
        {
            for (sample_count_t i = 0; i != n; ++i)
            {
                out_sample[i] = in_sample0[i] * gain0 + in_sample1[i] * gain1;
                gain0 += gain0_slope;
                gain1 += gain1_slope;
            }

            remaining -= n;
            if (remaining < 0)
                remaining = 0;
        }
        else
        {
            mix_vec_simd(out_sample, in_sample0, gain0, in_sample1, gain1, n);
        }
    }


private:
    int remaining;
    sample_type gain0, gain1;
    sample_type gain0_slope, gain1_slope;
};


template <typename sample_type>
class xfade_ar
{
public:
    xfade_ar(void)
    {}

    static const int sine_table_size = 4092;

    template <typename input_buffer_type, typename output_buffer_type, typename sample_count_t,
              typename muladd_helper>
    inline void perform(input_buffer_type const & in0, input_buffer_type const & in1, input_buffer_type const & pos,
                        output_buffer_type & out, sample_count_t n, muladd_helper & ma)
    {
        assert(n > 0);
        sample_type * out_sample = get_samples(out);
        const sample_type * in_sample0 = get_samples(in0);
        const sample_type * in_sample1 = get_samples(in1);
        const sample_type * in_sample_pos = get_samples(pos);

        for (sample_count_t i = 0; i != n; ++i)
        {
            sample_type fade = in_sample_pos[i] * sample_type(sine_table_size/4) + sample_type(sine_table_size/4);

            sample_type gain0 = shared_half_sin_table<sample_type, sine_table_size, 0>::get(int(fade));
            sample_type gain1 = shared_half_sin_table<sample_type, sine_table_size, 0>::get(sine_table_size/2 - int(fade));

            out_sample[i] = ma(in_sample0[i] * gain0 + in_sample1[i] * gain1);
        }
    }

    DEFINE_PERFORM_WRAPPER_3_1
};


} /* namespace nova */

#endif /* DSP_XFADE_HPP */
