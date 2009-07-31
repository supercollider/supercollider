#include "utilities/ringbuffer.hpp"
#include <boost/thread.hpp>
#include <cassert>
#include <iostream>
#include <set>

#include <boost/guid.hpp>

using namespace PNPD;
using namespace boost;
using namespace std;

#ifndef CONST_SIZE
lf_ringbuffer<guid> rb(32);
#else
lf_ringbuffer_fixed<guid, 32> rb;
#endif
set<guid> working_set;
mutex guard;

void add1(void)
{
    uint max = 10000000;
    for (uint i = 0; i != max; ++i)
    {
        guid id = guid::create();
        {
            mutex::scoped_lock lock(guard);
            working_set.insert(id);
        }
        while (not rb.write_ringbuffer(id))
            thread::yield();
    }
}

volatile bool running = true;

void get1(void)
{
    for(;;)
    {
        guid id;

        bool success = rb.read_ringbuffer(&id);
        if (not running and not success)
            return;
        if (success)
        {
            mutex::scoped_lock lock(guard);
            assert(working_set.find(id) != working_set.end());
            working_set.erase(id);
        }
        else
            thread::yield();
    }
}


void add2(void)
{
    uint max = 10000000;
    for (uint i = 0; i != max; ++i)
    {
        guid id[2];
        id[0] = guid::create();
        id[1] = guid::create();
        {
            mutex::scoped_lock lock(guard);
            working_set.insert(id[0]);
            working_set.insert(id[1]);
        }

        uint remaining = 2;
        for (;;)
        {
            uint written = rb.write_ringbuffer(id, 2);
            if (written == remaining)
                break;
            remaining -= written;
            thread::yield();
        }
    }
}

void get2(void)
{
    for(;;)
    {
        guid id[2];

        uint count = rb.read_ringbuffer(id, 2);
        if (not running and not count)
            return;
        if (count)
        {
            mutex::scoped_lock lock(guard);
            for (uint i = 0; i != count; ++i)
            {
                assert(working_set.find(id[i]) != working_set.end());
                working_set.erase(id[i]);
            }
        }
        else
            thread::yield();
    }
}

int main()
{
    thread_group writer;
    thread_group reader;

    reader.create_thread(&get1);
    writer.create_thread(&add1);

    cout << "reader and writer thread created" << endl;

    writer.join_all();
    cout << "writer thread joined. waiting for reader to finish" << endl;

    running = false;
    reader.join_all();

    cout << "reader thread joined" << endl;

    if (not working_set.empty())
        cout << "damn ... " << working_set.size()
             << " nodes lost" << endl;

    assert (working_set.empty());


    running = true;
    reader.create_thread(&get2);
    writer.create_thread(&add2);

    cout << "reader and writer thread created" << endl;

    writer.join_all();
    cout << "writer thread joined. waiting for reader to finish" << endl;

    running = false;
    reader.join_all();

    cout << "reader thread joined" << endl;

    if (not working_set.empty())
        cout << "damn ... " << working_set.size()
             << " nodes lost" << endl;

    assert (working_set.empty());


};
