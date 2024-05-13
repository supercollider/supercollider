#include "utilities/sized_array.hpp"
#include "server/memory_pool.hpp"

#include <iostream>
#include <boost/test/unit_test.hpp>

namespace nova {
simple_pool<false> rt_pool;
} /* namespace nova */

using namespace nova;

#ifdef BOOST_HAS_RVALUE_REFS
using std::move;
#else
using boost::move;
#endif

BOOST_AUTO_TEST_CASE(sized_array_test_1) {
    sized_array<int> array(5);
    array[0] = -1;
    array[1] = 3;
    array[4] = 44;

    BOOST_REQUIRE_EQUAL(array.size(), 5u);
    BOOST_REQUIRE_EQUAL(array[1], 3);
    BOOST_REQUIRE_EQUAL(array[0], -1);
    BOOST_REQUIRE_EQUAL(*array.begin(), -1);
    BOOST_REQUIRE_EQUAL(array.front(), -1);
    BOOST_REQUIRE_EQUAL(array.back(), 44);
    BOOST_REQUIRE_EQUAL(*array.rbegin(), 44);
    BOOST_REQUIRE_EQUAL(*(array.rend() - 1), -1);

    sized_array<long> long_array(array);
    BOOST_REQUIRE_EQUAL(long_array.size(), 5u);
    BOOST_REQUIRE_EQUAL(long_array[1], 3);
    BOOST_REQUIRE_EQUAL(long_array[0], -1);

    // move assignment
    sized_array<int> array3(0);
    move(&array, &array + 1, &array3);
    BOOST_REQUIRE_EQUAL(array3.size(), 5u);
    BOOST_REQUIRE_EQUAL(array3[1], 3);
    BOOST_REQUIRE_EQUAL(array3[0], -1);
    BOOST_REQUIRE_EQUAL(array.size(), 0u);

    // move assignment
    sized_array<int> array4(move(array3));
    BOOST_REQUIRE_EQUAL(array4.size(), 5u);
    BOOST_REQUIRE_EQUAL(array4[1], 3);
    BOOST_REQUIRE_EQUAL(array4[0], -1);
    BOOST_REQUIRE_EQUAL(array3.size(), 0u);
}


template <typename Alloc1, typename Alloc2> void run_test_2(void) {
    int size = 1024;

    std::vector<int, typename Alloc1::template rebind<int>::other> vec;
    for (int i = 0; i != size; ++i)
        vec.push_back(-i);

    sized_array<int, typename Alloc2::template rebind<int>::other> array(vec);

    for (int i = 0; i != size; ++i)
        BOOST_REQUIRE_EQUAL(vec[i], array[i]);
}

BOOST_AUTO_TEST_CASE(sized_array_test_2) {
    rt_pool.init(1024 * 1024);
    run_test_2<std::allocator<void*>, std::allocator<void*>>();
    run_test_2<rt_pool_allocator<void*>, std::allocator<void*>>();
    run_test_2<std::allocator<void*>, rt_pool_allocator<void*>>();
    run_test_2<rt_pool_allocator<void*>, rt_pool_allocator<void*>>();
}

BOOST_AUTO_TEST_CASE(sized_array_test_3) {
    sized_array<int> array(5);
    array[0] = -1;
    array[1] = 3;
    array[4] = 44;

    BOOST_REQUIRE_EQUAL(array.size(), 5u);
    BOOST_REQUIRE_EQUAL(array[0], -1);
    BOOST_REQUIRE_EQUAL(array[1], 3);
    BOOST_REQUIRE_EQUAL(*array.begin(), -1);

    array.resize(6, 444);
    BOOST_REQUIRE_EQUAL(array.size(), 6u);
    BOOST_REQUIRE_EQUAL(array[0], -1);
    BOOST_REQUIRE_EQUAL(array[1], 3);
    BOOST_REQUIRE_EQUAL(array[5], 444);

    array.resize(2);
    BOOST_REQUIRE_EQUAL(array.size(), 2u);
    BOOST_REQUIRE_EQUAL(array[0], -1);
    BOOST_REQUIRE_EQUAL(array[1], 3);
}

BOOST_AUTO_TEST_CASE(sized_array_test_4) {
    sized_array<int> array0(int8_t(5));
    sized_array<int> array1(uint8_t(5));
    sized_array<int> array2(int16_t(5));
    sized_array<int> array3(uint16_t(5));
    sized_array<int> array4(int32_t(5));
    sized_array<int> array5(uint32_t(5));
    sized_array<int> array6(int64_t(5));
    sized_array<int> array7(uint64_t(5));

    sized_array<int> array8(size_t(5));
    sized_array<int> array9(short(5));
    sized_array<int> array10(int(5));
    sized_array<int> array11(long(5));
}
