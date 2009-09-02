//  fundamental filter functions
//  Copyright (C) 2005, 2007, 2008 Tim Blechmann
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

#ifndef DSP_FILTER_HPP
#define DSP_FILTER_HPP

#include <boost/array.hpp>
#include <boost/foreach.hpp>
#include <cmath>
#include <algorithm>

#include <boost/math/constants/constants.hpp>
#include <boost/mpl/if.hpp>
#include <boost/mpl/bool.hpp>


#include "muladd_helpers.hpp"
#include "sample_extractor.hpp"
#include "parameter.hpp"

#include "../utilities/branch_hints.hpp"
#include "../utilities/template_functions.hpp"

#include "cache_aligned_array.hpp"

namespace nova
{

/* the mathematical constants */
#define pi boost::math::constants::pi<internal_type>()

namespace detail
{

/* array of samples, usable as argument by muladd classes
 *
 * \todo where should this class go to?
 * */
template <typename sample_type, unsigned int n>
struct sample_block:
    public aligned_array<sample_type, n>
{
    sample_block operator+=(sample_type val)
    {
        for (unsigned int i = 0; i != n; ++i)
            aligned_array<sample_type, n>::operator[](i) += val;

        return *this;
    }

    sample_block operator*=(sample_type val)
    {
        for (unsigned int i = 0; i != n; ++i)
            aligned_array<sample_type, n>::operator[](i) *= val;

        return *this;
    }
};


} /* namespace detail */


#if 0
/** \brief 1-pole 1-zero filter */
void filter1p1z(float * dest, float * src, unsigned int n, float * p_last, const float * p_coef)
{
    float coef = *p_coef;
    float last = *p_last;

    if (coef < 1)
    {
        for (int i = 0; i < n; i++)
        {
            float smpl = *src++ + coef * last;
            *dest++ = smpl - last;
            last = smpl;
        }
        *p_last = undenormalize(last);
    }
    else
    {
        copyvec(dest, src, n);
        *p_last = 0;
    }
}

/** \brief 1-pole filter */
void filter1p(float * dest, float * src, unsigned int n, float * p_last, const float * p_coef)
{
    float coef = *p_coef;
    float last = *p_last;
    float feedback = 1 - coef;

    for (int i = 0; i < n; i++)
    {
        float smpl = coef * *src++ + feedback * last;
        *dest++ = smpl;
        last = smpl;
    }

    *p_last = undenormalize(last);
}

/** \brief 2-pole filter */
void filter2p(float * dest, float * src, unsigned int n, float * p_last, float * p_prev,
              const float * p_coef1, const float * p_coef2, const float * p_gain)
{
    float last = *p_last;
    float prev = *p_prev;
    float coef1 = *p_coef1;
    float coef2 = *p_coef2;
    float gain = *p_gain;

    for (int i = 0; i < n; i++)
    {
        float smpl =  *src++ + coef1 * last + coef2 * prev;
        *dest++ = gain * smpl;
        prev = last;
        last = smpl;
    }

    *p_last = undenormalize(last);
    *p_prev = undenormalize(prev);
}

/** \brief struct containing the biquad data */
struct biquadData
{
    float last;
    float prev;

    float fb1;
    float fb2;
    float ff1;
    float ff2;
    float ff3;
};


/** \brief raw biquad filter */
void filterbiquad(float * dest, float * src, unsigned int n, biquadData * data)
{
    float last = data->last;
    float prev = data->prev;
    float fb1 = data->fb1;
    float fb2 = data->fb2;
    float ff1 = data->ff1;
    float ff2 = data->ff2;
    float ff3 = data->ff3;

    for (int i = 0; i < n; i++)
    {
        float smpl =  *src++ + fb1 * last + fb2 * prev;
        smpl = undenormalize(smpl);
        *dest++ = ff1 * smpl + ff2 * last + ff3 * prev;
        prev = last;
        last = smpl;
    }

    data->last = last;
    data->prev = prev;
}

/** \brief raw biquad filter */
void filterbiquad_direct(float * dest, float * src, unsigned int n, biquadData * data)
{
    for (int i = 0; i < n; i++)
    {
        float smpl =  *src++ + data->fb1 * data->last + data->fb2 * data->prev;
        smpl = undenormalize(smpl);
        *dest++ = data->ff1 * smpl + data->ff2 * data->last + data->ff3 * data->prev;
        data->prev = data->last;
        data->last = smpl;
    }
}


/** \brief raw biquad filter, with loop unrolling */
void filterbiquad_8(float * dest, float * src, unsigned int n, biquadData * data)
{
    float last = data->last;
    float prev = data->prev;
    float fb1 = data->fb1;
    float fb2 = data->fb2;
    float ff1 = data->ff1;
    float ff2 = data->ff2;
    float ff3 = data->ff3;

    n = n / 8;
    for (int i = 0; i < n; i++)
    {
        float smpl =  *src++ + fb1 * last + fb2 * prev;
        smpl = undenormalize(smpl);
        *dest++ = ff1 * smpl + ff2 * last + ff3 * prev;
        prev = last;
        last = smpl;

        smpl =  *src++ + fb1 * last + fb2 * prev;
        smpl = undenormalize(smpl);
        *dest++ = ff1 * smpl + ff2 * last + ff3 * prev;
        prev = last;
        last = smpl;

        smpl =  *src++ + fb1 * last + fb2 * prev;
        smpl = undenormalize(smpl);
        *dest++ = ff1 * smpl + ff2 * last + ff3 * prev;
        prev = last;
        last = smpl;

        smpl =  *src++ + fb1 * last + fb2 * prev;
        smpl = undenormalize(smpl);
        *dest++ = ff1 * smpl + ff2 * last + ff3 * prev;
        prev = last;
        last = smpl;

        smpl =  *src++ + fb1 * last + fb2 * prev;
        smpl = undenormalize(smpl);
        *dest++ = ff1 * smpl + ff2 * last + ff3 * prev;
        prev = last;
        last = smpl;

        smpl =  *src++ + fb1 * last + fb2 * prev;
        smpl = undenormalize(smpl);
        *dest++ = ff1 * smpl + ff2 * last + ff3 * prev;
        prev = last;
        last = smpl;

        smpl =  *src++ + fb1 * last + fb2 * prev;
        smpl = undenormalize(smpl);
        *dest++ = ff1 * smpl + ff2 * last + ff3 * prev;
        prev = last;
        last = smpl;

        smpl =  *src++ + fb1 * last + fb2 * prev;
        smpl = undenormalize(smpl);
        *dest++ = ff1 * smpl + ff2 * last + ff3 * prev;
        prev = last;
        last = smpl;
    }

    data->last = last;
    data->prev = prev;
}


/** \brief raw biquad filter, with loop unrolling & relaxed denormal bashing */
void filterbiquad_8_relaxed(float * dest, float * src, unsigned int n, biquadData * data)
{
    float last = data->last;
    float prev = data->prev;
    float fb1 = data->fb1;
    float fb2 = data->fb2;
    float ff1 = data->ff1;
    float ff2 = data->ff2;
    float ff3 = data->ff3;

    n = n / 8;
    for (int i = 0; i < n; i++)
    {
        float smpl =  *src++ + fb1 * last + fb2 * prev;
        smpl = undenormalize(smpl);
        *dest++ = ff1 * smpl + ff2 * last + ff3 * prev;
        prev = last;
        last = smpl;

        smpl =  *src++ + fb1 * last + fb2 * prev;
        *dest++ = ff1 * smpl + ff2 * last + ff3 * prev;
        prev = last;
        last = smpl;

        smpl =  *src++ + fb1 * last + fb2 * prev;
        *dest++ = ff1 * smpl + ff2 * last + ff3 * prev;
        prev = last;
        last = smpl;

        smpl =  *src++ + fb1 * last + fb2 * prev;
        *dest++ = ff1 * smpl + ff2 * last + ff3 * prev;
        prev = last;
        last = smpl;

        smpl =  *src++ + fb1 * last + fb2 * prev;
        *dest++ = ff1 * smpl + ff2 * last + ff3 * prev;
        prev = last;
        last = smpl;

        smpl =  *src++ + fb1 * last + fb2 * prev;
        *dest++ = ff1 * smpl + ff2 * last + ff3 * prev;
        prev = last;
        last = smpl;

        smpl =  *src++ + fb1 * last + fb2 * prev;
        *dest++ = ff1 * smpl + ff2 * last + ff3 * prev;
        prev = last;
        last = smpl;

        smpl =  *src++ + fb1 * last + fb2 * prev;
        *dest++ = ff1 * smpl + ff2 * last + ff3 * prev;
        prev = last;
        last = smpl;
    }

    data->last = last;
    data->prev = prev;
}

void filterbiquad_unchecked(float * dest, float * src, unsigned int n, biquadData * data)
{
    float last = data->last;
    float prev = data->prev;
    float fb1 = data->fb1;
    float fb2 = data->fb2;
    float ff1 = data->ff1;
    float ff2 = data->ff2;
    float ff3 = data->ff3;

    for (int i = 0; i < n; i++)
    {
        float smpl =  *src++ + fb1 * last + fb2 * prev;
        *dest++ = ff1 * smpl + ff2 * last + ff3 * prev;
        prev = last;
        last = smpl;
    }

    data->last = last;
    data->prev = prev;
}
#endif

namespace detail
{
template <typename internal_type,
          bool checked>
class filter_base
{
protected:
    static inline internal_type check(internal_type arg)
    {
        if (checked)
        {
            if (likely(isnormal(arg)))
                return arg;
            else
                return 0;
        }
        else
            return arg;
    }
};
} /* namespace detail */

/** \brief templated n-th order system
 *
 *  implements the function:
 *  y(n) = b0 * x(n) + b1 * x(n - 1) + ... + bM * x(n - M) - a1 * y(n - 1) - ... - aM * y(n - M)
 *
 *         b0 + b1 * z^-1 + ... + bM * z^(-M)
 *  H(z) = ----------------------------------
 *          1 + a1 * z^-1 + ... + aM * z^(-M)
 *
 * */
template <uint order,
          typename sample_type,
          typename internal_type = sample_type,
          bool checked=true>
class nth_order_system:
    protected detail::filter_base<internal_type, checked>
{
public:
    nth_order_system(void)
    {
        reset();

        for (int i = 0; i != order; ++i)
            a[i] = 0;

        for (int i = 0; i != order+1; ++i)
            b[i] = 0;
    }

    /* transposed direct-form II, adapted from
     *
     * ``Introduction to Digital Filters with Audio Applications'', by  Julius O. Smith III,
     * */
    inline sample_type compute_sample(sample_type in, boost::array<internal_type, order> & z)
    {
        const internal_type input (in);

        const internal_type out = check(z[0] + input * b[0]);

        for (int i = 0; i != order - 1; ++i)
            z[i] = - out * a[i] + input * b[i + 1] + z[i+1];

        z[order - 1] = - out * a[order - 1] + input * b[order];

        return sample_type(out);
    }

    void reset(void)
    {
        z_.assign(0);
    }

protected:
    boost::array<internal_type, order> a;
    boost::array<internal_type, order+1> b;

    boost::array<internal_type, order> z_;
};

/** \brief templated second order system.
 *
 * */
template <typename sample_type,
          typename internal_type = sample_type, /* type used for internal representation */
          bool checked = true,                  /* denormal bashing */
          bool copy_temporary = true            /* copy temporary  */
          >
class biquad:
    protected nth_order_system<2, sample_type, internal_type, checked>
{
    typedef nth_order_system<2, sample_type, internal_type, checked> filter;

    typedef typename boost::mpl::if_c<copy_temporary,
                                      boost::array<internal_type, 2>,
                                      boost::array<internal_type, 2> &
                                      >::type temporary_type;

public:
    template <typename input_buffer_type, typename output_buffer_type>
    inline void perform(input_buffer_type const & in, output_buffer_type & out, uint n)
    {
        const sample_type * in_sample = get_samples(in);
        sample_type * out_sample = get_samples(out);

        temporary_type z = filter::z_;

        do
        {
            *out_sample = filter::compute_sample(*in_sample, z);

            ++out_sample;
            ++in_sample;
            --n;
        }
        while (n);
        filter::z_ = z;
    }


    void set_checked(internal_type new_a1, internal_type new_a2,
                     internal_type new_b0, internal_type new_b1, internal_type new_b2)
    {
        internal_type b1 = -new_a1;
        internal_type b2 = -new_a2;

        /* miller's stability checking code */
        internal_type discriminant = b1 * b1 + 4 * b2;
        if (discriminant < 0) /* imaginary roots -- resonant filter */
        {
            /* they're conjugates so we just check that the product
               is less than one */
            if (b2 >= -1.0)
                goto stable;
        }
        else    /* real roots */
        {
            /* check that the parabola 1 - fb1 x - fb2 x^2 has a
               vertex between -1 and 1, and that it's nonnegative
               at both ends, which implies both roots are in [1-,1]. */
            if (b1 <= 2.0 && b1 >= -2.0 &&
                1.0 - b1 - b2 >= 0 && 1.0 + b1 - b2 >= 0)
                goto stable;
        }

        /* if unstable, just bash to zero */
        new_a1 = new_a2 = new_b0 = new_b1 = new_b2 = 0;


    stable:
        filter::a[0] = new_a1;
        filter::a[1] = new_a2;
        filter::b[0] = new_b0;
        filter::b[1] = new_b1;
        filter::b[2] = new_b2;
    }

    void set(internal_type new_a1, internal_type new_a2,
             internal_type new_b0, internal_type new_b1, internal_type new_b2)
    {
        filter::a[0] = new_a1;
        filter::a[1] = new_a2;
        filter::b[0] = new_b0;
        filter::b[1] = new_b1;
        filter::b[2] = new_b2;
    }

};



/** \brief state variable filter
 *
 * */
template <typename sample_type,
          typename internal_type = sample_type,
          bool checked=true,
          bool copy_temporary = true>
class state_variable_filter:
    protected detail::filter_base<internal_type, checked>
{
    typedef detail::sample_block<sample_type, 4> sample_block;

    typedef typename boost::mpl::if_c<copy_temporary,
                                      internal_type,
                                      internal_type &
                                      >::type temporary_type;
public:
    state_variable_filter(void):
        low_(0), band_(0)
    {}

    template <typename input_buffer_type, typename output_buffer_type, typename sample_count_t, typename muladd_helper>
    inline void perform(input_buffer_type const & in, output_buffer_type & low_out, output_buffer_type & high_out,
                        output_buffer_type & band_out, output_buffer_type & notch_out, sample_count_t n, muladd_helper & ma)
    {
        const sample_type * in_sample = get_samples(in);
        sample_type * low_sample = get_samples(low_out);
        sample_type * high_sample = get_samples(high_out);
        sample_type * band_sample = get_samples(band_out);
        sample_type * notch_sample = get_samples(notch_out);

        temporary_type low = low_;
        temporary_type band = band_;

        for (sample_count_t i = 0; i != n; ++i)
        {
            const sample_block samples = compute_sample(in_sample[i], f_, inv_q_, band, low);

            ma(samples);

            low_sample[i] = samples[0];
            high_sample[i] = samples[1];
            band_sample[i] = samples[2];
            notch_sample[i] = samples[3];
        }

        low_ = low;
        band_ = band;
    }

    DEFINE_PERFORM_WRAPPER_1_4

    static sample_block compute_sample(sample_type in, const internal_type f, const internal_type inv_q,
                                       internal_type & band_old, internal_type & low_old)
    {
        const internal_type low = band_old * f + low_old;
        const internal_type high = in - low - inv_q * band_old;
        const internal_type band = high * f + band_old;
        const internal_type notch = high + low;

        const internal_type stabilized_band = band - cube(band) * 0.001; /* stabilize filter */

        band_old = check(stabilized_band);
        low_old = check(low);

        sample_block ret;
        ret[0] = low;
        ret[1] = high;
        ret[2] = band;
        ret[3] = notch;
        return ret;
    }

    void set_frequency(internal_type freq)
    {
        f_ = std::sin(2 * pi * freq);
    }

    void set_frequency(internal_type freq, internal_type samplerate)
    {
        set_frequency(freq / samplerate);
    }

    /* q mapping adapted from cyclone */
    void set_q(internal_type q)
    {
        const internal_type q_stretch = 1.2;
        internal_type r = (1 - q) * q_stretch;
        r = clip(r, 0., q_stretch);

        inv_q_ = r * r;
    }

private:
    internal_type low_;
    internal_type band_;

    internal_type inv_q_;
    internal_type f_;
};


#if 0
template <typename sample_type,
          typename internal_type,
          uint size,
          bool sign,
          bool checked>
class nth_order_allpass:
    private detail::filter_base<internal_type, checked>
{

    sample_type compute_sample(sample_type input, boost::array<internal_type, order> & z = z_)
    {
        internal_type in = input;

        internal_type n = a[order-1];
        internal_type d = 1;

        for (int i = 0; i != order; ++i)
        {
            n += z[i] * a[order-1-i];
            d += z[i] * a[i];
        }

        for (int i = 0; i != order-1; ++i)
        {
            z[i] = z[i+1];
        }

        internal_type ret;
        if (sign)
            ret = n / d;
        else
            ret = - n / d;

        z[0] = check(ret);

        return ret;
    }


protected:
    boost::array<internal_type, size> a;
    boost::array<internal_type, order> z_;
};


#endif

namespace detail
{
template <typename internal_type>
class mitra_regalia_eq
{
protected:
    static inline internal_type compute_sample(internal_type in, internal_type allpass_sample, internal_type K)
    {
        return ( (in + allpass_sample) +
                 (in - allpass_sample) * K) * 0.5;
    }

};

/** \brief low/high-shelf filter based on:
 *
 *  Tunable digital frequency response equalization filters
 *  Regalia, P.; Mitra, S.
 *  Acoustics, Speech, and Signal Processing [see also IEEE Transactions on Signal Processing], IEEE Transactions on
 *  Volume 35, Issue 1, Jan 1987 Page(s): 118 - 120
 *
 * */
template <bool low,
          typename sample_type,
          typename internal_type,
          bool checked,
          bool interpolating>
class mitra_regalia_shelf_base:
    protected detail::mitra_regalia_eq<internal_type>,
    protected detail::filter_base<internal_type, checked>
{
protected:
    typedef typename boost::mpl::if_c<interpolating,
                                      detail::exponential_interpolating_parameter<internal_type, 1, 2205>, /* 50ms @ 44100 */
                                      detail::noninterpolating_parameter<internal_type>
                                      >::type gain_parameter_type;

    typedef typename boost::mpl::if_c<interpolating,
                                      detail::linear_interpolating_parameter<internal_type, 2205>, /* 50ms @ 44100 */
                                      detail::noninterpolating_parameter<internal_type>
                                      >::type filter_parameter_type;

    mitra_regalia_shelf_base(void):
        a_(0), K(1), z_(0)
    {}

public:
    template <typename input_buffer_type, typename output_buffer_type>
    inline void perform(input_buffer_type const & in, output_buffer_type & out, uint n)
    {
        const sample_type * in_sample = get_samples(in);
        sample_type * out_sample = get_samples(out);

        internal_type z = z_;

        gain_parameter_type k (K);
        filter_parameter_type a (a_);

        assert(n);
        do
            *out_sample++ = compute_sample(*in_sample++, z, a, k);
        while(--n);

        z_ = z;
        K.update(k);
        a_.update(a);
    }

    void set_factor(internal_type k)
    {
        K = k;
    }

    void reset_factor(internal_type k)
    {
        K.reset(k);
    }

    void set_frequency(internal_type freq)
    {
        a_ = compute_a(freq * 2 * pi);
    }

    void reset_frequency(internal_type freq)
    {
        a_.reset(compute_a(freq * 2 * pi));
    }


protected:
    static inline sample_type compute_sample(sample_type in, internal_type & z, const internal_type a,
                                             const internal_type K)
    {
        internal_type allpassed = compute_allpass(in, a, z);
        return detail::mitra_regalia_eq<internal_type>::compute_sample(in, allpassed, K);
    }

    static inline internal_type compute_allpass(internal_type in, const internal_type a, internal_type & z)
    {
        internal_type new_z = in - z*a;
        internal_type ret = z + new_z*a;

        z = check(new_z);
        if (low)
            return -ret;
        else
            return ret;
    }

    static inline internal_type compute_a(internal_type omega)
    {
        internal_type tan_omega_2 = std::tan(omega * 0.5);
        return (tan_omega_2 - 1) / (tan_omega_2 + 1);
    }

protected:
    filter_parameter_type a_;
    gain_parameter_type K;
    internal_type z_;
};

template <bool low,
          typename sample_type,
          typename internal_type,
          bool checked,
          bool interpolating>
class mitra_regalia_first_order_filter:
    protected detail::mitra_regalia_eq<internal_type>,
    protected detail::filter_base<internal_type, checked>
{
protected:
    typedef typename boost::mpl::if_c<interpolating,
                                      detail::exponential_interpolating_parameter<internal_type, 1, 2205>, /* 50ms @ 44100 */
                                      detail::noninterpolating_parameter<internal_type>
                                      >::type gain_parameter_type;

    typedef typename boost::mpl::if_c<interpolating,
                                      detail::linear_interpolating_parameter<internal_type, 2205>, /* 50ms @ 44100 */
                                      detail::noninterpolating_parameter<internal_type>
                                      >::type filter_parameter_type;

    mitra_regalia_first_order_filter(void):
        a_(0), z_(0)
    {}

public:
    template <typename input_buffer_type, typename output_buffer_type>
    inline void perform(input_buffer_type const & in, output_buffer_type & out, uint n)
    {
        const sample_type * in_sample = get_samples(in);
        sample_type * out_sample = get_samples(out);

        internal_type z = z_;

        filter_parameter_type a (a_);

        for (uint i = 0; i != n; ++i)
            out_sample[i] = compute_sample(in_sample[i], z, a);

        z_ = z;
        a_.update(a);
    }

    void set_frequency(internal_type freq)
    {
        a_ = compute_a(freq * 2 * pi);
    }

    void reset_frequency(internal_type freq)
    {
        a_.reset(compute_a(freq * 2 * pi));
    }


protected:
    static inline sample_type compute_sample(sample_type in, internal_type & z, const internal_type a)
    {
        in = in * 0.5;
        internal_type allpassed = compute_allpass(in, a, z);

        if (low)
            return in - allpassed;
        else
            return in + allpassed;
    }

    static inline internal_type compute_allpass(internal_type in, const internal_type a, internal_type & z)
    {
        internal_type new_z = in - z*a;
        internal_type ret = z + new_z*a;

        z = check(new_z);
        return -ret;
    }

    static inline internal_type compute_a(internal_type omega)
    {
        internal_type tan_omega_2 = std::tan(omega * 0.5);
        return (tan_omega_2 - 1) / (tan_omega_2 + 1);
    }

protected:
    filter_parameter_type a_;
    internal_type z_;
};

template <bool pass,
          typename sample_type,
          typename internal_type,
          bool checked,
          bool interpolating=true>
class mitra_regalia_2nd_order:
    detail::filter_base<internal_type, checked>
{
    typedef boost::array<internal_type, 2> z_type;

    typedef typename boost::mpl::if_c<interpolating,
                                      detail::exponential_interpolating_parameter<internal_type, 1, 2205>, /* 50ms @ 44100 */
                                      detail::noninterpolating_parameter<internal_type>
                                      >::type gain_parameter_type;

    typedef typename boost::mpl::if_c<interpolating,
                                      detail::linear_interpolating_parameter<internal_type, 2205>, /* 50ms @ 44100 */
                                      detail::noninterpolating_parameter<internal_type>
                                      >::type filter_parameter_type;

public:
    mitra_regalia_2nd_order(void):
        a_(0), b_(0)
    {
        z_.assign(0);
    }

    template <typename input_buffer_type, typename output_buffer_type>
    inline void perform(input_buffer_type const & in, output_buffer_type & out, uint n)
    {
        const sample_type * in_sample = get_samples(in);
        sample_type * out_sample = get_samples(out);

        z_type z = z_;
        filter_parameter_type a (a_);
        filter_parameter_type b (b_);

        for (uint i = 0; i != n; ++i)
            out_sample[i] = compute_sample(in_sample[i], z, a, b);

        z_ = z;
        a_.update(a);
        b_.update(b);
    }


protected:
    static inline internal_type compute_allpass(const internal_type in, const internal_type a, const internal_type b,
                                                z_type & z)
    {
        internal_type step1 = in - z[1] * a;
        internal_type step2 = step1 - z[0] * b;

        internal_type back1 = z[0] + b * step2;
        internal_type back2 = z[1] + a * step1;

        z[0] = check(step2);
        z[1] = check(back1);

        return back2;
    }

    static inline sample_type compute_sample(sample_type in, z_type & z, const internal_type a,
                                             const internal_type b)
    {
        in = in * 0.5;
        internal_type allpassed = compute_allpass(in, a, b, z);

        if (pass)
            return in - allpassed;
        else
            return in + allpassed;
    }


    static inline internal_type compute_a(internal_type omega)
    {
        internal_type tan_omega_2 = std::tan(omega * 0.5);

        return (1 - tan_omega_2) / (1 + tan_omega_2);
    }

    static inline internal_type compute_b(internal_type omega_0)
    {
        return -std::cos(omega_0);
    }

public:
    void set_frequency(internal_type omega_0)
    {
        b_ = compute_b(omega_0 * 2 * pi);
    }

    void set_bandwidth(internal_type omega)
    {
        a_ = compute_a(omega * 2 * pi);
    }

    void reset_frequency(internal_type omega_0)
    {
        b_.reset(compute_b(omega_0 * 2 * pi));
    }

    void reset_bandwidth(internal_type omega)
    {
        a_.reset(compute_a(omega * 2 * pi));
    }

protected:
    filter_parameter_type a_;
    filter_parameter_type b_;
    boost::array<internal_type, 2> z_;
};


}

/** \brief low-shelf filter
 * */
template <typename sample_type,
          typename internal_type,
          bool checked,
          bool interpolating>
class mitra_regalia_low_shelf:
    public detail::mitra_regalia_shelf_base<true, sample_type, internal_type, checked, interpolating>
{};

/** \brief high-shelf filter
 * */
template <typename sample_type,
          typename internal_type,
          bool checked,
          bool interpolating>
class mitra_regalia_high_shelf:
    public detail::mitra_regalia_shelf_base<false, sample_type, internal_type, checked, interpolating>
{};


/** \brief low-pass filter
 * */
template <typename sample_type,
          typename internal_type,
          bool checked,
          bool interpolating>
class mitra_regalia_low_pass:
    public detail::mitra_regalia_first_order_filter<true, sample_type, internal_type, checked, interpolating>
{};

/** \brief high-pass filter
 * */
template <typename sample_type,
          typename internal_type,
          bool checked,
          bool interpolating>
class mitra_regalia_high_pass:
    public detail::mitra_regalia_first_order_filter<false, sample_type, internal_type, checked, interpolating>
{};

/** \brief band-pass filter
 * */
template <typename sample_type,
          typename internal_type,
          bool checked,
          bool interpolating>
class mitra_regalia_band_pass:
    public detail::mitra_regalia_2nd_order<true, sample_type, internal_type, checked, interpolating>
{};

/** \brief high-pass filter
 * */
template <typename sample_type,
          typename internal_type,
          bool checked,
          bool interpolating>
class mitra_regalia_band_reject:
    public detail::mitra_regalia_2nd_order<false, sample_type, internal_type, checked, interpolating>
{};


/** \brief bandpass/reject filter based on:
 *
 *  Tunable digital frequency response equalization filters
 *  Regalia, P.; Mitra, S.
 *  Acoustics, Speech, and Signal Processing [see also IEEE Transactions on Signal Processing], IEEE Transactions on
 *  Volume 35, Issue 1, Jan 1987 Page(s): 118 - 120
 *
 *  using Gray and Markel allpass lattice filter structure
 *
 * */
template <typename sample_type,
          typename internal_type,
          bool checked,
          bool interpolating=true>
class mitra_regalia_eq:
    detail::mitra_regalia_eq<internal_type>,
    detail::filter_base<internal_type, checked>
{
    typedef boost::array<internal_type, 2> z_type;

    typedef typename boost::mpl::if_c<interpolating,
                                      detail::exponential_interpolating_parameter<internal_type, 1, 2205>, /* 50ms @ 44100 */
                                      detail::noninterpolating_parameter<internal_type>
                                      >::type gain_parameter_type;

    typedef typename boost::mpl::if_c<interpolating,
                                      detail::linear_interpolating_parameter<internal_type, 2205>, /* 50ms @ 44100 */
                                      detail::noninterpolating_parameter<internal_type>
                                      >::type filter_parameter_type;

public:
    mitra_regalia_eq(void):
        a_(0), b_(0), K(1)
    {
        z_.assign(0);
    }

    template <typename input_buffer_type, typename output_buffer_type>
    inline void perform(input_buffer_type const & in, output_buffer_type & out, uint n)
    {
        const sample_type * in_sample = get_samples(in);
        sample_type * out_sample = get_samples(out);

        z_type z = z_;

        gain_parameter_type k (K);
        filter_parameter_type a (a_);
        filter_parameter_type b (b_);

        assert(n);
        do
            *out_sample++ = compute_sample(*in_sample++, z, a, b, k);
        while(--n);

        z_ = z;
        K.update(k);
        a_.update(a);
        b_.update(b);
    }


protected:
    static inline internal_type compute_allpass(internal_type in, const internal_type a, const internal_type b,
                                                z_type & z)
    {
        internal_type step1 = in - z[1] * a;
        internal_type step2 = step1 - z[0] * b;

        internal_type back1 = z[0] + b * step2;
        internal_type back2 = z[1] + a * step1;

        z[0] = check(step2);
        z[1] = check(back1);

        return back2;
    }


    static inline sample_type compute_sample(sample_type in, z_type & z, const internal_type a,
                                             const internal_type b, const internal_type K)
    {
        internal_type allpassed = compute_allpass(in, a, b, z);
        return detail::mitra_regalia_eq<internal_type>::compute_sample(in, allpassed, K);
    }

    static inline internal_type compute_a(internal_type omega)
    {
        internal_type tan_omega_2 = std::tan(omega * 0.5);

        return (1 - tan_omega_2) / (1 + tan_omega_2);
    }

    static inline internal_type compute_b(internal_type omega_0)
    {
        return -std::cos(omega_0);
    }

public:
    void set_frequency(internal_type omega_0)
    {
        b_ = compute_b(omega_0 * 2 * pi);
    }

    void set_bandwidth(internal_type omega)
    {
        a_ = compute_a(omega * 2 * pi);
    }

    void set_factor(internal_type k)
    {
        K = k;
    }

    void reset_frequency(internal_type omega_0)
    {
        b_.reset(compute_b(omega_0 * 2 * pi));
    }

    void reset_bandwidth(internal_type omega)
    {
        a_.reset(compute_a(omega * 2 * pi));
    }

    void reset_factor(internal_type k)
    {
        K.reset(k);
    }

protected:
    filter_parameter_type a_;
    filter_parameter_type b_;
    gain_parameter_type K;
    boost::array<internal_type, 2> z_;
};

#undef pi

} /* namespace nova */

#endif /* DSP_FILTER_HPP */
