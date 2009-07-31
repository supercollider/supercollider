//  templated sine oscillator class
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


#ifndef SINE_OSCILLATOR_HPP
#define SINE_OSCILLATOR_HPP

#include "oscillator.hpp"
#include "cos_table.hpp"

namespace nova
{
template <typename sample_type = float,
          typename table_type = sample_type,
          typename internal_type = sample_type,
          uint table_size = 4096,
          uint interpolation_precision = 1>
class sine_oscillator:
    private fixed_table_lookup_oscillator<sample_type, table_type, internal_type, table_size, interpolation_precision>
{
    static const uint interpolation_points = interpolation_precision + 1;

    typedef fixed_table_lookup_oscillator<sample_type, table_type, internal_type,
                                          table_size, interpolation_precision
                                          > base_type;

    typedef cos_table<table_type, table_size, 1> wave_table_type;
    static const wave_table_type cos_table_;

public:
    /* void set frequency for control-driven operation */
    void set_frequency(internal_type const & new_frequency)
    {
        base_type::set_frequency(new_frequency);
    }

    /* void set phase */
    void set_phase(internal_type const & new_phase)
    {
        base_type::set_phase(new_phase);
    }

    /* set inverse samplerate for signal-driven operation */
    void set_inv_samplerate(internal_type const & new_inv_samplerate)
    {
        base_type::set_inv_samplerate(new_inv_samplerate);
    }

    template <typename output_buffer_type>
    inline void perform(output_buffer_type & out, uint n)
    {
        base_type::perform(out, n, cos_table_);
    }

    template <typename input_buffer_type, typename output_buffer_type>
    inline void perform(input_buffer_type const & in, output_buffer_type & out, uint n)
    {
        base_type::perform(in, out, n, cos_table_);
    }
};

template <typename sample_type, typename table_type, typename internal_type, uint table_size,
          uint interpolation_precision>
const typename sine_oscillator<sample_type, table_type, internal_type, table_size, interpolation_precision>::wave_table_type
sine_oscillator<sample_type, table_type, internal_type, table_size, interpolation_precision>::cos_table_;

} /* namespace nova */

#endif /* SINE_OSCILLATOR_HPP */
