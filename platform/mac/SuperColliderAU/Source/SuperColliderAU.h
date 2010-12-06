/*
	SuperColliderAU Copyright (c) 2006 Gerard Roma.

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

#ifndef __SuperColliderAU_h__
#define __SuperColliderAU_h__


#include "AUMIDIEffectBase.h"
#include "SuperColliderAUVersion.h"

#if AU_DEBUG_DISPATCHER
#include "AUDebugDispatcher.h"
#endif

#include <pthread.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/time.h>
#include <CoreFoundation/CoreFoundation.h>

#include "SC_CoreAudio.h"
#include "SC_WorldOptions.h"
#include "SC_World.h"
#include "SC_HiddenWorld.h"

#include "SCProcess.h"
#include "OSCMessages.h"
#include "ControlSpecs.h"
#include "Resources.h"


using namespace std;

const CFStringRef kPortNumberKey = CFSTR("PortNumber");
const CFStringRef kBlockSizeKey = CFSTR("BlockSize");
const CFStringRef kBeatDivKey =  CFSTR("BeatDiv");
const CFStringRef kDoNoteOnKey =  CFSTR("DoNoteOn");
const CFStringRef kNumWireBufsKey =  CFSTR("NumWireBufs");
const CFStringRef kRtMemorySizeKey =  CFSTR("MemorySize");

const int kDefaultPortNumber = 9989;
const int kDefaultBlockSize = 64;
const int kDefaultBeatDiv = 1;
const int kDefaultNumWireBufs = 64;
const int kDefaultRtMemorySize = 8192;

#pragma mark ____SuperColliderAU
class SuperColliderAU : public AUMIDIEffectBase
{
public:
    Boolean haveSpecs;
	SuperColliderAU(AudioUnit component);
    virtual ~SuperColliderAU ();


	virtual	ComponentResult		GetParameterValueStrings(AudioUnitScope			inScope,
														 AudioUnitParameterID	inParameterID,
														 CFArrayRef *			outStrings);

	virtual	ComponentResult		GetParameterInfo(AudioUnitScope			inScope,
												 AudioUnitParameterID	inParameterID,
												 AudioUnitParameterInfo	&outParameterInfo);

	void initState();
	void sendChangedParameters();

	virtual ComponentResult		GetPropertyInfo(AudioUnitPropertyID		inID,
												AudioUnitScope			inScope,
												AudioUnitElement		inElement,
												UInt32 &				outDataSize,
												Boolean	&				outWritable );

	virtual ComponentResult		GetProperty(AudioUnitPropertyID		inID,
											AudioUnitScope			inScope,
											AudioUnitElement 		inElement,
											void *					outData);

 	virtual	bool				SupportsTail () { return false; }


	virtual ComponentResult	Version() { return kSuperColliderAUVersion; }

	virtual ComponentResult		Initialize();


    virtual ComponentResult 	Render(AudioUnitRenderActionFlags &		ioActionFlags,
                                       const AudioTimeStamp &			inTimeStamp,
                                       UInt32							inNumberFrames);


    ComponentResult				Reset(AudioUnitScope	inScope,
									  AudioUnitElement  inElement);

	virtual OSStatus HandleNoteOn(UInt8 inChannel, UInt8 inNoteNumber, UInt8 inVelocity, UInt32 inStartFrame);
	virtual OSStatus HandleNoteOff(UInt8 inChannel, UInt8 inNoteNumber, UInt8 inVelocity, UInt32 inStartFrame) ;


private:

    World* world;
	SCProcess* superCollider;
	Resources* resources;
	OSCMessages* messages;
	ControlSpecs* specs;
	float* state;
    Float64 currentBeat;
    int lastBeatSent;
    int lastTickSent;
    int ticksPerBeat;
	double beatsPerTick;
    Float64 previousBeat;
    int64 gOSCoffset;


    void syncOSCOffsetWithTimeOfDay();
	double nextTickFrames(Float64 beat,Float64 tempo, UInt32 nFrames);
	void resetBeats();
	int64 getOscTime( const AudioTimeStamp & inTimeStamp);

	Boolean doNoteOn;

	typedef struct note{
		long sampleTime;
		short number;
		short velocity;
	} note;
	vector<note> noteEvents;
};


#endif