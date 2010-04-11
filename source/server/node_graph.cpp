//  node graph, implementation
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

#include <fstream>

#include "dsp_thread_queue.hpp"
#include "dsp_thread_queue_node.hpp"
#include "node_graph.hpp"
#include "synth.hpp"
#include "utilities/exists.hpp"

namespace nova
{

void node_graph::add_node(server_node * n, node_position_constraint const & constraint)
{
    server_node * node = constraint.first;
    node_position position = constraint.second;

    std::pair< node_set_type::iterator, bool > inserted = node_set.insert(*n);

    assert(inserted.second == true); /* node id already present (should be checked earlier)! */

    switch (position)
    {
    case before:
    case after: {
        abstract_group * parent = node->parent_;
        parent->add_child(n, constraint);
        break;
    }

    case head:
    case tail: {
        abstract_group * group = static_cast<abstract_group*>(node);
        group->add_child(n, position);
        break;
    }

    case insert: {
        abstract_group * group = static_cast<abstract_group*>(node);
        group->add_child(n);
        break;
    }

    default:
        assert(false);      /* this point should not be reached! */
    }

    if (n->is_synth())
        synth_count_ += 1;
    else
        group_count_ += 1;
}

void node_graph::add_node(server_node * n)
{
    node_position_constraint to_root;

    to_root.first = &root_group_;
    to_root.second = head;

    add_node(n, to_root);
}

void node_graph::remove_node(server_node * n)
{
    if (!n->is_synth())
        group_free_all(static_cast<abstract_group*>(n));

    node_set.erase(*n);
    /** \todo recursively remove nodes from node_set
     *        for now this is done by the auto-unlink hook
     * */

    abstract_group * parent = n->parent_;
    parent->remove_child(n);
    if (n->is_synth())
        synth_count_ -= 1;
    else
        group_count_ -= 1;
}

std::auto_ptr<node_graph::dsp_thread_queue> node_graph::generate_dsp_queue(void)
{
    /* pessimize: reserve enough memory for the worst case */
    node_graph::dsp_thread_queue * ret = new node_graph::dsp_thread_queue(synth_count_);

    root_group_.fill_queue(*ret);
    return std::auto_ptr<node_graph::dsp_thread_queue>(ret);
}

void node_graph::synth_reassign_id(int32_t node_id)
{
    node_set_type::iterator it = node_set.find(node_id, hash_node(), equal_node());
    if (it == node_set.end())
        throw std::runtime_error("node id not found");
    server_node * node = &(*it);

    if (!node->is_synth())
        return;

    boost::hash<int32_t> hasher;

    int32_t hidden_id = -std::abs(node_id);

    while (!node_id_available(hidden_id))
        hidden_id = -std::abs<int32_t>(hasher(node_id));

    assert(hidden_id < 0);
    node_set.erase(*node);
    node->reset_id(hidden_id);
    node_set.insert(*node);
}


void group::fill_queue(thread_queue & queue)
{
    if (has_synth_children())
        fill_queue_recursive(queue, successor_container(0), 0);
}


abstract_group::~abstract_group(void)
{
    while (!child_nodes.empty()) {
        server_node & node = child_nodes.front();
        remove_child(&node);
    }
}

void abstract_group::pause(void)
{
    std::for_each(child_nodes.begin(), child_nodes.end(), boost::bind(&server_node::pause, _1));
}

void abstract_group::resume(void)
{
    std::for_each(child_nodes.begin(), child_nodes.end(), boost::bind(&server_node::resume, _1));
}

void abstract_group::add_child(server_node * node)
{
    assert (not has_child(node));

    child_nodes.push_front(*node);
    node->set_parent(this);
}

bool abstract_group::has_child(const server_node * node) const
{
    if (!node->server_node::parent_hook::is_linked())
        return false;

    /* find node in container */
    for (server_node_list::const_iterator it = child_nodes.begin();
         it != child_nodes.end(); ++it) {
        if (&*it == node)
            return true;
    }
    return false;
}

void abstract_group::remove_child(server_node * node)
{
    assert (has_child(node));
    node->server_node::parent_hook::unlink();
    node->clear_parent();
}

void abstract_group::set(const char * slot_str, float val)
{
    for(server_node_list::iterator it = child_nodes.begin();
        it != child_nodes.end(); ++it)
        it->set(slot_str, val);
}

void abstract_group::set(const char * slot_str, size_t count, float * val)
{
    for(server_node_list::iterator it = child_nodes.begin();
        it != child_nodes.end(); ++it)
        it->set(slot_str, count, val);
}

void abstract_group::set(slot_index_t slot_id, float val)
{
    for(server_node_list::iterator it = child_nodes.begin();
        it != child_nodes.end(); ++it)
        it->set(slot_id, val);
}

void abstract_group::set(slot_index_t slot_id, size_t count, float * val)
{
    for(server_node_list::iterator it = child_nodes.begin();
        it != child_nodes.end(); ++it)
        it->set(slot_id, count, val);
}

namespace
{
template <typename reverse_iterator>
inline int get_previous_activation_count(reverse_iterator it, reverse_iterator end, int previous_activation_limit)
{
    reverse_iterator prev = it;

    for(;;)
    {
        ++prev;
        if (prev == end)
            return previous_activation_limit; // we are the first item, so we use the previous activiation limit

        server_node & node = *prev;
        if (node.is_synth())
            return 1;
        else {
            abstract_group & grp = static_cast<abstract_group&>(node);
            int tail_nodes = grp.tail_nodes();

            if (tail_nodes != 0) /* use tail nodes of previous group */
                return tail_nodes;
            else                /* skip empty groups */
                continue;
        }
    }
}

} /* namespace */

abstract_group::successor_container
group::fill_queue_recursive(thread_queue & queue,
                            abstract_group::successor_container const & successors_from_parent,
                            int previous_activation_limit)
{
    assert (has_synth_children());

    typedef server_node_list::reverse_iterator r_iterator;

    abstract_group::successor_container successors(successors_from_parent);

    size_t children = child_count();

    typedef std::vector<server_node*, rt_pool_allocator<abstract_synth*> > sequential_child_list;
    sequential_child_list sequential_children;
    sequential_children.reserve(child_synths_);

    for (r_iterator it = child_nodes.rbegin();
         it != child_nodes.rend(); ++it)
    {
        server_node & node = *it;

        if (node.is_synth()) {
            r_iterator end_of_node = it;
            --end_of_node; // one element behind the last
            std::size_t node_count = 1;

            // we fill the child nodes in reverse order to an array
            for(;;)
            {
                sequential_children.push_back(&*it);
                ++it;
                if (it == child_nodes.rend())
                    break; // we found the beginning of this group

                if (!it->is_synth())
                    break; // we hit a child group, later we may want to add it's nodes, too?
                ++node_count;
            }

            --it; // we iterated one element too far, so we need to go back to the previous element
            assert(sequential_children.size() == node_count);

            sequential_child_list::reverse_iterator seq_it = sequential_children.rbegin();

            int activation_limit = get_previous_activation_count(it, child_nodes.rend(), previous_activation_limit);

            thread_queue_item * q_item =
                queue.allocate_queue_item(queue_node(static_cast<abstract_synth*>(*seq_it++), node_count),
                                          successors, activation_limit);

            queue_node & q_node = q_item->get_job();

            // now we can add all nodes sequentially
            for(;seq_it != sequential_children.rend(); ++seq_it)
                q_node.add_node(static_cast<abstract_synth*>(*seq_it));
            sequential_children.clear();

            assert(q_node.size() == node_count);

            /* advance successor list */
            successors = abstract_group::successor_container(1);
            successors[0] = q_item;

            if (activation_limit == 0)
                queue.add_initially_runnable(q_item);
            children -= node_count;
        }
        else {
            abstract_group & grp = static_cast<abstract_group&>(node);

            if (grp.has_synth_children())
            {
                int activation_limit = get_previous_activation_count(it, child_nodes.rend(), previous_activation_limit);
                successors = grp.fill_queue_recursive(queue, successors, activation_limit);
            }

            children -= 1;
        }
    }
    assert(children == 0);
    return successors;
}

abstract_group::successor_container
parallel_group::fill_queue_recursive(thread_queue & queue,
                                     abstract_group::successor_container const & successors,
                                     int activation_limit)
{
    assert (has_synth_children());
    std::vector<thread_queue_item*, rt_pool_allocator<void*> > collected_nodes;
    collected_nodes.reserve(child_synths_ + child_groups_ * 16); // pessimize

    for (server_node_list::iterator it = child_nodes.begin();
         it != child_nodes.end(); ++it)
    {
        server_node & node = *it;

        if (node.is_synth()) {
            thread_queue_item * q_item = queue.allocate_queue_item(queue_node(static_cast<abstract_synth*>(&node)),
                                                                   successors, activation_limit);

            if (activation_limit == 0)
                queue.add_initially_runnable(q_item);

            collected_nodes.push_back(q_item);
        }
        else {
            abstract_group & grp = static_cast<abstract_group&>(node);

            if (grp.has_synth_children())
            {
                successor_container group_successors =
                    grp.fill_queue_recursive(queue, successors, activation_limit);

                for (unsigned int i = 0; i != group_successors.size(); ++i)
                    collected_nodes.push_back(group_successors[i]);
            }
        }
    }

    successor_container ret(collected_nodes.size());

    memcpy(&ret[0], &collected_nodes[0], collected_nodes.size() * sizeof(thread_queue_item *));
/*    for (std::size_t i = 0; i != collected_nodes.size(); ++i)
        ret[i] = collected_nodes[i];*/
    return ret;
}

int parallel_group::tail_nodes(void) const
{
    int ret = child_synths_;
    for(group_list::const_iterator it = child_groups.begin();
        it != child_groups.end(); ++it)
    {
        ret += it->tail_nodes();
    }
    return ret;
}

void group::add_child(server_node * node, node_position_constraint const & constraint)
{
    assert (not has_child(node));

    server_node * ref = constraint.first;
    node_position position = constraint.second;

    assert ((position == after) ||
            (position == before) ||
            (position == insert));

    server_node_list::const_iterator it = child_nodes.iterator_to(*ref);

    if (position == after)
        ++it;

    child_nodes.insert(it, *node);
    node->set_parent(this);
}

void parallel_group::add_child(server_node * node, node_position_constraint const & constraint)
{
    abstract_group::add_child(node);
    /* later warn about this! */
}

void group::add_child(server_node * node, node_position position)
{
    assert (not has_child(node));

    if (position == head) {
        child_nodes.push_front(*node);
    }
    else {
        assert(position == tail);
        child_nodes.push_back(*node);
    }
    node->set_parent(this);
}

void parallel_group::add_child(server_node * node, node_position position)
{
    abstract_group::add_child(node);
    /* later warn about this! */
}

void * server_node::allocate(std::size_t size)
{
    return pool.malloc(size);
}

void server_node::free(void * ptr)
{
    pool.free(ptr);
}

server_node::node_pool server_node::pool(true);


} /* namespace nova */
