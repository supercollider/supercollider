//  node graph
//  Copyright (C) 2008, 2009, 2010 Tim Blechmann
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

#include "boost/tuple/tuple.hpp"

#include "dsp_thread_queue_node.hpp"
#include "group.hpp"
#include "node_types.hpp"

#include "dsp_thread_queue/dsp_thread_queue.hpp"

namespace nova {

class node_graph
{
private:
    group root_group_;
    friend class dependency_graph_generator;

    static const std::size_t node_set_bucket_count = 4096;

public:
    typedef thread_queue_item dsp_thread_queue_item;
    typedef thread_queue dsp_thread_queue;

    /** \brief ctor
     *
     * - initialize root node */
    node_graph(void):
        root_group_(0), generated_id(-2), synth_count_(0), group_count_(1),
        node_set(node_set_type::bucket_traits(node_buckets, node_set_bucket_count))
    {
        root_group_.add_ref();
        node_set.insert(root_group_);
    }

    ~node_graph(void)
    {}

    uint32_t synth_count(void) const
    {
        return synth_count_;
    }

    uint32_t group_count(void) const
    {
        return group_count_;
    }


    void add_node(server_node * s, node_position_constraint const & constraint);
    void add_node(server_node * s);

    void remove_node(server_node * n);

    void dump(std::string const & filename);

    group * root_group(void)
    {
        return &root_group_/* .get() */;
    }

    typedef std::unique_ptr<dsp_thread_queue> dsp_thread_queue_ptr;

    dsp_thread_queue_ptr generate_dsp_queue(void);

    server_node * find_node(int32_t node_id)
    {
        node_set_type::iterator it = node_set.find(node_id, hash_node(), equal_node());
        if (it == node_set.end())
            return NULL;
        return &(*it);
    }

    bool node_id_available(int32_t node_id)
    {
        node_set_type::iterator it = node_set.find(node_id, hash_node(), equal_node());
        return (it == node_set.end());
    }

    void synth_reassign_id(int32_t node_id);

    /** generate new hidden (negative) node id  */
    int32_t generate_node_id(void);

private:
    int32_t generated_id;

public:
    abstract_group * find_group(int32_t node_id)
    {
        server_node * node = find_node(node_id);
        if (!node || node->is_synth())
            return NULL;
        else
            return static_cast<abstract_group*>(node);
    }

    abstract_synth * find_synth(int32_t node_id)
    {
        server_node * node = find_node(node_id);
        if (node && node->is_synth())
            return static_cast<abstract_synth*>(node);
        else
            return NULL;
    }

    bool group_free_all(abstract_group * group)
    {
        size_t synths = 0, groups = 0;
        group->apply_deep_on_children([&](server_node & node) {
            release_node_id(&node);
            if (node.is_synth())
                synths += 1;
            else
                groups += 1;
        });

        group->free_children();
        synth_count_ -= synths;
        group_count_ -= groups;
        return true;
    }

    bool group_free_deep(abstract_group * group)
    {
        size_t synths;
        group->apply_deep_on_children([&](server_node & node) {
             if (node.is_synth()) {
                 release_node_id(&node);
                 synths += 1;
             }
        });

        group->free_synths_deep();
        synth_count_ -= synths;
        return true;
    }

    void release_node_id(server_node * node)
    {
        node_set.erase(*node);
    }

private:
    struct equal_node
    {
        bool operator()(server_node const & lhs,
                        int32_t const & rhs) const
        {
            return lhs.node_id == rhs;
        }

        bool operator()(int32_t const &lhs,
                        server_node const & rhs) const
        {
            return lhs == rhs.node_id;
        }
    };

    struct hash_node
    {
        std::size_t operator()(int32_t value)
        {
            return server_node::hash(value);
        }
    };

    typedef boost::intrusive::unordered_set< server_node,
                                             boost::intrusive::constant_time_size<false>,
                                             boost::intrusive::power_2_buckets<true>
                                           > node_set_type;

    uint32_t synth_count_, group_count_;

    node_set_type::bucket_type node_buckets[node_set_bucket_count];
    node_set_type node_set;
};

} /* namespace nova */

#endif /* SERVER_NODEGRAPH_HPP */
