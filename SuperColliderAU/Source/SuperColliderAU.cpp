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

#include "SuperColliderAU.h"



COMPONENT_ENTRY(SuperColliderAU)

SuperColliderAU::SuperColliderAU(AudioUnit component)
        : AUMIDIEffectBase(component)
{
    CreateElements();
    resources = new Resources();
    messages = new OSCMessages();
    superCollider = new SCProcess();
    this->haveSpecs = false;
    this->previousBeat = 0;
    this->lastBeatSent = 0;
    this->lastTickSent = -1;
    this->ticksPerBeat = 0;
	this->doNoteOn = false;
}


SuperColliderAU::~SuperColliderAU () {
    if(superCollider){
        //superCollider->quit();
        delete superCollider;
    }
	if(messages)delete messages;
	if(resources) delete resources;
    if (state) delete state;
}


ComponentResult	SuperColliderAU::Reset(AudioUnitScope inScope, AudioUnitElement inElement){
    printf("SuperColliderAU:reset \n");
    fflush(stdout);
	return noErr;
}


ComponentResult	SuperColliderAU::GetParameterValueStrings(	AudioUnitScope			inScope,
															AudioUnitParameterID	inParameterID,
															CFArrayRef *			outStrings)
{
    return kAudioUnitErr_InvalidProperty;
}



ComponentResult	SuperColliderAU::GetParameterInfo(	AudioUnitScope			inScope,
													AudioUnitParameterID	inParameterID,
													AudioUnitParameterInfo	&outParameterInfo )
{
    ComponentResult result = noErr;

	if (inScope != kAudioUnitScope_Global)
		return kAudioUnitErr_InvalidScope;

    if (inParameterID==0)
    {
        AUBase::FillInParameterName (outParameterInfo, CFSTR("Port"), true);
        outParameterInfo.unit = kAudioUnitParameterUnit_Generic;
        outParameterInfo.minValue = 0;
        outParameterInfo.maxValue = 9999;
        outParameterInfo.defaultValue = superCollider->portNum;
        outParameterInfo.flags = kAudioUnitParameterFlag_IsReadable;
    }

    else if (inScope == kAudioUnitScope_Global && this->haveSpecs)
    {
        if ( inParameterID < specs->numberOfParameters+1 ){
			AUBase::FillInParameterName (outParameterInfo, specs->getName(inParameterID-1, kParamNameSpecKey) , false);
			outParameterInfo.minValue = specs->getValue(inParameterID-1, kMinValueSpecKey);
			outParameterInfo.maxValue = specs->getValue(inParameterID-1, kMaxValueSpecKey);
			outParameterInfo.defaultValue = specs->getValue(inParameterID-1, kDefaultValueSpecKey);
			outParameterInfo.unit = specs->getValue(inParameterID-1, kUnitSpecKey);
			outParameterInfo.flags = 	kAudioUnitParameterFlag_IsWritable | kAudioUnitParameterFlag_IsReadable | specs->getDisplayFlag(inParameterID-1);
        }
        else  result = kAudioUnitErr_InvalidParameter;
    }
    else
    {
        result = kAudioUnitErr_InvalidParameter;
    }
    return result;
}


ComponentResult		SuperColliderAU::GetPropertyInfo (AudioUnitPropertyID	inID,
        AudioUnitScope		inScope,
        AudioUnitElement	inElement,
        UInt32 &		outDataSize,
        Boolean &		outWritable)
{
    return AUMIDIEffectBase::GetPropertyInfo (inID, inScope, inElement, outDataSize, outWritable);
}




ComponentResult		SuperColliderAU::GetProperty(	AudioUnitPropertyID inID,
        AudioUnitScope 		inScope,
        AudioUnitElement 	inElement,
        void *			outData )
{
    return AUMIDIEffectBase::GetProperty (inID, inScope, inElement, outData);
}



ComponentResult SuperColliderAU::Initialize()
{

    WorldOptions options = kDefaultWorldOptions;
    options.mPreferredSampleRate=GetSampleRate();
    options.mBufLength=kDefaultBlockSize;
    options.mPreferredHardwareBufferFrameSize = GetMaxFramesPerSlice();
	options.mMaxWireBufs=kDefaultNumWireBufs;
    options.mRealTimeMemorySize=kDefaultRtMemorySize;

    CFStringRef pluginsPath = resources->getResourcePath(resources->SC_PLUGIN_PATH);
    CFStringRef synthdefsPath = resources->getResourcePath(resources->SC_SYNTHDEF_PATH);
    CFDictionaryRef plugSpec = (CFDictionaryRef)resources->getPropertyList(resources->PLUGIN_SPEC_FILE);
    CFDictionaryRef serverConfig = (CFDictionaryRef)resources->getPropertyList(resources->SERVER_CONFIG_FILE);

	int udpPortNum = 9989;

    if (CFDictionaryContainsKey(serverConfig, kPortNumberKey)){
        CFNumberGetValue( (CFNumberRef) CFDictionaryGetValue(serverConfig, kPortNumberKey), kCFNumberIntType,(void *)&udpPortNum);
    }

    if (CFDictionaryContainsKey(serverConfig, kBlockSizeKey)){
        CFNumberGetValue( (CFNumberRef) CFDictionaryGetValue(serverConfig, kBlockSizeKey), kCFNumberIntType,(void *)&options.mBufLength);
    }

    if (CFDictionaryContainsKey(serverConfig, kBeatDivKey)){
        CFNumberGetValue( (CFNumberRef) CFDictionaryGetValue(serverConfig, kBeatDivKey), kCFNumberIntType,(void *)&ticksPerBeat);
		this->beatsPerTick = 1.0 / ticksPerBeat;
    }

    if (CFDictionaryContainsKey(serverConfig, kDoNoteOnKey)){
        //CFNumberGetValue( (CFNumberRef) CFDictionaryGetValue(serverConfig, kDoNoteOnKey),kCFNumberIntType,(void *)&doNoteOn);
		this->doNoteOn = CFBooleanGetValue( (CFBooleanRef) CFDictionaryGetValue(serverConfig, kDoNoteOnKey));
    }

	if (CFDictionaryContainsKey(serverConfig, kNumWireBufsKey)){
		CFNumberGetValue( (CFNumberRef) CFDictionaryGetValue(serverConfig, kNumWireBufsKey), kCFNumberIntType,(void *)&options.mMaxWireBufs);
	}
	if (CFDictionaryContainsKey(serverConfig, kRtMemorySizeKey)){
		CFNumberGetValue( (CFNumberRef) CFDictionaryGetValue(serverConfig, kRtMemorySizeKey),kCFNumberIntType,(void *)&options.mRealTimeMemorySize);
	}


	syncOSCOffsetWithTimeOfDay();

    superCollider->startUp(options, pluginsPath, synthdefsPath, udpPortNum);

    if (plugSpec) {
        this->haveSpecs = true;
        specs=new ControlSpecs(plugSpec);
        superCollider->synthName=specs->synthName;
        superCollider->makeSynth();
        Globals()->UseIndexedParameters(specs->numberOfParameters+1);
        state = new float[specs->numberOfParameters];
        initState();
    }
    else {
        this->haveSpecs = false;
        Globals()->UseIndexedParameters(1);
    }

    SetParameter(0, superCollider->portNum);
    scprintf("*******************************************************\n");
    scprintf("SuperColliderAU Initialized \n");
	scprintf("SuperColliderAU provided under the GNU GPL license. http://www.gnu.org/licenses/gpl-2.0.txt \n");
	scprintf("SuperColliderAU source code: http://supercollider.sf.net \n");
    scprintf("SuperColliderAU mPreferredHardwareBufferFrameSize: %d \n",options.mPreferredHardwareBufferFrameSize );
    scprintf("SuperColliderAU mBufLength: %d \n",options.mBufLength );
    scprintf("SuperColliderAU  port: %d \n", superCollider->portNum );
	scprintf("SuperColliderAU  mMaxWireBufs: %d \n", options.mMaxWireBufs );
    scprintf("SuperColliderAU  mRealTimeMemorySize: %d \n", options.mRealTimeMemorySize );
	scprintf("*******************************************************\n");
    fflush(stdout);
    return noErr;
}


void SuperColliderAU::initState()
{
    for (int i = 0;i< specs->numberOfParameters;i++)
    {
        state[i] =  specs->getValue(i, kDefaultValueSpecKey);
        SetParameter(i+1, state[i]);
    }
}


void SuperColliderAU::sendChangedParameters()
{
    for (int i = 0;i< specs->numberOfParameters;i++)
    {
        float parameterValue =  GetParameter(i+1);
        if (parameterValue != state[i] )
        {
            superCollider->sendParamChangeMessage(specs->getName(i, kParamNameSpecKey), parameterValue);
            state[i] = parameterValue;
        }
    }
}



ComponentResult SuperColliderAU::Render(    AudioUnitRenderActionFlags &ioActionFlags,
                                                const AudioTimeStamp &		inTimeStamp,
                                                UInt32						nFrames)
{

	ComponentResult result = noErr;
	AUOutputElement *theOutput = GetOutput(0);	// throws if error
	AUInputElement *theInput = GetInput(0);
	result = theInput->PullInput(ioActionFlags, inTimeStamp, 0, nFrames);
	Float64 sampleRate = GetSampleRate();

	if (result == noErr)
	{

		theInput->CopyBufferContentsTo (theOutput->GetBufferList());

		if(ProcessesInPlace()) theOutput->SetBufferList(theInput->GetBufferList());
		if (ShouldBypassEffect())
		{
			if(!ProcessesInPlace() )
			{
				theInput->CopyBufferContentsTo (theOutput->GetBufferList());
			}
		}
		else
		{

            if (IsInitialized())
            {
                const AudioBufferList & inBuffer = theInput->GetBufferList();
                AudioBufferList & outBuffer  = theOutput->GetBufferList();

                UInt32 inNumBuffers = inBuffer.mNumberBuffers;
                UInt32 outNumBuffers = outBuffer.mNumberBuffers;
                if ( (inNumBuffers < 1) || (outNumBuffers < 1) ){
                    result = kAudioUnitErr_FormatNotSupported;
                } else {
					int64 oscTime = getOscTime( inTimeStamp);
					const double kSecondsToOSC  = 4294967296.;

					Boolean playing = false;
					Boolean changed = false;
					Boolean looping = false;
					Float64 startBeat, endBeat;

					int err = CallHostTransportState(&playing, &changed, NULL, &looping, &startBeat, &endBeat);//@TODO
					if(changed) resetBeats();
					if (playing && ticksPerBeat>0){
						Float64 beat = 0;
						Float64 tempo = 0;
						int err = CallHostBeatAndTempo(&beat, &tempo);
							if (err ==noErr){
								double secondsPerBeat =  60.0 / tempo;
								double samplesPerTick = secondsPerBeat * sampleRate / ticksPerBeat;
								int totalFrames=nFrames;
								if(looping){
									if(beat<0)beat = endBeat+beat;
									double loopEndFrames=(endBeat-beat) * (secondsPerBeat * sampleRate);
									if(loopEndFrames>0 &&loopEndFrames<nFrames){
										totalFrames=loopEndFrames;
									}
								}

								double tickFrames= nextTickFrames(beat,tempo, nFrames);

								while (tickFrames < totalFrames){
									double tickSecs = tickFrames / sampleRate;
									double tickOSC = tickSecs * kSecondsToOSC;
									int64 nextTickOsc = (int64) oscTime +tickOSC ;
									if (tickFrames==0)nextTickOsc=0;
									superCollider->sendTick( nextTickOsc,0 );

									if(++lastTickSent>=ticksPerBeat){
										lastBeatSent++;
										lastTickSent=0;
									}
									tickFrames += samplesPerTick;
								}
						}
					}
                    if (haveSpecs) sendChangedParameters();
					uint i=0;
					for (i=0;i<noteEvents.size();i++){
						double eventSecs = (noteEvents[i].sampleTime-inTimeStamp.mSampleTime) / sampleRate ;
						superCollider->sendNote(oscTime,noteEvents[i].number,noteEvents[i].velocity);
					}
					noteEvents.clear();
                    superCollider->run(&inBuffer, &outBuffer, nFrames, inTimeStamp, sampleRate,oscTime);
                }
            }
		}

		if ( (ioActionFlags & kAudioUnitRenderAction_OutputIsSilence) && !ProcessesInPlace() )
		{
			AUBufferList::ZeroBuffer(theOutput->GetBufferList() );
		}
	}
	return result;
}



void SuperColliderAU::resetBeats(){
		this->previousBeat = 0;
		this->lastBeatSent = 0;
		this->lastTickSent = -1;
		superCollider->sendTick( 0,1 );
}


double SuperColliderAU::nextTickFrames(Float64 beat, Float64 tempo, UInt32 nFrames){

			double beatsPerCallback;

			if (beat<previousBeat){//looping
				lastBeatSent = (int) beat;
				lastTickSent =  (int) ((beat - ((int)beat)) / beatsPerTick);
				if(lastTickSent==0)lastTickSent=-1;
			}
			if(beat==0)resetBeats();
			if (lastBeatSent == 0)lastBeatSent = (int) beat;

			previousBeat=beat;

			double secondsPerCallBack = ((double) nFrames) / GetSampleRate();
			double secondsPerBeat =  60.0 / tempo;
			beatsPerCallback =  secondsPerCallBack / secondsPerBeat;

			int nextTick=lastTickSent+1;
            int nextBeat = lastBeatSent;
            double nextTickBeats = 0;

			if (nextTick == ticksPerBeat){
					nextTick = 0;
					nextBeat = lastBeatSent+1;
					int curBeatInt = (int)beat;
					if (curBeatInt ==nextBeat){
						nextTickBeats = beat - curBeatInt;
					}
					else{
						nextTickBeats = 1- (beat - curBeatInt);
					}
			} else {
				nextTickBeats = (nextBeat + (nextTick * beatsPerTick)) - beat;
				if (nextTickBeats<0)nextTickBeats=0;
			}
			return nextTickBeats * nFrames / beatsPerCallback;
}



// from scsynth
void SuperColliderAU::syncOSCOffsetWithTimeOfDay()
{
	// generate a value gOSCoffset such that
	// (gOSCOffset + systemTimeInOSCunits)
	// is equal to gettimeofday time in OSCunits.
	// Then if this machine is synced via NTP, we are synced with the world.
	// more accurate way to do this??

	struct timeval tv;

	int64 systemTimeBefore, systemTimeAfter, diff;
	int64 minDiff = 0x7fffFFFFffffFFFFLL;

	// take best of several tries
	const int numberOfTries = 5;
	int64 newOffset = gOSCoffset;
	for (int i=0; i<numberOfTries; ++i) {
		systemTimeBefore = AudioGetCurrentHostTime();
		gettimeofday(&tv, 0);
		systemTimeAfter = AudioGetCurrentHostTime();

		diff = systemTimeAfter - systemTimeBefore;
		if (diff < minDiff) {
			minDiff = diff;
			// assume that gettimeofday happens halfway between AudioGetCurrentHostTime calls
			int64 systemTimeBetween = systemTimeBefore + diff/2;
			int64 systemTimeInOSCunits = (int64)((double)AudioConvertHostTimeToNanos(systemTimeBetween) * kNanosToOSCunits);
			int64 timeOfDayInOSCunits  = ((int64)(tv.tv_sec + kSECONDS_FROM_1900_to_1970) << 32)
                + (int64)(tv.tv_usec * kMicrosToOSCunits);
			newOffset = timeOfDayInOSCunits - systemTimeInOSCunits;
		}
	}
	this->gOSCoffset = newOffset;
}

int64 SuperColliderAU::getOscTime( const AudioTimeStamp & inTimeStamp){
	int64 targetTimeOSC;

	if (inTimeStamp.mFlags & kAudioTimeStampHostTimeValid){
			targetTimeOSC = (int64)((double)AudioConvertHostTimeToNanos(inTimeStamp.mHostTime) * kNanosToOSCunits);
			return targetTimeOSC;
	}
	else{
		/*
		UInt32 size = sizeof(Float64);
		Float64 outPresentationLatency;
		ComponentResult result = AudioUnitGetProperty (mComponentInstance, kAudioUnitProperty_PresentationLatency, kAudioUnitScope_Output, 0, &outPresentationLatency, &size);
		*/
		int64 hostTime = AudioGetCurrentHostTime();
		targetTimeOSC = (int64)((double)AudioConvertHostTimeToNanos(hostTime) * kNanosToOSCunits);
		return this->gOSCoffset + targetTimeOSC;
	}

}



// EXPERIMENTAL


OSStatus SuperColliderAU::HandleNoteOn(UInt8 inChannel, UInt8 inNoteNumber, UInt8 inVelocity, UInt32 inStartFrame) {
	if(this->doNoteOn){
		note event;
		event.number = inNoteNumber;
		event.velocity = inVelocity;
		event.sampleTime = inStartFrame;
		noteEvents.push_back(event);
	}
	return noErr;
}


OSStatus SuperColliderAU::HandleNoteOff(UInt8 inChannel, UInt8 inNoteNumber, UInt8 inVelocity, UInt32 inStartFrame) {
	if(this->doNoteOn){
		note event;
		event.number = inNoteNumber;
		event.velocity = 0;
		event.sampleTime = inStartFrame;
		noteEvents.push_back(event);
	}
	return noErr;
}
