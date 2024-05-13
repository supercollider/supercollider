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
/*
#include "SC_CoreAudio.h"
#include <stdarg.h>
#include "SC_Prototypes.h"
#include "SC_HiddenWorld.h"
#include "SC_WorldOptions.h"
#include <stdlib.h>

#ifdef _WIN32
#include "SC_Win32Utils.h"
#else
#include <sys/time.h>
#endif
*/

// =====================================================================
// SC_TimeDLL
//
//   Delay-Locked-Loop after
//   Fons Adriaensen, "Using a DLL to filter time"
#ifndef M_PI
#    define M_PI 3.14159265358979323846
#endif
#define SC_TIME_DLL_BW 0.012

class SC_TimeDLL {
public:
    SC_TimeDLL(): m_b(0.), m_c(0.), m_t0(0.), m_t1(0.), m_e2(0.), m_np(0), m_ei(0.), m_ec(0) {}

    void Reset(double sampleRate, uint32_t periodFrames, double bandWidth, double t) {
        // compute coefficients
        m_np = periodFrames;
        m_b = 2 * M_PI * bandWidth * m_np / sampleRate;
        m_c = m_b * m_b / 2.;
        // initialize filter
        double tp = m_np / sampleRate;
        m_e2 = tp;
        m_t0 = t;
        m_t1 = t + tp;
        // initialize statistics
        m_ei = 0.;
        m_ec = 0;
    }

    void Update(double t) {
        // compute error
        double e = m_e = t - m_t1;
        // update filter
        m_t0 = m_t1;
        m_t1 += m_b * e + m_e2;
        m_e2 += m_c * e;
        // collect statistics
        m_ei += e;
        m_ec++;
    }

    double PeriodTime() const { return m_t0; }
    double NextPeriodTime() const { return m_t1; }
    double Period() const { return m_t1 - m_t0; }
    double SampleRate() const { return m_np / Period(); }
    double Error() const { return m_e; }
    double AvgError() const { return m_ec > 0 ? m_ei / m_ec : 0; }

private:
    double m_b, m_c;
    double m_t0, m_t1, m_e, m_e2;
    int m_np;
    double m_ei;
    int m_ec;
};
