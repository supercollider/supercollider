//  node graph
//  Copyright (C) 2008, 2009 Tim Blechmann
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

#ifndef SERVER_NODE_GRAPH_HPP
#define SERVER_NODE_GRAPH_HPP

#include <memory>

#include "dsp_thread_queue.hpp"
#include "dsp_thread_queue_node.hpp"
#include "group.hpp"
#include "node_types.hpp"
#include "memory_pool.hpp"


namespace nova
{

class node_graph
{
private:
    group root_group_;

public:
    typedef nova::dsp_thread_queue_item<dsp_queue_node, rt_pool_allocator<void*> > dsp_thread_queue_item;
    typedef nova::dsp_thread_queue<dsp_queue_node, rt_pool_allocator<void*> > dsp_thread_queue;

    /** \brief ctor
     *
     * - initialize root node */
    node_graph(void):
        root_group_(0)
    {
        root_group_.add_ref();
        node_set.insert(root_group_);

        to_root.first = &root_group_;
        to_root.second = head;
    }

    ~node_graph(void)
    {}

    void add_node(server_node * s, node_position_constraint const & constraint);
    void add_node(server_node * s);

    void remove_node(server_node * n);

    void dump(std::string const & filename);

    group * root_group(void)
    {
        return &root_group_/* .get() */;
    }

    std::auto_ptr<dsp_thread_queue> generate_dsp_queue(void);

    node_position_constraint to_root;

    server_node * find_node(uint32_t node_id)
    {
        node_set_type::iterator it = node_set.find(node_id, compare_node());
        if (it == node_set.end())
            return 0;
        return &(*it);
    }

    bool node_id_available(int node_id)
    {
        node_set_type::iterator it = node_set.find(node_id, compare_node());
        return (it == node_set.end());
    }

    void synth_reassign_id(int node_id)
    {
        const uint32_t base = 0xffffffff;
        node_set_type::iterator it = node_set.find(node_id, compare_node());
        if (it == node_set.end())
            throw std::runtime_error("node id not found");
        server_node * node = &(*it);

        if (!node->is_synth())
            return;

        uint32_t hidden_id = base;
        while (!node_id_available(hidden_id))
            hidden_id -= 1;

        node_set.erase(*node);
        node->reset_id(hidden_id);
        node_set.insert(*node);
    }

private:
    struct compare_node
    {
        bool operator()(server_node const & lhs,
                        uint32_t const & rhs) const
        {
            return lhs.node_id < rhs;
        }

        bool operator()(uint32_t const &lhs,
                        server_node const & rhs) const
        {
            return lhs < rhs.node_id;
        }
    };

    typedef boost::intrusive::set< server_node,
                                   boost::intrusive::constant_time_size<false>
                                   > node_set_type;
    node_set_type node_set;
};


} /* namespace nova */

#endif /* SERVER_NODEGRAPH_HPP */
