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
#include <boost/intrusive/treap_set.hpp>

#include "osc/OscReceivedElements.h"

#include "dynamic_endpoint.hpp"
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
 * */
class sc_notify_observers
{
    typedef std::vector<nova_endpoint> observer_vector;

public:
    sc_notify_observers(void):
        udp_socket(io_service)
    {
        udp_socket.open(udp::v4());
    }

    void add_observer(nova_endpoint const & ep)
    {
        observers.push_back(ep);
    }

    void remove_observer(nova_endpoint const & ep)
    {
        observer_vector::iterator it = std::find(observers.begin(),
                                                 observers.end(), ep);
        assert (it != observers.end());

        observers.erase(it);
    }

    void send_notification(const char * data, size_t length)
    {
        for (size_t i = 0; i != observers.size(); ++i)
            send_notification(data, length, observers[i]);
    }

private:
    void send_notification(const char * data, size_t length, nova_endpoint const & endpoint)
    {
        nova_protocol prot = endpoint.protocol();
        if (prot.family() == AF_INET && prot.type() == SOCK_DGRAM)
        {
            udp::endpoint ep(endpoint.address(), endpoint.port());
            udp_socket.send_to(boost::asio::buffer(data, length), ep);
        }
    }

    observer_vector observers;
    boost::asio::io_service io_service; /* we have an io_service for our own */
    udp::socket udp_socket;
};

class sc_scheduled_bundles
{
public:
    struct bundle_node:
        public boost::intrusive::bs_set_base_hook<>
    {
        bundle_node(time_tag const & timeout, const char * data, nova_endpoint const & endpoint):
            timeout_(timeout), data_(data), endpoint_(endpoint)
        {}

        void run(void);

        const time_tag timeout_;
        const char * const data_;
        const nova_endpoint endpoint_;

        friend bool operator< (const bundle_node & lhs, const bundle_node & rhs)
        {
            return priority_order(lhs, rhs);
        }

        friend bool priority_order (const bundle_node & lhs, const bundle_node & rhs)
        {
            return lhs.timeout_ < rhs.timeout_; // lower value, higher priority
        }
    };

    typedef boost::intrusive::treap_multiset<bundle_node> bundle_queue_t;

    void insert_bundle(time_tag const & timeout, const char * data, size_t length,
                       nova_endpoint const & endpoint);

    void execute_bundles(time_tag const & now);

    void clear_bundles(void)
    {
        bundle_q.clear_and_dispose(dispose_bundle);
    }

    static void dispose_bundle(bundle_node * node)
    {
        node->~bundle_node();
        rt_pool.free(node);
    }

private:
    bundle_queue_t bundle_q;
};

class sc_osc_handler:
    private detail::network_thread,
    public sc_notify_observers
{
    void open_udp_socket(udp const & protocol, unsigned int port)
    {
        udp_socket_.open(protocol);
        udp_socket_.bind(udp::endpoint(protocol, port));
    }

    bool open_socket(int family, int type, int protocol, unsigned int port)
    {
        if (protocol == IPPROTO_TCP)
        {
            return false;
        }
        else if (protocol == IPPROTO_UDP)
        {
            if ( type != SOCK_DGRAM )
                return false;

            if (family == AF_INET)
                open_udp_socket(udp::v4(), port);
            else if (family == AF_INET6)
                open_udp_socket(udp::v6(), port);
            else
                return false;
            start_receive_udp();
            return true;
        }
        return false;
    }

public:
    sc_osc_handler(int family, int type, int protocol, unsigned int port):
        dump_osc_packets(0), error_posting(1),
        udp_socket_(detail::network_thread::io_service_),
        tcp_socket_(detail::network_thread::io_service_)
    {
        if (!open_socket(family, type, protocol, port))
            throw std::runtime_error("cannot open socket");
    }

    ~sc_osc_handler(void)
    {}

    typedef osc::ReceivedPacket osc_received_packet;
    typedef osc::ReceivedBundle received_bundle;
    typedef osc::ReceivedMessage received_message;

    void send(const char * data, size_t size, nova_endpoint const & endpoint)
    {
        nova_protocol prot = endpoint.protocol();
        if (prot.family() == AF_INET && prot.type() == SOCK_DGRAM)
        {
            udp::endpoint ep(endpoint.address(), endpoint.port());
            send_udp(data, size, ep);
        }
    }

    void send_udp(const char * data, unsigned int size, udp::endpoint const & receiver)
    {
        udp_socket_.send_to(boost::asio::buffer(data, size), receiver);
    }

    struct received_packet:
        public audio_sync_callback
    {
        received_packet(const char * dat, size_t length, nova_endpoint const & endpoint):
            data(dat), length(length), endpoint_(endpoint)
        {}

        void * operator new(std::size_t size, void* ptr)
        {
            return ::operator new(size, ptr);
        }

        static received_packet * alloc_packet(const char * data, size_t length,
                                              nova_endpoint const & remote_endpoint);

        void run(void);

        const char * const data;
        const size_t length;
        const nova_endpoint endpoint_;
    };

private:
    /* @{ */
    /** socket handling */
    void start_receive_udp(void)
    {
        udp_socket_.async_receive_from(
            buffer(recv_buffer_), udp_remote_endpoint_,
            boost::bind(&sc_osc_handler::handle_receive_udp, this,
                        placeholders::error, placeholders::bytes_transferred));
    }

    void handle_receive_udp(const boost::system::error_code& error,
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
            start_receive_udp();
            return;
        }

        if (overflow_vector.empty())
            handle_packet_async(recv_buffer_.begin(), bytes_transferred, udp_remote_endpoint_);
        else
        {
            overflow_vector.insert(overflow_vector.end(),
                                   recv_buffer_.begin(), recv_buffer_.end());

            handle_packet_async(overflow_vector.data(), overflow_vector.size(), udp_remote_endpoint_);

            overflow_vector.clear();
        }

        start_receive_udp();
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
    void handle_packet_async(const char* data, size_t length, nova::nova_endpoint const & endpoint);
    void handle_packet(const char* data, size_t length, nova::nova_endpoint const & endpoint);
    void handle_packet_nrt(const char * data_, std::size_t length);

private:
    template <bool realtime>
    void handle_bundle(received_bundle const & bundle, nova_endpoint const & endpoint);
    template <bool realtime>
    void handle_message(received_message const & message, nova_endpoint const & endpoint);
    template <bool realtime>
    void handle_message_int_address(received_message const & message, nova_endpoint const & endpoint);
    template <bool realtime>
    void handle_message_sym_address(received_message const & message, nova_endpoint const & endpoint);

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
    udp::socket udp_socket_;
    udp::endpoint udp_remote_endpoint_;

    tcp::socket tcp_socket_;

    boost::array<char, 1<<15 > recv_buffer_;

    std::vector<char> overflow_vector;
    /* @} */
};

} /* namespace detail */

using detail::sc_osc_handler;

} /* namespace nova */


#endif /* SERVER_SC_OSC_HANDLER_HPP */
