/*
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 *
 * (C) Copyright 2005-8 Anthony Williams
 * (C) Copyright 2012-2013 Vicente J. Botet Escriba
 * (C) Copyright 2013 Andrey Semashev
 */
/*!
 * \file   interlocked.hpp
 *
 * \brief  This header is the Boost.Sync library implementation, see the library documentation
 *         at http://www.boost.org/doc/libs/release/libs/sync/doc/html/index.html.
 */

#ifndef BOOST_SYNC_DETAIL_INTERLOCKED_HPP_INCLUDED_
#define BOOST_SYNC_DETAIL_INTERLOCKED_HPP_INCLUDED_

#include <boost/atomic/detail/interlocked.hpp>
#include <boost/sync/detail/config.hpp>
#include <boost/sync/detail/header.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

namespace boost {

namespace sync {

namespace detail {

namespace windows {

#if defined(BOOST_MSVC) && (_MSC_VER >= 1400) && !defined(UNDER_CE)

#if _MSC_VER == 1400
extern "C" unsigned char _interlockedbittestandset(long *a, long b);
extern "C" unsigned char _interlockedbittestandreset(long *a, long b);
#else
extern "C" unsigned char _interlockedbittestandset(volatile long *a, long b);
extern "C" unsigned char _interlockedbittestandreset(volatile long *a, long b);
#endif

#pragma intrinsic(_interlockedbittestandset)
#pragma intrinsic(_interlockedbittestandreset)

BOOST_FORCEINLINE bool interlocked_bit_test_and_set(long* x, long bit) BOOST_NOEXCEPT
{
    return _interlockedbittestandset(x, bit) != 0;
}

BOOST_FORCEINLINE bool interlocked_bit_test_and_reset(long* x, long bit) BOOST_NOEXCEPT
{
    return _interlockedbittestandreset(x, bit) != 0;
}

#define BOOST_SYNC_DETAIL_BTS_DEFINED

#elif (defined(BOOST_MSVC) || defined(BOOST_INTEL_WIN)) && defined(_M_IX86)

BOOST_FORCEINLINE bool interlocked_bit_test_and_set(long* x, long bit) BOOST_NOEXCEPT
{
#ifndef BOOST_INTEL_CXX_VERSION
    __asm
    {
        mov eax,bit;
        mov edx,x;
        lock bts [edx], eax;
        setc al;
    };
#else
    bool ret;
    __asm
    {
        mov eax,bit
        mov edx,x
        lock bts [edx], eax
        setc ret
    };
    return ret;
#endif
}

BOOST_FORCEINLINE bool interlocked_bit_test_and_reset(long* x, long bit) BOOST_NOEXCEPT
{
#ifndef BOOST_INTEL_CXX_VERSION
    __asm
    {
        mov eax,bit;
        mov edx,x;
        lock btr [edx],eax;
        setc al;
    };
#else
    bool ret;
    __asm
    {
        mov eax,bit
        mov edx,x
        lock btr [edx],eax
        setc ret
    };
    return ret;
#endif
}

#define BOOST_SYNC_DETAIL_BTS_DEFINED
#endif

#ifndef BOOST_SYNC_DETAIL_BTS_DEFINED

BOOST_FORCEINLINE bool interlocked_bit_test_and_set(long* x, long bit) BOOST_NOEXCEPT
{
    long const value = 1 << bit;
    long old = *x;
    while (true)
    {
        long const current = BOOST_ATOMIC_INTERLOCKED_COMPARE_EXCHANGE(x, old | value, old);
        if (current == old)
            break;
        old = current;
    }
    return (old & value) != 0;
}

BOOST_FORCEINLINE bool interlocked_bit_test_and_reset(long* x, long bit) BOOST_NOEXCEPT
{
    long const value = 1 << bit;
    long old = *x;
    while (true)
    {
        long const current = BOOST_ATOMIC_INTERLOCKED_COMPARE_EXCHANGE(x, old & ~value, old);
        if (current == old)
            break;
        old = current;
    }
    return (old & value) != 0;
}

#endif

#undef BOOST_SYNC_DETAIL_BTS_DEFINED

#if defined(__INTEL_COMPILER)
#define BOOST_SYNC_COMPILER_BARRIER() __memory_barrier()
#elif defined(_MSC_VER) && _MSC_VER >= 1310 && !defined(_WIN32_WCE)
extern "C" void _ReadWriteBarrier(void);
#pragma intrinsic(_ReadWriteBarrier)
#define BOOST_SYNC_COMPILER_BARRIER() _ReadWriteBarrier()
#endif

#ifdef BOOST_SYNC_COMPILER_BARRIER

BOOST_FORCEINLINE long interlocked_read_acquire(long volatile* x) BOOST_NOEXCEPT
{
    long const res = *x;
    BOOST_SYNC_COMPILER_BARRIER();
    return res;
}
BOOST_FORCEINLINE void* interlocked_read_acquire(void* volatile* x) BOOST_NOEXCEPT
{
    void* const res = *x;
    BOOST_SYNC_COMPILER_BARRIER();
    return res;
}

BOOST_FORCEINLINE void interlocked_write_release(long volatile* x, long value) BOOST_NOEXCEPT
{
    BOOST_SYNC_COMPILER_BARRIER();
    *x = value;
}
BOOST_FORCEINLINE void interlocked_write_release(void* volatile* x, void* value) BOOST_NOEXCEPT
{
    BOOST_SYNC_COMPILER_BARRIER();
    *x = value;
}

#else

BOOST_FORCEINLINE long interlocked_read_acquire(long volatile* x) BOOST_NOEXCEPT
{
    return BOOST_ATOMIC_INTERLOCKED_EXCHANGE_ADD(x, 0);
}
BOOST_FORCEINLINE void* interlocked_read_acquire(void* volatile* x) BOOST_NOEXCEPT
{
    return BOOST_ATOMIC_INTERLOCKED_EXCHANGE_ADD_POINTER(x, 0);
}

BOOST_FORCEINLINE void interlocked_write_release(long volatile* x, long value) BOOST_NOEXCEPT
{
    BOOST_ATOMIC_INTERLOCKED_EXCHANGE(x, value);
}
BOOST_FORCEINLINE void interlocked_write_release(void* volatile* x, void* value) BOOST_NOEXCEPT
{
    BOOST_ATOMIC_INTERLOCKED_EXCHANGE_POINTER(x, value);
}

#endif

} // namespace windows

} // namespace detail

} // namespace sync

} // namespace boost

#include <boost/sync/detail/footer.hpp>

#endif // BOOST_SYNC_DETAIL_INTERLOCKED_HPP_INCLUDED_
