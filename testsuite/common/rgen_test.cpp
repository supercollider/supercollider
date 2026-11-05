#include <boost/test/unit_test.hpp>

#include "SC_RGen.h"

#include "rgen/rgen_output_3_14_1.hpp"
#include "rgen/rgen_output_3_15_0.hpp"

#include <algorithm>

namespace constants {

/// @attention if you change these constants, you have to regenerate the existing rgen_output_<version>.hpp
/// headers in the 'rgen' folder. See rgen/README.md for more information.
static constexpr uint32_t seed = 1000;
static constexpr double scale = 10;
static constexpr double low = 1;
static constexpr double high = 10;

}

template <typename T, typename U> bool constexpr check_constant(T a, U b) { return std::is_same_v<T, U> && a == b; }

static_assert(check_constant(sc_3_14_1::seed, constants::seed), "seed mismatch");
static_assert(check_constant(sc_3_15_0::seed, constants::seed), "seed mismatch");

static_assert(check_constant(sc_3_14_1::scale, constants::scale), "scale mismatch");
static_assert(check_constant(sc_3_15_0::scale, constants::scale), "scale mismatch");

static_assert(check_constant(sc_3_14_1::low, constants::low), "low mismatch");
static_assert(check_constant(sc_3_15_0::low, constants::low), "low mismatch");

static_assert(check_constant(sc_3_14_1::high, constants::high), "scale mismatch");
static_assert(check_constant(sc_3_15_0::high, constants::high), "scale mismatch");

template <typename T> bool compare_float(const T& a, const T& b) {
    // compare with epsilon
    return std::abs(a - b) < 1e-6;
}

template <typename A, typename B> bool compare_output(const A& a, const B& b) {
    using T = typename A::value_type;
    static_assert(std::is_same_v<T, typename B::value_type>, "type mismatch");

    if (a.size() != a.size()) {
        throw std::runtime_error("size_mismatch");
    }

    if constexpr (std::is_floating_point_v<T>) {
        return std::equal(a.begin(), a.end(), b.begin(), compare_float<T>);
    } else {
        return std::equal(a.begin(), a.end(), b.begin());
    }
}

template <typename Data, typename Func> bool compare_free_function(const Data& data, Func func) {
    using T = std::invoke_result_t<Func, uint32_t&, uint32_t&, uint32_t&>;
    static_assert(std::is_same_v<T, typename Data::value_type>, "type mismatch");

    RGen rgen;
    rgen.init(constants::seed);

    for (size_t i = 0; i < data.size(); ++i) {
        T result = func(rgen.s1, rgen.s2, rgen.s3);
        if constexpr (std::is_floating_point_v<T>) {
            if (!compare_float(result, data[i]))
                return false;
        } else {
            if (result != data[i])
                return false;
        }
    }

    return true;
}

template <typename Data, typename Func, typename... Args>
bool compare_member_function(const Data& data, Func memfn, Args... args) {
    using T = std::invoke_result_t<Func, RGen, Args...>;
    static_assert(std::is_same_v<T, typename Data::value_type>, "type mismatch");

    RGen rgen;
    rgen.init(constants::seed);

    for (size_t i = 0; i < data.size(); ++i) {
        T result = std::invoke(memfn, rgen, args...);
        if constexpr (std::is_floating_point_v<T>) {
            if (!compare_float(result, data[i]))
                return false;
        } else {
            if (result != data[i])
                return false;
        }
    }

    return true;
}

BOOST_AUTO_TEST_CASE(rgen_test_1) {
    // Compare the nominal output of SC 3.14.1 and SC 3.15.0 to make sure that the data set itself is correct.

    //------------------------- RGen member functions --------------------------//

    // 32-bit integer
    BOOST_TEST(compare_output(sc_3_14_1::rgen_trand_output, sc_3_15_0::rgen_trand_output));
    BOOST_TEST(compare_output(sc_3_14_1::rgen_irand_output, sc_3_15_0::rgen_irand_output));
    BOOST_TEST(compare_output(sc_3_14_1::rgen_irand2_output, sc_3_15_0::rgen_irand2_output));
    BOOST_TEST(compare_output(sc_3_14_1::rgen_ilinrand_output, sc_3_15_0::rgen_ilinrand_output));
    BOOST_TEST(compare_output(sc_3_14_1::rgen_ibilinrand_output, sc_3_15_0::rgen_ibilinrand_output));

    // 32-bit float
    BOOST_TEST(compare_output(sc_3_14_1::rgen_fcoin_output, sc_3_15_0::rgen_fcoin_output));
    BOOST_TEST(compare_output(sc_3_14_1::rgen_frand_output, sc_3_15_0::rgen_frand_output));
    BOOST_TEST(compare_output(sc_3_14_1::rgen_frand2_output, sc_3_15_0::rgen_frand2_output));
    BOOST_TEST(compare_output(sc_3_14_1::rgen_frand0_output, sc_3_15_0::rgen_frand0_output));
    BOOST_TEST(compare_output(sc_3_14_1::rgen_frand8_output, sc_3_15_0::rgen_frand8_output));
    BOOST_TEST(compare_output(sc_3_14_1::rgen_flinrand_output, sc_3_15_0::rgen_flinrand_output));
    BOOST_TEST(compare_output(sc_3_14_1::rgen_fbilinrand_output, sc_3_15_0::rgen_fbilinrand_output));
    BOOST_TEST(compare_output(sc_3_14_1::rgen_fsum3rand_output, sc_3_15_0::rgen_fsum3rand_output));

    // 64-bit float
    BOOST_TEST(compare_output(sc_3_14_1::rgen_drand_output, sc_3_15_0::rgen_drand_output));
#if 0
    // only implemented in SC 3.15.0
    BOOST_TEST(compare_output(sc_3_14_1::rgen_drand2_output, sc_3_15_0::rgen_drand2_output));
#endif
    BOOST_TEST(compare_output(sc_3_14_1::rgen_linrand_output, sc_3_15_0::rgen_linrand_output));
    BOOST_TEST(compare_output(sc_3_14_1::rgen_bilinrand_output, sc_3_15_0::rgen_bilinrand_output));
    BOOST_TEST(compare_output(sc_3_14_1::rgen_exprandrng_output, sc_3_15_0::rgen_exprandrng_output));
    BOOST_TEST(compare_output(sc_3_14_1::rgen_exprand_output, sc_3_15_0::rgen_exprand_output));
#if 0
    // only implemented in SC 3.15.0
    BOOST_TEST(compare_output(sc_3_14_1::rgen_biexprand_output, sc_3_15_0::rgen_biexprand_output));
#endif
    BOOST_TEST(compare_output(sc_3_14_1::rgen_sum3rand_output, sc_3_15_0::rgen_sum3rand_output));

    //------------------------- RGen free functions --------------------------//

    // 32-bit integer
    BOOST_TEST(compare_output(sc_3_14_1::trand_output, sc_3_15_0::trand_output));

    // 32-bit float
    BOOST_TEST(compare_output(sc_3_14_1::fcoin_output, sc_3_15_0::fcoin_output));
    BOOST_TEST(compare_output(sc_3_14_1::frand_output, sc_3_15_0::frand_output));
    BOOST_TEST(compare_output(sc_3_14_1::frand2_output, sc_3_15_0::frand2_output));
    BOOST_TEST(compare_output(sc_3_14_1::frand0_output, sc_3_15_0::frand0_output));
    BOOST_TEST(compare_output(sc_3_14_1::frand8_output, sc_3_15_0::frand8_output));

    // 64-bit float
    // the implementation of the drand() function in SC 3.14.1 is wrong because
    // it does not consider the endianess. This has been fixed in SC 3.15.0.
    BOOST_TEST(compare_output(sc_3_14_1::drand_output, sc_3_15_0::drand_output) == false);
#if 0
    // only implemented in SC 3.15.0
    BOOST_TEST(compare_output(sc_3_14_1::drand2_output, sc_3_15_0::drand2_output));
#endif
}

BOOST_AUTO_TEST_CASE(rgen_test_2) {
    // Compare the current output of RGen with that of SC 3.14.1 resp. 3.15.0.

    //------------------------- RGen member functions --------------------------//

    using namespace constants;

    // 32-bit integer
    BOOST_TEST(compare_member_function(sc_3_14_1::rgen_trand_output, &RGen::trand));
    BOOST_TEST(compare_member_function(sc_3_14_1::rgen_irand_output, &RGen::irand, scale));
    BOOST_TEST(compare_member_function(sc_3_14_1::rgen_irand2_output, &RGen::irand2, scale));
    BOOST_TEST(compare_member_function(sc_3_14_1::rgen_ilinrand_output, &RGen::ilinrand, scale));
    BOOST_TEST(compare_member_function(sc_3_14_1::rgen_ibilinrand_output, &RGen::ibilinrand, scale));

    // 32-bit float
    BOOST_TEST(compare_member_function(sc_3_14_1::rgen_fcoin_output, &RGen::fcoin));
    BOOST_TEST(compare_member_function(sc_3_14_1::rgen_frand_output, &RGen::frand));
    BOOST_TEST(compare_member_function(sc_3_14_1::rgen_frand2_output, &RGen::frand2));
    BOOST_TEST(compare_member_function(sc_3_14_1::rgen_frand0_output, &RGen::frand0));
    BOOST_TEST(compare_member_function(sc_3_14_1::rgen_frand8_output, &RGen::frand8));
    BOOST_TEST(compare_member_function(sc_3_14_1::rgen_flinrand_output, &RGen::flinrand));
    BOOST_TEST(compare_member_function(sc_3_14_1::rgen_fbilinrand_output, &RGen::fbilinrand));
    BOOST_TEST(compare_member_function(sc_3_14_1::rgen_fsum3rand_output, &RGen::fsum3rand));

    // 64-bit float
    BOOST_TEST(compare_member_function(sc_3_14_1::rgen_drand_output, &RGen::drand));
    // only implemented in SC 3.15.0
    BOOST_TEST(compare_member_function(sc_3_15_0::rgen_drand2_output, &RGen::drand2));
    BOOST_TEST(compare_member_function(sc_3_14_1::rgen_linrand_output, &RGen::linrand, scale));
    BOOST_TEST(compare_member_function(sc_3_14_1::rgen_bilinrand_output, &RGen::bilinrand, scale));
    BOOST_TEST(compare_member_function(sc_3_14_1::rgen_exprandrng_output, &RGen::exprandrng, low, high));
    BOOST_TEST(compare_member_function(sc_3_14_1::rgen_exprand_output, &RGen::exprand, scale));
    // only implemented in SC 3.15.0
    BOOST_TEST(compare_member_function(sc_3_15_0::rgen_biexprand_output, &RGen::biexprand, scale));
    BOOST_TEST(compare_member_function(sc_3_14_1::rgen_sum3rand_output, &RGen::sum3rand, scale));

    //------------------------- RGen free functions --------------------------//

    // 32-bit integer
    BOOST_TEST(compare_free_function(sc_3_14_1::trand_output, trand));

    // 32-bit float
    BOOST_TEST(compare_free_function(sc_3_14_1::fcoin_output, fcoin));
    BOOST_TEST(compare_free_function(sc_3_14_1::frand_output, frand));
    BOOST_TEST(compare_free_function(sc_3_14_1::frand2_output, frand2));
    BOOST_TEST(compare_free_function(sc_3_14_1::frand0_output, frand0));
    BOOST_TEST(compare_free_function(sc_3_14_1::frand8_output, frand8));

    // 64-bit float
    // the implementation of the drand() function in SC 3.14.1 is wrong because
    // it does not consider the endianess. This has been fixed in SC 3.15.0.
    BOOST_TEST(compare_free_function(sc_3_14_1::drand_output, drand) == false);
    BOOST_TEST(compare_free_function(sc_3_15_0::drand_output, drand));
    // only implemented in SC 3.15.0
    BOOST_TEST(compare_free_function(sc_3_15_0::drand2_output, drand2));
}

enum class range_policy { inclusive, exclusive, high_exclusive };

template <range_policy policy, typename T, typename Func, typename... Args>
bool check_range(const T low, const T high, Func memfn, Args... args) {
    using U = std::invoke_result_t<Func, RGen, Args...>;
    static_assert(std::is_same_v<T, U>, "type mismatch");

    RGen rgen;
    rgen.init(constants::seed);

    static constexpr size_t iterations = 1000000;

    for (size_t i = 0; i < iterations; ++i) {
        auto result = std::invoke(memfn, rgen, std::forward<Args>(args)...);
        if constexpr (policy == range_policy::inclusive) {
            if (result < low || result > high) {
                std::cout << "value " << result << " out of range" << std::endl;
                return false;
            }
        } else if constexpr (policy == range_policy::exclusive) {
            if (result <= low || result >= high) {
                std::cout << "value " << result << " out of range" << std::endl;
                return false;
            }
        } else {
            static_assert(policy == range_policy::high_exclusive, "unexpected range policy");
            if (result < low || result >= high) {
                std::cout << "value " << result << " out of range" << std::endl;
                return false;
            }
        }
    }

    return true;
}

template <typename Values, typename Func, typename... Args>
bool check_values(const Values& values, Func memfn, Args... args) {
    using T = std::invoke_result_t<Func, RGen, Args...>;
    using U = typename Values::value_type;
    static_assert(std::is_same_v<T, U>, "type mismatch");

    RGen rgen;
    rgen.init(constants::seed);

    static constexpr size_t iterations = 1000000;

    for (size_t i = 0; i < iterations; ++i) {
        auto result = std::invoke(memfn, rgen, std::forward<Args>(args)...);
        if (std::find(values.begin(), values.end(), result) == values.end()) {
            std::cout << "unexpected value " << result << std::endl;
            return false;
        }
    }

    return true;
}

BOOST_AUTO_TEST_CASE(rgen_test_3) {
    // Check whether the output of RGen stays within the designated range.
    // NOTE: we only need to test the member functions because they all eventually forward to the free functions.

    using namespace constants;

    // 32-bit integer
    // RGen::trand() has no bounds
    BOOST_TEST(check_range<range_policy::high_exclusive>((int32_t)0, (int32_t)scale, &RGen::irand, scale));
    BOOST_TEST(check_range<range_policy::inclusive>((int32_t)-scale, (int32_t)scale, &RGen::irand2, scale));
    BOOST_TEST(check_range<range_policy::high_exclusive>((int32_t)0, (int32_t)scale, &RGen::ilinrand, scale));
    BOOST_TEST(check_range<range_policy::exclusive>((int32_t)-scale, (int32_t)scale, &RGen::ibilinrand, scale));

    // 32-bit float
    BOOST_TEST(check_values(std::array { -1.f, 1.f }, &RGen::fcoin));
    BOOST_TEST(check_range<range_policy::high_exclusive>(0.f, 1.f, &RGen::frand));
    BOOST_TEST(check_range<range_policy::high_exclusive>(-1.f, 1.f, &RGen::frand2));
    BOOST_TEST(check_range<range_policy::high_exclusive>(1.f, 2.f, &RGen::frand0));
    BOOST_TEST(check_range<range_policy::high_exclusive>(-0.125f, 1.125f, &RGen::frand8));
    BOOST_TEST(check_range<range_policy::high_exclusive>(0.f, 1.f, &RGen::flinrand));
    BOOST_TEST(check_range<range_policy::exclusive>(-1.f, 1.f, &RGen::fbilinrand));
    // RGen::fsum3rand() has no bounds

    // 64-bit float
    BOOST_TEST(check_range<range_policy::high_exclusive>(0.0, 1.0, &RGen::drand));
    BOOST_TEST(check_range<range_policy::high_exclusive>(-1.0, 1.0, &RGen::drand2));
    BOOST_TEST(check_range<range_policy::high_exclusive>(0.0, scale, &RGen::linrand, scale));
    BOOST_TEST(check_range<range_policy::exclusive>(-scale, scale, &RGen::bilinrand, scale));
    BOOST_TEST(check_range<range_policy::high_exclusive>(low, high, &RGen::exprandrng, low, high));
    BOOST_TEST(
        check_range<range_policy::high_exclusive>(0.0, std::numeric_limits<double>::infinity(), &RGen::exprand, scale));
    // RGen::biexprand() has no bounds
    // RGen::sum3rand() has no bounds
}
