//  osc handler for supercollider-style communication, implementation
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

#include <iostream>

#include <boost/asio/placeholders.hpp>
#include <boost/asio/read.hpp>
#include <boost/bind.hpp>


#include "osc/OscOutboundPacketStream.h"
#include "osc/OscPrintReceivedElements.h"

#include "sc_msg_iter.h"
#include "sc_osc_handler.hpp"
#include "../server/server.hpp"
#include "utilities/sized_array.hpp"

#include "SC_OSC_Commands.h"

#ifdef _WIN32
#include "malloc.h" // for alloca
#endif

namespace nova {

using namespace std;
using nova::detail::nova_endpoint;
using nova::detail::endpoint_ptr;

namespace {

int32_t last_generated = 0;

server_node * find_node(int32_t target_id)
{
    if (target_id == -1)
        target_id = last_generated;

    server_node * node = instance->find_node(target_id);

    if (node == nullptr)
        log_printf("node not found: %d\n", target_id);

    return node;
}

abstract_group * find_group(int32_t target_id)
{
    if (target_id == -1)
        target_id = last_generated;

    abstract_group * node = instance->find_group(target_id);

    if (node == nullptr)
        log("node not found or not a group\n");
    return node;
}

bool check_node_id(int node_id)
{
    if (!instance->node_id_available(node_id)) {
        log_printf("node id %d already in use\n", node_id);
        return false;
    }
    return true;
}

void fill_notification(const server_node * node, osc::OutboundPacketStream & p)
{
    p << node->id();

    /* parent */
    const abstract_group * parent_node = node->get_parent();
    assert(parent_node);
    p << parent_node->id();

    /* previous/next */
    if (parent_node->is_parallel())
        p << -2 << -2; /* we are in a parallel group, so we have no notion of previous/next */
    else
    {
        const server_node * prev_node = node->previous_node();
        if (prev_node)
            p << prev_node->id();
        else
            p << -1;

        const server_node * next_node = node->next_node();
        if (next_node)
            p << next_node->id();
        else
            p << -1;
    }

    /* is_synth, head, tail */
    if (node->is_synth())
        p << 0;
    else {
        const abstract_group * node_group = static_cast<const abstract_group*>(node);
        p << 1;

        if (node_group->is_parallel())
            p << -2 << -2;
        else
        {
            const group * node_real_group = static_cast<const group*>(node_group);
            if (node_real_group->empty())
                p << -1 << -1;
            else
                p << node_real_group->head_node()->id()
                  << node_real_group->tail_node()->id();
        }
    }

    p << osc::EndMessage;
}

spin_lock system_callback_allocator_lock;

struct movable_string
{
    /** allocate new string, only allowed to be called from the rt context */
    explicit movable_string(const char * str)
    {
        size_t length = strlen(str) + 1; /* terminating \0 */
        char * data = (char*)system_callback::allocate(length);
        strcpy(data, str);
        data_ = data;
    }

    /** copy constructor has move semantics!!! */
    movable_string(movable_string const & rhs)
    {
        data_ = rhs.data_;
        const_cast<movable_string&>(rhs).data_ = nullptr;
    }

    ~movable_string(void)
    {
        if (data_)
            system_callback::deallocate((char*)data_);
    }

    const char * c_str(void) const
    {
        return data_;
    }

private:
    const char * data_ = nullptr;
};

template <typename T>
struct movable_array
{
    /** allocate new array, only allowed to be called from the rt context */
    movable_array(size_t length, const T * data, bool locked = false):
        length_(length)
    {
        data_ = (T*)system_callback::allocate(length * sizeof(T));
        for (size_t i = 0; i != length; ++i)
            data_[i] = data[i];
    }

    /** copy constructor has move semantics!!! */
    movable_array(movable_array const & rhs)
    {
        length_ = rhs.length_;
        data_ = rhs.data_;
        const_cast<movable_array&>(rhs).data_ = nullptr;
    }

    ~movable_array(void)
    {
        if (data_)
            system_callback::deallocate((char*)data_);
    }

    const T * data(void) const
    {
        return data_;
    }

    const T & operator[](size_t index) const
    {
        return data_[index];
    }

    size_t size(void) const
    {
        return length_;
    }

private:
    size_t length_ = 0;
    T * data_;
};

void send_done_message(endpoint_ptr const & endpoint)
{
    char buffer[128];
    osc::OutboundPacketStream p(buffer, 128);
    p << osc::BeginMessage("/done")
      << osc::EndMessage;

    endpoint->send(p.Data(), p.Size());
}

void send_done_message(endpoint_ptr const & endpoint, const char * cmd)
{
    char buffer[128];
    osc::OutboundPacketStream p(buffer, 128);
    p << osc::BeginMessage("/done")
      << cmd
      << osc::EndMessage;

    endpoint->send(p.Data(), p.Size());
}

void send_done_message(endpoint_ptr const & endpoint, const char * cmd, osc::int32 index)
{
    char buffer[128];
    osc::OutboundPacketStream p(buffer, 128);
    p << osc::BeginMessage("/done")
      << cmd
      << index
      << osc::EndMessage;

    endpoint->send(p.Data(), p.Size());
}

void send_fail_message(endpoint_ptr const & endpoint, const char * cmd, const char * content)
{
    char buffer[8192];
    osc::OutboundPacketStream p(buffer, 8192);
    p << osc::BeginMessage("/fail")
      << cmd << content
      << osc::EndMessage;

    endpoint->send(p.Data(), p.Size());
}

void send_fail_message(endpoint_ptr const & endpoint, const char * cmd, const char * content, int id)
{
    char buffer[8192];
    osc::OutboundPacketStream p(buffer, 8192);
    p << osc::BeginMessage("/fail")
      << cmd << content << (osc::int32)id
      << osc::EndMessage;

    endpoint->send(p.Data(), p.Size());
}


template <typename Functor>
struct fn_system_callback:
    public system_callback
{
    fn_system_callback (Functor const & fn):
        fn_(fn)
    {}

    void run(void) override
    {
        fn_();
    }

    Functor fn_;
};

template <typename Functor>
struct fn_sync_callback:
    public audio_sync_callback
{
    fn_sync_callback (Functor const & fn):
        fn_(fn)
    {}

    void run(void) override
    {
        fn_();
    }

    Functor fn_;
};

/** helper class for dispatching real-time and non real-time osc command callbacks
 *
 *  uses template specialization to avoid unnecessary callback rescheduling
 */
template <bool realtime>
struct cmd_dispatcher
{
    template <typename Functor>
    static void fire_system_callback(Functor const & f)
    {
        instance->add_system_callback(new fn_system_callback<Functor>(f));
    }

    template <typename Functor>
    static void fire_io_callback(Functor const & f)
    {
        instance->add_io_callback(new fn_system_callback<Functor>(f));
    }

    template <typename Functor>
    static void fire_rt_callback(Functor const & f)
    {
        instance->add_sync_callback(new fn_sync_callback<Functor>(f));
    }

    // CHECK: can we pass endpoint by reference?
    static void fire_done_message(endpoint_ptr endpoint, const char * cmd, osc::int32 index)
    {
        if (endpoint) {
            fire_io_callback([=]() {
                send_done_message(endpoint, cmd, index);
            });
        }
    }

    static void fire_message(endpoint_ptr endpoint, movable_array<char> & message)
    {
        if (endpoint) {
            fire_io_callback([=]() {
                endpoint->send(message.data(), message.size());
            });
        }
    }
};

template <>
struct cmd_dispatcher<false>
{
    template <typename Functor>
    static void fire_system_callback(Functor f)
    {
        f();
    }

    template <typename Functor>
    static void fire_rt_callback(Functor f)
    {
        f();
    }

    template <typename Functor>
    static void fire_io_callback(Functor f)
    {
        f();
    }

    static void fire_done_message(endpoint_ptr const & endpoint, const char * cmd, osc::int32 index)
    {
        send_done_message (endpoint, cmd, index);
    }

    static void fire_message(endpoint_ptr & endpoint, movable_array<char> & message)
    {
        if (endpoint)
            endpoint->send(message.data(), message.size());
    }
};

void report_failure(endpoint_ptr const & endpoint, std::exception const & error, const char * command)
{
    std::cout << error.what() << std::endl;
    send_fail_message(endpoint, command, error.what());
}

void report_failure(endpoint_ptr const & endpoint, std::exception const & error, const char * command, int bufnum)
{
    std::cout << error.what() << std::endl;
    send_fail_message(endpoint, command, error.what(), bufnum);
}

} /* namespace */

namespace detail {
using nova::log;

void fire_notification(movable_array<char> & msg)
{
    instance->send_notification(msg.data(), msg.size());
}

int sc_notify_observers::add_observer(endpoint_ptr const & ep)
{
    auto it = find(ep);
    if (it != observers.end())
        return already_registered;

    observers.push_back(ep);
    return observers.size() - 1;
}

int sc_notify_observers::remove_observer(endpoint_ptr const & ep)
{
    auto it = find(ep);

    if (it == observers.end())
        return not_registered;

    const int observerIndex = it - observers.begin();
    observers.erase(it);
    return observerIndex;
}

const char * sc_notify_observers::error_string(error_code error)
{
    switch (error) {
    case no_error:
        return "";

    case already_registered:
        return "notify: already registered";

    case not_registered:
        return "notify: not registered";

    default:
        assert(false);
        return "";
    }
}

sc_notify_observers::observer_vector::iterator sc_notify_observers::find(endpoint_ptr const & ep)
{
    for (auto it = observers.begin(); it != observers.end(); ++it) {

        udp_endpoint * elemUDP = dynamic_cast<udp_endpoint*>(it->get());
        udp_endpoint * testUDP = dynamic_cast<udp_endpoint*>(ep.get());
        if (elemUDP && testUDP) {
            if (*elemUDP == *testUDP)
                return it;
        }

        typedef sc_osc_handler::tcp_connection tcp_connection;

        tcp_connection * elemTCP = dynamic_cast<tcp_connection*>(it->get());
        tcp_connection * testTCP = dynamic_cast<tcp_connection*>(ep.get());
        if (elemTCP && testTCP) {
            if (*elemTCP == *testTCP)
                return it;
        }

    }
    return observers.end();
}


void sc_notify_observers::notify(const char * address_pattern, const server_node * node) const
{
    char buffer[128]; // 128 byte should be enough
    osc::OutboundPacketStream p(buffer, 128);
    p << osc::BeginMessage(address_pattern);
    fill_notification(node, p);

    movable_array<char> message(p.Size(), p.Data());
    cmd_dispatcher<true>::fire_io_callback(std::bind(fire_notification, message));
}

void fire_trigger(int32_t node_id, int32_t trigger_id, float value)
{
    char buffer[128]; // 128 byte should be enough
    osc::OutboundPacketStream p(buffer, 128);
    p << osc::BeginMessage("/tr") << osc::int32(node_id) << osc::int32(trigger_id) << value
      << osc::EndMessage;

    instance->send_notification(p.Data(), p.Size());
}

void sc_notify_observers::send_trigger(int32_t node_id, int32_t trigger_id, float value)
{
    cmd_dispatcher<true>::fire_io_callback([=](){
        fire_trigger(node_id, trigger_id, value);
    });
}

void free_mem_callback(movable_string & cmd,
                     movable_array<float> & values)
{}

void fire_node_reply(int32_t node_id, int reply_id, movable_string & cmd,
                     movable_array<float> & values)
{
    size_t buffer_size = 1024 + strlen(cmd.c_str()) + values.size()*sizeof(float);

    char * buffer = (buffer_size < 2048) ? (char*)alloca(buffer_size)
                                         : (char*)malloc(buffer_size);

    try {
        osc::OutboundPacketStream p(buffer, buffer_size);
        p << osc::BeginMessage(cmd.c_str()) << osc::int32(node_id) << osc::int32(reply_id);

        for (int i = 0; i != values.size(); ++i)
            p << values[i];

        p << osc::EndMessage;

        instance->send_notification(p.Data(), p.Size());

        cmd_dispatcher<true>::fire_rt_callback(std::bind(free_mem_callback, cmd, values));
    } catch (...) {
    }

    if (buffer_size >= 2048)
        free(buffer);
}

void sc_notify_observers::send_node_reply(int32_t node_id, int reply_id, const char* command_name,
                                          int argument_count, const float* values)
{
    spin_lock::scoped_lock lock(system_callback_allocator_lock); // called from rt helper threads, so we need to lock the memory pool
    movable_string cmd(command_name);
    movable_array<float> value_array(argument_count, values);

    cmd_dispatcher<true>::fire_io_callback(std::bind(fire_node_reply, node_id, reply_id, cmd, value_array));
}

void sc_notify_observers::send_notification(const char * data, size_t length)
{
    for (auto & observer: observers)
        observer->send(data, length);
}

void udp_endpoint::send(const char *data, size_t length)
{
    instance->sc_notify_observers::send_udp(data, length, endpoint_);
}

void sc_notify_observers::send_udp(const char * data, size_t size, udp::endpoint const & receiver)
{
    std::lock_guard<std::mutex> lock(udp_mutex);
    sc_notify_observers::udp_socket.send_to(boost::asio::buffer(data, size), receiver);
}




void sc_scheduled_bundles::bundle_node::run(void)
{
    typedef osc::ReceivedBundleElement bundle_element;
    typedef osc::ReceivedBundle received_bundle;
    typedef osc::ReceivedMessage received_message;

    bundle_element element(data_);

    if (element.IsBundle()) {
        received_bundle bundle(element);
        instance->handle_bundle<true>(bundle, endpoint_);
    } else {
        received_message message(element);
        instance->handle_message<true>(message, element.Size(), endpoint_);
    }
}

void sc_scheduled_bundles::insert_bundle(time_tag const & timeout, const char * data, size_t length,
                                         endpoint_ptr const & endpoint)
{
    /* allocate chunk from realtime pool */
    void * chunk = rt_pool.malloc(sizeof(bundle_node) + length+4);
    bundle_node * node = (bundle_node*)chunk;
    char * cpy = (char*)chunk + sizeof(bundle_node);

    memcpy(cpy, data - 4, length+4);

    new(node) bundle_node(timeout, cpy, endpoint);

    bundle_q.insert(*node);
}

void sc_scheduled_bundles::execute_bundles(time_tag const & last, time_tag const & now)
{
    World * world = &sc_factory->world;

    while(!bundle_q.empty()) {
        bundle_node & front = *bundle_q.top();
        time_tag const & next_timestamp = front.timeout_;

        if (now < next_timestamp)
            break;

        if (last < next_timestamp) {
            // between last and now
            time_tag time_since_last = next_timestamp - last;
            float samples_since_last = time_since_last.to_samples(world->mSampleRate);

            float sample_offset;
            float subsample_offset = std::modf(samples_since_last, &sample_offset);

            world->mSampleOffset = (int)sample_offset;
            world->mSubsampleOffset = subsample_offset;
        } else
            world->mSampleOffset = world->mSubsampleOffset = 0;

        front.run();
        bundle_q.erase_and_dispose(bundle_q.top(), &dispose_bundle);
    }

    world->mSampleOffset = world->mSubsampleOffset = 0;
}


void sc_osc_handler::open_tcp_acceptor(tcp const & protocol, unsigned int port)
{
    tcp_acceptor_.open(protocol);

    tcp_acceptor_.bind(tcp::endpoint(protocol, port));
    tcp_acceptor_.listen();
    start_tcp_accept();
}

void sc_osc_handler::open_udp_socket(udp const & protocol, unsigned int port)
{
    sc_notify_observers::udp_socket.open(protocol);
    sc_notify_observers::udp_socket.bind(udp::endpoint(protocol, port));
}

bool sc_osc_handler::open_socket(int family, int type, int protocol, unsigned int port)
{
    if (protocol == IPPROTO_TCP)
    {
        if ( type != SOCK_STREAM )
            return false;

        if (family == AF_INET)
            open_tcp_acceptor(tcp::v4(), port);
        else if (family == AF_INET6)
            open_tcp_acceptor(tcp::v6(), port);
        else
            return false;
        return true;
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

void sc_osc_handler::start_receive_udp()
{
    using namespace boost;
    sc_notify_observers::udp_socket.async_receive_from(
                buffer(recv_buffer_), udp_remote_endpoint_,
                bind(&sc_osc_handler::handle_receive_udp, this,
                     asio::placeholders::error, asio::placeholders::bytes_transferred));
}

void sc_osc_handler::handle_receive_udp(const boost::system::error_code& error,
                                        std::size_t bytes_transferred)
{
    if (unlikely(error == error::operation_aborted))
        return;    /* we're done */

    if (error) {
        std::cout << "sc_osc_handler received error code " << error << std::endl;
        start_receive_udp();
        return;
    }

    handle_packet_async(recv_buffer_.begin(), bytes_transferred, make_shared<udp_endpoint>(udp_remote_endpoint_));

    start_receive_udp();
    return;
}

void sc_osc_handler::tcp_connection::start(sc_osc_handler * self)
{
    using namespace boost;
    typedef boost::endian::big_int32_t big_int32_t;
    asio::ip::tcp::no_delay option(true);
    socket_.set_option(option);

    const bool check_password = self->tcp_password_;

    if (check_password) {
        std::array<char, 32> password;
        big_int32_t msglen;
        for (unsigned int i=0; i!=4; ++i) {
            size_t size = socket_.receive(asio::buffer(&msglen, 4));
            if (size != sizeof(big_int32_t))
                return;

            if (msglen > password.size())
                return;

            size = socket_.receive(asio::buffer(password.data(), msglen));

            bool verified = true;
            if (size != msglen || strcmp(password.data(), self->tcp_password_) != 0)
                verified = false;

            if (!verified)
                throw std::runtime_error("cannot verify password");
        }
    }

    osc_handler = self;

    async_read_msg_size();
}


void sc_osc_handler::tcp_connection::send(const char *data, size_t length)
{
    try {
        boost::endian::big_int32_t len(length);

        socket_.send(boost::asio::buffer(&len, sizeof(len)));
        size_t written = socket_.send(boost::asio::buffer(data, length));
        assert(length == written);
    } catch (std::exception const & err) {
        std::cout << "Exception when sending message over TCP: " << err.what();
    }
}


void sc_osc_handler::tcp_connection::async_read_msg_size()
{
    namespace asio = boost::asio;
    pointer ptr = std::static_pointer_cast<sc_osc_handler::tcp_connection>(shared_from_this());

    asio::async_read(socket_, asio::buffer(&msg_size_, 4), [=] (const boost::system::error_code& error, std::size_t bytes_transferred) {
        if (error == boost::asio::error::eof)
            return; // connection closed

        if (error) {
            cout << "tcp_connection received error: " << error.message() << endl;
            return;
        }

        ptr->handle_message_size();
    });
}

void sc_osc_handler::tcp_connection::handle_message_size()
{
    msg_buffer_.resize(msg_size_);

    namespace asio = boost::asio;
    pointer ptr = std::static_pointer_cast<sc_osc_handler::tcp_connection>(shared_from_this());
    asio::async_read(socket_, asio::buffer(msg_buffer_), [=] (const boost::system::error_code& error, std::size_t bytes_transferred) {
        if (error == boost::asio::error::eof)
            return; // connection closed

        if (error) {
            cout << "tcp_connection received error: " << error.message() << endl;
            return;
        }
        assert(bytes_transferred == ptr->msg_size_);

        ptr->handle_message();
    });
}

void sc_osc_handler::tcp_connection::handle_message()
{
    assert(msg_size_ == msg_buffer_.size());

    osc_handler->handle_packet_async( msg_buffer_.data(), msg_buffer_.size(), shared_from_this() );
    async_read_msg_size();
}




void sc_osc_handler::start_tcp_accept(void)
{
    tcp_connection::pointer new_connection = tcp_connection::create(tcp_acceptor_.get_io_service());

    tcp_acceptor_.async_accept(new_connection->socket(),
                               boost::bind(&sc_osc_handler::handle_tcp_accept, this, new_connection,
                                           boost::asio::placeholders::error));
}

void sc_osc_handler::handle_tcp_accept(tcp_connection::pointer new_connection,
                                       const boost::system::error_code& error)
{
    if (!error)
        new_connection->start(this);

    start_tcp_accept();
}


void sc_osc_handler::handle_packet_async(const char * data, size_t length,
                                         endpoint_ptr const & endpoint)
{
    received_packet * p = received_packet::alloc_packet(data, length, endpoint);
    if( !p )
        return;

    if (dump_osc_packets == 1) {
        osc_received_packet packet (data, length);

        if (packet.IsMessage()) {
            received_message message (packet);

            const char * address = message.AddressPattern();
            if (strcmp(address, "/status") != 0) // we ignore /status messages
                cout << "received osc message " << message << endl;
        } else
            cout << "received osc bundle " << packet << endl;
    }

    instance->add_sync_callback(p);
}

time_tag sc_osc_handler::handle_bundle_nrt(const char * data, size_t length)
{
    osc_received_packet packet(data, length);
    if (!packet.IsBundle())
        throw std::runtime_error("packet needs to be an osc bundle");

    received_bundle bundle(packet);
    handle_bundle<false> (bundle, nullptr);
    return bundle.TimeTag();
}


sc_osc_handler::received_packet *
sc_osc_handler::received_packet::alloc_packet(const char * data, size_t length,
                                              endpoint_ptr const & remote_endpoint)
{
    /* received_packet struct and data array are located in one memory chunk */
    void * chunk = received_packet::allocate(sizeof(received_packet) + length);
    if( !chunk ) {
        std::cerr << "Memory allocation failure: OSC message not handled\n";
        return nullptr;
    }

    received_packet * p = (received_packet*)chunk;
    char * cpy = (char*)(chunk) + sizeof(received_packet);
    memcpy(cpy, data, length);

    new(p) received_packet(cpy, length, remote_endpoint);
    return p;
}

void sc_osc_handler::received_packet::run(void)
{
    instance->handle_packet(data, length, endpoint_);
}

void sc_osc_handler::handle_packet(const char * data, std::size_t length, endpoint_ptr const & endpoint)
{
    osc_received_packet packet(data, length);
    if (packet.IsBundle()) {
        received_bundle bundle(packet);
        handle_bundle<true> (bundle, endpoint);
    } else {
        received_message message(packet);
        handle_message<true> (message, packet.Size(), endpoint);
    }
}

template <bool realtime>
void sc_osc_handler::handle_bundle(received_bundle const & bundle, endpoint_ptr const & endpoint)
{
    time_tag bundle_time = bundle.TimeTag();

    typedef osc::ReceivedBundleElementIterator bundle_iterator;
    typedef osc::ReceivedBundleElement bundle_element;

    if (bundle_time <= now) {
        if (!bundle_time.is_immediate()) {
            time_tag late = now - bundle_time;
            log_printf("late: %f\n", late.to_seconds());
        };
        for (bundle_iterator it = bundle.ElementsBegin(); it != bundle.ElementsEnd(); ++it) {
            bundle_element const & element = *it;

            if (element.IsBundle()) {
                received_bundle inner_bundle(element);
                handle_bundle<realtime>(inner_bundle, endpoint);
            } else {
                received_message message(element);
                handle_message<realtime>(message, element.Size(), endpoint);
            }
        }
    } else {
        for (bundle_iterator it = bundle.ElementsBegin(); it != bundle.ElementsEnd(); ++it) {
            bundle_element const & element = *it;
            scheduled_bundles.insert_bundle(bundle_time, element.Contents(), element.Size(), endpoint);
        }
    }
}

template <bool realtime>
void sc_osc_handler::handle_message(received_message const & message, size_t msg_size,
                                    endpoint_ptr const & endpoint)
{
    try {
        if (message.AddressPatternIsUInt32())
            handle_message_int_address<realtime>(message, msg_size, endpoint);
        else
            handle_message_sym_address<realtime>(message, msg_size, endpoint);
    } catch (std::exception const & e) {
        log_printf("exception in handle_message: %s\n", e.what());
    }
}

namespace {

typedef sc_osc_handler::received_message received_message;

int first_arg_as_int(received_message const & message)
{
    osc::ReceivedMessageArgumentStream args = message.ArgumentStream();
    osc::int32 val;

    args >> val;

    return val;
}

template <bool realtime>
void handle_quit(endpoint_ptr endpoint)
{
    instance->quit_received = true;
    cmd_dispatcher<realtime>::fire_system_callback( [=] () {
        instance->prepare_to_terminate();
        send_done_message(endpoint, "/quit");
        instance->terminate();
    });
}

template <bool realtime>
void handle_notify(received_message const & message, endpoint_ptr endpoint)
{
    int enable = first_arg_as_int(message);

    cmd_dispatcher<realtime>::fire_system_callback( [=]() {

        int observer = 0;

        if (enable) {
            observer = instance->add_observer(endpoint);

            if (observer < 0)
                send_fail_message(endpoint, "/notify", sc_notify_observers::error_string( (sc_notify_observers::error_code)observer ));
        } else {
            observer = instance->remove_observer(endpoint);
            if (observer < 0)
                send_fail_message(endpoint, "/notify", sc_notify_observers::error_string( (sc_notify_observers::error_code)observer ));
        }

        if (observer >= 0)
            send_done_message(endpoint, "/notify", observer);
    });
}

template <bool realtime>
void handle_status(endpoint_ptr endpoint)
{
    cmd_dispatcher<realtime>::fire_io_callback([=]() {
        if (unlikely(instance->quit_received)) // we don't reply once we are about to quit
            return;

        char buffer[1024];
        typedef osc::int32 i32;

        float peak_load, average_load;
        instance->cpu_load(peak_load, average_load);

        osc::OutboundPacketStream p(buffer, 1024);
        p << osc::BeginMessage("/status.reply")
          << (i32)1                                 /* unused */
          << (i32)sc_factory->ugen_count()          /* ugens */
          << (i32)instance->synth_count()           /* synths */
          << (i32)instance->group_count()           /* groups */
          << (i32)instance->definition_count()      /* synthdefs */
          << average_load                           /* average cpu % */
          << peak_load                              /* peak cpu % */
          << instance->get_samplerate()             /* nominal samplerate */
          << instance->smooth_samplerate             /* actual samplerate */
          << osc::EndMessage;

        endpoint->send(p.Data(), p.Size());
    });
}

void handle_dumpOSC(received_message const & message)
{
    int val = first_arg_as_int(message);
    val = min (1, val);    /* we just support one way of dumping osc messages */

    instance->dumpOSC(val);     /* thread-safe */
}

template <bool realtime>
void handle_sync(received_message const & message, endpoint_ptr endpoint)
{
    int id = first_arg_as_int(message);

    // ping pong: we go through the nrt->rt channel to ensure that earlier messages have been completely dispatched
    cmd_dispatcher<realtime>::fire_system_callback([=]() {
        cmd_dispatcher<realtime>::fire_rt_callback([=]() {
            cmd_dispatcher<realtime>::fire_io_callback([=]() {
                char buffer[128];
                osc::OutboundPacketStream p(buffer, 128);
                p << osc::BeginMessage("/synced")
                  << id
                  << osc::EndMessage;

                endpoint->send(p.Data(), p.Size());
            });
        });
    });
}

void handle_clearSched(void)
{
    instance->clear_scheduled_bundles();
}

void handle_error(received_message const & message)
{
    int val = first_arg_as_int(message);

    instance->set_error_posting(val);     /* thread-safe */
}

void handle_unhandled_message(received_message const & msg)
{
    log_printf("unhandled message: %s\n", msg.AddressPattern());
}

static bool node_position_sanity_check(node_position_constraint const & constraint)
{
    switch (constraint.second) {
    case head:
    case tail:
    case insert: {
        server_node * target = constraint.first;
        if (!target->is_group()) {
            log_printf("Invalid position constraint (target: %d, addAction: %d)\n", target->id(), constraint.second);
            return false;
        }
        break;
    }
    case before:
    case after:
    case replace:
        break;
    }

    return true;
}

sc_synth * add_synth(const char * name, int node_id, int action, int target_id)
{
    if (!check_node_id(node_id))
        return nullptr;

    server_node * target = find_node(target_id);
    if (target == nullptr)
        return nullptr;

    node_position_constraint pos = make_pair(target, node_position(action));
    if (!node_position_sanity_check(pos))
        return nullptr;

    abstract_synth * synth = instance->add_synth(name, node_id, pos);
    if (!synth)
        log_printf("Cannot create synth (synthdef: %s, node id: %d)\n", name, node_id);

    last_generated = node_id;
    return static_cast<sc_synth*>(synth);
}

/* extract float or int32 as float from argument iterator */
inline float extract_float_argument(osc::ReceivedMessageArgumentIterator const & it)
{
    if (it->IsFloat())
        return it->AsFloatUnchecked();
    if (it->IsInt32())
        return float(it->AsInt32Unchecked());
    if (it->IsInt64())
        return float(it->AsInt64Unchecked());

    throw std::runtime_error("type cannot be converted to float");
}

inline void verify_argument(osc::ReceivedMessageArgumentIterator const & it,
                            osc::ReceivedMessageArgumentIterator const & end)
{
    if (it == end)
        throw std::runtime_error("unexpected end of argument list");
}

template <bool IsAudio, typename slot_type>
static void apply_control_bus_mapping(server_node & node, slot_type slot, int bus_index);

template <typename control_id_type>
void set_control_array(server_node * node, control_id_type control, osc::ReceivedMessageArgumentIterator & it)
{
    size_t array_size = it->ComputeArrayItemCount(); ++it;

    if (it->IsArrayBegin()) {
        // nested arrays are basically user errors, but we handle them like normal arrays
        log("Warning in /s_new handler: nested array argument detected");
        set_control_array<control_id_type>(node, control, it);
        return;
    } else {
        for (size_t i = 0; i != array_size; ++i) {
            if (it->IsString() || it->IsSymbol()) {
                char const * name = it->AsStringUnchecked(); ++it;
                int bus_id;

                switch (name[0]) {
                case 'c':
                    bus_id = atoi(name+1);
                    static_cast<sc_synth*>(node)->map_control_bus<false>(control, i, bus_id);
                    break;

                case 'a':
                    bus_id = atoi(name+1);
                    static_cast<sc_synth*>(node)->map_control_bus<true>(control, i, bus_id);
                    break;

                default:
                    throw runtime_error("invalid name for control mapping");
                }
            } else {
                float value = extract_float_argument(it++);
                node->set_control_array_element(control, i, value);
            }
        }
    }

    if (!it->IsArrayEnd())
        throw runtime_error("missing array end tag");
    ++it; // skip array end
}

template <typename ControlSpecifier>
void set_control(server_node * node, ControlSpecifier const & control, osc::ReceivedMessageArgumentIterator & it)
{
    if (it->IsArrayBegin())
        set_control_array(node, control, it);
    else if (it->IsString() || it->IsSymbol()) {
        char const * name = it->AsStringUnchecked(); ++it;
        int bus_id;

        switch (name[0]) {
        case 'c':
            bus_id = atoi(name+1);
            apply_control_bus_mapping<false>(*node, control, bus_id);
            break;

        case 'a':
            bus_id = atoi(name+1);
            apply_control_bus_mapping<true>(*node, control, bus_id);
            break;

        default:
            throw runtime_error("invalid name for control mapping");
        }

    } else {
        float value = extract_float_argument(it++);
        node->set(control, value);
    }
}

/* set control values of node from string/float or int/float pair */
void set_control(server_node * node, osc::ReceivedMessageArgumentIterator & it, osc::ReceivedMessageArgumentIterator end)
{
    if (it->IsInt32()) {
        osc::int32 index = it->AsInt32Unchecked(); ++it;
        if (it == end) return; // sclang sometimes uses an integer instead of an empty argument list
        set_control(node, index, it);
    } else if (it->IsString()) {
        const char * str = it->AsStringUnchecked(); ++it;
        set_control(node, str, it);
    } else
        throw runtime_error("invalid argument");
}

void handle_s_new(received_message const & msg)
{
    osc::ReceivedMessageArgumentIterator args = msg.ArgumentsBegin(), end = msg.ArgumentsEnd();

    const char * def_name = args->AsString(); ++args;
    int32_t id = args->AsInt32(); ++args;

    if (id == -1)
        id = instance->generate_node_id();

    int32_t action, target;

    if (args != end) {
        action = args->AsInt32(); ++args;
    } else
        action = 0;

    if (args != end) {
        target = args->AsInt32(); ++args;
    } else
        target = 0;

    sc_synth * synth = add_synth(def_name, id, action, target);

    if (synth == nullptr)
        return;

    try {
        while (args != end)
            set_control(synth, args, end);
    } catch(std::exception & e) {
        log_printf("exception in /s_new: %s\n", e.what());
    }
}


void handle_g_new(received_message const & msg)
{
    osc::ReceivedMessageArgumentStream args = msg.ArgumentStream();

    while(!args.Eos()) {
        osc::int32 node_id, action, target_id;
        args >> node_id >> action >> target_id;

        if (node_id == -1)
            node_id = instance->generate_node_id();
        else if (!check_node_id(node_id))
            continue;

        server_node * target = find_node(target_id);

        if (!target)
            continue;

        node_position_constraint pos = make_pair(target, node_position(action));
        if (!node_position_sanity_check(pos))
            continue;

        instance->add_group(node_id, pos);
        last_generated = node_id;
    }
}

void handle_g_freeall(received_message const & msg)
{
    osc::ReceivedMessageArgumentStream args = msg.ArgumentStream();

    while(!args.Eos())
    {
        osc::int32 id;
        args >> id;

        abstract_group * group = find_group(id);
        if (!group)
            continue;

        instance->group_free_all(group);
    }
}

void handle_g_deepFree(received_message const & msg)
{
    osc::ReceivedMessageArgumentStream args = msg.ArgumentStream();

    while(!args.Eos())
    {
        osc::int32 id;
        args >> id;

        abstract_group * group = find_group(id);
        if (!group)
            continue;

        instance->group_free_deep(group);
    }
}

void g_query_tree_fill_node(osc::OutboundPacketStream & p, bool flag, server_node const & node)
{
    p << osc::int32(node.id());
    if (node.is_synth())
        p << -1;
    else
        p << osc::int32(static_cast<abstract_group const &>(node).child_count());

    if (node.is_synth()) {
        sc_synth const & scsynth = static_cast<sc_synth const&>(node);
        p << scsynth.definition_name();

        if (flag) {
            osc::int32 controls = scsynth.mNumControls;
            p << controls;

            for (int i = 0; i != controls; ++i) {
                p << osc::int32(i); /** \todo later we can return symbols */

                if (scsynth.mMapControls[i] != (scsynth.mControls+i)) {
                    /* we use a bus mapping */
                    int bus = (scsynth.mMapControls[i]) - (scsynth.mNode.mWorld->mControlBus);
                    char str[10];
                    sprintf(str, "s%d", bus);
                    p << str;
                }
                else
                    p << scsynth.mControls[i];
            }
        }
    } else {
        abstract_group const & group = static_cast<abstract_group const &>(node);

        group.apply_on_children( [&](server_node const & node) {
            g_query_tree_fill_node(p, flag, node);
        });
    }
}

template <bool realtime>
void g_query_tree(int node_id, bool flag, endpoint_ptr endpoint)
{
    server_node * node = find_node(node_id);
    if (!node || node->is_synth())
        return;

    abstract_group * group = static_cast<abstract_group*>(node);

    size_t max_msg_size = 1<<16;
    for(;;) {
        try {
            if (max_msg_size > 1<<22)
                return;

            sized_array<char, rt_pool_allocator<char> > data(max_msg_size);

            osc::OutboundPacketStream p(data.c_array(), max_msg_size);
            p << osc::BeginMessage("/g_queryTree.reply")
              << (flag ? 1 : 0)
              << node_id
              << osc::int32(group->child_count());

            group->apply_on_children([&](server_node const & node) {
                g_query_tree_fill_node(p, flag, node);
            });
            p << osc::EndMessage;

            movable_array<char> message(p.Size(), data.c_array());
            cmd_dispatcher<realtime>::fire_message(endpoint, message);
            return;
        }
        catch(...)
        {
            max_msg_size *= 2; /* if we run out of memory, retry with doubled memory resources */
        }
    }
}

template <bool realtime>
void handle_g_queryTree(received_message const & msg, endpoint_ptr endpoint)
{
    osc::ReceivedMessageArgumentStream args = msg.ArgumentStream();

    while(!args.Eos())
    {
        try {
            osc::int32 id, flag;
            args >> id >> flag;
            g_query_tree<realtime>(id, flag, endpoint);
        }
        catch (std::exception & e) {
            log_printf("exception in handle_g_queryTree: %s\n", e.what());
        }
    }
}

typedef std::basic_stringstream <char,
                                 std::char_traits <char>/*,
                                 rt_pool_allocator<char>*/ > rt_string_stream;

void fill_spaces(rt_string_stream & stream, int level)
{
    for (int i = 0; i != level*3; ++i)
        stream << ' ';
}

void dump_controls(rt_string_stream & stream, abstract_synth const & synth, int indentation_level)
{
    const size_t number_of_slots = synth.number_of_slots();

    bool eol_pending = false;

    for (size_t control_index = 0; control_index != number_of_slots; ++control_index) {
        const char * name_of_slot = synth.name_of_slot(control_index);

        if (name_of_slot) {
            if (eol_pending) {
                stream << endl;
                eol_pending = false;
            }

            fill_spaces(stream, indentation_level);
            stream << synth.name_of_slot(control_index) << ": ";
            eol_pending = true;
        } else
            stream << ", ";


        stream << synth.get(control_index);
    }
    if (eol_pending)
        stream << endl;
}

void g_dump_node(rt_string_stream & stream, server_node & node, bool flag, int level)
{
    using namespace std;
    fill_spaces(stream, level);

    if (node.is_synth()) {
        abstract_synth const & synth = static_cast<abstract_synth const &>(node);
        stream << synth.id() << " " << synth.definition_name() << endl;

        if (flag)
            dump_controls(stream, synth, level + 1);

    } else {
        abstract_group & group = static_cast<abstract_group &>(node);
        stream << group.id();

        if (group.is_parallel())
            stream << " parallel group";
        else
            stream << " group";
        stream << endl;

        group.apply_on_children( [&](server_node & node) {
            g_dump_node(stream, node, flag, level + 1);
        });
    }
}

void g_dump_tree(int id, bool flag)
{
    server_node * node = find_node(id);
    if (!node)
        return;

    // FIXME: can we completely avoid all internal allocations?
    rt_string_stream stream;
    stream << "NODE TREE Group " << id << std::endl;

    g_dump_node(stream, *node, flag, 1);
    log(stream.str().c_str(), stream.str().size());
}

void handle_g_dumpTree(received_message const & msg)
{
    osc::ReceivedMessageArgumentStream args = msg.ArgumentStream();

    while(!args.Eos())
    {
        try {
            osc::int32 id, flag;
            args >> id >> flag;
            g_dump_tree(id, flag);
        }
        catch (std::exception & e) {
            log_printf("exception in /g_dumpTree: %s\n", e.what());
        }
    }
}

void handle_n_free(received_message const & msg)
{
    osc::ReceivedMessageArgumentStream args = msg.ArgumentStream();

    while(!args.Eos())
    {
        try {
            osc::int32 id;
            args >> id;

            server_node * node = find_node(id);
            if (!node)
                continue;

            instance->free_node(node);
        }
        catch (std::exception & e) {
            log_printf("exception in /n_free: %s\n", e.what());
        }
    }
}

/** macro to define an os command handler with a starting node id
 *
 *  it is mainly intended as decorator to avoid duplicate error handling code
 */
#define HANDLE_N_DECORATOR(cmd, function)                               \
void handle_n_##cmd(received_message const & msg)                       \
{                                                                       \
    osc::ReceivedMessageArgumentIterator it = msg.ArgumentsBegin();     \
    osc::int32 id = it->AsInt32(); ++it;                                \
                                                                        \
    server_node * node = find_node(id);                                 \
    if (!node)                                                          \
        return;                                                         \
                                                                        \
    try {                                                               \
        while (it != msg.ArgumentsEnd())                                \
            function(node, it);                                         \
    } catch(std::exception & e) {                                       \
        log_printf("Exception during /n_" #cmd "handler: %s\n", e.what());\
    }                                                                   \
}

void set_control(server_node * node, osc::ReceivedMessageArgumentIterator & it)
{
    if (it->IsInt32()) {
        osc::int32 index = it->AsInt32Unchecked(); ++it;
        set_control(node, index, it);
    } else if (it->IsString()) {
        const char * str = it->AsStringUnchecked(); ++it;
        set_control(node, str, it);
    } else
        throw runtime_error("invalid argument");
}


HANDLE_N_DECORATOR(set, set_control)

void set_control_n(server_node * node, osc::ReceivedMessageArgumentIterator & it)
{
    if (it->IsInt32()) {
        osc::int32 index = it->AsInt32Unchecked(); ++it;
        osc::int32 count = it->AsInt32(); ++it;

        for (int i = 0; i != count; ++i)
            node->set(index + i, extract_float_argument(it++));
    }
    else if (it->IsString()) {
        const char * str = it->AsStringUnchecked(); ++it;
        osc::int32 count = it->AsInt32(); ++it;

        sized_array<float> values(count);
        for (int i = 0; i != count; ++i)
            values[i] = extract_float_argument(it++);

        node->set_control_array(str, count, values.c_array());
    } else
        throw runtime_error("invalid argument");
}

HANDLE_N_DECORATOR(setn, set_control_n)

void fill_control(server_node * node, osc::ReceivedMessageArgumentIterator & it)
{
    if (it->IsInt32()) {
        osc::int32 index = it->AsInt32Unchecked(); ++it;
        osc::int32 count = it->AsInt32(); ++it;
        float value = extract_float_argument(it++);

        for (int i = 0; i != count; ++i)
            node->set(index + i, value);
    }
    else if (it->IsString()) {
        const char * str = it->AsStringUnchecked(); ++it;
        osc::int32 count = it->AsInt32(); ++it;
        float value = extract_float_argument(it++);

        sized_array<float> values(count);
        for (int i = 0; i != count; ++i)
            values[i] = value;

        node->set_control_array(str, count, values.c_array());
    } else
        throw runtime_error("invalid argument");
}

HANDLE_N_DECORATOR(fill, fill_control)

template <bool IsAudio, typename slot_type>
void apply_control_bus_mapping(server_node & node, slot_type slot, int bus_index)
{
    if (node.is_synth())
        static_cast<sc_synth&>(node).map_control_bus<IsAudio>(slot, bus_index);
    else {
        static_cast<abstract_group&>(node).apply_on_children( [&] (server_node & node) {
            apply_control_bus_mapping<IsAudio, slot_type>(node, slot, bus_index);
        });
    }
}

template <bool IsAudio, typename slot_type>
void apply_control_busn_mapping(server_node & node, slot_type slot, int bus_index, int count)
{
    if (node.is_synth())
        static_cast<sc_synth&>(node).map_control_buses<IsAudio>(slot, bus_index, count);
    else {
        static_cast<abstract_group&>(node).apply_on_children( [&] (server_node & node) {
            apply_control_busn_mapping<IsAudio, slot_type>(node, slot, bus_index, count);
        });
    }
}

template <bool IsAudio>
void map_control(server_node * node, osc::ReceivedMessageArgumentIterator & it)
{
    if (it->IsInt32()) {
        osc::int32 control_index = it->AsInt32Unchecked(); ++it;
        osc::int32 control_bus_index = it->AsInt32(); ++it;

        apply_control_bus_mapping<IsAudio>(*node, control_index, control_bus_index);
    }
    else if (it->IsString()) {
        const char * control_name = it->AsStringUnchecked(); ++it;
        osc::int32 control_bus_index = it->AsInt32(); ++it;

        apply_control_bus_mapping<IsAudio>(*node, control_name, control_bus_index);
    } else
        throw runtime_error("invalid argument");
}

template <bool IsAudio>
void mapn_control(server_node * node, osc::ReceivedMessageArgumentIterator & it)
{
    if (it->IsInt32()) {
        osc::int32 control_index = it->AsInt32Unchecked(); ++it;
        osc::int32 bus_index = it->AsInt32(); ++it;
        osc::int32 count = it->AsInt32(); ++it;

        apply_control_busn_mapping<IsAudio>(*node, control_index, bus_index, count);
    }
    else if (it->IsString()) {
        const char * control_name = it->AsStringUnchecked(); ++it;
        osc::int32 bus_index = it->AsInt32(); ++it;
        osc::int32 count = it->AsInt32(); ++it;

        apply_control_busn_mapping<IsAudio>(*node, control_name, bus_index, count);
    } else
        throw runtime_error("invalid argument");
}


HANDLE_N_DECORATOR(map, map_control<false>)
HANDLE_N_DECORATOR(mapa, map_control<true>)
HANDLE_N_DECORATOR(mapn, mapn_control<false>)
HANDLE_N_DECORATOR(mapan, mapn_control<true>)

template <nova::node_position Relation>
void handle_n_before_or_after(received_message const & msg)
{
    osc::ReceivedMessageArgumentStream args = msg.ArgumentStream();

    while(!args.Eos()) {
        osc::int32 node_a, node_b;
        args >> node_a >> node_b;

        server_node * node = find_node(node_a);
        if (!node) continue;

        server_node * target_node = find_node(node_b);
        if (!target_node) continue;

        abstract_group::move_before_or_after<Relation>(node, target_node);
        instance->notification_node_moved(node);
    }

    instance->request_dsp_queue_update();
}

template <nova::node_position Position>
void handle_g_head_or_tail(received_message const & msg)
{
    osc::ReceivedMessageArgumentStream args = msg.ArgumentStream();

    while(!args.Eos()) {
        osc::int32 node_id, target_id;
        args >> target_id >> node_id;

        server_node * node = find_node(node_id);
        if (!node) continue;

        abstract_group * target_group = find_group(target_id);
        if (!target_group) continue;

        abstract_group::move_to_head_or_tail<Position>(node, target_group);
        instance->notification_node_moved(node);
    }
    instance->request_dsp_queue_update();
}

template <bool realtime>
void handle_n_query(received_message const & msg, endpoint_ptr endpoint)
{
    osc::ReceivedMessageArgumentStream args = msg.ArgumentStream();

    while(!args.Eos()) {
        osc::int32 node_id;
        args >> node_id;

        server_node * node = find_node(node_id);
        if (!node)
            continue;

        char buffer[128]; // 128 byte should be enough
        osc::OutboundPacketStream p(buffer, 128);
        p << osc::BeginMessage("/n_info");
        fill_notification(node, p);

        movable_array<char> message(p.Size(), p.Data());
        cmd_dispatcher<realtime>::fire_message(endpoint, message);
    }
}

void handle_n_order(received_message const & msg)
{
    osc::ReceivedMessageArgumentStream args = msg.ArgumentStream();

    osc::int32 action, target_id;
    args >> action >> target_id;

    server_node * target = find_node(target_id);

    if (target == nullptr)
        return;

    abstract_group * target_parent;
    if (action == before || action == after)
        target_parent = target->get_parent();
    else {
        if (target->is_synth())
            throw std::runtime_error("invalid argument for n_order: argument is no synth");
        target_parent = static_cast<abstract_group*>(target);
    }

    while (!args.Eos()) {
        osc::int32 node_id;
        args >> node_id;

        server_node * node = find_node(node_id);
        if (node == nullptr)
            continue;

        abstract_group * node_parent = node->get_parent();

        /** TODO: this can be optimized if node_parent == target_parent */
        node_parent->remove_child(node);
        if (action == before ||
            action == after)
            target_parent->add_child(node, make_pair(target, node_position(action)));
        else
            target_parent->add_child(node, node_position(action));

        instance->notification_node_moved(node);
    }
    instance->request_dsp_queue_update();
}


void handle_n_run(received_message const & msg)
{
    osc::ReceivedMessageArgumentStream args = msg.ArgumentStream();

    while(!args.Eos()) {
        osc::int32 node_id, run_flag;
        args >> node_id >> run_flag;

        server_node * node = find_node(node_id);
        if(!node)
            continue;

        if (run_flag)
            instance->node_resume(node);
        else
            instance->node_pause(node);
    }
}

void enable_tracing(server_node & node)
{
    if (node.is_synth()) {
        sc_synth & synth = static_cast<sc_synth&>(node);
        synth.enable_tracing();
    } else {
        abstract_group & group = static_cast<abstract_group&>(node);
        group.apply_on_children(enable_tracing);
    }
}

void handle_n_trace(received_message const & msg)
{
    osc::ReceivedMessageArgumentStream args = msg.ArgumentStream();

    while(!args.Eos()) {
        osc::int32 node_id;
        args >> node_id;

        server_node * node = find_node(node_id);
        if (!node)
            continue;

        enable_tracing(*node);
    }
}


void handle_s_noid(received_message const & msg)
{
    osc::ReceivedMessageArgumentStream args = msg.ArgumentStream();

    while(!args.Eos()) {
        osc::int32 node_id;
        args >> node_id;
        instance->synth_reassign_id(node_id);
    }
}

int32_t get_control_index(sc_synth * s, osc::ReceivedMessageArgumentIterator & it, osc::OutboundPacketStream & p)
{
    int32_t control;
    if (it->IsInt32())
    {
        control = it->AsInt32Unchecked(); ++it;
        p << control;
    }
    else if (it->IsString())
    {
        const char * control_str = it->AsStringUnchecked(); ++it;
        control = s->resolve_slot(control_str);
        p << control_str;
    }
    else if (it->IsSymbol())
    {
        const char * control_str = it->AsSymbolUnchecked(); ++it;
        control = s->resolve_slot(control_str);
        p << osc::Symbol(control_str);
    }
    else
        throw std::runtime_error("wrong argument type");
    return control;
}

template <bool realtime>
void handle_s_get(received_message const & msg, size_t msg_size, endpoint_ptr endpoint)
{
    osc::ReceivedMessageArgumentIterator it = msg.ArgumentsBegin();

    if (!it->IsInt32())
        throw std::runtime_error("wrong argument type");

    int32_t node_id = it->AsInt32Unchecked(); ++it;

    server_node * node = find_node(node_id);
    if (!node || !node->is_synth())
        throw std::runtime_error("node is not a synth");

    sc_synth * s = static_cast<sc_synth*>(node);

    size_t alloc_size = msg_size + sizeof(float) * (msg.ArgumentCount()-1) + 128;

    sized_array<char, rt_pool_allocator<char> > return_message(alloc_size);

    osc::OutboundPacketStream p(return_message.c_array(), alloc_size);
    p << osc::BeginMessage("/n_set")
      << node_id;

    while (it != msg.ArgumentsEnd())
    {
        int32_t control = get_control_index(s, it, p);
        p << s->get(control);
    }
    p << osc::EndMessage;

    movable_array<char> message(p.Size(), return_message.c_array());
    cmd_dispatcher<realtime>::fire_message(endpoint, message);
}

template <bool realtime>
void handle_s_getn(received_message const & msg, size_t msg_size, endpoint_ptr endpoint)
{
    osc::ReceivedMessageArgumentIterator it = msg.ArgumentsBegin();

    if (!it->IsInt32())
        throw std::runtime_error("wrong argument type");

    int32_t node_id = it->AsInt32Unchecked(); ++it;

    server_node * node = find_node(node_id);
    if (!node || !node->is_synth())
        throw std::runtime_error("node is not a synth");

    sc_synth * s = static_cast<sc_synth*>(node);

    /* count argument values */
    size_t argument_count = 0;
    for (osc::ReceivedMessageArgumentIterator local = it; local != msg.ArgumentsEnd(); ++local)
    {
        ++local; /* skip control */
        if (local == msg.ArgumentsEnd())
            break;
        if (!it->IsInt32())
            throw std::runtime_error("invalid count");
        argument_count += it->AsInt32Unchecked(); ++it;
    }

    size_t alloc_size = msg_size + sizeof(float) * (argument_count) + 128;

    sized_array<char, rt_pool_allocator<char> > return_message(alloc_size);

    osc::OutboundPacketStream p(return_message.c_array(), alloc_size);
    p << osc::BeginMessage("/n_setn")
      << node_id;

    while (it != msg.ArgumentsEnd())
    {
        int32_t control = get_control_index(s, it, p);

        if (!it->IsInt32())
            throw std::runtime_error("integer argument expected");

        int32_t control_count = it->AsInt32Unchecked(); ++it;
        if (control_count < 0)
            break;

        for (int i = 0; i != control_count; ++i)
            p << s->get(control + i);
    }
    p << osc::EndMessage;

    movable_array<char> message(p.Size(), return_message.c_array());
    cmd_dispatcher<realtime>::fire_message(endpoint, message);
}


/** wrapper class for osc completion message
 */
struct completion_message
{
    /** constructor should only be used from the real-time thread */
    completion_message(size_t size, const void * data):
        size_(size)
    {
        if (size) {
            data_ = system_callback::allocate(size);
            memcpy(data_, data, size);
        }
    }

    /** default constructor creates uninitialized object */
    completion_message(void):
        size_(0)
    {}

    /** copy constructor has move semantics!!! */
    completion_message(completion_message const & rhs)
    {
        operator=(rhs);
    }

    completion_message& operator=(completion_message const & rhs)
    {
        size_ = rhs.size_;
        data_ = rhs.data_;
        const_cast<completion_message&>(rhs).size_ = 0;
        return *this;
    }

    ~completion_message(void)
    {
        if (size_)
            system_callback::deallocate(data_);
    }

    /** handle package in the rt thread
     *  not to be called from the rt thread
     */
    void trigger_async(endpoint_ptr endpoint)
    {
        if (size_) {
            sc_osc_handler::received_packet * p = sc_osc_handler::received_packet::alloc_packet((char*)data_, size_, endpoint);
            if( p )
                instance->add_sync_callback(p);
        }
    }

    /** handle package directly
     *  only to be called from the rt thread
     */
    void handle(endpoint_ptr endpoint)
    {
        if (size_)
            instance->handle_packet((char*)data_, size_, endpoint);
    }

    size_t size_;
    void * data_;
};

completion_message extract_completion_message(osc::ReceivedMessageArgumentStream & args)
{
    osc::Blob blob(nullptr, 0);

    if (!args.Eos()) {
        try {
            args >> blob;
        }
        catch (osc::WrongArgumentTypeException & e)
        {}
    }

    return completion_message (blob.size, blob.data);
}

completion_message extract_completion_message(osc::ReceivedMessageArgumentIterator & it)
{
    const void * data = nullptr;
    osc::osc_bundle_element_size_t length = 0;

    if (it->IsBlob())
        it->AsBlobUnchecked(data, length);
    ++it;
    return completion_message(length, data);
}


template <bool realtime>
void b_alloc_2_rt(uint32_t index, completion_message & msg, sample * free_buf, endpoint_ptr endpoint);
void b_alloc_3_nrt(uint32_t index, sample * free_buf, endpoint_ptr endpoint);

template <bool realtime>
void b_alloc_1_nrt(uint32_t bufnum, uint32_t frames, uint32_t channels, completion_message & msg, endpoint_ptr endpoint)
{
    sc_ugen_factory::buffer_lock_t buffer_lock(sc_factory->buffer_guard(bufnum));
    try {
        sample * free_buf = sc_factory->get_nrt_mirror_buffer(bufnum);
        sc_factory->allocate_buffer(bufnum, frames, channels);
        cmd_dispatcher<realtime>::fire_rt_callback(std::bind(b_alloc_2_rt<realtime>, bufnum, msg, free_buf, endpoint));
    } catch (std::exception const & error) {
        report_failure(endpoint, error, "/b_alloc", bufnum);
    }
}

template <bool realtime>
void b_alloc_2_rt(uint32_t index, completion_message & msg, sample * free_buf, endpoint_ptr endpoint)
{
    sc_factory->buffer_sync(index);
    msg.handle(endpoint);
    cmd_dispatcher<realtime>::fire_system_callback(std::bind(b_alloc_3_nrt, index, free_buf, endpoint));
}

void b_alloc_3_nrt(uint32_t index, sample * free_buf, endpoint_ptr endpoint)
{
    free_aligned(free_buf);
    send_done_message(endpoint, "/b_alloc", index);
}

template <bool realtime>
void handle_b_alloc(received_message const & msg, endpoint_ptr endpoint)
{
    osc::ReceivedMessageArgumentStream args = msg.ArgumentStream();

    osc::int32 index, frames, channels;

    args >> index >> frames;

    if (!args.Eos())
        args >> channels;
    else
        channels = 1;

    completion_message message = extract_completion_message(args);

    cmd_dispatcher<realtime>::fire_system_callback(std::bind(b_alloc_1_nrt<realtime>, index, frames,
                                                           channels, message, endpoint));
}

template <bool realtime>
void b_free_1_nrt(uint32_t index, completion_message & msg, endpoint_ptr endpoint);
template <bool realtime>
void b_free_2_rt(uint32_t index, sample * free_buf, completion_message & msg, endpoint_ptr endpoint);
void b_free_3_nrt(uint32_t index, sample * free_buf, endpoint_ptr endpoint);

template <bool realtime>
void b_free_1_nrt(uint32_t index, completion_message & msg, endpoint_ptr endpoint)
{
    sc_ugen_factory::buffer_lock_t buffer_lock(sc_factory->buffer_guard(index));
    sample * free_buf = sc_factory->get_nrt_mirror_buffer(index);
    sc_factory->free_buffer(index);
    cmd_dispatcher<realtime>::fire_rt_callback(std::bind(b_free_2_rt<realtime>,
                                                           index, free_buf, msg, endpoint));
}

template <bool realtime>
void b_free_2_rt(uint32_t index, sample * free_buf, completion_message & msg, endpoint_ptr endpoint)
{
    sc_factory->buffer_sync(index);
    cmd_dispatcher<realtime>::fire_system_callback(std::bind(b_free_3_nrt, index, free_buf, endpoint));
    msg.handle(endpoint);
}

void b_free_3_nrt(uint32_t index, sample * free_buf, endpoint_ptr endpoint)
{
    free_aligned(free_buf);
    send_done_message(endpoint, "/b_free", index);
}


template <bool realtime>
void handle_b_free(received_message const & msg, endpoint_ptr endpoint)
{
    osc::ReceivedMessageArgumentStream args = msg.ArgumentStream();

    osc::int32 index;
    args >> index;

    completion_message message = extract_completion_message(args);

    cmd_dispatcher<realtime>::fire_system_callback(std::bind(b_free_1_nrt<realtime>, index, message, endpoint));
}

template <bool realtime>
void b_allocRead_2_rt(uint32_t index, completion_message & msg, sample * free_buf, endpoint_ptr endpoint);
void b_allocRead_3_nrt(uint32_t index, sample * free_buf, endpoint_ptr endpoint);

template <bool realtime>
void b_allocRead_1_nrt(uint32_t bufnum, movable_string & filename, uint32_t start, uint32_t frames, completion_message & msg,
                       endpoint_ptr endpoint)
{
    sc_ugen_factory::buffer_lock_t buffer_lock(sc_factory->buffer_guard(bufnum));
    sample * free_buf = sc_factory->get_nrt_mirror_buffer(bufnum);
    try {
        sc_factory->buffer_read_alloc(bufnum, filename.c_str(), start, frames);
        cmd_dispatcher<realtime>::fire_rt_callback(std::bind(b_allocRead_2_rt<realtime>, bufnum, msg, free_buf, endpoint));
    } catch (std::exception const & error) {
        report_failure(endpoint, error, "/b_allocRead", bufnum);
    }
}

template <bool realtime>
void b_allocRead_2_rt(uint32_t index, completion_message & msg, sample * free_buf,
                      endpoint_ptr endpoint)
{
    sc_factory->buffer_sync(index);
    msg.handle(endpoint);
    cmd_dispatcher<realtime>::fire_system_callback(std::bind(b_allocRead_3_nrt, index, free_buf, endpoint));
}

void b_allocRead_3_nrt(uint32_t index, sample * free_buf, endpoint_ptr endpoint)
{
    free_aligned(free_buf);
    send_done_message(endpoint, "/b_allocRead", index);
}

template <bool realtime>
void handle_b_allocRead(received_message const & msg, endpoint_ptr endpoint)
{
    osc::ReceivedMessageArgumentStream args = msg.ArgumentStream();

    osc::int32 index;
    const char * filename;

    osc::int32 start = 0;
    osc::int32 frames = 0;

    args >> index >> filename;

    if (!args.Eos())
        args >> start;

    if (!args.Eos())
        args >> frames;

    completion_message message = extract_completion_message(args);

    movable_string fname(filename);
    cmd_dispatcher<realtime>::fire_system_callback(std::bind(b_allocRead_1_nrt<realtime>, index,
                                                               fname, start, frames, message, endpoint));
}

template <bool realtime>
void b_allocReadChannel_2_rt(uint32_t bufnum, completion_message & msg, sample * free_buf,
                             endpoint_ptr endpoint);
void b_allocReadChannel_3_nrt(uint32_t bufnum, sample * free_buf, endpoint_ptr endpoint);

template <bool realtime>
void b_allocReadChannel_1_nrt(uint32_t bufnum, movable_string const & filename, uint32_t start, uint32_t frames,
                              movable_array<uint32_t> const & channels, completion_message & msg,
                              endpoint_ptr endpoint)
{
    sc_ugen_factory::buffer_lock_t buffer_lock(sc_factory->buffer_guard(bufnum));
    sample * free_buf = sc_factory->get_nrt_mirror_buffer(bufnum);

    try {
        sc_factory->buffer_alloc_read_channels(bufnum, filename.c_str(), start, frames, channels.size(), channels.data());

        cmd_dispatcher<realtime>::fire_rt_callback(std::bind(b_allocReadChannel_2_rt<realtime>,
                                                             bufnum, msg, free_buf, endpoint));
    } catch (std::exception const & error) {
        report_failure(endpoint, error, "/b_allocReadChannel", bufnum);
    }
}

template <bool realtime>
void b_allocReadChannel_2_rt(uint32_t bufnum, completion_message & msg, sample * free_buf,
                             endpoint_ptr endpoint)
{
    sc_factory->buffer_sync(bufnum);
    msg.handle(endpoint);
    cmd_dispatcher<realtime>::fire_system_callback(std::bind(b_allocReadChannel_3_nrt,
                                                             bufnum, free_buf, endpoint));
}

void b_allocReadChannel_3_nrt(uint32_t bufnum, sample * free_buf, endpoint_ptr endpoint)
{
    free_aligned(free_buf);
    send_done_message(endpoint, "/b_allocReadChannel", bufnum);
}


template <bool realtime>
void handle_b_allocReadChannel(received_message const & msg, endpoint_ptr endpoint)
{
    osc::ReceivedMessageArgumentIterator arg = msg.ArgumentsBegin();

    osc::int32 bufnum = arg->AsInt32(); arg++;
    const char * filename = arg->AsString(); arg++;

    osc::int32 start = arg->AsInt32(); arg++;
    size_t frames = arg->AsInt32(); arg++;

    size_t channel_args = msg.ArgumentCount() - 4; /* we already consumed 4 elements */

    size_t channel_count = 0;
    sized_array<uint, rt_pool_allocator<uint> > channels(channel_args);

    // sclang formats the last completion message as int, so we skip the last element
    for (uint i = 0; i != channel_args - 1; ++i)  {
        if (arg->IsInt32()) {
            channels[i] = arg->AsInt32Unchecked(); arg++;
            ++channel_count;
        }
    }

    /* we reached the message blob */
    completion_message message = extract_completion_message(arg);

    movable_array<uint32_t> channel_mapping(channel_count, channels.c_array());
    movable_string fname(filename);

    cmd_dispatcher<realtime>::fire_system_callback(std::bind(b_allocReadChannel_1_nrt<realtime>,
                                                           bufnum, fname, start, frames, channel_mapping,
                                                           message, endpoint));
}

const char * b_write = "/b_write";

template <bool realtime>
void b_write_nrt_1(uint32_t bufnum, movable_string const & filename, movable_string const & header_format,
                   movable_string const & sample_format, uint32_t start, uint32_t frames, bool leave_open,
                   completion_message & msg, endpoint_ptr endpoint)
{
    sc_ugen_factory::buffer_lock_t buffer_lock(sc_factory->buffer_guard(bufnum));
    try {
        sc_factory->buffer_write(bufnum, filename.c_str(), header_format.c_str(), sample_format.c_str(), start, frames, leave_open);
        msg.trigger_async(endpoint);
        cmd_dispatcher<realtime>::fire_done_message(endpoint, b_write, bufnum);
    } catch (std::exception const & error) {
        report_failure(endpoint, error, b_write, bufnum);
    }
}

void fire_b_write_exception(void)
{
    throw std::runtime_error("wrong arguments for /b_write");
}

template <bool realtime>
void handle_b_write(received_message const & msg, endpoint_ptr endpoint)
{
    osc::ReceivedMessageArgumentIterator arg = msg.ArgumentsBegin();
    osc::ReceivedMessageArgumentIterator end = msg.ArgumentsEnd();

    /* required args */
    osc::int32 bufnum = arg->AsInt32(); arg++;
    const char * filename = arg->AsString(); arg++;
    const char * header_format = arg->AsString(); arg++;
    const char * sample_format = arg->AsString(); arg++;

    /* optional args */
    osc::int32 frames = -1;
    osc::int32 start = 0;
    osc::int32 leave_open = 0;

    completion_message message;

    if (arg != end) {
        if (!arg->IsInt32())
            fire_b_write_exception();
        frames = arg->AsInt32Unchecked(); arg++;
    } else
        goto fire_callback;

    if (arg != end) {
        if (!arg->IsInt32())
            fire_b_write_exception();
        start = arg->AsInt32Unchecked(); arg++;
    } else
        goto fire_callback;

    if (arg != end) {
        if (!arg->IsInt32())
            fire_b_write_exception();
        leave_open = arg->AsInt32Unchecked(); arg++;
    } else
        goto fire_callback;

    if (arg != end)
        message = extract_completion_message(arg);

fire_callback:
    movable_string fname(filename);
    movable_string header_f(header_format);
    movable_string sample_f(sample_format);

    cmd_dispatcher<realtime>::fire_system_callback(std::bind(b_write_nrt_1<realtime>, bufnum, fname, header_f, sample_f,
                                                             start, frames, bool(leave_open), message, endpoint));
}


const char * b_read = "/b_read";

template <bool realtime>
void b_read_rt_2(uint32_t bufnum, completion_message & msg, endpoint_ptr endpoint);

template <bool realtime>
void b_read_nrt_1(uint32_t bufnum, movable_string & filename, uint32_t start_file, uint32_t frames,
                  uint32_t start_buffer, bool leave_open, completion_message & msg, endpoint_ptr endpoint)
{
    sc_ugen_factory::buffer_lock_t buffer_lock(sc_factory->buffer_guard(bufnum));

    try {
        sc_factory->buffer_read(bufnum, filename.c_str(), start_file, frames, start_buffer, leave_open);
        cmd_dispatcher<realtime>::fire_rt_callback(std::bind(b_read_rt_2<realtime>, bufnum, msg, endpoint));
    } catch (std::exception const & error) {
        report_failure(endpoint, error, b_read, bufnum);
    }
}

template <bool realtime>
void b_read_rt_2(uint32_t index, completion_message & msg, endpoint_ptr endpoint)
{
    sc_factory->buffer_sync(index);
    msg.handle(endpoint);
    cmd_dispatcher<realtime>::fire_done_message(endpoint, b_read, index);
}

void fire_b_read_exception(void)
{
    throw std::runtime_error("wrong arguments for /b_read");
}

template <bool realtime>
void handle_b_read(received_message const & msg, endpoint_ptr endpoint)
{
    osc::ReceivedMessageArgumentIterator arg = msg.ArgumentsBegin();
    osc::ReceivedMessageArgumentIterator end = msg.ArgumentsEnd();

    /* required args */
    osc::int32 bufnum = arg->AsInt32(); arg++;
    const char * filename = arg->AsString(); arg++;

    /* optional args */
    osc::int32 start_file = 0;
    osc::int32 frames = -1;
    osc::int32 start_buffer = 0;
    osc::int32 leave_open = 0;

    completion_message message;

    if (arg != end) {
        if (!arg->IsInt32())
            fire_b_read_exception();
        start_file = arg->AsInt32Unchecked(); arg++;
    } else
        goto fire_callback;

    if (arg != end) {
        if (!arg->IsInt32())
            fire_b_read_exception();
        frames = arg->AsInt32Unchecked(); arg++;
    } else
        goto fire_callback;

    if (arg != end) {
        if (!arg->IsInt32())
            fire_b_read_exception();
        start_buffer = arg->AsInt32Unchecked(); arg++;
    } else
        goto fire_callback;

    if (arg != end) {
        if (!arg->IsInt32())
            fire_b_read_exception();
        leave_open = arg->AsInt32Unchecked(); arg++;
    } else
        goto fire_callback;

    if (arg != end)
        message = extract_completion_message(arg);

fire_callback:
    movable_string fname(filename);

    cmd_dispatcher<realtime>::fire_system_callback(std::bind(b_read_nrt_1<realtime>, bufnum, fname,
                                                             start_file, frames, start_buffer,
                                                             bool(leave_open), message, endpoint));
}

const char * b_readChannel = "/b_readChannel";

template <bool realtime>
void b_readChannel_rt_2(uint32_t index, completion_message & msg, endpoint_ptr endpoint);

template <bool realtime>
void b_readChannel_nrt_1(uint32_t bufnum, movable_string & filename, uint32_t start_file, uint32_t frames,
                         uint32_t start_buffer, bool leave_open, movable_array<uint32_t> & channel_map,
                         completion_message & msg, endpoint_ptr endpoint)
{
    try {
        sc_factory->buffer_read_channel(bufnum, filename.c_str(), start_file, frames, start_buffer, leave_open,
                                        channel_map.size(), channel_map.data());
        cmd_dispatcher<realtime>::fire_system_callback(std::bind(b_readChannel_rt_2<realtime>, bufnum, msg, endpoint));
    } catch (std::exception const & error) {
        report_failure(endpoint, error, b_readChannel, bufnum);
    }
}

template <bool realtime>
void b_readChannel_rt_2(uint32_t index, completion_message & msg, endpoint_ptr endpoint)
{
    sc_factory->buffer_sync(index);
    msg.handle(endpoint);
    cmd_dispatcher<realtime>::fire_done_message(endpoint, b_readChannel, index);
}

void fire_b_readChannel_exception(void)
{
    throw std::runtime_error("wrong arguments for /b_readChannel");
}

template <bool realtime>
void handle_b_readChannel(received_message const & msg, endpoint_ptr endpoint)
{
    osc::ReceivedMessageArgumentIterator arg = msg.ArgumentsBegin();
    osc::ReceivedMessageArgumentIterator end = msg.ArgumentsEnd();

    /* required args */
    osc::int32 index = arg->AsInt32(); arg++;
    const char * filename = arg->AsString(); arg++;

    /* optional args */
    osc::int32 start_file = 0;
    osc::int32 frames = -1;
    osc::int32 start_buffer = 0;
    osc::int32 leave_open = 0;

    sized_array<uint32_t, rt_pool_allocator<uint32_t> > channel_mapping(int32_t(msg.ArgumentCount())); /* larger than required */
    uint32_t channel_count = 0;

    completion_message message;

    if (arg != end) {
        if (!arg->IsInt32())
            fire_b_read_exception();
        start_file = arg->AsInt32Unchecked(); arg++;
    }
    else
        goto fire_callback;

    if (arg != end) {
        if (!arg->IsInt32())
            fire_b_read_exception();
        frames = arg->AsInt32Unchecked(); arg++;
    }
    else
        goto fire_callback;

    if (arg != end) {
        if (!arg->IsInt32())
            fire_b_write_exception();
        start_buffer = arg->AsInt32Unchecked(); arg++;
    }
    else
        goto fire_callback;

    if (arg != end) {
        if (!arg->IsInt32())
            fire_b_write_exception();
        leave_open = arg->AsInt32Unchecked(); arg++;
    }
    else
        goto fire_callback;

    while (arg != end)
    {
        if (arg->IsBlob()) {
            message = extract_completion_message(arg);
            goto fire_callback;
        }
        else if (arg->IsInt32()) {
            channel_mapping[channel_count] = arg->AsInt32Unchecked();
            ++arg;
        }
        else
            fire_b_readChannel_exception();
    }

fire_callback:
    movable_string fname(filename);
    movable_array<uint32_t> channel_map(channel_count, channel_mapping.c_array());

    cmd_dispatcher<realtime>::fire_system_callback(std::bind(b_readChannel_nrt_1<realtime>, index, fname,
                                                           start_file, frames, start_buffer,
                                                           bool(leave_open), channel_map, message, endpoint));
}


template <bool realtime>
void b_zero_rt_2(uint32_t index, completion_message & msg, endpoint_ptr endpoint);

template <bool realtime>
void b_zero_nrt_1(uint32_t index, completion_message & msg, endpoint_ptr endpoint)
{
    sc_factory->buffer_zero(index);
    cmd_dispatcher<realtime>::fire_rt_callback(std::bind(b_zero_rt_2<realtime>, index, msg, endpoint));
}

const char * b_zero = "/b_zero";
template <bool realtime>
void b_zero_rt_2(uint32_t index, completion_message & msg, endpoint_ptr endpoint)
{
    sc_factory->increment_write_updates(index);
    msg.handle(endpoint);
    cmd_dispatcher<realtime>::fire_done_message(endpoint, b_zero, index);
}

template <bool realtime>
void handle_b_zero(received_message const & msg, endpoint_ptr endpoint)
{
    osc::ReceivedMessageArgumentStream args = msg.ArgumentStream();

    osc::int32 index;
    args >> index;
    completion_message message = extract_completion_message(args);

    cmd_dispatcher<realtime>::fire_system_callback(std::bind(b_zero_nrt_1<realtime>, index, message, endpoint));
}

void handle_b_set(received_message const & msg)
{
    osc::ReceivedMessageArgumentIterator it = msg.ArgumentsBegin();
    osc::ReceivedMessageArgumentIterator end = msg.ArgumentsEnd();
    verify_argument(it, end);
    osc::int32 buffer_index = it->AsInt32(); ++it;

    buffer_wrapper::sample_t * data = sc_factory->get_buffer(buffer_index);
    if( !data ) {
        log_printf("/b_set called on unallocated buffer");
        return;
    }

    while (it != end) {
        osc::int32 index = it->AsInt32(); ++it;
        float value = extract_float_argument(it++);

        data[index] = value;
    }
}

void handle_b_setn(received_message const & msg)
{
    osc::ReceivedMessageArgumentIterator it = msg.ArgumentsBegin();
    osc::ReceivedMessageArgumentIterator end = msg.ArgumentsEnd();
    verify_argument(it, end);
    osc::int32 buffer_index = it->AsInt32(); ++it;

    buffer_wrapper::sample_t * data = sc_factory->get_buffer(buffer_index);
    if( !data ) {
        log_printf("/b_setn called on unallocated buffer");
        return;
    }

    while (it != end) {
        osc::int32 index = it->AsInt32(); ++it;
        verify_argument(it, end);
        osc::int32 samples = it->AsInt32(); ++it;

        for (int i = 0; i != samples; ++i) {
            verify_argument(it, end);
            float value = extract_float_argument(it++);
            data[index+i] = value;
        }
    }
}

void handle_b_fill(received_message const & msg)
{
    osc::ReceivedMessageArgumentIterator it = msg.ArgumentsBegin();
    osc::ReceivedMessageArgumentIterator end = msg.ArgumentsEnd();
    verify_argument(it, end);
    osc::int32 buffer_index = it->AsInt32(); ++it;

    buffer_wrapper::sample_t * data = sc_factory->get_buffer(buffer_index);
    if( !data ) {
        log_printf("/b_fill called on unallocated buffer");
        return;
    }


    while (it != end) {
        osc::int32 index = it->AsInt32(); ++it;
        verify_argument(it, end);
        osc::int32 samples = it->AsInt32(); ++it;
        verify_argument(it, end);
        float value = extract_float_argument(it++);

        for (int i = 0; i != samples; ++i)
            data[index] = value;
    }
}

template <bool realtime>
void handle_b_query(received_message const & msg, endpoint_ptr endpoint)
{
    const size_t elem_size = 3*sizeof(int) * sizeof(float);

    osc::ReceivedMessageArgumentStream args = msg.ArgumentStream();
    size_t arg_count = msg.ArgumentCount();

    size_t size = elem_size * arg_count + 128; /* should be more than required */
    sized_array<char, rt_pool_allocator<char> > data(size);

    osc::OutboundPacketStream p(data.c_array(), size);
    p << osc::BeginMessage("/b_info");

    while (!args.Eos()) {
        osc::int32 buffer_index;
        args >> buffer_index;

        SndBuf * buf = sc_factory->get_buffer_struct(buffer_index);

        p << buffer_index
          << osc::int32(buf->frames)
          << osc::int32(buf->channels)
          << float (buf->samplerate);
    }

    p << osc::EndMessage;

    movable_array<char> message(p.Size(), data.c_array());
    cmd_dispatcher<realtime>::fire_message(endpoint, message);
}

template <bool realtime>
void b_close_rt_2(uint32_t index, completion_message & msg, endpoint_ptr endpoint);

template <bool realtime>
void b_close_nrt_1(uint32_t index, completion_message & msg, endpoint_ptr endpoint)
{
    sc_factory->buffer_close(index);
    cmd_dispatcher<realtime>::fire_rt_callback(std::bind(b_close_rt_2<realtime>, index, msg, endpoint));
}

template <bool realtime>
void b_close_rt_2(uint32_t index, completion_message & msg, endpoint_ptr endpoint)
{
    msg.handle(endpoint);
    cmd_dispatcher<realtime>::fire_done_message(endpoint, "/b_close", index);
}

template <bool realtime>
void handle_b_close(received_message const & msg, endpoint_ptr endpoint)
{
    osc::ReceivedMessageArgumentStream args = msg.ArgumentStream();
    osc::int32 index;
    args >> index;

    completion_message message = extract_completion_message(args);
    cmd_dispatcher<realtime>::fire_system_callback(std::bind(b_close_nrt_1<realtime>, index, message, endpoint));
}

template <bool realtime>
void handle_b_get(received_message const & msg, endpoint_ptr endpoint)
{
    const size_t elem_size = sizeof(int) * sizeof(float);
    osc::ReceivedMessageArgumentStream args = msg.ArgumentStream();
    const size_t index_count = msg.ArgumentCount() - 1;
    const size_t alloc_size = index_count * elem_size + 128; /* hopefully enough */

    sized_array<char, rt_pool_allocator<char> > return_message(alloc_size);

    osc::int32 buffer_index;
    args >> buffer_index;

    const SndBuf * buf = sc_factory->get_buffer_struct(buffer_index);
    const sample * data = buf->data;
    if( !data ) {
        log_printf("/b_get called on unallocated buffer");
        return;
    }

    const int max_sample = buf->frames * buf->channels;

    osc::OutboundPacketStream p(return_message.c_array(), alloc_size);
    p << osc::BeginMessage("/b_set")
      << buffer_index;

    while (!args.Eos())
    {
        osc::int32 index;
        args >> index;
        p << index;

        if (index < max_sample)
            p << data[index];
        else
            p << 0.f;
    }

    p << osc::EndMessage;

    movable_array<char> message(p.Size(), return_message.c_array());
    cmd_dispatcher<realtime>::fire_message(endpoint, message);
}

template<typename Alloc>
struct getn_data
{
    getn_data(int start, int count, const float * data):
        start_index_(start), data_(count)
    {
        data_.reserve(count);
        for (int i = 0; i != count; ++i)
            data_[i] = data[i];
    }

    int start_index_;
    std::vector<float, Alloc> data_;
};

template <bool realtime>
void handle_b_getn(received_message const & msg, endpoint_ptr endpoint)
{
    osc::ReceivedMessageArgumentStream args = msg.ArgumentStream();

    typedef getn_data<rt_pool_allocator<float> > getn_data;
    std::vector<getn_data, rt_pool_allocator<getn_data> > return_data;

    osc::int32 buffer_index;
    args >> buffer_index;

    const SndBuf * buf = sc_factory->get_buffer_struct(buffer_index);
    const sample * data = buf->data;
    if( !data ) {
        log_printf("/b_getn called on unallocated buffer");
        return;
    }
    const int max_sample = buf->frames * buf->channels;

    while (!args.Eos())
    {
        osc::int32 index, sample_count;
        args >> index >> sample_count;

        if (index + sample_count <= max_sample)
            return_data.push_back(getn_data(index, sample_count, data + index));
    }

    size_t alloc_size = 128;
    for (size_t i = 0; i != return_data.size(); ++i)
        alloc_size += return_data[i].data_.size() * (sizeof(float) + sizeof(int)) + 2*sizeof(int);

    sized_array<char, rt_pool_allocator<char> > return_message(alloc_size);

    osc::OutboundPacketStream p(return_message.c_array(), alloc_size);
    p << osc::BeginMessage("/b_setn")
      << buffer_index;

    for (size_t i = 0; i != return_data.size(); ++i) {
        p << osc::int32(return_data[i].start_index_)
          << osc::int32(return_data[i].data_.size());

        for (size_t j = 0; j != return_data[i].data_.size(); ++j)
            p << return_data[i].data_[j];
    }

    p << osc::EndMessage;

    movable_array<char> message(p.Size(), return_message.c_array());
    cmd_dispatcher<realtime>::fire_message(endpoint, message);
}


template <bool realtime>
void b_gen_rt_2(uint32_t index, sample * free_buf, endpoint_ptr endpoint);
void b_gen_nrt_3(uint32_t index, sample * free_buf, endpoint_ptr endpoint);

template <bool realtime>
void b_gen_nrt_1(movable_array<char> & message, endpoint_ptr endpoint)
{
    const char * data = (char*)message.data();
    const char * msg_data = OSCstrskip(data); // skip address
    size_t diff = msg_data - data;

    sc_msg_iter msg(message.size() - diff, msg_data);

    char nextTag = msg.nextTag();
    if (nextTag != 'i') {
        printf("/b_gen handler: invalid buffer index type %c\n", nextTag);
        return;
    }
    int index = msg.geti();

    const char * generator = (const char*)msg.gets4();
    if (!generator) {
        if (nextTag += 'i') {
            printf("/b_gen handler: invalid bufgen name\n");
            return;
        }
    }

    sample * free_buf = sc_factory->buffer_generate(index, generator, msg);
    cmd_dispatcher<realtime>::fire_rt_callback(std::bind(b_gen_rt_2<realtime>, index, free_buf, endpoint));
}

template <bool realtime>
void b_gen_rt_2(uint32_t index, sample * free_buf, endpoint_ptr endpoint)
{
    sc_factory->buffer_sync(index);
    cmd_dispatcher<realtime>::fire_system_callback(std::bind(b_gen_nrt_3, index, free_buf, endpoint));
}

void b_gen_nrt_3(uint32_t index, sample * free_buf, endpoint_ptr endpoint)
{
    free_aligned(free_buf);
    send_done_message(endpoint, "/b_gen", index);
}

template <bool realtime>
void handle_b_gen(received_message const & msg, size_t msg_size, endpoint_ptr endpoint)
{
    movable_array<char> cmd (msg_size, msg.AddressPattern());
    cmd_dispatcher<realtime>::fire_system_callback(std::bind(b_gen_nrt_1<realtime>, cmd, endpoint));
}


void handle_c_set(received_message const & msg)
{
    osc::ReceivedMessageArgumentIterator it = msg.ArgumentsBegin();

    while (it != msg.ArgumentsEnd()) {
        osc::int32 bus_index = it->AsInt32(); ++it;
        float value = extract_float_argument(it++);

        sc_factory->controlbus_set(bus_index, value);
    }
}

void handle_c_setn(received_message const & msg)
{
    osc::ReceivedMessageArgumentIterator it = msg.ArgumentsBegin();

    while (it != msg.ArgumentsEnd()) {
        osc::int32 bus_index, bus_count;
        bus_index = it->AsInt32(); ++it;
        bus_count = it->AsInt32(); ++it;

        for (int i = 0; i != bus_count; ++i) {
            float value = extract_float_argument(it++);
            sc_factory->controlbus_set(bus_index + i, value);
        }
    }
}

void handle_c_fill(received_message const & msg)
{
    osc::ReceivedMessageArgumentIterator it = msg.ArgumentsBegin();

    while (it != msg.ArgumentsEnd()) {
        osc::int32 bus_index, bus_count;
        bus_index = it->AsInt32(); ++it;
        bus_count = it->AsInt32(); ++it;
        float value = extract_float_argument(it++);
        sc_factory->controlbus_fill(bus_index, bus_count, value);
    }
}

template <bool realtime>
void handle_c_get(received_message const & msg,
                  endpoint_ptr endpoint)
{
    const size_t elem_size = sizeof(int) + sizeof(float);
    osc::ReceivedMessageArgumentStream args = msg.ArgumentStream();
    const size_t index_count = msg.ArgumentCount();
    const size_t alloc_size = index_count * elem_size + 128; /* hopefully enough */

    sized_array<char, rt_pool_allocator<char> > return_message(alloc_size);

    osc::OutboundPacketStream p(return_message.c_array(), alloc_size);
    p << osc::BeginMessage("/c_set");

    while (!args.Eos())
    {
        osc::int32 index;
        args >> index;

        p << index << sc_factory->controlbus_get(index);
    }

    p << osc::EndMessage;

    movable_array<char> message(p.Size(), return_message.c_array());
    cmd_dispatcher<realtime>::fire_message(endpoint, message);
}

template <bool realtime>
void handle_c_getn(received_message const & msg, endpoint_ptr endpoint)
{
    osc::ReceivedMessageArgumentStream args = msg.ArgumentStream();

    /* we pessimize, but better to allocate too much than too little */
    const size_t alloc_size = 128 +
        (2 * sizeof(int) + 128*sizeof(float)) * msg.ArgumentCount();

    sized_array<char, rt_pool_allocator<char> > return_message(alloc_size);

    osc::OutboundPacketStream p(return_message.c_array(), alloc_size);
    p << osc::BeginMessage("/c_setn");

    while (!args.Eos())
    {
        osc::int32 bus_index, bus_count;
        args >> bus_index >> bus_count;
        p << bus_index << bus_count;

        for (int i = 0; i != bus_count; ++i) {
            float value = sc_factory->controlbus_get(bus_index + i);
            p << value;
        }
    }

    p << osc::EndMessage;

    movable_array<char> message(p.Size(), return_message.c_array());
    cmd_dispatcher<realtime>::fire_message(endpoint, message);
}

std::pair<sc_synth_definition_ptr *, size_t> wrap_synthdefs(std::vector<sc_synthdef> && defs)
{
    std::vector<sc_synthdef> synthdefs(std::move(defs));
    size_t count = synthdefs.size();
    sc_synth_definition_ptr * definitions = new sc_synth_definition_ptr [count];

    for (size_t i = 0; i != count; ++i)
        definitions[i].reset(new sc_synth_definition(std::move(synthdefs[i])));
    return std::make_pair(definitions, count);
}

std::pair<sc_synth_definition_ptr *, size_t> wrap_synthdefs(std::vector<sc_synthdef> const & defs)
{
    size_t count = defs.size();
    sc_synth_definition_ptr * definitions = new sc_synth_definition_ptr [count];

    for (size_t i = 0; i != count; ++i)
        definitions[i].reset(new sc_synth_definition(defs[i]));
    return std::make_pair(definitions, count);
}

template <bool realtime>
void d_recv_rt2(sc_synth_definition_ptr * definitions, size_t definition_count, completion_message & msg,
                endpoint_ptr endpoint);
void d_recv_nrt3(sc_synth_definition_ptr * definitions, endpoint_ptr endpoint);

template <bool realtime>
void d_recv_nrt(movable_array<char> & def, completion_message & msg, endpoint_ptr endpoint)
{
    size_t count;
    sc_synth_definition_ptr * definitions;
    std::vector<sc_synthdef> synthdefs (read_synthdefs(def.data(), def.data() + def.size()));

    boost::tie(definitions, count) = wrap_synthdefs(std::move(synthdefs));

    cmd_dispatcher<realtime>::fire_rt_callback(std::bind(d_recv_rt2<realtime>, definitions, count, msg, endpoint));
}

template <bool realtime>
void d_recv_rt2(sc_synth_definition_ptr * definitions, size_t definition_count, completion_message & msg,
                endpoint_ptr endpoint)
{
    std::for_each(definitions, definitions + definition_count, [](sc_synth_definition_ptr const & definition) {
        instance->register_definition(definition);
    });

    msg.handle(endpoint);
    cmd_dispatcher<realtime>::fire_system_callback(std::bind(d_recv_nrt3, definitions, endpoint));
}

void d_recv_nrt3(sc_synth_definition_ptr * definitions, endpoint_ptr endpoint)
{
    delete[] definitions;
    send_done_message(endpoint, "/d_recv");
}

template <bool realtime>
void handle_d_recv(received_message const & msg,
                   endpoint_ptr endpoint)
{
    const void * synthdef_data;
    osc::osc_bundle_element_size_t synthdef_size;

    osc::ReceivedMessageArgumentIterator args = msg.ArgumentsBegin();

    args->AsBlob(synthdef_data, synthdef_size); ++args;
    movable_array<char> def(synthdef_size, (const char*)synthdef_data);
    completion_message message = extract_completion_message(args);

    cmd_dispatcher<realtime>::fire_system_callback(std::bind(d_recv_nrt<realtime>, def, message, endpoint));
}

template <bool realtime>
void d_load_rt2(sc_synth_definition_ptr * definitions, size_t definition_count, completion_message & msg,
                endpoint_ptr endpoint);
void d_load_nrt3(sc_synth_definition_ptr * definitions, endpoint_ptr endpoint);

template <bool realtime>
void d_load_nrt(movable_string & path, completion_message & msg, endpoint_ptr endpoint)
{
    size_t count;
    sc_synth_definition_ptr * definitions;
    /* TODO: we need to implment some file name pattern matching */
    boost::tie(definitions, count) = wrap_synthdefs(sc_read_synthdefs_file(path.c_str()));

    cmd_dispatcher<realtime>::fire_rt_callback(std::bind(d_load_rt2<realtime>, definitions, count, msg, endpoint));
}

template <bool realtime>
void d_load_rt2(sc_synth_definition_ptr * definitions, size_t definition_count, completion_message & msg,
                endpoint_ptr endpoint)
{
    std::for_each(definitions, definitions + definition_count, [](sc_synth_definition_ptr const & definition) {
        instance->register_definition(definition);
    });

    msg.handle(endpoint);
    cmd_dispatcher<realtime>::fire_system_callback(std::bind(d_load_nrt3, definitions, endpoint));
}

void d_load_nrt3(sc_synth_definition_ptr * definitions, endpoint_ptr endpoint)
{
    delete[] definitions;
    send_done_message(endpoint, "/d_load");
}


template <bool realtime>
void handle_d_load(received_message const & msg,
                   endpoint_ptr endpoint)
{
    osc::ReceivedMessageArgumentIterator args = msg.ArgumentsBegin();
    const char * path = args->AsString(); args++;
    completion_message message = extract_completion_message(args);

    cmd_dispatcher<realtime>::fire_system_callback(std::bind(d_load_nrt<realtime>, movable_string(path),
                                                               message, endpoint));
}


template <bool realtime>
void d_loadDir_rt2(sc_synth_definition_ptr * definitions, size_t definition_count, completion_message & msg,
                   endpoint_ptr endpoint);
void d_loadDir_nrt3(sc_synth_definition_ptr * definitions, endpoint_ptr endpoint);

template <bool realtime>
void d_loadDir_nrt1(movable_string & path, completion_message & msg, endpoint_ptr endpoint)
{
    size_t count;
    sc_synth_definition_ptr * definitions;
    boost::tie(definitions, count) = wrap_synthdefs(sc_read_synthdefs_dir(path.c_str()));

    cmd_dispatcher<realtime>::fire_rt_callback(std::bind(d_loadDir_rt2<realtime>, definitions, count, msg, endpoint));
}

template <bool realtime>
void d_loadDir_rt2(sc_synth_definition_ptr * definitions, size_t definition_count, completion_message & msg,
                   endpoint_ptr endpoint)
{
    std::for_each(definitions, definitions + definition_count, [](sc_synth_definition_ptr const & definition) {
        instance->register_definition(definition);
    });

    msg.handle(endpoint);
    cmd_dispatcher<realtime>::fire_system_callback(std::bind(d_loadDir_nrt3, definitions, endpoint));
}

void d_loadDir_nrt3(sc_synth_definition_ptr * definitions, endpoint_ptr endpoint)
{
    delete[] definitions;
    send_done_message(endpoint, "/d_loadDir");
}

template <bool realtime>
void handle_d_loadDir(received_message const & msg,
                      endpoint_ptr endpoint)
{
    osc::ReceivedMessageArgumentStream args = msg.ArgumentStream();
    const char * path;

    args >> path;
    completion_message message = extract_completion_message(args);

    cmd_dispatcher<realtime>::fire_system_callback(std::bind(d_loadDir_nrt1<realtime>,
                                                               movable_string(path), message, endpoint));
}


void handle_d_free(received_message const & msg)
{
    osc::ReceivedMessageArgumentStream args = msg.ArgumentStream();

    while(!args.Eos())
    {
        const char * defname;
        args >> defname;

        instance->remove_definition(defname);
    }
}

void insert_parallel_group(int node_id, int action, int target_id)
{
    if (node_id == -1)
        node_id = instance->generate_node_id();
    else if (!check_node_id(node_id))
        return;

    server_node * target = find_node(target_id);
    if(!target)
        return;

    node_position_constraint pos = make_pair(target, node_position(action));
    if (!node_position_sanity_check(pos))
        return;

    instance->add_parallel_group(node_id, pos);
    last_generated = node_id;
}

void handle_p_new(received_message const & msg)
{
    osc::ReceivedMessageArgumentStream args = msg.ArgumentStream();

    while(!args.Eos()) {
        osc::int32 id, action, target;
        args >> id >> action >> target;

        insert_parallel_group(id, action, target);
    }
}

void handle_u_cmd(received_message const & msg, int size)
{
    sc_msg_iter args(size, msg.AddressPattern());

    int node_id = args.geti();

    server_node * target_synth = find_node(node_id);

    if (target_synth == nullptr || target_synth->is_group())
        return;

    sc_synth * synth = static_cast<sc_synth*>(target_synth);

    int ugen_index = args.geti();
    const char * cmd_name = args.gets();

    synth->apply_unit_cmd(cmd_name, ugen_index, &args);
}

void handle_cmd(received_message const & msg, int size, endpoint_ptr endpoint, int skip_bytes)
{
    sc_msg_iter args(size, msg.AddressPattern() + skip_bytes);

    const char * cmd = args.gets();

    sc_factory->run_cmd_plugin(&sc_factory->world, cmd, &args, endpoint.get());
}

} /* namespace */

template <bool realtime>
void sc_osc_handler::handle_message_int_address(received_message const & message,
                                                size_t msg_size, endpoint_ptr const & endpoint)
{
    uint32_t address = message.AddressPatternAsUInt32();

    switch (address) {
    case cmd_none:
        break;

    case cmd_quit:
        handle_quit<realtime>(endpoint);
        break;

    case cmd_s_new:
        handle_s_new(message);
        break;

    case cmd_s_noid:
        handle_s_noid(message);
        break;

    case cmd_s_get:
        handle_s_get<realtime>(message, msg_size, endpoint);
        break;

    case cmd_s_getn:
        handle_s_getn<realtime>(message, msg_size, endpoint);
        break;

    case cmd_notify:
        handle_notify<realtime>(message, endpoint);
        break;

    case cmd_status:
        handle_status<realtime>(endpoint);
        break;

    case cmd_dumpOSC:
        handle_dumpOSC(message);
        break;

    case cmd_sync:
        handle_sync<realtime>(message, endpoint);
        break;

    case cmd_clearSched:
        handle_clearSched();
        break;

    case cmd_error:
        handle_error(message);
        break;

    case cmd_g_new:
        handle_g_new(message);
        break;

    case cmd_g_head:
        handle_g_head_or_tail<head>(message);
        break;

    case cmd_g_tail:
        handle_g_head_or_tail<tail>(message);
        break;

    case cmd_g_freeAll:
        handle_g_freeall(message);
        break;

    case cmd_g_deepFree:
        handle_g_deepFree(message);
        break;

    case cmd_g_queryTree:
        handle_g_queryTree<realtime>(message, endpoint);
        break;

    case cmd_g_dumpTree:
        handle_g_dumpTree(message);
        break;

    case cmd_n_free:
        handle_n_free(message);
        break;

    case cmd_n_set:
        handle_n_set(message);
        break;

    case cmd_n_setn:
        handle_n_setn(message);
        break;

    case cmd_n_fill:
        handle_n_fill(message);
        break;

    case cmd_n_map:
        handle_n_map(message);
        break;

    case cmd_n_mapn:
        handle_n_mapn(message);
        break;

    case cmd_n_mapa:
        handle_n_mapa(message);
        break;

    case cmd_n_mapan:
        handle_n_mapan(message);
        break;

    case cmd_n_query:
        handle_n_query<realtime>(message, endpoint);
        break;

    case cmd_n_order:
        handle_n_order(message);
        break;

    case cmd_n_run:
        handle_n_run(message);
        break;

    case cmd_n_before:
        handle_n_before_or_after<before>(message);
        break;

    case cmd_n_after:
        handle_n_before_or_after<after>(message);
        break;

    case cmd_n_trace:
        handle_n_trace(message);
        break;

    case cmd_b_alloc:
        handle_b_alloc<realtime>(message, endpoint);
        break;

    case cmd_u_cmd:
        handle_u_cmd(message, msg_size);
        break;

    case cmd_b_free:
        handle_b_free<realtime>(message, endpoint);
        break;

    case cmd_b_allocRead:
        handle_b_allocRead<realtime>(message, endpoint);
        break;

    case cmd_b_allocReadChannel:
        handle_b_allocReadChannel<realtime>(message, endpoint);
        break;

    case cmd_b_read:
        handle_b_read<realtime>(message, endpoint);
        break;

    case cmd_b_readChannel:
        handle_b_readChannel<realtime>(message, endpoint);
        break;

    case cmd_b_write:
        handle_b_write<realtime>(message, endpoint);
        break;

    case cmd_b_zero:
        handle_b_zero<realtime>(message, endpoint);
        break;

    case cmd_b_set:
        handle_b_set(message);
        break;

    case cmd_b_setn:
        handle_b_setn(message);
        break;

    case cmd_b_fill:
        handle_b_fill(message);
        break;

    case cmd_b_query:
        handle_b_query<realtime>(message, endpoint);
        break;

    case cmd_b_get:
        handle_b_get<realtime>(message, endpoint);
        break;

    case cmd_b_getn:
        handle_b_getn<realtime>(message, endpoint);
        break;

    case cmd_b_gen:
        handle_b_gen<realtime>(message, msg_size, endpoint);
        break;

    case cmd_b_close:
        handle_b_close<realtime>(message, endpoint);
        break;

    case cmd_c_set:
        handle_c_set(message);
        break;

    case cmd_c_setn:
        handle_c_setn(message);
        break;

    case cmd_c_fill:
        handle_c_fill(message);
        break;

    case cmd_c_get:
        handle_c_get<realtime>(message, endpoint);
        break;

    case cmd_c_getn:
        handle_c_getn<realtime>(message, endpoint);
        break;

    case cmd_d_recv:
        handle_d_recv<realtime>(message, endpoint);
        break;

    case cmd_d_load:
        handle_d_load<realtime>(message, endpoint);
        break;

    case cmd_d_loadDir:
        handle_d_loadDir<realtime>(message, endpoint);
        break;

    case cmd_d_free:
        handle_d_free(message);
        break;

    case cmd_p_new:
        handle_p_new(message);
        break;

    case cmd_cmd:
        handle_cmd(message, msg_size, endpoint, 4);
        break;

    default:
        handle_unhandled_message(message);
    }
}

namespace
{

template <bool realtime>
void dispatch_group_commands(const char * address, received_message const & message,
                             endpoint_ptr const & endpoint)
{
    assert(address[1] == 'g');
    assert(address[2] == '_');

    if (strcmp(address+3, "new") == 0) {
        handle_g_new(message);
        return;
    }
    if (strcmp(address+3, "head") == 0) {
        handle_g_head_or_tail<head>(message);
        return;
    }
    if (strcmp(address+3, "tail") == 0) {
        handle_g_head_or_tail<tail>(message);
        return;
    }
    if (strcmp(address+3, "freeAll") == 0) {
        handle_g_freeall(message);
        return;
    }
    if (strcmp(address+3, "deepFree") == 0) {
        handle_g_deepFree(message);
        return;
    }
    if (strcmp(address+3, "queryTree") == 0) {
        handle_g_queryTree<realtime>(message, endpoint);
        return;
    }

    if (strcmp(address+3, "dumpTree") == 0) {
        handle_g_dumpTree(message);
        return;
    }
}

template <bool realtime>
void dispatch_node_commands(const char * address, received_message const & message,
                            endpoint_ptr const & endpoint)
{
    assert(address[1] == 'n');
    assert(address[2] == '_');

    if (strcmp(address+3, "free") == 0) {
        handle_n_free(message);
        return;
    }

    if (strcmp(address+3, "set") == 0) {
        handle_n_set(message);
        return;
    }

    if (strcmp(address+3, "setn") == 0) {
        handle_n_setn(message);
        return;
    }

    if (strcmp(address+3, "fill") == 0) {
        handle_n_fill(message);
        return;
    }

    if (strcmp(address+3, "map") == 0) {
        handle_n_map(message);
        return;
    }

    if (strcmp(address+3, "mapn") == 0) {
        handle_n_mapn(message);
        return;
    }

    if (strcmp(address+3, "mapa") == 0) {
        handle_n_mapa(message);
        return;
    }

    if (strcmp(address+3, "mapan") == 0) {
        handle_n_mapan(message);
        return;
    }

    if (strcmp(address+3, "run") == 0) {
        handle_n_run(message);
        return;
    }

    if (strcmp(address+3, "before") == 0) {
        handle_n_before_or_after<before>(message);
        return;
    }

    if (strcmp(address+3, "after") == 0) {
        handle_n_before_or_after<after>(message);
        return;
    }

    if (strcmp(address+3, "order") == 0) {
        handle_n_order(message);
        return;
    }

    if (strcmp(address+3, "query") == 0) {
        handle_n_query<realtime>(message, endpoint);
        return;
    }

    if (strcmp(address+3, "trace") == 0) {
        handle_n_trace(message);
        return;
    }
}

template <bool realtime>
void dispatch_buffer_commands(const char * address, received_message const & message,
                              size_t msg_size, endpoint_ptr const & endpoint)
{
    assert(address[1] == 'b');
    assert(address[2] == '_');

    if (strcmp(address+3, "alloc") == 0) {
        handle_b_alloc<realtime>(message, endpoint);
        return;
    }

    if (strcmp(address+3, "free") == 0) {
        handle_b_free<realtime>(message, endpoint);
        return;
    }

    if (strcmp(address+3, "allocRead") == 0) {
        handle_b_allocRead<realtime>(message, endpoint);
        return;
    }
    if (strcmp(address+3, "allocReadChannel") == 0) {
        handle_b_allocReadChannel<realtime>(message, endpoint);
        return;
    }

    if (strcmp(address+3, "read") == 0) {
        handle_b_read<realtime>(message, endpoint);
        return;
    }

    if (strcmp(address+3, "readChannel") == 0) {
        handle_b_readChannel<realtime>(message, endpoint);
        return;
    }

    if (strcmp(address+3, "write") == 0) {
        handle_b_write<realtime>(message, endpoint);
        return;
    }

    if (strcmp(address+3, "zero") == 0) {
        handle_b_zero<realtime>(message, endpoint);
        return;
    }

    if (strcmp(address+3, "set") == 0) {
        handle_b_set(message);
        return;
    }

    if (strcmp(address+3, "setn") == 0) {
        handle_b_setn(message);
        return;
    }

    if (strcmp(address+3, "fill") == 0) {
        handle_b_fill(message);
        return;
    }

    if (strcmp(address+3, "query") == 0) {
        handle_b_query<realtime>(message, endpoint);
        return;
    }

    if (strcmp(address+3, "get") == 0) {
        handle_b_get<realtime>(message, endpoint);
        return;
    }

    if (strcmp(address+3, "getn") == 0) {
        handle_b_getn<realtime>(message, endpoint);
        return;
    }

    if (strcmp(address+3, "gen") == 0) {
        handle_b_gen<realtime>(message, msg_size, endpoint);
        return;
    }

    if (strcmp(address+3, "close") == 0) {
        handle_b_close<realtime>(message, endpoint);
        return;
    }
}

template <bool realtime>
void dispatch_control_bus_commands(const char * address, received_message const & message,
                                   endpoint_ptr const & endpoint)
{
    assert(address[1] == 'c');
    assert(address[2] == '_');

    if (strcmp(address+3, "set") == 0) {
        handle_c_set(message);
        return;
    }

    if (strcmp(address+3, "setn") == 0) {
        handle_c_setn(message);
        return;
    }

    if (strcmp(address+3, "fill") == 0) {
        handle_c_fill(message);
        return;
    }

    if (strcmp(address+3, "get") == 0) {
        handle_c_get<realtime>(message, endpoint);
        return;
    }

    if (strcmp(address+3, "getn") == 0) {
        handle_c_getn<realtime>(message, endpoint);
        return;
    }
}

template <bool realtime>
void dispatch_synthdef_commands(const char * address, received_message const & message,
                                endpoint_ptr const & endpoint)
{
    assert(address[1] == 'd');
    assert(address[2] == '_');

    if (strcmp(address+3, "recv") == 0) {
        handle_d_recv<realtime>(message, endpoint);
        return;
    }

    if (strcmp(address+3, "load") == 0) {
        handle_d_load<realtime>(message, endpoint);
        return;
    }

    if (strcmp(address+3, "loadDir") == 0) {
        handle_d_loadDir<realtime>(message, endpoint);
        return;
    }

    if (strcmp(address+3, "free") == 0) {
        handle_d_free(message);
        return;
    }
}

template <bool realtime>
void dispatch_synth_commands(const char * address, received_message const & message, size_t msg_size,
                             endpoint_ptr const & endpoint)
{
    assert(address[1] == 's');
    assert(address[2] == '_');

    if (strcmp(address+3, "new") == 0) {
        handle_s_new(message);
        return;
    }

    if (strcmp(address+3, "noid") == 0) {
        handle_s_noid(message);
        return;
    }

    if (strcmp(address+3, "get") == 0) {
        handle_s_get<realtime>(message, msg_size, endpoint);
        return;
    }

    if (strcmp(address+3, "getn") == 0) {
        handle_s_getn<realtime>(message, msg_size, endpoint);
        return;
    }
}

} /* namespace */

template <bool realtime>
void sc_osc_handler::handle_message_sym_address(received_message const & message,
                                                size_t msg_size, endpoint_ptr const & endpoint)
{
    const char * address = message.AddressPattern();

    /* scsynth doesn't require the leading / */
    if(address[0] != '/')
        address -= 1;

    if (address[2] == '_')
    {
        if (address[1] == 'g') {
            dispatch_group_commands<realtime>(address, message, endpoint);
            return;
        }

        if (address[1] == 'n') {
            dispatch_node_commands<realtime>(address, message, endpoint);
            return;
        }

        if (address[1] == 'b') {
            dispatch_buffer_commands<realtime>(address, message, msg_size, endpoint);
            return;
        }

        if (address[1] == 'c') {
            dispatch_control_bus_commands<realtime>(address, message, endpoint);
            return;
        }

        if (address[1] == 'd') {
            dispatch_synthdef_commands<realtime>(address, message, endpoint);
            return;
        }

        if (address[1] == 's') {
            dispatch_synth_commands<realtime>(address, message, msg_size, endpoint);
            return;
        }
    }

    if (strcmp(address+1, "p_new") == 0) {
        handle_p_new(message);
        return;
    }

    if (strcmp(address+1, "u_cmd") == 0) {
        handle_u_cmd(message, msg_size);
        return;
    }

    if (strcmp(address+1, "status") == 0) {
        handle_status<realtime>(endpoint);
        return;
    }

    if (strcmp(address+1, "sync") == 0) {
        handle_sync<realtime>(message, endpoint);
        return;
    }

    if (strcmp(address+1, "quit") == 0) {
        handle_quit<realtime>(endpoint);
        return;
    }

    if (strcmp(address+1, "notify") == 0) {
        handle_notify<realtime>(message, endpoint);
        return;
    }

    if (strcmp(address+1, "dumpOSC") == 0) {
        handle_dumpOSC(message);
        return;
    }

    if (strcmp(address+1, "clearSched") == 0) {
        handle_clearSched();
        return;
    }

    if (strcmp(address+1, "error") == 0) {
        handle_error(message);
        return;
    }

    if (strcmp(address+1, "cmd") == 0) {
        handle_cmd(message, msg_size, endpoint, 8);
        return;
    }

    if (strcmp(address+1, "none") == 0)
        return;

    handle_unhandled_message(message);
}


template <bool realtime>
void handle_asynchronous_plugin_cleanup(World * world, void *cmdData,
                                       AsyncFreeFn cleanup)
{
    if (cleanup)
        (cleanup)(world, cmdData);
}

template <bool realtime>
void handle_asynchronous_plugin_stage4(World * world, const char * cmdName, void *cmdData, AsyncStageFn stage4,
                                       AsyncFreeFn cleanup, completion_message & msg, endpoint_ptr endpoint)
{
    if (stage4)
        (stage4)(world, cmdData);

    cmd_dispatcher<realtime>::fire_rt_callback(std::bind(handle_asynchronous_plugin_cleanup<realtime>, world, cmdData,
                                                           cleanup));

    send_done_message(endpoint, cmdName);
}

template <bool realtime>
void handle_asynchronous_plugin_stage3(World * world, const char * cmdName, void *cmdData, AsyncStageFn stage3, AsyncStageFn stage4,
                                       AsyncFreeFn cleanup, completion_message & msg, endpoint_ptr endpoint)
{
    if (stage3) {
        bool success = (stage3)(world, cmdData);
        if (success)
            msg.handle(endpoint);
    }
    cmd_dispatcher<realtime>::fire_system_callback(std::bind(handle_asynchronous_plugin_stage4<realtime>, world, cmdName,
                                                               cmdData, stage4, cleanup, msg, endpoint));
}

template <bool realtime>
void handle_asynchronous_plugin_stage2(World * world, const char * cmdName, void *cmdData, AsyncStageFn stage2,
                                       AsyncStageFn stage3, AsyncStageFn stage4,
                                       AsyncFreeFn cleanup, completion_message & msg, endpoint_ptr endpoint)
{
    if (stage2)
        (stage2)(world, cmdData);

    cmd_dispatcher<realtime>::fire_rt_callback(std::bind(handle_asynchronous_plugin_stage3<realtime>, world, cmdName,
                                                           cmdData, stage3, stage4,
                                                           cleanup, msg, endpoint));
}

void sc_osc_handler::do_asynchronous_command(World * world, void* replyAddr, const char* cmdName, void *cmdData,
                                             AsyncStageFn stage2, AsyncStageFn stage3, AsyncStageFn stage4, AsyncFreeFn cleanup,
                                             int completionMsgSize, void* completionMsgData)
{
    completion_message msg(completionMsgSize, completionMsgData);
    endpoint_ptr shared_endpoint;

    nova_endpoint * endpoint = replyAddr ? static_cast<nova_endpoint*>(replyAddr)
                                         : nullptr;

    if (endpoint)
        shared_endpoint = endpoint->shared_from_this();

    if (world->mRealTime)
        cmd_dispatcher<true>::fire_system_callback(std::bind(handle_asynchronous_plugin_stage2<true>, world, cmdName,
                                                               cmdData, stage2, stage3, stage4, cleanup, msg, shared_endpoint));
    else
        cmd_dispatcher<false>::fire_system_callback(std::bind(handle_asynchronous_plugin_stage2<false>, world, cmdName,
                                                                cmdData, stage2, stage3, stage4, cleanup, msg, shared_endpoint));
}


} /* namespace detail */
} /* namespace nova */
