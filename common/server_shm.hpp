//  shared memory interface to the supercollider server
//  Copyright (C) 2011 Tim Blechmann
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

#include <boost/foreach.hpp>
#include <boost/ref.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/containers/vector.hpp>

namespace detail_server_shm {

using namespace std;
using namespace boost;
using namespace boost::interprocess;
namespace bi = boost::interprocess;

struct scope_buffer
{
	offset_ptr<float> data;
	int frames;
};

struct server_shared_memory
{
	typedef offset_ptr<float> sh_float_ptr;
	typedef bi::allocator<scope_buffer, managed_shared_memory::segment_manager> scope_buffer_allocator;
	typedef bi::vector<scope_buffer, scope_buffer_allocator> scope_buffer_vector;

	server_shared_memory(managed_shared_memory & segment, int control_busses, int num_scope_buffers = 128, int max_scope_frames = 8192):
		num_control_busses(control_busses), scope_buffers(scope_buffer_allocator(segment.get_segment_manager())),
		max_scope_frames(max_scope_frames)
	{
		control_busses_ = (float*)segment.allocate(control_busses * sizeof(float));
		std::fill(control_busses_.get(), control_busses_.get() + control_busses, 0);

		for (int i = 0; i != num_scope_buffers; ++i) {
			scope_buffer buffer;
			buffer.data = (float*)segment.allocate(max_scope_frames * sizeof(float));
			buffer.frames = 0; // set from ugen
			scope_buffers.push_back(buffer);
		}
	}

	void destroy(managed_shared_memory & segment)
	{
		segment.deallocate(control_busses_.get());

		BOOST_FOREACH(scope_buffer & buf, scope_buffers) {
			segment.deallocate(buf.data.get());
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

	int get_scope_buffer(int index, float ** outBuffer, int **outFrames)
	{
		if (index < scope_buffers.size()) {
			scope_buffer & buf = scope_buffers[index];
			*outBuffer = buf.data.get();
			*outFrames = &buf.frames;

			return max_scope_frames;
		} else {
			*outBuffer = NULL;
			*outFrames = NULL;
			return 0;
		}
	}

private:
	int num_control_busses;
	sh_float_ptr control_busses_; // control busses
	scope_buffer_vector scope_buffers;
	int max_scope_frames;
};

static inline string make_shmem_name(uint port_number)
{
	return string("SuperColliderServer_") + lexical_cast<string>(port_number);
}

class server_shared_memory_creator
{
public:
	server_shared_memory_creator(uint port_number, uint control_busses):
		shmem_name(detail_server_shm::make_shmem_name(port_number)),
		segment(open_or_create, shmem_name.c_str(), 8192 * 1024)
	{
		segment.flush();
		shm = segment.construct<server_shared_memory>(shmem_name.c_str())(ref(segment), control_busses);
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

	int get_scope_buffer(int index, float ** outBuffer, int **outFrames)
	{
		return shm->get_scope_buffer(index, outBuffer, outFrames);
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

	int get_scope_buffer(int index, float ** outBuffer, int **outFrames)
	{
		return shm->get_scope_buffer(index, outBuffer, outFrames);
	}

private:
	string shmem_name;
	managed_shared_memory segment;
	server_shared_memory * shm;
};

}

using detail_server_shm::server_shared_memory_client;
using detail_server_shm::server_shared_memory_creator;

#endif /* SERVER_SHM_HPP */
