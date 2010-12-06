//  $Id$
//
//  templated oscillator class
//  Copyright (C) 2007 Tim Blechmann
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

//  $Revision$
//  $LastChangedRevision$
//  $LastChangedDate$
//  $LastChangedBy$



#ifndef OSCILLATOR_HPP
#define OSCILLATOR_HPP

#include "interpolation.hpp"
#include "phasor.hpp"
#include "utilities.hpp"


namespace nova
{
template <typename sample_type = float,
          typename table_type = sample_type,
          typename internal_type = sample_type,
          uint table_size = 64,
          uint interpolation_precision = 0>
class fixed_table_lookup_oscillator:
    private phasor<internal_type, internal_type, table_size>
{
    static const uint interpolation_points = interpolation_precision + 1;

    BOOST_STATIC_ASSERT(boost::is_floating_point<sample_type>::value);
    BOOST_STATIC_ASSERT(boost::is_floating_point<internal_type>::value);
    BOOST_STATIC_ASSERT(boost::is_floating_point<table_type>::value);

    typedef phasor<internal_type, internal_type, table_size> phasor_type;

public:
    /* void set frequency for control-driven operation */
    void set_frequency(internal_type const & new_frequency)
    {
        phasor_type::set_frequency(new_frequency);
    }

    /* void set phase */
    void set_phase(internal_type const & new_phase)
    {
        phasor_type::set_phase(new_phase);
    }

    /* set inverse samplerate for signal-driven operation */
    void set_inv_samplerate(internal_type const & new_inv_samplerate)
    {
        phasor_type::set_inv_samplerate(new_inv_samplerate);
    }

    template <typename output_buffer_type, typename wavetable_type>
    inline void perform(output_buffer_type & out, uint n, wavetable_type const & wavetable)
    {
        sample_type * out_sample = get_samples(out);
        const table_type * table = get_samples(wavetable);

        internal_type phase = phasor_type::phase_;
        do
        {
            internal_type index = phasor_type::compute_sample(phase, phasor_type::phase_inc_);

            *out_sample = interpolation_wrapper<interpolation_precision>(table, index);

            ++out_sample;
            --n;
        }
        while (n);
        phasor_type::phase_ = phase;
    }

    template <typename input_buffer_type, typename output_buffer_type, typename wavetable_type>
    inline void perform(input_buffer_type const & in, output_buffer_type & out, uint n, wavetable_type const & wavetable)
    {
        const sample_type * in_sample = get_samples(in);
        sample_type * out_sample = get_samples(out);

        const table_type * table = get_samples(wavetable);

        internal_type phase = phasor_type::phase_;
        do
        {
            internal_type phase_inc = phasor_type::freq_factor * *in_sample;
            internal_type index = phasor_type::compute_sample(phase, phase_inc);

            *out_sample = interpolation_wrapper<interpolation_precision>(table, index);

            ++out_sample;
            ++in_sample;
            --n;
        }
        while (n);
        phasor_type::phase_ = phase;
    }

};

template <typename sample_type = float,
          typename table_type = sample_type,
          typename internal_type = sample_type,
          uint interpolation_precision = 0>
class table_lookup_oscillator:
    private phasor<internal_type, internal_type, 1>
{
    static const uint interpolation_points = interpolation_precision + 1;

    BOOST_STATIC_ASSERT(boost::is_floating_point<sample_type>::value);
    BOOST_STATIC_ASSERT(boost::is_floating_point<internal_type>::value);
    BOOST_STATIC_ASSERT(boost::is_floating_point<table_type>::value);

    typedef phasor<internal_type, internal_type, 1> phasor_type;

public:
    /* void set frequency for control-driven operation */
    void set_frequency(internal_type const & new_frequency)
    {
        phasor_type::set_frequency(new_frequency);
    }

    /* void set phase */
    void set_phase(internal_type const & new_phase)
    {
        phasor_type::set_phase(new_phase);
    }

    /* set inverse samplerate for signal-driven operation */
    void set_inv_samplerate(internal_type const & new_inv_samplerate)
    {
        phasor_type::set_inv_samplerate(new_inv_samplerate);
    }

    template <typename output_buffer_type, typename wavetable_type>
    inline void perform(output_buffer_type & out, uint n, wavetable_type const & wavetable, uint table_size)
    {
        sample_type * out_sample = get_samples(out);
        const table_type * table = get_samples(wavetable);

        internal_type phase = phasor_type::phase_;
        do
        {
            internal_type index = phasor_type::compute_sample(phase, phasor_type::phase_inc_) * table_size;

            *out_sample = interpolation_wrapper<interpolation_precision>(table, index);

            ++out_sample;
            --n;
        }
        while (n);
        phasor_type::phase_ = phase;
    }

    template <typename input_buffer_type, typename output_buffer_type, typename wavetable_type>
    inline void perform(input_buffer_type const & in, output_buffer_type & out, uint n, wavetable_type const & wavetable,
                        uint table_size)
    {
        const sample_type * in_sample = get_samples(in);
        sample_type * out_sample = get_samples(out);

        const table_type * table = get_samples(wavetable);

        internal_type phase = phasor_type::phase_;
        do
        {
            internal_type phase_inc = phasor_type::freq_factor * *in_sample;
            internal_type index = phasor_type::compute_sample(phase, phase_inc) * table_size;

            *out_sample = interpolation_wrapper<interpolation_precision>(table, index);

            ++out_sample;
            ++in_sample;
            --n;
        }
        while (n);
        phasor_type::phase_ = phase;
    }

};


} /* namespace nova */

#endif /* OSCILLATOR_HPP */
