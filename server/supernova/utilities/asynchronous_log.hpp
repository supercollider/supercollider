//  class for asynchronous logging
//  Copyright (C) 2011 Tim Blechmann
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

#include <array>
#include <cstring>
#include <cstdio>
#include <cstdarg>
#include <thread>


#include <boost/lockfree/spsc_queue.hpp>
#include <boost/mpl/if.hpp>
#include <boost/sync/semaphore.hpp>

namespace nova {
namespace asynchronous_log_impl {

struct asynchronous_log : boost::noncopyable {
    bool log_printf(const char* fmt, ...) {
        va_list vargs;
        va_start(vargs, fmt);

        bool result = log_printf(fmt, vargs);
        va_end(vargs);
        return result;
    }

    bool log_printf(const char* fmt, va_list vargs) {
        std::array<char, 4096> scratchpad;
        size_t print_result = vsnprintf(scratchpad.data(), scratchpad.size(), fmt, vargs);

        if (print_result >= scratchpad.size())
            fprintf(stderr, "warning: log message truncated");

        return log(scratchpad.data(), print_result);
    }

    bool log(const char* string) {
        size_t length = strlen(string);
        return log(string, length);
    }

    bool log(const char* string, size_t length) {
        size_t total_enqueued = buffer.push(string, length);
        if (total_enqueued == 0)
            return false;

        sem.post();

        if (total_enqueued == length)
            return true;

        string += total_enqueued;
        length -= total_enqueued;

        for (;;) {
            size_t enqueued = buffer.push(string, length);
            if (enqueued == 0)
                continue;

            sem.post();

            if (enqueued == length)
                break;

            string += enqueued;
            length -= enqueued;
        }

        return true;
    }

    size_t read_log(char* out_buffer, size_t size) {
        if (sem.try_wait())
            return read(out_buffer, size);
        else
            return 0;
    }

    size_t read_log_waiting(char* out_buffer, size_t size) {
        sem.wait();
        return read(out_buffer, size);
    }

    size_t read(char* out_buffer, size_t size) { return buffer.pop(out_buffer, size); }

    void interrrupt(void) { sem.post(); }

private:
    boost::lockfree::spsc_queue<char, boost::lockfree::capacity<262144>> buffer;
    boost::sync::semaphore sem;
};

struct asynchronous_log_thread : asynchronous_log {
public:
    asynchronous_log_thread(void): running_flag(true), thread_(std::bind(&asynchronous_log_thread::run, this)) {}

    ~asynchronous_log_thread(void) {
        running_flag = false;
        interrrupt();
        thread_.join();
    }

    void run(void) {
        while (running_flag.load()) {
            size_t read_chars = read_log_waiting(out_buffer.data(), out_buffer.size());
            post_outbuffer(read_chars);

            while (read_chars == out_buffer.size()) {
                read_chars = read(out_buffer.data(), out_buffer.size());
                post_outbuffer(read_chars);
            }
        }
        size_t read_chars = read_log(out_buffer.data(), out_buffer.size());
        post_outbuffer(read_chars);
    }

    void post_outbuffer(size_t read_chars) {
        for (size_t i = 0; i != read_chars; ++i)
            putchar(out_buffer[i]);
        fflush(stdout);
    }

private:
    std::atomic_bool running_flag;
    std::thread thread_;
    std::array<char, 4096> out_buffer;
};

} /* namespace asynchronous_log_impl */

using asynchronous_log_impl::asynchronous_log;
using asynchronous_log_impl::asynchronous_log_thread;

} /* namespace nova */
