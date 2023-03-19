//  dsp thread queue nodes
//  Copyright (C) 2008-2015 Tim Blechmann
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

#include <cstdint>
#include <memory>
#include <vector>


#include "server/synth.hpp"
#include "../sc/sc_synth.hpp"
#include "function_attributes.h"

namespace nova {

/* optimized for sc_synth, since we don't support other types of synths for now */
class queue_node_data {
    typedef std::uint_fast8_t thread_count_type;

public:
    explicit queue_node_data(abstract_synth* node): node(static_cast<sc_synth*>(node)) {}

    explicit queue_node_data(queue_node_data const& rhs) = default;
    explicit queue_node_data(queue_node_data&& rhs) = default;

    void operator()(thread_count_type index) {
        if (unlikely(!node->is_running()))
            return;
        node->perform();
    }

private:
    intrusive_ptr<sc_synth> node;
    template <typename Alloc> friend class dsp_queue_node;
};

/* implements runnable concept */
template <typename Alloc = std::allocator<queue_node_data>> class dsp_queue_node {
    typedef std::vector<queue_node_data, typename Alloc::template rebind<queue_node_data>::other> node_container;

    typedef std::uint_fast16_t node_count_type;
    typedef std::uint_fast8_t thread_count_type;

public:
    explicit dsp_queue_node(queue_node_data&& node): first(std::move(node)) {}

    dsp_queue_node(queue_node_data&& node, std::size_t container_size): first(std::move(node)) {
        nodes.reserve(container_size - 1);
    }

    HOT void operator()(thread_count_type thread_index) {
        first(thread_index);

        int remaining = node_count;
        if (remaining == 0)
            return; // fast-path

        queue_node_data* __restrict__ data = nodes.data();

        if (remaining & 1) {
            (*data)(thread_index);

            if (remaining == 1)
                return;
            remaining -= 1;
            data += 1;
        }

        if (remaining & 2) {
            (*data)(thread_index);
            (*(data + 1))(thread_index);

            if (remaining == 2)
                return;
            remaining -= 2;
            data += 2;
        }

        if (remaining & 4) {
            (*data)(thread_index);
            (*(data + 1))(thread_index);
            (*(data + 2))(thread_index);
            (*(data + 3))(thread_index);

            if (remaining == 4)
                return;
            remaining -= 4;
            data += 4;
        }

        assert(remaining >= 8);
        assert((remaining % 8) == 0);

        for (;;) {
            (*data)(thread_index);
            (*(data + 1))(thread_index);
            (*(data + 2))(thread_index);
            (*(data + 3))(thread_index);
            (*(data + 4))(thread_index);
            (*(data + 5))(thread_index);
            (*(data + 6))(thread_index);
            (*(data + 7))(thread_index);

            if (remaining == 8)
                return;
            remaining -= 8;
            data += 8;
        }
    }

    void add_node(abstract_synth* node) {
        nodes.emplace_back(std::move(queue_node_data(node)));
        ++node_count;
    }

    node_count_type size(void) const { return node_count + 1; }

private:
    queue_node_data first;
    node_container nodes;
    node_count_type node_count = 0;
};

} /* namespace nova */
