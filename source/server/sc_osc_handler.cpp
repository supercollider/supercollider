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

    cmd_p_new = 63,

    NUMBER_OF_COMMANDS = 64
};

int first_arg_as_int(received_message const & message)
{
    osc::ReceivedMessageArgumentStream args = message.ArgumentStream();
    osc::int32 val;

    args >> val;

    return val;
}

bool check_node_id(int node_id)
{
    if (!instance->node_id_available(node_id)) {
        cerr << "node id " << node_id << " already in use" << endl;
        return false;
    }
    return true;
}

template <typename Functor>
struct fn_system_callback:
    public system_callback
{
    fn_system_callback (Functor const & fn):
        fn_(fn)
    {}

    void run(void)
    {
        fn_();
    }

    Functor fn_;
};

template <typename Functor>
void fire_system_callback(Functor const & f)
{
    instance->add_system_callback(new fn_system_callback<Functor>(f));
}


template <typename Functor>
struct fn_sync_callback:
    public audio_sync_callback
{
    fn_sync_callback (Functor const & fn):
        fn_(fn)
    {}

    void run(void)
    {
        fn_();
    }

    Functor fn_;
};

template <typename Functor>
void fire_rt_callback(Functor const & f)
{
    instance->add_sync_callback(new fn_sync_callback<Functor>(f));
}

void send_done_message(udp::endpoint const & endpoint)
{
    char buffer[128];
    osc::OutboundPacketStream p(buffer, 128);
    p << osc::BeginMessage("/done")
    << osc::EndMessage;

    instance->send_udp(p.Data(), p.Size(), endpoint);
}

/** send done message from the system callback, only to be called from the rt thread */
void fire_done_message(udp::endpoint const & endpoint)
{
    fire_system_callback(boost::bind(send_done_message, endpoint));
}


struct sc_response_callback:
    public system_callback
{
    sc_response_callback(udp::endpoint const & endpoint):
        endpoint_(endpoint)
    {}

    inline void send_done(void)
    {
        send_done_message(endpoint_);
    }

    const udp::endpoint endpoint_;
};

void quit_perform(udp::endpoint const & endpoint)
{
    send_done_message(endpoint);
    instance->terminate();
}

void handle_quit(udp::endpoint const & endpoint)
{
    fire_system_callback(boost::bind(quit_perform, endpoint));
}

void notify_perform(bool enable, udp::endpoint const & endpoint)
{
    if (enable)
        instance->add_observer(endpoint);
    else
        instance->remove_observer(endpoint);
    send_done_message(endpoint);
}

void handle_notify(received_message const & message, udp::endpoint const & endpoint)
{
    int enable = first_arg_as_int(message);
    fire_system_callback(boost::bind(notify_perform, bool(enable), endpoint));
}

void status_perform(udp::endpoint const & endpoint)
{
    char buffer[1024];
    typedef osc::int32 i32;
    osc::OutboundPacketStream p(buffer, 1024);
    p << osc::BeginMessage("status.reply")
      << (i32)1                                    /* unused */
      << (i32)ugen_factory.ugen_count()   /* ugens */
      << (i32)instance->synth_count()     /* synths */
      << (i32)instance->group_count()     /* groups */
      << (i32)instance->prototype_count() /* synthdefs */
      << instance->cpu_load()                 /* average cpu % */
      << instance->cpu_load()                 /* peak cpu % */
      << instance->get_samplerate()           /* nominal samplerate */
      << instance->get_samplerate()           /* actual samplerate */
      << osc::EndMessage;

    instance->send_udp(p.Data(), p.Size(), endpoint);
}

void handle_status(udp::endpoint const & endpoint)
{
    fire_system_callback(boost::bind(status_perform, endpoint));
}

void handle_dumpOSC(received_message const & message)
{
    int val = first_arg_as_int(message);
    val = min (1, val);    /* we just support one way of dumping osc messages */

    instance->dumpOSC(val);     /* thread-safe */
}

void sync_perform(osc::int32 id, udp::endpoint const & endpoint)
{
    char buffer[128];
    osc::OutboundPacketStream p(buffer, 128);
    p << osc::BeginMessage("/synced")
      << id
      << osc::EndMessage;

    instance->send_udp(p.Data(), p.Size(), endpoint);
}

void handle_sync(received_message const & message, udp::endpoint const & endpoint)
{
    int id = first_arg_as_int(message);

    fire_system_callback(boost::bind(sync_perform, id, endpoint));
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

server_node * find_node(int target_id)
{
    server_node * node = instance->find_node(target_id);

    if (node == NULL) {
        cerr << "node not found\n" << endl;
        return NULL;
    }
    return node;
}

sc_synth * add_synth(const char * name, int node_id, int action, int target_id)
{
    if (!check_node_id(node_id))
        return 0;

    server_node * target = find_node(target_id);
    if (target == NULL)
        return NULL;

    node_position_constraint pos = make_pair(target, node_position(action));
    abstract_synth * synth = instance->add_synth(name, node_id, pos);

    return static_cast<sc_synth*>(synth);
}

/* set control values of node from string/float or int/float pair */
void set_control(server_node * node, osc::ReceivedMessageArgumentIterator & it)
{
    if (it->IsInt32()) {
        osc::int32 index = it->AsInt32Unchecked(); ++it;
        float value = it->AsFloat(); ++it;

        node->set(index, value);
    }
    else if (it->IsString()) {
        const char * str = it->AsString(); ++it;
        float value = it->AsFloat(); ++it;

        node->set(str, value);
    }
}

void handle_s_new(received_message const & msg)
{
    osc::ReceivedMessageArgumentIterator args = msg.ArgumentsBegin();

    const char * def_name = args->AsString(); ++args;
    int32_t id = args->AsInt32(); ++args;
    int32_t action = args->AsInt32(); ++args;
    int32_t target = args->AsInt32(); ++args;

    sc_synth * synth = add_synth(def_name, id, action, target);

    if (synth == NULL)
        return;

    while(args != msg.ArgumentsEnd())
    {
        try {
            set_control(synth, args);
        }
        catch(std::exception & e)
        {
            cout << "Exception during /s_new handler: " << e.what() << endl;
        }
    }
}


void insert_group(int node_id, int action, int target_id)
{
    if (!check_node_id(node_id))
        return;

    server_node * target = find_node(target_id);

    if (!target)
        return;

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

        server_node * node = find_node(id);

        if (node == NULL)
            continue;

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

        server_node * node = find_node(id);

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

/** \todo handle array arguments */
void handle_n_set(received_message const & msg)
{
    osc::ReceivedMessageArgumentIterator it = msg.ArgumentsBegin();
    osc::int32 id = it->AsInt32(); ++it;

    server_node * node = find_node(id);
    if(!node)
        return;

    while(it != msg.ArgumentsEnd())
    {
        try
        {
            set_control(node, it);
        }
        catch(std::exception & e)
        {
            cout << "Exception during /n_set handler: " << e.what() << endl;
        }
    }
}

/** \todo handle array arguments */
void handle_n_setn(received_message const & msg)
{
    osc::ReceivedMessageArgumentIterator it = msg.ArgumentsBegin();
    osc::int32 id = it->AsInt32(); ++it;

    server_node * node = find_node(id);
    if(!node)
        return;

    while(it != msg.ArgumentsEnd())
    {
        try {
            if (it->IsInt32()) {
                osc::int32 index = it->AsInt32Unchecked(); ++it;
                osc::int32 count = it->AsInt32(); ++it;

                for (int i = 0; i != count; ++i) {
                    float value = it->AsFloat(); ++it;
                    node->set(index + i, value);
                }
            }
            else if (it->IsString()) {
                /** \todo how to resolve arrayed arguments ? */
            }
        }
        catch(std::exception & e)
        {
            cout << "Exception during /n_setn handler: " << e.what() << endl;
        }
    }
}

void handle_n_fill(received_message const & msg)
{
    osc::ReceivedMessageArgumentIterator it = msg.ArgumentsBegin();
    osc::int32 id = it->AsInt32(); ++it;

    server_node * node = find_node(id);
    if(!node)
        return;

    while(it != msg.ArgumentsEnd())
    {
        try {
            if (it->IsInt32()) {
                osc::int32 index = it->AsInt32Unchecked(); ++it;
                osc::int32 count = it->AsInt32(); ++it;
                float value = it->AsFloat(); ++it;

                for (int i = 0; i != count; ++i)
                    node->set(index + i, value);
            }
            else if (it->IsString()) {
                /** \todo how to resolve arrayed arguments ? */
            }
        }
        catch(std::exception & e)
        {
            cout << "Exception during /n_setn handler: " << e.what() << endl;
        }
    }
}

void handle_n_run(received_message const & msg)
{
    osc::ReceivedMessageArgumentStream args = msg.ArgumentStream();

    while(!args.Eos())
    {
        osc::int32 node_id, run_flag;
        args >> node_id >> run_flag;

        server_node * node = find_node(node_id);
        if(!node)
            continue;

        if (run_flag)
            node->resume();
        else
            node->pause();
    }
}

/** wrapper class for osc completion message
 */
struct completion_message
{
    /** constructor should only be used from the real-time thread */
    completion_message(size_t size, const void * data):
        size_(size)
    {
        if (size)
        {
            data_ = system_callback::allocate(size);
            memcpy(data_, data, size);
        }
    }

    /** copy constructor has move semantics!!! */
    completion_message(completion_message const & rhs)
    {
        size_ = rhs.size_;
        data_ = rhs.data_;
        const_cast<completion_message&>(rhs).size_ = 0;
    }

    ~completion_message(void)
    {
        if (size_)
            system_callback::deallocate(data_);
    }

    /** handle package in the rt thread
     *  not to be called from the rt thread
     */
    void trigger_async(udp::endpoint const & endpoint)
    {
        if (size_)
        {
            sc_osc_handler::received_packet * p =
                sc_osc_handler::received_packet::alloc_packet((char*)data_, size_, endpoint);
            instance->add_sync_callback(p);
        }
    }

    /** handle package directly
     *  only to be called from the rt thread
     */
    void handle(udp::endpoint const & endpoint)
    {
        if (size_)
            instance->handle_packet((char*)data_, size_, endpoint);
    }

    size_t size_;
    void * data_;
};

struct movable_string
{
    /** allocate new string, only allowed to be called from the rt thread */
    explicit movable_string(const char * str)
    {
        size_t length = strlen(str);
        char * ret = (char*)system_callback::allocate(length + 1); /* terminating \0 */
        strcpy(ret, str);
    }

    /** copy constructor has move semantics!!! */
    movable_string(movable_string const & rhs)
    {
        data_ = rhs.data_;
        const_cast<movable_string&>(rhs).data_ = NULL;
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
    const char * data_;
};

template <typename T>
struct movable_array
{
    /** allocate new array, only allowed to be called from the rt thread */
    movable_array(size_t length, T * data):
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
        const_cast<movable_array&>(rhs).data_ = NULL;
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

    const size_t length(void) const
    {
        return length_;
    }

private:
    size_t length_;
    T * data_;
};



/** responding callback, which is executing an osc message when done */
struct sc_async_callback:
    public sc_response_callback
{
protected:
    sc_async_callback(size_t msg_size, const void * data, udp::endpoint const & endpoint):
        sc_response_callback(endpoint), msg_(msg_size, data)
    {}

    static char * copy_string(const char * str)
    {
        size_t length = strlen(str);
        char * ret = (char*)system_callback::allocate(length + 1); /* terminating \0 */
        strcpy(ret, str);
        return ret;
    }

    void schedule_async_message(void)
    {
        msg_.trigger_async(endpoint_);
    }

    completion_message msg_;
};


void b_alloc_2_rt(uint32_t index, completion_message & msg, sample * free_buf, udp::endpoint const & endpoint);
void b_alloc_3_nrt(sample * free_buf, udp::endpoint const & endpoint);

void b_alloc_1_nrt(uint32_t index, uint32_t frames, uint32_t channels, completion_message & msg, udp::endpoint const & endpoint)
{
    sample * free_buf = ugen_factory.get_nrt_mirror_buffer(index);
    ugen_factory.allocate_buffer(index, frames, channels);
    fire_rt_callback(boost::bind(b_alloc_2_rt, index, msg, free_buf, endpoint));
}

void b_alloc_2_rt(uint32_t index, completion_message & msg, sample * free_buf, udp::endpoint const & endpoint)
{
    ugen_factory.buffer_sync(index);
    msg.handle(endpoint);
    fire_system_callback(boost::bind(b_alloc_3_nrt, free_buf, endpoint));
}

void b_alloc_3_nrt(sample * free_buf, udp::endpoint const & endpoint)
{
    free_aligned(free_buf);
    send_done_message(endpoint);
}

void handle_b_alloc(received_message const & msg, udp::endpoint const & endpoint)
{
    osc::ReceivedMessageArgumentStream args = msg.ArgumentStream();

    osc::int32 index, frames, channels;
    osc::Blob blob(0, 0);

    args >> index >> frames >> channels;

    if (!args.Eos())
        args >> blob;

    completion_message message(blob.size, blob.data);
    fire_system_callback(boost::bind(b_alloc_1_nrt, index, frames, channels, message, endpoint));
}

void b_free_1_nrt(uint32_t index, completion_message & msg, udp::endpoint const & endpoint);
void b_free_2_rt(uint32_t index, sample * free_buf, completion_message & msg, udp::endpoint const & endpoint);
void b_free_3_nrt(sample * free_buf, udp::endpoint const & endpoint);

void b_free_1_nrt(uint32_t index, completion_message & msg, udp::endpoint const & endpoint)
{
    sample * free_buf = ugen_factory.get_nrt_mirror_buffer(index);
    ugen_factory.free_buffer(index);
    fire_rt_callback(boost::bind(b_free_2_rt, index, free_buf, msg, endpoint));
}

void b_free_2_rt(uint32_t index, sample * free_buf, completion_message & msg, udp::endpoint const & endpoint)
{
    ugen_factory.buffer_sync(index);
    fire_system_callback(boost::bind(b_free_3_nrt, free_buf, endpoint));
    msg.handle(endpoint);
}

void b_free_3_nrt(sample * free_buf, udp::endpoint const & endpoint)
{
    free_aligned(free_buf);
    send_done_message(endpoint);
}


void handle_b_free(received_message const & msg, udp::endpoint const & endpoint)
{
    osc::ReceivedMessageArgumentStream args = msg.ArgumentStream();

    osc::int32 index;
    osc::Blob blob(0, 0);

    args >> index;

    if (!args.Eos())
        args >> blob;

    completion_message message(blob.size, blob.data);
    fire_system_callback(boost::bind(b_free_1_nrt, index, message, endpoint));
}

void b_allocRead_2_rt(uint32_t index, completion_message & msg, sample * free_buf, udp::endpoint const & endpoint);
void b_allocRead_3_nrt(sample * free_buf, udp::endpoint const & endpoint);

void b_allocRead_1_nrt(uint32_t index, movable_string & filename, uint32_t start, uint32_t frames, completion_message & msg,
                       udp::endpoint const & endpoint)
{
    sample * free_buf = ugen_factory.get_nrt_mirror_buffer(index);
    int error = ugen_factory.buffer_read_alloc(index, filename.c_str(), start, frames);
    if (!error)
        fire_rt_callback(boost::bind(b_allocRead_2_rt, index, msg, free_buf, endpoint));
    else
        /* post nice error message */;
}

void b_allocRead_2_rt(uint32_t index, completion_message & msg, sample * free_buf,
                      udp::endpoint const & endpoint)
{
    ugen_factory.buffer_sync(index);
    msg.handle(endpoint);
    fire_system_callback(boost::bind(b_allocRead_3_nrt, free_buf, endpoint));
}

void b_allocRead_3_nrt(sample * free_buf, udp::endpoint const & endpoint)
{
    free_aligned(free_buf);
    send_done_message(endpoint);
}

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

    completion_message message(blob.size, blob.data);
    movable_string fname(filename);
    fire_system_callback(boost::bind(b_allocRead_1_nrt, index, fname, start, frames, message, endpoint));
}

void b_allocReadChannel_2_rt(uint32_t index, completion_message & msg, sample * free_buf,
                             udp::endpoint const & endpoint);
void b_allocReadChannel_3_nrt(sample * free_buf, udp::endpoint const & endpoint);

void b_allocReadChannel_1_nrt(uint32_t index, movable_string & filename, uint32_t start, uint32_t frames,
                              movable_array<uint32_t> const & channels, completion_message & msg,
                              udp::endpoint const & endpoint)
{
    sample * free_buf = ugen_factory.get_nrt_mirror_buffer(index);
    int error = ugen_factory.buffer_alloc_read_channels(index, filename.c_str(), start, frames,
                                                        channels.length(), channels.data());
    if (!error)
        fire_rt_callback(boost::bind(b_allocReadChannel_2_rt, index, msg, free_buf, endpoint));
}

void b_allocReadChannel_2_rt(uint32_t index, completion_message & msg, sample * free_buf,
                             udp::endpoint const & endpoint)
{
    ugen_factory.buffer_sync(index);
    msg.handle(endpoint);
    fire_system_callback(boost::bind(b_allocReadChannel_3_nrt, free_buf, endpoint));
}

void b_allocReadChannel_3_nrt(sample * free_buf, udp::endpoint const & endpoint)
{
    free_aligned(free_buf);
    send_done_message(endpoint);
}


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
    unsigned long length = 0;
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

    movable_array<uint32_t> channel_mapping(channel_count, channels.c_array());
    completion_message message(length, data);
    movable_string fname(filename);

    fire_system_callback(boost::bind(b_allocReadChannel_1_nrt, index, fname, start, frames, channel_mapping, message, endpoint));
}

void b_write_nrt_1(uint32_t index, movable_string const & filename, movable_string const & header_format,
                   movable_string const & sample_format, uint32_t start, uint32_t frames, bool leave_open,
                   completion_message & msg, udp::endpoint const & endpoint)
{
    ugen_factory.buffer_write(index, filename.c_str(), header_format.c_str(), sample_format.c_str(), start, frames, leave_open);
    msg.trigger_async(endpoint);
    fire_done_message(endpoint);
}

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

    unsigned long length = 0;
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
    completion_message message(length, data);
    movable_string fname(filename);
    movable_string header_f(header_format);
    movable_string sample_f(sample_format);

    fire_system_callback(boost::bind(b_write_nrt_1, index, fname, header_f, sample_f,
                                     start, frames, bool(leave_open), message, endpoint));
}



void b_read_rt_2(uint32_t index, completion_message & msg, udp::endpoint const & endpoint);

void b_read_nrt_1(uint32_t index, movable_string & filename, uint32_t start_file, uint32_t frames,
                  uint32_t start_buffer, bool leave_open, completion_message & msg, udp::endpoint const & endpoint)
{
    ugen_factory.buffer_read(index, filename.c_str(), start_file, frames, start_buffer, leave_open);
    fire_rt_callback(boost::bind(b_read_rt_2, index, msg, endpoint));
}

void b_read_rt_2(uint32_t index, completion_message & msg, udp::endpoint const & endpoint)
{
    ugen_factory.buffer_sync(index);
    msg.handle(endpoint);
    fire_done_message(endpoint);
}

void fire_b_read_exception(void)
{
    throw std::runtime_error("wrong arguments for /b_read");
}

void handle_b_read(received_message const & msg, udp::endpoint const & endpoint)
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

    unsigned long length = 0;
    const void * data = 0;

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
            fire_b_read_exception();
        start_buffer = arg->AsInt32Unchecked(); arg++;
    }
    else
        goto fire_callback;

    if (arg != end) {
        if (!arg->IsInt32())
            fire_b_read_exception();
        leave_open = arg->AsInt32Unchecked(); arg++;
    }
    else
        goto fire_callback;

    if (arg != end) {
        if (!arg->IsBlob())
            fire_b_read_exception();
        arg->AsBlobUnchecked(data, length);
    }

fire_callback:
    completion_message message(length, data);
    movable_string fname(filename);

    fire_system_callback(boost::bind(b_read_nrt_1, index, fname, start_file, frames, start_buffer,
                                     bool(leave_open), message, endpoint));
}


void b_readChannel_rt_2(uint32_t index, completion_message & msg, udp::endpoint const & endpoint);

void b_readChannel_nrt_1(uint32_t index, movable_string & filename, uint32_t start_file, uint32_t frames,
                         uint32_t start_buffer, bool leave_open, movable_array<uint32_t> & channel_map,
                         completion_message & msg, udp::endpoint const & endpoint)
{
    ugen_factory.buffer_read_channel(index, filename.c_str(), start_file, frames, start_buffer, leave_open,
                                     channel_map.length(), channel_map.data());
    fire_rt_callback(boost::bind(b_readChannel_rt_2, index, msg, endpoint));
}

void b_readChannel_rt_2(uint32_t index, completion_message & msg, udp::endpoint const & endpoint)
{
    ugen_factory.buffer_sync(index);
    msg.handle(endpoint);
    fire_done_message(endpoint);
}

void fire_b_readChannel_exception(void)
{
    throw std::runtime_error("wrong arguments for /b_readChannel");
}

void handle_b_readChannel(received_message const & msg, udp::endpoint const & endpoint)
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

    unsigned long length = 0;
    const void * data = 0;

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
            arg->AsBlobUnchecked(data, length);
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
    completion_message message(length, data);
    movable_string fname(filename);
    movable_array<uint32_t> channel_map(channel_count, channel_mapping.c_array());

    fire_system_callback(boost::bind(b_readChannel_nrt_1, index, fname, start_file, frames, start_buffer,
                                     bool(leave_open), channel_map, message, endpoint));
}


void b_zero_rt_2(uint32_t index, completion_message & msg, udp::endpoint const & endpoint);

void b_zero_nrt_1(uint32_t index, completion_message & msg, udp::endpoint const & endpoint)
{
    ugen_factory.buffer_zero(index);
    fire_rt_callback(boost::bind(b_zero_rt_2, index, msg, endpoint));
}

void b_zero_rt_2(uint32_t index, completion_message & msg, udp::endpoint const & endpoint)
{
    ugen_factory.increment_write_updates(index);
    msg.handle(endpoint);
    fire_done_message(endpoint);
}

void handle_b_zero(received_message const & msg, udp::endpoint const & endpoint)
{
    osc::ReceivedMessageArgumentStream args = msg.ArgumentStream();

    osc::int32 index;
    osc::Blob blob(0, 0);

    args >> index;

    if (!args.Eos())
        args >> blob;

    completion_message message(blob.size, blob.data);

    fire_system_callback(boost::bind(b_zero_nrt_1, index, message, endpoint));
}

void handle_b_set(received_message const & msg)
{
    osc::ReceivedMessageArgumentStream args = msg.ArgumentStream();
    osc::int32 buffer_index;
    args >> buffer_index;

    buffer_wrapper::sample_t * data = ugen_factory.get_buffer(buffer_index);

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

    buffer_wrapper::sample_t * data = ugen_factory.get_buffer(buffer_index);

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

    buffer_wrapper::sample_t * data = ugen_factory.get_buffer(buffer_index);

    while (!args.Eos()) {
        osc::int32 index;
        osc::int32 samples;
        float value;
        args >> index >> samples >> value;

        for (int i = 0; i != samples; ++i)
            data[index] = value;
    }
}

void b_query_nrt(movable_array<char> data, udp::endpoint const & endpoint)
{
    instance->send_udp(data.data(), data.length(), endpoint);
}

void handle_b_query(received_message const & msg, udp::endpoint const & endpoint)
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

        SndBuf * buf = ugen_factory.get_buffer_struct(buffer_index);

        p << buffer_index
          << osc::int32(buf->frames)
          << osc::int32(buf->channels)
          << float (buf->samplerate);
    }

    p << osc::EndMessage;

    movable_array<char> message(p.Size(), data.c_array());

    fire_system_callback(boost::bind(b_query_nrt, message, endpoint));
}

void b_close_nrt_1(uint32_t index)
{
    ugen_factory.buffer_close(index);
}

void handle_b_close(received_message const & msg, udp::endpoint const & endpoint)
{
    osc::ReceivedMessageArgumentStream args = msg.ArgumentStream();
    osc::int32 index;

    args >> index;

    fire_system_callback(boost::bind(b_close_nrt_1, index));
}

void b_get_nrt(movable_array<char> const & data, udp::endpoint const & endpoint)
{
    instance->send_udp(data.data(), data.length(), endpoint);
}

void handle_b_get(received_message const & msg, udp::endpoint const & endpoint)
{
    const size_t elem_size = sizeof(int) * sizeof(float);
    osc::ReceivedMessageArgumentStream args = msg.ArgumentStream();
    const size_t index_count = msg.ArgumentCount() - 1;
    const size_t alloc_size = index_count * elem_size + 128; /* hopefully enough */

    sized_array<char, rt_pool_allocator<char> > return_message(alloc_size);

    osc::int32 buffer_index;
    args >> buffer_index;

    const SndBuf * buf = ugen_factory.get_buffer_struct(buffer_index);
    const sample * data = buf->data;
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
    fire_system_callback(boost::bind(b_get_nrt, message, endpoint));
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

void b_getn_nrt(movable_array<char> const & data, udp::endpoint const & endpoint)
{
    instance->send_udp(data.data(), data.length(), endpoint);
}

void handle_b_getn(received_message const & msg, udp::endpoint const & endpoint)
{
    osc::ReceivedMessageArgumentStream args = msg.ArgumentStream();

    typedef getn_data<rt_pool_allocator<float> > getn_data;
    std::vector<getn_data, rt_pool_allocator<getn_data> > return_data;

    osc::int32 buffer_index;
    args >> buffer_index;

    const SndBuf * buf = ugen_factory.get_buffer_struct(buffer_index);
    const sample * data = buf->data;
    const int max_sample = buf->frames * buf->channels;


    while (!args.Eos())
    {
        osc::int32 index, sample_count;
        args >> index >> sample_count;

        if (index + sample_count < max_sample)
            return_data.push_back(getn_data(index, sample_count, data + index));
    }

    size_t alloc_size = 128;
    for (size_t i = 0; i != return_data.size(); ++i)
        alloc_size += return_data[i].data_.size() * sizeof(float) + 2*sizeof(int);

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
    fire_system_callback(boost::bind(b_getn_nrt, message, endpoint));
}

void handle_c_set(received_message const & msg)
{
    osc::ReceivedMessageArgumentStream args = msg.ArgumentStream();

    while (!args.Eos()) {
        osc::int32 bus_index;
        float value;
        args >> bus_index >> value;

        ugen_factory.controlbus_set(bus_index, value);
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
            ugen_factory.controlbus_set(bus_index + i, value);
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

        ugen_factory.controlbus_fill(bus_index, bus_count, value);
    }
}

struct c_get_callback:
    public sc_response_callback
{
    static const int reply_size = sizeof(int) + sizeof(float);

public:
    c_get_callback(received_message const & msg, udp::endpoint const & endpoint):
        sc_response_callback(endpoint)
    {
        const size_t elements = msg.ArgumentCount() / 2;
        const size_t size = elements * reply_size + 128; /* more than required */
        data_ = (char*)allocate(size);

        osc::OutboundPacketStream p(data_, size);

        osc::ReceivedMessageArgumentStream args = msg.ArgumentStream();
        p << osc::BeginMessage("/c_set");
        while (!args.Eos()) {
            osc::int32 bus_index;
            args >> bus_index;

            float value = ugen_factory.controlbus_get(bus_index);

            p << bus_index
              << value;
        }

        p << osc::EndMessage;
        msg_size_ = p.Size();
    }

private:
    void run(void)
    {
        instance->send_udp(data_, msg_size_, endpoint_);
    }

    ~c_get_callback(void)
    {
        deallocate(data_);
    }

    char * data_;
    size_t msg_size_;
};

void c_get_nrt(movable_array<char> const & data, udp::endpoint const & endpoint)
{
    instance->send_udp(data.data(), data.length(), endpoint);
}

void handle_c_get(received_message const & msg,
                  udp::endpoint const & endpoint)
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

        p << index << ugen_factory.controlbus_get(index);
    }

    p << osc::EndMessage;

    movable_array<char> message(p.Size(), return_message.c_array());
    fire_system_callback(boost::bind(b_get_nrt, message, endpoint));
}

void c_getn_nrt(movable_array<char> const & data, udp::endpoint const & endpoint)
{
    instance->send_udp(data.data(), data.length(), endpoint);
}

void handle_c_getn(received_message const & msg, udp::endpoint const & endpoint)
{
    osc::ReceivedMessageArgumentStream args = msg.ArgumentStream();

    size_t alloc_size = 128;

    while (!args.Eos())
    {
        osc::int32 index, bus_count;
        args >> index >> bus_count;

        alloc_size += 2 * sizeof(int) + bus_count * sizeof(float);
    }

    sized_array<char, rt_pool_allocator<char> > return_message(alloc_size);

    args = msg.ArgumentStream();

    osc::OutboundPacketStream p(return_message.c_array(), alloc_size);
    p << osc::BeginMessage("/c_setn");

    while (!args.Eos())
    {
        osc::int32 bus_index, bus_count;
        args >> bus_index >> bus_count;
        p << bus_index << bus_count;

        for (int i = 0; i != bus_count; ++i) {
            float value = ugen_factory.controlbus_get(bus_index + i);
            p << value;
        }
    }

    p << osc::EndMessage;

    movable_array<char> message(p.Size(), return_message.c_array());
    fire_system_callback(boost::bind(c_getn_nrt, message, endpoint));
}

struct d_recv_callback:
    public sc_async_callback
{
    d_recv_callback(size_t def_size, const void * def, size_t msg_size, const void * msg,
                    udp::endpoint const & endpoint):
        sc_async_callback(msg_size, msg, endpoint), def_size_(def_size_)
    {
        def_ = (char*)allocate(def_size + 1);
        memcpy(def_, def, def_size);
        def_[def_size] = 0;
    }

    ~d_recv_callback(void)
    {
        deallocate(def_);
    }

    void run(void)
    {
        std::string def_string(def_);
        std::stringstream stream(def_string);
        sc_synthdef synthdef(stream);
        instance->register_prototype(new sc_synth_prototype(synthdef));
        schedule_async_message();
        send_done();
    }

    char * def_;
    const size_t def_size_;
};

void handle_d_recv(received_message const & msg,
                   udp::endpoint const & endpoint)
{
    osc::Blob synthdef(0, 0), blob(0, 0);
    osc::ReceivedMessageArgumentStream args = msg.ArgumentStream();

    args >> synthdef >> blob;
    instance->add_system_callback(new d_recv_callback(synthdef.size, synthdef.data, blob.size, blob.data, endpoint));
}

struct d_load_callback:
    public sc_async_callback
{
    d_load_callback(const char * path, size_t msg_size, const void * msg,
                    udp::endpoint const & endpoint):
        sc_async_callback(msg_size, msg, endpoint),
        path_(copy_string(path))
    {}

    ~d_load_callback(void)
    {
        deallocate((char*)path_);
    }

    void run(void)
    {
        sc_read_synthdef(*instance, path_); /* todo: we need to implment some file name pattern matching */
        schedule_async_message();
        send_done();
    }

    const char * const path_;
};

void handle_d_load(received_message const & msg,
                   udp::endpoint const & endpoint)
{
    osc::ReceivedMessageArgumentStream args = msg.ArgumentStream();
    const char * path;
    osc::Blob blob(0, 0);

    args >> path;
    if (!args.Eos())
        args >> blob;

    instance->add_system_callback(new d_load_callback(path, blob.size, blob.data, endpoint));
}


struct d_loadDir_callback:
    public d_load_callback
{
    d_loadDir_callback(const char * path, size_t msg_size, const void * msg,
                       udp::endpoint const & endpoint):
        d_load_callback(path, msg_size, msg, endpoint)
    {}

    void run(void)
    {
        sc_read_synthdefs_dir(*instance, path_);
        schedule_async_message();
        send_done();
    }
};

void handle_d_loadDir(received_message const & msg,
                      udp::endpoint const & endpoint)
{
    osc::ReceivedMessageArgumentStream args = msg.ArgumentStream();
    const char * path;
    osc::Blob blob(0, 0);

    args >> path;
    if (!args.Eos())
        args >> blob;

    instance->add_system_callback(new d_loadDir_callback(path, blob.size, blob.data, endpoint));
}


void handle_d_free(received_message const & msg)
{
    osc::ReceivedMessageArgumentStream args = msg.ArgumentStream();

    while(!args.Eos())
    {
        const char * defname;
        args >> defname;

        instance->remove_prototype(defname);
    }
}

void insert_parallel_group(int node_id, int action, int target_id)
{
    if (!check_node_id(node_id))
        return;

    server_node * target = find_node(target_id);
    if(!target)
        return;

    node_position_constraint pos = make_pair(target, node_position(action));

    instance->add_parallel_group(node_id, pos);
}

void handle_p_new(received_message const & msg)
{
    osc::ReceivedMessageArgumentStream args = msg.ArgumentStream();

    while(!args.Eos())
    {
        osc::int32 id, action, target;
        args >> id >> action >> target;

        insert_parallel_group(id, action, target);
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

    case cmd_s_new:
        handle_s_new(message);
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

    case cmd_n_set:
        handle_n_set(message);
        break;

    case cmd_n_setn:
        handle_n_setn(message);
        break;

    case cmd_n_fill:
        handle_n_fill(message);
        break;

    case cmd_n_run:
        handle_n_run(message);
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

    case cmd_b_read:
        handle_b_read(message, endpoint);
        break;

    case cmd_b_readChannel:
        handle_b_readChannel(message, endpoint);
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

    case cmd_b_get:
        handle_b_get(message, endpoint);
        break;

    case cmd_b_getn:
        handle_b_getn(message, endpoint);
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
        handle_c_get(message, endpoint);
        break;

    case cmd_c_getn:
        handle_c_getn(message, endpoint);
        break;

    case cmd_d_recv:
        handle_d_recv(message, endpoint);
        break;

    case cmd_d_load:
        handle_d_load(message, endpoint);
        break;

    case cmd_d_loadDir:
        handle_d_loadDir(message, endpoint);
        break;

    case cmd_d_free:
        handle_d_free(message);
        break;

    case cmd_p_new:
        handle_p_new(message);
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

    if (strcmp(address+3, "run") == 0) {
        handle_n_run(message);
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

    if (strcmp(address+3, "read") == 0) {
        handle_b_read(message, endpoint);
        return;
    }

    if (strcmp(address+3, "readChannel") == 0) {
        handle_b_readChannel(message, endpoint);
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

    if (strcmp(address+3, "get") == 0) {
        handle_b_get(message, endpoint);
        return;
    }

    if (strcmp(address+3, "getn") == 0) {
        handle_b_getn(message, endpoint);
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

    if (strcmp(address+3, "get") == 0) {
        handle_c_get(message, endpoint);
        return;
    }

    if (strcmp(address+3, "getn") == 0) {
        handle_c_getn(message, endpoint);
        return;
    }
}

void dispatch_synthdef_commands(received_message const & message,
                                udp::endpoint const & endpoint)
{
    const char * address = message.AddressPattern();
    assert(address[0] == '/');
    assert(address[1] == 'd');
    assert(address[2] == '_');

    if (strcmp(address+3, "recv") == 0) {
        handle_d_recv(message, endpoint);
        return;
    }

    if (strcmp(address+3, "load") == 0) {
        handle_d_load(message, endpoint);
        return;
    }

    if (strcmp(address+3, "loadDir") == 0) {
        handle_d_loadDir(message, endpoint);
        return;
    }

    if (strcmp(address+3, "free") == 0) {
        handle_d_free(message);
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

    if (address[2] == '_')
    {
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

        if (address[1] == 'd') {
            dispatch_synthdef_commands(message, endpoint);
            return;
        }
    }

    if (strcmp(address+1, "s_new") == 0) {
        handle_s_new(message);
        return;
    }

    if (strcmp(address+1, "p_new") == 0) {
        handle_p_new(message);
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
