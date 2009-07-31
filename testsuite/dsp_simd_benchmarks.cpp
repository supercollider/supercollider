//  $Id$
//
//  simd benchmarks
//
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

typedef float sample;
typedef unsigned int uint;

#include "dsp/arithmetic_wrapper.hpp"
#include "boost/random.hpp"

typedef boost::mt19937 rand_gen_t;
rand_gen_t rand_gen;
boost::uniform_real<sample> dist;
boost::variate_generator<rand_gen_t, boost::uniform_real<sample> > rng(rand_gen, dist);

using namespace nova;

sample * vec1;
sample * vec2;
sample * vec3;
sample * vec4;

const uint iterations = 1000000000;

#define noinline  __attribute__ ((noinline))

void allocate(uint vector_size)
{
    uint malloc_memory_alignment = 64;
    posix_memalign((void**)&vec1, malloc_memory_alignment, vector_size * sizeof(sample));
    posix_memalign((void**)&vec2, malloc_memory_alignment, vector_size * sizeof(sample));
    posix_memalign((void**)&vec3, malloc_memory_alignment, vector_size * sizeof(sample));
    posix_memalign((void**)&vec4, malloc_memory_alignment, vector_size * sizeof(sample));
}

void deallocate(void)
{
    free(vec1);
    free(vec2);
    free(vec3);
    free(vec4);
}

/* plus benchmarks (binary arithmetics) */
namespace plus
{
template<int d>
noinline void vec_simple(uint vector_size)
{
    plus_wrapper<d>::vec(vec1, vec2, vec3, vector_size);
}

template<int d>
noinline void vec_simd(uint vector_size)
{
    plus_wrapper<d>::vec_8(vec1, vec2, vec3, vector_size);
}

template<int d>
noinline void vec_simple_8(uint vector_size)
{
    plus_vec_8(vec1, vec2, vec3, vector_size);
}

template <uint vs>
noinline void vec_simd_mp(void)
{
    plus_wrapper<vs>::vec(vec1, vec2, vec3);
}

template<int d>
noinline void simple(sample arg, uint vector_size)
{
    plus_wrapper<d>::scalar(vec1, vec2, arg, vector_size);
}

template<int d>
noinline void simd(sample arg, uint vector_size)
{
    plus_wrapper<d>::scalar_8(vec1, vec2, arg, vector_size);
}

template<int d>
noinline void simple_8(sample arg, uint vector_size)
{
    plus_vec_8(vec1, vec2, arg, vector_size);
}


template <uint vs>
noinline void simd_mp(sample arg)
{
    plus_wrapper<vs>::scalar(vec1, vec2, arg);
}

template<int vector_size>
noinline void bench(void)
{
    sample arg = 1.0;
    for (int i = 0; i != iterations/vector_size; ++i)
    {
        vec_simple<vector_size>(vector_size);
        vec_simple_8<vector_size>(vector_size);
        vec_simd<vector_size>(vector_size);
        vec_simd_mp<vector_size>();

        simple<vector_size>(arg, vector_size);
        simple_8<vector_size>(arg, vector_size);
        simd<vector_size>(arg, vector_size);
        simd_mp<vector_size>(arg);
    }
}
}

namespace cpy
{
    template<int d>
    noinline void simple(uint vector_size)
    {
        copyvec(vec1, vec2, vector_size);
    }

    template<int d>
    noinline void simple_8(uint vector_size)
    {
        copyvec_8(vec1, vec2, vector_size);
    }

    template<int d>
    noinline void simd(uint vector_size)
    {
        copyvec_simd(vec1, vec2, vector_size);
    }

    template <uint vs>
    noinline void simd_mp(void)
    {
        copyvec_simd_mp<vs>(vec1, vec2);
    }

    template<int vector_size>
    noinline void bench(void)
    {
        for (int i = 0; i != iterations/vector_size; ++i)
        {
            simple<vector_size>(vector_size);
            simple_8<vector_size>(vector_size);
            simd<vector_size>(vector_size);
            simd_mp<vector_size>();
        }
    }
}


/* minimum benchmarks (branching algorithm) */
namespace minimum
{
    template<int d>
    noinline void vec_simple(uint vector_size)
    {
        min_wrapper<d>::vec(vec1, vec2, vec3, vector_size);
    }

    template<int d>
    noinline void vec_simple_8(uint vector_size)
    {
        min_vec_8(vec1, vec2, vec3, vector_size);
    }

    template<int d>
    noinline void vec_simd(uint vector_size)
    {
        min_wrapper<d>::vec_8(vec1, vec2, vec3, vector_size);
    }

    template <uint vs>
    noinline void vec_simd_mp(void)
    {
        min_wrapper<vs>::vec(vec1, vec2, vec3);
    }

    template<int d>
    noinline void simple(sample arg, uint vector_size)
    {
        min_wrapper<d>::scalar(vec1, vec2, arg, vector_size);
    }

    template<int d>
    noinline void simple_8(sample arg, uint vector_size)
    {
        min_vec_8(vec1, vec2, arg, vector_size);
    }

    template<int d>
    noinline void simd(sample arg, uint vector_size)
    {
        min_wrapper<d>::scalar_8(vec1, vec2, arg, vector_size);
    }

    template <uint vs>
    noinline void simd_mp(sample arg)
    {
        min_wrapper<vs>::scalar(vec1, vec2, arg);
    }

    template<int vector_size>
    noinline void bench(void)
    {
        sample arg = 1.0;
        for (int i = 0; i != iterations/vector_size; ++i)
        {
            vec_simple<vector_size>(vector_size);
            vec_simple_8<vector_size>(vector_size);
            vec_simd<vector_size>(vector_size);
            vec_simd_mp<vector_size>();

            simple<vector_size>(arg, vector_size);
            simple_8<vector_size>(arg, vector_size);
            simd<vector_size>(arg, vector_size);
            simd_mp<vector_size>(arg);
        }
    }
}

/* abs benchmarks (simple bitmasking algorithm) */
namespace absolut
{
    template<int d>
    noinline void simple(uint vector_size)
    {
        abs_wrapper<d>::vec(vec1, vec2, vector_size);
    }

    template<int d>
    noinline void simple_8(uint vector_size)
    {
        abs_vec_8(vec1, vec2, vector_size);
    }

    template<int d>
    noinline void simd(uint vector_size)
    {
        abs_wrapper<d>::vec_8(vec1, vec2, vector_size);
    }

    template <uint vs>
    noinline void simd_mp(void)
    {
        abs_wrapper<vs>::vec(vec1, vec2);
    }

    template<int vector_size>
    noinline void bench(void)
    {
        for (int i = 0; i != iterations/vector_size; ++i)
        {
            simple<vector_size>(vector_size);
            simple_8<vector_size>(vector_size);
            simd<vector_size>(vector_size);
            simd_mp<vector_size>();
        }
    }
}

/* sgn benchmarks (complex bitmasking algorithm) */
namespace sign
{
    template<int d>
    noinline void simple(uint vector_size)
    {
        sgn_wrapper<d>::vec(vec1, vec2, vector_size);
    }

    template<int d>
    noinline void simple_8(uint vector_size)
    {
        sgn_vec_8(vec1, vec2, vector_size);
    }

    template<int d>
    noinline void simd(uint vector_size)
    {
        sgn_wrapper<d>::vec_8(vec1, vec2, vector_size);
    }

    template <uint vs>
    noinline void simd_mp(void)
    {
        sgn_wrapper<vs>::vec(vec1, vec2);
    }

    template<int vector_size>
    noinline void bench(void)
    {
        for (int i = 0; i != iterations/vector_size; ++i)
        {
            simple<vector_size>(vector_size);
            simple_8<vector_size>(vector_size);
            simd<vector_size>(vector_size);
            simd_mp<vector_size>();
        }
    }
}

/* undenormalize benchmarks (really complex bitmasking algorithm) */
namespace denormal_bashing
{
    template<int d>
    noinline void simple(uint vector_size)
    {
        testcopyvec(vec1, vec2, vector_size);
    }

    template<int d>
    noinline void simd(uint vector_size)
    {
        testcopyvec_simd(vec1, vec2, vector_size);
    }

    template <uint vs>
    noinline void simd_mp(void)
    {
        testcopyvec_simd_mp<vs>(vec1, vec2);
    }

    template<int vector_size>
    noinline void bench(void)
    {
        for (int i = 0; i != iterations/vector_size; ++i)
        {
            simple<vector_size>(vector_size);
            simd<vector_size>(vector_size);
            simd_mp<vector_size>();
        }
    }
}


/* sqrt benchmarks (built-in math operation) */
namespace sqrtt
{
    template<int d>
    noinline void simple(uint vector_size)
    {
        sqrt_vec(vec1, vec2, vector_size);
    }

    template<int d>
    noinline void simple_8(uint vector_size)
    {
        sqrt_vec_8(vec1, vec2, vector_size);
    }

    template<int d>
    noinline void simd(uint vector_size)
    {
        sqrt_vec_simd(vec1, vec2, vector_size);
    }

    template <uint vs>
    noinline void simd_mp(void)
    {
        sqrt_vec_simd_mp<vs>(vec1, vec2);
    }

    template<int vector_size>
    noinline void bench(void)
    {
        for (int i = 0; i != iterations/vector_size; ++i)
        {
            simple<vector_size>(vector_size);
            simple_8<vector_size>(vector_size);
            simd<vector_size>(vector_size);
            simd_mp<vector_size>();
        }
    }
}

/* rcp benchmarks (built-in math operation) */
namespace recip
{
    template<int d>
    noinline void simple(uint vector_size)
    {
        rcp_vec(vec1, vec2, vector_size);
    }

    template<int d>
    noinline void simple_8(uint vector_size)
    {
        rcp_vec_8(vec1, vec2, vector_size);
    }

    template<int d>
    noinline void simd(uint vector_size)
    {
        rcp_vec_simd(vec1, vec2, vector_size);
    }

    template <uint vs>
    noinline void simd_mp(void)
    {
        rcp_vec_simd_mp<vs>(vec1, vec2);
    }

    template<int vector_size>
    noinline void bench(void)
    {
        for (int i = 0; i != iterations/vector_size; ++i)
        {
            simple<vector_size>(vector_size);
            simple_8<vector_size>(vector_size);
            simd<vector_size>(vector_size);
            simd_mp<vector_size>();
        }
    }
}


/* accumulating benchmarks */
namespace accum_peak
{
    template<int d>
    sample noinline simple(uint vector_size)
    {
        sample peak = 0;
        abspeak(vec1, peak, vector_size);
        return peak;
    }

    template<int d>
    sample noinline simple_8(uint vector_size)
    {
        sample peak = 0;
        abspeak_8(vec1, peak, vector_size);
        return peak;
    }

    template<int d>
    sample noinline simd(uint vector_size)
    {
        sample peak = 0;
        abspeak_simd(vec1, peak, vector_size);
        return peak;
    }

    template <uint vs>
    sample noinline simd_mp(void)
    {
        sample peak = 0;
        abspeak_simd_mp<vs>(vec1, peak);
        return peak;
    }

    template<int vector_size>
    noinline void bench(void)
    {
        for (int i = 0; i != iterations/vector_size; ++i)
        {
            simple<vector_size>(vector_size);
            simple_8<vector_size>(vector_size);
            simd<vector_size>(vector_size);
            simd_mp<vector_size>();
        }
    }
}

namespace accum_rms
{
    template<int d>
    noinline sample simple(uint vector_size)
    {
        sample peak = 0;
        rms(vec1, peak, vector_size);
        return peak;
    }

    template<int d>
    noinline sample simple_8(uint vector_size)
    {
        sample peak = 0;
        rms_8(vec1, peak, vector_size);
        return peak;
    }

    template<int d>
    noinline sample simd(uint vector_size)
    {
        sample peak = 0;
        rms_simd(vec1, peak, vector_size);
        return peak;
    }

    template <uint vs>
    noinline sample simd_mp(void)
    {
        sample peak = 0;
        rms_simd_mp<vs>(vec1, peak);
        return peak;
    }

    template<int vector_size>
    noinline void bench(void)
    {
        for (int i = 0; i != iterations/vector_size; ++i)
        {
            simple<vector_size>(vector_size);
            simple_8<vector_size>(vector_size);
            simd<vector_size>(vector_size);
            simd_mp<vector_size>();
        }
    }
}

namespace accum_peakrms
{
    template<int d>
    noinline sample simple(uint vector_size)
    {
        sample rms = 0;
        sample peak = 0;
        peak_rms(vec1, peak, rms, vector_size);
        return peak + rms;
    }

    template<int d>
    noinline sample simple_8(uint vector_size)
    {
        sample rms = 0;
        sample peak = 0;
        peak_rms_8(vec1, peak, rms, vector_size);
        return peak + rms;
    }

    template<int d>
    noinline sample simd(uint vector_size)
    {
        sample peak = 0;
        sample rms = 0;
        peak_rms_simd(vec1, peak, rms, vector_size);
        return peak + rms;
    }

    template <uint vs>
    noinline sample simd_mp(void)
    {
        sample peak = 0;
        sample rms = 0;
        peak_rms_simd_mp<vs>(vec1, peak, rms);
        return peak + rms;
    }

    template<int vector_size>
    noinline void bench(void)
    {
        for (int i = 0; i != iterations/vector_size; ++i)
        {
            simple<vector_size>(vector_size);
            simple_8<vector_size>(vector_size);
            simd<vector_size>(vector_size);
            simd_mp<vector_size>();
        }
    }
}



#if 0
#define RUN_BENCHES(NAMESPACE)\
NAMESPACE::bench<64>();

int main(void)
{
    uint vec_sz = 4096;

    allocate(vec_sz);

    RUN_BENCHES(cpy);
    RUN_BENCHES(plus);
    RUN_BENCHES(minimum);
    RUN_BENCHES(absolut);
    RUN_BENCHES(sign);
    RUN_BENCHES(denormal_bashing);
    RUN_BENCHES(sqrtt);

    for (int i = 0; i != 4096; ++i)
        vec1[i] = rng();

    RUN_BENCHES(accum_peak);
    RUN_BENCHES(accum_rms);
    RUN_BENCHES(accum_peakrms);
    RUN_BENCHES(recip);

    deallocate();
}
#else

#define RUN_BENCHES(NAMESPACE)\
NAMESPACE::bench<16>();\
NAMESPACE::bench<32>();\
NAMESPACE::bench<64>();\
NAMESPACE::bench<128>();\
NAMESPACE::bench<256>();\
NAMESPACE::bench<512>();\
NAMESPACE::bench<1024>();

int main(void)
{
    uint vec_sz = 4096;

    allocate(vec_sz);

    RUN_BENCHES(plus);

    deallocate();
}
#endif

