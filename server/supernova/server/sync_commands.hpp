//  osc responder classes for the nova server
//  Copyright (C) 2008 Tim Blechmann
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

#include "server_scheduler.hpp"

namespace nova {

class set_cmd_index : public audio_sync_callback {
public:
    set_cmd_index(int node_id, slot_index_t const& slot_id, float value):
        node_id(node_id),
        slot_id(slot_id),
        value(value) {}

private:
    virtual void run(void) override { instance->set_node_slot(node_id, slot_id, value); }

    const int node_id;
    const slot_index_t slot_id;
    const float value;
};

class set_cmd_str : public audio_sync_callback {
public:
    set_cmd_str(int node_id, const char* name, float value): node_id(node_id), value(value) {
        slot_id = (char*)allocate(strlen(name) * sizeof(char));
        strcpy(slot_id, name);
    }

    ~set_cmd_str(void) { deallocate(slot_id); }

private:
    virtual void run(void) override { instance->set_node_slot(node_id, slot_id, value); }

    const int node_id;
    char* slot_id;
    const float value;
};

}
