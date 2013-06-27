/*
	SuperCollider real time audio synthesis system
    Copyright (c) 2002 James McCartney. All rights reserved.
	http://www.audiosynth.com

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
*/


#ifndef _SC_Lock_
#define _SC_Lock_

#if __cplusplus < 201103L
#include <boost/chrono.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread.hpp>

typedef boost::mutex SC_Lock;
typedef boost::timed_mutex timed_mutex;
using boost::lock_guard;
using boost::unique_lock;
using boost::cv_status;
typedef boost::condition_variable_any condition_variable_any;
namespace mutex_chrono = boost::chrono;
using boost::thread;
namespace this_thread = boost::this_thread;
namespace chrono = boost::chrono;
namespace thread_namespace = boost;

#else
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>

typedef std::mutex SC_Lock;
typedef std::timed_mutex timed_mutex;
using std::lock_guard;
using std::unique_lock;
using std::cv_status;
typedef std::condition_variable_any condition_variable_any;
namespace mutex_chrono = std::chrono;
using std::thread;
namespace this_thread = std::this_thread;
namespace chrono = std::chrono;
namespace thread_namespace = std;

#endif

#endif
