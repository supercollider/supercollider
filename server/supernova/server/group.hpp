//  group
//  Copyright (C) 2008, 2009, 2010, 2011 Tim Blechmann
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

#include <boost/tuple/tuple.hpp> /* for boost::tie */

#include "memory_pool.hpp"
#include "node_types.hpp"
#include "dsp_thread_queue_node.hpp"

#include "dsp_thread_queue/dsp_thread_queue.hpp"
#include "utilities/exists.hpp"

namespace nova
{

typedef nova::dsp_queue_node<rt_pool_allocator<void*> > queue_node;
typedef nova::dsp_thread_queue_item<dsp_queue_node<rt_pool_allocator<void*> >,
                                    rt_pool_allocator<void*> > thread_queue_item;
typedef nova::dsp_thread_queue<dsp_queue_node<rt_pool_allocator<void*> >,
                               rt_pool_allocator<void*> > thread_queue;

struct abstract_group_tag;

typedef bi::list_base_hook<bi::link_mode<bi::auto_unlink>,
                           bi::tag<abstract_group_tag>  >
group_list_hook;

typedef boost::intrusive::list<class abstract_group,
                               bi::constant_time_size<false>,
                               bi::base_hook<group_list_hook> >
group_list;

class abstract_group:
    public server_node,
    public group_list_hook
{
    friend class dependency_graph_generator;

protected:
    server_node_list child_nodes;
    group_list child_groups;
    const bool is_parallel_;

    abstract_group(int node_id, bool is_parallel):
        server_node(node_id, false), is_parallel_(is_parallel), child_synth_count(0), child_group_count(0)
    {}

public:
    virtual ~abstract_group(void);

    bool is_parallel(void) const
    {
        return is_parallel_;
    }


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

    bool has_child(const server_node * node) const;

    bool empty(void) const
    {
        return child_nodes.empty();
    }

    /* returns true, if this or any of the child group has synth children */
    bool has_synth_children(void) const
    {
        if (child_synth_count)
            return true;

        for (group_list::const_iterator it = child_groups.begin(); it != child_groups.end(); ++it)
            if (it->has_synth_children())
                return true;

        return false;
    }

    std::size_t child_count(void) const
    {
        assert(child_group_count == child_groups.size());
        assert(child_synth_count + child_group_count == child_nodes.size());
        return child_synth_count + child_group_count;
    }

    /* number of child synths and groups */
    std::pair<std::size_t, std::size_t> child_count_deep(void) const
    {
        std::size_t synths = child_synth_count;
        std::size_t groups = child_group_count;

        for (group_list::const_iterator it = child_groups.begin(); it != child_groups.end(); ++it)
        {
            std::size_t recursive_synths, recursive_groups;
            boost::tie(recursive_synths, recursive_groups) = it->child_count_deep();
            groups += recursive_groups;
            synths += recursive_synths;
        }
        return std::make_pair(synths, groups);
    }

    template<typename functor>
    void apply_on_children(functor const & f)
    {
        for (server_node_list::iterator it = child_nodes.begin(); it != child_nodes.end(); ++it)
            f(*it);
    }

    template<typename functor>
    void apply_on_children(functor const & f) const
    {
        for (server_node_list::const_iterator it = child_nodes.begin(); it != child_nodes.end(); ++it)
            f(*it);
    }
    /* @} */

    /* @{ */
    void register_as_child(void)
    {
        parent_->child_groups.push_back(*this);
    }

    void unregister_as_child(void)
    {
        group_list_hook::unlink();
    }
    /* @} */


public:
    server_node * next_node(server_node * node)
    {
        assert(has_child(node));
        server_node_list::iterator next = ++server_node_list::s_iterator_to(*node);
        if (unlikely(next == child_nodes.end()))
            return 0;
        else
            return &(*next);
    }

    server_node * previous_node(server_node * node)
    {
        assert(has_child(node));
        server_node_list::iterator it = server_node_list::s_iterator_to(*node);
        if (unlikely(it == child_nodes.begin()))
            return 0;
        else
            return &(*--it);
    }

    void free_children(void)
    {
        child_nodes.clear_and_dispose(boost::mem_fn(&server_node::clear_parent));
        assert(child_synth_count == 0);
        assert(child_group_count == 0);
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
        assert(child_synth_count == 0);
    }

    /** remove node from child_nodes, clear node->parent */
    void remove_child(server_node * node);
    /* @} */

    void set(slot_index_t slot_id, float val);
    void set(const char * slot_str, float val);
    void set(const char * slot_str, size_t hashed_str, float val);

    void set_control_array(slot_index_t slot_str, size_t count, float * val);
    void set_control_array(const char * slot_str, size_t count, float * val);
    void set_control_array(const char * slot_str, size_t hashed_str, size_t count, float * val);

    void set_control_array_element(slot_index_t slot_str, size_t count, float val);
    void set_control_array_element(const char * slot_str, size_t count, float val);
    void set_control_array_element(const char * slot_str, size_t hashed_str, size_t count, float val);

    /* move node to head or tail of target */
    template <node_position Position>
    static void move_to_head_or_tail(server_node * node, abstract_group * target)
    {
        assert((Position == head) || (Position == tail));

        server_node_list::const_iterator target_iterator = (Position == head) ? target->child_nodes.begin()
                                                                              : target->child_nodes.end();

        server_node_list::const_iterator node_iterator = server_node_list::s_iterator_to(*node);

        abstract_group * node_parent = node->get_parent();
        if (node_parent != target) {
            if (node->is_synth()) {
                node_parent->child_synth_count -= 1;
                target->child_synth_count += 1;
            } else {
                node_parent->child_group_count -= 1;
                target->child_group_count += 1;

                group_list::const_iterator group_target_iterator = (Position == head) ? target->child_groups.begin()
                                                                                    : target->child_groups.end();

                abstract_group * node_as_group = static_cast<abstract_group*>(node);
                target->child_groups.splice(group_target_iterator, node_parent->child_groups,
                                            group_list::s_iterator_to(*node_as_group));
            }
            node->parent_ = target;
        }

        target->child_nodes.splice(target_iterator, node_parent->child_nodes, node_iterator);
    }

    template <node_position Relation>
    static void move_before_or_after(server_node * node, server_node * target)
    {
        assert((Relation == before) || (Relation == after));
        abstract_group * target_parent = node->get_parent();

        if (Relation == after && target->next_node() == NULL) {
            // for the sake of simplicity, move the node to the tail of the target's parent group
            move_to_head_or_tail<tail>(node, target_parent);
            return;
        }

        server_node * node_behind = (Relation == before) ? target
                                                         : target->next_node();
        assert(node_behind);
        server_node_list::const_iterator target_iterator = server_node_list::s_iterator_to(*node_behind);
        server_node_list::const_iterator node_iterator = server_node_list::s_iterator_to(*node);

        abstract_group * node_parent = node->get_parent();
        if (node_parent != target_parent) {
            if (node->is_synth()) {
                node_parent->child_synth_count -= 1;
                target_parent->child_synth_count += 1;
            } else {
                node_parent->child_group_count -= 1;
                target_parent->child_group_count += 1;

                group_list::const_iterator group_target_iterator = group_list::s_iterator_to(*static_cast<abstract_group*>(node));

                target_parent->child_groups.splice(target_parent->child_groups.end(), node_parent->child_groups,
                                                group_target_iterator);
            }
            node->parent_ = target_parent;
        }

        target_parent->child_nodes.splice(target_iterator, node_parent->child_nodes, node_iterator);
    }

    friend class node_graph;
    std::size_t child_synth_count, child_group_count;
};


inline void server_node::clear_parent(void)
{
    if (is_synth())
        --parent_->child_synth_count;
    else {
        --parent_->child_group_count;
        static_cast<abstract_group*>(this)->unregister_as_child();
    }

    parent_ = 0;
    release();
}

inline void server_node::set_parent(abstract_group * parent)
{
    add_ref();
    assert(parent_ == 0);
    parent_ = parent;

    if (is_synth())
        ++parent->child_synth_count;
    else {
        ++parent->child_group_count;
        static_cast<abstract_group*>(this)->register_as_child();
    }
}



inline server_node * server_node::previous_node(void)
{
    return parent_->previous_node(this);
}

inline server_node * server_node::next_node(void)
{
    return parent_->next_node(this);
}

inline const server_node * server_node::previous_node(void) const
{
    return const_cast<server_node*>(this)->previous_node();
}

inline const server_node * server_node::next_node(void) const
{
    return const_cast<server_node*>(this)->next_node();
}


class group:
    public abstract_group
{
public:
    group(int node_id):
        abstract_group(node_id, false)
    {}

    const server_node * head_node(void) const
    {
        return &*child_nodes.begin();
    }

    const server_node * tail_node(void) const
    {
        return &*child_nodes.rbegin();
    }

private:
    void add_child(server_node * node, node_position_constraint const & constraint);
    void add_child(server_node * node, node_position);

    friend class dependency_graph_generator;

    virtual int tail_nodes(void) const
    {
        if (empty())
            return 0;

        for (server_node_list::const_reverse_iterator it = child_nodes.rbegin(); it != child_nodes.rend(); ++it)
        {
            const server_node * tail = &*it;

            if (tail->is_synth())
                return 1;
            const abstract_group * tail_group = static_cast<const abstract_group*>(tail);
            if (!tail_group->empty())
                return tail_group->tail_nodes();
        }
        return 0;
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

    virtual int tail_nodes(void) const;

    friend class dependency_graph_generator;
};

} /* namespace nova */

#endif /* SERVER_GROUP_HPP */
