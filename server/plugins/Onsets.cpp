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

#include "SC_PlugIn.h"
#include "SCComplex.h"
#include "FFT_UGens.h"

#include "Onsets.h"

//////////////////////////////////////////////////////////////////////////////////////////////////

// for operation on one buffer
// almost like PV_GET_BUF except it outputs unit->outval rather than -1 when FFT not triggered
#define Onsets_GET_BUF                                                                                                 \
    float fbufnum = ZIN0(0);                                                                                           \
    if (fbufnum < 0.f) {                                                                                               \
        ZOUT0(0) = unit->outval;                                                                                       \
        return;                                                                                                        \
    }                                                                                                                  \
    ZOUT0(0) = fbufnum;                                                                                                \
    uint32 ibufnum = (uint32)fbufnum;                                                                                  \
    World* world = unit->mWorld;                                                                                       \
    SndBuf* buf;                                                                                                       \
    if (ibufnum >= world->mNumSndBufs) {                                                                               \
        int localBufNum = ibufnum - world->mNumSndBufs;                                                                \
        Graph* parent = unit->mParent;                                                                                 \
        if (localBufNum <= parent->localBufNum) {                                                                      \
            buf = parent->mLocalSndBufs + localBufNum;                                                                 \
        } else {                                                                                                       \
            buf = world->mSndBufs;                                                                                     \
        }                                                                                                              \
    } else {                                                                                                           \
        buf = world->mSndBufs + ibufnum;                                                                               \
    }                                                                                                                  \
    LOCK_SNDBUF(buf);


//////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////////

void Onsets_Ctor(Onsets* unit) {
    if (ZIN0(8) > 0)
        SETCALC(Onsets_next_rawodf);
    else
        SETCALC(Onsets_next);

    unit->m_needsinit = true;
    unit->m_ods = (OnsetsDS*)RTAlloc(unit->mWorld, sizeof(OnsetsDS));

    ZOUT0(0) = unit->outval = 0.f;
}

void Onsets_next(Onsets* unit, int inNumSamples) {
    Onsets_GET_BUF

        // In practice, making the polar conversion here in SC is more efficient because SC provides a lookup table
        // method.
        SCPolarBuf* p = ToPolarApx(buf);

    OnsetsDS* ods = unit->m_ods;

    int odftype = (int)ZIN0(2);
    float relaxtime = ZIN0(3);
    int medspan = (int)ZIN0(6);

    if (unit->m_needsinit) {
        // Init happens here because we need to be sure about FFT size.
        unit->m_odsdata = (float*)RTAlloc(unit->mWorld, onsetsds_memneeded(odftype, buf->samples, medspan));

        onsetsds_init(ods, unit->m_odsdata, ODS_FFT_SC3_POLAR, odftype, buf->samples, medspan, FULLRATE);
        onsetsds_setrelax(ods, relaxtime, buf->samples >> 1);

        unit->m_needsinit = false;
    }

    // Here is the best place to set parameters - after init is ensured
    // These are "painless" to set:
    ods->thresh = ZIN0(1);
    ods->floor = ZIN0(4);
    ods->mingap = (int)ZIN0(5);
    ods->whtype = (int)ZIN0(7);

    // Now to process
    unit->outval = onsetsds_process(ods, (float*)p);

    ZOUT0(0) = unit->outval;
}

void Onsets_next_rawodf(Onsets* unit, int inNumSamples) {
    Onsets_GET_BUF

        // In practice, making the polar conversion here in SC is more efficient because SC provides a lookup table
        // method.
        SCPolarBuf* p = ToPolarApx(buf);

    OnsetsDS* ods = unit->m_ods;

    int odftype = (int)ZIN0(2);
    float relaxtime = ZIN0(3);
    int medspan = (int)ZIN0(6);

    if (unit->m_needsinit) {
        // Init happens here because we need to be sure about FFT size.
        unit->m_odsdata = (float*)RTAlloc(unit->mWorld, onsetsds_memneeded(odftype, buf->samples, medspan));

        onsetsds_init(ods, unit->m_odsdata, ODS_FFT_SC3_POLAR, odftype, buf->samples, medspan, FULLRATE);
        onsetsds_setrelax(ods, relaxtime, buf->samples >> 1);

        unit->m_needsinit = false;
    }

    // Here is the best place to set parameters - after init is ensured
    // These are "painless" to set:
    ods->thresh = ZIN0(1);
    ods->floor = ZIN0(4);
    ods->mingap = (int)ZIN0(5);
    ods->whtype = (int)ZIN0(7);

    // Now to process
    onsetsds_process(ods, (float*)p);
    // But we want the ODF, not the triggers, for this special mode...
    // unit->outval = ods->odfvalpost;
    unit->outval = ods->odfvals[0];

    ZOUT0(0) = unit->outval;
}

void Onsets_Dtor(Onsets* unit) {
    if (!unit->m_needsinit) {
        RTFree(unit->mWorld, unit->m_odsdata);
    }
    RTFree(unit->mWorld, unit->m_ods);
}
