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

#include "dependency_graph_generator.hpp"
#include "dsp_thread_queue_node.hpp"
#include "node_graph.hpp"
#include "synth.hpp"
#include "utilities/exists.hpp"

#include "dsp_thread_queue/dsp_thread_queue.hpp"

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

node_graph::dsp_thread_queue_ptr node_graph::generate_dsp_queue(void)
{
    dependency_graph_generator gen;

    node_graph::dsp_thread_queue * ret = gen(this);
    return dsp_thread_queue_ptr(ret);
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

int32_t node_graph::generate_node_id(void)
{
    int32_t new_id;
    do {
        for (;;) {
            new_id = -std::abs<int32_t>(server_node::hash(generated_id));
            if (likely (new_id != generated_id))
                break;

            generated_id -= 1;
        }

    } while (!node_id_available(new_id));
    generated_id = new_id;
    return generated_id;
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

void abstract_group::set(slot_index_t slot_id, float val)
{
    for(server_node_list::iterator it = child_nodes.begin();
        it != child_nodes.end(); ++it)
        it->set(slot_id, val);
}

void abstract_group::set(const char * slot_str, float val)
{
    size_t hashed_str = string_hash(slot_str);
    for(server_node_list::iterator it = child_nodes.begin();
        it != child_nodes.end(); ++it)
        it->set(slot_str, hashed_str, val);
}

void abstract_group::set(const char * slot_str, std::size_t hashed_str, float val)
{
    for(server_node_list::iterator it = child_nodes.begin();
        it != child_nodes.end(); ++it)
        it->set(slot_str, hashed_str, val);
}


void abstract_group::set_control_array(const char * slot_str, size_t count, float * val)
{
    size_t hashed_str = string_hash(slot_str);
    set_control_array(slot_str, hashed_str, count, val);
}

void abstract_group::set_control_array(const char * slot_str, std::size_t hashed_str, size_t count, float * val)
{
    for(server_node_list::iterator it = child_nodes.begin(); it != child_nodes.end(); ++it)
        it->set_control_array(slot_str, hashed_str, count, val);
}

void abstract_group::set_control_array(slot_index_t slot_id, size_t count, float * val)
{
    for(server_node_list::iterator it = child_nodes.begin(); it != child_nodes.end(); ++it)
        it->set_control_array(slot_id, count, val);
}


void abstract_group::set_control_array_element(const char * slot_str, size_t index, float val)
{
    size_t hashed_str = string_hash(slot_str);
    set_control_array_element(slot_str, hashed_str, index, val);
}

void abstract_group::set_control_array_element(const char * slot_str, std::size_t hashed_str, size_t index, float val)
{
    for(server_node_list::iterator it = child_nodes.begin(); it != child_nodes.end(); ++it)
        it->set_control_array_element(slot_str, hashed_str, index, val);
}

void abstract_group::set_control_array_element(slot_index_t slot_id, size_t index, float val)
{
    for(server_node_list::iterator it = child_nodes.begin(); it != child_nodes.end(); ++it)
        it->set_control_array_element(slot_id, index, val);
}


int parallel_group::tail_nodes(void) const
{
    int ret = child_synth_count;
    for(group_list::const_iterator it = child_groups.begin(); it != child_groups.end(); ++it)
        ret += it->tail_nodes();

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
    assert((position == head) || (position == tail));

    if (position == head)
        child_nodes.push_front(*node);
    else
        child_nodes.push_back(*node);
    node->set_parent(this);
}

void parallel_group::add_child(server_node * node, node_position position)
{
    abstract_group::add_child(node);
    /* later warn about this! */
}

} /* namespace nova */
