//  lag implementations
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

#ifndef DSP_LAG_HPP
#define DSP_LAG_HPP

#include <boost/array.hpp>
#include <cmath>

#include <boost/mpl/if.hpp>
#include <boost/mpl/bool.hpp>

#include "sample_extractor.hpp"
#include "parameter.hpp"
#include "../utilities/branch_hints.hpp"

namespace nova
{

template <unsigned int order,
          typename sample_type,
          typename internal_type = sample_type>
class lag_n
{
public:
    lag_n(void):
        b1_(0), b1_old(0), dirty(false)
    {
        state_.assign(0);
    }

    void set_lag(internal_type lag, internal_type samplerate)
    {
        if (lag)
        {
            b1_old = b1_;
            static const internal_type log001 = std::log(0.001);
            b1_ = std::exp(log001 / (lag * samplerate));
            dirty = true;
        }
        else
            b1_ = 0;
    }

    void tick(sample_type const & in, sample_type & out)
    {
        step(in, out, state_, b1_);
    }

    template <typename input_buffer_type, typename output_buffer_type>
    inline void perform(input_buffer_type const & in, output_buffer_type & out, uint n)
    {
        const sample_type * in_sample = get_samples(in);
        sample_type * out_sample = get_samples(out);

        boost::array<internal_type, order> state (state_);

        if (unlikely(dirty))
        {
            internal_type b1_slope = (b1_ - b1_old) / n;
            internal_type b1 = b1_;

            for (uint i = 0; i != n; ++i)
            {
                b1 += b1_slope;
                step(in[i], out[i], state, b1);
            }
            dirty = false;
        }
        else
        {
            for (uint i = 0; i != n; ++i)
                step(in[i], out[i], state, b1_);
        }

        state_ = state;
    }

private:
    inline void step(sample_type const & in, sample_type & out,
                     boost::array<internal_type, order> & state,
                     internal_type b1)
    {
        internal_type current = in;

        for (unsigned int i = 0; i != order; ++i)
        {
            current = current + b1_ * (state[i] - current);
            state[i] = current;
        }
        out = current;
    }

    boost::array<internal_type, order> state_;
    internal_type b1_, b1_old;
    bool dirty;
};


} /* namespace nova */

#endif /* DSP_LAG_HPP */
