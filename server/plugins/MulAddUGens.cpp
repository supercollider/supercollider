/*
	SuperCollider real time audio synthesis system
    Copyright (c) 2002 James McCartney. All rights reserved.
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


#include "SC_PlugIn.hpp"



#include "simd_memory.hpp"
#include "simd_binary_arithmetic.hpp"
#include "simd_ternary_arithmetic.hpp"
#include "simd_mix.hpp"

using nova::slope_argument;

#include "function_attributes.h"

static InterfaceTable *ft;

struct SIMD_Unit:
	SCUnit
{
	bool canUseSIMD (void) const
	{
		return (mBufLength & (nova::vec< float >::objects_per_cacheline - 1)) == 0;
	}

	template <typename UnitType, void (UnitType::*VectorCalcFunc)(int), void (UnitType::*ScalarCalcFunc)(int)>
	void set_vector_calc_function(void)
	{
		if (canUseSIMD())
			SCUnit::set_vector_calc_function<UnitType, VectorCalcFunc, ScalarCalcFunc>();
		else
			SCUnit::set_calc_function<UnitType, ScalarCalcFunc>();
	}
};

namespace {

struct MulAdd:
	SIMD_Unit
{
	enum { scalar, unrolled, unrolled_64};

	float mMulPrev, mAddPrev;

#define MULADD_CALCFUNC(METHOD_NAME)													\
	do {																				\
		if (bufferSize() == 64) {														\
			set_vector_calc_function<MulAdd, 											\
				&MulAdd::METHOD_NAME<unrolled_64>, &MulAdd::METHOD_NAME<scalar> > ();	\
		} else {																		\
			set_vector_calc_function<MulAdd, 											\
				&MulAdd::METHOD_NAME<unrolled>, &MulAdd::METHOD_NAME<scalar> > ();		\
		}																				\
		return;																			\
	} while (0)

	MulAdd(void)
	{
		mMulPrev = in0(1);
		mAddPrev = in0(2);

		if (mCalcRate != calc_FullRate) {
			set_calc_function<MulAdd, &MulAdd::next_scalar>();
			return;
		}

		assert(inRate(0) == calc_FullRate);

		switch (inRate(1)) {
		case calc_FullRate:
			switch (inRate(2)) {
			case calc_FullRate:
				MULADD_CALCFUNC(next_aa);

			case calc_BufRate:
				MULADD_CALCFUNC(next_ak);

			case calc_ScalarRate:
				if (mAddPrev == 0.f)
					MULADD_CALCFUNC(next_a0);
				else
					MULADD_CALCFUNC(next_ai);

			default:
				assert(false);
			}

		case calc_BufRate:
			switch (inRate(2)) {
			case calc_FullRate:
				MULADD_CALCFUNC(next_ka);

			case calc_BufRate:
				MULADD_CALCFUNC(next_kk);

			case calc_ScalarRate:
				if (mAddPrev == 0.f)
					MULADD_CALCFUNC(next_k0);
				else
					MULADD_CALCFUNC(next_ki);

			default:
				assert(false);
			}

		case calc_ScalarRate:
			switch (inRate(2)) {
			case calc_FullRate:
				if (mMulPrev == 1.0)
					MULADD_CALCFUNC(next_1a);
				else if (mMulPrev == 0.f)
					MULADD_CALCFUNC(next_0a);
				else
					MULADD_CALCFUNC(next_ia);

			case calc_BufRate:
				if (mMulPrev == 1.0)
					MULADD_CALCFUNC(next_1k);
				else if (mMulPrev == 0.f)
					MULADD_CALCFUNC(next_0k);
				else
					MULADD_CALCFUNC(next_ik);

			case calc_ScalarRate:
				if (mMulPrev == 1.0) {
					if (mAddPrev == 0)
						MULADD_CALCFUNC(next_10);
					else
						MULADD_CALCFUNC(next_1i);
				}

				else if (mMulPrev == 0.f) {
					if (mAddPrev == 0.f)
						MULADD_CALCFUNC(next_00);
					else
						MULADD_CALCFUNC(next_0i);
				}
				else {
					if (mAddPrev == 0.f)
						MULADD_CALCFUNC(next_i0);
					else
						MULADD_CALCFUNC(next_ii);
				}

			default:
				assert(false);
			}

		default:
			assert(false);
		}
	}

	inline bool mulChanged(void) const
	{
		return in0(1) != mMulPrev;
	}

	inline bool addChanged(void) const
	{
		return in0(2) != mAddPrev;
	}

#if __cplusplus < 199711L
	nova::detail::scalar_ramp_argument<float> mulSlope(void)
#else
	decltype(nova::slope_argument(0.f, 0.f)) mulSlope(void)
#endif
	{
		float nextMul = in0(1);
		float currentMul = mMulPrev;
		float slope = calcSlope(nextMul, currentMul);
		mMulPrev = nextMul;
		return slope_argument(currentMul, slope);
	}

#if __cplusplus < 199711L
	nova::detail::scalar_ramp_argument<float> addSlope(void)
#else
	decltype(nova::slope_argument(0.f, 0.f)) addSlope(void)
#endif
	{
		float nextAdd = in0(2);
		float currentAdd = mAddPrev;
		float slope = calcSlope(nextAdd, currentAdd);
		mAddPrev = nextAdd;
		return slope_argument(currentAdd, slope);
	}

	template <int type, typename Arg1, typename Arg2, typename Arg3>
	static void muladd(float * out, Arg1 const & arg1, Arg2 const & arg2, Arg3 const & arg3, int inNumSamples)
	{
		if (type == scalar)
			nova::muladd_vec(out, arg1, arg2, arg3, inNumSamples);
		if (type == unrolled)
			nova::muladd_vec_simd(out, arg1, arg2, arg3, inNumSamples);
		if (type == unrolled_64)
			nova::muladd_vec_simd<64>(out, arg1, arg2, arg3);
	}

	template <int type, typename Arg1, typename Arg2>
	static void plus_vec(float * out, Arg1 const & arg1, Arg2 const & arg2, int inNumSamples)
	{
		if (type == scalar)
			nova::plus_vec(out, arg1, arg2, inNumSamples);
		if (type == unrolled)
			nova::plus_vec_simd(out, arg1, arg2, inNumSamples);
		if (type == unrolled_64)
			nova::plus_vec_simd<64>(out, arg1, arg2);
	}

	template <int type, typename Arg1, typename Arg2>
	static void times_vec(float * out, Arg1 const & arg1, Arg2 const & arg2, int inNumSamples)
	{
		if (type == scalar)
			nova::times_vec(out, arg1, arg2, inNumSamples);
		if (type == unrolled)
			nova::times_vec_simd(out, arg1, arg2, inNumSamples);
		if (type == unrolled_64)
			nova::times_vec_simd<64>(out, arg1, arg2);
	}

	template <int type, typename Arg1, typename Arg2>
	static void slope_vec(float * out, Arg1 const & base, Arg2 const & slope, int inNumSamples)
	{
		if (type == scalar)
			nova::set_slope_vec(out, base, slope, inNumSamples);
		else
			nova::set_slope_vec_simd(out, base, slope, inNumSamples);
	}

	template <int type>
	static void copy_vec(float * out, const float * in, int inNumSamples)
	{
		if (in == out)
			return;

		if (type == scalar)
			nova::copyvec(out, in, inNumSamples);
		if (type == unrolled)
			nova::copyvec_simd(out, in, inNumSamples);
		if (type == unrolled_64)
			nova::copyvec_simd<64>(out, in);
	}

	template <int type>
	static void set_vec(float * out, float value, int inNumSamples)
	{
		if (type == scalar)
			nova::setvec(out, value, inNumSamples);
		if (type == unrolled)
			nova::setvec_simd(out, value, inNumSamples);
		if (type == unrolled_64)
			nova::setvec_simd<64>(out, value);
	}

	void next_scalar(int inNumSamples)
	{
		out0(0) = (in0(0) * in0(1)) + in0(2);
	}

	template <int SIMD>
	void next_aa(int inNumSamples)
	{
		muladd<SIMD>(out(0), in(0), in(1), in(2), inNumSamples);
	}

	template <int SIMD>
	void next_ak(int inNumSamples)
	{
		if (addChanged())
			muladd<SIMD>(out(0), in(0), in(1), addSlope(), inNumSamples);
		else {
			if (mAddPrev == 0)
				times_vec<SIMD>(out(0), in(0), in(1), inNumSamples);
			else
				next_ai<SIMD>(inNumSamples);
		}
	}

	template <int SIMD>
	void next_ai(int inNumSamples)
	{
		muladd<SIMD>(out(0), in(0), in(1), mAddPrev, inNumSamples);
	}

	template <int SIMD>
	void next_ka(int inNumSamples)
	{
		if (mulChanged())
			muladd<SIMD>(out(0), in(0), mulSlope(), in(2), inNumSamples);
		else
			next_ia<SIMD>(inNumSamples);
	}

	template <int SIMD>
	void next_kk(int inNumSamples)
	{
		if (addChanged()) {
			if (mulChanged())
				muladd<SIMD>(out(0), in(0), mulSlope(), addSlope(), inNumSamples);
			else {
				if (mMulPrev == 0) {
					float nextAdd = in0(2);
					float currentAdd = mAddPrev;
					mAddPrev = nextAdd;
					slope_vec<SIMD>(out(0), currentAdd, calcSlope(nextAdd, currentAdd), inNumSamples);
					return;
				}
				if (mMulPrev == 1.f)
					plus_vec<SIMD>(out(0), in(0), addSlope(), inNumSamples);
				else
					muladd<SIMD>(out(0), in(0), mMulPrev, addSlope(), inNumSamples);
			}
		} else
			next_ki<SIMD>(inNumSamples);
	}

	template <int SIMD>
	void next_ki(int inNumSamples)
	{
		if (mulChanged())
			muladd<SIMD>(out(0), in(0), mulSlope(), mAddPrev, inNumSamples);
		else
			next_ii<SIMD>(inNumSamples);
	}

	template <int SIMD>
	void next_ia(int inNumSamples)
	{
		if (mMulPrev == 0)
			next_0a<SIMD>(inNumSamples);
		else if (mMulPrev == 1.0)
			next_1a<SIMD>(inNumSamples);
		else
			muladd<SIMD>(out(0), in(0), mMulPrev, in(2), inNumSamples);
	}

	template <int SIMD>
	void next_ik(int inNumSamples)
	{
		if (addChanged()) {
			if (mMulPrev == 0.f) {
				float nextAdd = in0(2);
				float currentAdd = mAddPrev;
				mAddPrev = nextAdd;
				slope_vec<SIMD>(out(0), currentAdd, calcSlope(nextAdd, currentAdd), inNumSamples);
				return;
			}
			if (mMulPrev == 1.f)
				plus_vec<SIMD>(out(0), in(0), addSlope(), inNumSamples);
			else
				muladd<SIMD>(out(0), in(0), mMulPrev, addSlope(), inNumSamples);
		} else
			next_ii<SIMD>(inNumSamples);
	}

	template <int SIMD>
	void next_ii(int inNumSamples)
	{
		if (mMulPrev == 0)
			next_0i<SIMD>(inNumSamples);
		else if (mMulPrev == 1.f) {
			next_1i<SIMD>(inNumSamples);
		} else {
			if (mAddPrev == 0)
				times_vec<SIMD>(out(0), in(0), mMulPrev, inNumSamples);
			else
				muladd<SIMD>(out(0), in(0), mMulPrev, mAddPrev, inNumSamples);
		}
	}

	template <int SIMD>
	void next_1a(int inNumSamples)
	{
		plus_vec<SIMD>(out(0), in(0), in(2), inNumSamples);
	}

	template <int SIMD>
	void next_1k(int inNumSamples)
	{
		if (addChanged())
			plus_vec<SIMD>(out(0), in(0), addSlope(), inNumSamples);
		else
			next_1i<SIMD>(inNumSamples);
	}

	template <int SIMD>
	void next_1i(int inNumSamples)
	{
		if (mAddPrev == 0)
			copy_vec<SIMD>(out(0), in(0), inNumSamples);
		else
			plus_vec<SIMD>(out(0), in(0), mAddPrev, inNumSamples);
	}

	template <int SIMD>
	void next_0a(int inNumSamples)
	{
		copy_vec<SIMD>(out(0), in(2), inNumSamples);
	}

	template <int SIMD>
	void next_0k(int inNumSamples)
	{
		if (addChanged()) {
			float nextAdd = in0(2);
			float currentAdd = mAddPrev;
			mAddPrev = nextAdd;
			slope_vec<SIMD>(out(0), currentAdd, calcSlope(nextAdd, currentAdd), inNumSamples);
			return;
		} else
			next_0i<SIMD>(inNumSamples);
	}

	template <int SIMD>
	void next_0i(int inNumSamples)
	{
		set_vec<SIMD>(out(0), mAddPrev, inNumSamples);
	}

	template <int SIMD>
	void next_a0(int inNumSamples)
	{
		times_vec<SIMD>(out(0), in(0), in(1), inNumSamples);
	}

	template <int SIMD>
	void next_k0(int inNumSamples)
	{
		if (mulChanged())
			times_vec<SIMD>(out(0), in(0), mulSlope(), inNumSamples);
		else
			next_ik<SIMD>(inNumSamples);
	}

	template <int SIMD>
	void next_i0(int inNumSamples)
	{
		if (mMulPrev == 0.f)
			next_00<SIMD>(inNumSamples);
		else if (mMulPrev == 1.f)
			next_10<SIMD>(inNumSamples);
		else
			times_vec<SIMD>(out(0), in(0), mMulPrev, inNumSamples);
	}

	template <int SIMD>
	void next_10(int inNumSamples)
	{
		copy_vec<SIMD>(out(0), in(0), inNumSamples);
	}

	template <int SIMD>
	void next_00(int inNumSamples)
	{
		set_vec<SIMD>(out(0), 0.f, inNumSamples);
	}

};

DEFINE_XTORS(MulAdd)

struct Sum3:
	SIMD_Unit
{
	float mPrev1, mPrev2;

	Sum3(void)
	{
		mPrev1 = in0(1);
		mPrev2 = in0(2);

		if (mCalcRate != calc_FullRate) {
			set_calc_function<Sum3, &Sum3::next_scalar>();
			return;
		}

		assert(inRate(0) == calc_FullRate);

		switch (inRate(1)) {
		case calc_FullRate:
			switch (inRate(2)) {
			case calc_FullRate:
				set_vector_calc_function<Sum3, &Sum3::next_aaa<true>, &Sum3::next_aaa<false> >();
				return;

			case calc_BufRate:
				set_vector_calc_function<Sum3, &Sum3::next_aak<true>, &Sum3::next_aak<false> >();
				return;

			case calc_ScalarRate:
				set_vector_calc_function<Sum3, &Sum3::next_aai<true>, &Sum3::next_aai<false> >();
				return;

			default:
				assert(false);
			}

		case calc_BufRate:
			switch (inRate(2)) {
				case calc_BufRate:
					set_vector_calc_function<Sum3, &Sum3::next_akk<true>, &Sum3::next_akk<false> >();
					return;

				case calc_ScalarRate:
					set_vector_calc_function<Sum3, &Sum3::next_aki<true>, &Sum3::next_aki<false> >();
					return;

				default:
					assert(false);
			}

		case calc_ScalarRate:
			assert (inRate(2) == calc_ScalarRate);
			set_vector_calc_function<Sum3, &Sum3::next_aii<true>, &Sum3::next_aii<false> >();
			return;

		default:
			assert(false);
		}
	}

	template <bool SIMD, typename Arg1, typename Arg2, typename Arg3>
	static void sum_vec(float * out, Arg1 const & arg1, Arg2 const & arg2, Arg3 const & arg3, int inNumSamples)
	{
		if (SIMD)
			nova::sum_vec_simd(out, arg1, arg2, arg3, inNumSamples);
		else
			nova::sum_vec(out, arg1, arg2, arg3, inNumSamples);
	}

	void next_scalar(int inNumSamples)
	{
		out0(0) = in0(0) + in0(1) + in0(2);
	}

	template <bool SIMD>
	void next_aaa(int inNumSamples)
	{
		sum_vec<SIMD>(out(0), in(0), in(1), in(2), inNumSamples);
	}

	template <bool SIMD>
	void next_aak(int inNumSamples)
	{
		float next2 = in0(2);
		if (next2 != mPrev2) {
			float slope = calcSlope(next2, mPrev2);
			sum_vec<SIMD>(out(0), in(0), in(1), slope_argument(mPrev2, slope), inNumSamples);
			mPrev2 = next2;
		} else
			next_aai<SIMD>(inNumSamples);
	}

	template <bool SIMD>
	void next_aai(int inNumSamples)
	{
		sum_vec<SIMD>(out(0), in(0), in(1), mPrev2, inNumSamples);
	}

	template <bool SIMD>
	void next_aki(int inNumSamples)
	{
		float next1 = in0(1);
		if (next1 != mPrev1) {
			float slope = calcSlope(next1, mPrev1);
			sum_vec<SIMD>(out(0), in(0), slope_argument(mPrev1, slope), in0(2), inNumSamples);
			mPrev1 = next1;
		} else
			next_aii<SIMD>(inNumSamples);
	}

	template <bool SIMD>
	void next_akk(int inNumSamples)
	{
		float next2 = in0(2);

		if (next2 != mPrev2) {
			float next1 = in0(1);
			float slope2 = calcSlope(next2, mPrev2);
			if (next1 != mPrev1) {
				float slope1 = calcSlope(next1, mPrev1);
				sum_vec<SIMD>(out(0), in(0), slope_argument(mPrev1, slope1), slope_argument(mPrev2, slope2), inNumSamples);
				mPrev1 = next1;
			} else
				sum_vec<SIMD>(out(0), in(0), next1, slope_argument(mPrev2, slope2), inNumSamples);
			mPrev2 = next2;
			return;
		} else
			next_aki<SIMD>(inNumSamples);
	}

	template <bool SIMD>
	void next_aii(int inNumSamples)
	{
		sum_vec<SIMD>(out(0), in(0), in0(1), in0(2), inNumSamples);
	}
};

DEFINE_XTORS(Sum3)

struct Sum4:
	SIMD_Unit
{
	float mPrev1, mPrev2, mPrev3;

	Sum4(void)
	{
		mPrev1 = in0(1);
		mPrev2 = in0(2);
		mPrev3 = in0(3);

		if (mCalcRate != calc_FullRate) {
			set_calc_function<Sum4, &Sum4::next_scalar>();
			return;
		}

		assert(inRate(0) == calc_FullRate);

		switch (inRate(1)) {
		case calc_FullRate:
			switch (inRate(2)) {
			case calc_FullRate:
				switch (inRate(3)) {
				case calc_FullRate:
					set_vector_calc_function<Sum4, &Sum4::next_aaaa<true>, &Sum4::next_aaaa<false> >();
					return;

				case calc_BufRate:
					set_vector_calc_function<Sum4, &Sum4::next_aaak<true>, &Sum4::next_aaak<false> >();
					return;

				case calc_ScalarRate:
					set_vector_calc_function<Sum4, &Sum4::next_aaai<true>, &Sum4::next_aaai<false> >();
					return;

				default:
					assert(false);
				}

			case calc_BufRate:
				switch (inRate(3)) {
				case calc_BufRate:
					set_vector_calc_function<Sum4, &Sum4::next_aakk<true>, &Sum4::next_aakk<false> >();
					return;

				case calc_ScalarRate:
					set_vector_calc_function<Sum4, &Sum4::next_aaki<true>, &Sum4::next_aaki<false> >();
					return;

				default:
					assert(false);
				}

			case calc_ScalarRate:
				switch (inRate(3)) {
				case calc_ScalarRate:
					set_vector_calc_function<Sum4, &Sum4::next_aaii<true>, &Sum4::next_aaii<false> >();
					return;

				default:
					assert(false);
				}
			}
		case calc_BufRate:
			switch (inRate(2)) {
			case calc_BufRate:
				switch (inRate(3)) {
				case calc_BufRate:
					set_vector_calc_function<Sum4, &Sum4::next_akkk<true>, &Sum4::next_akkk<false> >();
					return;

				case calc_ScalarRate:
					set_vector_calc_function<Sum4, &Sum4::next_akki<true>, &Sum4::next_akki<false> >();
					return;

				default:
					assert(false);
				}

			case calc_ScalarRate:
				switch (inRate(3)) {
				case calc_ScalarRate:
					set_vector_calc_function<Sum4, &Sum4::next_akii<true>, &Sum4::next_akii<false> >();
					return;

				default:
					assert(false);
				}
			}

		case calc_ScalarRate:
			switch (inRate(2)) {
			case calc_ScalarRate:
				switch (inRate(3)) {
				case calc_ScalarRate:
					set_vector_calc_function<Sum4, &Sum4::next_aiii<true>, &Sum4::next_aiii<false> >();
					return;

				default:
					assert(false);
				}

			default:
				assert(false);
			}

		default:
			assert(false);
		}
	}

	void next_scalar(int inNumSamples)
	{
		out0(0) = in0(0) + in0(1) + in0(2) + in0(3);
	}

	template <bool SIMD, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
	static void sum_vec(float * out, Arg1 const & arg1, Arg2 const & arg2, Arg3 const & arg3, Arg4 const & arg4, int inNumSamples)
	{
		if (SIMD)
			nova::sum_vec_simd(out, arg1, arg2, arg3, arg4, inNumSamples);
		else
			nova::sum_vec(out, arg1, arg2, arg3, arg4, inNumSamples);
	}

	template <bool SIMD>
	void next_aaaa(int inNumSamples)
	{
		sum_vec<SIMD>(out(0), in(0), in(1), in(2), in(3), inNumSamples);
	}

	template <bool SIMD>
	void next_aaak(int inNumSamples)
	{
		float next3 = in0(3);
		if (next3 != mPrev3) {
			sum_vec<SIMD>(out(0), in(0), in(1), in(2), slope_argument(mPrev3, calcSlope(next3, mPrev3)), inNumSamples);
			mPrev3 = next3;
		} else
			next_aaai<SIMD>(inNumSamples);
	}

	template <bool SIMD>
	void next_aaai(int inNumSamples)
	{
		sum_vec<SIMD>(out(0), in(0), in(1), in(2), mPrev3, inNumSamples);
	}

	template <bool SIMD>
	void next_aakk(int inNumSamples)
	{
		float next3 = in0(3);
		if (next3 != mPrev3) {
			float next2 = in0(2);
			if (next2 != mPrev2) {
				sum_vec<SIMD>(out(0), in(0), in(1), slope_argument(mPrev2, calcSlope(next2, mPrev2)),
							  slope_argument(mPrev3, calcSlope(next3, mPrev3)), inNumSamples);
				mPrev2 = next2;
			} else
				sum_vec<SIMD>(out(0), in(0), in(1), next2, slope_argument(mPrev3, calcSlope(next3, mPrev3)), inNumSamples);
			mPrev3 = next3;
		} else
			next_aaki<SIMD>(inNumSamples);
	}

	template <bool SIMD>
	void next_aaki(int inNumSamples)
	{
		float next2 = in0(2);
		if (next2 != mPrev2) {
			sum_vec<SIMD>(out(0), in(0), in(1), slope_argument(mPrev2, calcSlope(next2, mPrev2)),
							mPrev3, inNumSamples);
			mPrev2 = next2;
		} else
			next_aaii<SIMD>(inNumSamples);
	}

	template <bool SIMD>
	void next_aaii(int inNumSamples)
	{
		sum_vec<SIMD>(out(0), in(0), in(1), mPrev2, mPrev3, inNumSamples);
	}

	template <bool SIMD>
	void next_akkk(int inNumSamples)
	{
		float next3 = in0(3);
		if (next3 != mPrev3) {
			float next2 = in0(2);
			if (next2 != mPrev2) {
				float next1 = in0(1);
				if (next1 != mPrev1) {
					sum_vec<SIMD>(out(0), in(0), slope_argument(mPrev1, calcSlope(next1, mPrev1)),
								  slope_argument(mPrev2, calcSlope(next2, mPrev2)),
								  slope_argument(mPrev3, calcSlope(next3, mPrev3)), inNumSamples);
					mPrev1 = next1;
				} else
					sum_vec<SIMD>(out(0), in(0), mPrev1, slope_argument(mPrev2, calcSlope(next2, mPrev2)),
								  slope_argument(mPrev3, calcSlope(next3, mPrev3)), inNumSamples);
				mPrev2 = next2;
			} else {
				float next1 = in0(1);
				if (next1 != mPrev1) {
					sum_vec<SIMD>(out(0), in(0), slope_argument(mPrev1, calcSlope(next1, mPrev1)),
								  next2, slope_argument(mPrev3, calcSlope(next3, mPrev3)), inNumSamples);
					mPrev1 = next1;
				} else
					sum_vec<SIMD>(out(0), in(0), mPrev1, mPrev2, slope_argument(mPrev3, calcSlope(next3, mPrev3)), inNumSamples);
			}
			mPrev3 = next3;
		} else
			next_akki<SIMD>(inNumSamples);
	}

	template <bool SIMD>
	void next_akki(int inNumSamples)
	{
		float next2 = in0(2);
		if (next2 != mPrev2) {
			float next1 = in0(1);
			if (next1 != mPrev1) {
				sum_vec<SIMD>(out(0), in(0), slope_argument(mPrev1, calcSlope(next1, mPrev1)),
								slope_argument(mPrev2, calcSlope(next2, mPrev2)),
								mPrev3, inNumSamples);
				mPrev1 = next1;
			} else {
				sum_vec<SIMD>(out(0), in(0), mPrev1, slope_argument(mPrev2, calcSlope(next2, mPrev2)),
							  mPrev3, inNumSamples);
			}
			mPrev2 = next2;
		} else
			next_akii<SIMD>(inNumSamples);
	}

	template <bool SIMD>
	void next_akii(int inNumSamples)
	{
		float next1 = in0(1);
		if (next1 != mPrev1) {
			sum_vec<SIMD>(out(0), in(0), slope_argument(mPrev1, calcSlope(next1, mPrev1)),
							mPrev2,
							mPrev3, inNumSamples);
			mPrev1 = next1;
		} else
			next_aiii<SIMD>(inNumSamples);
	}

	template <bool SIMD>
	void next_aiii(int inNumSamples)
	{
		sum_vec<SIMD>(out(0), in(0), mPrev1, mPrev2, mPrev3, inNumSamples);
	}
};

DEFINE_XTORS(Sum4)

}

////////////////////////////////////////////////////////////////////////////////////////////////////////

PluginLoad(MulAdd)
{
	ft = inTable;

	DefineSimpleUnit(MulAdd);
	DefineSimpleUnit(Sum3);
	DefineSimpleUnit(Sum4);
}
