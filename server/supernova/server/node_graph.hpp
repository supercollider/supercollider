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
#include <tuple>

#include "dsp_thread_queue_node.hpp"
#include "group.hpp"
#include "node_types.hpp"

#include "dsp_thread_queue/dsp_thread_queue.hpp"

namespace nova {

class node_graph
{
private:
    group root_group_ = {0};
    friend class dependency_graph_generator;

    static const std::size_t node_set_bucket_count = 4096;

public:
    typedef thread_queue_item dsp_thread_queue_item;
    typedef thread_queue dsp_thread_queue;

    /** \brief ctor
     *
     * - initialize root node */
    node_graph(void):
        node_set(node_set_type::bucket_traits(node_buckets, node_set_bucket_count))
    {
        node_set.insert(root_group_);
        root_group_.add_ref();
    }

    ~node_graph()
    {
        assert(root_group_.child_count() == 0);
    }

    uint32_t synth_count(void) const
    {
        return synth_count_;
    }

    uint32_t group_count(void) const
    {
        return group_count_;
    }

    template <typename Functor>
    void add_node(server_node * node, node_position_constraint const & constraint, Functor const & doOnFree)
    {
        server_node * reference_node = constraint.first;
        node_position position = constraint.second;

        std::pair< node_set_type::iterator, bool > inserted = node_set.insert(*node);
        node->add_ref();

        assert(inserted.second == true); /* node id already present (should be checked earlier)! */

        switch (position) {
        case before:
        case after: {
            abstract_group * parent = reference_node->parent_;
            parent->add_child(node, constraint);
            break;
        }

        case head:
        case tail: {
            abstract_group * group = static_cast<abstract_group*>(reference_node);
            group->add_child(node, position);
            break;
        }

        case insert: {
            abstract_group * group = static_cast<abstract_group*>(reference_node);
            group->add_child(node);
            break;
        }

        case replace: {
            const bool reference_node_is_group = reference_node->is_group();
            if (reference_node_is_group) {
                abstract_group * reference_group = static_cast<abstract_group*>(reference_node);
                reference_group->apply_deep_on_children( [&] (server_node & node) {
                    doOnFree(node);
                });
            }
            doOnFree(*reference_node);

            abstract_group * node_parent = reference_node->parent_;
            node_parent->replace_child(node, reference_node);

            if (reference_node_is_group)
                group_count_ -= 1;
            else
                synth_count_ -= 1;

            break;
        }

        default:
            assert(false);      /* this point should not be reached! */
        }

        if (node->is_synth())
            synth_count_ += 1;
        else
            group_count_ += 1;
    }

    void add_node(server_node * node, node_position_constraint const & constraint);
    void add_node(server_node * s);

    void remove_node(server_node * n)
    {
        remove_node(n, [](server_node &){});
    }

    template <typename Functor>
    void remove_node(server_node * node, Functor const & doOnFree)
    {
        if (!node->is_synth())
            group_free_all(static_cast<abstract_group*>(node), doOnFree);

        /** \todo recursively remove nodes from node_set
         *        for now this is done by the auto-unlink hook
         * */

        doOnFree(*node);
        abstract_group * parent = node->parent_;
        parent->remove_child(node);
        if (node->is_synth())
            synth_count_ -= 1;
        else
            group_count_ -= 1;

        release_node_id(node);
    }

    void dump(std::string const & filename);

    group * root_group(void)
    {
        return &root_group_;
    }

    typedef std::unique_ptr<dsp_thread_queue> dsp_thread_queue_ptr;

    dsp_thread_queue_ptr generate_dsp_queue(void);

    server_node * find_node(int32_t node_id)
    {
        node_set_type::iterator it = node_set.find(node_id, hash_node(), equal_node());
        if (it == node_set.end())
            return nullptr;
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
    int32_t generated_id = -2;

public:
    abstract_group * find_group(int32_t node_id)
    {
        server_node * node = find_node(node_id);
        if (!node || node->is_synth())
            return nullptr;
        else
            return static_cast<abstract_group*>(node);
    }

    abstract_synth * find_synth(int32_t node_id)
    {
        server_node * node = find_node(node_id);
        if (node && node->is_synth())
            return static_cast<abstract_synth*>(node);
        else
            return nullptr;
    }

    void group_free_all(abstract_group * group)
    {
        group_free_all(group, [](server_node &){});
    }

    void group_free_deep(abstract_group * group)
    {
        group_free_deep(group, [](server_node &){});
    }

    template <typename Functor>
    void group_free_all(abstract_group * group, Functor const & doOnFree)
    {
        size_t synths = 0, groups = 0;
        group->apply_deep_on_children([&](server_node & node) {
            release_node_id(&node);
            if (node.is_synth())
                synths += 1;
            else
                groups += 1;
            doOnFree(node);
        });

        group->free_children();
        synth_count_ -= synths;
        group_count_ -= groups;
    }

    template <typename Functor>
    void group_free_deep(abstract_group * group, Functor const & doOnFree)
    {
        size_t synths = 0;
        group->apply_deep_on_children([&](server_node & node) {
             if (node.is_synth()) {
                 release_node_id(&node);
                 synths += 1;
             }
             doOnFree(node);
        });

        group->free_synths_deep();
        synth_count_ -= synths;
    }

    void release_node_id(server_node * node)
    {
        node_set.erase(*node);
        node->release();
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

    uint32_t synth_count_ = 0, group_count_ = 1;

    node_set_type::bucket_type node_buckets[node_set_bucket_count];
    node_set_type node_set;
};

} /* namespace nova */

#endif /* SERVER_NODEGRAPH_HPP */
