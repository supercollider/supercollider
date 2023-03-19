/*
    Onset detector for SuperCollider
    Copyright (c) 2007 Dan Stowell. All rights reserved.
    http://onsetsds.sourceforge.net

    Now part of:

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

#include "onsetsds.h"

struct Onsets : Unit {
    float outval;
    float* m_odsdata;
    OnsetsDS* m_ods;

    bool m_needsinit;
};

extern "C" {
void Onsets_Ctor(Onsets* unit);
void Onsets_next(Onsets* unit, int inNumSamples);
void Onsets_next_rawodf(Onsets* unit, int inNumSamples);
void Onsets_Dtor(Onsets* unit);
}
