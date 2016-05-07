//  waitable_timer.hpp  --------------------------------------------------------------//

//  Copyright 2013 Andrey Semashev

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt


#ifndef BOOST_DETAIL_WINAPI_WAITABLE_TIMER_HPP
#define BOOST_DETAIL_WINAPI_WAITABLE_TIMER_HPP

#include <boost/detail/winapi/basic_types.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

#if !defined( BOOST_USE_WINDOWS_H )
extern "C" {
typedef boost::detail::winapi::VOID_
(WINAPI *PTIMERAPCROUTINE)(
    boost::detail::winapi::LPVOID_ lpArgToCompletionRoutine,
    boost::detail::winapi::DWORD_ dwTimerLowValue,
    boost::detail::winapi::DWORD_ dwTimerHighValue);

#if !defined( BOOST_NO_ANSI_APIS )
BOOST_SYMBOL_IMPORT boost::detail::winapi::HANDLE_ WINAPI
CreateWaitableTimerA(
    ::_SECURITY_ATTRIBUTES* lpTimerAttributes,
    boost::detail::winapi::BOOL_ bManualReset,
    boost::detail::winapi::LPCSTR_ lpTimerName);

BOOST_SYMBOL_IMPORT boost::detail::winapi::HANDLE_ WINAPI
OpenWaitableTimerA(
    boost::detail::winapi::DWORD_ dwDesiredAccess,
    boost::detail::winapi::BOOL_ bInheritHandle,
    boost::detail::winapi::LPCSTR_ lpTimerName);
#endif

BOOST_SYMBOL_IMPORT boost::detail::winapi::HANDLE_ WINAPI
CreateWaitableTimerW(
    ::_SECURITY_ATTRIBUTES* lpTimerAttributes,
    boost::detail::winapi::BOOL_ bManualReset,
    boost::detail::winapi::LPCWSTR_ lpTimerName);

BOOST_SYMBOL_IMPORT boost::detail::winapi::HANDLE_ WINAPI
OpenWaitableTimerW(
    boost::detail::winapi::DWORD_ dwDesiredAccess,
    boost::detail::winapi::BOOL_ bInheritHandle,
    boost::detail::winapi::LPCWSTR_ lpTimerName);

BOOST_SYMBOL_IMPORT boost::detail::winapi::BOOL_ WINAPI
SetWaitableTimer(
    boost::detail::winapi::HANDLE_ hTimer,
    const ::_LARGE_INTEGER* lpDueTime,
    boost::detail::winapi::LONG_ lPeriod,
    PTIMERAPCROUTINE pfnCompletionRoutine,
    boost::detail::winapi::LPVOID_ lpArgToCompletionRoutine,
    boost::detail::winapi::BOOL_ fResume);

BOOST_SYMBOL_IMPORT boost::detail::winapi::BOOL_ WINAPI
CancelWaitableTimer(boost::detail::winapi::HANDLE_ hTimer);
}
#endif

namespace boost {
namespace detail {
namespace winapi {

typedef ::PTIMERAPCROUTINE PTIMERAPCROUTINE_;

#if !defined( BOOST_NO_ANSI_APIS )
using ::OpenWaitableTimerA;
#endif
using ::OpenWaitableTimerW;
using ::CancelWaitableTimer;

#if !defined( BOOST_NO_ANSI_APIS )
BOOST_FORCEINLINE HANDLE_ CreateWaitableTimerA(PSECURITY_ATTRIBUTES_ lpTimerAttributes, BOOL_ bManualReset, LPCSTR_ lpTimerName)
{
    return ::CreateWaitableTimerA(reinterpret_cast< ::_SECURITY_ATTRIBUTES* >(lpTimerAttributes), bManualReset, lpTimerName);
}
#endif

BOOST_FORCEINLINE HANDLE_ CreateWaitableTimerW(PSECURITY_ATTRIBUTES_ lpTimerAttributes, BOOL_ bManualReset, LPCWSTR_ lpTimerName)
{
    return ::CreateWaitableTimerW(reinterpret_cast< ::_SECURITY_ATTRIBUTES* >(lpTimerAttributes), bManualReset, lpTimerName);
}

BOOST_FORCEINLINE BOOL_ SetWaitableTimer(
    HANDLE_ hTimer,
    const LARGE_INTEGER_* lpDueTime,
    LONG_ lPeriod,
    PTIMERAPCROUTINE_ pfnCompletionRoutine,
    LPVOID_ lpArgToCompletionRoutine,
    BOOL_ fResume)
{
    return ::SetWaitableTimer(hTimer, reinterpret_cast< const ::_LARGE_INTEGER* >(lpDueTime), lPeriod, pfnCompletionRoutine, lpArgToCompletionRoutine, fResume);
}

#if !defined( BOOST_NO_ANSI_APIS )
BOOST_FORCEINLINE HANDLE_ create_waitable_timer(PSECURITY_ATTRIBUTES_ lpTimerAttributes, BOOL_ bManualReset, LPCSTR_ lpTimerName)
{
    return ::CreateWaitableTimerA(reinterpret_cast< ::_SECURITY_ATTRIBUTES* >(lpTimerAttributes), bManualReset, lpTimerName);
}
#endif

BOOST_FORCEINLINE HANDLE_ create_waitable_timer(PSECURITY_ATTRIBUTES_ lpTimerAttributes, BOOL_ bManualReset, LPCWSTR_ lpTimerName)
{
    return ::CreateWaitableTimerW(reinterpret_cast< ::_SECURITY_ATTRIBUTES* >(lpTimerAttributes), bManualReset, lpTimerName);
}

BOOST_FORCEINLINE HANDLE_ create_anonymous_waitable_timer(PSECURITY_ATTRIBUTES_ lpTimerAttributes, BOOL_ bManualReset)
{
#ifdef BOOST_NO_ANSI_APIS
    return ::CreateWaitableTimerW(reinterpret_cast< ::_SECURITY_ATTRIBUTES* >(lpTimerAttributes), bManualReset, 0);
#else
    return ::CreateWaitableTimerA(reinterpret_cast< ::_SECURITY_ATTRIBUTES* >(lpTimerAttributes), bManualReset, 0);
#endif
}

}
}
}

#endif // BOOST_DETAIL_WINAPI_WAITABLE_TIMER_HPP
