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

    Chaos Ugens created by Lance Putnam on Mon Jul 19 2004.
 */

#include "SC_PlugIn.h"
#define TWOPI 6.283185307179586
#define PI 3.141592653589793
#define RECPI 0.3183098861837907
#define RECTWOPI 0.1591549430918953
#define ONESIXTH 0.1666666666666667

static InterfaceTable* ft;

struct NonLinear : public Unit {
    double x0, y0, xn, yn, xnm1, ynm1;
    float counter;
    // bool stable;
};

struct CuspN : public NonLinear {};
struct CuspL : public CuspN {
    double frac;
};

struct GbmanN : public NonLinear {};
struct GbmanL : public GbmanN {
    double frac;
};

struct HenonN : public Unit {
    double x0, x1, xn, xnm1, xnm2, a, b;
    float counter;
    bool stable;
};
struct HenonL : public HenonN {
    double frac;
};
struct HenonC : public HenonL {
    double xnm3, c0, c1, c2, c3;
};

struct LatoocarfianN : public NonLinear {};
struct LatoocarfianL : public LatoocarfianN {
    double frac;
};
struct LatoocarfianC : public LatoocarfianL {
    double xnm3, xnm2, c0, c1, c2, c3;
};

struct LinCongN : public NonLinear {};
struct LinCongL : public LinCongN {
    double frac;
};
struct LinCongC : public LinCongL {
    double xnm3, xnm2, c0, c1, c2, c3;
};

struct LorenzN : public NonLinear {
    double z0, zn, znm1;
};
struct LorenzL : public LorenzN {
    double frac;
};

struct QuadN : public NonLinear {};
struct QuadL : public QuadN {
    double frac;
};
struct QuadC : public QuadL {
    double xnm3, xnm2;
    double c0, c1, c2, c3;
};

struct StandardN : public NonLinear {};
struct StandardL : public StandardN {
    double frac;
};

struct FBSineN : public NonLinear {};
struct FBSineL : public FBSineN {
    double frac;
};
struct FBSineC : public FBSineL {
    double xnm3, xnm2;
    double c0, c1, c2, c3;
};

extern "C" {

void CuspN_next(CuspN* unit, int inNumSamples);
void CuspN_Ctor(CuspN* unit);
void CuspL_next(CuspL* unit, int inNumSamples);
void CuspL_Ctor(CuspL* unit);

void GbmanN_next(GbmanN* unit, int inNumSamples);
void GbmanN_Ctor(GbmanN* unit);
void GbmanL_next(GbmanL* unit, int inNumSamples);
void GbmanL_Ctor(GbmanL* unit);

void HenonN_next(HenonN* unit, int inNumSamples);
void HenonN_Ctor(HenonN* unit);
void HenonL_next(HenonL* unit, int inNumSamples);
void HenonL_Ctor(HenonL* unit);
void HenonC_next(HenonC* unit, int inNumSamples);
void HenonC_Ctor(HenonC* unit);

void LinCongN_next(LinCongN* unit, int inNumSamples);
void LinCongN_Ctor(LinCongN* unit);
void LinCongL_next(LinCongL* unit, int inNumSamples);
void LinCongL_Ctor(LinCongL* unit);
void LinCongC_next(LinCongC* unit, int inNumSamples);
void LinCongC_Ctor(LinCongC* unit);

void LatoocarfianN_next(LatoocarfianN* unit, int inNumSamples);
void LatoocarfianN_Ctor(LatoocarfianN* unit);
void LatoocarfianL_next(LatoocarfianL* unit, int inNumSamples);
void LatoocarfianL_Ctor(LatoocarfianL* unit);
void LatoocarfianC_next(LatoocarfianC* unit, int inNumSamples);
void LatoocarfianC_Ctor(LatoocarfianC* unit);

void LorenzL_next(LorenzL* unit, int inNumSamples);
void LorenzL_Ctor(LorenzL* unit);

void QuadN_next(QuadN* unit, int inNumSamples);
void QuadN_Ctor(QuadN* unit);
void QuadL_next(QuadL* unit, int inNumSamples);
void QuadL_Ctor(QuadL* unit);
void QuadC_next(QuadC* unit, int inNumSamples);
void QuadC_Ctor(QuadC* unit);

void StandardN_next(StandardN* unit, int inNumSamples);
void StandardN_Ctor(StandardN* unit);
void StandardL_next(StandardL* unit, int inNumSamples);
void StandardL_Ctor(StandardL* unit);

void FBSineN_next(FBSineN* unit, int inNumSamples);
void FBSineN_Ctor(FBSineN* unit);
void FBSineL_next(FBSineL* unit, int inNumSamples);
void FBSineL_Ctor(FBSineL* unit);
void FBSineC_next(FBSineC* unit, int inNumSamples);
void FBSineC_Ctor(FBSineC* unit);
}

////////////////////////////////////////////////////////////////////////////////

// calc 3rd order interpolation coefs from four points
static inline void ipol3Coef(double xnm3, double xnm2, double xnm1, double xn, double& c0, double& c1, double& c2,
                             double& c3) {
    c0 = xnm2;
    c1 = 0.5f * (xnm1 - xnm3);
    c2 = xnm3 - (2.5f * xnm2) + xnm1 + xnm1 - 0.5f * xn;
    c3 = 0.5f * (xn - xnm3) + 1.5f * (xnm2 - xnm1);
}

// do 3rd order interpolation using coefs
static inline double ipol3(float frac, double c0, double c1, double c2, double c3) {
    return ((c3 * frac + c2) * frac + c1) * frac + c0;
}

// quick 2pi modulo
inline double mod2pi(double in) {
    const double lo = (double)0.;
    const double hi = TWOPI;

    if (in >= hi) {
        in -= hi;
        if (in < hi)
            return in;
    } else if (in < lo) {
        in += hi;
        if (in >= lo)
            return in;
    } else
        return in;

    return in - hi * (double)((int)(in * RECTWOPI));
}

////////////////////////////////////////////////////////////////////////////////

void CuspN_next(CuspN* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float freq = ZIN0(0);
    double a = ZIN0(1);
    double b = ZIN0(2);
    double x0 = ZIN0(3);

    double xn = unit->xn;
    float counter = unit->counter;

    float samplesPerCycle;
    if (freq < unit->mRate->mSampleRate)
        samplesPerCycle = unit->mRate->mSampleRate / sc_max(freq, 0.001f);
    else
        samplesPerCycle = 1.f;

    if (unit->x0 != x0) {
        unit->x0 = xn = x0;
    }

    for (int i = 0; i < inNumSamples; ++i) {
        if (counter >= samplesPerCycle) {
            counter -= samplesPerCycle;

            xn = a - (b * sqrt(sc_abs(xn)));
        }
        counter++;

        ZXP(out) = xn;
    }

    unit->xn = xn;
    unit->counter = counter;
}

void CuspN_Ctor(CuspN* unit) {
    SETCALC(CuspN_next);

    unit->x0 = ZIN0(3);
    unit->xn = unit->x0;
    unit->counter = 0.f;

    CuspN_next(unit, 1);
}

////////////////////////////////////////////////////////////////////////////////

void CuspL_next(CuspL* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float freq = ZIN0(0);
    double a = ZIN0(1);
    double b = ZIN0(2);
    double x0 = ZIN0(3);

    double xn = unit->xn;
    float counter = unit->counter;
    double xnm1 = unit->xnm1;
    double frac = unit->frac;

    float samplesPerCycle;
    double slope;
    if (freq < unit->mRate->mSampleRate) {
        samplesPerCycle = unit->mRate->mSampleRate / sc_max(freq, 0.001f);
        slope = 1.f / samplesPerCycle;
    } else {
        samplesPerCycle = 1.f;
        slope = 1.f;
    }

    if (unit->x0 != x0) {
        xnm1 = xn;
        unit->x0 = xn = x0;
    }

    double dx = xn - xnm1;

    for (int i = 0; i < inNumSamples; ++i) {
        if (counter >= samplesPerCycle) {
            counter -= samplesPerCycle;
            frac = 0.f;

            xnm1 = xn;

            xn = a - (b * sqrt(sc_abs(xn)));

            dx = xn - xnm1;
        }
        counter++;

        ZXP(out) = xnm1 + dx * frac;
        frac += slope;
    }

    unit->xn = xn;
    unit->counter = counter;
    unit->xnm1 = xnm1;
    unit->frac = frac;
}

void CuspL_Ctor(CuspL* unit) {
    SETCALC(CuspL_next);

    unit->x0 = ZIN0(3);
    unit->xn = unit->x0;
    unit->xnm1 = unit->x0;
    unit->counter = 0.f;
    unit->frac = 0.f;

    CuspL_next(unit, 1);
}

////////////////////////////////////////////////////////////////////////////////

void FBSineN_next(FBSineN* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float freq = ZIN0(0);
    double a = ZIN0(1);
    double b = ZIN0(2);
    double c = ZIN0(3);
    double d = ZIN0(4);
    double x0 = ZIN0(5);
    double y0 = ZIN0(6);

    double xn = unit->xn;
    double yn = unit->yn;
    float counter = unit->counter;

    float samplesPerCycle;
    if (freq < unit->mRate->mSampleRate)
        samplesPerCycle = unit->mRate->mSampleRate / sc_max(freq, 0.001f);
    else
        samplesPerCycle = 1.f;

    if ((unit->x0 != x0) || (unit->y0 != y0)) {
        unit->x0 = xn = x0;
        unit->y0 = yn = y0;
    }

    for (int i = 0; i < inNumSamples; ++i) {
        if (counter >= samplesPerCycle) {
            counter -= samplesPerCycle;

            xn = sin(a * yn + b * xn);
            yn = c * yn + d;
            yn = mod2pi(yn);
        }
        counter++;
        ZXP(out) = xn;
    }
    unit->xn = xn;
    unit->yn = yn;
    unit->counter = counter;
}

void FBSineN_Ctor(FBSineN* unit) {
    SETCALC(FBSineN_next);
    unit->x0 = ZIN0(5);
    unit->y0 = ZIN0(6);
    unit->xn = unit->x0;
    unit->yn = unit->y0;
    unit->counter = 0.f;

    FBSineN_next(unit, 1);
}

////////////////////////////////////////////////////////////////////////////////

void FBSineL_next(FBSineL* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float freq = ZIN0(0);
    double a = ZIN0(1);
    double b = ZIN0(2);
    double c = ZIN0(3);
    double d = ZIN0(4);
    double x0 = ZIN0(5);
    double y0 = ZIN0(6);

    double xn = unit->xn;
    double yn = unit->yn;
    double xnm1 = unit->xnm1;
    float counter = unit->counter;
    double frac = unit->frac;

    float samplesPerCycle;
    double slope;
    if (freq < unit->mRate->mSampleRate) {
        samplesPerCycle = unit->mRate->mSampleRate / sc_max(freq, 0.001f);
        slope = 1.f / samplesPerCycle;
    } else {
        samplesPerCycle = 1.f;
        slope = 1.f;
    }

    if ((unit->x0 != x0) || (unit->y0 != y0)) {
        xnm1 = xn;
        unit->x0 = xn = x0;
        unit->y0 = yn = y0;
    }

    double dx = xn - xnm1;

    for (int i = 0; i < inNumSamples; ++i) {
        if (counter >= samplesPerCycle) {
            counter -= samplesPerCycle;
            frac = 0.0;
            xnm1 = xn;

            xn = sin(a * yn + b * xn);
            yn = c * yn + d;
            yn = mod2pi(yn);

            dx = xn - xnm1;
        }
        counter++;
        ZXP(out) = xnm1 + dx * frac;
        frac += slope;
    }
    unit->xn = xn;
    unit->yn = yn;
    unit->xnm1 = xnm1;
    unit->counter = counter;
    unit->frac = frac;
}

void FBSineL_Ctor(FBSineL* unit) {
    SETCALC(FBSineL_next);
    unit->x0 = ZIN0(5);
    unit->y0 = ZIN0(6);
    unit->xn = unit->x0;
    unit->yn = unit->y0;
    unit->xnm1 = unit->x0;
    unit->counter = 0.f;
    unit->frac = 0.0;
    FBSineL_next(unit, 1);
}

////////////////////////////////////////////////////////////////////////////////

void FBSineC_next(FBSineC* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float freq = ZIN0(0);
    double a = ZIN0(1);
    double b = ZIN0(2);
    double c = ZIN0(3);
    double d = ZIN0(4);
    double x0 = ZIN0(5);
    double y0 = ZIN0(6);

    double xn = unit->xn;
    double xnm1 = unit->xnm1;
    double xnm2 = unit->xnm2;
    double xnm3 = unit->xnm3;
    double yn = unit->yn;
    float counter = unit->counter;
    double frac = unit->frac;
    double c0 = unit->c0;
    double c1 = unit->c1;
    double c2 = unit->c2;
    double c3 = unit->c3;

    float samplesPerCycle;
    double slope;
    if (freq < unit->mRate->mSampleRate) {
        samplesPerCycle = unit->mRate->mSampleRate / sc_max(freq, 0.001f);
        slope = 1.f / samplesPerCycle;
    } else
        samplesPerCycle = slope = 1.f;

    if ((unit->x0 != x0) || (unit->y0 != y0)) {
        unit->x0 = xn = x0;
        unit->y0 = y0;
        xnm3 = xnm2;
        xnm2 = xnm1;
        xnm1 = xn;
    }

    for (int i = 0; i < inNumSamples; ++i) {
        if (counter >= samplesPerCycle) {
            counter -= samplesPerCycle;

            frac = 0.0;
            xnm3 = xnm2;
            xnm2 = xnm1;
            xnm1 = xn;

            xn = sin(a * yn + b * xn);
            yn = c * yn + d;
            yn = mod2pi(yn);

            ipol3Coef(xnm3, xnm2, xnm1, xn, c0, c1, c2, c3);
        }
        counter++;
        ZXP(out) = ipol3(frac, c0, c1, c2, c3);
        frac += slope;
    }

    unit->xn = xn;
    unit->xnm1 = xnm1;
    unit->xnm2 = xnm2;
    unit->xnm3 = xnm3;
    unit->yn = yn;
    unit->counter = counter;
    unit->frac = frac;
    unit->c0 = c0;
    unit->c1 = c1;
    unit->c2 = c2;
    unit->c3 = c3;
}

void FBSineC_Ctor(FBSineC* unit) {
    SETCALC(FBSineC_next);

    unit->x0 = ZIN0(5);
    unit->y0 = ZIN0(6);
    unit->xn = unit->x0;
    unit->yn = unit->y0;
    unit->xnm1 = unit->xnm2 = unit->xnm3 = unit->x0;
    unit->counter = 0.f;
    unit->frac = 0.0;
    unit->c0 = unit->c1 = unit->c2 = unit->c3 = 0.;

    FBSineC_next(unit, 1);
}

////////////////////////////////////////////////////////////////////////////////

void GbmanN_next(GbmanN* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float freq = ZIN0(0);

    double xn = unit->xn;
    double yn = unit->yn;
    float counter = unit->counter;

    float samplesPerCycle;
    if (freq < unit->mRate->mSampleRate)
        samplesPerCycle = unit->mRate->mSampleRate / sc_max(freq, 0.001f);
    else
        samplesPerCycle = 1.f;

    for (int i = 0; i < inNumSamples; ++i) {
        if (counter >= samplesPerCycle) {
            counter -= samplesPerCycle;
            double xPrev = xn;
            if (xPrev < 0.f)
                xn = 1.f - yn - xPrev;
            else
                xn = 1.f - yn + xPrev;

            yn = xPrev;
        }
        counter++;
        ZXP(out) = xn;
    }
    unit->xn = xn;
    unit->yn = yn;
    unit->counter = counter;
}

void GbmanN_Ctor(GbmanN* unit) {
    SETCALC(GbmanN_next);
    unit->xn = ZIN0(1);
    unit->yn = ZIN0(2);
    unit->counter = 0.f;
    GbmanN_next(unit, 1);
}

////////////////////////////////////////////////////////////////////////////////

void GbmanL_next(GbmanL* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float freq = ZIN0(0);

    double xn = unit->xn;
    double yn = unit->yn;
    float counter = unit->counter;
    double frac = unit->frac;

    float samplesPerCycle;
    double slope;
    if (freq < unit->mRate->mSampleRate) {
        samplesPerCycle = unit->mRate->mSampleRate / sc_max(freq, 0.001f);
        slope = 1.f / samplesPerCycle;
    } else {
        samplesPerCycle = 1.f;
        slope = 1.f;
    }

    double diff = xn - yn;

    for (int i = 0; i < inNumSamples; ++i) {
        if (counter >= samplesPerCycle) {
            counter -= samplesPerCycle;
            frac = 0.f;

            double xPrev = xn;
            if (xPrev < 0.f)
                xn = 1.f - yn - xPrev;
            else
                xn = 1.f - yn + xPrev;

            yn = xPrev;

            diff = xn - xPrev;
        }
        counter++;
        ZXP(out) = yn + (frac * diff);
        frac += slope;
    }

    unit->xn = xn;
    unit->yn = yn;
    unit->counter = counter;
    unit->frac = frac;
}

void GbmanL_Ctor(GbmanL* unit) {
    SETCALC(GbmanL_next);
    unit->xn = ZIN0(1);
    unit->yn = ZIN0(2);
    unit->counter = 0.f;
    unit->frac = 0.f;
    GbmanL_next(unit, 1);
}

////////////////////////////////////////////////////////////////////////////////

void HenonN_next(HenonN* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float freq = ZIN0(0);
    double a = ZIN0(1);
    double b = ZIN0(2);
    double x0 = ZIN0(3);
    double x1 = ZIN0(4);

    double xn = unit->xn;
    double xnm1 = unit->xnm1;
    double xnm2 = unit->xnm2;
    float counter = unit->counter;
    bool stable = unit->stable;

    float samplesPerCycle;
    if (freq < unit->mRate->mSampleRate)
        samplesPerCycle = unit->mRate->mSampleRate / sc_max(freq, 0.001f);
    else
        samplesPerCycle = 1.f;

    if ((unit->a != a) || (unit->b != b) || (unit->x0 != x0) || (unit->x1 != x1)) {
        if (!stable) {
            xnm2 = x0;
            xnm1 = x0;
            xn = x1;
        }
        stable = true;
        unit->a = a;
        unit->b = b;
        unit->x0 = x0;
        unit->x1 = x1;
    }

    for (int i = 0; i < inNumSamples; ++i) {
        if (counter >= samplesPerCycle) {
            counter -= samplesPerCycle;
            if (stable) {
                xn = 1.f - (a * xnm1 * xnm1) + (b * xnm2);
                // Print("%g, ", xn);
                if ((xn > 1.5f) || (xn < -1.5f)) {
                    stable = false;
                    xn = 0.f;
                    xnm2 = x0;
                    xnm1 = x1;
                } else {
                    xnm2 = xnm1;
                    xnm1 = xn;
                }
            }
        }
        counter++;

        ZXP(out) = xnm2;
    }

    unit->xn = xn;
    unit->xnm1 = xnm1;
    unit->xnm2 = xnm2;
    unit->counter = counter;
    unit->stable = stable;
}

void HenonN_Ctor(HenonN* unit) {
    SETCALC(HenonN_next);

    unit->x0 = ZIN0(3);
    unit->x1 = ZIN0(4);
    unit->xn = unit->x1;
    unit->xnm1 = unit->x0;
    unit->xnm2 = unit->x1;
    unit->a = ZIN0(1);
    unit->b = ZIN0(2);

    unit->counter = 0.f;
    unit->stable = true;

    HenonN_next(unit, 1);
}

////////////////////////////////////////////////////////////////////////////////

void HenonL_next(HenonL* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float freq = ZIN0(0);
    double a = ZIN0(1);
    double b = ZIN0(2);
    double x0 = ZIN0(3);
    double x1 = ZIN0(4);

    double xn = unit->xn;
    double xnm1 = unit->xnm1;
    double xnm2 = unit->xnm2;
    float counter = unit->counter;
    bool stable = unit->stable;
    double frac = unit->frac;

    float samplesPerCycle;
    double slope;
    if (freq < unit->mRate->mSampleRate) {
        samplesPerCycle = unit->mRate->mSampleRate / sc_max(freq, 0.001f);
        slope = 1.f / samplesPerCycle;
    } else
        samplesPerCycle = slope = 1.f;

    if ((unit->a != a) || (unit->b != b) || (unit->x0 != x0) || (unit->x1 != x1)) {
        if (!stable) {
            xnm2 = x0;
            xnm1 = x0;
            xn = x1;
        }
        stable = true;
        unit->a = a;
        unit->b = b;
        unit->x0 = x0;
        unit->x1 = x1;
    }

    double diff = xnm1 - xnm2;

    for (int i = 0; i < inNumSamples; ++i) {
        if (counter >= samplesPerCycle) {
            counter -= samplesPerCycle;
            if (stable) {
                xn = 1.f - (a * xnm1 * xnm1) + (b * xnm2);
                if (xn > 1.5f || xn < -1.5f) {
                    stable = false;
                    diff = 0.f;
                    xn = 1.f;
                    xnm1 = 0.f;
                    xnm2 = 0.f;
                } else {
                    xnm2 = xnm1;
                    xnm1 = xn;
                    diff = xnm1 - xnm2;
                }
                frac = 0.f;
            }
            // Print("slope %g, x2 %g\n", slope, x2);
        }
        counter++;

        ZXP(out) = xnm2 + (diff * frac);
        frac += slope;
    }

    unit->xn = xn;
    unit->xnm1 = xnm1;
    unit->xnm2 = xnm2;
    unit->counter = counter;
    unit->stable = stable;
    unit->frac = frac;
}

void HenonL_Ctor(HenonL* unit) {
    SETCALC(HenonL_next);

    unit->x0 = ZIN0(3);
    unit->x1 = ZIN0(4);
    unit->xn = unit->x1;
    unit->xnm1 = unit->x0;
    unit->xnm2 = unit->x1;
    unit->a = ZIN0(1);
    unit->b = ZIN0(2);
    unit->counter = 0.f;
    unit->stable = true;

    unit->frac = 0.;

    HenonL_next(unit, 1);
}


////////////////////////////////////////////////////////////////////////////////

void HenonC_next(HenonC* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float freq = ZIN0(0);
    double a = ZIN0(1);
    double b = ZIN0(2);
    double x0 = ZIN0(3);
    double x1 = ZIN0(4);

    double xn = unit->xn;
    double xnm1 = unit->xnm1;
    double xnm2 = unit->xnm2;
    double xnm3 = unit->xnm3;
    float counter = unit->counter;
    bool stable = unit->stable;
    double frac = unit->frac;
    double c0 = unit->c0;
    double c1 = unit->c1;
    double c2 = unit->c2;
    double c3 = unit->c3;

    float samplesPerCycle;
    double slope;
    if (freq < unit->mRate->mSampleRate) {
        samplesPerCycle = unit->mRate->mSampleRate / sc_max(freq, 0.001f);
        slope = 1.f / samplesPerCycle;
    } else
        samplesPerCycle = slope = 1.f;

    if ((unit->a != a) || (unit->b != b) || (unit->x0 != x0) || (unit->x1 != x1)) {
        if (!stable) {
            xnm3 = xnm2;
            xnm2 = x0;
            xnm1 = x0;
            xn = x1;
        }
        stable = true;
        unit->a = a;
        unit->b = b;
        unit->x0 = x0;
        unit->x1 = x1;
    }

    for (int i = 0; i < inNumSamples; ++i) {
        if (counter >= samplesPerCycle) {
            counter -= samplesPerCycle;
            frac = 0.0;

            if (stable) {
                xnm3 = xnm2;
                xnm2 = xnm1;
                xnm1 = xn;

                xn = 1.f - (a * xnm1 * xnm1) + (b * xnm2);
                if ((xn > 1.5) || (xn < -1.5)) {
                    stable = false;
                    xn = 1.;
                    xnm1 = xnm2 = xnm3 = 0.;
                }
                ipol3Coef(xnm3, xnm2, xnm1, xn, c0, c1, c2, c3);
            }
            // Print("slope %g, x2 %g\n", slope, x2);
        }
        counter++;
        ZXP(out) = ipol3(frac, c0, c1, c2, c3);
        frac += slope;
    }

    unit->xn = xn;
    unit->xnm1 = xnm1;
    unit->xnm2 = xnm2;
    unit->xnm3 = xnm3;
    unit->counter = counter;
    unit->stable = stable;
    unit->frac = frac;
    unit->c0 = c0;
    unit->c1 = c1;
    unit->c2 = c2;
    unit->c3 = c3;
}

void HenonC_Ctor(HenonC* unit) {
    SETCALC(HenonC_next);

    unit->x0 = ZIN0(3);
    unit->x1 = ZIN0(4);
    unit->xn = unit->x1;
    unit->xnm1 = unit->x0;
    unit->xnm2 = unit->xnm3 = unit->x1;
    unit->a = ZIN0(1);
    unit->b = ZIN0(2);
    unit->counter = 0.f;
    unit->stable = true;
    unit->frac = 0.0;
    unit->c0 = unit->c1 = unit->c2 = unit->c3 = 0.;

    HenonC_next(unit, 1);
}

////////////////////////////////////////////////////////////////////////////////

void LatoocarfianN_next(LatoocarfianN* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float freq = ZIN0(0);
    double a = ZIN0(1);
    double b = ZIN0(2);
    double c = ZIN0(3);
    double d = ZIN0(4);
    double x0 = ZIN0(5);
    double y0 = ZIN0(6);

    double xn = unit->xn;
    double yn = unit->yn;
    float counter = unit->counter;

    float samplesPerCycle;
    if (freq < unit->mRate->mSampleRate)
        samplesPerCycle = unit->mRate->mSampleRate / sc_max(freq, 0.001f);
    else
        samplesPerCycle = 1.f;

    if ((unit->x0 != x0) || (unit->y0 != y0)) {
        unit->x0 = xn = x0;
        unit->y0 = yn = y0;
    }

    for (int i = 0; i < inNumSamples; ++i) {
        if (counter >= samplesPerCycle) {
            counter -= samplesPerCycle;

            const double xnm1 = xn;

            xn = sin(yn * b) + c * sin(xnm1 * b);
            yn = sin(xnm1 * a) + d * sin(yn * a);
        }
        counter++;
        ZXP(out) = xn;
    }

    unit->xn = xn;
    unit->yn = yn;
    unit->counter = counter;
}

void LatoocarfianN_Ctor(LatoocarfianN* unit) {
    SETCALC(LatoocarfianN_next);

    unit->x0 = ZIN0(5);
    unit->y0 = ZIN0(6);
    unit->xn = unit->x0;
    unit->yn = unit->y0;
    unit->counter = 0.f;

    LatoocarfianN_next(unit, 1);
}


////////////////////////////////////////////////////////////////////////////////

void LatoocarfianL_next(LatoocarfianL* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float freq = ZIN0(0);
    double a = ZIN0(1);
    double b = ZIN0(2);
    double c = ZIN0(3);
    double d = ZIN0(4);
    double x0 = ZIN0(5);
    double y0 = ZIN0(6);

    double xn = unit->xn;
    double yn = unit->yn;
    float counter = unit->counter;
    double xnm1 = unit->xnm1;
    double frac = unit->frac;

    float samplesPerCycle;
    double slope;
    if (freq < unit->mRate->mSampleRate) {
        samplesPerCycle = unit->mRate->mSampleRate / sc_max(freq, 0.001f);
        slope = 1.f / samplesPerCycle;
    } else {
        samplesPerCycle = 1.f;
        slope = 1.f;
    }

    if ((unit->x0 != x0) || (unit->y0 != y0)) {
        xnm1 = xn;
        unit->x0 = xn = x0;
        unit->y0 = yn = y0;
    }

    double dx = xn - xnm1;

    for (int i = 0; i < inNumSamples; ++i) {
        if (counter >= samplesPerCycle) {
            counter -= samplesPerCycle;
            frac = 0.f;

            xnm1 = xn;

            xn = sin(yn * b) + c * sin(xnm1 * b);
            yn = sin(xnm1 * a) + d * sin(yn * a);

            dx = xn - xnm1;
        }
        counter++;
        ZXP(out) = xnm1 + dx * frac;
        frac += slope;
    }

    unit->xn = xn;
    unit->yn = yn;
    unit->counter = counter;
    unit->xnm1 = xnm1;
    unit->frac = frac;
}

void LatoocarfianL_Ctor(LatoocarfianL* unit) {
    SETCALC(LatoocarfianL_next);

    unit->x0 = ZIN0(5);
    unit->y0 = ZIN0(6);
    unit->xn = unit->x0;
    unit->yn = unit->y0;
    unit->xnm1 = unit->x0;
    unit->counter = 0.f;
    unit->frac = 0.f;

    LatoocarfianL_next(unit, 1);
}

////////////////////////////////////////////////////////////////////////////////

void LatoocarfianC_next(LatoocarfianC* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float freq = ZIN0(0);
    double a = ZIN0(1);
    double b = ZIN0(2);
    double c = ZIN0(3);
    double d = ZIN0(4);
    double x0 = ZIN0(5);
    double y0 = ZIN0(6);

    double xn = unit->xn;
    double xnm1 = unit->xnm1;
    double xnm2 = unit->xnm2;
    double xnm3 = unit->xnm3;
    double yn = unit->yn;
    float counter = unit->counter;
    double frac = unit->frac;
    double c0 = unit->c0;
    double c1 = unit->c1;
    double c2 = unit->c2;
    double c3 = unit->c3;

    float samplesPerCycle;
    double slope;
    if (freq < unit->mRate->mSampleRate) {
        samplesPerCycle = unit->mRate->mSampleRate / sc_max(freq, 0.001f);
        slope = 1.f / samplesPerCycle;
    } else
        samplesPerCycle = slope = 1.f;

    if ((unit->x0 != x0) || (unit->y0 != y0)) {
        xnm3 = xnm2;
        xnm2 = xnm1;
        xnm1 = xn;
        unit->x0 = xn = x0;
        unit->y0 = yn = y0;
    }

    for (int i = 0; i < inNumSamples; ++i) {
        if (counter >= samplesPerCycle) {
            counter -= samplesPerCycle;

            frac = 0.0;
            xnm3 = xnm2;
            xnm2 = xnm1;
            xnm1 = xn;

            xn = sin(yn * b) + c * sin(xnm1 * b);
            yn = sin(xnm1 * a) + d * sin(yn * a);

            ipol3Coef(xnm3, xnm2, xnm1, xn, c0, c1, c2, c3);
        }
        counter++;
        ZXP(out) = ipol3(frac, c0, c1, c2, c3);
        frac += slope;
    }

    unit->xn = xn;
    unit->xnm1 = xnm1;
    unit->xnm2 = xnm2;
    unit->xnm3 = xnm3;
    unit->yn = yn;
    unit->counter = counter;
    unit->frac = frac;
    unit->c0 = c0;
    unit->c1 = c1;
    unit->c2 = c2;
    unit->c3 = c3;
}

void LatoocarfianC_Ctor(LatoocarfianC* unit) {
    SETCALC(LatoocarfianC_next);

    unit->x0 = ZIN0(5);
    unit->y0 = ZIN0(6);
    unit->xn = unit->x0;
    unit->yn = unit->y0;
    unit->xnm1 = unit->xnm2 = unit->xnm3 = unit->x0;
    unit->counter = 0.f;
    unit->frac = 0.0;
    unit->c0 = unit->c1 = unit->c2 = unit->c3 = unit->x0;

    LatoocarfianC_next(unit, 1);
}

////////////////////////////////////////////////////////////////////////////////

void LinCongN_next(LinCongN* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float freq = ZIN0(0);
    double a = ZIN0(1); // multiplier
    double c = ZIN0(2); // increment
    double m = sc_max(ZIN0(3), 0.001f); // modulus

    double xn = unit->xn;
    float counter = unit->counter;

    double scaleFactor = 2.f / m;
    double xscaled = (xn * scaleFactor) - 1.f;

    float samplesPerCycle;
    if (freq < unit->mRate->mSampleRate)
        samplesPerCycle = unit->mRate->mSampleRate / sc_max(freq, 0.001f);
    else
        samplesPerCycle = 1.f;

    for (int i = 0; i < inNumSamples; ++i) {
        if (counter >= samplesPerCycle) {
            counter -= samplesPerCycle;

            xn = (xn * a) + c;
            xn = sc_mod(xn, m);

            xscaled = (xn * scaleFactor) - 1.f;
        }
        counter++;
        ZXP(out) = xscaled;
    }
    unit->xn = xn;
    unit->counter = counter;
}

void LinCongN_Ctor(LinCongN* unit) {
    SETCALC(LinCongN_next);
    unit->xn = ZIN0(4); // initial x
    unit->counter = 0.f;
    LinCongN_next(unit, 1);
}

////////////////////////////////////////////////////////////////////////////////

void LinCongL_next(LinCongL* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float freq = ZIN0(0);
    double a = ZIN0(1); // multiplier
    double c = ZIN0(2); // increment
    double m = sc_max(ZIN0(3), 0.001f); // modulus

    double xn = unit->xn;
    float counter = unit->counter;
    double xnm1 = unit->xnm1;
    double frac = unit->frac;

    double scaleFactor = 2.f / m;
    double xscaled = (xn * scaleFactor) - 1.f;

    float samplesPerCycle;
    double slope;
    if (freq < unit->mRate->mSampleRate) {
        samplesPerCycle = unit->mRate->mSampleRate / sc_max(freq, 0.001f);
        slope = 1.f / samplesPerCycle;
    } else {
        samplesPerCycle = 1.f;
        slope = 1.f;
    }

    double diff = xscaled - xnm1;

    for (int i = 0; i < inNumSamples; ++i) {
        if (counter >= samplesPerCycle) {
            counter -= samplesPerCycle;
            frac = 0.f;
            xnm1 = xscaled;

            xn = (xn * a) + c;
            xn = sc_mod(xn, m);

            xscaled = (xn * scaleFactor) - 1.f;
            diff = xscaled - xnm1;
        }
        counter++;
        ZXP(out) = xnm1 + (frac * diff);
        frac += slope;
    }

    unit->xn = xn;
    unit->counter = counter;
    unit->xnm1 = xnm1;
    unit->frac = frac;
}

void LinCongL_Ctor(LinCongL* unit) {
    SETCALC(LinCongL_next);
    unit->xn = ZIN0(4);
    unit->counter = 0.f;
    unit->xnm1 = unit->xn;
    unit->frac = 0.f;
    LinCongL_next(unit, 1);
}

////////////////////////////////////////////////////////////////////////////////

void LinCongC_next(LinCongC* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float freq = ZIN0(0);
    double a = ZIN0(1); // multiplier
    double c = ZIN0(2); // increment
    double m = sc_max(ZIN0(3), 0.001f); // modulus

    double xn = unit->xn;
    double xnm1 = unit->xnm1;
    double xnm2 = unit->xnm2;
    double xnm3 = unit->xnm3;
    float counter = unit->counter;
    double frac = unit->frac;
    double c0 = unit->c0;
    double c1 = unit->c1;
    double c2 = unit->c2;
    double c3 = unit->c3;

    double scaleFactor = 2.f / m;
    double xscaled = (xn * scaleFactor) - 1.f;

    float samplesPerCycle;
    double slope;
    if (freq < unit->mRate->mSampleRate) {
        samplesPerCycle = unit->mRate->mSampleRate / sc_max(freq, 0.001f);
        slope = 1.f / samplesPerCycle;
    } else
        samplesPerCycle = slope = 1.f;

    for (int i = 0; i < inNumSamples; ++i) {
        if (counter >= samplesPerCycle) {
            counter -= samplesPerCycle;

            frac = 0.0;
            xnm3 = xnm2;
            xnm2 = xnm1;
            xnm1 = xscaled;

            xn = (xn * a) + c;
            xn = sc_mod(xn, m);

            xscaled = (xn * scaleFactor) - 1.f;

            ipol3Coef(xnm3, xnm2, xnm1, xscaled, c0, c1, c2, c3);
        }
        counter++;
        ZXP(out) = ipol3(frac, c0, c1, c2, c3);
        frac += slope;
    }

    unit->xn = xn;
    unit->xnm1 = xnm1;
    unit->xnm2 = xnm2;
    unit->xnm3 = xnm3;
    unit->counter = counter;
    unit->frac = frac;
    unit->c0 = c0;
    unit->c1 = c1;
    unit->c2 = c2;
    unit->c3 = c3;
}

void LinCongC_Ctor(LinCongC* unit) {
    SETCALC(LinCongC_next);

    unit->x0 = ZIN0(4);
    unit->xn = unit->x0;
    unit->xnm1 = unit->xnm2 = unit->xnm3 = unit->x0;
    unit->counter = 0.f;
    unit->frac = 0.0;
    unit->c0 = unit->c1 = unit->c2 = unit->c3 = unit->x0;

    LinCongC_next(unit, 1);
}

////////////////////////////////////////////////////////////////////////////////

void LorenzL_next(LorenzL* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float freq = ZIN0(0);
    double s = ZIN0(1);
    double r = ZIN0(2);
    double b = ZIN0(3);
    double h = ZIN0(4);
    double x0 = ZIN0(5);
    double y0 = ZIN0(6);
    double z0 = ZIN0(7);

    double xn = unit->xn;
    double yn = unit->yn;
    double zn = unit->zn;
    float counter = unit->counter;
    double xnm1 = unit->xnm1;
    double ynm1 = unit->ynm1;
    double znm1 = unit->znm1;
    double frac = unit->frac;

    float samplesPerCycle;
    double slope;
    if (freq < unit->mRate->mSampleRate) {
        samplesPerCycle = unit->mRate->mSampleRate / sc_max(freq, 0.001f);
        slope = 1.f / samplesPerCycle;
    } else {
        samplesPerCycle = 1.f;
        slope = 1.f;
    }

    if ((unit->x0 != x0) || (unit->y0 != y0) || (unit->z0 != z0)) {
        xnm1 = xn;
        ynm1 = yn;
        znm1 = zn;
        unit->x0 = xn = x0;
        unit->y0 = yn = y0;
        unit->z0 = zn = z0;
    }

    double dx = xn - xnm1;

    for (int i = 0; i < inNumSamples; ++i) {
        if (counter >= samplesPerCycle) {
            counter -= samplesPerCycle;
            frac = 0.f;

            xnm1 = xn;
            ynm1 = yn;
            znm1 = zn;

            double k1x, k2x, k3x, k4x, k1y, k2y, k3y, k4y, k1z, k2z, k3z, k4z, kxHalf, kyHalf, kzHalf;
            double hTimesS = h * s;

            // 4th order Runge-Kutta
            k1x = hTimesS * (ynm1 - xnm1);
            k1y = h * (xnm1 * (r - znm1) - ynm1);
            k1z = h * (xnm1 * ynm1 - b * znm1);
            kxHalf = k1x * 0.5;
            kyHalf = k1y * 0.5;
            kzHalf = k1z * 0.5;

            k2x = hTimesS * (ynm1 + kyHalf - xnm1 - kxHalf);
            k2y = h * ((xnm1 + kxHalf) * (r - znm1 - kzHalf) - (ynm1 + kyHalf));
            k2z = h * ((xnm1 + kxHalf) * (ynm1 + kyHalf) - b * (znm1 + kzHalf));
            kxHalf = k2x * 0.5;
            kyHalf = k2y * 0.5;
            kzHalf = k2z * 0.5;

            k3x = hTimesS * (ynm1 + kyHalf - xnm1 - kxHalf);
            k3y = h * ((xnm1 + kxHalf) * (r - znm1 - kzHalf) - (ynm1 + kyHalf));
            k3z = h * ((xnm1 + kxHalf) * (ynm1 + kyHalf) - b * (znm1 + kzHalf));

            k4x = hTimesS * (ynm1 + k3y - xnm1 - k3x);
            k4y = h * ((xnm1 + k3x) * (r - znm1 - k3z) - (ynm1 + k3y));
            k4z = h * ((xnm1 + k3x) * (ynm1 + k3y) - b * (znm1 + k3z));

            xn = xn + (k1x + 2.0 * (k2x + k3x) + k4x) * ONESIXTH;
            yn = yn + (k1y + 2.0 * (k2y + k3y) + k4y) * ONESIXTH;
            zn = zn + (k1z + 2.0 * (k2z + k3z) + k4z) * ONESIXTH;

            // Euler's method
            //			xn = xnm1 + h * (s * (ynm1 - xnm1));
            //			yn = ynm1 + h * (xnm1 * (r - znm1) - ynm1);
            //			zn = znm1 + h * (xnm1 * ynm1 - b * znm1);

            dx = xn - xnm1;
        }
        counter++;
        ZXP(out) = (xnm1 + dx * frac) * 0.04f;
        frac += slope;
    }

    unit->xn = xn;
    unit->yn = yn;
    unit->zn = zn;
    unit->counter = counter;
    unit->xnm1 = xnm1;
    unit->ynm1 = ynm1;
    unit->znm1 = znm1;
    unit->frac = frac;
}

void LorenzL_Ctor(LorenzL* unit) {
    SETCALC(LorenzL_next);

    unit->x0 = unit->xn = unit->xnm1 = ZIN0(5);
    unit->y0 = unit->yn = unit->ynm1 = ZIN0(6);
    unit->z0 = unit->zn = unit->znm1 = ZIN0(7);
    unit->counter = 0.f;
    unit->frac = 0.f;

    LorenzL_next(unit, 1);
}

////////////////////////////////////////////////////////////////////////////////

void QuadN_next(QuadN* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float freq = ZIN0(0);
    double a = ZIN0(1);
    double b = ZIN0(2);
    double c = ZIN0(3);
    double x0 = ZIN0(4);

    double xn = unit->xn;
    float counter = unit->counter;

    float samplesPerCycle;
    if (freq < unit->mRate->mSampleRate)
        samplesPerCycle = unit->mRate->mSampleRate / sc_max(freq, 0.001f);
    else
        samplesPerCycle = 1.f;

    if (unit->x0 != x0) {
        unit->x0 = xn = x0;
    }

    for (int i = 0; i < inNumSamples; ++i) {
        if (counter >= samplesPerCycle) {
            counter -= samplesPerCycle;

            xn = a * xn * xn + b * xn + c;
        }
        counter++;
        ZXP(out) = xn;
    }

    unit->xn = xn;
    unit->counter = counter;
}

void QuadN_Ctor(QuadN* unit) {
    SETCALC(QuadN_next);

    unit->x0 = ZIN0(4);
    unit->xn = unit->x0;
    unit->counter = 0.f;

    QuadN_next(unit, 1);
}

////////////////////////////////////////////////////////////////////////////////

void QuadL_next(QuadL* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float freq = ZIN0(0);
    double a = ZIN0(1);
    double b = ZIN0(2);
    double c = ZIN0(3);
    double x0 = ZIN0(4);

    double xn = unit->xn;
    float counter = unit->counter;
    double xnm1 = unit->xnm1;
    double frac = unit->frac;

    float samplesPerCycle;
    double slope;
    if (freq < unit->mRate->mSampleRate) {
        samplesPerCycle = unit->mRate->mSampleRate / sc_max(freq, 0.001f);
        slope = 1.f / samplesPerCycle;
    } else {
        samplesPerCycle = 1.f;
        slope = 1.f;
    }

    if (unit->x0 != x0) {
        xnm1 = xn;
        unit->x0 = xn = x0;
    }

    double dx = xn - xnm1;

    for (int i = 0; i < inNumSamples; ++i) {
        if (counter >= samplesPerCycle) {
            counter -= samplesPerCycle;
            frac = 0.f;

            xnm1 = xn;

            xn = a * xn * xn + b * xn + c;

            dx = xn - xnm1;
        }
        counter++;

        ZXP(out) = xnm1 + dx * frac;
        frac += slope;
    }

    unit->xn = xn;
    unit->counter = counter;
    unit->xnm1 = xnm1;
    unit->frac = frac;
}

void QuadL_Ctor(QuadL* unit) {
    SETCALC(QuadL_next);

    unit->x0 = ZIN0(4);
    unit->xn = unit->x0;
    unit->xnm1 = unit->x0;
    unit->counter = 0.f;
    unit->frac = 0.f;

    QuadL_next(unit, 1);
}

////////////////////////////////////////////////////////////////////////////////

void QuadC_next(QuadC* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float freq = ZIN0(0);
    double a = ZIN0(1);
    double b = ZIN0(2);
    double c = ZIN0(3);
    double x0 = ZIN0(4);

    double xn = unit->xn;
    double xnm1 = unit->xnm1;
    double xnm2 = unit->xnm2;
    double xnm3 = unit->xnm3;
    float counter = unit->counter;
    double frac = unit->frac;
    double c0 = unit->c0;
    double c1 = unit->c1;
    double c2 = unit->c2;
    double c3 = unit->c3;

    float samplesPerCycle;
    double slope;
    if (freq < unit->mRate->mSampleRate) {
        samplesPerCycle = unit->mRate->mSampleRate / sc_max(freq, 0.001f);
        slope = 1.f / samplesPerCycle;
    } else
        samplesPerCycle = slope = 1.f;

    if (unit->x0 != x0) {
        xnm3 = xnm2;
        xnm2 = xnm1;
        xnm1 = xn;
        unit->x0 = xn = x0;
    }

    for (int i = 0; i < inNumSamples; ++i) {
        if (counter >= samplesPerCycle) {
            counter -= samplesPerCycle;

            frac = 0.0;
            xnm3 = xnm2;
            xnm2 = xnm1;
            xnm1 = xn;

            xn = a * xn * xn + b * xn + c;

            ipol3Coef(xnm3, xnm2, xnm1, xn, c0, c1, c2, c3);
        }
        counter++;
        ZXP(out) = ipol3(frac, c0, c1, c2, c3);
        frac += slope;
    }

    unit->xn = xn;
    unit->xnm1 = xnm1;
    unit->xnm2 = xnm2;
    unit->xnm3 = xnm3;
    unit->counter = counter;
    unit->frac = frac;
    unit->c0 = c0;
    unit->c1 = c1;
    unit->c2 = c2;
    unit->c3 = c3;
}

void QuadC_Ctor(QuadC* unit) {
    SETCALC(QuadC_next);

    unit->x0 = ZIN0(4);
    unit->xn = unit->x0;
    unit->xnm1 = unit->xnm2 = unit->xnm3 = unit->x0;
    unit->c0 = unit->c1 = unit->c2 = unit->c3 = unit->x0;
    unit->counter = 0.f;
    unit->frac = 0.0;

    QuadC_next(unit, 1);
}

////////////////////////////////////////////////////////////////////////////////

void StandardN_next(StandardN* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float freq = ZIN0(0);
    double k = ZIN0(1);
    double x0 = ZIN0(2);
    double y0 = ZIN0(3);

    double xn = unit->xn;
    double output = (xn - PI) * RECPI;
    double yn = unit->yn;
    float counter = unit->counter;

    float samplesPerCycle;
    if (freq < unit->mRate->mSampleRate)
        samplesPerCycle = unit->mRate->mSampleRate / sc_max(freq, 0.001f);
    else
        samplesPerCycle = 1.f;

    if ((unit->x0 != x0) || (unit->y0 != y0)) {
        unit->x0 = xn = x0;
        unit->y0 = yn = y0;
    }

    for (int i = 0; i < inNumSamples; ++i) {
        if (counter >= samplesPerCycle) {
            counter -= samplesPerCycle;

            yn = yn + k * sin(xn);
            yn = mod2pi(yn);

            xn = xn + yn;
            xn = mod2pi(xn);

            output = (xn - PI) * RECPI;
        }
        counter++;
        ZXP(out) = output;
    }

    unit->xn = xn;
    unit->yn = yn;
    unit->counter = counter;
}

void StandardN_Ctor(StandardN* unit) {
    SETCALC(StandardN_next);

    unit->x0 = ZIN0(2);
    unit->y0 = ZIN0(3);
    unit->xn = unit->x0;
    unit->yn = unit->y0;
    unit->counter = 0.f;

    StandardN_next(unit, 1);
}

////////////////////////////////////////////////////////////////////////////////

void StandardL_next(StandardL* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float freq = ZIN0(0);
    double k = ZIN0(1);
    double x0 = ZIN0(2);
    double y0 = ZIN0(3);

    double xn = unit->xn;
    double yn = unit->yn;
    float counter = unit->counter;
    double xnm1 = unit->xnm1;
    double frac = unit->frac;

    float samplesPerCycle;
    double slope;
    if (freq < unit->mRate->mSampleRate) {
        samplesPerCycle = unit->mRate->mSampleRate / sc_max(freq, 0.001f);
        slope = 1.f / samplesPerCycle;
    } else {
        samplesPerCycle = 1.f;
        slope = 1.f;
    }

    if ((unit->x0 != x0) || (unit->y0 != y0)) {
        xnm1 = xn;
        unit->x0 = xn = x0;
        unit->y0 = yn = y0;
    }

    double dx = xn - xnm1;

    for (int i = 0; i < inNumSamples; ++i) {
        if (counter >= samplesPerCycle) {
            counter -= samplesPerCycle;
            frac = 0.f;

            xnm1 = xn;

            yn = yn + k * sin(xn);
            yn = mod2pi(yn);

            xn = xn + yn;
            xn = mod2pi(xn);

            dx = xn - xnm1;
        }
        counter++;
        ZXP(out) = (xnm1 + dx * frac - PI) * RECPI;
        frac += slope;
    }

    unit->xn = xn;
    unit->yn = yn;
    unit->counter = counter;
    unit->xnm1 = xnm1;
    unit->frac = frac;
}

void StandardL_Ctor(StandardL* unit) {
    SETCALC(StandardL_next);

    unit->x0 = ZIN0(2);
    unit->y0 = ZIN0(3);
    unit->xn = unit->x0;
    unit->yn = unit->y0;
    unit->xnm1 = unit->x0;
    unit->counter = 0.f;
    unit->frac = 0.f;

    StandardL_next(unit, 1);
}

////////////////////////////////////////////////////////////////////////////////


PluginLoad(Chaos) {
    ft = inTable;

    DefineSimpleUnit(CuspN);
    DefineSimpleUnit(CuspL);
    DefineSimpleUnit(FBSineN);
    DefineSimpleUnit(FBSineL);
    DefineSimpleUnit(FBSineC);
    DefineSimpleUnit(GbmanN);
    DefineSimpleUnit(GbmanL);
    DefineSimpleUnit(HenonN);
    DefineSimpleUnit(HenonL);
    DefineSimpleUnit(HenonC);
    DefineSimpleUnit(LatoocarfianN);
    DefineSimpleUnit(LatoocarfianL);
    DefineSimpleUnit(LatoocarfianC);
    DefineSimpleUnit(LinCongN);
    DefineSimpleUnit(LinCongL);
    DefineSimpleUnit(LinCongC);
    DefineSimpleUnit(LorenzL);
    DefineSimpleUnit(QuadN);
    DefineSimpleUnit(QuadL);
    DefineSimpleUnit(QuadC);
    DefineSimpleUnit(StandardN);
    DefineSimpleUnit(StandardL);
}
