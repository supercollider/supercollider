//  chaotic generators
//
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

#ifndef CHAOS_CHAOS_HPP
#define CHAOS_CHAOS_HPP

#include <cassert>
#include <boost/array.hpp>
#include <boost/static_assert.hpp>
#include <boost/type_traits.hpp>
#include <boost/math/constants/constants.hpp>
#include "../../utilities/branch_hints.hpp"
#include "../../utilities/force_inline.hpp"
#include "../../utilities/template_functions.hpp"

#include "../sample_extractor.hpp"
#include "../noise_utils.hpp"

#include <boost/mpl/if.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/at.hpp>


namespace nova
{

namespace chaos
{

template <typename sample_type_arg,
          unsigned int dimension,
          unsigned int parameters,
          unsigned int output_dimension_arg>
class chaos_parameters_copy;

template <typename internal_type,
          unsigned int dimension_arg,
          unsigned int parameter_arg,
          unsigned int output_dimension_arg = dimension_arg>
class chaos_generator_base
{
protected:
    static const unsigned int dimension = dimension_arg;
    static const unsigned int parameters = parameter_arg;
    static const unsigned int output_dimension = output_dimension_arg;

    typedef chaos_parameters_copy<internal_type, dimension, parameters, output_dimension_arg> state_copy;

    chaos_generator_base(internal_type const & init = internal_type(0))
    {
        data.assign(init);
    }

public:
    void set(unsigned int dim, internal_type const & value)
    {
        data[dim] = value;
    }

    void set_parameter(unsigned int par, internal_type const & value)
    {
        params[par] = value;
    }

    typedef boost::array<internal_type, dimension> dimension_array;

protected:
    void fix_and_set(unsigned int dim, internal_type const & value)
    {
        data[dim] = undenormalize(value);
    }

    void clip_and_set(unsigned int dim, internal_type value,
                      internal_type const & low, internal_type const &high)
    {
        if (value < low)
            value = low;
        else if (value > high)
            value = high;

        set(dim, value);
    }

private:
    template <typename, unsigned int, unsigned int, unsigned int>
    friend class chaos_parameters_copy;

    dimension_array data;
    boost::array<internal_type, parameters> params;
};


template <typename internal_type,
          unsigned int dimension_arg,
          unsigned int output_dimension_arg>
class chaos_generator_base<internal_type, dimension_arg, 0, output_dimension_arg>
{
protected:
    static const unsigned int dimension = dimension_arg;
    static const unsigned int parameters = 0;
    static const unsigned int output_dimension = output_dimension_arg;

    typedef chaos_parameters_copy<internal_type, dimension, parameters, output_dimension_arg> state_copy;

    chaos_generator_base(internal_type const & init = internal_type(0))
    {
        data.assign(init);
    }

public:
    void set(unsigned int dim, internal_type const & value)
    {
        data[dim] = value;
    }

    typedef boost::array<internal_type, dimension> dimension_array;

protected:
    void fix_and_set(unsigned int dim, internal_type const & value)
    {
        data[dim] = undenormalize(value);
    }

    void clip_and_set(unsigned int dim, internal_type value,
                      internal_type const & low, internal_type const &high)
    {
        if (value < low)
            value = low;
        else if (value > high)
            value = high;

        set(dim, value);
    }

private:
    template <typename, unsigned int, unsigned int, unsigned int>
    friend class chaos_parameters_copy;

    dimension_array data;
};

template <typename internal_type_arg,
          unsigned int dimension_arg,
          unsigned int parameter_arg,
          unsigned int output_dimension_arg>
class chaos_parameters_copy
{
public:
    typedef internal_type_arg internal_type;

    static const unsigned int dimension = dimension_arg;
    static const unsigned int parameters = parameter_arg;
    static const unsigned int output_dimension = output_dimension_arg;
    typedef chaos_generator_base<internal_type, dimension, parameters, output_dimension_arg> generator;

    typedef typename generator::dimension_array dimension_array;

public:
    /** \brief copy system state & parameters  */
    chaos_parameters_copy(generator & gen):
        gen(gen)
    {
        data = gen.data;
        params = gen.params;
    }

    /** \brief copy system state back to generator  */
    ~chaos_parameters_copy(void)
    {
        gen.data = data;
    }

    internal_type get(unsigned int dim) const
    {
        return data[dim];
    }

    internal_type get_parameter(unsigned int dim) const
    {
        return params[dim];
    }

    void set(unsigned int dim, internal_type val)
    {
        data[dim] = val;
    }

    dimension_array const & get_data(void) const
    {
        return data;
    }

    void clip_and_set(unsigned int dim, internal_type value,
                      internal_type const & low, internal_type const &high)
    {
        if (value < low)
            value = low;
        else if (value > high)
            value = high;

        set(dim, value);
    }

private:
    dimension_array data;
    boost::array<internal_type, parameters> params;
    generator & gen;
};

template <typename internal_type_arg,
          unsigned int dimension_arg,
          unsigned int output_dimension_arg>
class chaos_parameters_copy<internal_type_arg, dimension_arg, 0, output_dimension_arg>
{
public:
    typedef internal_type_arg internal_type;

    static const unsigned int dimension = dimension_arg;
    static const unsigned int parameters = 0;
    static const unsigned int output_dimension = output_dimension_arg;
    typedef chaos_generator_base<internal_type, dimension, parameters, output_dimension_arg> generator;

    typedef typename generator::dimension_array dimension_array;

public:
    /** \brief copy system state & parameters  */
    chaos_parameters_copy(generator & gen):
        gen(gen)
    {
        data = gen.data;
    }

    /** \brief copy system state back to generator  */
    ~chaos_parameters_copy(void)
    {
        gen.data = data;
    }

    internal_type get(unsigned int dim) const
    {
        return data[dim];
    }

    void set(unsigned int dim, internal_type val)
    {
        data[dim] = val;
    }

    dimension_array const & get_data(void) const
    {
        return data;
    }

    void clip_and_set(unsigned int dim, internal_type value,
                      internal_type const & low, internal_type const &high)
    {
        if (value < low)
            value = low;
        else if (value > high)
            value = high;

        set(dim, value);
    }

private:
    dimension_array data;
    generator & gen;
};

namespace detail
{
template <typename sample_type,
          typename system_state_type,
          void (*compute_sample)(system_state_type &)>
class nop_interpolator
{
    static const unsigned int dimension = system_state_type::output_dimension;

    typedef boost::array<sample_type*, dimension> output_array;
    typedef typename system_state_type::internal_type internal_type;
    typedef std::size_t size_t;

public:
    static always_inline void perform(system_state_type & state, output_array & out, unsigned int n)
    {
        perform(state, out.begin(), n);
    }

    static always_inline void perform(system_state_type & state, sample_type * const * out, unsigned int n)
    {
        for (size_t sample = 0; sample != n; ++sample)
        {
            (*compute_sample)(state);

            for (size_t channel = 0; channel != dimension; ++channel)
                out[channel][sample] = state.get(channel);
        }
    }

    /* small api wrapper for one-dimensional systems */
    static always_inline void perform(system_state_type & state, sample_type * out, unsigned int n)
    {
        BOOST_STATIC_ASSERT(dimension == 1);

        for (size_t sample = 0; sample != n; ++sample)
        {
            (*compute_sample)(state);
            out[sample] = state.get(0);
        }
    }

    /* dummy to match api */
    template <typename float_type>
    void set_frequency(float_type fn)
    {
        assert(false);
    }
};

template <typename sample_type,
          typename system_state_type,
          void (*compute_sample)(system_state_type &),
          typename phase_count_type>
class sample_hold_interpolator
{
    static const unsigned int dimension = system_state_type::output_dimension;

    typedef boost::array<sample_type*, dimension> output_array;
    typedef boost::array<sample_type, dimension>  sample_array;
    typedef typename system_state_type::internal_type internal_type;
    typedef std::size_t size_t;

public:
    sample_hold_interpolator(phase_count_type const & fn = phase_count_type(1)):
        remaining_(0)
    {
        set_frequency(fn);
        current_.assign(0);
    }

    always_inline void perform(system_state_type & state, output_array const & out, unsigned int n)
    {
        perform (state, out.begin(), n);
    }

    always_inline void perform(system_state_type & state, sample_type * const * out, unsigned int n)
    {
        register sample_array current = current_;
        register phase_count_type remaining = remaining_;

        unsigned int elapsed = 0;
        do
        {
            if (remaining <= 0)
            {
                do
                {
                    (*compute_sample)(state);
                    remaining += interpolation_steps;
                }
                while(unlikely(remaining <= 0));

                for (unsigned int i = 0; i != dimension; ++i)
                    current[i] = state.get(i);
            }

            unsigned int to_go = std::min(n, uint(std::ceil(remaining)));

            assert (to_go <= n and to_go >= 1);

            for (unsigned int channel = 0; channel != dimension; ++channel)
                for (unsigned int sample = 0; sample != to_go; ++sample)
                    out[channel][elapsed + sample] = current[channel];

            remaining -= phase_count_type(to_go);
            n -= to_go;
            elapsed += to_go;
        }
        while (n);

        current_ = current;
        remaining_ = remaining;
    }

    /* small api wrapper for one-dimensional systems, the compiler should be able to optimize this */
    void perform(system_state_type & state, sample_type * out, unsigned int n)
    {
        BOOST_STATIC_ASSERT(dimension == 1);

        register boost::array<sample_type*, 1> out_array;
        out_array[0] = out;
        perform(state, out_array, n);
    }

    /** set frequency from normalized frequency */
    template <typename float_type>
    void set_frequency(float_type fn)
    {
        if (fn > 0)
            interpolation_steps = phase_count_type(1) / phase_count_type(fn);
    }

protected:
    sample_array current_;                /**< current samples */
    phase_count_type remaining_;          /**< remaining samples */
    phase_count_type interpolation_steps; /**< interpolation steps */
};

template <typename sample_type,
          typename system_state_type,
          void (*compute_sample)(system_state_type &),
          typename phase_count_type>
class linear_interpolator:
    public sample_hold_interpolator<sample_type, system_state_type, compute_sample, phase_count_type>
{
    typedef sample_hold_interpolator<sample_type, system_state_type, compute_sample, phase_count_type> sh_base;

    static const unsigned int dimension = system_state_type::output_dimension;

    typedef boost::array<sample_type*, dimension> output_array;
    typedef boost::array<sample_type, dimension>  sample_array;
    typedef typename system_state_type::internal_type internal_type;
    typedef std::size_t size_t;

public:
    linear_interpolator(phase_count_type const & fn = phase_count_type(0.5)):
        sh_base(fn)
    {
        slope_.assign(0);
    }

    always_inline void perform(system_state_type & state, output_array const & out, unsigned int n)
    {
        perform (state, out.begin(), n);
    }

    always_inline void perform(system_state_type & state, sample_type * const* out, unsigned int n)
    {
        register sample_array current = sh_base::current_;
        register phase_count_type remaining = sh_base::remaining_;

        register sample_array slope = slope_;

        using namespace std;

        unsigned int elapsed = 0;
        do
        {
            if (remaining <= 0)
            {
                do
                {
                    (*compute_sample)(state);
                    remaining += sh_base::interpolation_steps;
                }
                while(unlikely(remaining <= 0));

                for (size_t channel = 0; channel != dimension; ++channel)
                {
                    sample_type diff = state.get(channel) - current[channel];
                    slope[channel] = diff / sample_type(sh_base::interpolation_steps);
                }

            }

            unsigned int to_go = min(n, uint(ceil(remaining)));

            assert (to_go <= n and to_go >= 1);

            for (unsigned int channel = 0; channel != dimension; ++channel)
            {
                for (unsigned int sample = 0; sample != to_go; ++sample)
                {
                    out[channel][elapsed+sample] = current[channel];
                    current[channel] += slope[channel];
                }
            }

            elapsed += to_go;
            remaining -= phase_count_type(to_go);
            n -= to_go;
        }
        while (n);

        sh_base::current_ = current;
        sh_base::remaining_ = remaining;
        slope_ = slope;
    }

    /* small api wrapper for one-dimensional systems, the compiler should be able to optimize this */
    void perform(system_state_type & state, sample_type * out, unsigned int n)
    {
        BOOST_STATIC_ASSERT(dimension == 1);

        register boost::array<sample_type*, 1> out_array;
        out_array[0] = out;
        perform(state, out_array, n);
    }

protected:
    sample_array slope_;                  /**< slope samples */
};

template <typename sample_type,
          typename system_state_type,
          void (*compute_sample)(system_state_type &),
          typename phase_count_type>
class quadratic_interpolator:
    public linear_interpolator<sample_type, system_state_type, compute_sample, phase_count_type>
{
    typedef sample_hold_interpolator<sample_type, system_state_type, compute_sample, phase_count_type> sh_base;
    typedef linear_interpolator<sample_type, system_state_type, compute_sample, phase_count_type> linear_base;

    static const unsigned int dimension = system_state_type::output_dimension;

    typedef boost::array<sample_type*, dimension> output_array;
    typedef boost::array<sample_type, dimension>  sample_array;
    typedef typename system_state_type::internal_type internal_type;
    typedef std::size_t size_t;

public:
    quadratic_interpolator(phase_count_type const & fn = phase_count_type(0.5)):
        linear_base(fn)
    {
        curve_.assign(0);
    }

    always_inline void perform(system_state_type & state, output_array const & out, unsigned int n)
    {
        perform (state, out.begin(), n);
    }

    always_inline void perform(system_state_type & state, sample_type * const * out, unsigned int n)
    {
        register sample_array current = sh_base::current_;
        register phase_count_type remaining = sh_base::remaining_;
        register sample_array slope = linear_base::slope_;
        register sample_array curve = curve_;

        using namespace std;

        unsigned int elapsed = 0;
        do
        {
            if (remaining <= 0)
            {
                do
                {
                    (*compute_sample)(state);
                    remaining += sh_base::interpolation_steps;
                }
                while(unlikely(remaining <= 0));

                const sample_type steps = sample_type(sh_base::interpolation_steps);
                for (size_t channel = 0; channel != dimension; ++channel)
                {
                    sample_type next = state.get(channel);
                    sample_type diff = next - current[channel];

                    curve[channel] = (diff - steps * slope[channel]) * 0.5 /
                        (steps * (steps - sample_type(1)));
                }

            }

            const unsigned int to_go = min(n, uint(ceil(remaining)));

            assert (to_go <= n and to_go >= 1);

            for (unsigned int channel = 0; channel != dimension; ++channel)
            {
                for (unsigned int sample = 0; sample != to_go; ++sample)
                {
                    out[channel][elapsed+sample] = current[channel];
                    current[channel] += slope[channel];
                    slope[channel] += curve[channel];
                }
            }

            elapsed += to_go;
            remaining -= phase_count_type(to_go);
            n -= to_go;
        }
        while (n);

        sh_base::current_ = current;
        sh_base::remaining_ = remaining;
        linear_base::slope_ = slope;
        curve_ = curve;
    }

    /* small api wrapper for one-dimensional systems, the compiler should be able to optimize this */
    always_inline void perform(system_state_type & state, sample_type * out, unsigned int n)
    {
        BOOST_STATIC_ASSERT(dimension == 1);

        register boost::array<sample_type*, 1> out_array;
        out_array[0] = out;
        perform(state, out_array, n);
    }

private:
    sample_array curve_;                  /**< curve samples */
};

template <unsigned int interpolator,
          typename sample_type,
          class state_copy,
          void (*compute_sample)(state_copy &),
          typename phase_count_type = float>
struct select_interpolator
{
private:
    BOOST_STATIC_ASSERT(interpolator < 4);

    typedef typename
    boost::mpl::vector<detail::nop_interpolator<sample_type, state_copy, compute_sample>,
                       detail::sample_hold_interpolator<sample_type, state_copy, compute_sample, phase_count_type>,
                       detail::linear_interpolator<sample_type, state_copy, compute_sample, phase_count_type>,
                       detail::quadratic_interpolator<sample_type, state_copy, compute_sample, phase_count_type>
                       > interpolator_sequence;

public:
    typedef typename boost::mpl::at_c<interpolator_sequence, interpolator>::type type;
};

} /* namespace detail */

template <unsigned int interpolator,
          typename system,
          typename phase_count_type = float>
class select_interpolator
{
    typedef typename system::sample_type sample_type;
    typedef typename system::state_copy state_copy;

public:
    typedef typename detail::select_interpolator<interpolator, sample_type,
                                                 state_copy, &system::compute_sample,
                                                 phase_count_type
                                                 >::type type;
};


} /* namespace chaos */
} /* namespace nova */

/* macro to ease the definition of the basic class members */
#define NOVA_CHAOS_GENERATOR_HEADER(base_type, dims, pars)              \
    NOVA_CHAOS_GENERATOR_HEADER_OUTDIMS(base_type, dims, pars, dims)


#define NOVA_CHAOS_GENERATOR_HEADER_OUTDIMS(base_type, dims, pars, out_dims) \
    typedef base_type<sample_type_arg, internal_type> this_type;        \
    typedef chaos_generator_base<internal_type, dims, pars, out_dims> generator_base; \
                                                                        \
public:                                                                 \
    static const unsigned int dimension = generator_base::dimension;    \
    static const unsigned int output_dimension = generator_base::output_dimension; \
                                                                        \
    typedef sample_type_arg sample_type;                                \
    typedef typename generator_base::state_copy state_copy;             \
                                                                        \
    /* @{ */                                                            \
    /** generate at sampling rate */                                    \
    template <typename output_buffer_type>                              \
    always_inline void perform(output_buffer_type & out, unsigned int n) \
    {                                                                   \
        assert(output_dimension == 1);                                  \
        perform(get_samples(out), n);                                   \
    }                                                                   \
                                                                        \
    always_inline void perform(sample_type * out_sample, unsigned int n) \
    {                                                                   \
        BOOST_STATIC_ASSERT(output_dimension == 1);                     \
        register state_copy local (*this); /* copy state to registers */ \
                                                                        \
        typedef typename select_interpolator<0, this_type>::type interpolator; \
                                                                        \
        interpolator::perform(local, out_sample, n);                    \
    }                                                                   \
                                                                        \
    always_inline void perform(sample_type ** out_sample, unsigned int n) \
    {                                                                   \
        register state_copy local (*this); /* copy state to registers */ \
                                                                        \
        typedef typename select_interpolator<0, this_type>::type interpolator; \
                                                                        \
        interpolator::perform(local, out_sample, n);                    \
    }                                                                   \
                                                                        \
    /* @} */                                                            \
                                                                        \
    /* @{ */                                                            \
    /** generate via interpolator */                                    \
    template <typename interpolator_type, typename output_buffer_type>  \
    always_inline void perform(interpolator_type & interpolator, output_buffer_type & out, unsigned int n) \
    {                                                                   \
        assert(output_dimension == 1);                                  \
        perform(get_samples(out), n);                                   \
    }                                                                   \
                                                                        \
    template <typename interpolator_type>                               \
    always_inline void perform(interpolator_type & interpolator, sample_type * out_sample, unsigned int n) \
    {                                                                   \
        /* BOOST_STATIC_ASSERT(output_dimension == 1); */               \
        assert(output_dimension == 1);                                  \
        register state_copy local (*this); /* copy state to registers */ \
                                                                        \
        interpolator.perform(local, out_sample, n);                     \
    }                                                                   \
                                                                        \
    template <typename interpolator_type>                               \
    always_inline void perform(interpolator_type & interpolator, sample_type ** out_sample, unsigned int n) \
    {                                                                   \
        register state_copy local (*this); /* copy state to registers */ \
                                                                        \
        interpolator.perform(local, out_sample, n);                     \
    }                                                                   \
    /* @} */

#define NOVA_CHAOS_PARAMETER_SETTER_SIMPLE(INDEX, NAME) \
    void set_##NAME(internal_type const & NAME)         \
    {                                                   \
        generator_base::set_parameter(INDEX, NAME);     \
    }

#define NOVA_CHAOS_PARAMETER_SETTER_CLIPPING(INDEX, NAME, LOW, HIGH)    \
    void set_##NAME(internal_type const & NAME)                         \
    {                                                                   \
        static const internal_type clip_low = LOW;                      \
        static const internal_type clip_high = HIGH;                    \
        generator_base::set_parameter(INDEX, clip<internal_type>(NAME, clip_low, clip_high)); \
    }


#endif /* CHAOS_CHAOS_HPP */
