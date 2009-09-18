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

#include "node_types.hpp"
#include "utilities/exists.hpp"

namespace nova
{

class synth;

typedef nova::dsp_thread_queue_item<dsp_queue_node> thread_queue_item;
typedef nova::dsp_thread_queue<dsp_queue_node> thread_queue;

class abstract_group:
    public server_node
{
public:
    typedef std::vector<thread_queue_item*> successor_container;

    typedef boost::intrusive::list<server_node,
                                   boost::intrusive::constant_time_size<false> >
    node_list;

protected:
    node_list child_nodes;

    abstract_group(int node_id):
        server_node(node_id, false)
    {}

public:
    virtual ~abstract_group(void);

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
    /** child management  */
    void add_child(server_node * node);
    virtual void add_child(server_node * node, node_position_constraint const &) = 0;
    virtual void add_child(server_node * node, node_position) = 0;

    bool has_child(server_node * node);

    bool empty(void) const
    {
        return child_nodes.empty();
    }

private:
    struct node_disposer
    {
        void operator()(server_node * node)
        {
            node->clear_parent();
        }
    };

    struct is_synth
    {
        bool operator()(server_node const & node)
        {
            return node.is_synth();
        }
    };

public:
    void free_children(void)
    {
        child_nodes.clear_and_dispose(node_disposer());
    }

    void free_synths_deep(void)
    {
        child_nodes.remove_and_dispose_if(is_synth(), node_disposer());

        /* now there are only group classes */
        for(node_list::iterator it = child_nodes.begin(); it != child_nodes.end(); ++it) {
            abstract_group * group = static_cast<abstract_group*>(&*it);
            group->free_synths_deep();
        }
    }

    void remove_child(server_node * node);
    /* @} */

    void set(const char * slot_str, float val);
    void set(slot_index_t slot_id, float val);

    friend class node_graph;
};

class group:
    public abstract_group
{
public:
    group(int node_id):
        abstract_group(node_id)
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
        abstract_group(node_id)
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
