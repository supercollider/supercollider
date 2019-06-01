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


#include "SC_PlugIn.h"

#include <algorithm> /* for std::min and std::max */

#include <boost/align/is_aligned.hpp>

#include "simd_peakmeter.hpp"

#ifdef NOVA_SIMD
#    include "simd_memory.hpp"

#    include "function_attributes.h"

#endif

static InterfaceTable* ft;

//////////////////////////////////////////////////////////////////////////////////////////////////

struct Trig1 : public Unit {
    float m_prevtrig;
    long mCounter;
};

struct Trig : public Unit {
    float mLevel;
    float m_prevtrig;
    long mCounter;
};

struct SendTrig : public Unit {
    float m_prevtrig;
};

struct SendReply : public Unit {
    float m_prevtrig;
    int m_valueSize;
    int m_valueOffset;
    float* m_values;
    int m_cmdNameSize;
    char* m_cmdName;
};


struct Poll : public Unit {
    int m_samplesRemain, m_intervalSamples;
    float m_trig;
    float m_lastPoll;
    char* m_id_string;
    bool m_mayprint;
};


struct ToggleFF : public Unit {
    float mLevel;
    float m_prevtrig;
};

struct SetResetFF : public Unit {
    float mLevel;
    float m_prevtrig, m_prevreset;
};

struct Latch : public Unit {
    float mLevel;
    float m_prevtrig;
};

struct Gate : public Unit {
    float mLevel;
};

struct Schmidt : public Unit {
    float mLevel;
};

struct PulseDivider : public Unit {
    float mLevel;
    float m_prevtrig;
    long mCounter;
};

struct PulseCount : public Unit {
    float mLevel;
    float m_prevtrig, m_prevreset;
};

struct Stepper : public Unit {
    float mLevel;
    float m_prevtrig, m_prevreset;
};

struct TDelay : public Unit {
    long mCounter;
    float m_prevtrig;
};

struct ZeroCrossing : public Unit {
    float mLevel, m_prevfrac, m_previn;
    int32 mCounter;
};

struct Timer : public Unit {
    float mLevel, m_prevfrac, m_previn;
    int32 mCounter;
};

struct Sweep : public Unit {
    double mLevel;
    float m_previn;
};

struct Phasor : public Unit {
    double mLevel;
    float m_previn;
};

struct Peak : public Unit {
    float mLevel;
    float m_prevtrig;
};

struct RunningMin : public Unit {
    float mLevel;
    float m_prevtrig;
};

struct RunningMax : public Unit {
    float mLevel;
    float m_prevtrig;
};

struct PeakFollower : public Unit {
    float mLevel;
    float mDecay;
};

struct MostChange : public Unit {
    float mPrevA, mPrevB;
    int mRecent;
};

struct LeastChange : public Unit {
    float mPrevA, mPrevB;
    int mRecent;
};

struct LastValue : public Unit {
    float mPrev;
    float mCurr;
};


struct Done : public Unit {
    Unit* m_src;
};

struct FreeSelf : public Unit {
    float m_prevtrig;
};

struct PauseSelf : public Unit {
    float m_prevtrig;
};

struct Pause : public Unit {
    int m_state;
};

struct Free : public Unit {
    float m_prevtrig;
};

struct FreeSelfWhenDone : public Unit {
    Unit* m_src;
};

struct PauseSelfWhenDone : public Unit {
    Unit* m_src;
};

extern "C" {
void Trig1_Ctor(Trig1* unit);
void Trig1_next(Trig1* unit, int inNumSamples);
void Trig1_next_k(Trig1* unit, int inNumSamples);

void Trig_Ctor(Trig* unit);
void Trig_next(Trig* unit, int inNumSamples);
void Trig_next_k(Trig* unit, int inNumSamples);

void SendTrig_Ctor(SendTrig* unit);
void SendTrig_next(SendTrig* unit, int inNumSamples);
void SendTrig_next_aka(SendTrig* unit, int inNumSamples);

void SendReply_Ctor(SendReply* unit);
void SendReply_next(SendReply* unit, int inNumSamples);
void SendReply_next_aka(SendReply* unit, int inNumSamples);

void Poll_Ctor(Poll* unit);
void Poll_next_aa(Poll* unit, int inNumSamples);
void Poll_next_ak(Poll* unit, int inNumSamples);
void Poll_next_kk(Poll* unit, int inNumSamples);

void SetResetFF_Ctor(SetResetFF* unit);
void SetResetFF_next_a(SetResetFF* unit, int inNumSamples);
void SetResetFF_next_k(SetResetFF* unit, int inNumSamples);

void ToggleFF_Ctor(ToggleFF* unit);
void ToggleFF_next(ToggleFF* unit, int inNumSamples);

void Latch_Ctor(Latch* unit);
void Latch_next_ak(Latch* unit, int inNumSamples);
void Latch_next_aa(Latch* unit, int inNumSamples);

void Gate_Ctor(Gate* unit);
void Gate_next_ak(Gate* unit, int inNumSamples);
void Gate_next_aa(Gate* unit, int inNumSamples);

void Schmidt_Ctor(Schmidt* unit);
void Schmidt_next(Schmidt* unit, int inNumSamples);

void PulseDivider_Ctor(PulseDivider* unit);
void PulseDivider_next(PulseDivider* unit, int inNumSamples);

void PulseCount_Ctor(PulseCount* unit);
void PulseCount_next_a(PulseCount* unit, int inNumSamples);
void PulseCount_next_k(PulseCount* unit, int inNumSamples);
void PulseCount_next_0(PulseCount* unit, int inNumSamples);

void Stepper_Ctor(Stepper* unit);
void Stepper_next_aa(Stepper* unit, int inNumSamples);
void Stepper_next_ak(Stepper* unit, int inNumSamples);
void Stepper_next_a0(Stepper* unit, int inNumSamples);

void TDelay_Ctor(TDelay* unit);
void TDelay_next(TDelay* unit, int inNumSamples);

void ZeroCrossing_Ctor(ZeroCrossing* unit);
void ZeroCrossing_next_a(ZeroCrossing* unit, int inNumSamples);

void Timer_Ctor(Timer* unit);
void Timer_next_a(Timer* unit, int inNumSamples);

void Sweep_Ctor(Sweep* unit);
void Sweep_next_0k(Sweep* unit, int inNumSamples);
void Sweep_next_0a(Sweep* unit, int inNumSamples);
void Sweep_next_kk(Sweep* unit, int inNumSamples);
void Sweep_next_ka(Sweep* unit, int inNumSamples);
void Sweep_next_ak(Sweep* unit, int inNumSamples);
void Sweep_next_aa(Sweep* unit, int inNumSamples);

void Phasor_Ctor(Phasor* unit);
void Phasor_next_kk(Phasor* unit, int inNumSamples);
void Phasor_next_ak(Phasor* unit, int inNumSamples);
void Phasor_next_aa(Phasor* unit, int inNumSamples);

void Peak_Ctor(Peak* unit);
void Peak_next_ak(Peak* unit, int inNumSamples);
void Peak_next_ai(Peak* unit, int inNumSamples);
void Peak_next_aa(Peak* unit, int inNumSamples);
void Peak_next_ak_k(Peak* unit, int inNumSamples);
void Peak_next_ai_k(Peak* unit, int inNumSamples);
void Peak_next_aa_k(Peak* unit, int inNumSamples);

void RunningMin_Ctor(RunningMin* unit);
void RunningMin_next_ak(RunningMin* unit, int inNumSamples);
void RunningMin_next_ai(RunningMin* unit, int inNumSamples);
void RunningMin_next_aa(RunningMin* unit, int inNumSamples);

void RunningMax_Ctor(RunningMax* unit);
void RunningMax_next_ak(RunningMax* unit, int inNumSamples);
void RunningMax_next_ai(RunningMax* unit, int inNumSamples);
void RunningMax_next_aa(RunningMax* unit, int inNumSamples);


void PeakFollower_Ctor(PeakFollower* unit);
void PeakFollower_next(PeakFollower* unit, int inNumSamples);
void PeakFollower_next_ai(PeakFollower* unit, int inNumSamples);

void MostChange_Ctor(MostChange* unit);
void MostChange_next_ak(MostChange* unit, int inNumSamples);
void MostChange_next_ka(MostChange* unit, int inNumSamples);
void MostChange_next_aa(MostChange* unit, int inNumSamples);

void LeastChange_Ctor(LeastChange* unit);
void LeastChange_next_ak(LeastChange* unit, int inNumSamples);
void LeastChange_next_ka(LeastChange* unit, int inNumSamples);
void LeastChange_next_aa(LeastChange* unit, int inNumSamples);

void LastValue_Ctor(LastValue* unit);
void LastValue_next_ak(LastValue* unit, int inNumSamples);
void LastValue_next_kk(LastValue* unit, int inNumSamples);

void Done_Ctor(Done* unit);
void Done_next(Done* unit, int inNumSamples);

void FreeSelf_Ctor(FreeSelf* unit);
void FreeSelf_next(FreeSelf* unit, int inNumSamples);

void FreeSelfWhenDone_Ctor(FreeSelfWhenDone* unit);
void FreeSelfWhenDone_next(FreeSelfWhenDone* unit, int inNumSamples);

void PauseSelf_Ctor(PauseSelf* unit);
void PauseSelf_next(PauseSelf* unit, int inNumSamples);

void Pause_Ctor(Pause* unit);
void Pause_next(Pause* unit, int inNumSamples);

void Free_Ctor(Free* unit);
void Free_next(Free* unit, int inNumSamples);

void PauseSelfWhenDone_Ctor(PauseSelfWhenDone* unit);
void PauseSelfWhenDone_next(PauseSelfWhenDone* unit, int inNumSamples);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef NOVA_SIMD
FLATTEN void Trig1_next_nova(Trig1* unit, int inNumSamples);
FLATTEN void Trig1_next_k_nova(Trig1* unit, int inNumSamples);
#endif

void Trig1_Ctor(Trig1* unit) {
    if (unit->mCalcRate == calc_FullRate && INRATE(0) != calc_FullRate) {
#ifdef NOVA_SIMD
        if (boost::alignment::is_aligned(BUFLENGTH, 16))
            SETCALC(Trig1_next_k_nova);
        else
#endif
            SETCALC(Trig1_next_k);
    } else {
#ifdef NOVA_SIMD
        if (boost::alignment::is_aligned(BUFLENGTH, 16))
            SETCALC(Trig1_next_nova);
        else
#endif
            SETCALC(Trig1_next);
    }
    unit->mCounter = 0;
    unit->m_prevtrig = 0.f;

    Trig1_next(unit, 1);
}

void Trig1_next(Trig1* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* trig = ZIN(0);
    float dur = ZIN0(1);
    float sr = unit->mRate->mSampleRate;
    float prevtrig = unit->m_prevtrig;
    unsigned long counter = unit->mCounter;

    LOOP1(
        inNumSamples, float curtrig = ZXP(trig); float zout; if (counter > 0) { zout = --counter ? 1.f : 0.f; } else {
            if (curtrig > 0.f && prevtrig <= 0.f) {
                counter = (long)(dur * sr + .5f);
                if (counter < 1)
                    counter = 1;
                zout = 1.f;
            } else {
                zout = 0.f;
            }
        } prevtrig = curtrig;
        ZXP(out) = zout;);
    unit->m_prevtrig = prevtrig;
    unit->mCounter = counter;
}

void Trig1_next_k(Trig1* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float curtrig = ZIN0(0);
    float dur = ZIN0(1);
    float sr = unit->mRate->mSampleRate;
    float prevtrig = unit->m_prevtrig;
    unsigned long counter = unit->mCounter;

    LOOP1(
        inNumSamples, float zout; if (counter > 0) { zout = --counter ? 1.f : 0.f; } else {
            if (curtrig > 0.f && prevtrig <= 0.f) {
                counter = (long)(dur * sr + .5f);
                if (counter < 1)
                    counter = 1;
                zout = 1.f;
            } else {
                zout = 0.f;
            }
        } prevtrig = curtrig;
        ZXP(out) = zout;);
    unit->m_prevtrig = prevtrig;
    unit->mCounter = counter;
}

#ifdef NOVA_SIMD
void Trig1_next_nova(Trig1* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* trig = ZIN(0);
    float dur = ZIN0(1);
    float sr = unit->mRate->mSampleRate;
    float prevtrig = unit->m_prevtrig;
    unsigned long counter = unit->mCounter;

    if (counter > inNumSamples) {
        nova::setvec_simd(OUT(0), 1.f, inNumSamples);
        counter -= inNumSamples;
        assert(counter > 0);
        prevtrig = IN(0)[inNumSamples - 1];
    } else {
        LOOP1(
            inNumSamples, float curtrig = ZXP(trig); float zout;
            if (counter > 0) { zout = --counter ? 1.f : 0.f; } else {
                if (curtrig > 0.f && prevtrig <= 0.f) {
                    counter = (long)(dur * sr + .5f);
                    if (counter < 1)
                        counter = 1;
                    zout = 1.f;
                } else {
                    zout = 0.f;
                }
            } prevtrig = curtrig;
            ZXP(out) = zout;);
    }
    unit->m_prevtrig = prevtrig;
    unit->mCounter = counter;
}


void Trig1_next_k_nova(Trig1* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float curtrig = ZIN0(0);
    float dur = ZIN0(1);
    float sr = unit->mRate->mSampleRate;
    float prevtrig = unit->m_prevtrig;
    unsigned long counter = unit->mCounter;

    if (counter > inNumSamples) {
        nova::setvec_simd(OUT(0), 1.f, inNumSamples);
        counter -= inNumSamples;
        assert(counter > 0);
        prevtrig = curtrig;
    } else if (counter == 0 && (curtrig <= 0.f || prevtrig > 0.f)) {
        nova::zerovec_simd(OUT(0), inNumSamples);
        prevtrig = curtrig;
    } else {
        LOOP1(
            inNumSamples, float zout; if (counter > 0) { zout = --counter ? 1.f : 0.f; } else {
                if (curtrig > 0.f && prevtrig <= 0.f) {
                    counter = (long)(dur * sr + .5f);
                    if (counter < 1)
                        counter = 1;
                    zout = 1.f;
                } else {
                    zout = 0.f;
                }
            } prevtrig = curtrig;
            ZXP(out) = zout;);
    }

    unit->m_prevtrig = prevtrig;
    unit->mCounter = counter;
}
#endif


////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef NOVA_SIMD
void Trig_next_nova(Trig* unit, int inNumSamples);
void Trig_next_k_nova(Trig* unit, int inNumSamples);
#endif

void Trig_Ctor(Trig* unit) {
    if (unit->mCalcRate == calc_FullRate && INRATE(0) != calc_FullRate) {
#ifdef NOVA_SIMD
        if (boost::alignment::is_aligned(BUFLENGTH, 16))
            SETCALC(Trig_next_k_nova);
        else
#endif
            SETCALC(Trig_next_k);
    } else {
#ifdef NOVA_SIMD
        if (boost::alignment::is_aligned(BUFLENGTH, 16))
            SETCALC(Trig_next_nova);
        else
#endif
            SETCALC(Trig_next);
    }

    unit->mCounter = 0;
    unit->m_prevtrig = 0.f;
    unit->mLevel = 0.f;

    Trig_next(unit, 1);
}

void Trig_next(Trig* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* trig = ZIN(0);
    float dur = ZIN0(1);
    float sr = unit->mRate->mSampleRate;
    float prevtrig = unit->m_prevtrig;
    float level = unit->mLevel;
    unsigned long counter = unit->mCounter;

    LOOP1(
        inNumSamples, float curtrig = ZXP(trig); float zout; if (counter > 0) { zout = --counter ? level : 0.f; } else {
            if (curtrig > 0.f && prevtrig <= 0.f) {
                counter = (long)(dur * sr + .5f);
                if (counter < 1)
                    counter = 1;
                level = curtrig;
                zout = level;
            } else {
                zout = 0.f;
            }
        } prevtrig = curtrig;
        ZXP(out) = zout;);
    unit->m_prevtrig = prevtrig;
    unit->mCounter = counter;
    unit->mLevel = level;
}

void Trig_next_k(Trig* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float curtrig = ZIN0(0);
    float dur = ZIN0(1);
    float sr = unit->mRate->mSampleRate;
    float prevtrig = unit->m_prevtrig;
    float level = unit->mLevel;
    unsigned long counter = unit->mCounter;

    LOOP1(
        inNumSamples, float zout; if (counter > 0) { zout = --counter ? level : 0.f; } else {
            if (curtrig > 0.f && prevtrig <= 0.f) {
                counter = (long)(dur * sr + .5f);
                if (counter < 1)
                    counter = 1;
                level = curtrig;
                zout = level;
            } else {
                zout = 0.f;
            }
        } prevtrig = curtrig;
        ZXP(out) = zout;);
    unit->m_prevtrig = prevtrig;
    unit->mCounter = counter;
    unit->mLevel = level;
}

#ifdef NOVA_SIMD
void Trig_next_nova(Trig* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* trig = ZIN(0);
    float dur = ZIN0(1);
    float sr = unit->mRate->mSampleRate;
    float prevtrig = unit->m_prevtrig;
    float level = unit->mLevel;
    unsigned long counter = unit->mCounter;

    if (counter > inNumSamples) {
        nova::setvec_simd(OUT(0), level, inNumSamples);
        counter -= inNumSamples;
        assert(counter > 0);
        prevtrig = IN(0)[inNumSamples - 1];
    } else {
        LOOP1(
            inNumSamples, float curtrig = ZXP(trig); float zout;
            if (counter > 0) { zout = --counter ? level : 0.f; } else {
                if (curtrig > 0.f && prevtrig <= 0.f) {
                    counter = (long)(dur * sr + .5f);
                    if (counter < 1)
                        counter = 1;
                    level = curtrig;
                    zout = level;
                } else {
                    zout = 0.f;
                }
            } prevtrig = curtrig;
            ZXP(out) = zout;);
    }
    unit->m_prevtrig = prevtrig;
    unit->mCounter = counter;
    unit->mLevel = level;
}

void Trig_next_k_nova(Trig* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float curtrig = ZIN0(0);
    float dur = ZIN0(1);
    float sr = unit->mRate->mSampleRate;
    float prevtrig = unit->m_prevtrig;
    float level = unit->mLevel;
    unsigned long counter = unit->mCounter;

    if (counter > inNumSamples) {
        nova::setvec_simd(OUT(0), level, inNumSamples);
        counter -= inNumSamples;
        assert(counter > 0);
        prevtrig = curtrig;
    } else if (counter == 0 && (curtrig <= 0.f || prevtrig > 0.f)) {
        nova::zerovec_simd(OUT(0), inNumSamples);
        prevtrig = curtrig;
    } else {
        LOOP1(
            inNumSamples, float zout; if (counter > 0) { zout = --counter ? level : 0.f; } else {
                if (curtrig > 0.f && prevtrig <= 0.f) {
                    counter = (long)(dur * sr + .5f);
                    if (counter < 1)
                        counter = 1;
                    level = curtrig;
                    zout = level;
                } else {
                    zout = 0.f;
                }
            } prevtrig = curtrig;
            ZXP(out) = zout;);
    }
    unit->m_prevtrig = prevtrig;
    unit->mCounter = counter;
    unit->mLevel = level;
}
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////

void SendTrig_Ctor(SendTrig* unit) {
    if (INRATE(2) == calc_FullRate) {
        SETCALC(SendTrig_next_aka);
    } else {
        SETCALC(SendTrig_next);
    }
    unit->m_prevtrig = 0.f;
}

void SendTrig_next(SendTrig* unit, int inNumSamples) {
    float* trig = ZIN(0);
    float prevtrig = unit->m_prevtrig;

    LOOP1(
        inNumSamples, float curtrig = ZXP(trig); if (curtrig > 0.f && prevtrig <= 0.f) {
            SendTrigger(&unit->mParent->mNode, (int)ZIN0(1), ZIN0(2));
        } prevtrig = curtrig;);
    unit->m_prevtrig = prevtrig;
}

void SendTrig_next_aka(SendTrig* unit, int inNumSamples) {
    float* trig = ZIN(0);
    int id = (int)ZIN0(1);
    float* value = ZIN(2);
    float prevtrig = unit->m_prevtrig;

    LOOP1(
        inNumSamples, float curtrig = ZXP(trig); float curval = ZXP(value);
        if (curtrig > 0.f && prevtrig <= 0.f) { SendTrigger(&unit->mParent->mNode, id, curval); } prevtrig = curtrig;);
    unit->m_prevtrig = prevtrig;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////

static void Unit_next_nop(SendReply* unit, int inNumSamples) {}

void SendReply_Ctor(SendReply* unit) {
    const int kVarOffset = 3;

    unit->m_prevtrig = 0.f;
    unit->m_cmdNameSize = IN0(2);
    unit->m_valueSize = unit->mNumInputs - unit->m_cmdNameSize - kVarOffset;
    unit->m_valueOffset = kVarOffset + unit->m_cmdNameSize;

    // allocations
    const int cmdNameAllocSize = (unit->m_cmdNameSize + 1) * sizeof(char);
    const int valuesAllocSize = unit->m_valueSize * sizeof(float);
    char* chunk = (char*)RTAlloc(unit->mWorld, cmdNameAllocSize + valuesAllocSize);

    if (!chunk) {
        Print("SendReply: RT memory allocation failed\n");
        SETCALC(Unit_next_nop);
        return;
    }

    unit->m_cmdName = chunk;
    unit->m_values = (float*)(chunk + cmdNameAllocSize);

    for (int i = 0; i < (int)unit->m_cmdNameSize; i++)
        unit->m_cmdName[i] = (char)IN0(kVarOffset + i);

    // terminate string
    unit->m_cmdName[unit->m_cmdNameSize] = 0;

    if (INRATE(0) == calc_FullRate)
        SETCALC(SendReply_next_aka);
    else
        SETCALC(SendReply_next);
}

void SendReply_Dtor(SendReply* unit) { RTFree(unit->mWorld, unit->m_cmdName); }


void SendReply_next(SendReply* unit, int inNumSamples) {
    float* trig = IN(0);
    float prevtrig = unit->m_prevtrig;
    float* values = unit->m_values;
    int valueSize = unit->m_valueSize;
    int valueOffset = unit->m_valueOffset;
    for (int j = 0; j < inNumSamples; j++) {
        float curtrig = trig[j];
        if (curtrig > 0.f && prevtrig <= 0.f) {
            for (int i = 0; i < valueSize; i++)
                values[i] = IN(i + valueOffset)[0];

            SendNodeReply(&unit->mParent->mNode, (int)ZIN0(1), unit->m_cmdName, unit->m_valueSize, values);
        }
        prevtrig = curtrig;
    }
    unit->m_prevtrig = prevtrig;
}

void SendReply_next_aka(SendReply* unit, int inNumSamples) {
    float* trig = IN(0);
    float prevtrig = unit->m_prevtrig;
    float* values = unit->m_values;
    int valueSize = unit->m_valueSize;
    int valueOffset = unit->m_valueOffset;
    for (int j = 0; j < inNumSamples; j++) {
        float curtrig = trig[j];
        if (curtrig > 0.f && prevtrig <= 0.f) {
            for (int i = 0; i < valueSize; i++) {
                int offset = INRATE(i + valueOffset) != calc_FullRate ? 0 : j;
                values[i] = IN(i + valueOffset)[offset];
            }
            SendNodeReply(&unit->mParent->mNode, (int)ZIN0(1), unit->m_cmdName, unit->m_valueSize, values);
        }
        prevtrig = curtrig;
    }
    unit->m_prevtrig = prevtrig;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////

void Poll_Ctor(Poll* unit) {
    if (INRATE(0) == calc_FullRate) {
        if (INRATE(1) == calc_FullRate) {
            SETCALC(Poll_next_aa);
        } else {
            SETCALC(Poll_next_ak);
        }
    } else {
        SETCALC(Poll_next_kk);
    }

    unit->m_trig = IN0(0);
    const int idSize = (int)IN0(3); // number of chars in the id string
    unit->m_id_string = (char*)RTAlloc(unit->mWorld, (idSize + 1) * sizeof(char));

    if (!unit->m_id_string) {
        Print("Poll: RT memory allocation failed\n");
        SETCALC(Unit_next_nop);
        return;
    }

    for (int i = 0; i < idSize; i++)
        unit->m_id_string[i] = (char)IN0(4 + i);

    unit->m_id_string[idSize] = '\0';
    unit->m_mayprint = unit->mWorld->mVerbosity >= -1;

    Poll_next_kk(unit, 1);
}

void Poll_Dtor(Poll* unit) { RTFree(unit->mWorld, unit->m_id_string); }

void Poll_next_aa(Poll* unit, int inNumSamples) {
    float* in = IN(1);
    float* trig = IN(0);
    float lasttrig = unit->m_trig;
    for (int i = 0; i < inNumSamples; i++) {
        if ((lasttrig <= 0.0) && (trig[i] > 0.0)) {
            if (unit->m_mayprint)
                Print("%s: %g\n", unit->m_id_string, in[i]);

            if (IN0(2) >= 0.0)
                SendTrigger(&unit->mParent->mNode, (int)IN0(2), in[i]);
        }
        lasttrig = trig[i];
    }
    unit->m_trig = lasttrig;
}

void Poll_next_kk(Poll* unit, int inNumSamples) {
    float in = IN0(1);
    float trig = IN0(0);
    if ((unit->m_trig <= 0.0) && (trig > 0.0)) {
        if (unit->m_mayprint)
            Print("%s: %g\n", unit->m_id_string, in);

        if (IN0(2) >= 0.0)
            SendTrigger(&unit->mParent->mNode, (int)IN0(2), in);
    }
    unit->m_trig = trig;
}


void Poll_next_ak(Poll* unit, int inNumSamples) {
    float in = IN0(1);
    float* trig = IN(0);
    float lasttrig = unit->m_trig;
    for (int i = 0; i < inNumSamples; i++) {
        if ((lasttrig <= 0.0) && (trig[i] > 0.0)) {
            if (unit->m_mayprint) {
                Print("%s: %g\n", unit->m_id_string, in);
            }
            if (IN0(2) >= 0.0)
                SendTrigger(&unit->mParent->mNode, (int)IN0(2), in);
        }
        lasttrig = trig[i];
    }
    unit->m_trig = lasttrig;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////

void SetResetFF_Ctor(SetResetFF* unit) {
    if (INRATE(1) == calc_FullRate) {
        SETCALC(SetResetFF_next_a);
    } else {
        SETCALC(SetResetFF_next_k);
    }

    unit->m_prevtrig = 0.f;
    unit->m_prevreset = 0.f;
    unit->mLevel = 0.f;

    SetResetFF_next_k(unit, 1);
}


void SetResetFF_next_a(SetResetFF* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* trig = ZIN(0);
    float* reset = ZIN(1);
    float prevtrig = unit->m_prevtrig;
    float prevreset = unit->m_prevreset;
    float level = unit->mLevel;

    LOOP1(inNumSamples, float curtrig = ZXP(trig); float curreset = ZXP(reset);
          if (prevreset <= 0.f && curreset > 0.f) level = 0.f; else if (prevtrig <= 0.f && curtrig > 0.f) level = 1.f;
          ZXP(out) = level; prevtrig = curtrig; prevreset = curreset;);
    unit->m_prevtrig = prevtrig;
    unit->m_prevreset = prevreset;
    unit->mLevel = level;
}

void SetResetFF_next_k(SetResetFF* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* trig = ZIN(0);
    float curreset = ZIN0(1);
    float prevtrig = unit->m_prevtrig;
    float prevreset = unit->m_prevreset;
    float level = unit->mLevel;

    float curtrig = ZXP(trig);
    if (prevreset <= 0.f && curreset > 0.f)
        level = 0.f;
    else if (prevtrig <= 0.f && curtrig > 0.f)
        level = 1.f;
    ZXP(out) = level;
    prevtrig = curtrig;

    LOOP(inNumSamples - 1, curtrig = ZXP(trig); if (prevtrig <= 0.f && curtrig > 0.f) level = 1.f; ZXP(out) = level;
         prevtrig = curtrig;);
    unit->m_prevtrig = prevtrig;
    unit->m_prevreset = curreset;
    unit->mLevel = level;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////


void ToggleFF_Ctor(ToggleFF* unit) {
    SETCALC(ToggleFF_next);

    unit->m_prevtrig = 0.f;
    unit->mLevel = 0.f;

    ZOUT0(0) = 0.f;
}


void ToggleFF_next(ToggleFF* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* trig = ZIN(0);
    float prevtrig = unit->m_prevtrig;
    float level = unit->mLevel;

    LOOP1(inNumSamples, float curtrig = ZXP(trig); if (prevtrig <= 0.f && curtrig > 0.f) level = 1.f - level;
          ZXP(out) = level; prevtrig = curtrig;);
    unit->m_prevtrig = prevtrig;
    unit->mLevel = level;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////


#ifdef NOVA_SIMD
void Latch_next_ak_nova(Latch* unit, int inNumSamples) {
    float level = unit->mLevel;

    float curtrig = ZIN0(1);
    if (unit->m_prevtrig <= 0.f && curtrig > 0.f)
        level = ZIN0(0);

    nova::setvec_simd(OUT(0), level, inNumSamples);

    unit->m_prevtrig = curtrig;
    unit->mLevel = level;
}

void Latch_next_ak_nova_64(Latch* unit, int inNumSamples) {
    float level = unit->mLevel;

    float curtrig = ZIN0(1);
    if (unit->m_prevtrig <= 0.f && curtrig > 0.f)
        level = ZIN0(0);

    nova::setvec_simd<64>(OUT(0), level);

    unit->m_prevtrig = curtrig;
    unit->mLevel = level;
}
#endif

void Latch_Ctor(Latch* unit) {
    if (INRATE(1) == calc_FullRate) {
        SETCALC(Latch_next_aa);
    } else {
#ifdef NOVA_SIMD
        if (BUFLENGTH == 64)
            SETCALC(Latch_next_ak_nova_64);
        if (boost::alignment::is_aligned(BUFLENGTH, 16))
            SETCALC(Latch_next_ak_nova);
        else
#endif
            SETCALC(Latch_next_ak);
    }

    unit->m_prevtrig = 0.f;
    unit->mLevel = 0.f;

    ZOUT0(0) = ZIN0(1) > 0.f ? ZIN0(0) : 0.f;
}


void Latch_next_ak(Latch* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float level = unit->mLevel;

    float curtrig = ZIN0(1);
    if (unit->m_prevtrig <= 0.f && curtrig > 0.f)
        level = ZIN0(0);

    LOOP1(inNumSamples, ZXP(out) = level;);

    unit->m_prevtrig = curtrig;
    unit->mLevel = level;
}


void Latch_next_aa(Latch* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* in = ZIN(0);
    float* trig = ZIN(1);
    float prevtrig = unit->m_prevtrig;
    float level = unit->mLevel;

    LOOP1(
        inNumSamples, float curtrig = ZXP(trig); if (prevtrig <= 0.f && curtrig > 0.f) level = ZXP(in);
        else { PZ(in); } ZXP(out) = level; prevtrig = curtrig;);
    unit->m_prevtrig = prevtrig;
    unit->mLevel = level;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef NOVA_SIMD
FLATTEN void Gate_next_ak_nova(Gate* unit, int inNumSamples) {
    float* trig = ZIN(1);
    float level = unit->mLevel;

    float curtrig = ZXP(trig);
    if (curtrig > 0.f) {
        nova::copyvec_simd(OUT(0), IN(0), inNumSamples);
        unit->mLevel = IN(0)[inNumSamples - 1];
    } else
        nova::setvec_simd(OUT(0), level, inNumSamples);
}

FLATTEN void Gate_next_ak_nova_64(Gate* unit, int inNumSamples) {
    float* trig = ZIN(1);
    float level = unit->mLevel;

    float curtrig = ZXP(trig);
    if (curtrig > 0.f) {
        nova::copyvec_simd<64>(OUT(0), IN(0));
        unit->mLevel = IN(0)[inNumSamples - 1];
    } else
        nova::setvec_simd<64>(OUT(0), level);
}
#endif

void Gate_Ctor(Gate* unit) {
    if (INRATE(1) == calc_FullRate) {
        SETCALC(Gate_next_aa);
    } else {
#ifdef NOVA_SIMD
        if (BUFLENGTH == 64)
            SETCALC(Gate_next_ak_nova_64);
        if (boost::alignment::is_aligned(BUFLENGTH, 16))
            SETCALC(Gate_next_ak_nova);
        else
#endif
            SETCALC(Gate_next_ak);
    }

    unit->mLevel = 0.f;

    Gate_next_ak(unit, 1);
}


void Gate_next_ak(Gate* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* in = ZIN(0);
    float* trig = ZIN(1);
    float level = unit->mLevel;

    float curtrig = ZXP(trig);
    if (curtrig > 0.f) {
        LOOP1(inNumSamples, level = ZXP(in); ZXP(out) = level;);
        unit->mLevel = level;
    } else {
        LOOP1(inNumSamples, ZXP(out) = level;);
    }
}

void Gate_next_aa(Gate* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* in = ZIN(0);
    float* trig = ZIN(1);
    float level = unit->mLevel;

    LOOP1(
        inNumSamples, float curtrig = ZXP(trig); if (curtrig > 0.f) level = ZXP(in);
        else { PZ(in); } ZXP(out) = level;);
    unit->mLevel = level;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////


void Schmidt_Ctor(Schmidt* unit) {
    SETCALC(Schmidt_next);

    unit->mLevel = 0.f;

    Schmidt_next(unit, 1);
}

void Schmidt_next(Schmidt* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* in = ZIN(0);
    float lo = ZIN0(1);
    float hi = ZIN0(2);
    float level = unit->mLevel;
    LOOP1(
        inNumSamples, float zin = ZXP(in); if (level == 1.) {
            if (zin < lo)
                level = 0.f;
        } else {
            if (zin > hi)
                level = 1.f;
        } ZXP(out) = level;);
    unit->mLevel = level;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void PulseDivider_Ctor(PulseDivider* unit) {
    SETCALC(PulseDivider_next);

    unit->m_prevtrig = 0.f;
    unit->mLevel = 0.f;
    unit->mCounter = (long)floor(ZIN0(2) + 0.5);

    PulseDivider_next(unit, 1);
}


void PulseDivider_next(PulseDivider* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* trig = ZIN(0);
    long div = (long)ZIN0(1);
    float prevtrig = unit->m_prevtrig;
    long counter = unit->mCounter;

    LOOP1(
        inNumSamples, float z; float curtrig = ZXP(trig); if (prevtrig <= 0.f && curtrig > 0.f) {
            counter++;
            if (counter >= div) {
                counter = 0;
                z = 1.f;
            } else {
                z = 0.f;
            }
        } else { z = 0.f; } ZXP(out) = z;
        prevtrig = curtrig;);
    unit->mCounter = counter;
    unit->m_prevtrig = prevtrig;
}

//////////////////////////////////////////////////////////////////////////////////////////


void PulseCount_Ctor(PulseCount* unit) {
    if (INRATE(1) == calc_FullRate) {
        SETCALC(PulseCount_next_a);
    } else if (INRATE(1) == calc_BufRate) {
        SETCALC(PulseCount_next_k);
    } else {
        SETCALC(PulseCount_next_0);
    }
    unit->m_prevtrig = 0.f;
    unit->m_prevreset = 0.f;
    unit->mLevel = 0.f;

    PulseCount_next_k(unit, 1);
}


void PulseCount_next_a(PulseCount* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* trig = ZIN(0);
    float* reset = ZIN(1);
    float prevtrig = unit->m_prevtrig;
    float prevreset = unit->m_prevreset;
    float level = unit->mLevel;

    LOOP1(
        inNumSamples, float curtrig = ZXP(trig); float curreset = ZXP(reset);
        if (prevreset <= 0.f && curreset > 0.f) level = 0.f;
        else if (prevtrig <= 0.f && curtrig > 0.f) { level += 1.f; } ZXP(out) = level; prevtrig = curtrig;
        prevreset = curreset;);
    unit->mLevel = level;
    unit->m_prevtrig = prevtrig;
    unit->m_prevreset = prevreset;
}

void PulseCount_next_k(PulseCount* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* trig = ZIN(0);
    float curreset = ZIN0(1);
    float prevtrig = unit->m_prevtrig;
    float prevreset = unit->m_prevreset;
    float level = unit->mLevel;

    LOOP1(
        inNumSamples, float curtrig = ZXP(trig); if (prevreset <= 0.f && curreset > 0.f) level = 0.f;
        else if (prevtrig <= 0.f && curtrig > 0.f) { level += 1.f; } ZXP(out) = level; prevtrig = curtrig;
        prevreset = curreset;);
    unit->mLevel = level;
    unit->m_prevtrig = prevtrig;
    unit->m_prevreset = prevreset;
}

void PulseCount_next_0(PulseCount* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* trig = ZIN(0);
    float prevtrig = unit->m_prevtrig;
    float level = unit->mLevel;

    LOOP1(
        inNumSamples, float curtrig = ZXP(trig);
        if (prevtrig <= 0.f && curtrig > 0.f) { level += 1.f; } ZXP(out) = level; prevtrig = curtrig;);
    unit->mLevel = level;
    unit->m_prevtrig = prevtrig;
}

//////////////////////////////////////////////////////////////////////////////////////////


void Stepper_Ctor(Stepper* unit) {
    if (unit->mCalcRate == calc_FullRate && INRATE(0) == calc_FullRate && INRATE(1) == calc_ScalarRate) {
        SETCALC(Stepper_next_a0);
    } else if (unit->mCalcRate == calc_FullRate && INRATE(0) == calc_FullRate && INRATE(1) != calc_FullRate) {
        SETCALC(Stepper_next_ak);
    } else {
        SETCALC(Stepper_next_aa);
    }

    int32 resetval = (int32)ZIN0(5);

    unit->m_prevtrig = 0.f;
    unit->m_prevreset = 0.f;
    unit->mLevel = (float)resetval;

    Stepper_next_ak(unit, 1);
}


void Stepper_next_aa(Stepper* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* trig = ZIN(0);
    float* reset = ZIN(1);
    int32 zmin = (int32)ZIN0(2);
    int32 zmax = (int32)ZIN0(3);
    int32 step = (int32)ZIN0(4);
    int32 resetval = (int32)ZIN0(5);
    float prevtrig = unit->m_prevtrig;
    float prevreset = unit->m_prevreset;
    float level = unit->mLevel;

    LOOP1(
        inNumSamples, float curtrig = ZXP(trig); float curreset = ZXP(reset); if (prevreset <= 0.f && curreset > 0.f) {
            level = (float)sc_wrap(resetval, zmin, zmax);
        } else if (prevtrig <= 0.f && curtrig > 0.f) {
            level = (float)sc_wrap((int32)level + step, zmin, zmax);
        } ZXP(out) = level;
        prevtrig = curtrig; prevreset = curreset;);
    unit->mLevel = level;
    unit->m_prevtrig = prevtrig;
    unit->m_prevreset = prevreset;
}

void Stepper_next_ak(Stepper* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* trig = ZIN(0);
    float curreset = ZIN0(1);
    int32 zmin = (int32)ZIN0(2);
    int32 zmax = (int32)ZIN0(3);
    int32 step = (int32)ZIN0(4);
    int32 resetval = (int32)ZIN0(5);
    float prevtrig = unit->m_prevtrig;
    float prevreset = unit->m_prevreset;
    float level = unit->mLevel;

    LOOP1(
        inNumSamples, float curtrig = ZXP(trig); if (prevreset <= 0.f && curreset > 0.f) {
            level = (float)sc_wrap(resetval, zmin, zmax);
        } else if (prevtrig <= 0.f && curtrig > 0.f) {
            level = (float)sc_wrap((int32)level + step, zmin, zmax);
        } ZXP(out) = level;
        prevtrig = curtrig; prevreset = curreset;);
    unit->mLevel = level;
    unit->m_prevtrig = prevtrig;
    unit->m_prevreset = prevreset;
}

void Stepper_next_a0(Stepper* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* trig = ZIN(0);
    int32 zmin = (int32)ZIN0(2);
    int32 zmax = (int32)ZIN0(3);
    int32 step = (int32)ZIN0(4);
    float prevtrig = unit->m_prevtrig;
    float level = unit->mLevel;

    LOOP1(
        inNumSamples, float curtrig = ZXP(trig); if (prevtrig <= 0.f && curtrig > 0.f) {
            level = (float)sc_wrap((int32)level + step, zmin, zmax);
        } ZXP(out) = level;
        prevtrig = curtrig;);
    unit->mLevel = level;
    unit->m_prevtrig = prevtrig;
}

//////////////////////////////////////////////////////////////////////////////////////////

void TDelay_Ctor(TDelay* unit) {
    SETCALC(TDelay_next);

    unit->m_prevtrig = 0.f;
    unit->mCounter = 0;

    TDelay_next(unit, 1);
}


void TDelay_next(TDelay* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* trig = ZIN(0);
    float dur = ZIN0(1);
    float prevtrig = unit->m_prevtrig;
    long counter = unit->mCounter;

    LOOP1(
        inNumSamples, float curtrig = ZXP(trig); float zout; if (counter > 1) {
            counter--;
            zout = 0.f;
        } else if (counter <= 0) {
            if (prevtrig <= 0.f && curtrig > 0.f) {
                counter = (long)(dur * unit->mRate->mSampleRate + .5f);
                if (counter < 1)
                    counter = 1;
            }
            zout = 0.f;
        } else {
            counter = 0;
            zout = 1.f;
        } ZXP(out) = zout;
        prevtrig = curtrig;);
    unit->m_prevtrig = prevtrig;
    unit->mCounter = counter;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////


void ZeroCrossing_Ctor(ZeroCrossing* unit) {
    SETCALC(ZeroCrossing_next_a);

    unit->m_prevfrac = 0.f;
    unit->m_previn = ZIN0(0);
    ZOUT0(0) = unit->mLevel = 0.f;
    unit->mCounter = 0;
}

void ZeroCrossing_next_a(ZeroCrossing* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* in = ZIN(0);
    float previn = unit->m_previn;
    float prevfrac = unit->m_prevfrac;
    float level = unit->mLevel;
    long counter = unit->mCounter;

    LOOP1(
        inNumSamples, counter++; float curin = ZXP(in); if (counter > 4 && previn <= 0.f && curin > 0.f) {
            float frac = -previn / (curin - previn);
            level = unit->mRate->mSampleRate / (frac + counter - prevfrac);
            prevfrac = frac;
            counter = 0;
        } ZXP(out) = level;
        previn = curin;);

    unit->m_previn = previn;
    unit->m_prevfrac = prevfrac;
    unit->mLevel = level;
    unit->mCounter = counter;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void Timer_Ctor(Timer* unit) {
    SETCALC(Timer_next_a);

    unit->m_prevfrac = 0.f;
    unit->m_previn = ZIN0(0);
    ZOUT0(0) = unit->mLevel = 0.f;
    unit->mCounter = 0;
}

void Timer_next_a(Timer* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* in = ZIN(0);
    float previn = unit->m_previn;
    float prevfrac = unit->m_prevfrac;
    float level = unit->mLevel;
    long counter = unit->mCounter;

    LOOP1(
        inNumSamples, counter++; float curin = ZXP(in); if (previn <= 0.f && curin > 0.f) {
            float frac = -previn / (curin - previn);
            level = unit->mRate->mSampleDur * (frac + counter - prevfrac);
            prevfrac = frac;
            counter = 0;
        } ZXP(out) = level;
        previn = curin;);

    unit->m_previn = previn;
    unit->m_prevfrac = prevfrac;
    unit->mLevel = level;
    unit->mCounter = counter;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////

void Sweep_Ctor(Sweep* unit) {
    if (INRATE(0) == calc_ScalarRate) {
        if (INRATE(1) == calc_FullRate) {
            SETCALC(Sweep_next_0a);
        } else {
            SETCALC(Sweep_next_0k);
        }
    } else if (INRATE(0) == calc_BufRate) {
        if (INRATE(1) == calc_FullRate) {
            SETCALC(Sweep_next_ka);
        } else {
            SETCALC(Sweep_next_kk);
        }
    } else {
        if (INRATE(1) == calc_FullRate) {
            SETCALC(Sweep_next_aa);
        } else {
            SETCALC(Sweep_next_ak);
        }
    }

    unit->m_previn = ZIN0(0);
    ZOUT0(0) = unit->mLevel = 0.f;
}

void Sweep_next_0k(Sweep* unit, int inNumSamples) {
    float* out = ZOUT(0);
    double rate = ZIN0(1) * SAMPLEDUR;
    double level = unit->mLevel;

    LOOP1(inNumSamples, level += rate; ZXP(out) = level;);

    unit->mLevel = level;
}

void Sweep_next_0a(Sweep* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* rate = ZIN(1);
    double level = unit->mLevel;
    float sampledur = SAMPLEDUR;

    LOOP1(inNumSamples, float zrate = ZXP(rate) * sampledur; level += zrate; ZXP(out) = level;);

    unit->mLevel = level;
}

void Sweep_next_kk(Sweep* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float curin = ZIN0(0);
    double rate = ZIN0(1) * SAMPLEDUR;
    float previn = unit->m_previn;
    double level = unit->mLevel;

    if (previn <= 0.f && curin > 0.f) {
        float frac = -previn / (curin - previn);
        level = frac * rate;
    }

    LOOP1(inNumSamples, level += rate; ZXP(out) = level;);

    unit->m_previn = curin;
    unit->mLevel = level;
}

void Sweep_next_ka(Sweep* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float curin = ZIN0(0);
    float* rate = ZIN(1);
    float previn = unit->m_previn;
    double level = unit->mLevel;
    float sampledur = SAMPLEDUR;

    if (previn <= 0.f && curin > 0.f) {
        float frac = -previn / (curin - previn);
        level = frac * rate[ZOFF] * sampledur;
    }

    LOOP1(inNumSamples, float zrate = ZXP(rate) * sampledur; level += zrate; ZXP(out) = level;);

    unit->m_previn = curin;
    unit->mLevel = level;
}

void Sweep_next_ak(Sweep* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* in = ZIN(0);
    double rate = ZIN0(1) * SAMPLEDUR;
    float previn = unit->m_previn;
    double level = unit->mLevel;

    LOOP1(
        inNumSamples, float curin = ZXP(in); if (previn <= 0.f && curin > 0.f) {
            float frac = -previn / (curin - previn);
            level = frac * rate;
        } else { level += rate; } ZXP(out) = level;
        previn = curin;);

    unit->m_previn = previn;
    unit->mLevel = level;
}

void Sweep_next_aa(Sweep* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* in = ZIN(0);
    float* rate = ZIN(1);
    float previn = unit->m_previn;
    double level = unit->mLevel;
    float sampledur = SAMPLEDUR;

    LOOP1(
        inNumSamples, float curin = ZXP(in); float zrate = ZXP(rate) * sampledur; if (previn <= 0.f && curin > 0.f) {
            float frac = -previn / (curin - previn);
            level = frac * zrate;
        } else { level += zrate; } ZXP(out) = level;
        previn = curin;);

    unit->m_previn = previn;
    unit->mLevel = level;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void Phasor_Ctor(Phasor* unit) {
    if (unit->mCalcRate == calc_FullRate) {
        if (INRATE(0) == calc_FullRate) {
            if (INRATE(1) == calc_FullRate) {
                SETCALC(Phasor_next_aa);
            } else {
                SETCALC(Phasor_next_ak);
            }
        } else {
            SETCALC(Phasor_next_kk);
        }
    } else {
        SETCALC(Phasor_next_ak);
    }

    unit->m_previn = ZIN0(0);
    ZOUT0(0) = unit->mLevel = ZIN0(2);
}

void Phasor_next_kk(Phasor* unit, int inNumSamples) {
    float* out = ZOUT(0);

    float in = ZIN0(0);
    double rate = ZIN0(1);
    double start = ZIN0(2);
    double end = ZIN0(3);
    float resetPos = ZIN0(4);

    float previn = unit->m_previn;
    double level = unit->mLevel;

    if (previn <= 0.f && in > 0.f) {
        level = resetPos;
    }
    LOOP1(inNumSamples, level = sc_wrap(level, start, end); ZXP(out) = level; level += rate;);

    unit->m_previn = in;
    unit->mLevel = level;
}

void Phasor_next_ak(Phasor* unit, int inNumSamples) {
    float* out = ZOUT(0);

    float* in = ZIN(0);
    double rate = ZIN0(1);
    double start = ZIN0(2);
    double end = ZIN0(3);
    float resetPos = ZIN0(4);

    float previn = unit->m_previn;
    double level = unit->mLevel;

    LOOP1(
        inNumSamples, float curin = ZXP(in); if (previn <= 0.f && curin > 0.f) {
            float frac = 1.f - previn / (curin - previn);
            level = resetPos + frac * rate;
        } ZXP(out) = level;
        level += rate; level = sc_wrap(level, start, end);

        previn = curin;);

    unit->m_previn = previn;
    unit->mLevel = level;
}

void Phasor_next_aa(Phasor* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* in = ZIN(0);
    float* rate = ZIN(1);
    double start = ZIN0(2);
    double end = ZIN0(3);
    float resetPos = ZIN0(4);

    float previn = unit->m_previn;
    double level = unit->mLevel;

    LOOP1(
        inNumSamples, float curin = ZXP(in); double zrate = ZXP(rate); if (previn <= 0.f && curin > 0.f) {
            float frac = 1.f - previn / (curin - previn);
            level = resetPos + frac * zrate;
        } ZXP(out) = level;
        level += zrate; level = sc_wrap(level, start, end); previn = curin;);

    unit->m_previn = previn;
    unit->mLevel = level;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////


void Peak_next_ak_unroll(Peak* unit, int inNumSamples);
void Peak_next_ai_unroll(Peak* unit, int inNumSamples);

#ifdef NOVA_SIMD
FLATTEN void Peak_next_ak_k_nova(Peak* unit, int inNumSamples);
FLATTEN void Peak_next_ai_k_nova(Peak* unit, int inNumSamples);
#endif

void Peak_Ctor(Peak* unit) {
    if (BUFLENGTH == 1 && INRATE(0) == calc_FullRate) {
        /* audio-rate input with control-rate output */
        if (INRATE(1) == calc_FullRate) {
            SETCALC(Peak_next_aa_k);
        } else if (INRATE(1) == calc_ScalarRate) {
#ifdef NOVA_SIMD
            if (INBUFLENGTH(0) & 7)
                SETCALC(Peak_next_ai_k);
            else
                SETCALC(Peak_next_ai_k_nova);
#else
            SETCALC(Peak_next_ai_k);
#endif
        } else {
#ifdef NOVA_SIMD
            if (INBUFLENGTH(0) & 7)
                SETCALC(Peak_next_ak_k);
            else
                SETCALC(Peak_next_ak_k_nova);
#else
            SETCALC(Peak_next_ak_k);
#endif
        }
    } else {
        if (INRATE(1) == calc_FullRate) {
            SETCALC(Peak_next_aa);
        } else if (INRATE(1) == calc_ScalarRate) {
            if (BUFLENGTH & 15)
                SETCALC(Peak_next_ai);
            else
                SETCALC(Peak_next_ai_unroll);
        } else {
            if (BUFLENGTH & 15)
                SETCALC(Peak_next_ak);
            else
                SETCALC(Peak_next_ak_unroll);
        }
    }
    unit->m_prevtrig = 0.f;
    ZOUT0(0) = unit->mLevel = ZIN0(0);
}

void Peak_next_ak(Peak* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* in = ZIN(0);
    float curtrig = ZIN0(1);
    float level = unit->mLevel;
    float inlevel;
    LOOP1(inNumSamples, inlevel = std::abs(ZXP(in)); level = std::max(inlevel, level); ZXP(out) = level;);
    if (unit->m_prevtrig <= 0.f && curtrig > 0.f)
        level = inlevel;
    unit->m_prevtrig = curtrig;
    unit->mLevel = level;
}

void Peak_next_ai(Peak* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* in = ZIN(0);
    float level = unit->mLevel;
    float inlevel;
    LOOP1(inNumSamples, inlevel = std::abs(ZXP(in)); level = std::max(inlevel, level); ZXP(out) = level;);
    unit->mLevel = level;
}

void Peak_next_aa(Peak* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* in = ZIN(0);
    float* trig = ZIN(1);
    float prevtrig = unit->m_prevtrig;
    float level = unit->mLevel;

    LOOP1(inNumSamples, float curtrig = ZXP(trig); float inlevel = std::abs(ZXP(in)); level = std::max(inlevel, level);
          ZXP(out) = level; if (prevtrig <= 0.f && curtrig > 0.f) level = inlevel; prevtrig = curtrig;);
    unit->m_prevtrig = prevtrig;
    unit->mLevel = level;
}

static inline float Peak_unroll_body(Peak* unit, int inNumSamples, float& level) {
    float* out = OUT(0);
    float* in = IN(0);
    float inlevel;

    using namespace std;
    for (int i = 0; i != inNumSamples; i += 8, out += 8, in += 8) {
        float level0 = max(abs(in[0]), level);
        float level1 = max(abs(in[1]), level0);
        float level2 = max(abs(in[2]), level1);
        float level3 = max(abs(in[3]), level2);
        float level4 = max(abs(in[4]), level3);
        float level5 = max(abs(in[5]), level4);
        float level6 = max(abs(in[6]), level5);
        inlevel = abs(in[7]);
        float level7 = max(inlevel, level6);
        out[0] = level0;
        out[1] = level1;
        out[2] = level2;
        out[3] = level3;
        out[4] = level4;
        out[5] = level5;
        out[6] = level6;
        out[7] = level7;
        level = level7;
    }
    return inlevel; /* input level of the last sample */
}

void Peak_next_ak_unroll(Peak* unit, int inNumSamples) {
    float curtrig = ZIN0(1);
    float level = unit->mLevel;
    float inlevel = Peak_unroll_body(unit, inNumSamples, level);

    if (unit->m_prevtrig <= 0.f && curtrig > 0.f)
        level = inlevel;
    unit->m_prevtrig = curtrig;
    unit->mLevel = level;
}

void Peak_next_ai_unroll(Peak* unit, int inNumSamples) {
    float level = unit->mLevel;
    Peak_unroll_body(unit, inNumSamples, level);
    unit->mLevel = level;
}

void Peak_next_ak_k(Peak* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* in = ZIN(0);
    float curtrig = ZIN0(1);
    float level;
    inNumSamples = INBUFLENGTH(0);
    if (unit->m_prevtrig <= 0.f && curtrig > 0.f) {
        level = std::abs(ZXP(in));
        inNumSamples -= 1;
    } else
        level = unit->mLevel;
    LOOP1(inNumSamples, level = std::max(std::abs(ZXP(in)), level););
    ZXP(out) = level;
    unit->m_prevtrig = curtrig;
    unit->mLevel = level;
}

void Peak_next_ai_k(Peak* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* in = ZIN(0);
    float level = unit->mLevel;
    inNumSamples = INBUFLENGTH(0);
    LOOP1(inNumSamples, level = std::max(std::abs(ZXP(in)), level););
    ZXP(out) = level;
    unit->mLevel = level;
}

void Peak_next_aa_k(Peak* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* in = ZIN(0);
    float* trig = ZIN(1);
    float prevtrig = unit->m_prevtrig;
    float level = unit->mLevel;
    float out_level;
    bool triggered = false;

    inNumSamples = INBUFLENGTH(0);
    LOOP1(
        inNumSamples, float curtrig = ZXP(trig); float inlevel = std::abs(ZXP(in)); level = std::max(inlevel, level);
        if (prevtrig <= 0.f && curtrig > 0.f) {
            triggered = true;
            out_level = level;
            level = inlevel;
        } prevtrig = curtrig;);
    if (triggered)
        ZXP(out) = out_level;
    else
        ZXP(out) = level;
    unit->m_prevtrig = prevtrig;
    unit->mLevel = level;
}

#ifdef NOVA_SIMD
void Peak_next_ak_k_nova(Peak* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* in = IN(0);
    float curtrig = ZIN0(1);
    float level = unit->mLevel;
    float inlevel;
    inNumSamples = INBUFLENGTH(0);
    inlevel = nova::peak_vec_simd(in, &level, inNumSamples);
    ZXP(out) = level;
    if (unit->m_prevtrig <= 0.f && curtrig > 0.f)
        level = inlevel;
    unit->m_prevtrig = curtrig;
    unit->mLevel = level;
}

void Peak_next_ai_k_nova(Peak* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* in = ZIN(0);
    float level = unit->mLevel;
    float inlevel;
    inNumSamples = INBUFLENGTH(0);
    inlevel = nova::peak_vec_simd(in, &level, inNumSamples);
    ZXP(out) = level;
    unit->mLevel = level;
}
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////

void RunningMin_Ctor(RunningMin* unit) {
    if (INRATE(1) == calc_FullRate) {
        SETCALC(RunningMin_next_aa);
    } else if (INRATE(1) == calc_ScalarRate) {
        SETCALC(RunningMin_next_ai);
    } else {
        SETCALC(RunningMin_next_ak);
    }
    unit->m_prevtrig = 0.f;
    ZOUT0(0) = unit->mLevel = ZIN0(0);
}

void RunningMin_next_ak(RunningMin* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* in = ZIN(0);
    float curtrig = ZIN0(1);
    float level = unit->mLevel;
    float inlevel;
    LOOP1(inNumSamples, inlevel = ZXP(in); if (inlevel < level) level = inlevel; ZXP(out) = level;);
    if (unit->m_prevtrig <= 0.f && curtrig > 0.f)
        level = inlevel;
    unit->m_prevtrig = curtrig;
    unit->mLevel = level;
}

void RunningMin_next_ai(RunningMin* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* in = ZIN(0);
    float level = unit->mLevel;
    float inlevel;
    LOOP1(inNumSamples, inlevel = ZXP(in); if (inlevel < level) level = inlevel; ZXP(out) = level;);
    unit->mLevel = level;
}

void RunningMin_next_aa(RunningMin* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* in = ZIN(0);
    float* trig = ZIN(1);
    float prevtrig = unit->m_prevtrig;
    float level = unit->mLevel;

    LOOP1(inNumSamples, float curtrig = ZXP(trig); float inlevel = ZXP(in); if (inlevel < level) level = inlevel;
          ZXP(out) = level; if (prevtrig <= 0.f && curtrig > 0.f) level = inlevel; prevtrig = curtrig;);
    unit->m_prevtrig = prevtrig;
    unit->mLevel = level;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////
void RunningMax_Ctor(RunningMax* unit) {
    if (INRATE(1) == calc_FullRate) {
        SETCALC(RunningMax_next_aa);
    } else if (INRATE(1) == calc_ScalarRate) {
        SETCALC(RunningMax_next_ai);
    } else {
        SETCALC(RunningMax_next_ak);
    }
    unit->m_prevtrig = 0.f;
    ZOUT0(0) = unit->mLevel = ZIN0(0);
}

void RunningMax_next_ak(RunningMax* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* in = ZIN(0);
    float curtrig = ZIN0(1);
    float level = unit->mLevel;
    float inlevel;
    LOOP1(inNumSamples, inlevel = ZXP(in); if (inlevel > level) level = inlevel; ZXP(out) = level;);
    if (unit->m_prevtrig <= 0.f && curtrig > 0.f)
        level = inlevel;
    unit->m_prevtrig = curtrig;
    unit->mLevel = level;
}

void RunningMax_next_ai(RunningMax* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* in = ZIN(0);
    float level = unit->mLevel;
    float inlevel;
    LOOP1(inNumSamples, inlevel = ZXP(in); if (inlevel > level) level = inlevel; ZXP(out) = level;);
    unit->mLevel = level;
}

void RunningMax_next_aa(RunningMax* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* in = ZIN(0);
    float* trig = ZIN(1);
    float prevtrig = unit->m_prevtrig;
    float level = unit->mLevel;

    LOOP1(inNumSamples, float curtrig = ZXP(trig); float inlevel = ZXP(in); if (inlevel > level) level = inlevel;
          ZXP(out) = level; if (prevtrig <= 0.f && curtrig > 0.f) level = inlevel; prevtrig = curtrig;);
    unit->m_prevtrig = prevtrig;
    unit->mLevel = level;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////


void PeakFollower_Ctor(PeakFollower* unit) {
    if (INRATE(1) == calc_ScalarRate) {
        SETCALC(PeakFollower_next_ai);
    } else {
        SETCALC(PeakFollower_next);
    }

    unit->mDecay = ZIN0(1);
    ZOUT0(0) = unit->mLevel = ZIN0(0);
}

void PeakFollower_next(PeakFollower* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* in = ZIN(0);
    float decay = ZIN0(1);
    float level = unit->mLevel;


    if (decay == unit->mDecay) {
        LOOP1(
            inNumSamples, float inlevel = std::abs(ZXP(in)); if (inlevel >= level) { level = inlevel; } else {
                level = inlevel + decay * (level - inlevel);
            } ZXP(out) = level;);

    } else {
        float decay_slope = CALCSLOPE(decay, unit->mDecay);
        if (decay >= 0.f && unit->mDecay >= 0.f) {
            LOOP1(
                inNumSamples, float inlevel = std::abs(ZXP(in)); if (inlevel >= level) { level = inlevel; } else {
                    level = inlevel + decay * (level - inlevel);
                    decay += decay_slope;
                };
                ZXP(out) = level;);
        } else if (decay <= 0.f && unit->mDecay <= 0.f) {
            LOOP1(
                inNumSamples, float inlevel = std::abs(ZXP(in)); if (inlevel >= level) { level = inlevel; } else {
                    level = inlevel + decay * (level + inlevel);
                    decay += decay_slope;
                };
                ZXP(out) = level;);
        } else {
            LOOP1(
                inNumSamples, float inlevel = std::abs(ZXP(in)); if (inlevel >= level) { level = inlevel; } else {
                    level = (1.f - std::abs(decay)) * inlevel + decay * level;
                    decay += decay_slope;
                };
                ZXP(out) = level;);
        };
    }

    unit->mLevel = level;
    unit->mDecay = decay;
}


void PeakFollower_next_ai(PeakFollower* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* in = ZIN(0);
    float decay = ZIN0(1);
    float level = unit->mLevel;

    LOOP1(
        inNumSamples, float inlevel = std::abs(ZXP(in)); if (inlevel >= level) { level = inlevel; } else {
            level = inlevel + decay * (level - inlevel);
        } ZXP(out) = level;);

    unit->mLevel = level;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////


void MostChange_Ctor(MostChange* unit) {
    if (INRATE(0) == calc_FullRate) {
        if (INRATE(1) == calc_FullRate) {
            SETCALC(MostChange_next_aa);
        } else {
            SETCALC(MostChange_next_ak);
        }
    } else {
        if (INRATE(1) == calc_FullRate) {
            SETCALC(MostChange_next_ka);
        } else {
            SETCALC(MostChange_next_aa);
        }
    }
    unit->mPrevA = 0.f;
    unit->mPrevB = 0.f;
    unit->mRecent = 1;
    MostChange_next_aa(unit, 1);
}

void MostChange_next_ak(MostChange* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float xb = ZIN0(1);
    float prevA = unit->mPrevA;
    float prevB = unit->mPrevB;
    int recent = unit->mRecent;
    LOOP1(
        inNumSamples, float xa = ZXP(a); float diff = std::abs(xa - prevA) - std::abs(xb - prevB); if (diff > 0.f) {
            recent = 0;
            ZXP(out) = xa;
        } else if (diff < 0.f) {
            recent = 1;
            ZXP(out) = xb;
        } else { ZXP(out) = recent ? xb : xa; } prevA = xa;
        prevB = xb;);
    unit->mPrevA = prevA;
    unit->mPrevB = prevB;
    unit->mRecent = recent;
}

void MostChange_next_aa(MostChange* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float* b = ZIN(1);
    float prevA = unit->mPrevA;
    float prevB = unit->mPrevB;
    int recent = unit->mRecent;
    LOOP1(
        inNumSamples, float xa = ZXP(a); float xb = ZXP(b); float diff = std::abs(xa - prevA) - std::abs(xb - prevB);
        if (diff > 0.f) {
            recent = 0;
            ZXP(out) = xa;
        } else if (diff < 0.f) {
            recent = 1;
            ZXP(out) = xb;
        } else { ZXP(out) = recent ? xb : xa; } prevA = xa;
        prevB = xb;);
    unit->mPrevA = prevA;
    unit->mPrevB = prevB;
    unit->mRecent = recent;
}

void MostChange_next_ka(MostChange* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float xa = ZIN0(0);
    float* b = ZIN(1);
    float prevA = unit->mPrevA;
    float prevB = unit->mPrevB;
    int recent = unit->mRecent;
    LOOP1(
        inNumSamples, float xb = ZXP(b); float diff = std::abs(xa - prevA) - std::abs(xb - prevB); if (diff > 0.f) {
            recent = 0;
            ZXP(out) = xa;
        } else if (diff < 0.f) {
            recent = 1;
            ZXP(out) = xb;
        } else { ZXP(out) = recent ? xb : xa; } prevA = xa;
        prevB = xb;);
    unit->mPrevA = prevA;
    unit->mPrevB = prevB;
    unit->mRecent = recent;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////

void LeastChange_Ctor(LeastChange* unit) {
    if (INRATE(0) == calc_FullRate) {
        if (INRATE(1) == calc_FullRate) {
            SETCALC(LeastChange_next_aa);
        } else {
            SETCALC(LeastChange_next_ak);
        }
    } else {
        if (INRATE(1) == calc_FullRate) {
            SETCALC(LeastChange_next_ka);
        } else {
            SETCALC(LeastChange_next_aa);
        }
    }
    unit->mPrevA = 0.f;
    unit->mPrevB = 0.f;
    unit->mRecent = 0;
    LeastChange_next_aa(unit, 1);
}

void LeastChange_next_ak(LeastChange* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float xb = ZIN0(1);
    float prevA = unit->mPrevA;
    float prevB = unit->mPrevB;
    int recent = unit->mRecent;
    LOOP1(
        inNumSamples, float xa = ZXP(a); float diff = std::abs(xa - prevA) - std::abs(xb - prevB); if (diff < 0.f) {
            recent = 0;
            ZXP(out) = xa;
        } else if (diff > 0.f) {
            recent = 1;
            ZXP(out) = xb;
        } else { ZXP(out) = recent ? xb : xa; } prevA = xa;
        prevB = xb;);
    unit->mPrevA = prevA;
    unit->mPrevB = prevB;
    unit->mRecent = recent;
}

void LeastChange_next_aa(LeastChange* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float* b = ZIN(1);
    float prevA = unit->mPrevA;
    float prevB = unit->mPrevB;
    int recent = unit->mRecent;
    LOOP1(
        inNumSamples, float xa = ZXP(a); float xb = ZXP(b); float diff = std::abs(xa - prevA) - std::abs(xb - prevB);
        if (diff < 0.f) {
            recent = 0;
            ZXP(out) = xa;
        } else if (diff > 0.f) {
            recent = 1;
            ZXP(out) = xb;
        } else { ZXP(out) = recent ? xb : xa; } prevA = xa;
        prevB = xb;);
    unit->mPrevA = prevA;
    unit->mPrevB = prevB;
    unit->mRecent = recent;
}

void LeastChange_next_ka(LeastChange* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float xa = ZIN0(0);
    float* b = ZIN(1);
    float prevA = unit->mPrevA;
    float prevB = unit->mPrevB;
    int recent = unit->mRecent;
    LOOP1(
        inNumSamples, float xb = ZXP(b); float diff = std::abs(xa - prevA) - std::abs(xb - prevB); if (diff < 0.f) {
            recent = 0;
            ZXP(out) = xa;
        } else if (diff > 0.f) {
            recent = 1;
            ZXP(out) = xb;
        } else { ZXP(out) = recent ? xb : xa; } prevA = xa;
        prevB = xb;);
    unit->mPrevA = prevA;
    unit->mPrevB = prevB;
    unit->mRecent = recent;
}


////////////////////

void LastValue_Ctor(LastValue* unit) {
    if (INRATE(0) == calc_FullRate) {
        SETCALC(LastValue_next_ak);
    } else {
        SETCALC(LastValue_next_kk);
    }

    unit->mPrev = ZIN0(0);
    unit->mCurr = ZIN0(0);
    LastValue_next_kk(unit, 1);
}

void LastValue_next_kk(LastValue* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float inval = ZIN0(0);
    float delta = ZIN0(1);
    float diff = std::abs(inval - unit->mCurr);
    if (diff >= delta) {
        unit->mPrev = unit->mCurr;
        unit->mCurr = inval;
    }
    float level = unit->mPrev;
    LOOP1(inNumSamples, ZXP(out) = level;);
}


void LastValue_next_ak(LastValue* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* in = ZIN(0);
    float delta = ZIN0(1);
    float prev = unit->mPrev;
    float curr = unit->mCurr;

    LOOP1(
        inNumSamples, float inval = ZXP(in); float diff = std::abs(inval - curr); if (diff >= delta) {
            prev = curr;
            curr = inval;
        } ZXP(out) = prev);
    unit->mPrev = prev;
    unit->mCurr = curr;
}


//////////////////////////////////////////////////////////////////////////////////////////

void Done_Ctor(Done* unit) {
    SETCALC(Done_next);

    unit->m_src = unit->mInput[0]->mFromUnit;

    Done_next(unit, 1);
}


void Done_next(Done* unit, int inNumSamples) {
    float* out = OUT(0);
    Unit* src = unit->m_src;
    if (src)
        *out = src->mDone ? 1.f : 0.f;
    else
        *out = 0.f;
}

//////////////////////////////////////////////////////////////////////////////////////////

void FreeSelf_Ctor(FreeSelf* unit) {
    SETCALC(FreeSelf_next);
    unit->m_prevtrig = 0.f;
    FreeSelf_next(unit, 1);
}


void FreeSelf_next(FreeSelf* unit, int inNumSamples) {
    float in = ZIN0(0);
    if (in > 0.f && unit->m_prevtrig <= 0.f) {
        NodeEnd(&unit->mParent->mNode);
    }
    unit->m_prevtrig = in;
}

//////////////////////////////////////////////////////////////////////////////////////////

void PauseSelf_Ctor(PauseSelf* unit) {
    SETCALC(PauseSelf_next);
    unit->m_prevtrig = 0.f;
    PauseSelf_next(unit, 1);
}


void PauseSelf_next(PauseSelf* unit, int inNumSamples) {
    float in = ZIN0(0);
    if (in > 0.f && unit->m_prevtrig <= 0.f) {
        NodeRun(&unit->mParent->mNode, 0);
    }
    unit->m_prevtrig = in;
}

//////////////////////////////////////////////////////////////////////////////////////////

void Pause_Ctor(Pause* unit) {
    SETCALC(Pause_next);

    unit->m_state = 1;

    ZOUT0(0) = ZIN0(0);
}


void Pause_next(Pause* unit, int inNumSamples) {
    float in = ZIN0(0);
    int state = in == 0.f ? 0 : 1;
    if (state != unit->m_state) {
        unit->m_state = state;
        int id = (int)ZIN0(1);
        Node* node = SC_GetNode(unit->mWorld, id);
        if (node) {
            NodeRun(node, state);
        }
    }
    ZOUT0(0) = in;
}

//////////////////////////////////////////////////////////////////////////////////////////

void Free_Ctor(Free* unit) {
    SETCALC(Free_next);

    unit->m_prevtrig = 0.f;

    ZOUT0(0) = ZIN0(0);
}


void Free_next(Free* unit, int inNumSamples) {
    float trig = ZIN0(0);
    if (trig > 0.f && unit->m_prevtrig <= 0) {
        int id = (int)ZIN0(1);
        Node* node = SC_GetNode(unit->mWorld, id);
        if (node) {
            NodeEnd(node);
        }
    }
    unit->m_prevtrig = trig;
    ZOUT0(0) = trig;
}

//////////////////////////////////////////////////////////////////////////////////////////

void FreeSelfWhenDone_Ctor(FreeSelfWhenDone* unit) {
    unit->m_src = unit->mInput[0]->mFromUnit;

    if (unit->m_src) {
        SETCALC(FreeSelfWhenDone_next);
        FreeSelfWhenDone_next(unit, 1);
    } else {
        SETCALC(ClearUnitOutputs);
        ClearUnitOutputs(unit, 1);
    }
}


void FreeSelfWhenDone_next(FreeSelfWhenDone* unit, int inNumSamples) {
    float* out = OUT(0);
    float* in = IN(0);
    Unit* src = unit->m_src;
    if (src->mDone) {
        NodeEnd(&unit->mParent->mNode);
        SETCALC(ClearUnitOutputs);
    }
    *out = *in;
}

//////////////////////////////////////////////////////////////////////////////////////////

void PauseSelfWhenDone_Ctor(PauseSelfWhenDone* unit) {
    unit->m_src = unit->mInput[0]->mFromUnit;

    if (unit->m_src) {
        SETCALC(PauseSelfWhenDone_next);
        PauseSelfWhenDone_next(unit, 1);
    } else {
        SETCALC(ClearUnitOutputs);
        ClearUnitOutputs(unit, 1);
    }
}


void PauseSelfWhenDone_next(PauseSelfWhenDone* unit, int inNumSamples) {
    float* out = OUT(0);
    float* in = IN(0);
    Unit* src = unit->m_src;
    if (src->mDone) {
        NodeRun(&unit->mParent->mNode, 0);
        SETCALC(ClearUnitOutputs);
    }
    *out = *in;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

struct SendPeakRMS : public Unit {
    // rate, level lag, replyid, channel count, [channels, ], cmd name size, [cmdname, ]
    static const int rateIndex = 0;
    static const int levelLagIndex = 1;
    static const int replyIdIndex = 2;
    static const int channelCountIndex = 3;
    static const int signalStartIndex = 4;

    SendPeakRMS(void) {
        SendPeakRMS* unit = this;

        mChannelCount = (unsigned int)IN0(channelCountIndex);
        size_t channelDataAllocSize = mChannelCount * 3 * sizeof(float);

        int cmdSizeIndex = signalStartIndex + mChannelCount;
        size_t cmdNameSize = IN0(cmdSizeIndex);
        size_t cmdNameAllocSize = (cmdNameSize + 1) * sizeof(char);

        void* allocData = RTAlloc(unit->mWorld, channelDataAllocSize + cmdNameAllocSize);
        if (!allocData) {
            Print("SendPeakRMS: RT memory allocation failed\n");
            SETCALC(Unit_next_nop);
            return;
        }

        memset(allocData, 0, channelDataAllocSize);
        mChannelData = (float*)allocData;

        char* cmdName = (char*)(allocData) + channelDataAllocSize;

        size_t cmdNameIndex = cmdSizeIndex + 1;
        for (int i = 0; i < cmdNameSize; i++)
            cmdName[i] = (char)IN0(cmdNameIndex + i);
        cmdName[cmdNameSize] = 0;

        if ((FULLBUFLENGTH & 15) == 0) {
            if (mCalcRate == calc_FullRate)
                SETCALC(SendPeakRMS::perform_a<true>);
            else
                SETCALC(SendPeakRMS::perform_k<true>);
        } else {
            if (mCalcRate == calc_FullRate)
                SETCALC(SendPeakRMS::perform_a<false>);
            else
                SETCALC(SendPeakRMS::perform_k<false>);
        }

        float replyRate = IN0(rateIndex);

        mAudioSamplesPerTick = FULLRATE / replyRate;
        mControlSamplesPerTick = BUFRATE / replyRate;

        mPhaseRemain = (mCalcRate == calc_FullRate) ? mAudioSamplesPerTick : mControlSamplesPerTick;

        float32 lag = ZIN0(levelLagIndex);
        mB1 = (lag != 0.f) ? exp(log001 / (lag * replyRate)) : 0.f;
    }

    ~SendPeakRMS(void) {
        SendPeakRMS* unit = this;
        RTFree(unit->mWorld, mChannelData);
    }

    unsigned int mChannelCount;
    float* mChannelData;

    float mB1;
    int mAudioSamplesPerTick;
    int mControlSamplesPerTick;
    int mPhaseRemain;

    void performLevelLag(float& out, float y0, float& y1) {
        if (y0 >= y1)
            out = y1 = y0;
        else
            out = y1 = y0 + mB1 * (y1 - y0);
    }

    char* getCmdName(void) {
        void* buffer = mChannelData;
        return (char*)(buffer) + mChannelCount * 3 * sizeof(float);
    }

    void sendReply(void) {
        SendPeakRMS* unit = this;
        float* reply = (float*)alloca(mChannelCount * 2 * sizeof(float));


        for (int i = 0; i != mChannelCount; ++i) {
            float& maxLevel = reply[2 * i];
            float& rms = reply[2 * i + 1];

            performLevelLag(maxLevel, mChannelData[2 * i], mChannelData[2 * mChannelCount + i]);

            if (INRATE(signalStartIndex + i) == calc_FullRate)
                rms = std::sqrt(mChannelData[2 * i + 1] / (float)mAudioSamplesPerTick);
            else
                rms = std::sqrt(mChannelData[2 * i + 1] / (float)mControlSamplesPerTick);
        }

        SendNodeReply(&unit->mParent->mNode, (int)ZIN0(replyIdIndex), getCmdName(), mChannelCount * 2, reply);
        memset(mChannelData, 0, mChannelCount * 2 * sizeof(float));
    }

    template <bool simd> void analyzeFullBlock(void) {
        SendPeakRMS* unit = this;
        for (int i = 0; i != mChannelCount; ++i) {
            float* in = IN(signalStartIndex + i);
            int numSamples = INBUFLENGTH(signalStartIndex + i);

            float& level = mChannelData[2 * i];
            float& sqrsum = mChannelData[2 * i + 1];
            if (numSamples == 1)
                nova::peak_rms_vec(in, &level, &sqrsum, 1);
            else {
                if (simd)
                    nova::peak_rms_vec_simd(in, &level, &sqrsum, numSamples);
                else
                    nova::peak_rms_vec(in, &level, &sqrsum, numSamples);
            }
        }
    }

    void analyzePartialBlock(int firstSample, int samplesToAnalyze) {
        SendPeakRMS* unit = this;
        for (int i = 0; i != mChannelCount; ++i) {
            float* in = IN(signalStartIndex + i) + firstSample;
            int numSamples = INBUFLENGTH(signalStartIndex + i);

            float& level = mChannelData[2 * i];
            float& sqrsum = mChannelData[2 * i + 1];
            if (numSamples == 1) {
                if (firstSample == 0)
                    nova::peak_rms_vec(in, &level, &sqrsum, 1);
            } else {
                if (!(samplesToAnalyze & 15) && !(firstSample & 3)) // check for unrolling and alignment
                    nova::peak_rms_vec_simd(in, &level, &sqrsum, samplesToAnalyze);
                else
                    nova::peak_rms_vec(in, &level, &sqrsum, samplesToAnalyze);
            }
        }
    }

    template <bool simd> inline void next_k(int inNumSamples) {
        mPhaseRemain -= 1;

        if (mPhaseRemain <= 0) {
            mPhaseRemain += mControlSamplesPerTick;
            sendReply();
        }

        analyzeFullBlock<simd>();
    }

    template <bool simd> inline void next_a(int inNumSamples) {
        if (mPhaseRemain >= inNumSamples) {
            mPhaseRemain -= inNumSamples;
            analyzeFullBlock<simd>();
        } else {
            if (mPhaseRemain == 0) {
                sendReply();
                mPhaseRemain = mAudioSamplesPerTick;
            }

            int startSample = 0;
            int samplesToAnalyze = std::min(mPhaseRemain, inNumSamples);
            int remain = inNumSamples;

            do {
                analyzePartialBlock(startSample, samplesToAnalyze);

                startSample += samplesToAnalyze;
                mPhaseRemain -= samplesToAnalyze;
                if (mPhaseRemain == 0) {
                    sendReply();
                    mPhaseRemain = mAudioSamplesPerTick;
                }

                remain -= samplesToAnalyze;
                samplesToAnalyze = std::min(remain, mPhaseRemain);
            } while (remain);
        }
    }

    template <bool simd> static void perform_k(Unit* unit, int inNumSamples) {
        static_cast<SendPeakRMS*>(unit)->next_k<simd>(inNumSamples);
    }

    template <bool simd> static void perform_a(Unit* unit, int inNumSamples) {
        static_cast<SendPeakRMS*>(unit)->next_a<simd>(inNumSamples);
    }
};

static void SendPeakRMS_Ctor(SendPeakRMS* unit) { new (unit) SendPeakRMS(); }
static void SendPeakRMS_Dtor(SendPeakRMS* unit) { unit->~SendPeakRMS(); }

////////////////////////////////////////////////////////////////////////////////////////////////////////

PluginLoad(Trigger) {
    ft = inTable;

    DefineSimpleUnit(Trig1);
    DefineSimpleUnit(Trig);
    DefineSimpleUnit(SendTrig);
    DefineDtorUnit(SendReply);
    DefineDtorUnit(Poll);
    DefineSimpleUnit(ToggleFF);
    DefineSimpleUnit(SetResetFF);
    DefineSimpleUnit(Latch);
    DefineSimpleUnit(Gate);
    DefineSimpleUnit(Schmidt);
    DefineSimpleUnit(PulseDivider);
    DefineSimpleUnit(PulseCount);
    DefineSimpleUnit(Stepper);
    DefineSimpleUnit(TDelay);
    DefineSimpleUnit(ZeroCrossing);
    DefineSimpleUnit(Timer);
    DefineSimpleUnit(Sweep);
    DefineSimpleUnit(Phasor);
    DefineSimpleUnit(Peak);
    DefineSimpleUnit(RunningMin);
    DefineSimpleUnit(RunningMax);
    DefineSimpleUnit(PeakFollower);
    DefineSimpleUnit(MostChange);
    DefineSimpleUnit(LeastChange);
    DefineSimpleUnit(LastValue);
    DefineSimpleUnit(Done);
    DefineSimpleUnit(Pause);
    DefineSimpleUnit(FreeSelf);
    DefineSimpleUnit(PauseSelf);
    DefineSimpleUnit(Free);
    DefineSimpleUnit(FreeSelfWhenDone);
    DefineSimpleUnit(PauseSelfWhenDone);

    DefineDtorUnit(SendPeakRMS);
}
