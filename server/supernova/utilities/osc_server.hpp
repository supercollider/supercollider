//  osc responder class
//  Copyright (C) 2008 Tim Blechmann
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

#ifndef OSC_SERVER_HPP
#define OSC_SERVER_HPP

#include <string>
#include <iostream>

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>

#include "branch_hints.hpp"
#include "osc_dispatcher.hpp"

#include "static_allocator.hpp"
#include "nova-tt/semaphore.hpp"
#include "nova-tt/thread_priority.hpp"

namespace nova
{

namespace detail
{
using namespace boost::asio;
using namespace boost::asio::ip;

class network_thread
{
public:
    network_thread(void):
        thread_(network_thread::start, this)
    {
        sem.wait();
    }

    ~network_thread(void)
    {
        io_service_.stop();
        thread_.join();
    }

    io_service & get_io_service(void)
    {
        return io_service_;
    }

    void send_udp(const char * data, unsigned int size, udp::endpoint const & receiver)
    {
        udp::socket socket(io_service_);
        socket.open(udp::v4());
        socket.send_to(boost::asio::buffer(data, size), receiver);
    }

private:
    static void start(network_thread * self)
    {
#ifdef NOVA_TT_PRIORITY_RT
        thread_set_priority_rt(thread_priority_interval_rt().first);
#endif
        self->run();
    }

    void run(void)
    {
        sem.post();

        io_service::work work(io_service_);
        io_service_.run();
    }

protected:
    io_service io_service_;

private:
    semaphore sem;
    boost::thread thread_;
};

class osc_server:
    private network_thread,
    private osc_dispatcher
{
public:
    osc_server(unsigned int port):
        socket_(network_thread::io_service_, udp::endpoint(udp::v4(), port))
    {
        start_receive();
    }

    void add_responder(std::string const & address, osc_responder * resp)
    {
        osc_dispatcher::add_responder(address, resp);
    }

    void remove_responder(std::string const & address, osc_responder * resp)
    {
        osc_dispatcher::remove_responder(address, resp);
    }

private:
    void start_receive(void)
    {
        socket_.async_receive_from(
            buffer(recv_buffer_), remote_endpoint_,
            boost::bind(&osc_server::handle_receive, this,
                        placeholders::error, placeholders::bytes_transferred));
    }

    void handle_receive(const boost::system::error_code& error,
                        std::size_t bytes_transferred)
    {
        if (unlikely(error == error::operation_aborted))
            return;    /* we're done */

        if (error == error::message_size)
        {
            overflow_vector.insert(overflow_vector.end(),
                                   recv_buffer_.begin(), recv_buffer_.end());
            return;
        }

        if (error)
        {
            std::cout << "osc_server received error code " << error << std::endl;
            start_receive();
            return;
        }

        if (overflow_vector.empty())
            osc_dispatcher::handle_packet(recv_buffer_.begin(), bytes_transferred);
        else
        {
            overflow_vector.insert(overflow_vector.end(),
                                   recv_buffer_.begin(), recv_buffer_.end());

            osc_dispatcher::handle_packet(overflow_vector.data(), overflow_vector.size());

            overflow_vector.clear();
        }

        start_receive();
        return;
    }

    udp::socket socket_;
    boost::array<char, 1<<13 > recv_buffer_;
    udp::endpoint remote_endpoint_;

    typedef static_allocator<char, 1<<20> string_allocator; /* 1 MB memory pool ...
                                                             *
                                                             * not thread-safe, but that is
                                                             * not required here
                                                             *
                                                             * */
/*     typedef std::basic_string<char, std::char_traits<char>, string_allocator> string; */
    typedef std::string string;
    string overflow_vector;
};

} /* namespace */

using detail::network_thread;
using detail::osc_server;

} /* namespace nova */

#endif /* OSC_SERVER_HPP */
