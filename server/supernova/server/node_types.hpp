//  node types
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

#pragma once

#include <boost/detail/atomic_count.hpp>
#include <boost/intrusive/list.hpp>
#include <boost/intrusive/unordered_set.hpp>

#include "memory_pool.hpp"
#include "synth_definition.hpp"
#include "utilities/static_pool.hpp"

namespace nova {

class server_node;

class abstract_group;
class group;

namespace bi = boost::intrusive;

typedef boost::intrusive::list<class server_node, boost::intrusive::constant_time_size<false>> server_node_list;

class server_node : public bi::list_base_hook<bi::link_mode<bi::auto_unlink>>, /* group member */
                    public bi::unordered_set_base_hook<bi::link_mode<bi::auto_unlink>> /* for node_id mapping */
{
protected:
    server_node(int32_t node_id, bool type): node_id(node_id), node_is_synth(type), use_count_(0) {}

    virtual ~server_node(void) { assert(parent_ == 0); }

    /* @{ */
    /** node id handling */
    void reset_id(int32_t new_id) { node_id = new_id; }

public:
    int32_t node_id;

    int32_t id(void) const { return node_id; }
    /* @} */

    typedef bi::list_base_hook<bi::link_mode<bi::auto_unlink>> parent_hook;

    /* @{ */
    /** node_id mapping */
    friend std::size_t hash_value(server_node const& that) { return hash(that.id()); }

    static int32_t hash(int32_t id) {
        return id * 2654435761U; // knuth hash, 32bit should be enough
    }

    friend bool operator<(server_node const& lhs, server_node const& rhs) { return lhs.node_id < rhs.node_id; }

    friend bool operator==(server_node const& lhs, server_node const& rhs) { return lhs.node_id == rhs.node_id; }
    /* @} */

    bool is_synth(void) const { return node_is_synth; }

    bool is_group(void) const { return !node_is_synth; }

    /* @{ */
    /* set a scalar slot */
    virtual void set(slot_index_t slot_id, float val) = 0;
    virtual void set(const char* slot_str, float val) = 0;
    virtual void set(const char* slot_str, size_t hashed_str, float val) = 0;
    /* @} */

    /* @{ */
    /* set an arrayed slot from array */
    virtual void set_control_array(slot_index_t slot_str, size_t n, float* values) = 0;
    virtual void set_control_array(const char* slot_str, size_t n, float* values) = 0;
    virtual void set_control_array(const char* slot_str, size_t hashed_str, size_t n, float* values) = 0;
    /* @} */

    /* @{ */
    /* set an element of an arrayed slot */
    virtual void set_control_array_element(slot_index_t slot_str, size_t n, float values) = 0;
    virtual void set_control_array_element(const char* slot_str, size_t n, float values) = 0;
    virtual void set_control_array_element(const char* slot_str, size_t hashed_str, size_t n, float values) = 0;
    /* @} */

    /* @{ */
    /** group traversing */
    inline const server_node* previous_node(void) const;
    inline server_node* previous_node(void);
    inline const server_node* next_node(void) const;
    inline server_node* next_node(void);
    /* @} */

private:
    const bool node_is_synth;

    /** support for pausing node */
    /* @{ */
    bool running_ = true;

public:
    virtual void pause(void) { running_ = false; }

    virtual void resume(void) { running_ = true; }

    bool is_running(void) const { return running_; }
    /* @} */

private:
    friend class node_graph;
    friend class abstract_group;
    friend class group;
    friend class parallel_group;


public:
    /* @{ */
    /** parent group handling */
    const abstract_group* get_parent(void) const { return parent_; }

    abstract_group* get_parent(void) { return parent_; }

    inline void set_parent(abstract_group* parent);
    inline void clear_parent(void);
    /* @} */

private:
    abstract_group* parent_ = nullptr;

public:
    /* memory management for server_nodes */
    /* @{ */
    inline void* operator new(std::size_t size) { return rt_pool.malloc(size); }

    inline void operator delete(void* p) { rt_pool.free(p); }
    /* @} */

public:
    /* refcountable */
    /* @{ */
    void add_ref(void) { ++use_count_; }

    void release(void) {
        if (unlikely(--use_count_ == 0))
            delete this;
    }

private:
    boost::detail::atomic_count use_count_;
    /* @} */
};

inline void intrusive_ptr_add_ref(server_node* p) { p->add_ref(); }

inline void intrusive_ptr_release(server_node* p) { p->release(); }

enum node_position {
    head = 0,
    tail = 1,
    before = 2,
    after = 3,
    replace = 4,
    insert = 5 /* for pgroups */
};

typedef std::pair<server_node*, node_position> node_position_constraint;


} /* namespace nova */
