//  $Id$
//
//  templated phasor class
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

#ifndef PHASOR_HPP
#define PHASOR_HPP

#include <boost/static_assert.hpp>
#include <boost/type_traits.hpp>
#include "../utilities/branch_hints.hpp"
#include "../utilities/template_functions.hpp"

#include "sample_extractor.hpp"

namespace nova
{

/* no template metaprogramming in a templated class .. */
namespace detail
{
/** \brief templated version of phasor step */
template<typename sample_type, typename internal_type>
inline sample_type phasor_compute_sample(internal_type & phase, const internal_type & phase_inc,
                                         const internal_type wrap)
{
    phase += phase_inc;
    phase = wrap_optimistic(phase, wrap);
    return sample_type(phase);
}

/** \brief template metaprogramming version of the phasor loop */
/* @{ */
#define PHASOR_PERFORM(sample_type, internal_type)                      \
    template<uint n>                                                    \
    inline void phasor_perform(const sample_type * in, sample_type * out, internal_type & phase, \
                               const internal_type freq_factor, const internal_type wrap) \
    {                                                                   \
        const internal_type phase_inc = internal_type(*in) * freq_factor; \
        *out = phasor_compute_sample<sample_type, internal_type>(phase, phase_inc, wrap); \
        phasor_perform<n-1>(in+1, out+1, phase, freq_factor, wrap);     \
    }                                                                   \
                                                                        \
    template<>                                                          \
    inline void phasor_perform<0>(const sample_type * in, sample_type * out, internal_type & phase, \
                                  const internal_type inv_samplerate, const internal_type wrap) \
    {                                                                   \
    }                                                                   \
                                                                        \
    template<uint n>                                                    \
    inline void phasor_perform(sample_type * out, internal_type & phase, const internal_type phase_inc, \
                               const internal_type wrap)                \
    {                                                                   \
        *out = phasor_compute_sample<sample_type, internal_type>(phase, phase_inc, wrap); \
        phasor_perform<n-1>(out+1, phase, phase_inc, wrap);             \
    }                                                                   \
                                                                        \
    template<>                                                          \
    inline void phasor_perform<0>(sample_type * out, internal_type & phase, const internal_type phase_inc, \
                                  const internal_type wrap)             \
    {                                                                   \
    }


PHASOR_PERFORM(float, float);
PHASOR_PERFORM(float, double);
PHASOR_PERFORM(double, float);
PHASOR_PERFORM(double, double);
#undef PHASOR_PERFORM
/* @} */

}

/** templated phasor class
 *
 *  - checked: no overhead for wrap_arg = 1
 *
 * */
template <typename sample_type = float,
          typename internal_type = sample_type,
          uint wrap_arg = 1>
class phasor
{
    BOOST_STATIC_ASSERT(boost::is_floating_point<sample_type>::value);
    BOOST_STATIC_ASSERT(boost::is_floating_point<internal_type>::value);

public:
    /* audio-rate controlled phasor */
    /* @{ */
    template <typename input_buffer_type, typename output_buffer_type>
    inline void perform(input_buffer_type const & in, output_buffer_type & out, uint n)
    {
        const sample_type * in_sample = get_samples(in);
        sample_type * out_sample = get_samples(out);

        internal_type phase = phase_;
        do
        {
            internal_type phase_inc = internal_type(*in_sample) * freq_factor;

            *out_sample = compute_sample(phase, phase_inc);

            ++out_sample;
            ++in_sample;
            --n;
        }
        while (n);
        phase_ = phase;
    }

    template <typename input_buffer_type, typename output_buffer_type>
    inline void perform_8(input_buffer_type const & in, output_buffer_type & out, uint n)
    {
        const sample_type * in_sample = get_samples(in);
        sample_type * out_sample = get_samples(out);

        internal_type phase = phase_;
        n /= 8;
        do
        {
            detail::phasor_perform<8>(in_sample, out_sample, phase, freq_factor);

            out_sample += 8;
            in_sample += 8;
            --n;
        }
        while (n);
        phase_ = phase;
    }

    template <uint n, typename input_buffer_type, typename output_buffer_type>
    inline void perform(input_buffer_type const & in, output_buffer_type & out)
    {
        const sample_type * in_sample = get_samples(in);
        sample_type * out_sample = get_samples(out);

        internal_type phase = phase_;
        detail::phasor_perform<n>(in_sample, out_sample, phase, freq_factor, wrap());
        phase_ = phase;
    }
    /* @} */

public:
    /* control-rate controlled phasor */
    /* @{ */
    template <typename output_buffer_type>
    inline void perform(output_buffer_type & out, uint n)
    {
        sample_type * out_sample = get_samples(out);

        internal_type phase = phase_;
        do
        {
            *out_sample = compute_sample(phase, phase_inc_);

            ++out_sample;
            --n;
        }
        while (n);
        phase_ = phase;
    }

    template <typename output_buffer_type>
    inline void perform_8(output_buffer_type & out, uint n)
    {
        sample_type * out_sample = get_samples(out);

        internal_type phase = phase_;
        n /= 8;

        do
        {
            detail::phasor_perform<8>(out_sample, phase, phase_inc_, wrap());

            out_sample += 8;
            --n;
        }
        while (n);
        phase_ = phase;
    }

    template <uint n, typename output_buffer_type>
    inline void perform(output_buffer_type & out)
    {
        sample_type * out_sample = get_samples(out);

        internal_type phase = phase_;
        detail::phasor_perform<n>(out_sample, phase, phase_inc_, wrap());

        phase_ = phase;
    }
    /* @} */

protected:
    inline static internal_type wrap(void)
    {
        return internal_type(wrap_arg);
    }

public:
    /* void set frequency for control-driven operation */
    void set_frequency(internal_type const & new_frequency)
    {
        phase_inc_ = new_frequency * wrap();
    }

    /* void set phase */
    void set_phase(internal_type const & new_phase)
    {
        phase_ = new_phase;
    }

    /* set inverse samplerate for signal-driven operation */
    void set_inv_samplerate(internal_type const & inv_samplerate)
    {
        freq_factor = inv_samplerate * wrap();
    }

protected:
    /* phasor step */
    static inline sample_type compute_sample(internal_type & phase, const internal_type phase_inc)
    {
        return detail::phasor_compute_sample<sample_type, internal_type>(phase, phase_inc, wrap());
    }

    internal_type phase_;
    internal_type phase_inc_;

    internal_type freq_factor;
};


} /* namespace */

#endif /* PHASOR_HPP */
