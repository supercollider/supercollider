// semaphore.hpp, mutex/condition_varibale emulation
//
// Copyright (C) 2013 Tim Blechmann
// Copyright (C) 2013 Andrey Semashev
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_SYNC_DETAIL_SEMAPHORE_SEMAPHORE_EMULATION_HPP_INCLUDED_
#define BOOST_SYNC_DETAIL_SEMAPHORE_SEMAPHORE_EMULATION_HPP_INCLUDED_

#include <boost/utility/enable_if.hpp>
#include <boost/sync/detail/config.hpp>
#include <boost/sync/detail/time_traits.hpp>
#include <boost/sync/locks/lock_guard.hpp>
#include <boost/sync/locks/unique_lock.hpp>
#include <boost/sync/mutexes/mutex.hpp>
#include <boost/sync/condition_variables/condition_variable.hpp>
#include <boost/sync/detail/header.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

#define BOOST_SYNC_SEMAPHORE_EMULATED

namespace boost {

namespace sync  {

BOOST_SYNC_DETAIL_OPEN_ABI_NAMESPACE {

class semaphore
{
    BOOST_DELETED_FUNCTION(semaphore(semaphore const&))
    BOOST_DELETED_FUNCTION(semaphore& operator=(semaphore const&))

public:
    explicit semaphore(unsigned int i = 0) : m_count(i)
    {
    }

    void post()
    {
        sync::lock_guard< sync::mutex > lock(m_mutex);
        ++m_count;
        m_cond.notify_one();
    }

    void wait()
    {
        sync::unique_lock< sync::mutex > lock(m_mutex);
        while (m_count == 0)
            m_cond.wait(lock);

        --m_count;
    }

    bool try_wait()
    {
        sync::lock_guard< sync::mutex > lock(m_mutex);
        if (m_count == 0)
            return false;

        --m_count;
        return true;
    }

    template< typename Time >
    typename enable_if_c< sync::detail::time_traits< Time >::is_specialized, bool >::type timed_wait(Time const& timeout)
    {
        sync::unique_lock< sync::mutex > lock(m_mutex);
        while (m_count == 0)
        {
            if (!m_cond.timed_wait(lock, timeout))
            {
                if (m_count == 0)
                    return false;
                else
                    break;
            }
        }

        --m_count;
        return true;
    }

    template< typename Duration >
    typename enable_if< detail::is_time_tag_of< Duration, detail::time_duration_tag >, bool >::type wait_for(Duration const& duration)
    {
        return timed_wait(duration);
    }

    template< typename TimePoint >
    typename enable_if< detail::is_time_tag_of< TimePoint, detail::time_point_tag >, bool >::type wait_until(TimePoint const& abs_time)
    {
        return timed_wait(abs_time);
    }

private:
    sync::mutex m_mutex;
    sync::condition_variable m_cond;
    unsigned int m_count;
};

} // namespace abi

} // namespace sync

} // namespace boost

#include <boost/sync/detail/footer.hpp>

#endif // BOOST_SYNC_DETAIL_SEMAPHORE_SEMAPHORE_EMULATION_HPP_INCLUDED_
