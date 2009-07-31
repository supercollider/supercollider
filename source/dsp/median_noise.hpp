//
//
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

#ifndef DSP_MEDIAN_NOISE_HPP
#define DSP_MEDIAN_NOISE_HPP

#include "noise.hpp"
#include "median_filter.hpp"

/* define perform wrapper for onedimensional noise generator */
#define DEFINE_PERFORM_WRAPPER_NOISE                                    \
    template <typename output_buffer_type, typename sample_count_t>     \
    inline void perform(output_buffer_type & out, sample_count_t n, noise_engine & engine_) \
    {                                                                   \
        ::nova::detail::muladd_helper_nop<sample_type> ma;              \
        perform(out, n, engine_, ma);                                   \
    }


namespace nova
{

namespace detail
{

template <typename sample_type,
          typename noise_engine,
          typename noise_generator,
          int interpolation_order,
          unsigned int max_median = 255>
class lf_median_noise_external_engine:
    public interpolation_selector<sample_type, interpolation_order>::type
{
    typedef typename interpolation_selector<sample_type, interpolation_order>::type base_class;


    typedef nova::median_filter<sample_type, max_median> median_filter_t;

    struct generate
    {
        generate(noise_engine & engine, median_filter_t & median_filter):
            engine(engine), median_filter(median_filter)
        {}

        sample_type operator()(void)
        {
            noise_generator gen(engine);
            sample_type noise = gen();
            sample_type ret = median_filter.step(noise);
            engine = gen.engine();
            return ret;
        }

        noise_engine & engine;
        median_filter_t & median_filter;
    };

    median_filter_t median_filter;

public:
    lf_median_noise_external_engine(void):
        median_filter(1)
    {}

    void set_median(std::size_t median)
    {
        median_filter.resize(median);
    }

    template <typename output_buffer_type, typename sample_count_t, typename muladd_helper>
    inline void perform(output_buffer_type & out, sample_count_t n, noise_engine & engine_, muladd_helper & ma)
    {
        generate gen(engine_, median_filter);

        base_class::perform(gen, out, n, ma);

        engine_ = gen.engine;
    }

    DEFINE_PERFORM_WRAPPER_NOISE

    void tick(sample_type & out, noise_engine & engine_)
    {
        sample_type * out_sample = &out;
        perform(out_sample, 1, engine_);
    }
};

template <typename sample_type,
          typename noise_engine,
          typename noise_generator,
          int interpolation_order,
          unsigned int max_median = 255>
class lf_median_noise:
    private lf_median_noise_external_engine<sample_type, noise_engine, noise_generator, interpolation_order,
                                            max_median>
{
    typedef typename
    nova::detail::lf_median_noise_external_engine<sample_type, noise_engine, noise_generator, interpolation_order,
                                                  max_median> base_class;

    noise_engine engine;

public:
    template <typename output_buffer_type, typename sample_count_t, typename muladd_helper>
    inline void perform(output_buffer_type & out, sample_count_t n, muladd_helper & ma)
    {
        base_class::perform(out, n, engine, ma);
    }

    DEFINE_PERFORM_WRAPPER_0_1

    void tick(sample_type & out)
    {
        base_class::tick(out, engine);
    }

    void set_median(std::size_t median)
    {
        base_class::set_median(median);
    }

    void set_frequency(sample_type freq, sample_type sr)
    {
        base_class::set_frequency(freq, sr);
    }

    void set_frequency(sample_type fn)
    {
        base_class::set_frequency(fn);
    }
};

/** base classes for median-filtered white noise classes */
/* @{ */
template <typename sample_type, int interpolation_order, typename noise_engine>
class lf_median_white_noise_external_engine:
    public lf_median_noise_external_engine<sample_type, noise_engine,
                                           white_noise_generator<sample_type, noise_engine>,
                                           interpolation_order>
{};

template <typename sample_type, int interpolation_order>
class lf_median_white_noise:
    public lf_median_noise<sample_type, cheap_noise_engine,
                           white_noise_generator<sample_type, cheap_noise_engine>,
                           interpolation_order>
{};
/* @} */

} /* namespace detail */

template <typename sample_type, typename noise_engine>
class median_white_noise_external_engine:
    public detail::lf_median_white_noise_external_engine <sample_type, 0, noise_engine>
{};

template <typename sample_type>
class median_white_noise:
    public detail::lf_median_white_noise<sample_type, 0>
{};

template <typename sample_type, typename noise_engine>
class lf0_median_white_noise_external_engine:
    public detail::lf_median_white_noise_external_engine <sample_type, 1, noise_engine>
{};

template <typename sample_type>
class lf0_median_white_noise:
    public detail::lf_median_white_noise<sample_type, 1>
{};

template <typename sample_type, typename noise_engine>
class lf1_median_white_noise_external_engine:
    public detail::lf_median_white_noise_external_engine <sample_type, 2, noise_engine>
{};

template <typename sample_type>
class lf1_median_white_noise:
    public detail::lf_median_white_noise<sample_type, 2>
{};

template <typename sample_type, typename noise_engine>
class lf2_median_white_noise_external_engine:
    public detail::lf_median_white_noise_external_engine <sample_type, 3, noise_engine>
{};

template <typename sample_type>
class lf2_median_white_noise:
    public detail::lf_median_white_noise<sample_type, 3>
{};

template <typename sample_type, typename noise_engine>
class lf2a_median_white_noise_external_engine:
    public detail::lf_median_white_noise_external_engine <sample_type, 4, noise_engine>
{};

template <typename sample_type>
class lf2a_median_white_noise:
    public detail::lf_median_white_noise<sample_type, 4>
{};


} /* namespace nova */

#undef DEFINE_PERFORM_WRAPPER_NOISE

#endif /* DSP_MEDIAN_NOISE_HPP */
