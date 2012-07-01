//  static pool class
//  Copyright (C) 2009 Tim Blechmann
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; see the file COPYING.  If not, write to
//  the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
//  Boston, MA 02111-1307, USA.

#ifndef UTILITIES_STATIC_POOL_HPP
#define UTILITIES_STATIC_POOL_HPP

extern "C"
{
#include "tlsf.h"
}

#include <array>

#include <boost/noncopyable.hpp>
#include <boost/static_assert.hpp>
#include <boost/mpl/arithmetic.hpp>
#include <boost/mpl/modulus.hpp>

#include "nova-tt/spin_lock.hpp"
#include "nova-tt/dummy_mutex.hpp"
#include "nova-tt/mlock.hpp"

namespace nova {

template <std::size_t bytes,
          bool blocking = false>
class static_pool:
    boost::noncopyable
{
    BOOST_STATIC_ASSERT((boost::mpl::modulus<boost::mpl::int_<bytes>, boost::mpl::int_<sizeof(long)> >::value == 0));

    static const std::size_t poolsize = bytes/sizeof(long);

    typedef typename boost::mpl::if_c<blocking,
                                      spin_lock,
                                      dummy_mutex>::type mutex_type;

    typedef typename mutex_type::scoped_lock scoped_lock;

    struct data:
        mutex_type
    {
        std::array<long, poolsize> pool;
    };

    void lock_memory(void)
    {
        mlock(data_.pool.data(), poolsize * sizeof(long));
    }

public:
    static_pool(bool lock = false) throw()
    {
        /* first lock, then assign */
        if (lock)
            lock_memory();

        data_.pool.fill(0);
        std::size_t status = init_memory_pool(bytes, data_.pool.begin());
        assert(status > 0);

    }

    ~static_pool() throw()
    {
        scoped_lock lock(data_);
        destroy_memory_pool(data_.pool.begin());
    }

    void * malloc(std::size_t size)
    {
        scoped_lock lock(data_);
        return malloc_ex(size, data_.pool.begin());
    }

    void free(void * p)
    {
        scoped_lock lock(data_);
        free_ex(p, data_.pool.begin());
    }

    std::size_t get_max_size(void)
    {
        return ::get_max_size(data_.pool.begin());
    }

private:
    data data_;
};

} /* namespace nova */

#endif /* UTILITIES_STATIC_POOL_HPP */

