//  osc handler for supercollider-style communication
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

#ifndef SERVER_SC_OSC_HANDLER_HPP
#define SERVER_SC_OSC_HANDLER_HPP

#include <vector>
#include <algorithm>

#include <boost/date_time/microsec_time_clock.hpp>
#include <boost/intrusive/list.hpp>

#include "osc/OscReceivedElements.h"

#include "memory_pool.hpp"
#include "server_scheduler.hpp"
#include "../utilities/osc_server.hpp"
#include "../utilities/static_pool.hpp"
#include "../utilities/time_tag.hpp"


namespace nova
{

namespace detail
{
using namespace boost::asio;
using namespace boost::asio::ip;

/**
 * observer to receive osc notifications
 *
 * \todo shall we use a separate thread for observer notifications?
 * \todo for now we are ipv4 only
 * */
class sc_notify_observers
{
    typedef std::vector<udp::endpoint> udp_observer_vector;

public:
    sc_notify_observers(void):
        udp_socket(io_service)
    {
        udp_socket.open(udp::v4());
    }

    void add_observer(udp::endpoint const & ep)
    {
        udp_observers.push_back(ep);
    }

    void remove_observer(udp::endpoint const & ep)
    {
        udp_observer_vector::iterator it = std::find(udp_observers.begin(),
                                                     udp_observers.end(), ep);
        assert (it != udp_observers.end());

        udp_observers.erase(it);
    }

    void send_notification(const char * data, size_t length)
    {
        for (size_t i = 0; i != udp_observers.size(); ++i)
            udp_socket.send_to(boost::asio::buffer(data, length), udp_observers[i]);
    }

private:
    udp_observer_vector udp_observers;
    boost::asio::io_service io_service; /* we have an io_service for our own */
    udp::socket udp_socket;
};

class sc_scheduled_bundles
{
public:
    struct bundle_node:
        public boost::intrusive::list_base_hook<>
    {
        bundle_node(time_tag const & timeout, const char * data, udp::endpoint const & endpoint):
            timeout_(timeout), data_(data), endpoint_(endpoint)
        {}

        void run(void);

        const time_tag timeout_;
        const char * const data_;
        const udp::endpoint endpoint_;
    };

    typedef boost::intrusive::list<bundle_node> bundle_list_t;

    void insert_bundle(time_tag const & timeout, const char * data, size_t length,
                    udp::endpoint const & endpoint);

    void execute_bundles(time_tag const & now);

    void clear_bundles(void)
    {
        while(!bundles.empty())
        {
            bundle_node & front = *bundles.begin();

            bundles.pop_front();
            front.~bundle_node();
            rt_pool.free(&front);
        }
    }

private:
    bundle_list_t bundles;
};

class sc_osc_handler:
    private detail::network_thread,
    public sc_notify_observers
{
public:
    sc_osc_handler(unsigned int port):
        dump_osc_packets(0), error_posting(1),
        socket_(detail::network_thread::io_service_, udp::endpoint(udp::v4(), port))
    {
        start_receive();
    }

    ~sc_osc_handler(void)
    {}

    typedef osc::ReceivedPacket osc_received_packet;
    typedef osc::ReceivedBundle received_bundle;
    typedef osc::ReceivedMessage received_message;

    void send_udp(const char * data, unsigned int size, udp::endpoint const & receiver)
    {
        socket_.send_to(boost::asio::buffer(data, size), receiver);
    }

    struct received_packet:
        public audio_sync_callback
    {
        received_packet(const char * dat, size_t length, udp::endpoint const & endpoint):
            data(dat), length(length), endpoint_(endpoint)
        {}

        void * operator new(std::size_t size, void* ptr)
        {
            return ::operator new(size, ptr);
        }

        static received_packet * alloc_packet(const char * data, size_t length,
                                              udp::endpoint const & remote_endpoint);

        void run(void);

        const char * const data;
        const size_t length;
        const udp::endpoint endpoint_;
    };

private:
    /* @{ */
    /** socket handling */
    void start_receive(void)
    {
        socket_.async_receive_from(
            buffer(recv_buffer_), remote_endpoint_,
            boost::bind(&sc_osc_handler::handle_receive, this,
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
            std::cout << "sc_osc_handler received error code " << error << std::endl;
            start_receive();
            return;
        }

        if (overflow_vector.empty())
            handle_packet(recv_buffer_.begin(), bytes_transferred);
        else
        {
            overflow_vector.insert(overflow_vector.end(),
                                   recv_buffer_.begin(), recv_buffer_.end());

            handle_packet(overflow_vector.data(), overflow_vector.size());

            overflow_vector.clear();
        }

        start_receive();
        return;
    }

    /* handle message, called from receiver thread */
public:
    void dumpOSC(int i)
    {
        dump_osc_packets = i;
    }

private:
    int dump_osc_packets;
    void handle_packet(const char * data, size_t length);
    /* @} */

    /* @{ */
public:
    /** \todo how to handle temporary message error suppression? */

    void set_error_posting(int val)
    {
        error_posting = val;
    }

private:
    int error_posting;
    /* @} */

    /* @{ */
    /** packet handling */
public:
    void handle_packet(const char * data_, std::size_t length, udp::endpoint const & endpoint);
private:
    void handle_bundle(received_bundle const & bundle, udp::endpoint const & endpoint);
    void handle_message(received_message const & message, udp::endpoint const & endpoint);
    void handle_message_int_address(received_message const & message, udp::endpoint const & endpoint);
    void handle_message_sym_address(received_message const & message, udp::endpoint const & endpoint);

    friend class sc_scheduled_bundles::bundle_node;
    /* @} */

    /* @{ */
    /** bundle scheduling */
public:
    void clear_scheduled_bundles(void)
    {
        scheduled_bundles.clear_bundles();
    }

    void execute_scheduled_bundles(void)
    {
        time_tag now = time_tag::from_ptime(boost::date_time::microsec_clock<boost::posix_time::ptime>::universal_time());
        scheduled_bundles.execute_bundles(now);
    }

    sc_scheduled_bundles scheduled_bundles;
    /* @} */

private:
    /* @{ */
    /** socket handling */
    udp::socket socket_;
    udp::endpoint remote_endpoint_;

    boost::array<char, 1<<13 > recv_buffer_;

    std::vector<char> overflow_vector;
    /* @} */
};

} /* namespace detail */

using detail::sc_osc_handler;

} /* namespace nova */


#endif /* SERVER_SC_OSC_HANDLER_HPP */
