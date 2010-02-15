//  dsp thread queue nodes
//  Copyright (C) 2008 Tim Blechmann
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

#ifndef SERVER_DSP_THREAD_QUEUE_NODE_HPP
#define SERVER_DSP_THREAD_QUEUE_NODE_HPP

#include <memory>
#include <vector>

#include <boost/cstdint.hpp>

#include "node_types.hpp"
#include "synth.hpp"

namespace nova
{

class queue_node_data
{
    typedef boost::uint_fast8_t thread_count_type;

public:
    queue_node_data(synth * node):
        node(node)
    {}

    queue_node_data(queue_node_data const & rhs):
        node(rhs.node)
    {}

    void operator()(thread_count_type index)
    {
        if (unlikely(!node->is_running()))
            return;
        dsp_context context(44100, 64, index);
        node->run(context);
    }

private:
    synth_ptr node;
    friend class dsp_queue_node;
};

/* implements runnable concept */
class dsp_queue_node
{
    typedef std::vector<queue_node_data> node_container;

    typedef boost::uint_fast16_t node_count_type;
    typedef boost::uint_fast8_t thread_count_type;

public:
    dsp_queue_node(std::size_t container_size):
        node_count(0)
    {
        nodes.reserve(container_size);
    }

    void operator()(thread_count_type thread_index)
    {
        for (node_count_type i = 0; i != node_count; ++i)
            nodes[i](thread_index);
    }

    void add_node(synth * node)
    {
        nodes.push_back(queue_node_data(node));
        ++node_count;
    }

    node_count_type size(void) const
    {
        return node_count;
    }

private:
    node_container nodes;
    node_count_type node_count;
};

} /* namespace nova */

#endif /* SERVER_DSP_THREAD_QUEUE_NODE_HPP */
