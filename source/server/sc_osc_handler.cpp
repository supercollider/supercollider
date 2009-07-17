//  osc handler for supercollider-style communication, implementation
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


#include <iostream>

#include "osc/OscOutboundPacketStream.h"
#include "osc/OscPrintReceivedElements.h"

#include "sc_osc_handler.hpp"
#include "server.hpp"

namespace nova
{

using namespace std;

namespace detail
{

void sc_scheduled_bundles::bundle_node::run(void)
{
    typedef osc::ReceivedBundleElement bundle_element;
    typedef osc::ReceivedBundle received_bundle;
    typedef osc::ReceivedMessage received_message;

    bundle_element element(data_); /* odd, but true :/ */

    if (element.IsBundle()) {
        received_bundle bundle(element);
        instance->handle_bundle(bundle, endpoint_);
    } else {
        received_message message(element);
        instance->handle_message(message, endpoint_);
    }
}

void sc_scheduled_bundles::insert_bundle(time_tag const & timeout, const char * data, size_t length,
                                         udp::endpoint const & endpoint)
{
    /* allocate chunk from realtime pool */
    void * chunk = rt_pool.malloc(sizeof(bundle_node) + length+4);
    bundle_node * node = (bundle_node*)chunk;
    char * cpy = (char*)chunk + sizeof(bundle_node);

    memcpy(cpy, data - 4, length+4);

    new(node) bundle_node(timeout, cpy, endpoint);

    bundle_list_t::iterator it;
    for (it = bundles.begin(); it != bundles.end(); ++it)
    {
        if (!(timeout < it->timeout_))
            break;
    }

    bundles.insert(it, *node);
}

void sc_scheduled_bundles::execute_bundles(time_tag const & now)
{
    while(!bundles.empty())
    {
        bundle_node & front = *bundles.begin();

        if (front.timeout_ <= now) {
            front.run();
            bundles.pop_front();
            front.~bundle_node();
            rt_pool.free(&front);
        }
        else
            return;
    }
}


void sc_osc_handler::handle_packet(const char * data, size_t length)
{
    received_packet * p = received_packet::alloc_packet(data, length, remote_endpoint_);

    if (dump_osc_packets == 1) {
        osc_received_packet packet (data, length);
        cout << "received osc packet " << packet;
    }

    instance->add_sync_callback(p);
}

sc_osc_handler::received_packet::received_packet *
sc_osc_handler::received_packet::alloc_packet(const char * data, size_t length,
                                              udp::endpoint const & remote_endpoint)
{
    /* received_packet struct and data array are located in one memory chunk */
    void * chunk = received_packet::allocate(sizeof(received_packet) + length);
    received_packet * p = (received_packet*)chunk;
    char * cpy = (char*)(chunk) + sizeof(received_packet);
    memcpy(cpy, data, length);

    new(p) received_packet(cpy, length, remote_endpoint);
    return p;
}

void sc_osc_handler::received_packet::run(void)
{
    osc_received_packet packet(data, length);
    if (packet.IsBundle())
        instance->handle_bundle(this);
    else
        instance->handle_message(this);
}

void sc_osc_handler::handle_bundle(received_packet * packet)
{
    osc_received_packet received_packet(packet->data, packet->length);
    received_bundle bundle(received_packet);
    handle_bundle(bundle, packet->endpoint_);
}

void sc_osc_handler::handle_bundle(received_bundle const & bundle, udp::endpoint const & endpoint)
{
    time_tag now = time_tag::from_ptime(boost::date_time::microsec_clock<boost::posix_time::ptime>::universal_time());
    time_tag bundle_time = bundle.TimeTag();

    typedef osc::ReceivedBundleElementIterator bundle_iterator;
    typedef osc::ReceivedBundleElement bundle_element;

    if ( bundle_time <= now) {
        for (bundle_iterator it = bundle.ElementsBegin(); it != bundle.ElementsEnd(); ++it) {
            bundle_element const & element = *it;

            if (element.IsBundle()) {
                received_bundle inner_bundle(element);
                handle_bundle(inner_bundle, endpoint);
            } else {
                received_message message(element);
                handle_message(message, endpoint);
            }
        }
    } else {
        for (bundle_iterator it = bundle.ElementsBegin(); it != bundle.ElementsEnd(); ++it) {
            bundle_element const & element = *it;
            scheduled_bundles.insert_bundle(bundle_time, element.Contents(), element.Size(), endpoint);
        }
    }
}

void sc_osc_handler::handle_message(received_packet * packet)
{
    osc_received_packet received_packet(packet->data, packet->length);
    received_message message(received_packet);
    handle_message (message, packet->endpoint_);
}

void sc_osc_handler::handle_message(received_message const & message, udp::endpoint const & endpoint)
{
    try
    {
        if (message.AddressPatternIsUInt32())
            handle_message_int_address(message, endpoint);
        else
            handle_message_sym_address(message, endpoint);
    }
    catch (std::exception const & e)
    {
        cerr << e.what() << endl;
    }
}

namespace {

typedef sc_osc_handler::received_message received_message;

enum {
    cmd_none = 0,

    cmd_notify = 1,
    cmd_status = 2,
    cmd_quit = 3,
    cmd_cmd = 4,

    cmd_d_recv = 5,
    cmd_d_load = 6,
    cmd_d_loadDir = 7,
    cmd_d_freeAll = 8,

    cmd_s_new = 9,
    cmd_n_trace = 10,
    cmd_n_free = 11,
    cmd_n_run = 12,
    cmd_n_cmd = 13,
    cmd_n_map = 14,
    cmd_n_set = 15,
    cmd_n_setn = 16,
    cmd_n_fill = 17,
    cmd_n_before = 18,
    cmd_n_after = 19,

    cmd_u_cmd = 20,

    cmd_g_new = 21,
    cmd_g_head = 22,
    cmd_g_tail = 23,
    cmd_g_freeAll = 24,
    cmd_c_set = 25,
    cmd_c_setn = 26,
    cmd_c_fill = 27,

    cmd_b_alloc = 28,
    cmd_b_allocRead = 29,
    cmd_b_read = 30,
    cmd_b_write = 31,
    cmd_b_free = 32,
    cmd_b_close = 33,
    cmd_b_zero = 34,
    cmd_b_set = 35,
    cmd_b_setn = 36,
    cmd_b_fill = 37,
    cmd_b_gen = 38,
    cmd_dumpOSC = 39,

    cmd_c_get = 40,
    cmd_c_getn = 41,
    cmd_b_get = 42,
    cmd_b_getn = 43,
    cmd_s_get = 44,
    cmd_s_getn = 45,
    cmd_n_query = 46,
    cmd_b_query = 47,

    cmd_n_mapn = 48,
    cmd_s_noid = 49,

    cmd_g_deepFree = 50,
    cmd_clearSched = 51,

    cmd_sync = 52,
    cmd_d_free = 53,

    cmd_b_allocReadChannel = 54,
    cmd_b_readChannel = 55,
    cmd_g_dumpTree = 56,
    cmd_g_queryTree = 57,

    cmd_error = 58,

    cmd_s_newargs = 59,

    cmd_n_mapa = 60,
    cmd_n_mapan = 61,
    NUMBER_OF_COMMANDS = 62
};

int first_arg_as_int(received_message const & message)
{
    osc::ReceivedMessageArgumentStream args = message.ArgumentStream();
    osc::int32 val;

    args >> val;

    return val;
}

struct sc_response_callback:
        public system_callback
{
    sc_response_callback(udp::endpoint const & endpoint):
        endpoint_(endpoint)
    {}

    void send_done(void)
    {
        char buffer[128];
        osc::OutboundPacketStream p(buffer, 128);
        p << osc::BeginMessage("/done")
          << osc::EndMessage;

        instance->send_udp(p.Data(), p.Size(), endpoint_);
    }

    udp::endpoint endpoint_;
};

struct quit_callback:
        public sc_response_callback
{
    quit_callback(udp::endpoint const & endpoint):
        sc_response_callback(endpoint)
    {}

    void run(void)
    {
        send_done();
        instance->terminate();
    }
};

void handle_quit(udp::endpoint const & endpoint)
{
    instance->add_system_callback(new quit_callback(endpoint));
}

struct notify_callback:
    public sc_response_callback
{
    notify_callback(bool enable, udp::endpoint const & endpoint):
        sc_response_callback(endpoint), enable_(enable)
    {}

    void run(void)
    {
        if (enable_)
            instance->add_observer(endpoint_);
        else
            instance->remove_observer(endpoint_);
        send_done();
    }

    bool enable_;
};

void handle_notify(received_message const & message, udp::endpoint const & endpoint)
{
    int enable = first_arg_as_int(message);

    instance->add_system_callback(new notify_callback(enable != 0, endpoint));
}

struct status_callback:
        public sc_response_callback
{
    status_callback(udp::endpoint const & endpoint):
        sc_response_callback(endpoint)
    {}

    void run(void)
    {
        char buffer[1024];
        osc::OutboundPacketStream p(buffer, 1024);
        p << osc::BeginMessage("status.reply")
          << 1                  /* unused */
          << -1                 /* ugens */
          << -1                 /* synths */
          << -1                 /* groups */
          << -1                 /* synthdefs */
          << -1.f               /* average cpu % */
          << -1.f               /* peak cpu % */
          << -1.0               /* nominal samplerate */
          << -1.0               /* actual samplerate */
          << osc::EndMessage;

        instance->send_udp(p.Data(), p.Size(), endpoint_);
    }
};

void handle_status(udp::endpoint const & endpoint)
{
    instance->add_system_callback(new status_callback(endpoint));
}

void handle_dumpOSC(received_message const & message)
{
    int val = first_arg_as_int(message);
    val = min (1, val);    /* we just support one way of dumping osc messages */

    instance->dumpOSC(val);     /* thread-safe */
}

struct sync_callback:
        public sc_response_callback
{
    sync_callback(int id, udp::endpoint const & endpoint):
        sc_response_callback(endpoint), id_(id)
    {}

    void run(void)
    {
        char buffer[128];
        osc::OutboundPacketStream p(buffer, 128);
        p << osc::BeginMessage("/synced")
          << id_
          << osc::EndMessage;

        instance->send_udp(p.Data(), p.Size(), endpoint_);
    }

    int id_;
};

void handle_sync(received_message const & message, udp::endpoint const & endpoint)
{
    int id = first_arg_as_int(message);

    instance->add_system_callback(new sync_callback(id, endpoint));
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
    cerr << "unhandled message " << msg.AddressPattern() << endl;
}

void insert_group(int node_id, int action, int target_id)
{
    server_node * target = instance->find_node(target_id);

    if (target == NULL) {
        cerr << "target node not found\n" << endl;
        return;
    }
    node_position_constraint pos = make_pair(target, node_position(action));

    instance->add_group(node_id, pos);
}

void handle_g_new(received_message const & msg)
{
    osc::ReceivedMessageArgumentStream args = msg.ArgumentStream();

    while(!args.Eos())
    {
        osc::int32 id, action, target;
        args >> id >> action >> target;

        insert_group(id, action, target);
    }
}

void handle_g_head(received_message const & msg)
{
    osc::ReceivedMessageArgumentStream args = msg.ArgumentStream();

    while(!args.Eos())
    {
        osc::int32 id, target;
        args >> id >> target;

        insert_group(id, head, target);
    }
}

void handle_g_tail(received_message const & msg)
{
    osc::ReceivedMessageArgumentStream args = msg.ArgumentStream();

    while(!args.Eos())
    {
        osc::int32 id, target;
        args >> id >> target;

        insert_group(id, tail, target);
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

            instance->free_node(id);
        }
        catch (std::exception e) {
            cerr << e.what() << endl;
        }
    }
}

} /* namespace */

void sc_osc_handler::handle_message_int_address(received_message const & message, udp::endpoint const & endpoint)
{
    uint32_t address = message.AddressPatternAsUInt32();

    switch (address)
    {
    case cmd_quit:
        handle_quit(endpoint);
        break;

    case cmd_notify:
        handle_notify(message, endpoint);
        break;

    case cmd_status:
        handle_status(endpoint);
        break;

    case cmd_dumpOSC:
        handle_dumpOSC(message);
        break;

    case cmd_sync:
        handle_sync(message, endpoint);
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
        handle_g_head(message);
        break;

    case cmd_g_tail:
        handle_g_tail(message);
        break;

    case cmd_n_free:
        handle_n_free(message);
        break;

    default:
        handle_unhandled_message(message);
    }
}

namespace
{
void dispatch_group_commands(received_message const & message, udp::endpoint const & endpoint)
{
    const char * address = message.AddressPattern();
    assert(address[0] == '/');
    assert(address[1] == 'g');
    assert(address[2] == '_');

    if (strcmp(address+3, "new") == 0) {
        handle_g_new(message);
        return;
    }
    if (strcmp(address+3, "head") == 0) {
        handle_g_head(message);
        return;
    }
    if (strcmp(address+3, "tail") == 0) {
        handle_g_tail(message);
        return;
    }
}

void dispatch_node_commands(received_message const & message, udp::endpoint const & endpoint)
{
    const char * address = message.AddressPattern();
    assert(address[0] == '/');
    assert(address[1] == 'n');
    assert(address[2] == '_');

    if (strcmp(address+3, "free") == 0) {
        handle_n_free(message);
        return;
    }
}

} /* namespace */

void sc_osc_handler::handle_message_sym_address(received_message const & message, udp::endpoint const & endpoint)
{
    const char * address = message.AddressPattern();

#ifndef NDEBUG
    cerr << "handling message " << address << endl;
#endif

    assert(address[0] == '/');

    if (address[1] == 'g') {
        dispatch_group_commands(message, endpoint);
        return;
    }

    if (address[1] == 'n') {
        dispatch_node_commands(message, endpoint);
        return;
    }

    if (strcmp(address+1, "status") == 0) {
        handle_status(endpoint);
        return;
    }

    if (strcmp(address+1, "sync") == 0) {
        handle_sync(message, endpoint);
        return;
    }

    if (strcmp(address+1, "quit") == 0) {
        handle_quit(endpoint);
        return;
    }

    if (strcmp(address+1, "notify") == 0) {
        handle_notify(message, endpoint);
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

    handle_unhandled_message(message);
}

} /* namespace detail */
} /* namespace nova */

