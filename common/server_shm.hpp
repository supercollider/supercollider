//  shared memory interface to the supercollider server
//  Copyright (C) 2011 Tim Blechmann
//  Copyright (C) 2011 Jakob Leben
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

#ifndef SERVER_SHM_HPP
#define SERVER_SHM_HPP

#include "scope_buffer.hpp"

#include <boost/foreach.hpp>
#include <boost/ref.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/atomic.hpp>

namespace detail_server_shm {

using namespace std;
using namespace boost;
using namespace boost::interprocess;
namespace bi = boost::interprocess;

static inline string make_shmem_name(uint port_number)
{
	return string("SuperColliderServer_") + lexical_cast<string>(port_number);
}

static inline string make_scope_buffer_name(const string &shmem_name, int buffer_index)
{
	return shmem_name + "_ScopeBuffer_" + lexical_cast<string>(buffer_index);
}

struct server_shared_memory
{
	typedef offset_ptr<float> sh_float_ptr;
	typedef offset_ptr<scope_buffer> scope_buffer_ptr;

	typedef bi::allocator<scope_buffer_ptr, managed_shared_memory::segment_manager> scope_buffer_allocator;
	typedef bi::vector<scope_buffer_ptr, scope_buffer_allocator> scope_buffer_vector;

	server_shared_memory(
		managed_shared_memory & segment, const string & shmem_name_,
		int control_busses, int num_scope_buffers = 128
	):
		shmem_name(shmem_name_),
		num_control_busses(control_busses),
		scope_buffers(scope_buffer_allocator(segment.get_segment_manager()))
	{
		control_busses_ = (float*)segment.allocate(control_busses * sizeof(float));
		std::fill(control_busses_.get(), control_busses_.get() + control_busses, 0);

		for (int i = 0; i != num_scope_buffers; ++i) {
			string name = make_scope_buffer_name(shmem_name_, i);
			scope_buffer_ptr buf = segment.construct<scope_buffer>(name.c_str())();
			scope_buffers.push_back(buf);
		}
	}

	void destroy(managed_shared_memory & segment)
	{
		segment.deallocate(control_busses_.get());

		for (int i = 0; i != scope_buffers.size(); ++i) {
			string name = make_scope_buffer_name(shmem_name,i);
			segment.destroy<scope_buffer>(name.c_str());
		}
	}

	void set_control_bus(int bus, float value)
	{
		// TODO: we need to set the control busses via a work queue
	}

	float * get_control_busses(void)
	{
		return control_busses_.get();
	}

	scope_buffer * get_scope_buffer(uint index)
	{
		if (index < scope_buffers.size())
			return scope_buffers[index].get();
		else
			return 0;
	}

private:
	string shmem_name;
	int num_control_busses;
	sh_float_ptr control_busses_; // control busses
	scope_buffer_vector scope_buffers;
	interprocess_mutex mutex;
};

class server_shared_memory_creator
{
public:
	server_shared_memory_creator(uint port_number, uint control_busses):
		shmem_name(detail_server_shm::make_shmem_name(port_number)),
		segment(open_or_create, shmem_name.c_str(), 8192 * 1024)
	{
		segment.flush();
		shm = segment.construct<server_shared_memory>(shmem_name.c_str())(ref(segment), shmem_name, control_busses);
	}

	static void cleanup(uint port_number)
	{
		shared_memory_object::remove(detail_server_shm::make_shmem_name(port_number).c_str());
	}

	~server_shared_memory_creator(void)
	{
		shm->destroy(segment);
		segment.destroy<server_shared_memory>(shmem_name.c_str());
		shared_memory_object::remove(shmem_name.c_str());
	}

	float * get_control_busses(void)
	{
		return shm->get_control_busses();
	}

	scope_buffer_writer get_scope_buffer_writer(uint index, uint channels, uint size)
	{
		scope_buffer *buf = shm->get_scope_buffer(index);
		if (buf)
			return scope_buffer_writer(buf, segment, channels, size);
		else
			return scope_buffer_writer();
	}

	void release_scope_buffer_writer( scope_buffer_writer & writer )
	{
		writer.release( segment );
	}

private:
	string shmem_name;
	managed_shared_memory segment;

protected:
	server_shared_memory * shm;
};


class server_shared_memory_client
{
public:
	server_shared_memory_client(uint port_number):
		shmem_name(detail_server_shm::make_shmem_name(port_number)),
		segment(open_only, shmem_name.c_str())
	{
		pair<server_shared_memory*, size_t> res = segment.find<server_shared_memory> (shmem_name.c_str());
		if (res.second != 1)
			throw std::runtime_error("Cannot connect to shared memory");
		shm = res.first;
	}

	float * get_control_busses(void)
	{
		return shm->get_control_busses();
	}

	scope_buffer_reader get_scope_buffer_reader(uint index)
	{
		scope_buffer *buf = shm->get_scope_buffer(index);
		return scope_buffer_reader(buf);
	}

private:
	string shmem_name;
	managed_shared_memory segment;
	server_shared_memory * shm;
};

}

using detail_server_shm::server_shared_memory_client;
using detail_server_shm::server_shared_memory_creator;
using detail_server_shm::scope_buffer_writer;
using detail_server_shm::scope_buffer_reader;
using detail_server_shm::scope_buffer;

#endif /* SERVER_SHM_HPP */
