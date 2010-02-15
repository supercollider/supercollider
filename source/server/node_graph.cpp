//  node graph, implementation
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
    node_graph::dsp_thread_queue * ret = new node_graph::dsp_thread_queue();

    root_group_.fill_queue(*ret);
    return std::auto_ptr<node_graph::dsp_thread_queue>(ret);
}


void group::fill_queue(thread_queue & queue)
{
    successor_container successors;

    fill_queue_recursive(queue, successors, 0);
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

bool abstract_group::has_child(server_node * node)
{
    if (!node->server_node::parent_hook::is_linked())
        return false;

    /* find node in container */
    for (server_node_list::iterator it = child_nodes.begin();
         it != child_nodes.end(); ++it) {
        if (&*it == node)
            return true;
    }
    return false;
}

void abstract_group::remove_child(server_node * node)
{
    assert (has_child(node));
/*         child_nodes.remove(&*node); */
    node->server_node::parent_hook::unlink();
    node->clear_parent();
}

void abstract_group::set(const char * slot_str, float val)
{
    for(server_node_list::iterator it = child_nodes.begin();
        it != child_nodes.end(); ++it)
        it->set(slot_str, val);
}

void abstract_group::set(slot_index_t slot_id, float val)
{
    for(server_node_list::iterator it = child_nodes.begin();
        it != child_nodes.end(); ++it)
        it->set(slot_id, val);
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
                            abstract_group::successor_container successors,
                            int previous_activation_limit)
{
    size_t children = child_count();
    for (server_node_list::reverse_iterator it = child_nodes.rbegin();
         it != child_nodes.rend(); ++it)
    {
        server_node & node = *it;

        if (node.is_synth()) {
            dsp_queue_node q_node(children); // we reserve space for all children to avoid hitting the allocator too often
            q_node.add_node(static_cast<synth*>(&node));

            server_node_list::reverse_iterator synth_it = it;

            for(;;)
            {
                ++synth_it;
                if (synth_it == child_nodes.rend())
                    break; // we found the beginning of this group

                server_node & prev_node = *synth_it;
                if (!prev_node.is_synth())
                    break; // we hit a child group, later we may want to add it's nodes, too?
                q_node.add_node(static_cast<synth*>(&prev_node));

                it = synth_it; // now we consumed this node and can continue
            }

            int activation_limit = get_previous_activation_count(it, child_nodes.rend(), previous_activation_limit);

            thread_queue_item * q_item = new thread_queue_item(q_node, successors, activation_limit);

            queue.add_queue_item(q_item);

            if (successors.empty())
                successors.push_back(q_item);
            else {
                successors[0] = q_item;
                successors.resize(1);
            }

            if (activation_limit == 0)
                queue.add_initially_runnable(q_item);
            children -= q_node.size();
        }
        else {
            int activation_limit = get_previous_activation_count(it, child_nodes.rend(), previous_activation_limit);

            abstract_group & grp = static_cast<abstract_group&>(node);
            successors = grp.fill_queue_recursive(queue, successors, activation_limit);
            children -= 1;
        }
    }
    assert(children == 0);
    return successors;
}

abstract_group::successor_container
parallel_group::fill_queue_recursive(thread_queue & queue,
                                     abstract_group::successor_container successors,
                                     int activation_limit)
{
    if (child_nodes.empty())
        return successors;

    successor_container ret;

    for (server_node_list::iterator it = child_nodes.begin();
         it != child_nodes.end(); ++it)
    {
        server_node & node = *it;

        if (node.is_synth()) {
            dsp_queue_node q_node(1);
            q_node.add_node(static_cast<synth*>(&node));

            thread_queue_item * q_item = new thread_queue_item(q_node, successors, activation_limit);

            queue.add_queue_item(q_item);

            if (activation_limit == 0)
                queue.add_initially_runnable(q_item);

            ret.push_back(q_item);
        }
        else {
            abstract_group & grp = static_cast<abstract_group&>(node);
            successor_container group_successors =
                grp.fill_queue_recursive(queue, successors, activation_limit);

            for (unsigned int i = 0; i != group_successors.size(); ++i)
                ret.push_back(group_successors[i]);
        }
    }
    return ret;
}

int parallel_group::tail_nodes(void) const
{
    int ret = 0;
    for(server_node_list::const_iterator it = child_nodes.begin();
        it != child_nodes.end(); ++it)
    {
        const server_node & node = *it;

        if (node.is_synth()) {
            ret += 1;
        } else {
            abstract_group const & grp = static_cast<abstract_group const &>(node);
            ret += grp.tail_nodes();
        }
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
