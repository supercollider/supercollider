//  muladd helper classes
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

#ifndef DSP_MULADD_HELPERS_HPP
#define DSP_MULADD_HELPERS_HPP

#include "helpers.hpp"

namespace nova
{
namespace detail
{

#if 0
auto concept muladd_helper<typename T>
{
    template <typename arg_sample_type>
    arg_sample_type operator()(arg_sample_type);

};
#endif

/* dummy muladd helper */
template <typename sample_type>
struct muladd_helper_nop
{
    template <typename arg_sample_type>
    arg_sample_type operator()(arg_sample_type sample)
    {
        return sample;
    }
};

/* muladd helper, constant factor */
template <typename sample_type>
struct muladd_helper_mul_c
{
    muladd_helper_mul_c(sample_type const & factor):
        factor(factor)
    {}

    template <typename arg_sample_type>
    arg_sample_type operator()(arg_sample_type sample)
    {
        return sample * factor;
    }

    sample_type factor;
};

/* muladd helper, linear changing factor */
template <typename sample_type>
struct muladd_helper_mul_l
{
    muladd_helper_mul_l(sample_type const & factor, sample_type const & slope):
        factor(factor), slope(slope)
    {}

    template <typename arg_sample_type>
    arg_sample_type operator()(arg_sample_type sample)
    {
        arg_sample_type ret = sample * factor;
        factor += slope;
        return ret;
    }

    sample_type factor;
    sample_type slope;
};

/* muladd helper, factor vector */
template <typename sample_type, typename input_buffer_type = sample_type*>
struct muladd_helper_mul_v
{
    muladd_helper_mul_v(input_buffer_type const & input):
        factors( get_samples(input) )
    {}

    template <typename arg_sample_type>
    arg_sample_type operator()(arg_sample_type sample)
    {
        return sample * *factors++;
    }

    const sample_type * __restrict__ factors;
};


/* muladd helper, constant offset */
template <typename sample_type>
struct muladd_helper_add_c
{
    muladd_helper_add_c(sample_type const & offset):
        offset(offset)
    {}

    template <typename arg_sample_type>
    arg_sample_type operator()(arg_sample_type sample)
    {
        return sample + offset;
    }

    sample_type offset;
};

/* muladd helper, linear changing offset */
template <typename sample_type>
struct muladd_helper_add_l
{
    muladd_helper_add_l(sample_type const & offset, sample_type const & slope):
        offset(offset), slope(slope)
    {}

    template <typename arg_sample_type>
    arg_sample_type operator()(arg_sample_type sample)
    {
        arg_sample_type ret = sample + offset;
        offset += slope;
        return ret;
    }

    sample_type offset;
    sample_type slope;
};

/* muladd helper, offset vector */
template <typename sample_type, typename input_buffer_type = sample_type*>
struct muladd_helper_add_v
{
    muladd_helper_add_v(input_buffer_type const & input):
        offsets( get_samples(input) )
    {}

    template <typename arg_sample_type>
    arg_sample_type operator()(arg_sample_type sample)
    {
        return sample + *offsets++;
    }

    const sample_type * __restrict__ offsets;
};



/* muladd helper, constant factor, constant offset */
template <typename sample_type>
struct muladd_helper_mul_c_add_c
{
    muladd_helper_mul_c_add_c(sample_type const & factor, sample_type const & offset):
        factor(factor), offset(offset)
    {}

    template <typename arg_sample_type>
    arg_sample_type operator()(arg_sample_type sample)
    {
        return sample * factor + offset;
    }

    sample_type factor, offset;
};

/* muladd helper, constant factor, linear changing offset */
template <typename sample_type>
struct muladd_helper_mul_c_add_l
{
    muladd_helper_mul_c_add_l(sample_type const & factor, sample_type const & offset,
                              sample_type const & offset_slope):
        factor(factor), offset(offset), offset_slope(offset_slope)
    {}

    template <typename arg_sample_type>
    arg_sample_type operator()(arg_sample_type sample)
    {
        arg_sample_type ret = sample * factor + offset;
        offset += offset_slope;
        return ret;
    }

    sample_type factor, offset, offset_slope;
};


/* muladd helper, constant factor, offset vector */
template <typename sample_type, typename input_buffer_type = sample_type*>
struct muladd_helper_mul_c_add_v
{
    muladd_helper_mul_c_add_v(sample_type const & factor, input_buffer_type const & offsets):
        factor(factor), offsets( get_samples(offsets) )
    {}

    template <typename arg_sample_type>
    arg_sample_type operator()(arg_sample_type sample)
    {
        return sample * factor + *offsets++;
    }

    sample_type factor;
    const sample_type * __restrict__ offsets;
};



/* muladd helper, linear changing factor, constant offset */
template <typename sample_type>
struct muladd_helper_mul_l_add_c
{
    muladd_helper_mul_l_add_c(sample_type const & factor, sample_type const & factor_slope,
                              sample_type const & offset):
        factor(factor), factor_slope(factor_slope), offset(offset)
    {}

    template <typename arg_sample_type>
    arg_sample_type operator()(arg_sample_type sample)
    {
        arg_sample_type ret = sample * factor + offset;
        factor += factor_slope;
        return ret;
    }

    sample_type factor, factor_slope, offset;
};

/* muladd helper, linear changing factor, linear changing offset */
template <typename sample_type>
struct muladd_helper_mul_l_add_l
{
    muladd_helper_mul_l_add_l(sample_type const & factor, sample_type const & factor_slope,
                              sample_type const & offset, sample_type const & offset_slope):
        factor(factor), factor_slope(factor_slope), offset(offset), offset_slope(offset_slope)
    {}

    template <typename arg_sample_type>
    arg_sample_type operator()(arg_sample_type sample)
    {
        arg_sample_type ret = sample * factor + offset;
        offset += offset_slope;
        factor += factor_slope;
        return ret;
    }

    sample_type factor, factor_slope, offset, offset_slope;
};


/* muladd helper, linear changing factor, offset vector */
template <typename sample_type, typename input_buffer_type = sample_type*>
struct muladd_helper_mul_l_add_v
{
    muladd_helper_mul_l_add_v(sample_type const & factor, sample_type const & factor_slope,
                              input_buffer_type const & offsets):
        factor(factor), factor_slope(factor_slope), offsets( get_samples(offsets) )
    {}

    template <typename arg_sample_type>
    arg_sample_type operator()(arg_sample_type sample)
    {
        arg_sample_type ret = sample * factor + *offsets++;
        factor += factor_slope;
        return ret;
    }

    sample_type factor, factor_slope;
    const sample_type * __restrict__ offsets;
};




/* muladd helper, factor vector, constant offset */
template <typename sample_type, typename input_buffer_type = sample_type*>
struct muladd_helper_mul_v_add_c
{
    muladd_helper_mul_v_add_c(input_buffer_type const & factors, sample_type const & offset):
        factors( get_samples(factors) ), offset(offset)
    {}

    template <typename arg_sample_type>
    arg_sample_type operator()(arg_sample_type sample)
    {
        return sample * *factors++ + offset;
    }

    const sample_type * __restrict__ factors;
    sample_type offset;
};

/* muladd helper, factor vector, linear changing offset */
template <typename sample_type, typename input_buffer_type = sample_type*>
struct muladd_helper_mul_v_add_l
{
    muladd_helper_mul_v_add_l(input_buffer_type const & factors, sample_type const & offset,
                              sample_type const & offset_slope):
        factors( get_samples(factors) ), offset(offset), offset_slope(offset_slope)
    {}

    template <typename arg_sample_type>
    arg_sample_type operator()(arg_sample_type sample)
    {
        arg_sample_type ret = sample * *factors++ + offset;
        offset += offset_slope;
        return ret;
    }

    const sample_type * __restrict__ factors;
    sample_type offset, offset_slope;
};


/* muladd helper, factor vector, offset vector */
template <typename sample_type, typename input_buffer_type = sample_type*>
struct muladd_helper_mul_v_add_v
{
    /* offsets and factors should not point to the same memory region */
    muladd_helper_mul_v_add_v(input_buffer_type const & factors, input_buffer_type const & offsets):
        factors( get_samples(factors) ), offsets( get_samples(offsets) )
    {}

    template <typename arg_sample_type>
    arg_sample_type operator()(arg_sample_type sample)
    {
        return sample * *factors++ + *offsets++;
    }

    const sample_type * __restrict__ factors;
    const sample_type * __restrict__ offsets;
};

} /* namespace detail */
} /* namespace nova */



/* defines a perform function, which wraps a nop muladd wrapper */
#define DEFINE_PERFORM_WRAPPER_0_1                                  \
    template <typename output_buffer_type, typename sample_count_t> \
    inline void perform(output_buffer_type & out, sample_count_t n) \
    {                                                               \
        ::nova::detail::muladd_helper_nop<sample_type> ma;          \
        perform(out, n, ma);                                        \
    }

#define DEFINE_PERFORM_WRAPPER_1_1                                      \
    template <typename input_buffer_type, typename output_buffer_type, typename sample_count_t> \
    inline void perform(input_buffer_type const & in, output_buffer_type & out, sample_count_t n) \
    {                                                                   \
        ::nova::detail::muladd_helper_nop<sample_type> ma;              \
        perform(in, out, n, ma);                                        \
    }

#define DEFINE_PERFORM_WRAPPER_2_1                                      \
    template <typename input_buffer_type, typename output_buffer_type, typename sample_count_t> \
    inline void perform(input_buffer_type const & in1, input_buffer_type const & in2, \
                        output_buffer_type & out, sample_count_t n)     \
    {                                                                   \
        ::nova::detail::muladd_helper_nop<sample_type> ma;              \
        perform(in1, in2, out, n, ma);                                  \
    }

#define DEFINE_PERFORM_WRAPPER_3_1                                      \
    template <typename input_buffer_type, typename output_buffer_type, typename sample_count_t> \
    inline void perform(input_buffer_type const & in1, input_buffer_type const & in2, \
                        input_buffer_type const & in3,                  \
                        output_buffer_type & out, sample_count_t n)     \
    {                                                                   \
        ::nova::detail::muladd_helper_nop<sample_type> ma;              \
        perform(in1, in2, in3, out, n, ma);                             \
    }

#define DEFINE_PERFORM_WRAPPER_1_4                                      \
    template <typename input_buffer_type, typename output_buffer_type, typename sample_count_t> \
    inline void perform(input_buffer_type const & in, output_buffer_type & out1, output_buffer_type & out2, \
                        output_buffer_type & out3, output_buffer_type & out4, sample_count_t n) \
    {                                                                   \
        ::nova::detail::muladd_helper_nop<sample_type> ma;              \
        perform(in, out1, out2, out3, out4, n, ma);                     \
    }



#endif /* DSP_MULADD_HELPERS_HPP */
