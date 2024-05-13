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

#pragma once

#include <boost/lockfree/detail/tagged_ptr.hpp>
#include <atomic>

namespace nova {

/**
 * simple freelist implementation without any memory allocation features
 * */
class freelist {
    struct freelist_node {
        boost::lockfree::detail::tagged_ptr<freelist_node> next;
    };

    typedef boost::lockfree::detail::tagged_ptr<freelist_node> tagged_ptr;

public:
    freelist(void): pool_(tagged_ptr(nullptr)) {}

    freelist(freelist const& rhs) = delete;
    freelist& operator=(freelist const& rhs) = delete;

    void* pop(void) {
        for (;;) {
            tagged_ptr old_pool = pool_.load(std::memory_order_consume);

            if (!old_pool.get_ptr())
                return nullptr;

            freelist_node* new_pool_ptr = old_pool->next.get_ptr();
            tagged_ptr new_pool(new_pool_ptr, old_pool.get_tag() + 1);

            if (pool_.compare_exchange_weak(old_pool, new_pool)) {
                void* ptr = old_pool.get_ptr();
                return reinterpret_cast<void*>(ptr);
            }
        }
    }

    void push(void* n) {
        void* node = n;
        for (;;) {
            tagged_ptr old_pool = pool_.load(std::memory_order_consume);

            freelist_node* new_pool_ptr = reinterpret_cast<freelist_node*>(node);
            tagged_ptr new_pool(new_pool_ptr, old_pool.get_tag() + 1);

            new_pool->next.set_ptr(old_pool.get_ptr());

            if (pool_.compare_exchange_weak(old_pool, new_pool))
                return;
        }
    }

private:
    std::atomic<tagged_ptr> pool_;
};


} /* namespace nova */
