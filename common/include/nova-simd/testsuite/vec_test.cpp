#include <iostream>
#define BOOST_TEST_MAIN
#include <boost/test/included/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

#include <cmath>

#include "../vec.hpp"

using namespace nova;
using namespace std;

template <typename F>
void test_get_set(void)
{
    vec<F> v;
    for (int i = 0; i != v.size; ++i)
        v.set(i, (F)1.0*i);

    for (int i = 0; i != v.size; ++i)
        BOOST_REQUIRE_EQUAL( v.get(i), F(1.0*i));

    vec<F> w;
    w.set_slope(0, 1.0);
    for (int i = 0; i != v.size; ++i)
        BOOST_REQUIRE_EQUAL( w.get(i), F(1.0*i));
}

template <typename VecType>
void compare_float_mask(VecType const & v, unsigned int mask)
{
   for (int i = 0; i != v.size; ++i) {
        union {
            float f;
            unsigned int i;
        } x;
        x.f = v.get(i);
        BOOST_REQUIRE_EQUAL( x.i, mask);
    }
}

void test_gen_float(void)
{
    typedef vec<float> vec_t;

    vec_t zero = vec_t::gen_zero();
    for (int i = 0; i != zero.size; ++i)
        BOOST_REQUIRE_EQUAL( zero.get(i), 0.f);

    vec_t one = vec_t::gen_one();
    for (int i = 0; i != one.size; ++i)
        BOOST_REQUIRE_EQUAL( one.get(i), 1.f);

    vec_t half = vec_t::gen_05();
    for (int i = 0; i != one.size; ++i)
        BOOST_REQUIRE_EQUAL( half.get(i), 0.5f);

    vec_t sign_mask = vec_t::gen_sign_mask();
    compare_float_mask(sign_mask, 0x80000000);

    vec_t abs_mask = vec_t::gen_abs_mask();
    compare_float_mask(abs_mask, 0x7fffffff);

    vec_t exp_mask = vec_t::gen_exp_mask();
    compare_float_mask(exp_mask, 0x7F800000);

    vec_t exp1_mask = vec_t::gen_exp_mask_1();
    compare_float_mask(exp1_mask, 0x3F000000);
}

BOOST_AUTO_TEST_CASE( get_set )
{
    test_get_set<float>();
    test_get_set<double>();
    test_gen_float();
}
