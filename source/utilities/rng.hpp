//  $Id$
//  
//  some random number generator stuff
//  Copyright (C) 2006 Tim Blechmann
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

#ifndef _RNG_HPP
#define _RNG_HPP

#ifdef __SSE2__
#include <emmintrin.h>
#endif

/* the sse2 code is adapted from Kipp Owens, Rajiv Parikh,
   Copyright (c) 2003 Intel Corporation */

namespace nova
{
    class FastRng
    {

    public:
        FastRng(void):
            position(4)
        {
            int seed = 78269186;
#ifdef __SSE2__
            cur_seed = _mm_set_epi32 (seed, seed + 2, seed, seed + 1);
#endif
        }
        
        float GenFloat(void)
        {
            Step();
            return ((float)((current & 0x7fffffff) - 0x40000000)) * (float)(1.0 / 0x40000000);
        }

        int GenInt(void)
        {
            Step();
            return current;
        }

    private:
        void Step(void)
        {
#ifndef __SSE2__
            current = 214013 * current + 2531011;
            /* #elif defined(PD) */
            /*             current = 435898247 * current + 382842987; */
#else /* defined(__SSE2__) */
            if (position != 4)
            {
                current = sse_currents[position];
                position++;
            }
            else
            {
                __m128i cur_seed_split;
/*                 __m128i multiplier; */
/*                 __m128i adder; */
/*                 __m128i mod_mask; */
/*                 __m128i sra_mask; */
/*                 __m128i sseresult; */


/*                 static const unsigned int mult[4] = */
/*                     { 214013, 17405, 214013, 69069 }; */
/*                 static const unsigned int gadd[4] = */
/*                     { 2531011, 10395331, 13737667, 1 }; */
/*                 static const unsigned int mask[4] = */
/*                     { 0xFFFFFFFF, 0, 0xFFFFFFFF, 0 }; */
/*                 static const unsigned int masklo[4] = */
/*                     { 0x00007FFF, 0x00007FFF, 0x00007FFF, 0x00007FFF }; */

/*                 adder = _mm_loadu_si128 ((__m128i *) gadd); */
/*                 multiplier = _mm_loadu_si128 ((__m128i *) mult); */
/*                 mod_mask = _mm_loadu_si128 ((__m128i *) mask); */
/*                 sra_mask = _mm_loadu_si128 ((__m128i *) masklo); */

                static const __m128i multi = _mm_set_epi32(214013, 17405, 214013, 69069);
                static const __m128i adder = _mm_set_epi32(2531011, 10395331, 13737667, 1);
                static const __m128i mask = _mm_set_epi32(0xFFFFFFFF, 0, 0xFFFFFFFF, 0);
                static const __m128i masklo = _mm_set_epi32(0x00007FFF, 0x00007FFF, 0x00007FFF, 0x00007FFF);
                
                __m128i multiplier = multi;

                cur_seed_split = _mm_shuffle_epi32 (cur_seed, _MM_SHUFFLE (2, 3, 0, 1));

    
                cur_seed = _mm_mul_epu32 (cur_seed, multiplier);
                multiplier = _mm_shuffle_epi32 (multiplier, _MM_SHUFFLE (2, 3, 0, 1));
                cur_seed_split = _mm_mul_epu32 (cur_seed_split, multiplier);

/*                 cur_seed = _mm_and_si128 (cur_seed, mod_mask); */
/*                 cur_seed_split = _mm_and_si128 (cur_seed_split, mod_mask); */

                cur_seed_split = _mm_shuffle_epi32 (cur_seed_split, _MM_SHUFFLE (2, 3, 0, 1));
                cur_seed = _mm_or_si128 (cur_seed, cur_seed_split);

                cur_seed = _mm_add_epi32 (cur_seed, adder);

                
                _mm_storeu_si128 ((__m128i *) sse_currents, cur_seed);

                current = sse_currents[0];
                position = 1;
            }
#endif
        }

        int current;
        unsigned short position;
#ifdef __SSE2__
        unsigned int sse_currents[4];
        __m128i cur_seed;
#endif
    };
};

#endif /* _RNG_HPP */
