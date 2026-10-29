/*
    SuperCollider real time audio synthesis system
    Copyright (c) 2002 James McCartney. All rights reserved.
    http://www.audiosynth.com

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
*/

#pragma once

// This file contains the forward declarations of all the static plugin (un)load functions.
// If CMake is configured with STATIC_PLUGINS=ON, scsynth and Supernova call loadStaticPlugins()
// and unloadStaticPlugins() to load resp. unload all statically linked plugins.

#ifdef STATIC_PLUGINS

struct InterfaceTable;

void IO_Load(InterfaceTable* table);
void Osc_Load(InterfaceTable* table);
void Delay_Load(InterfaceTable* table);
void BinaryOp_Load(InterfaceTable* table);
void Filter_Load(InterfaceTable* table);
void Gendyn_Load(InterfaceTable* table);
void LF_Load(InterfaceTable* table);
void Noise_Load(InterfaceTable* table);
void MulAdd_Load(InterfaceTable* table);
void Grain_Load(InterfaceTable* table);
void Pan_Load(InterfaceTable* table);
void Reverb_Load(InterfaceTable* table);
void Trigger_Load(InterfaceTable* table);
void UnaryOp_Load(InterfaceTable* table);
#    ifndef NO_LIBSNDFILE
void DiskIO_Load(InterfaceTable* table);
#    endif
#    ifndef NO_X11
void UIUGens_Load(InterfaceTable* table);
#    endif
void Test_Load(InterfaceTable* table);
void PhysicalModeling_Load(InterfaceTable* table);
void Demand_Load(InterfaceTable* table);
void DynNoise_Load(InterfaceTable* table);
void FFT_UGens_Load(InterfaceTable* table);
void iPhone_Load(InterfaceTable* table);

#    ifndef NO_LIBSNDFILE
void DiskIO_Unload();
#    endif
#    ifndef NO_X11
void UIUGens_Unload();
#    endif

void loadStaticPlugins(InterfaceTable* table) {
    IO_Load(table);
    Osc_Load(table);
    Delay_Load(table);
    BinaryOp_Load(table);
    Filter_Load(table);
    Gendyn_Load(table);
    LF_Load(table);
    Noise_Load(table);
    MulAdd_Load(table);
    Grain_Load(table);
    Pan_Load(table);
    Reverb_Load(table);
    Trigger_Load(table);
    UnaryOp_Load(table);
#    ifndef NO_LIBSNDFILE
    DiskIO_Load(table);
#    endif
#    ifndef NO_X11
    UIUGens_Load(table);
#    endif
    PhysicalModeling_Load(table);
    Test_Load(table);
    Demand_Load(table);
    DynNoise_Load(table);
#    if defined(SC_IPHONE) && !TARGET_IPHONE_SIMULATOR
    iPhone_Load(table);
#    endif
    FFT_UGens_Load(table);
}

void unloadStaticPlugins() {
#    ifndef NO_LIBSNDFILE
    DiskIO_Unload();
#    endif
#    ifndef NO_X11
    UIUGens_Unload();
#    endif
}

#endif // STATIC_PLUGINS
