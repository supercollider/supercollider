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

#include "sc_osc_handler.hpp"
#include "server.hpp"

namespace nova
{

using namespace std;

namespace detail
{

void sc_osc_handler::handle_packet(const char * data, size_t length)
{
    received_packet * p = received_packet::alloc_packet(data, length, remote_endpoint_);
    instance->add_sync_callback(p);
}

sc_osc_handler::received_packet::received_packet *
sc_osc_handler::received_packet::alloc_packet(const char * data, size_t length,
                                              udp::endpoint const & remote_endpoint)
{
#if 0
    /* received_packet struct and data array are located in one memory chunk */
    void * chunk = received_packet::allocate(sizeof(received_packet) + length);
    received_packet * p = (received_packet*)chunk;
    char * cpy = (char*)(chunk) + sizeof(received_packet);
    memcpy(cpy, data, length);

    /* this doesn't work :/ */
    new(p) received_packet(cpy, length, remote_endpoint);
    return p;
#else
    return new received_packet(data, length, remote_endpoint);
#endif
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
    bool handled = handle_bundle(bundle, packet);
/*     if (handled) */
/*         delete packet; */
}

bool sc_osc_handler::handle_bundle(received_bundle const & bundle, received_packet * packet)
{
    time_tag now(~0L);
    time_tag bundle_time = bundle.TimeTag();

    if (bundle_time < now) {
        typedef osc::ReceivedBundleElementIterator bundle_iterator;
        typedef osc::ReceivedBundleElement bundle_element;

        bool handled = true;

        for (bundle_iterator it = bundle.ElementsBegin(); it != bundle.ElementsEnd(); ++it) {
            bundle_element const & element = *it;

            if (element.IsBundle()) {
                received_bundle inner_bundle(element);
                bool handled = handle_bundle(inner_bundle, packet);
                if (!handled)
                    handled = false;
            } else {
                received_message message(element);
                handle_message(message, packet);
            }
        }

        return handled;
    } else {
        /** todo defer bundles with timetags */
        return true;
    }
}

void sc_osc_handler::handle_message(received_packet * packet)
{
    osc_received_packet received_packet(packet->data, packet->length);
    received_message message(received_packet);
    handle_message (message, packet);
/*     delete packet; */
}

void sc_osc_handler::handle_message(received_message const & message, received_packet * packet)
{
    try
    {
        if (message.AddressPatternIsUInt32())
            handle_message_int_address(message, packet);
        else
            handle_message_sym_address(message, packet);
    }
    catch (std::exception const & e)
    {
        cerr << e.what() << endl;
    }
}

namespace {

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

void handle_notify(sc_osc_handler::received_message const & message, udp::endpoint const & endpoint)
{
    osc::ReceivedMessageArgumentStream args = message.ArgumentStream();
    osc::int32 enable;

    args >> enable;

    instance->add_system_callback(new notify_callback(enable != 0, endpoint));
}

} /* namespace */

void sc_osc_handler::handle_message_int_address(received_message const & message, received_packet * packet)
{
    uint32_t address = message.AddressPatternAsUInt32();

    switch (address)
    {
    case cmd_quit:
        handle_quit(packet->endpoint_);
        break;

    case cmd_notify:
        handle_notify(message, packet->endpoint_);
        break;

    default:
        cerr << "unhandled message" << endl;
    }
}

void sc_osc_handler::handle_message_sym_address(received_message const & message, received_packet * packet)
{
    const char * address = message.AddressPattern();

#ifndef NDEBUG
    cerr << "handling message " << address << endl;
#endif

    assert(address[0] == '/');
    if (strcmp(address+1, "quit") == 0) {
        handle_quit(packet->endpoint_);
        return;
    }

    if (strcmp(address+1, "notify") == 0) {
        handle_notify(message, packet->endpoint_);
        return;
    }

}

} /* namespace detail */
} /* namespace nova */

