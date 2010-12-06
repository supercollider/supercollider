//  generic signal analizer classes
//  Copyright (C) 2008 Tim Blechmann
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

#ifndef DSP_ANALYZE_HPP
#define DSP_ANALYZE_HPP

#include "sample_extractor.hpp"
#include "../utilities/branch_hints.hpp"

#include "muladd_helpers.hpp"

namespace nova
{

template <typename sample_type>
class zero_crossings
{
    enum state_t {pos, neg};

    state_t state_;

    static sample_type compute_sample(sample_type current_sample, state_t & state)
    {
        if (current_sample > 0)
            if (likely(state == pos))
                return 0;
            else
            {
                state = pos;
                return sample_type(1.f);
            }
        else
            if (likely(state == neg))
                return 0;
            else
            {
                state = neg;
                return sample_type(1.f);
            }
    }

public:
    zero_crossings(void):
        state_(neg)
    {}

    template <typename input_buffer_type, typename output_buffer_type, typename sample_count_t,
              typename muladd_helper>
    inline void perform(input_buffer_type const & in, output_buffer_type & out, sample_count_t n,
                        muladd_helper & ma)
    {
        const sample_type * in_sample = get_samples(in);
        sample_type * out_sample = get_samples(out);

        state_t state = state_;

        do
        {
            *out_sample++ = ma(compute_sample(*in_sample++, state));
            --n;
        }
        while(n);
        state_ = state;
    }

    DEFINE_PERFORM_WRAPPER_1_1
};

template <typename sample_type>
class zero_crossing_direction
{
    enum state_t {pos, null, neg};

    state_t state_;

    static sample_type compute_sample(sample_type current_sample, state_t & state)
    {
        if (current_sample > 0)
        {
            if (likely(state == pos))
                return 0;
            else
            {
                state = pos;
                return 1.f;
            }
        }
        else if (current_sample < 0)
        {
            if (likely(state == neg))
                return 0;
            else
            {
                state = neg;
                return -1.f;
            }
        }
        else
        {
            /* current_sample == 0 */
            state = null;
            return 0;
        }
    }

public:
    zero_crossing_direction(void):
        state_(neg)
    {}

    template <typename input_buffer_type, typename output_buffer_type, typename sample_count_t,
              typename muladd_helper>
    inline void perform(input_buffer_type const & in, output_buffer_type & out, sample_count_t n,
                        muladd_helper & ma)
    {
        const sample_type * in_sample = get_samples(in);
        sample_type * out_sample = get_samples(out);

        state_t state = state_;

        do
        {
            *out_sample++ = ma(compute_sample(*in_sample++, state));
            --n;
        }
        while(n);
        state_ = state;
    }

    DEFINE_PERFORM_WRAPPER_1_1
};

template <typename sample_type>
class signal_change
{
    sample_type last_;

    static sample_type compute_sample(sample_type current_sample, sample_type & last)
    {
        sample_type ret;
        if (current_sample < last)
            ret = -1.f;
        else if (current_sample > last)
            ret = 1.f;
        else
            ret = 0.f;

        last = current_sample;
        return ret;
    }

public:
    signal_change(void):
        last_(0)
    {}

    template <typename input_buffer_type, typename output_buffer_type, typename sample_count_t,
              typename muladd_helper>
    inline void perform(input_buffer_type const & in, output_buffer_type & out, sample_count_t n,
                        muladd_helper & ma)
    {
        const sample_type * in_sample = get_samples(in);
        sample_type * out_sample = get_samples(out);

        sample_type last = last_;

        do
        {
            *out_sample++ = ma(compute_sample(*in_sample++, last));
            --n;
        }
        while(n);
        last_ = last;
    }

    DEFINE_PERFORM_WRAPPER_1_1
};

template <typename sample_type>
class signal_delta
{
    sample_type last_;

    static sample_type compute_sample(sample_type current_sample, sample_type & last)
    {
        sample_type ret = current_sample - last;

        last = current_sample;
        return ret;
    }

public:
    signal_delta(void):
        last_(0)
    {}

    template <typename input_buffer_type, typename output_buffer_type, typename sample_count_t,
              typename muladd_helper>
    inline void perform(input_buffer_type const & in, output_buffer_type & out, sample_count_t n,
                        muladd_helper & ma)
    {
        const sample_type * in_sample = get_samples(in);
        sample_type * out_sample = get_samples(out);

        sample_type last = last_;

        do
        {
            *out_sample++ = ma(compute_sample(*in_sample++, last));
            --n;
        }
        while(n);
        last_ = last;
    }

    DEFINE_PERFORM_WRAPPER_1_1
};


template <typename sample_type>
class signal_threshold
{
    sample_type low_;
    sample_type high_;
    bool up_;

    static sample_type compute_sample(sample_type current_sample, sample_type low, sample_type high)
    {
        if (current_sample >= high or current_sample <= low)
            return 1.f;
        else
            return 0.f;
    }

public:
    signal_threshold(sample_type const & low = 0, sample_type const & high = 9):
        low_(low), high_(high)
    {}

    void set_low(sample_type const & low)
    {
        low_ = low;
    }

    void set_high(sample_type const & high)
    {
        high_ = high;
    }

    template <typename input_buffer_type, typename output_buffer_type, typename sample_count_t,
              typename muladd_helper>
    inline void perform(input_buffer_type const & in, output_buffer_type & out, sample_count_t n, muladd_helper & ma)
    {
        const sample_type * in_sample = get_samples(in);
        sample_type * out_sample = get_samples(out);

        const sample_type low = low_;
        const sample_type high = high_;

        do
        {
            *out_sample++ = ma(compute_sample(*in_sample++, low, high));
        }
        while (--n);
    }

    DEFINE_PERFORM_WRAPPER_1_1
};


} /* namespace nova */

#endif /* DSP_ANALYZE_HPP */
