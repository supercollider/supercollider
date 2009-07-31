//  fast sine oscillator
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


#ifndef DSP_FAST_SINOSC_HPP
#define DSP_FAST_SINOSC_HPP

#include <cmath>

#include <boost/math/constants/constants.hpp>

#include "sample_extractor.hpp"
#include "../utilities/branch_hints.hpp"

#include "muladd_helpers.hpp"

namespace nova
{

/* fast sine wav oscillator based on a ringing filter, adapted from musicdsp */
template <typename sample_type, typename internal_type = sample_type>
class fast_sinosc
{
public:
    fast_sinosc(void):
        y1_(0), y2_(1)
    {}

    void set_frequency(internal_type const & fn)
    {
        internal_type w = fn * 2 * boost::math::constants::pi<internal_type>();
        b1 = 2*std::cos(w);
    }

    void set_frequency_with_phase(internal_type const & fn)
    {
        internal_type w = fn * 2 * boost::math::constants::pi<internal_type>();
        internal_type phase = std::asin(y1_);
        b1 = 2*std::cos(w);
        y2_ = std::sin(phase - w);
    }

    void init(internal_type const & fn, internal_type const & ip)
    {
        internal_type w = fn * 2 * boost::math::constants::pi<internal_type>();

        b1 = 2*std::cos(w);

        y1_ = std::sin(ip - w);
        y2_ = std::sin(ip - 2*w);
    }

    template <typename output_buffer_type, typename sample_count_t, typename muladd_helper>
    inline void perform(output_buffer_type & out, sample_count_t n, muladd_helper & ma)
    {
        assert(n > 0);
        sample_type * out_sample = get_samples(out);

        register internal_type y1 = y1_;
        register internal_type y2 = y2_;

        while (likely(n >= 3))
        {
            internal_type y0 = b1 * y1 - y2;
            *out_sample++ = ma(sample_type(y0));
            y2 = b1 * y0 - y1;
            *out_sample++ = ma(sample_type(y2));
            y1 = b1 * y2 - y0;
            *out_sample++ = ma(sample_type(y1));
            n -= 3;
        }

        while (n)
        {
            internal_type y0 = b1 * y1 - y2;
            *out_sample++ = ma(sample_type(y0));
            --n;
        }

        y1_ = y1;
        y2_ = y2;
    }

    DEFINE_PERFORM_WRAPPER_0_1

protected:
    internal_type y1_, y2_;
    internal_type b1;
};

template <typename sample_type, typename internal_type = sample_type>
class fast_sinosc_sa:
    fast_sinosc<sample_type, internal_type>
{
    fast_sinosc_sa(void)
    {}

    template <typename input_buffer_type, typename output_buffer_type, typename sample_count_t, typename muladd_helper>
    inline void perform(input_buffer_type const & in, output_buffer_type & out, sample_count_t n, muladd_helper & ma)
    {
        assert(n > 0);
        sample_type * out_sample = get_samples(out);
        const sample_type * in_sample = get_samples(out);

        register internal_type y1 = fast_sinosc<sample_type, internal_type>::y1_;
        register internal_type y2 = fast_sinosc<sample_type, internal_type>::y2_;
        internal_type b1 = fast_sinosc<sample_type, internal_type>::b1;
        internal_type f = f_;

        while (n)
        {
            internal_type fn = *in_sample++;

            if (fn != f)
            {
                internal_type w = fn * 2 * boost::math::constants::pi<internal_type>() * inv_sr;
                b1 = 2*std::cos(w);
                f = fn;
            }

            internal_type y0 = b1 * y1 - y2;
            *out_sample++ = ma(sample_type(y0));
            --n;
        }

        fast_sinosc<sample_type, internal_type>::y1_ = y1;
        fast_sinosc<sample_type, internal_type>::y2_ = y2;
        fast_sinosc<sample_type, internal_type>::b1 = b1;
        f_ = f;
    }

    DEFINE_PERFORM_WRAPPER_1_1

private:
    internal_type inv_sr;
    internal_type f_;
};


} /* namespace nova */

#endif /* DSP_FAST_SINOSC_HPP */
