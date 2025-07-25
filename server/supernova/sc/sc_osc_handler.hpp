//  osc handler for supercollider-style communication
//  Copyright (C) 2009, 2010 Tim Blechmann
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

#include <algorithm>
#include <mutex>
#include <vector>

#ifndef BOOST_ASIO_HAS_STD_ARRAY
#    ifdef __clang__ // clang workaround
#        define BOOST_ASIO_HAS_STD_ARRAY
#    endif
#endif

// AppleClang workaround
#if defined(__apple_build_version__) && __apple_build_version__ > 10000000
#    define BOOST_ASIO_HAS_STD_STRING_VIEW 1
#endif

// libc++ workaround
#if defined(_LIBCPP_VERSION) && _LIBCPP_VERSION >= 7000 && _LIBCPP_VERSION < 9000
#    define BOOST_ASIO_HAS_STD_STRING_VIEW 1
#endif

#include <boost/asio/ip/tcp.hpp>

#include <boost/enable_shared_from_this.hpp>
#include <boost/date_time/microsec_time_clock.hpp>
#include <boost/intrusive/treap_set.hpp>

#include <boost/endian/arithmetic.hpp>

#include "osc/OscReceivedElements.h"

#include "../server/memory_pool.hpp"
#include "../server/server_args.hpp"
#include "../server/server_scheduler.hpp"
#include "../utilities/osc_server.hpp"
#include "../utilities/sized_array.hpp"
#include "../utilities/static_pool.hpp"
#include "../utilities/time_tag.hpp"

struct FifoMsg;

namespace nova {

typedef bool (*AsyncStageFn)(World* inWorld, void* cmdData);
typedef void (*AsyncFreeFn)(World* inWorld, void* cmdData);

namespace detail {

using namespace boost::asio;
using namespace boost::asio::ip;

struct nova_endpoint : public std::enable_shared_from_this<nova_endpoint> {
    virtual void send(const char* data, size_t length) = 0;
};

class udp_endpoint : public nova_endpoint {
public:
    udp_endpoint(udp::endpoint const& ep): endpoint_(ep) {}

    bool operator==(udp_endpoint const& rhs) const { return endpoint_ == rhs.endpoint_; }

private:
    void send(const char* data, size_t length) override final;

    udp::endpoint endpoint_;
};

typedef std::shared_ptr<nova_endpoint> endpoint_ptr;

/**
 * observer to receive osc notifications
 * */
class sc_notify_observers {
    typedef std::vector<endpoint_ptr> observer_vector;

public:
    typedef enum { no_error = 0, already_registered = -1, not_registered = -2 } error_code;

    sc_notify_observers(boost::asio::io_context& io_context): udp_socket(io_context) {}

    int add_observer(endpoint_ptr const& ep);
    int remove_observer(endpoint_ptr const& ep);

    /* @{ */
    /** notifications, should be called from the real-time thread */
    void notification_node_started(const server_node* node) { notify("/n_go", node); }

    void notification_node_ended(const server_node* node) { notify("/n_end", node); }

    void notification_node_turned_off(const server_node* node) { notify("/n_off", node); }

    void notification_node_turned_on(const server_node* node) { notify("/n_on", node); }

    void notification_node_moved(const server_node* node) { notify("/n_move", node); }

    void send_trigger(int32_t node_id, int32_t trigger_id, float value);

    void send_node_reply(int32_t node_id, int reply_id, const char* command_name, int argument_count,
                         const float* values);
    /* @} */

    /** send notifications, should not be called from the real-time thread */
    void send_notification(const char* data, size_t length);

    void send_udp(const char* data, size_t size, udp::endpoint const& receiver);

    static const char* error_string(error_code);

private:
    observer_vector::iterator find(endpoint_ptr const& ep);

    void notify(const char* address_pattern, const server_node* node) const;
    void send_notification(const char* data, size_t length, nova_endpoint* endpoint);

    observer_vector observers;

protected:
    udp::socket udp_socket;
    std::mutex udp_mutex;
};

class sc_scheduled_bundles {
public:
    struct bundle_node : public boost::intrusive::bs_set_base_hook<> {
        bundle_node(time_tag const& timeout, const char* data, endpoint_ptr const& endpoint):
            timeout_(timeout),
            data_(data),
            endpoint_(endpoint) {}

        void run(void);

        const time_tag timeout_;
        const char* const data_;
        endpoint_ptr endpoint_;

        friend bool operator<(const bundle_node& lhs, const bundle_node& rhs) { return priority_order(lhs, rhs); }

        friend bool priority_order(const bundle_node& lhs, const bundle_node& rhs) {
            return lhs.timeout_ < rhs.timeout_; // lower value, higher priority
        }
    };

    typedef boost::intrusive::treap_multiset<bundle_node> bundle_queue_t;

    void insert_bundle(time_tag const& timeout, const char* data, size_t length, endpoint_ptr const& endpoint);

    void execute_bundles(time_tag const& last, time_tag const& now);

    void clear_bundles(void) { bundle_q.clear_and_dispose(dispose_bundle); }

    static void dispose_bundle(bundle_node* node) {
        node->~bundle_node();
        rt_pool.free(node);
    }

private:
    bundle_queue_t bundle_q;
};

class sc_osc_handler : private detail::network_thread, public sc_notify_observers {
    /* @{ */
    /** constructor helpers */
    void open_tcp_acceptor(ip::address address, unsigned int port);
    void open_udp_socket(ip::address address, unsigned int port);
    bool open_socket(int protocol, ip::address address, unsigned int port);
    /* @} */

public:
    sc_osc_handler(server_arguments const& args):
        sc_notify_observers(detail::network_thread::io_context_),
        tcp_acceptor_(detail::network_thread::io_context_),
        tcp_password_(args.server_password.size() ? args.server_password.c_str() : nullptr) {
        if (!args.non_rt) {
            if (args.tcp_port && !open_socket(IPPROTO_TCP, args.socket_address, args.tcp_port))
                throw std::runtime_error("cannot open socket");
            if (args.udp_port && !open_socket(IPPROTO_UDP, args.socket_address, args.udp_port))
                throw std::runtime_error("cannot open socket");
        }
    }

    void start_receive_thread(void) { detail::network_thread::start_receive(); }

    typedef osc::ReceivedPacket ReceivedPacket;
    typedef osc::ReceivedBundle ReceivedBundle;
    typedef osc::ReceivedMessage ReceivedMessage;

    class received_packet : public audio_sync_callback {
        received_packet(const char* dat, size_t length, endpoint_ptr const& endpoint):
            data(dat),
            length(length),
            endpoint_(endpoint) {}

        void* operator new(std::size_t size, void* ptr) { return ::operator new(size, ptr); }

    public:
        static received_packet* alloc_packet(const char* data, size_t length, endpoint_ptr const& remote_endpoint);

        void run(void) override final;

        const char* const data;
        const size_t length;
        endpoint_ptr endpoint_;
    };

private:
    /* @{ */
    /** udp socket handling */
    void start_receive_udp();
    void handle_receive_udp(const boost::system::error_code& error, std::size_t bytes_transferred);
    /* @} */

    /* @{ */
    /** tcp connection handling */
public:
    class tcp_connection : public nova_endpoint {
    public:
        using pointer = std::shared_ptr<tcp_connection>;
        using executor = tcp::socket::executor_type;

        static pointer create(const executor& executor) { return pointer(new tcp_connection(executor)); }

        tcp::socket& socket() { return socket_; }

        void start(sc_osc_handler* self);

        bool operator==(tcp_connection const& rhs) const { return &rhs == this; }

    private:
        tcp_connection(const executor& executor): socket_(executor) {}

        void send(const char* data, size_t length) override final;

        void async_read_msg_size();
        void handle_message_size();
        void handle_message();

        tcp::socket socket_;
        sc_osc_handler* osc_handler;
        boost::endian::big_int32_t msg_size_;
        std::vector<char> msg_buffer_;
        std::mutex socket_mutex_;
    };

private:
    void start_tcp_accept(void);
    void handle_tcp_accept(tcp_connection::pointer new_connection, const boost::system::error_code& error);
    /* @} */

public:
    void dumpOSC(int i) { dump_osc_packets = i; }

private:
    int dump_osc_packets = 0;

    /* @{ */
public:
    /** \todo how to handle temporary message error suppression? */

    void set_error_posting(int val) { error_posting = val; }

    int get_error_posting() { return error_posting; }

private:
    int error_posting = 1;
    /* @} */

    /* @{ */
    /** packet handling */
public:
    void handle_packet_async(const char* data, size_t length, endpoint_ptr const& endpoint);
    void handle_packet(const char* data, size_t length, endpoint_ptr const& endpoint);
    time_tag handle_bundle_nrt(const char* data_, std::size_t length);

private:
    template <bool realtime> void handle_bundle(ReceivedBundle const& bundle, endpoint_ptr const& endpoint);
    template <bool realtime>
    void handle_message(ReceivedMessage const& message, size_t msg_size, endpoint_ptr const& endpoint);
    template <bool realtime>
    void handle_message_int_address(ReceivedMessage const& message, size_t msg_size, endpoint_ptr const& endpoint);
    template <bool realtime>
    void handle_message_sym_address(ReceivedMessage const& message, size_t msg_size, endpoint_ptr const& endpoint);

    friend struct sc_scheduled_bundles::bundle_node;
    /* @} */

    /* @{ */
    /** bundle scheduling */
public:
    void clear_scheduled_bundles(void) { scheduled_bundles.clear_bundles(); }

    void execute_scheduled_bundles(void) { scheduled_bundles.execute_bundles(last, now); }

    void increment_logical_time(time_tag const& diff) {
        last = now;
        now += diff;
    }

    void set_last_now(time_tag const& lasts, time_tag const& nows) {
        now = nows;
        last = lasts;
    }

    void add_last_now(time_tag const& add) {
        now += add;
        last += add;
    }

    void update_time_from_system(void) {
        now = time_tag::from_ptime(boost::date_time::microsec_clock<boost::posix_time::ptime>::universal_time());
        last = now - time_per_tick;
    }

    time_tag const& current_time(void) const { return now; }
    time_tag next_time(void) const { return now + time_per_tick; }

    sc_scheduled_bundles scheduled_bundles;
    time_tag now, last;
    time_tag time_per_tick;
    /* @} */

    void do_asynchronous_command(World* world, void* replyAddr, const char* cmdName, void* cmdData, AsyncStageFn stage2,
                                 AsyncStageFn stage3, AsyncStageFn stage4, AsyncFreeFn cleanup, int completionMsgSize,
                                 void* completionMsgData) const;

    void send_message_from_RT(const World* world, FifoMsg& msg) const;

    void send_message_to_RT(const World* world, FifoMsg& msg) const;

    bool quit_received = false;

private:
    /* @{ */
    udp::endpoint udp_remote_endpoint_;

    tcp::acceptor tcp_acceptor_;
    const char* tcp_password_; /* we are not owning this! */

    std::array<char, 1 << 15> recv_buffer_;

    static constexpr int udp_receive_buffer_size = 4 * 1024 * 1024;
    static constexpr int udp_send_buffer_size = 4 * 1024 * 1024;
    static constexpr int udp_fallback_buffer_size = 1 * 1024 * 1024;
    /* @} */
};

} /* namespace detail */

using detail::sc_osc_handler;

} /* namespace nova */
