//  templated callback system
//  Copyright (C) 2008, 2009 Tim Blechmann
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

#include <memory>
#include <exception>
#include <iostream>

#include <boost/checked_delete.hpp>

#include <boost/lockfree/spsc_queue.hpp>
#include <boost/lockfree/queue.hpp>


namespace nova {

/** \brief simple templated callback system, using a lockfree fifo */
template <class callback_type, bool mpmc = true, class callback_deleter = boost::checked_deleter<callback_type>>
class callback_system : private callback_deleter {
    typedef typename boost::mpl::if_c<mpmc, boost::lockfree::queue<callback_type*>,
                                      boost::lockfree::spsc_queue<callback_type*>>::type queue_type;

public:
    callback_system(size_t element_count = 2048): callbacks(element_count) {}

    /** \brief adds a new Callback to the Scheduler, threadsafe */
    inline void add_callback(callback_type* cb) { callbacks.push(cb); }

    /** \brief run all callbacks */
    inline void run_callbacks(void) {
        callbacks.consume_all([this](callback_type* cb) { run_callback(cb); });
    }

    /** \brief run one callback
     *
     * assumes, that the queue contains at least one callback
     *
     * */
    void run_callback(void) {
        callbacks.consume_one([this](callback_type* cb) { run_callback(cb); });
    }

private:
    /** run a callback, handle exceptions */
    bool run_callback(callback_type* runme) {
        bool ret;
        try {
            runme->run();
            ret = true;
        } catch (std::exception const& e) {
            std::cout << "unhandled exception while running callback: " << e.what() << std::endl;
            ret = false;
        }
        callback_deleter::operator()(runme);
        return ret;
    }

protected:
    queue_type callbacks; /**< \brief fifo for callbacks */
};

} /* namespace nova */
