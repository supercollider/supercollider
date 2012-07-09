#ifndef BOOST_ATOMIC_DETAIL_INTERLOCKED_HPP
#define BOOST_ATOMIC_DETAIL_INTERLOCKED_HPP

//  Copyright (c) 2009 Helge Bahmann
//
//  Distributed under the Boost Software License, Version 1.0.
//  See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <boost/detail/interlocked.hpp>

namespace boost {
namespace atomics {
namespace detail {

static inline void
x86_full_fence(void)
{
    long tmp;
    BOOST_INTERLOCKED_EXCHANGE(&tmp, 0);
}

static inline void
platform_fence_before(memory_order)
{
}

static inline void
platform_fence_after(memory_order)
{
}

static inline void
platform_fence_before_store(memory_order)
{
}

static inline void
platform_fence_after_store(memory_order order)
{
    if (order == memory_order_seq_cst)
        x86_full_fence();
}

static inline void
platform_fence_after_load(memory_order order)
{
    if (order == memory_order_seq_cst) {
        x86_full_fence();
    }
}

template<typename T>
bool
platform_cmpxchg32_strong(T & expected, T desired, volatile T * ptr)
{
    T prev = expected;
    expected = (T)BOOST_INTERLOCKED_COMPARE_EXCHANGE((long *)(ptr), (long)desired, (long)expected);
    bool success = (prev==expected);
    return success;
}

#if defined(_WIN64)
template<typename T>
bool
platform_cmpxchg64_strong(T & expected, T desired, volatile T * ptr)
{
    T prev = expected;
    expected = (T) _InterlockedCompareExchange64((long long *)(ptr), (long long)desired, (long long)expected);
    bool success = (prev==expected);
    return success;
}

template<typename T>
void
platform_store64(T value, volatile T * ptr)
{
    *ptr = value;
}

template<typename T>
T
platform_load64(volatile T * ptr)
{
    return *ptr;
}
#endif

}
}

#define BOOST_ATOMIC_THREAD_FENCE 2
static inline void
atomic_thread_fence(memory_order order)
{
    if (order == memory_order_seq_cst) {
        atomics::detail::x86_full_fence();
    }
}

class atomic_flag {
private:
    atomic_flag(const atomic_flag &) /* = delete */ ;
    atomic_flag & operator=(const atomic_flag &) /* = delete */ ;
    uint32_t v_;
public:
    atomic_flag(void) : v_(false) {}

    void
    clear(memory_order order = memory_order_seq_cst) volatile
    {
        atomics::detail::platform_fence_before_store(order);
        const_cast<volatile uint32_t &>(v_) = 0;
        atomics::detail::platform_fence_after_store(order);
    }

    bool
    test_and_set(memory_order order = memory_order_seq_cst) volatile
    {
        atomics::detail::platform_fence_before(order);
        uint32_t expected = v_;
        do {
            if (expected == 1)
                break;
        } while (!atomics::detail::platform_cmpxchg32(expected, (uint32_t)1, &v_));
        atomics::detail::platform_fence_after(order);
        return expected;
    }
};

}

#define BOOST_ATOMIC_FLAG_LOCK_FREE 2

#include <boost/atomic/detail/base.hpp>

#if !defined(BOOST_ATOMIC_FORCE_FALLBACK)

#define BOOST_ATOMIC_CHAR_LOCK_FREE 2
#define BOOST_ATOMIC_SHORT_LOCK_FREE 2
#define BOOST_ATOMIC_INT_LOCK_FREE 2
#define BOOST_ATOMIC_LONG_LOCK_FREE 2
#define BOOST_ATOMIC_LLONG_LOCK_FREE 0
#define BOOST_ATOMIC_POINTER_LOCK_FREE 2
#define BOOST_ATOMIC_BOOL_LOCK_FREE 2

#include <boost/atomic/detail/cas32strong.hpp>
#if defined(_WIN64)
#include <boost/atomic/detail/cas64strong.hpp>
#endif

#endif /* !defined(BOOST_ATOMIC_FORCE_FALLBACK) */

#endif
