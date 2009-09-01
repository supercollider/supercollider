//  nova freelist class
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

#ifndef UTILITIES_FREELIST_HPP
#define UTILITIES_FREELIST_HPP

#include <boost/lockfree/detail/tagged_ptr.hpp>
#include <boost/lockfree/atomic_int.hpp>
#include <boost/noncopyable.hpp>


namespace nova
{

/**
 * simple freelist implementation without any memory allocation features
 * */
class freelist
{
    struct freelist_node
    {
        boost::lockfree::tagged_ptr<freelist_node> next;
    };

    typedef boost::lockfree::tagged_ptr<freelist_node> tagged_ptr;

public:
    freelist(void):
        pool_(NULL)
    {}

    void * pop (void)
    {
        for(;;)
        {
            tagged_ptr old_pool(pool_);

            if (!old_pool)
                return 0;

            freelist_node * new_pool = old_pool->next.get_ptr();

            if (pool_.cas(old_pool, new_pool))
                return reinterpret_cast<void*>(old_pool.get_ptr());
        }
    }

    void push (void * n)
    {
        for(;;)
        {
            tagged_ptr old_pool (pool_);

            freelist_node * new_pool = reinterpret_cast<freelist_node*>(n);

            new_pool->next.set_ptr(old_pool.get_ptr());

            if (pool_.cas(old_pool, new_pool))
                return;
        }
    }

private:
    tagged_ptr pool_;
};


} /* namespace nova */

#endif /* UTILITIES_FREELIST_HPP */
