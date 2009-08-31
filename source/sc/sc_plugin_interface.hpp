//  interface for supercollider plugins
//  Copyright (C) 2009 Tim Blechmann
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

#ifndef SC_PLUGIN_INTERFACE_HPP
#define SC_PLUGIN_INTERFACE_HPP

#include "../server/audio_bus_manager.hpp"

#include "supercollider/Headers/plugin_interface/SC_InterfaceTable.h"
#include "supercollider/Headers/plugin_interface/SC_World.h"

namespace nova
{

class sc_plugin_interface
{
public:
    sc_plugin_interface(void);
    ~sc_plugin_interface(void);

    void set_audio_channels(int audio_inputs, int audio_outputs);

    InterfaceTable sc_interface;
    World world;

    audio_bus_manager audio_busses;
};

} /* namespace nova */

#endif /* SC_PLUGIN_INTERFACE_HPP */
