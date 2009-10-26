//  group
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

#ifndef SERVER_GROUP_HPP
#define SERVER_GROUP_HPP

#include <set>

#include "memory_pool.hpp"
#include "node_types.hpp"
#include "utilities/exists.hpp"

namespace nova
{

class synth;

typedef nova::dsp_thread_queue_item<dsp_queue_node, rt_pool_allocator<void*> > thread_queue_item;
typedef nova::dsp_thread_queue<dsp_queue_node, rt_pool_allocator<void*> > thread_queue;

class abstract_group:
    public server_node
{
public:
    typedef thread_queue_item::successor_list successor_container;

protected:
    server_node_list child_nodes;
    const bool is_parallel_;

    abstract_group(int node_id, bool is_parallel):
        server_node(node_id, false), is_parallel_(is_parallel)
    {}

public:
    virtual ~abstract_group(void);

    bool is_parallel(void) const
    {
        return is_parallel_;
    }

protected:
    virtual successor_container fill_queue_recursive(thread_queue & queue,
                                                     successor_container,
                                                     int activation_limit) = 0;
    friend class group;
    friend class parallel_group;

public:
    /* count the tail nodes to get activation count */
    virtual int tail_nodes(void) const = 0;

    /* @{ */
    /** pause/resume handling (set pause/resume on children)  */
    virtual void pause(void);
    virtual void resume(void);
    /* @} */

    /* @{ */
    /** child management  */
    void add_child(server_node * node);
    virtual void add_child(server_node * node, node_position_constraint const &) = 0;
    virtual void add_child(server_node * node, node_position) = 0;

    bool has_child(server_node * node);

    bool empty(void) const
    {
        return child_nodes.empty();
    }

    std::size_t child_count(void) const
    {
        return child_nodes.size();
    }

    template<typename functor>
    void apply_on_children(functor const & f)
    {
        for (server_node_list::iterator it = child_nodes.begin(); it != child_nodes.end(); ++it)
            f(*it);
    }

public:
    server_node * next_node(server_node * node)
    {
        assert(has_child(node));
        server_node_list::iterator next = ++server_node_list::s_iterator_to(*node);
        if (next == child_nodes.end())
            return 0;
        else
            return &(*next);
    }

    server_node * previous_node(server_node * node)
    {
        assert(has_child(node));
        server_node_list::iterator it = server_node_list::s_iterator_to(*node);
        if (it == child_nodes.begin())
            return 0;
        else
            return &(*--it);
    }

    void free_children(void)
    {
        child_nodes.clear_and_dispose(boost::mem_fn(&server_node::clear_parent));
    }

    void free_synths_deep(void)
    {
        child_nodes.remove_and_dispose_if(boost::mem_fn(&server_node::is_synth),
                                          boost::mem_fn(&server_node::clear_parent));

        /* now there are only group classes */
        for(server_node_list::iterator it = child_nodes.begin(); it != child_nodes.end(); ++it) {
            abstract_group * group = static_cast<abstract_group*>(&*it);
            group->free_synths_deep();
        }
    }

    /** remove node from child_nodes, clear node->parent */
    void remove_child(server_node * node);
    /* @} */

    void set(const char * slot_str, float val);
    void set(const char * slot_str, size_t count, float * val);
    void set(slot_index_t slot_id, float val);
    void set(slot_index_t slot_str, size_t count, float * val);

    friend class node_graph;
};

inline server_node * server_node::previous_node(void)
{
    return parent_->previous_node(this);
}

inline server_node * server_node::next_node(void)
{
    return parent_->next_node(this);
}


class group:
    public abstract_group
{
public:
    group(int node_id):
        abstract_group(node_id, false)
    {}

private:
    void add_child(server_node * node, node_position_constraint const & constraint);
    void add_child(server_node * node, node_position);

    void fill_queue(thread_queue & queue);

    virtual successor_container fill_queue_recursive(thread_queue & queue,
                                                     successor_container,
                                                     int activation_limit);

    friend class node_graph;
    friend class server_node;

    virtual int tail_nodes(void) const
    {
        return 1;
    }
};

typedef intrusive_ptr<group> group_ptr;

class parallel_group:
    public abstract_group
{
public:
    parallel_group(int node_id):
        abstract_group(node_id, true)
    {}

private:
    void add_child(server_node * node, node_position_constraint const & constraint);
    void add_child(server_node * node, node_position);

    virtual successor_container fill_queue_recursive(thread_queue & queue,
                                                     successor_container,
                                                     int activation_limit);

    virtual int tail_nodes(void) const;

    friend class node_graph;
    friend class server_node;
};

} /* namespace nova */

#endif /* SERVER_GROUP_HPP */
