#ifndef BOOST_ATOMIC_DETAIL_LOCKPOOL_HPP
#define BOOST_ATOMIC_DETAIL_LOCKPOOL_HPP

//  Copyright (c) 2011 Helge Bahmann
//
//  Distributed under the Boost Software License, Version 1.0.
//  See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)


#ifndef BOOST_ATOMIC_FLAG_LOCK_FREE
#include <boost/thread/mutex.hpp>
#endif

namespace boost {
namespace atomics {
namespace detail {

#ifndef BOOST_ATOMIC_FLAG_LOCK_FREE

class lockpool {
public:
    typedef mutex lock_type;
    class scoped_lock {
    private:
        mutex::scoped_lock guard;
    public:
        explicit
        scoped_lock(const volatile void * addr) : guard( lock_for(addr) )
        {
        }
    };
private:
    static mutex pool_[41];

    static mutex &
    lock_for(const volatile void * addr)
    {
        std::size_t index = reinterpret_cast<std::size_t>(addr) % 41;
        return pool_[index];
    }
};

#else

class lockpool {
public:
    typedef atomic_flag lock_type;

    class scoped_lock {
    private:
        atomic_flag & flag_;

        scoped_lock(const scoped_lock &) /* = delete */;
        void operator=(const scoped_lock &) /* = delete */;

    public:
        explicit
        scoped_lock(const volatile void * addr) : flag_( lock_for(addr) )
        {
            do {
            } while (flag_.test_and_set(memory_order_acquire));
        }

        ~scoped_lock(void)
        {
            flag_.clear(memory_order_release);
        }
    };

private:
    static atomic_flag pool_[41];

    static lock_type &
    lock_for(const volatile void * addr)
    {
        std::size_t index = reinterpret_cast<std::size_t>(addr) % 41;
        return pool_[index];
    }
};

#endif

}
}
}

#endif
