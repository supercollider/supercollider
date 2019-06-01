/*
    AudioUnits driver interface.
    Copyright (c) 2006-2008 Gerard Roma.

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
#if SC_AUDIO_API == SC_AUDIO_API_AUDIOUNITS
#    include "SC_AU.h"


void AUCallback(SC_AUAudioDriver* driver, AudioBufferList* in, AudioBufferList* out, AudioTimeStamp* inTimeStamp,
                UInt32 inFramesToProcess, Float64 sampleRate, int64 oscTime) {
    driver->Callback(in, out, inTimeStamp, inFramesToProcess, sampleRate, oscTime);
}

void sc_SetDenormalFlags();

SC_AudioDriver* SC_NewAudioDriver(struct World* inWorld) { return new SC_AUAudioDriver(inWorld); }

SC_AUAudioDriver::SC_AUAudioDriver(struct World* inWorld): SC_CoreAudioDriver(inWorld) {}

SC_AUAudioDriver::~SC_AUAudioDriver() {}


bool SC_AUAudioDriver::DriverSetup(int* outNumSamples, double* outSampleRate) {
    // params are set into the wolrd options at AU initialize
    if (!mPreferredSampleRate && !mPreferredHardwareBufferFrameSize)
        return false;
    *outSampleRate = mPreferredSampleRate;
    *outNumSamples = mPreferredHardwareBufferFrameSize;
    return true;
}

bool SC_AUAudioDriver::DriverStart() { return true; }

bool SC_AUAudioDriver::DriverStop() { return true; }


void SC_AUAudioDriver::Callback(const AudioBufferList* in, AudioBufferList* out, AudioTimeStamp* inTimeStamp,
                                UInt32 inFramesToProcess, Float64 sampleRate, int64 oscTime) {
    this->mNumSamplesPerCallback = inFramesToProcess;
    this->mOSCincrement = (int64)(this->mOSCincrementNumerator / sampleRate);
    this->Run(in, out, oscTime);
}
#endif