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

#ifndef DSP_NOISE_HPP
#define DSP_NOISE_HPP

#include <boost/mpl/vector.hpp>
#include <boost/mpl/at.hpp>

#include "../utilities/branch_hints.hpp"

#include "noise_utils.hpp"
#include "noise_engines.hpp"
#include "sample_extractor.hpp"
#include "muladd_helpers.hpp"


namespace nova
{

namespace detail
{

/** interpolator for sample-rate noise
 *
 *  generator is required to return sample_type, when being called
 *
 * */
template <typename sample_type>
class sr_interpolator
{
protected:
    template <typename generator, typename output_buffer_type, typename sample_count_t, typename muladd_helper>
    inline void perform(generator & gen, output_buffer_type & out, sample_count_t n, muladd_helper & ma)
    {
        sample_type * out_sample = nova::get_samples(out);

        for (sample_count_t i = 0; i != n; ++i)
            out_sample[i] = ma(gen());
    }
};

/** interpolator for sample&hold interpolated noise
 *
 *  generator is required to return sample_type, when being called
 *
 * */
template <typename sample_type,
          typename phase_count_type = float>
class sh_interpolator
{
public:
    void set_frequency(sample_type freq, sample_type sr)
    {
        set_frequency(sr / freq);
    }

    void set_frequency(sample_type fn)
    {
        interpolation_steps = fn;
    }

protected:
    sh_interpolator(void):
        current_(0), remaining_(0), interpolation_steps(1)
    {}

    template <typename generator, typename output_buffer_type, typename sample_count_t, typename muladd_helper>
    inline void perform(generator & gen, output_buffer_type & out, sample_count_t n, muladd_helper & ma)
    {
        sample_type * out_sample = get_samples(out);

        assert(n);

        sample_type current = current_;
        phase_count_type remaining = remaining_;

        do
        {
            if (remaining <= 0)
            {
                current = gen();
                remaining += interpolation_steps;
                remaining = std::max(remaining, phase_count_type(1));
            }

            sample_count_t to_go = std::min(n, sample_count_t(std::ceil(remaining)));

            assert (to_go <= n and to_go >= 1);

            for (sample_count_t i = 0; i != to_go; ++i)
                out_sample[i] = ma(current);

            remaining -= phase_count_type(to_go);
            n -= to_go;
            out_sample += to_go;
        }
        while (n);

        current_ = current;
        remaining_ = remaining;
    }

private:
    sample_type current_;
    phase_count_type remaining_;
    phase_count_type interpolation_steps;
};


template <typename sample_type,
          typename phase_count_type = float>
class linear_interpolator
{
protected:
    linear_interpolator(void):
        current_(0), remaining_(0), slope_(0), interpolation_steps(1), inv_interpolation_steps(-1)
    {}

public:
    void set_frequency(sample_type freq, sample_type sr)
    {
        interpolation_steps = sr / freq;
        inv_interpolation_steps = freq / sr;
    }

    void set_frequency(sample_type freq)
    {
        interpolation_steps = sample_type(1) / freq;
        inv_interpolation_steps = freq;
    }

    /* set normalized and inverse normalized frequency, avoiding division */
    void set_frequency_advanced(sample_type fn, sample_type inv_fn)
    {
        interpolation_steps = fn;
        inv_interpolation_steps = inv_fn;
    }


protected:
    template <typename generator, typename output_buffer_type, typename sample_count_t, typename muladd_helper>
    inline void perform(generator & gen, output_buffer_type & out, sample_count_t n, muladd_helper & ma)
    {
        sample_type * out_sample = get_samples(out);

        assert(n);

        sample_type slope = slope_;

        sample_type current = current_;
        phase_count_type remaining = remaining_;

        do
        {
            if (remaining <= 0)
            {
                sample_type next = gen();
                remaining += interpolation_steps;
                remaining = std::max(remaining, phase_count_type(1));

                slope = (next - current) * inv_interpolation_steps;
            }

            sample_count_t to_go = std::min(n, sample_count_t(std::ceil(remaining)));

            assert (to_go <= n and to_go >= 1);

            for (sample_count_t i = 0; i != to_go; ++i)
            {
                out_sample[i] = ma(current);
                current += slope;
            }

            remaining -= phase_count_type(to_go);
            n -= to_go;
            out_sample += to_go;
        }
        while (n);

        current_ = current;
        remaining_ = remaining;
        slope_ = slope;
    }

    sample_type current_;
    phase_count_type remaining_;
    sample_type slope_;
    phase_count_type interpolation_steps, inv_interpolation_steps;
};

template <typename sample_type,
          typename phase_count_type = float>
class quadratic_interpolator:
    public linear_interpolator<sample_type, phase_count_type>
{
    typedef linear_interpolator<sample_type, phase_count_type> base;
protected:
    quadratic_interpolator(void):
        curve_(0), next_value(0)
    {}

    template <typename generator, typename output_buffer_type, typename sample_count_t, typename muladd_helper>
    inline void perform(generator & gen, output_buffer_type & out, sample_count_t n, muladd_helper & ma)
    {
        sample_type * out_sample = get_samples(out);

        assert(n);

        sample_type slope = base::slope_;
        sample_type current = base::current_;
        phase_count_type remaining = base::remaining_;

        sample_type curve = curve_;

        do
        {
            if (remaining <= 0)
            {
                remaining += base::interpolation_steps;
                remaining = std::max(remaining, phase_count_type(2));
                sample_type steps = sample_type(base::interpolation_steps);

                /* supercollider-style curve computation,
                 * adapted for reduced frequency quantization */
                sample_type value = next_value;
                next_value = gen();
                sample_type next_midpoint = (next_value + value) * sample_type(0.5);

                curve = sample_type(2) * (next_midpoint - current - steps * slope) / (steps * steps + steps);
            }

            sample_count_t to_go = std::min(n, sample_count_t(std::ceil(remaining)));

            assert (to_go <= n and to_go >= 1);

            for (sample_count_t i = 0; i != to_go; ++i)
            {
                out_sample[i] = ma(current);
                slope += curve;
                current += slope;
            }

            remaining -= phase_count_type(to_go);
            n -= to_go;
            out_sample += to_go;
        }
        while (n);

        base::current_ = current;
        base::remaining_ = remaining;
        base::slope_ = slope;
        curve_ = curve;
    }

private:
    phase_count_type curve_;
    phase_count_type next_value;
};

template <typename sample_type,
          typename phase_count_type = float>
class quadratic_interpolator2:
    public linear_interpolator<sample_type, phase_count_type>
{
    typedef linear_interpolator<sample_type, phase_count_type> base;

protected:
    quadratic_interpolator2(void):
        curve_(0), next_value(0), next_midpoint(0)
    {}

    template <typename generator, typename output_buffer_type, typename sample_count_t, typename muladd_helper>
    inline void perform(generator & gen, output_buffer_type & out, sample_count_t n, muladd_helper & ma)
    {
        sample_type * out_sample = get_samples(out);

        assert(n);

        sample_type slope = base::slope_;
        sample_type current = base::current_;
        phase_count_type remaining = base::remaining_;

        sample_type curve = curve_;

        do
        {
            if (remaining <= 0)
            {
                remaining += base::interpolation_steps;
                remaining = std::max(remaining, phase_count_type(2));
                sample_type steps = sample_type(base::interpolation_steps);

                /* my own curve computation */
                sample_type next = gen();
                sample_type diff = next - current;
                curve = (diff - steps * slope) * 0.5 /
                    (steps * steps - steps);
            }

            sample_count_t to_go = std::min(n, sample_count_t(std::ceil(remaining)));

            assert (to_go <= n and to_go >= 1);

            for (sample_count_t i = 0; i != to_go; ++i)
            {
                out_sample[i] = ma(current);
                slope += curve;
                current += slope;
            }

            remaining -= phase_count_type(to_go);
            n -= to_go;
            out_sample += to_go;
        }
        while (n);

        base::current_ = current;
        base::remaining_ = remaining;
        base::slope_ = slope;
        curve_ = curve;
    }

private:
    phase_count_type curve_;
    phase_count_type next_value, next_midpoint;
};


} /* namespace detail */


/* define perform wrapper for onedimensional noise generator */
#define DEFINE_PERFORM_WRAPPER_NOISE                                    \
    template <typename output_buffer_type, typename sample_count_t>     \
    inline void perform(output_buffer_type & out, sample_count_t n, noise_engine & engine_) \
    {                                                                   \
        ::nova::detail::muladd_helper_nop<sample_type> ma;              \
        perform(out, n, engine_, ma);                                   \
    }




/** grey noise generator with external rng */
template <typename sample_type, typename noise_engine>
class grey_noise_external_engine:
    private detail::sr_interpolator<sample_type>
{
    BOOST_STATIC_ASSERT((boost::is_same<typename noise_engine::result_type, boost::uint32_t>::value));

    typedef boost::int32_t int32_t;

    struct generate
    {
        generate(noise_engine const & engine, int32_t const & counter):
            engine(engine), counter(counter)
        {}

        /* algorithm adapted from supercollider sources */
        sample_type operator()(void)
        {
            counter ^= 1L << (engine() & 31);
            return counter * sample_type(4.6566128730773926e-10);
        }

        noise_engine engine;
        int32_t counter;
    };

    int32_t counter;

public:
    grey_noise_external_engine(void):
        counter(0)
    {}

    template <typename output_buffer_type, typename sample_count_t, typename muladd_helper>
    inline void perform(output_buffer_type & out, sample_count_t n, noise_engine & engine_, muladd_helper & ma)
    {
        generate gen(engine_, counter);

        detail::sr_interpolator<sample_type>::perform(gen, out, n, ma);

        engine_ = gen.engine;
        counter = gen.counter;
    }

    DEFINE_PERFORM_WRAPPER_NOISE

    void tick(sample_type & out, noise_engine & engine_)
    {
        out = detail::clipped_noise_scale<sample_type>(engine_());
    }
};


/** grey noise generator with internal rng */
template <typename sample_type>
class grey_noise:
    private grey_noise_external_engine<sample_type, detail::taus88_engine>
{
    typedef grey_noise_external_engine<sample_type, detail::taus88_engine> base_class;

public:
    grey_noise(void):
        engine_(detail::thread_local_noise())
    {}

    template <typename output_buffer_type, typename sample_count_t, typename muladd_helper>
    inline void perform(output_buffer_type & out, sample_count_t n, muladd_helper & ma)
    {
        base_class::perform(out, n, engine_, ma);
    }

    DEFINE_PERFORM_WRAPPER_0_1

    void tick(sample_type & out)
    {
        base_class::tick(out, engine_);
    }

private:
    detail::taus88_engine engine_;
};

/** brown noise generator with external rng */
template <typename sample_type, typename noise_engine>
class brown_noise_external_engine:
    private detail::sr_interpolator<sample_type>
{
    BOOST_STATIC_ASSERT((boost::is_same<typename noise_engine::result_type, boost::uint32_t>::value));

    struct generate
    {
        generate(noise_engine const & engine, sample_type const & state):
            engine(engine), state(state)
        {}

        /* algorithm adapted from supercollider sources */
        sample_type operator()(void)
        {
            state += detail::noise_scale_8<sample_type>(engine());
            if (state > sample_type(1))
                state = sample_type(2) - state;
            else if (state < sample_type(-1))
                state = sample_type(-2.f) - state;
            return state;
        }

        noise_engine engine;
        sample_type state;
    };

    sample_type state;

public:
    brown_noise_external_engine(void):
        state(0)
    {}

    template <typename output_buffer_type, typename sample_count_t, typename muladd_helper>
    inline void perform(output_buffer_type & out, sample_count_t n, noise_engine & engine_, muladd_helper & ma)
    {
        generate gen(engine_, state);

        detail::sr_interpolator<sample_type>::perform(gen, out, n, ma);

        engine_ = gen.engine;
        state = gen.state;
    }

    DEFINE_PERFORM_WRAPPER_NOISE

    void tick(sample_type & out, noise_engine & engine_)
    {
        generate gen(engine_, state);
        out = gen();
        engine_ = gen.engine;
        state = gen.state;
    }
};


/** brown noise generator with internal rng */
template <typename sample_type>
class brown_noise:
    private brown_noise_external_engine<sample_type, detail::cheap_noise_engine>
{
    typedef brown_noise_external_engine<sample_type, detail::cheap_noise_engine> base_class;

public:
    brown_noise(void):
        engine_(detail::thread_local_noise())
    {}

    template <typename output_buffer_type, typename sample_count_t, typename muladd_helper>
    inline void perform(output_buffer_type & out, sample_count_t n, muladd_helper & ma)
    {
        base_class::perform(out, n, engine_, ma);
    }

    DEFINE_PERFORM_WRAPPER_0_1

    void tick(sample_type & out)
    {
        base_class::tick(out, engine_);
    }

private:
    detail::cheap_noise_engine engine_;
};


namespace detail
{

/** larry trammell's stochastic voss-mccartney algorithm (old version)
 *
 * 5 stage implementation:
 * a = [ 0.00224  0.00821  0.00798  0.00694  0.00992 ]
 * p = [ 0.15571  0.30194  0.74115  0.93003  0.98035 ]
 *
 *
 *  7 octaves, 0.15dB error
 * */
template <typename sample_type, typename noise_engine>
struct pink_stochastic_voss_mccartney_old
{
    typedef boost::array<sample_type, 5> state_type;

    pink_stochastic_voss_mccartney_old(noise_engine const & engine, state_type const & state):
        engine(engine), state(state)
    {}

    sample_type operator()(void)
    {
        boost::uint32_t rv = engine();

        if (rv > 668769358u)
            state[0] = white_noise_scale<sample_type>(engine()) * 0.00224;

        if (rv > 1296822425u)
            state[1] = white_noise_scale<sample_type>(engine()) * 0.00821;

        if (rv > 3183215011u)
            state[2] = white_noise_scale<sample_type>(engine()) * 0.00798;

        if (rv > 3994448434u)
            state[3] = white_noise_scale<sample_type>(engine()) * 0.00694;

        if (rv > 4210571188u)
            state[4] = white_noise_scale<sample_type>(engine()) * 0.00992;

        return (state[0] + state[1] + state[2] + state[3] + state[4]) * (19.2169062);
    }

    noise_engine engine;
    state_type state;
};

/** larry trammell's stochastic voss-mccartney algorithm
 *
 * pA =   [  3.8024    2.9694    2.5970    3.0870    3.4006  ]
 * pP =   [  0.00198   0.01280   0.04900   0.17000   0.68200 ]
 * pSUM = [  0.00198   0.01478   0.06378   0.23378   0.91578 ]
 *
 * 9 octaves, 0.25dB
 *
 * */
template <typename sample_type, typename noise_engine>
struct pink_stochastic_voss_mccartney
{
    typedef boost::array<sample_type, 6> state_type;

    pink_stochastic_voss_mccartney(noise_engine const & engine, state_type const & state):
        engine(engine), state(state)
    {}

    template <int stage>
    sample_type update_state(sample_type val)
    {
        const int index = stage + 1;

        state[0] -= state[index];
        state[index] = val;
        sample_type ret = state[0] = state[0] + val;
        return ret;
    }

    sample_type operator()(void)
    {
        boost::uint32_t rv = engine();

        if (rv <= 8504035u)
            return update_state<0>(white_noise_scale<sample_type>(engine()) * sample_type(3.8024*0.04803));
        else if (rv <= 63479617u)
            return update_state<1>(white_noise_scale<sample_type>(engine()) * sample_type(2.9694*0.04803));
        else if (rv <= 273933014u)
            return update_state<2>(white_noise_scale<sample_type>(engine()) * sample_type(2.5970*0.04803));
        else if (rv <= 1004077454u)
            return update_state<3>(white_noise_scale<sample_type>(engine()) * sample_type(3.0870*0.04803));
        else if (rv <= 3933245150u)
            return update_state<4>(white_noise_scale<sample_type>(engine()) * sample_type(3.4006*0.04803));

        return state[0];
    }

    noise_engine engine;
    state_type state;
};

/** pink noise, filter designed by paul kellett,
 *  taken from musicdsp archive
 *
 *  11 octaves (9.2Hz at 44100Hz sampling rate), 0.05dB
 *
 *   */
template <typename sample_type, typename noise_engine>
struct pink_kellett
{
    typedef boost::array<sample_type, 7> state_type;

    pink_kellett(noise_engine const & engine, state_type const & state):
        engine(engine), state(state)
    {}

    sample_type operator()(void)
    {
        sample_type white = white_noise_scale<sample_type>(engine());

        state[0] = sample_type(0.99886) * state[0] + white * sample_type(0.0555179);
        state[1] = sample_type(0.99332) * state[1] + white * sample_type(0.0750759);
        state[2] = sample_type(0.96900) * state[2] + white * sample_type(0.1538520);
        state[3] = sample_type(0.86650) * state[3] + white * sample_type(0.3104856);
        state[4] = sample_type(0.55000) * state[4] + white * sample_type(0.5329522);
        state[5] = sample_type(-0.7616) * state[5] + white * sample_type(-0.0168980);

        sample_type pink = state[0] + state[1] + state[2] + state[3] + state[4] + state[5]
            + state[6] + white * sample_type(0.5362);

        state[6] = white * sample_type(0.115926);
        return pink * sample_type(0.11); /* gain compensation */
    }

    noise_engine engine;
    state_type state;
};

/** pink noise, filter designed by paul kellett, economy version,
 *  taken from musicdsp archive
 *
 *  11 octaves (9.2Hz at 44100Hz sampling rate), 0.5dB
 *
 *   */
template <typename sample_type, typename noise_engine>
struct pink_kellett_economy
{
    typedef boost::array<sample_type, 3> state_type;

    pink_kellett_economy(noise_engine const & engine, state_type const & state):
        engine(engine), state(state)
    {}

    sample_type operator()(void)
    {
        sample_type white = white_noise_scale<sample_type>(engine());

        state[0] = sample_type(0.99765) * state[0] + white * sample_type(0.0990460);
        state[1] = sample_type(0.96300) * state[1] + white * sample_type(0.2965164);
        state[2] = sample_type(0.57000) * state[2] + white * sample_type(1.0526913);

        sample_type pink = state[0] + state[1] + state[2] + white * sample_type(0.1848);

        return pink * sample_type(0.11); /* gain compensation */
    }

    noise_engine engine;
    state_type state;
};


} /* detail */


/** pink noise generator with external rng */
template <typename sample_type, typename noise_engine>
class pink_noise_external_engine:
    private detail::sr_interpolator<sample_type>
{
    BOOST_STATIC_ASSERT((boost::is_same<typename noise_engine::result_type, boost::uint32_t>::value));

    typedef detail::pink_stochastic_voss_mccartney<sample_type, noise_engine> generate;

    typename generate::state_type state;

public:
    pink_noise_external_engine(void)
    {
        state.assign(0);
    }

    template <typename output_buffer_type, typename sample_count_t, typename muladd_helper>
    inline void perform(output_buffer_type & out, sample_count_t n, noise_engine & engine_, muladd_helper & ma)
    {
        generate gen(engine_, state);

        detail::sr_interpolator<sample_type>::perform(gen, out, n, ma);

        engine_ = gen.engine;
        state = gen.state;
    }

    DEFINE_PERFORM_WRAPPER_NOISE

    void tick(sample_type & out, noise_engine & engine_)
    {
        generate gen(engine_, state);
        out = gen();
        engine_ = gen.engine;
        state = gen.state;
    }
};


/** pink noise generator with internal rng */
template <typename sample_type>
class pink_noise:
    private pink_noise_external_engine<sample_type, detail::cheap_noise_engine>
{
    typedef pink_noise_external_engine<sample_type, detail::cheap_noise_engine> base_class;

public:
    pink_noise(void):
        engine_(detail::thread_local_noise())
    {}

    template <typename output_buffer_type, typename sample_count_t, typename muladd_helper>
    inline void perform(output_buffer_type & out, sample_count_t n, muladd_helper & ma)
    {
        base_class::perform(out, n, engine_, ma);
    }

    DEFINE_PERFORM_WRAPPER_0_1

    void tick(sample_type & out)
    {
        base_class::tick(out, engine_);
    }

private:
    detail::cheap_noise_engine engine_;
};


namespace detail
{

template <typename sample_type, int interpolation_order>
struct interpolation_selector
{
    typedef typename boost::mpl::vector<sr_interpolator<sample_type>,
                                        sh_interpolator<sample_type>,
                                        linear_interpolator<sample_type>,
                                        quadratic_interpolator<sample_type>,
                                        quadratic_interpolator2<sample_type>
                                        >
    interpolators;

    typedef typename boost::mpl::at_c<interpolators, interpolation_order>::type type;
};


template <typename sample_type, class noise_engine>
struct white_noise_generator
{
    BOOST_STATIC_ASSERT((boost::is_same<typename noise_engine::result_type, boost::uint32_t>::value));

    white_noise_generator(noise_engine const & engine):
        engine(engine)
    {}

    sample_type operator()(void)
    {
        return detail::white_noise_scale<sample_type>(engine());
    }

    noise_engine engine;
};


template <typename sample_type, typename noise_engine>
struct clipped_noise_generator
{
    BOOST_STATIC_ASSERT((boost::is_same<typename noise_engine::result_type, boost::uint32_t>::value));

    clipped_noise_generator(noise_engine const & engine):
        engine(engine)
    {}

    sample_type operator()(void)
    {
        return detail::clipped_noise_scale<sample_type>(engine());
    }

    noise_engine engine;
};



template <typename sample_type, typename noise_engine, int interpolation_order, class generator>
class lf_noise_external_engine:
    public interpolation_selector<sample_type, interpolation_order>::type
{
    typedef typename interpolation_selector<sample_type, interpolation_order>::type base_class;

public:
    template <typename output_buffer_type, typename sample_count_t, typename muladd_helper>
    inline void perform(output_buffer_type & out, sample_count_t n, noise_engine & engine_, muladd_helper & ma)
    {
        generator gen(engine_);

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

template <typename sample_type, typename noise_engine, int interpolation_order, class generator>
class lf_noise:
    public interpolation_selector<sample_type, interpolation_order>::type
{
    typedef typename interpolation_selector<sample_type, interpolation_order>::type base_class;

public:
    template <typename output_buffer_type, typename sample_count_t, typename muladd_helper>
    inline void perform(output_buffer_type & out, sample_count_t n, muladd_helper & ma)
    {
        generator gen(engine_);

        base_class::perform(gen, out, n, ma);

        engine_ = gen.engine;
    }

    DEFINE_PERFORM_WRAPPER_0_1

    void tick(sample_type & out)
    {
        sample_type * out_sample = &out;
        muladd_helper_nop<sample_type> ma;
        perform(out_sample, 1, ma);
    }

private:
    noise_engine engine_;
};


template <typename sample_type, typename noise_engine, int interpolation_order>
class lf_white_noise_external_engine:
    public lf_noise_external_engine<sample_type, noise_engine, interpolation_order,
                                    white_noise_generator<sample_type, noise_engine> >
{};

template <typename sample_type, int interpolation_order>
class lf_white_noise:
    public lf_noise<sample_type, cheap_noise_engine, interpolation_order,
                    white_noise_generator<sample_type, cheap_noise_engine> >
{};

template <typename sample_type, typename noise_engine, int interpolation_order>
class lf_clipped_noise_external_engine:
    public lf_noise_external_engine<sample_type, noise_engine, interpolation_order,
                                    clipped_noise_generator<sample_type, noise_engine> >
{};

template <typename sample_type, int interpolation_order>
class lf_clipped_noise:
    public lf_noise<sample_type, cheap_noise_engine, interpolation_order,
                    clipped_noise_generator<sample_type, cheap_noise_engine> >
{};

} /* namespace detail */


/** white noise generator with external rng */
template <typename sample_type, typename noise_engine>
class white_noise_external_engine:
    public detail::lf_white_noise_external_engine<sample_type, noise_engine, 0>
{};

/** white noise generator with internal rng */
template <typename sample_type>
class white_noise:
    public detail::lf_white_noise<sample_type, 0>
{};


/** downsampled white noise generator with external rng, using a sample&hold interpolator */
template <typename sample_type, typename noise_engine>
class lf0_white_noise_external_engine:
    public detail::lf_white_noise_external_engine<sample_type, noise_engine, 1>
{};

/** downsampled white noise generator with internal rng, using a sample&hold interpolator */
template <typename sample_type>
class lf0_white_noise:
    public detail::lf_white_noise<sample_type, 1>
{};


/** downsampled white noise generator with external rng, using a linear interpolator */
template <typename sample_type, typename noise_engine>
class lf1_white_noise_external_engine:
    public detail::lf_white_noise_external_engine<sample_type, noise_engine, 2>
{};

/** downsampled white noise generator with internal rng, using a linear interpolator */
template <typename sample_type>
class lf1_white_noise:
    public detail::lf_white_noise<sample_type, 2>
{};


/** downsampled white noise generator with external rng, using a quadratic interpolator */
template <typename sample_type, typename noise_engine>
class lf2_white_noise_external_engine:
    public detail::lf_white_noise_external_engine<sample_type, noise_engine, 3>
{};

/** downsampled white noise generator with internal rng, using a quadratic interpolator */
template <typename sample_type>
class lf2_white_noise:
    public detail::lf_white_noise<sample_type, 3>
{};


/** downsampled white noise generator with external rng, using a quadratic interpolator */
template <typename sample_type, typename noise_engine>
class lf2a_white_noise_external_engine:
    public detail::lf_white_noise_external_engine<sample_type, noise_engine, 4>
{};

/** downsampled white noise generator with internal rng, using a quadratic interpolator */
template <typename sample_type>
class lf2a_white_noise:
    public detail::lf_white_noise<sample_type, 4>
{};




/** clipped noise generator with external rng */
template <typename sample_type, typename noise_engine>
class clipped_noise_external_engine:
    public detail::lf_clipped_noise_external_engine<sample_type, noise_engine, 0>
{};

/** clipped noise generator with internal rng */
template <typename sample_type>
class clipped_noise:
    public detail::lf_clipped_noise<sample_type, 0>
{};


/** downsampled clipped noise generator with external rng, using a sample&hold interpolator */
template <typename sample_type, typename noise_engine>
class lf0_clipped_noise_external_engine:
    public detail::lf_clipped_noise_external_engine<sample_type, noise_engine, 1>
{};

/** downsampled clipped noise generator with internal rng, using a sample&hold interpolator */
template <typename sample_type>
class lf0_clipped_noise:
    public detail::lf_clipped_noise<sample_type, 1>
{};


/** downsampled clipped noise generator with external rng, using a linear interpolator */
template <typename sample_type, typename noise_engine>
class lf1_clipped_noise_external_engine:
    public detail::lf_clipped_noise_external_engine<sample_type, noise_engine, 2>
{};

/** downsampled clipped noise generator with internal rng, using a linear interpolator */
template <typename sample_type>
class lf1_clipped_noise:
    public detail::lf_clipped_noise<sample_type, 2>
{};


/** downsampled clipped noise generator with external rng, using a quadratic interpolator */
template <typename sample_type, typename noise_engine>
class lf2_clipped_noise_external_engine:
    public detail::lf_clipped_noise_external_engine<sample_type, noise_engine, 3>
{};

/** downsampled clipped noise generator with internal rng, using a quadratic interpolator */
template <typename sample_type>
class lf2_clipped_noise:
    public detail::lf_clipped_noise<sample_type, 3>
{};


/** downsampled clipped noise generator with external rng, using a quadratic interpolator 2 */
template <typename sample_type, typename noise_engine>
class lf2a_clipped_noise_external_engine:
    public detail::lf_clipped_noise_external_engine<sample_type, noise_engine, 4>
{};

/** downsampled clipped noise generator with internal rng, using a quadratic interpolator 2 */
template <typename sample_type>
class lf2a_clipped_noise:
    public detail::lf_clipped_noise<sample_type, 4>
{};


} /* namespace nova */

#undef DEFINE_PERFORM_WRAPPER_NOISE

#endif /* DSP_NOISE_HPP */
