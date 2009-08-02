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
#include "utilities/sized_array.hpp"

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
    instance->handle_packet(data, length, endpoint_);
}

void sc_osc_handler::handle_packet(const char * data, std::size_t length, udp::endpoint const & endpoint)
{
    osc_received_packet packet(data, length);
    if (packet.IsBundle())
    {
        received_bundle bundle(packet);
        handle_bundle(bundle, endpoint);
    }
    else
    {
        received_message message(packet);
        handle_message (message, endpoint);
    }
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

    const udp::endpoint endpoint_;
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

    const bool enable_;
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

    const int id_;
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

void handle_g_freeall(received_message const & msg)
{
    osc::ReceivedMessageArgumentStream args = msg.ArgumentStream();

    while(!args.Eos())
    {
        osc::int32 id;
        args >> id;

        server_node * node = instance->find_node(id);

        if (node == NULL) {
            cerr << "node not found\n" << endl;
            continue;
        }
        if (node->is_synth()) {
            cerr << "node is a synth\n" << endl;
            continue;
        }

        abstract_group * group = static_cast<abstract_group*>(node);

        group->free_children();
    }
}

void handle_g_deepFree(received_message const & msg)
{
    osc::ReceivedMessageArgumentStream args = msg.ArgumentStream();

    while(!args.Eos())
    {
        osc::int32 id;
        args >> id;

        server_node * node = instance->find_node(id);

        if (node == NULL) {
            cerr << "node not found\n" << endl;
            continue;
        }
        if (node->is_synth()) {
            cerr << "node is a synth\n" << endl;
            continue;
        }

        abstract_group * group = static_cast<abstract_group*>(node);

        group->free_synths_deep();
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
        catch (std::exception & e) {
            cerr << e.what() << endl;
        }
    }
}

/** responding callback, which is executing an osc message when done */
struct sc_async_callback:
    public sc_response_callback
{
protected:
    sc_async_callback(size_t msg_size, const void * data, udp::endpoint const & endpoint):
        sc_response_callback(endpoint), msg_size_(msg_size)
    {
        if (msg_size_) {
            data_ = system_callback::allocate(msg_size);
            memcpy(data_, data, msg_size);
        }
    }

    static char * copy_string(const char * str)
    {
        size_t length = strlen(str);
        char * ret = (char*)system_callback::allocate(length + 1); /* terminating \0 */
        strcpy(ret, str);
        return ret;
    }

    void schedule_async_message(void)
    {
        if (msg_size_) {
            sc_osc_handler::received_packet * p =
                sc_osc_handler::received_packet::alloc_packet((char*)data_, msg_size_, endpoint_);
            instance->add_sync_callback(p);
        }
    }

    ~sc_async_callback(void)
    {
        if (msg_size_)
            system_callback::deallocate(data_);
    }

    const size_t msg_size_;
    void * data_;
};

struct b_alloc_callback:
    public sc_async_callback
{
    b_alloc_callback(int index, int frames, int channels, size_t msg_size, const void * data,
                     udp::endpoint const & endpoint):
        sc_async_callback(msg_size, data, endpoint),
        index_(index), frames_(frames), channels_(channels)
    {}

    void run(void)
    {
        instance->allocate_buffer(index_, frames_, channels_);
        schedule_async_message();
        send_done();
    }

    const int index_;
    const int frames_;
    const int channels_;
};

void handle_b_alloc(received_message const & msg, udp::endpoint const & endpoint)
{
    osc::ReceivedMessageArgumentStream args = msg.ArgumentStream();

    osc::int32 index, frames, channels;
    osc::Blob blob(0, 0);

    args >> index >> frames >> channels;

    if (!args.Eos())
        args >> blob;

    instance->add_system_callback(new b_alloc_callback(index, frames, channels,
                                                       blob.size, blob.data, endpoint));
}

struct b_free_callback:
    public sc_async_callback
{
    b_free_callback(int index, size_t msg_size, const void * data,
                     udp::endpoint const & endpoint):
        sc_async_callback(msg_size, data, endpoint),
        index_(index)
    {}

    void run(void)
    {
        instance->free_buffer(index_);
        schedule_async_message();
        send_done();
    }

    const int index_;
};

void handle_b_free(received_message const & msg, udp::endpoint const & endpoint)
{
    osc::ReceivedMessageArgumentStream args = msg.ArgumentStream();

    osc::int32 index;
    osc::Blob blob(0, 0);

    args >> index;

    if (!args.Eos())
        args >> blob;

    instance->add_system_callback(new b_free_callback(index, blob.size, blob.data, endpoint));
}

struct b_allocRead_callback:
    public sc_async_callback
{
    b_allocRead_callback(int index, const char * filename, size_t start, size_t frames,
                         size_t msg_size, const void * data, udp::endpoint const & endpoint):
        sc_async_callback(msg_size, data, endpoint),
        index_(index), filename_(copy_string(filename)), start_(start), frames_(frames)
    {}

    ~b_allocRead_callback(void)
    {
        deallocate(filename_);
    }

    void run(void)
    {
        instance->read_buffer_allocate(index_, filename_, start_, frames_);
        schedule_async_message();
        send_done();
    }

    const int index_;
    char * const filename_;
    const size_t start_;
    const size_t frames_;
};

void handle_b_allocRead(received_message const & msg, udp::endpoint const & endpoint)
{
    osc::ReceivedMessageArgumentStream args = msg.ArgumentStream();

    osc::int32 index;
    const char * filename;

    osc::int32 start = 0;
    osc::int32 frames = 0;
    osc::Blob blob (0, 0);

    args >> index >> filename;

    if (!args.Eos())
        args >> start;

    if (!args.Eos())
        args >> frames;

    if (!args.Eos())
        args >> blob;

    instance->add_system_callback(new b_allocRead_callback(index, filename, start, frames,
                                                           blob.size, blob.data, endpoint));
}

struct b_allocReadChannel_callback:
    public b_allocRead_callback
{
    b_allocReadChannel_callback(int index, const char * filename, size_t start, size_t frames,
                                size_t channel_count, const uint * channels,
                                size_t msg_size, const void * data, udp::endpoint const & endpoint):
        b_allocRead_callback(index, filename, start, frames, msg_size, data, endpoint),
        channel_count_(channel_count)
    {
        channels_ = (uint*)allocate(channel_count * sizeof (uint));
        memcpy(channels_, channels, channel_count * sizeof (uint));
    }

    ~b_allocReadChannel_callback(void)
    {
        deallocate(channels_);
    }

    void run(void)
    {
        instance->read_buffer_channels_allocate(index_, filename_, start_, frames_,
                                                channel_count_, channels_);
        schedule_async_message();
        send_done();
    }

    const size_t channel_count_;
    uint * channels_;
};

void handle_b_allocReadChannel(received_message const & msg, udp::endpoint const & endpoint)
{
    osc::ReceivedMessageArgumentIterator arg = msg.ArgumentsBegin();

    osc::int32 index = arg->AsInt32(); arg++;
    const char * filename = arg->AsString(); arg++;

    osc::int32 start = arg->AsInt32(); arg++;
    size_t frames = arg->AsInt32(); arg++;

    size_t channel_args = msg.ArgumentCount() - 4; /* we already consumed 4 elements */

    size_t channel_count = 0;
    sized_array<uint, rt_pool_allocator<uint> > channels(channel_args);

    const void * data = 0;
    size_t length = 0;
    for (uint i = 0; i != channel_args; ++i)
    {
        if (arg->IsInt32()) {
            channels[i] = arg->AsInt32Unchecked();
            ++channel_count;
        }
        else {
            /* we reached the message blob */
            if (!arg->IsBlob())
                throw std::runtime_error("wrong arguments for /b_allocReadChannel");

            arg->AsBlobUnchecked(data, length);
            break;
        }
    }

    instance->add_system_callback(
        new b_allocReadChannel_callback(index, filename, start, frames,
                                        channel_count, channels.c_array(),
                                        length, data, endpoint));
}

struct b_write_callback:
    public sc_async_callback
{
    b_write_callback(int index, const char * filename, const char * header_format,
                     const char * sample_format, size_t start, size_t frames, bool leave_open,
                     size_t msg_size, const void * data, udp::endpoint const & endpoint):
        sc_async_callback(msg_size, data, endpoint),
        index_(index), filename_(copy_string(filename)), header_format_(copy_string(header_format)),
        sample_format_(copy_string(sample_format)), leave_open_(leave_open),
        start_(start), frames_(frames)
    {}

    ~b_write_callback(void)
    {
        deallocate(filename_);
        deallocate(header_format_);
        deallocate(sample_format_);
    }

    void run(void)
    {
        /** \todo for now we ignore the leave_open flag  */
        instance->write_buffer(index_, filename_, header_format_, sample_format_, start_, frames_);
        schedule_async_message();
        send_done();
    }

    const int index_;
    char * const filename_;
    char * const header_format_;
    char * const sample_format_;
    const bool leave_open_;
    const size_t start_;
    const size_t frames_;
};


void fire_b_write_exception(void)
{
    throw std::runtime_error("wrong arguments for /b_allocReadChannel");
}

void handle_b_write(received_message const & msg, udp::endpoint const & endpoint)
{
    osc::ReceivedMessageArgumentIterator arg = msg.ArgumentsBegin();
    osc::ReceivedMessageArgumentIterator end = msg.ArgumentsEnd();

    /* required args */
    osc::int32 index = arg->AsInt32(); arg++;
    const char * filename = arg->AsString(); arg++;
    const char * header_format = arg->AsString(); arg++;
    const char * sample_format = arg->AsString(); arg++;

    /* optional args */
    osc::int32 frames = -1;
    osc::int32 start = 0;
    osc::int32 leave_open = 0;

    size_t length = 0;
    const void * data = 0;

    if (arg != end) {
        if (!arg->IsInt32())
            fire_b_write_exception();
        frames = arg->AsInt32Unchecked(); arg++;
    }
    else
        goto fire_callback;

    if (arg != end) {
        if (!arg->IsInt32())
            fire_b_write_exception();
        start = arg->AsInt32Unchecked(); arg++;
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

    if (arg != end) {
        if (!arg->IsBlob())
            fire_b_write_exception();
        arg->AsBlobUnchecked(data, length);
    }

fire_callback:
    instance->add_system_callback(
        new b_write_callback(index, filename, header_format, sample_format, frames, start,
                             leave_open, length, data, endpoint));
}

struct b_zero_callback:
    public sc_async_callback
{
    b_zero_callback(int index, size_t msg_size, const void * data,
                    udp::endpoint const & endpoint):
        sc_async_callback(msg_size, data, endpoint),
        index_(index)
    {}

    void run(void)
    {
        instance->zero_buffer(index_);
        schedule_async_message();
        send_done();
    }

    const int index_;
};

void handle_b_zero(received_message const & msg, udp::endpoint const & endpoint)
{
    osc::ReceivedMessageArgumentStream args = msg.ArgumentStream();

    osc::int32 index;
    osc::Blob blob(0, 0);

    args >> index;

    if (!args.Eos())
        args >> blob;

    instance->add_system_callback(new b_zero_callback(index, blob.size, blob.data, endpoint));
}

void handle_b_set(received_message const & msg)
{
    osc::ReceivedMessageArgumentStream args = msg.ArgumentStream();
    osc::int32 buffer_index;
    args >> buffer_index;

    buffer_wrapper::sample_t * data = instance->get_buffer(buffer_index).data;

    while (!args.Eos()) {
        osc::int32 index;
        float value;

        args >> index >> value;
        data[index] = value;
    }
}

void handle_b_setn(received_message const & msg)
{
    osc::ReceivedMessageArgumentStream args = msg.ArgumentStream();
    osc::int32 buffer_index;
    args >> buffer_index;

    buffer_wrapper::sample_t * data = instance->get_buffer(buffer_index).data;

    while (!args.Eos()) {
        osc::int32 index;
        osc::int32 samples;
        args >> index >> samples;

        for (int i = 0; i != samples; ++i) {
            float value;
            args >> value;
            data[index] = value;
        }
    }
}

void handle_b_fill(received_message const & msg)
{
    osc::ReceivedMessageArgumentStream args = msg.ArgumentStream();
    osc::int32 buffer_index;
    args >> buffer_index;

    buffer_wrapper::sample_t * data = instance->get_buffer(buffer_index).data;

    while (!args.Eos()) {
        osc::int32 index;
        osc::int32 samples;
        float value;
        args >> index >> samples >> value;

        for (int i = 0; i != samples; ++i)
            data[index] = value;
    }
}

struct b_query_callback:
    public sc_response_callback
{
    static const size_t elem_size = 3*sizeof(int) * sizeof(float);

    b_query_callback(received_message const & msg,
                     udp::endpoint const & endpoint):
        sc_response_callback(endpoint)
    {
        osc::ReceivedMessageArgumentStream args = msg.ArgumentStream();
        size_t arg_count = msg.ArgumentCount();

        size_t size = elem_size * arg_count + 128; /* should be more than required */
        data_ = (char*)allocate(size);

        osc::OutboundPacketStream p(data_, size);
        p << osc::BeginMessage("/b_info");

        while (!args.Eos()) {
            osc::int32 buffer_index;
            args >> buffer_index;

            buffer_wrapper & buffer = instance->get_buffer(buffer_index);

            p << buffer_index
              << int(buffer.frames_)
              << int(buffer.channels_)
              << float (buffer.sample_rate_);
        }

        p << osc::EndMessage;
        msg_size_ = p.Size();
    }

    ~b_query_callback(void)
    {
        deallocate(data_);
    }

    void run(void)
    {
        instance->send_udp(data_, msg_size_, endpoint_);
    }

    char * data_;
    size_t msg_size_;
};

void handle_b_query(received_message const & msg, udp::endpoint const & endpoint)
{
    instance->add_system_callback(new b_query_callback(msg, endpoint));
}

void handle_c_set(received_message const & msg)
{
    osc::ReceivedMessageArgumentStream args = msg.ArgumentStream();

    while (!args.Eos()) {
        osc::int32 bus_index;
        float value;
        args >> bus_index >> value;

        instance->set_control_bus(bus_index, value);
    }
}

void handle_c_setn(received_message const & msg)
{
    osc::ReceivedMessageArgumentStream args = msg.ArgumentStream();

    while (!args.Eos()) {
        osc::int32 bus_index, bus_count;
        args >> bus_index >> bus_count;

        for (int i = 0; i != bus_count; ++i) {
            float value;
            args >> value;
            instance->set_control_bus(bus_index + i, value);
        }
    }
}

void handle_c_fill(received_message const & msg)
{
    osc::ReceivedMessageArgumentStream args = msg.ArgumentStream();

    while (!args.Eos()) {
        osc::int32 bus_index, bus_count;
        float value;
        args >> bus_index >> bus_count >> value;

        for (int i = 0; i != bus_count; ++i)
            instance->set_control_bus(bus_index + i, value);
    }
}

} /* namespace */

void sc_osc_handler::handle_message_int_address(received_message const & message,
                                                udp::endpoint const & endpoint)
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

    case cmd_g_freeAll:
        handle_g_freeall(message);
        break;

    case cmd_g_deepFree:
        handle_g_deepFree(message);
        break;

    case cmd_n_free:
        handle_n_free(message);
        break;

    case cmd_b_alloc:
        handle_b_alloc(message, endpoint);
        break;

    case cmd_b_free:
        handle_b_free(message, endpoint);
        break;

    case cmd_b_allocRead:
        handle_b_allocRead(message, endpoint);
        break;

    case cmd_b_allocReadChannel:
        handle_b_allocReadChannel(message, endpoint);
        break;

    case cmd_b_write:
        handle_b_write(message, endpoint);
        break;

    case cmd_b_zero:
        handle_b_zero(message, endpoint);
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
        handle_b_query(message, endpoint);
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

    default:
        handle_unhandled_message(message);
    }
}

namespace
{
void dispatch_group_commands(received_message const & message,
                             udp::endpoint const & endpoint)
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
    if (strcmp(address+3, "freeAll") == 0) {
        handle_g_freeall(message);
        return;
    }
    if (strcmp(address+3, "deepFree") == 0) {
        handle_g_deepFree(message);
        return;
    }
}

void dispatch_node_commands(received_message const & message,
                            udp::endpoint const & endpoint)
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

void dispatch_buffer_commands(received_message const & message,
                              udp::endpoint const & endpoint)
{
    const char * address = message.AddressPattern();
    assert(address[0] == '/');
    assert(address[1] == 'b');
    assert(address[2] == '_');

    if (strcmp(address+3, "alloc") == 0) {
        handle_b_alloc(message, endpoint);
        return;
    }

    if (strcmp(address+3, "free") == 0) {
        handle_b_free(message, endpoint);
        return;
    }

    if (strcmp(address+3, "allocRead") == 0) {
        handle_b_allocRead(message, endpoint);
        return;
    }
    if (strcmp(address+3, "allocReadChannel") == 0) {
        handle_b_allocReadChannel(message, endpoint);
        return;
    }

    if (strcmp(address+3, "write") == 0) {
        handle_b_write(message, endpoint);
        return;
    }

    if (strcmp(address+3, "zero") == 0) {
        handle_b_zero(message, endpoint);
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
        handle_b_query(message, endpoint);
        return;
    }
}

void dispatch_control_bus_commands(received_message const & message,
                                   udp::endpoint const & endpoint)
{
    const char * address = message.AddressPattern();
    assert(address[0] == '/');
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
}


} /* namespace */

void sc_osc_handler::handle_message_sym_address(received_message const & message,
                                                udp::endpoint const & endpoint)
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

    if (address[1] == 'b') {
        dispatch_buffer_commands(message, endpoint);
        return;
    }

    if (address[1] == 'c') {
        dispatch_control_bus_commands(message, endpoint);
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

