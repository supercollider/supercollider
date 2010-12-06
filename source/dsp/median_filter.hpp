//  median filter classes
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


#ifndef DSP_MEDIAN_FILTER_HPP
#define DSP_MEDIAN_FILTER_HPP

#include "cache_aligned_array.hpp"
#include "../utilities/branch_hints.hpp"
#include <boost/foreach.hpp>

#include <algorithm>
#include <cmath>

namespace nova
{



/** \brief templated median filter class
 *
 *  - using an insertion sort algorithm
 **/
template <typename sample_type,
          unsigned int maxsize>
class static_median_filter
{
    typedef std::size_t size_t;

    struct element
    {
        element(void):
            value(0), index(0)
        {}

        element(element const & rhs):
            value(rhs.value), index(rhs.index)
        {}

        element(sample_type f, size_t i):
            value(f), index(i)
        {}

        bool operator<(element const & rhs) const
        {
            return value < rhs.value;
        }

        void set (sample_type val, size_t i)
        {
            value = val;
            index = i;
        }

        sample_type value;
        size_t index;
    };

public:
    static_median_filter(void):
        oldest(0)
    {
        for (int i = 0; i != maxsize; ++i)
            vec[i].set(0.f, i);
    }

    sample_type step(sample_type in)
    {
        unsigned int index = index_of_oldest();

        if (in < vec[index].value)
        {
            while ( likely(index > 0) and likely(vec[index-1].value > in ))
            {
                /* shift elements to the right */
                vec[index] = vec[index-1];
                index -= 1;
            }
        }
        else
        {
            while ( likely((index+1) < vec.size()) and likely(vec[index+1].value < in ))
            {
                /* shift elements to the left */
                vec[index] = vec[index+1];
                index += 1;
            }
        }

        vec[index].set(in, oldest);
        oldest += 1;

        if (unlikely (oldest >= vec.size()))
            oldest -= vec.size();

        const size_t median = vec.size() >> 1;
        return vec[median].value;
    }

private:
    size_t index_of_oldest(void)
    {
        for (size_t i = 0; i != vec.size(); ++i)
            if (unlikely(vec[i].index == oldest))
                return i;
        assert(false);
        return 0;
    }

    aligned_array<element, maxsize> vec;
    size_t oldest;
};


/** \brief templated median filter class
 *
 *  - using an insertion sort algorithm
 **/
template <typename sample_type,
          unsigned int maxsize>
class median_filter
{
    typedef std::size_t size_t;


    struct element
    {
        element(void):
            value(0), index(0)
        {}

        element(element const & rhs):
            value(rhs.value), index(rhs.index)
        {}

        element(sample_type f, size_t i):
            value(f), index(i)
        {}

        bool operator<(element const & rhs) const
        {
            return value < rhs.value;
        }

        void set (sample_type val, size_t i)
        {
            value = val;
            index = i;
        }

        sample_type value;
        size_t index;
    };

    struct cmp_index
    {
        bool operator()(element const & lhs, element const & rhs) const
        {
            return lhs.index > rhs.index;
        }
    };

public:
    median_filter(size_t size):
        oldest(0), vecsize(size)
    {
        assert (size != 0);
        assert (size <= maxsize);

        for (uint i = 0; i != size; ++i)
            vec[i].set(0.f, i);
    }

    void resize(size_t new_size)
    {
        using std::sort;

        assert (new_size != 0);
        assert (new_size <= maxsize);

        if (unlikely (new_size == vecsize))
            return;

        const size_t size = vecsize;
        if (new_size < size)
        {
            /* shrink region */

            BOOST_FOREACH(element & e, vec)
            {
                if (e.index < oldest)
                    e.index += maxsize;
            }

            /* sort old region by index */
            sort(vec.begin(), vec.begin() + size, cmp_index());

            const size_t diff = size - new_size;

            /* clear unnecessary region */
            for (size_t i = 0; i != diff; ++i)
                vec[new_size+i].set(0.f, 0);

            /* reset settings */
            vecsize = new_size;
            oldest = 0;

            for (size_t i = 0; i != vecsize; ++i)
                vec[i].index = vecsize-1-i;
        }
        else
        {
            /* fill buffer */
            const size_t diff = new_size - size;
            for (size_t i = 0; i != diff; ++i)
                vec[vecsize+i].set(0.f, vecsize+i);

            vecsize = new_size;

            /* index of oldest doesn't need to be changed */
        }

        /* vec needs to be sorted for the insertion sort algorithm  */
        sort(vec.begin(), vec.begin() + vecsize);
        assert(vecsize == new_size);
    }

    sample_type step(sample_type in)
    {
        uint index = index_of_oldest();

        if (in < vec[index].value)
        {
            while ( likely(index > 0) and likely(vec[index-1].value > in ))
            {
                /* shift elements to the right */
                vec[index] = vec[index-1];
                index -= 1;
            }
        }
        else
        {
            while ( likely((index+1) < vecsize) and likely(vec[index+1].value < in ))
            {
                /* shift elements to the left */
                vec[index] = vec[index+1];
                index += 1;
            }
        }

        vec[index].set(in, oldest);
        oldest += 1;

        if (unlikely (oldest >= vecsize))
            oldest -= vecsize;

        const size_t median = vecsize >> 1;
        return vec[median].value;
    }

    template <typename input_buffer_type, typename output_buffer_type>
    inline void perform(input_buffer_type const & in, output_buffer_type & out, uint n)
    {
        assert(n > 0);
        const sample_type * in_sample = get_samples(in);
        sample_type * out_sample = get_samples(out);
        do
        {
            *out_sample++ = step(*in_sample++);

            --n;
        }
        while (n);
    }

private:
    size_t index_of_oldest(void)
    {
        for (size_t i = 0; i != vecsize; ++i)
            if (unlikely(vec[i].index == oldest))
                return i;
        assert(false);
        return 0;
    }

    aligned_array<element, maxsize> vec;
    size_t oldest;
    size_t vecsize;
};

/** \brief templated fractional median filter class
 *
 **/
template <typename sample_type,
          unsigned int maxsize,
          typename phase_type = sample_type,
          bool interpolating_median = true>
class fractional_median_filter
{
    typedef std::size_t size_t;

    median_filter<sample_type, maxsize+1> median_low, median_high;
    phase_type size;
    sample_type low_factor;
    sample_type high_factor;

public:
    fractional_median_filter(phase_type size_arg = maxsize):
        median_low(1), median_high(2)
    {
        assert (size_arg <= maxsize);
        resize(size_arg);
    }

    void resize(phase_type new_size)
    {
        assert (new_size >= 1);
        assert (new_size <= maxsize);
        int low = std::floor(new_size);
        int high = low + 1;

        size = new_size;
        sample_type fract = 1 - (new_size - sample_type(low));
        assert (fract >= 0);

        low_factor = fract;
        high_factor = 1 - fract;

        median_low.resize(low);
        median_high.resize(high);
    }

    sample_type step(sample_type in)
    {
        /* write sample to ringbuffer */
        sample_type low = median_low.step(in);
        sample_type high = median_high.step(in);

        return low_factor * low + high_factor * high;
    }

    template <typename input_buffer_type, typename output_buffer_type>
    inline void perform(input_buffer_type const & in, output_buffer_type & out, uint n)
    {
        assert(n > 0);
        const sample_type * in_sample = get_samples(in);
        sample_type * out_sample = get_samples(out);
        do
        {
            *out_sample++ = step(*in_sample++);

            --n;
        }
        while (n);
    }
};


} /* namespace nova */

#endif /* DSP_MEDIAN_FILTER_HPP */
