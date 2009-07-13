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

#include <lockfree/fifo.hpp>

#include "osc/OscReceivedElements.h"

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

class sc_osc_handler:
    private detail::network_thread
{
public:
    sc_osc_handler(unsigned int port):
        socket_(detail::network_thread::io_service_, udp::endpoint(udp::v4(), port))
    {
        start_receive();
    }

    ~sc_osc_handler(void)
    {}

    typedef osc::ReceivedPacket osc_received_packet;
    typedef osc::ReceivedBundle received_bundle;
    typedef osc::ReceivedMessage received_message;

private:
    struct received_packet:
        public audio_sync_callback
    {
        received_packet(const char * dat, size_t length, udp::endpoint const & endpoint):
            data(dat), length(length), endpoint_(endpoint)
        {
#if 1
            char * cpy = (char*)received_packet::allocate(length);
            memcpy(cpy, dat, length);
            data = cpy;
#endif
        }

#if 1
        ~received_packet(void)
        {
            deallocate(const_cast<char*>(data));
        }
#endif

        static received_packet * alloc_packet(const char * data, size_t length,
                                              udp::endpoint const & remote_endpoint);

        void run(void);

        const char * data;
        size_t length;
        udp::endpoint endpoint_;
    };

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
    void handle_packet(const char * data, size_t length);
    /* @} */

    /* @{ */
    /** packet handling */
    void handle_bundle(received_packet * bundle);
    void handle_message(received_packet * message);
    bool handle_bundle(received_bundle const & bundle, received_packet * packet);
    void handle_message(received_message const & message, received_packet * packet);
    void handle_message_int_address(received_message const & message, received_packet * packet);
    void handle_message_sym_address(received_message const & message, received_packet * packet);
    /* @} */

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
