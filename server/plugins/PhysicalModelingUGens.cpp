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

// some basic physical modeling ugens - julian rohrhuber 1/04
// these are very simple implementations with cartoonification aspects.


#include "SC_PlugIn.h"

static InterfaceTable* ft;

//////////////////////////////////////////////////////////////////////////////////////////////////


struct Spring : public Unit {
    float m_pos;
    float m_vel;
};

struct Ball : public Unit {
    float m_pos;
    float m_vel;
    float m_prev;
};

struct TBall : public Unit {
    double m_pos;
    float m_vel;
    double m_prev;
};


extern "C" {

void Spring_Ctor(Spring* unit);
void Spring_next(Spring* unit, int inNumSamples);

// void Friction_Ctor(Friction *unit);
// void Friction_next(Friction *unit, int inNumSamples);

void Ball_Ctor(Ball* unit);
void Ball_next(Ball* unit, int inNumSamples);

void TBall_Ctor(TBall* unit);
void TBall_next(TBall* unit, int inNumSamples);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////


void Spring_Ctor(Spring* unit) {
    SETCALC(Spring_next);
    unit->m_vel = 0.f;
    unit->m_pos = 0.f;
    Spring_next(unit, 1);
}

// in, spring, damping

void Spring_next(Spring* unit, int inNumSamples) {
    float pos = unit->m_pos;
    float vel = unit->m_vel;
    float* out = ZOUT(0); // out force
    float* in = ZIN(0); // in force
    float spring = ZIN0(1); // spring constant
    float damping = 1.f - ZIN0(2); // damping
    float c = SAMPLEDUR;
    float rc = SAMPLERATE;
    spring = spring * c;
    LOOP1(inNumSamples, float force = ZXP(in) * c - pos * spring; vel = (force + vel) * damping; pos += vel;
          ZXP(out) = force * rc;);

    unit->m_pos = pos;
    unit->m_vel = vel;
}

//////////////////////////////////////////////////////////////////////////////////////////

void Ball_Ctor(Ball* unit) {
    SETCALC(Ball_next);
    unit->m_vel = 0.f;
    unit->m_pos = ZIN0(0);
    unit->m_prev = ZIN0(0);
    Ball_next(unit, 1);
}


void Ball_next(Ball* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* in = ZIN(0); // floor position
    float g_in = ZIN0(1); // gravity
    float damping = 1 - ZIN0(2); // damping
    float k = ZIN0(3); // friction

    float pos = unit->m_pos;
    float vel = unit->m_vel;
    float prev_floor = unit->m_prev;
    float c = SAMPLEDUR;
    float maxvel = c * 1000.f;
    float minvel = 0.f - maxvel;
    float inter = c * 1000.f;
    RGen& rgen = *unit->mParent->mRGen;
    float g = c * g_in;
    k = (double)k * (double)g_in; // stickyness proportional to gravity

    LOOP1(
        inNumSamples, float floor = ZXP(in); float floorvel; float dither; vel -= g; pos += vel;
        float dist = pos - floor; floorvel = floor - prev_floor; floorvel = sc_clip(floorvel, minvel, maxvel);
        float vel_diff = floorvel - vel; if (sc_abs(dist) < k) { // sticky friction: maybe vel dependant?
            if (sc_abs(dist) < (k * 0.005)) {
                vel = 0.f;
                pos = floor + g;
            } else {
                vel = vel_diff * inter + vel;
                pos = (floor - pos) * inter + pos;
            }

        } else if (dist <= 0.f) {
            pos = floor - dist;
            vel = vel_diff;
            vel *= damping;

            dither = rgen.frand() * 0.00005f * g_in; // dither to reduce jitter
            // if(sc_abs(dist) < 0.000001) { vel += dither; }
            vel += dither;
        } prev_floor = floor;
        ZXP(out) = pos;);

    unit->m_pos = pos;
    unit->m_vel = vel;
    unit->m_prev = prev_floor;
}


//////////////////////////////////////////////////////////////////////////////////////////

void TBall_Ctor(TBall* unit) {
    SETCALC(TBall_next);
    unit->m_vel = 0.f;
    unit->m_pos = ZIN0(0);
    unit->m_prev = ZIN0(0);
    TBall_next(unit, 1);
}

void TBall_next(TBall* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* in = ZIN(0); // floor position
    float g_in = ZIN0(1); // gravity
    float damping = 1 - ZIN0(2); // damping
    float k = ZIN0(3); // friction

    double pos = unit->m_pos;
    float vel = unit->m_vel;
    double prev_floor = unit->m_prev;
    float c = SAMPLEDUR;
    float maxvel = c * 1000.f;
    float minvel = 0.f - maxvel;
    float inter = c * 10000.f;
    RGen& rgen = *unit->mParent->mRGen;
    float g = c * g_in;
    k = (double)k * (double)g_in; // stickyness proportional to gravity

    LOOP1(
        inNumSamples, double floor = ZXP(in); float floorvel; float outval = 0.f; float dither; vel -= g; pos += vel;
        double dist = pos - floor; floorvel = floor - prev_floor; floorvel = sc_clip(floorvel, minvel, maxvel);
        float vel_diff = floorvel - vel; if (sc_abs(dist) < k) { // sticky friction: vel dependant?
            if (sc_abs(dist) < (k * 0.005)) {
                vel = 0.f;
                pos = floor + g;
            } else {
                vel = vel_diff * inter + vel;
                pos = (floor - pos) * inter + pos;
            }
        } else if (dist <= 0.f) {
            pos = floor - dist;
            vel = floorvel - vel;
            vel *= damping;
            outval = vel;
            dither = rgen.frand() * 0.001f * g_in; // dither to reduce sampling jitter
            // if(sc_abs(dist) < 0.003) { vel += dither; }
            vel += dither;
        } prev_floor = floor;
        ZXP(out) = outval;);

    unit->m_pos = pos;
    unit->m_vel = vel;
    unit->m_prev = prev_floor;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////

PluginLoad(PhysicalModeling) {
    ft = inTable;

    DefineSimpleUnit(Spring);
    DefineSimpleUnit(Ball);
    DefineSimpleUnit(TBall);
}
