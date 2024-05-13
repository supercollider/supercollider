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
#include "SIMD_Unit.hpp"

static InterfaceTable* ft;

namespace {

struct MulAdd : SIMD_Unit {
    ControlRateInput<1> mMul;
    ControlRateInput<2> mAdd;

#define MULADD_CALCFUNC(METHOD_NAME)                                                                                   \
    do {                                                                                                               \
        set_unrolled_calc_function<MulAdd, &MulAdd::METHOD_NAME<unrolled_64>, &MulAdd::METHOD_NAME<unrolled>,          \
                                   &MulAdd::METHOD_NAME<scalar>>();                                                    \
        return;                                                                                                        \
    } while (0)

    MulAdd(void) {
        mMul.init(this);
        mAdd.init(this);

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
                if (mAdd == 0.f)
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
                if (mAdd == 0.f)
                    MULADD_CALCFUNC(next_k0);
                else
                    MULADD_CALCFUNC(next_ki);

            default:
                assert(false);
            }

        case calc_ScalarRate:
            switch (inRate(2)) {
            case calc_FullRate:
                if (mMul == 1.0)
                    MULADD_CALCFUNC(next_1a);
                else if (mMul == 0.f)
                    MULADD_CALCFUNC(next_0a);
                else
                    MULADD_CALCFUNC(next_ia);

            case calc_BufRate:
                if (mMul == 1.0)
                    MULADD_CALCFUNC(next_1k);
                else if (mMul == 0.f)
                    MULADD_CALCFUNC(next_0k);
                else
                    MULADD_CALCFUNC(next_ik);

            case calc_ScalarRate:
                if (mMul == 1.0) {
                    if (mAdd == 0)
                        MULADD_CALCFUNC(next_10);
                    else
                        MULADD_CALCFUNC(next_1i);
                }

                else if (mMul == 0.f) {
                    if (mAdd == 0.f)
                        MULADD_CALCFUNC(next_00);
                    else
                        MULADD_CALCFUNC(next_0i);
                } else {
                    if (mAdd == 0.f)
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

    inline bool mulChanged(void) const { return mMul.changed(this); }

    inline bool addChanged(void) const { return mAdd.changed(this); }

#if __cplusplus <= 199711L
    nova::detail::scalar_ramp_argument<float> mulSlope(void)
#else
    decltype(nova::slope_argument(0.f, 0.f)) mulSlope(void)
#endif
    {
        return mMul.slope(this);
    }

#if __cplusplus <= 199711L
    nova::detail::scalar_ramp_argument<float> addSlope(void)
#else
    decltype(nova::slope_argument(0.f, 0.f)) addSlope(void)
#endif
    {
        return mAdd.slope(this);
    }

    void next_scalar(int inNumSamples) { out0(0) = (in0(0) * in0(1)) + in0(2); }

    template <int SIMD> void next_aa(int inNumSamples) { muladd<SIMD>(out(0), in(0), in(1), in(2), inNumSamples); }

    template <int SIMD> void next_ak(int inNumSamples) {
        if (addChanged())
            muladd<SIMD>(out(0), in(0), in(1), addSlope(), inNumSamples);
        else {
            if (mAdd == 0.f)
                times_vec<SIMD>(out(0), in(0), in(1), inNumSamples);
            else
                next_ai<SIMD>(inNumSamples);
        }
    }

    template <int SIMD> void next_ai(int inNumSamples) { muladd<SIMD>(out(0), in(0), in(1), mAdd, inNumSamples); }

    template <int SIMD> void next_ka(int inNumSamples) {
        if (mulChanged())
            muladd<SIMD>(out(0), in(0), mulSlope(), in(2), inNumSamples);
        else
            next_ia<SIMD>(inNumSamples);
    }

    template <int SIMD> void next_kk(int inNumSamples) {
        if (addChanged()) {
            if (mulChanged())
                muladd<SIMD>(out(0), in(0), mulSlope(), addSlope(), inNumSamples);
            else {
                if (mMul == 0)
                    slope_vec<SIMD>(out(0), addSlope(), inNumSamples);
                else if (mMul == 1.f)
                    plus_vec<SIMD>(out(0), in(0), addSlope(), inNumSamples);
                else
                    muladd<SIMD>(out(0), in(0), mMul, addSlope(), inNumSamples);
            }
        } else
            next_ki<SIMD>(inNumSamples);
    }

    template <int SIMD> void next_ki(int inNumSamples) {
        if (mulChanged())
            muladd<SIMD>(out(0), in(0), mulSlope(), mAdd, inNumSamples);
        else
            next_ii<SIMD>(inNumSamples);
    }

    template <int SIMD> void next_ia(int inNumSamples) {
        if (mMul == 0)
            next_0a<SIMD>(inNumSamples);
        else if (mMul == 1.0)
            next_1a<SIMD>(inNumSamples);
        else
            muladd<SIMD>(out(0), in(0), mMul, in(2), inNumSamples);
    }

    template <int SIMD> void next_ik(int inNumSamples) {
        if (addChanged()) {
            if (mMul == 0.f)
                slope_vec<SIMD>(out(0), addSlope(), inNumSamples);
            else if (mMul == 1.f)
                plus_vec<SIMD>(out(0), in(0), addSlope(), inNumSamples);
            else
                muladd<SIMD>(out(0), in(0), mMul, addSlope(), inNumSamples);
        } else
            next_ii<SIMD>(inNumSamples);
    }

    template <int SIMD> void next_ii(int inNumSamples) {
        if (mMul == 0)
            next_0i<SIMD>(inNumSamples);
        else if (mMul == 1.f) {
            next_1i<SIMD>(inNumSamples);
        } else {
            if (mAdd == 0)
                times_vec<SIMD>(out(0), in(0), mMul, inNumSamples);
            else
                muladd<SIMD>(out(0), in(0), mMul, mAdd, inNumSamples);
        }
    }

    template <int SIMD> void next_1a(int inNumSamples) { plus_vec<SIMD>(out(0), in(0), in(2), inNumSamples); }

    template <int SIMD> void next_1k(int inNumSamples) {
        if (addChanged())
            plus_vec<SIMD>(out(0), in(0), addSlope(), inNumSamples);
        else
            next_1i<SIMD>(inNumSamples);
    }

    template <int SIMD> void next_1i(int inNumSamples) {
        if (mAdd == 0)
            copy_vec<SIMD>(out(0), in(0), inNumSamples);
        else
            plus_vec<SIMD>(out(0), in(0), mAdd, inNumSamples);
    }

    template <int SIMD> void next_0a(int inNumSamples) { copy_vec<SIMD>(out(0), in(2), inNumSamples); }

    template <int SIMD> void next_0k(int inNumSamples) {
        if (addChanged())
            slope_vec<SIMD>(out(0), addSlope(), inNumSamples);
        else
            next_0i<SIMD>(inNumSamples);
    }

    template <int SIMD> void next_0i(int inNumSamples) { set_vec<SIMD>(out(0), mAdd, inNumSamples); }

    template <int SIMD> void next_a0(int inNumSamples) { times_vec<SIMD>(out(0), in(0), in(1), inNumSamples); }

    template <int SIMD> void next_k0(int inNumSamples) {
        if (mulChanged())
            times_vec<SIMD>(out(0), in(0), mulSlope(), inNumSamples);
        else
            next_ik<SIMD>(inNumSamples);
    }

    template <int SIMD> void next_i0(int inNumSamples) {
        if (mMul == 0.f)
            next_00<SIMD>(inNumSamples);
        else if (mMul == 1.f)
            next_10<SIMD>(inNumSamples);
        else
            times_vec<SIMD>(out(0), in(0), mMul, inNumSamples);
    }

    template <int SIMD> void next_10(int inNumSamples) { copy_vec<SIMD>(out(0), in(0), inNumSamples); }

    template <int SIMD> void next_00(int inNumSamples) { set_vec<SIMD>(out(0), 0.f, inNumSamples); }
};

struct Sum3 : SIMD_Unit {
    ControlRateInput<1> in1;
    ControlRateInput<2> in2;

    Sum3(void) {
        in1.init(this);
        in2.init(this);

        if (mCalcRate != calc_FullRate) {
            set_calc_function<Sum3, &Sum3::next_scalar>();
            return;
        }

        assert(inRate(0) == calc_FullRate);

        switch (inRate(1)) {
        case calc_FullRate:
            switch (inRate(2)) {
            case calc_FullRate:
                set_vector_calc_function<Sum3, &Sum3::next_aaa<true>, &Sum3::next_aaa<false>>();
                return;

            case calc_BufRate:
                set_vector_calc_function<Sum3, &Sum3::next_aak<true>, &Sum3::next_aak<false>>();
                return;

            case calc_ScalarRate:
                set_vector_calc_function<Sum3, &Sum3::next_aai<true>, &Sum3::next_aai<false>>();
                return;

            default:
                assert(false);
            }

        case calc_BufRate:
            switch (inRate(2)) {
            case calc_BufRate:
                set_vector_calc_function<Sum3, &Sum3::next_akk<true>, &Sum3::next_akk<false>>();
                return;

            case calc_ScalarRate:
                set_vector_calc_function<Sum3, &Sum3::next_aki<true>, &Sum3::next_aki<false>>();
                return;

            default:
                assert(false);
            }

        case calc_ScalarRate:
            assert(inRate(2) == calc_ScalarRate);
            set_vector_calc_function<Sum3, &Sum3::next_aii<true>, &Sum3::next_aii<false>>();
            return;

        default:
            assert(false);
        }
    }

    template <bool SIMD, typename Arg1, typename Arg2, typename Arg3>
    static void sum_vec(float* out, Arg1 const& arg1, Arg2 const& arg2, Arg3 const& arg3, int inNumSamples) {
        if (SIMD)
            nova::sum_vec_simd(out, arg1, arg2, arg3, inNumSamples);
        else
            nova::sum_vec(out, arg1, arg2, arg3, inNumSamples);
    }

    void next_scalar(int inNumSamples) { out0(0) = in0(0) + in0(1) + in0(2); }

    template <bool SIMD> void next_aaa(int inNumSamples) { sum_vec<SIMD>(out(0), in(0), in(1), in(2), inNumSamples); }

    template <bool SIMD> void next_aak(int inNumSamples) {
        if (in2.changed(this))
            sum_vec<SIMD>(out(0), in(0), in(1), in2.slope(this), inNumSamples);
        else
            next_aai<SIMD>(inNumSamples);
    }

    template <bool SIMD> void next_aai(int inNumSamples) { sum_vec<SIMD>(out(0), in(0), in(1), in2, inNumSamples); }

    template <bool SIMD> void next_aki(int inNumSamples) {
        if (in1.changed(this))
            sum_vec<SIMD>(out(0), in(0), in1.slope(this), in2, inNumSamples);
        else
            next_aii<SIMD>(inNumSamples);
    }

    template <bool SIMD> void next_akk(int inNumSamples) {
        if (in2.changed(this)) {
            if (in1.changed(this))
                sum_vec<SIMD>(out(0), in(0), in1.slope(this), in2.slope(this), inNumSamples);
            else
                sum_vec<SIMD>(out(0), in(0), in1, in2.slope(this), inNumSamples);
        } else
            next_aki<SIMD>(inNumSamples);
    }

    template <bool SIMD> void next_aii(int inNumSamples) { sum_vec<SIMD>(out(0), in(0), in0(1), in0(2), inNumSamples); }
};

struct Sum4 : SIMD_Unit {
    ControlRateInput<1> in1;
    ControlRateInput<2> in2;
    ControlRateInput<3> in3;

    Sum4(void) {
        in1.init(this);
        in2.init(this);
        in3.init(this);

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
                    set_vector_calc_function<Sum4, &Sum4::next_aaaa<true>, &Sum4::next_aaaa<false>>();
                    return;

                case calc_BufRate:
                    set_vector_calc_function<Sum4, &Sum4::next_aaak<true>, &Sum4::next_aaak<false>>();
                    return;

                case calc_ScalarRate:
                    set_vector_calc_function<Sum4, &Sum4::next_aaai<true>, &Sum4::next_aaai<false>>();
                    return;

                default:
                    assert(false);
                }

            case calc_BufRate:
                switch (inRate(3)) {
                case calc_BufRate:
                    set_vector_calc_function<Sum4, &Sum4::next_aakk<true>, &Sum4::next_aakk<false>>();
                    return;

                case calc_ScalarRate:
                    set_vector_calc_function<Sum4, &Sum4::next_aaki<true>, &Sum4::next_aaki<false>>();
                    return;

                default:
                    assert(false);
                }

            case calc_ScalarRate:
                switch (inRate(3)) {
                case calc_ScalarRate:
                    set_vector_calc_function<Sum4, &Sum4::next_aaii<true>, &Sum4::next_aaii<false>>();
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
                    set_vector_calc_function<Sum4, &Sum4::next_akkk<true>, &Sum4::next_akkk<false>>();
                    return;

                case calc_ScalarRate:
                    set_vector_calc_function<Sum4, &Sum4::next_akki<true>, &Sum4::next_akki<false>>();
                    return;

                default:
                    assert(false);
                }

            case calc_ScalarRate:
                switch (inRate(3)) {
                case calc_ScalarRate:
                    set_vector_calc_function<Sum4, &Sum4::next_akii<true>, &Sum4::next_akii<false>>();
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
                    set_vector_calc_function<Sum4, &Sum4::next_aiii<true>, &Sum4::next_aiii<false>>();
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

    void next_scalar(int inNumSamples) { out0(0) = in0(0) + in0(1) + in0(2) + in0(3); }

    template <bool SIMD, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
    static void sum_vec(float* out, Arg1 const& arg1, Arg2 const& arg2, Arg3 const& arg3, Arg4 const& arg4,
                        int inNumSamples) {
        if (SIMD)
            nova::sum_vec_simd(out, arg1, arg2, arg3, arg4, inNumSamples);
        else
            nova::sum_vec(out, arg1, arg2, arg3, arg4, inNumSamples);
    }

    template <bool SIMD> void next_aaaa(int inNumSamples) {
        sum_vec<SIMD>(out(0), in(0), in(1), in(2), in(3), inNumSamples);
    }

    template <bool SIMD> void next_aaak(int inNumSamples) {
        if (in3.changed(this))
            sum_vec<SIMD>(out(0), in(0), in(1), in(2), in3.slope(this), inNumSamples);
        else
            next_aaai<SIMD>(inNumSamples);
    }

    template <bool SIMD> void next_aaai(int inNumSamples) {
        sum_vec<SIMD>(out(0), in(0), in(1), in(2), in3, inNumSamples);
    }

    template <bool SIMD> void next_aakk(int inNumSamples) {
        if (in3.changed(this)) {
            if (in2.changed(this))
                sum_vec<SIMD>(out(0), in(0), in(1), in2.slope(this), in3.slope(this), inNumSamples);
            else
                sum_vec<SIMD>(out(0), in(0), in(1), in2, in3.slope(this), inNumSamples);
        } else
            next_aaki<SIMD>(inNumSamples);
    }

    template <bool SIMD> void next_aaki(int inNumSamples) {
        if (in2.changed(this))
            sum_vec<SIMD>(out(0), in(0), in(1), in2.slope(this), in3, inNumSamples);
        else
            next_aaii<SIMD>(inNumSamples);
    }

    template <bool SIMD> void next_aaii(int inNumSamples) {
        sum_vec<SIMD>(out(0), in(0), in(1), in2, in3, inNumSamples);
    }

    template <bool SIMD> void next_akkk(int inNumSamples) {
        if (in3.changed(this)) {
            if (in2.changed(this)) {
                if (in1.changed(this))
                    sum_vec<SIMD>(out(0), in(0), in1.slope(this), in2.slope(this), in3.slope(this), inNumSamples);
                else
                    sum_vec<SIMD>(out(0), in(0), in1, in2.slope(this), in3.slope(this), inNumSamples);
            } else {
                if (in1.changed(this))
                    sum_vec<SIMD>(out(0), in(0), in1.slope(this), in2, in3.slope(this), inNumSamples);
                else
                    sum_vec<SIMD>(out(0), in(0), in1, in2, in3.slope(this), inNumSamples);
            }
        } else
            next_akki<SIMD>(inNumSamples);
    }

    template <bool SIMD> void next_akki(int inNumSamples) {
        if (in2.changed(this)) {
            if (in1.changed(this))
                sum_vec<SIMD>(out(0), in(0), in1.slope(this), in2.slope(this), in3, inNumSamples);
            else
                sum_vec<SIMD>(out(0), in(0), in1, in2.slope(this), in3, inNumSamples);
        } else
            next_akii<SIMD>(inNumSamples);
    }

    template <bool SIMD> void next_akii(int inNumSamples) {
        if (in1.changed(this))
            sum_vec<SIMD>(out(0), in(0), in1.slope(this), in2, in3, inNumSamples);
        else
            next_aiii<SIMD>(inNumSamples);
    }

    template <bool SIMD> void next_aiii(int inNumSamples) { sum_vec<SIMD>(out(0), in(0), in1, in2, in3, inNumSamples); }
};

}

////////////////////////////////////////////////////////////////////////////////////////////////////////

PluginLoad(MulAdd) {
    ft = inTable;

    registerUnit<MulAdd>(ft, "MulAdd");
    registerUnit<Sum3>(ft, "Sum3");
    registerUnit<Sum4>(ft, "Sum4");
}
