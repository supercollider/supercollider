#ifndef BOOST_ATOMIC_HPP
#define BOOST_ATOMIC_HPP

//  Copyright (c) 2011 Helge Bahmann
//
//  Distributed under the Boost Software License, Version 1.0.
//  See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <cstddef>
#include <boost/cstdint.hpp>

#include <boost/memory_order.hpp>

#include <boost/atomic/platform.hpp>
#include <boost/atomic/detail/type-classifier.hpp>
#include <boost/type_traits/is_signed.hpp>

namespace boost {

#ifndef BOOST_ATOMIC_CHAR_LOCK_FREE
#define BOOST_ATOMIC_CHAR_LOCK_FREE 0
#endif

#ifndef BOOST_ATOMIC_CHAR16_T_LOCK_FREE
#define BOOST_ATOMIC_CHAR16_T_LOCK_FREE 0
#endif

#ifndef BOOST_ATOMIC_CHAR32_T_LOCK_FREE
#define BOOST_ATOMIC_CHAR32_T_LOCK_FREE 0
#endif

#ifndef BOOST_ATOMIC_WCHAR_T_LOCK_FREE
#define BOOST_ATOMIC_WCHAR_T_LOCK_FREE 0
#endif

#ifndef BOOST_ATOMIC_SHORT_LOCK_FREE
#define BOOST_ATOMIC_SHORT_LOCK_FREE 0
#endif

#ifndef BOOST_ATOMIC_INT_LOCK_FREE
#define BOOST_ATOMIC_INT_LOCK_FREE 0
#endif

#ifndef BOOST_ATOMIC_LONG_LOCK_FREE
#define BOOST_ATOMIC_LONG_LOCK_FREE 0
#endif

#ifndef BOOST_ATOMIC_LLONG_LOCK_FREE
#define BOOST_ATOMIC_LLONG_LOCK_FREE 0
#endif

#ifndef BOOST_ATOMIC_POINTER_LOCK_FREE
#define BOOST_ATOMIC_POINTER_LOCK_FREE 0
#endif

#define BOOST_ATOMIC_ADDRESS_LOCK_FREE BOOST_ATOMIC_POINTER_LOCK_FREE

#ifndef BOOST_ATOMIC_BOOL_LOCK_FREE
#define BOOST_ATOMIC_BOOL_LOCK_FREE 0
#endif

#ifndef BOOST_ATOMIC_THREAD_FENCE
#define BOOST_ATOMIC_THREAD_FENCE 0
static inline void
atomic_thread_fence(memory_order)
{
}
#endif

#ifndef BOOST_ATOMIC_SIGNAL_FENCE
#define BOOST_ATOMIC_SIGNAL_FENCE 0
static inline void
atomic_signal_fence(memory_order order)
{
    atomic_thread_fence(order);
}
#endif

template<typename T>
class atomic : public atomics::detail::base_atomic<T, typename atomics::detail::type_classifier<T>::test, sizeof(T), boost::is_signed<T>::value > {
private:
    typedef T value_type;
    typedef atomics::detail::base_atomic<T, typename atomics::detail::type_classifier<T>::test, sizeof(T), boost::is_signed<T>::value > super;
public:
    atomic(void) : super() {}
    explicit atomic(const value_type & v) : super(v) {}

    atomic & operator=(value_type v) volatile
    {
        super::operator=(v);
        return *const_cast<atomic *>(this);
    }
private:
    atomic(const atomic &) /* =delete */ ;
    atomic & operator=(const atomic &) /* =delete */ ;
};

typedef atomic<char> atomic_char;
typedef atomic<unsigned char> atomic_uchar;
typedef atomic<signed char> atomic_schar;
typedef atomic<uint8_t> atomic_uint8_t;
typedef atomic<int8_t> atomic_int8_t;
typedef atomic<unsigned short> atomic_ushort;
typedef atomic<short> atomic_short;
typedef atomic<uint16_t> atomic_uint16_t;
typedef atomic<int16_t> atomic_int16_t;
typedef atomic<unsigned int> atomic_uint;
typedef atomic<int> atomic_int;
typedef atomic<uint32_t> atomic_uint32_t;
typedef atomic<int32_t> atomic_int32_t;
typedef atomic<unsigned long> atomic_ulong;
typedef atomic<long> atomic_long;
typedef atomic<uint64_t> atomic_uint64_t;
typedef atomic<int64_t> atomic_int64_t;
#ifdef BOOST_HAS_LONG_LONG
typedef atomic<unsigned long long> atomic_ullong;
typedef atomic<long long> atomic_llong;
#endif
typedef atomic<void*> atomic_address;
typedef atomic<bool> atomic_bool;

#ifndef BOOST_ATOMIC_FLAG_LOCK_FREE
#define BOOST_ATOMIC_FLAG_LOCK_FREE 0
class atomic_flag {
public:
    atomic_flag(void) : v_(false) {}

    bool
    test_and_set(memory_order order = memory_order_seq_cst)
    {
        return v_.exchange(true, order);
    }

    void
    clear(memory_order order = memory_order_seq_cst) volatile
    {
        v_.store(false, order);
    }
private:
    atomic_flag(const atomic_flag &) /* = delete */ ;
    atomic_flag & operator=(const atomic_flag &) /* = delete */ ;
    atomic<bool> v_;
};
#endif

typedef atomic<char> atomic_char;
typedef atomic<unsigned char> atomic_uchar;
typedef atomic<signed char> atomic_schar;
typedef atomic<uint8_t> atomic_uint8_t;
typedef atomic<int8_t> atomic_int8_t;
typedef atomic<unsigned short> atomic_ushort;
typedef atomic<short> atomic_short;
typedef atomic<uint16_t> atomic_uint16_t;
typedef atomic<int16_t> atomic_int16_t;
typedef atomic<unsigned int> atomic_uint;
typedef atomic<int> atomic_int;
typedef atomic<uint32_t> atomic_uint32_t;
typedef atomic<int32_t> atomic_int32_t;
typedef atomic<unsigned long> atomic_ulong;
typedef atomic<long> atomic_long;
typedef atomic<uint64_t> atomic_uint64_t;
typedef atomic<int64_t> atomic_int64_t;
typedef atomic<unsigned long long> atomic_ullong;
typedef atomic<long long> atomic_llong;
typedef atomic<void*> atomic_address;
typedef atomic<bool> atomic_bool;

}

#endif
