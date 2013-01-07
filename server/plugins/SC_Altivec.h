/*
	SuperCollider real time audio synthesis system
    Copyright (c) 2003 James McCartney. All rights reserved.
	http://www.audiosynth.com

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
*/

#ifndef _SC_Altivec_
#define _SC_Altivec_

#if defined(HAS_ALTIVEC) || defined(__ALTIVEC__) && !defined(__APPLE_ALTIVEC__)
# include <altivec.h>
/* From <altivec.h>:
   You are allowed to undef these for C++ compatibility. */
# ifdef bool
#  undef bool
# endif
#endif

#if __VEC__

typedef vector signed int vint32;
typedef vector unsigned int vuint32;
typedef vector float vfloat32;

// Since gcc 3.3 vector initializers are surrounded by brackets. <sk>
#if defined(__GNUC__) && ((__GNUC__ >= 4) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 3)))
# define vinit(x)	{ x, x, x, x }
#else
# define vinit(x)	( x )
#endif

//#define vload(x) (*vtempptr = (x), vec_splat(vtemp,0))
#define define_vtemp vfloat32 vtemp; float *vtempptr = (float*)&vtemp;
#define define_vones vfloat32 vones = vec_ctf(vec_splat_s32(1),0);
#define define_vzero vfloat32 vzero = (vfloat32)vec_splat_s32(0);
#define vi0123 (vec_unpackh(vec_unpackh((vector signed char)vec_lvsl(0,(int*)0))))
#define v0123 (vec_ctf(vec_unpackh(vec_unpackh((vector signed char)vec_lvsl(0,(int*)0))), 0))
#define v0123_4ths (vec_ctf(vec_unpackh(vec_unpackh((vector signed char)vec_lvsl(0,(int*)0))), 2))
#define vstart(x, vslope) (vec_madd(vslope, v0123_4ths, vload(x)))

inline vint32 vec_not(vint32 arg)
{
	return vec_nor(arg, arg);
}

inline vfloat32 vec_not(vfloat32 arg)
{
	return vec_nor(arg, arg);
}

inline vfloat32 vec_mul(vfloat32 a, vfloat32 b)
{
	define_vzero;
	return vec_madd(a, b, vzero);
}

#define vec_2sComp(x) (vec_sub(vec_sub (x, x), x))

#define USEVEC (ft->mAltivecAvailable && !(BUFLENGTH & 3))

typedef union vec_union {
        int32		i[4];
        float32		f[4];
        vint32		vi;
        vfloat32	vf;
} vec_union;

inline vfloat32 vload( float f )
{
        vec_union temp;
        temp.f[0] = f;
        return vec_splat( vec_lde( 0, temp.f ), 0 );
}

inline vint32 vload( int32 i )
{
        vec_union temp;
        temp.i[0] = i;
        return vec_splat( vec_lde( 0, temp.i ), 0 );
}

inline vint32 vload( int32 a, int32 b, int32 c, int32 d )
{
        vec_union temp;
        temp.i[0] = a;
        temp.i[1] = b;
        temp.i[2] = c;
        temp.i[3] = d;
        return temp.vi;
}

inline vector float vec_float_1( void )
{
	return vec_ctf( vec_splat_u32(1), 0);
}

inline vector float vec_reciprocal( vector float v )
{
	vector float reciprocal = vec_re( v );
	return vec_madd( reciprocal, vec_nmsub( reciprocal, v, vec_float_1()), reciprocal ); //Newton Rapheson refinement
}


// seed = ((seed & mask) << shift1) ^ (((seed << shift2) ^ seed) >> shift3);

#define define_trshifts \
	vuint32 trmask = ((vuint32)(0xFFFFFFFE,0xFFFFFFF8,0xFFFFFFF0,0)); \
	vuint32 trshift1 = ((vuint32)(12, 14,  7,  0)); \
	vuint32 trshift2 = ((vuint32)(13,  2,  3,  0)); \
	vuint32 trshift3 = ((vuint32)(19, 25, 11,  0));

inline vuint32 trands(vuint32 seed, vuint32 trmask, vuint32 shift1, vuint32 shift2, vuint32 shift3)
{
	return vec_xor(vec_sl(vec_and(seed, trmask),shift1), vec_sr(vec_xor(vec_sl(seed,shift2),seed),shift3));
}

#endif
#endif


