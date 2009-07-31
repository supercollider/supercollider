//  $Id$
//  
//  benchmark file for dsp tests
//  Copyright (C) 2005 Tim Blechmann
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


#include "dsp/arithmetic.hpp"

using namespace PNPD;

#include "dsp/simd_sse_mp.hpp"
/* #include "dsp/simd_sse_gcc.hpp" */


extern "C"
void zerotest_test(float *dst)
{
    zerovec_simdi<1024>(dst);
}


int main()
{
/* 	for (int i = 0; i != 10000000; ++i) */
/* 	{ */
/* 		float b1[64]; */
/* 		float b2[64]; */
/* 		float b3[64]; */
/* 		float p = 0; */
/* 		float rmsf = 0; */
		
/* 		zerovec(b1, 64); */
/* 		zerovec_8(b2, 64); */
/* 		zerovec_simd(b3, 64); */

/* 		setvec(b1, 1, 64); */
/* 		setvec_8(b1, 1, 64); */
/* 		setvec_simd(b1, 1, 64); */

/* 		addvec(b3, b1, 64); */
/* 		addvec_8(b3, b1, 64); */
/* 		addvec_simd(b3, b1, 64); */

/* 		times_vec_scalar(b1, b2, 0.2, 64); */
/* 		times_vec_scalar_8(b1, b2, 0.2, 64); */
/* 		times_vec_scalar_simd(b1, b2, 0.2, 64); */

/* 		clip_vec(b1, b1, b2, b3, 64); */
/* 		clip_vec_8(b1, b1, b2, b3, 64); */
/* 		clip_vec_simd(b1, b1, b2, b3, 64); */


		
/* 		peak_rms(b1, 64, p, rmsf); */

/* 		p = 0; */
/* 		rmsf = 0; */
/* 		peak_rms_8(b1, 64, p, rmsf); */

/* 		p = 0; */
/* 		rmsf = 0; */
/* 		peak_rms_simd(b1, 64, p, rmsf); */

/* 		rmsf = 0; */
/* 		rms(b1, 64, rmsf); */
/* 		rms_8(b1, 64, rmsf); */
/* 		rms_simd(b1, 64, rmsf); */

/* 		p = 0; */
/* 		abspeak(b1, 64, p); */
/* 		abspeak_8(b1, 64, p); */
/* 		abspeak_simd(b1, 64, p); */

		
/* 		testcopyvec(b1, b2, 64); */
/* 		testcopyvec_simd(b1, b2, 64); */

/* 		sgn_vec(b1, b2, 64); */
/* 		sgn_vec_simd(b1, b2, 64); */

/* 		rsqrt_vec(b1,b1,64); */
/* 		rsqrt_vec_8(b1,b1,64); */
/* 		rsqrt_vec_simd(b1,b1,64); */
/* 		rsqrt_hp_vec_simd(b1,b1,64); */

/* 		rcp_vec(b1,b1,64); */
/* 		rcp_vec_8(b1,b1,64); */
/* 		rcp_vec_simd(b1,b1,64); */
/* 	} */
}
