/*
    WebAudio driver interface.
    Copyright (c) 2020 Hanns Holger Rutz.

    ====================================================================

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

#include "SC_CoreAudio.h"
#include <stdarg.h>
#include "SC_Prototypes.h"
#include "SC_HiddenWorld.h"
#include "SC_WorldOptions.h"
#include "SC_Time.hpp"
#include <math.h>
#include <stdlib.h>

int32 server_timeseed() { return 0; } // TODO

int64 oscTimeNow() { return 0; } // TODO

void initializeScheduler() {}

// TODO: currently a no-op implementation

class SC_WebAudioDriver : public SC_AudioDriver {

protected:
    virtual bool DriverSetup(int* outNumSamplesPerCallback, double* outSampleRate);
    virtual bool DriverStart();
    virtual bool DriverStop();

public:
    SC_WebAudioDriver(struct World* inWorld);
    virtual ~SC_WebAudioDriver();

    int WebAudioCallback(const void* input, void* output, unsigned long frameCount);
};

SC_AudioDriver* SC_NewAudioDriver(struct World* inWorld) { return new SC_WebAudioDriver(inWorld); }

SC_WebAudioDriver::SC_WebAudioDriver(struct World* inWorld):
    SC_AudioDriver(inWorld) //,
    // mStream(0)
{
}

SC_WebAudioDriver::~SC_WebAudioDriver() {
}

static int SC_WebAudioStreamCallback(const void* input, void* output, unsigned long frameCount,
                                      void* userData) {
    SC_WebAudioDriver* driver = (SC_WebAudioDriver*)userData;

    return driver->WebAudioCallback(input, output, frameCount);
}

void sc_SetDenormalFlags();

int SC_WebAudioDriver::WebAudioCallback(const void* input, void* output, unsigned long frameCount) {
    sc_SetDenormalFlags();
    World* world = mWorld;
    (void) frameCount; // suppress unused parameter warnings

    mAudioSync.Signal();

    return 0;
}

bool SC_WebAudioDriver::DriverSetup(int* outNumSamples, double* outSampleRate) {
    return true;
}

bool SC_WebAudioDriver::DriverStart() {
    return true;
}

bool SC_WebAudioDriver::DriverStop() {
    return true;
}
