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




#if SC_AUDIO_API == SC_AUDIO_API_AUDIOUNITS
#ifndef _SC_AU_
#define _SC_AU_


#include "SC_CoreAudio.h"
#include "SC_Prototypes.h"
#include "SC_HiddenWorld.h"
#include <CoreAudio/HostTime.h>

class SC_AUAudioDriver : public SC_CoreAudioDriver
{

protected:
	virtual bool DriverSetup(int* outNumSamplesPerCallback, double* outSampleRate);
	virtual bool DriverStart();
	virtual bool DriverStop();

public:
    SC_AUAudioDriver(struct World *inWorld);
	virtual ~SC_AUAudioDriver();
    void Callback (const AudioBufferList* in, AudioBufferList* out, AudioTimeStamp* inTimeStamp, UInt32 inFramesToProcess, Float64 sampleRate, int64 oscTime);
};

extern "C" {
	void AUCallback(SC_AUAudioDriver *driver,AudioBufferList* in, AudioBufferList* out, AudioTimeStamp* inTimeStamp, UInt32 inFramesToProcess, Float64 sampleRate, int64 oscTime);
}



#endif
#endif