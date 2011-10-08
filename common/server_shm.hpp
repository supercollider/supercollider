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

#include <boost/ref.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>

namespace detail {

struct server_shared_memory
{
	typedef boost::interprocess::managed_shared_memory managed_shared_memory;
	typedef boost::interprocess::offset_ptr<float> sh_float_ptr;

	server_shared_memory(managed_shared_memory & segment, int control_busses):
		num_control_busses(control_busses)
	{
		control_busses_ = (float*)segment.allocate(control_busses * sizeof(float));
		std::fill(control_busses_.get(), control_busses_.get() + control_busses, 0);
	}

	void destroy(managed_shared_memory & segment)
	{
		segment.deallocate(control_busses_.get());
	}

	void set_control_bus(int bus, float value)
	{
		// TODO: we need to set the control busses via a work queue
	}

	float * get_control_busses(void)
	{
		return control_busses_.get();
	}

private:
	int num_control_busses;
	sh_float_ptr control_busses_; // control busses
};

static inline std::string make_shmem_name(uint port_number)
{
	return std::string("SuperColliderServer_") + boost::lexical_cast<std::string>(port_number);
}

}

class server_shared_memory_creator
{
	typedef boost::interprocess::managed_shared_memory managed_shared_memory;
	typedef boost::interprocess::shared_memory_object shared_memory_object;
	typedef boost::interprocess::open_or_create_t open_or_create_t;
	typedef std::string string;
	typedef detail::server_shared_memory server_shared_memory;

public:
	server_shared_memory_creator(uint port_number, uint control_busses):
		shmem_name(detail::make_shmem_name(port_number)),
		segment(open_or_create_t(), shmem_name.c_str(), 65536)
	{
		segment.flush();
		shm = segment.construct<server_shared_memory>(shmem_name.c_str())(boost::ref(segment), control_busses);
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

private:
	string shmem_name;
	managed_shared_memory segment;

protected:
	server_shared_memory * shm;
};


class server_shared_memory_client
{
	typedef boost::interprocess::open_only_t open_only_t;
	typedef std::string string;
	typedef detail::server_shared_memory server_shared_memory;
	typedef boost::interprocess::managed_shared_memory managed_shared_memory;

public:
	server_shared_memory_client(uint port_number):
		shmem_name(detail::make_shmem_name(port_number)),
		segment(open_only_t(), shmem_name.c_str())
	{
		std::pair<detail::server_shared_memory*, std::size_t> res = segment.find<server_shared_memory> (shmem_name.c_str());
		if (res.second != 1)
			throw std::runtime_error("Cannot connect to shared memory");
		shm = res.first;
	}

	float * get_control_busses(void)
	{
		return shm->get_control_busses();
	}

private:
	string shmem_name;
	managed_shared_memory segment;
	detail::server_shared_memory * shm;
};

#endif /* SERVER_SHM_HPP */
