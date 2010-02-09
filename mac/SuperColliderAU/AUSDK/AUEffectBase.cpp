/*	Copyright © 2007 Apple Inc. All Rights Reserved.

	Disclaimer: IMPORTANT:  This Apple software is supplied to you by
			Apple Inc. ("Apple") in consideration of your agreement to the
			following terms, and your use, installation, modification or
			redistribution of this Apple software constitutes acceptance of these
			terms.  If you do not agree with these terms, please do not use,
			install, modify or redistribute this Apple software.

			In consideration of your agreement to abide by the following terms, and
			subject to these terms, Apple grants you a personal, non-exclusive
			license, under Apple's copyrights in this original Apple software (the
			"Apple Software"), to use, reproduce, modify and redistribute the Apple
			Software, with or without modifications, in source and/or binary forms;
			provided that if you redistribute the Apple Software in its entirety and
			without modifications, you must retain this notice and the following
			text and disclaimers in all such redistributions of the Apple Software.
			Neither the name, trademarks, service marks or logos of Apple Inc.
			may be used to endorse or promote products derived from the Apple
			Software without specific prior written permission from Apple.  Except
			as expressly stated in this notice, no other rights or licenses, express
			or implied, are granted by Apple herein, including but not limited to
			any patent rights that may be infringed by your derivative works or by
			other works in which the Apple Software may be incorporated.

			The Apple Software is provided by Apple on an "AS IS" basis.  APPLE
			MAKES NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION
			THE IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS
			FOR A PARTICULAR PURPOSE, REGARDING THE APPLE SOFTWARE OR ITS USE AND
			OPERATION ALONE OR IN COMBINATION WITH YOUR PRODUCTS.

			IN NO EVENT SHALL APPLE BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL
			OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
			SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
			INTERRUPTION) ARISING IN ANY WAY OUT OF THE USE, REPRODUCTION,
			MODIFICATION AND/OR DISTRIBUTION OF THE APPLE SOFTWARE, HOWEVER CAUSED
			AND WHETHER UNDER THEORY OF CONTRACT, TORT (INCLUDING NEGLIGENCE),
			STRICT LIABILITY OR OTHERWISE, EVEN IF APPLE HAS BEEN ADVISED OF THE
			POSSIBILITY OF SUCH DAMAGE.
*/
/*=============================================================================
	AUEffectBase.cpp

=============================================================================*/

#include "AUEffectBase.h"

/*
	This class does not deal as well as it should with N-M effects...

	The problem areas are (if the channels don't match):
		ProcessInPlace if the channels don't match - there will be problems if InputChan != OutputChan
		Bypass - its just passing the buffers through when not processing them

	This will be fixed in a future update...
*/

//_____________________________________________________________________________
//
AUEffectBase::AUEffectBase(	AudioUnit	audioUnit,
							bool 		inProcessesInPlace ) :
	AUBase(audioUnit, 1, 1),		// 1 in bus, 1 out bus
	mBypassEffect(false),
	mParamSRDep (false),
	mProcessesInPlace(inProcessesInPlace)
{
}

//_____________________________________________________________________________
//
AUEffectBase::~AUEffectBase()
{
	Cleanup();
}

//_____________________________________________________________________________
//
void AUEffectBase::Cleanup()
{
	for (KernelList::iterator it = mKernelList.begin(); it != mKernelList.end(); ++it)
		delete *it;

	mKernelList.clear();
}


//_____________________________________________________________________________
//
ComponentResult AUEffectBase::Initialize()
{
		// get our current numChannels for input and output
	SInt16 auNumInputs = (SInt16) GetInput(0)->GetStreamFormat().mChannelsPerFrame;
	SInt16 auNumOutputs = (SInt16) GetOutput(0)->GetStreamFormat().mChannelsPerFrame;

		// does the unit publish specific information about channel configurations?
    const AUChannelInfo *auChannelConfigs = NULL;
    UInt32 numIOconfigs = SupportedNumChannels(&auChannelConfigs);

    if ((numIOconfigs > 0) && (auChannelConfigs != NULL))
    {
        bool foundMatch = false;
        for (UInt32 i = 0; (i < numIOconfigs) && !foundMatch; ++i)
        {
            SInt16 configNumInputs = auChannelConfigs[i].inChannels;
            SInt16 configNumOutputs = auChannelConfigs[i].outChannels;
            if ((configNumInputs < 0) && (configNumOutputs < 0))
            {
					// unit accepts any number of channels on input and output
                if (((configNumInputs == -1) && (configNumOutputs == -2))
					|| ((configNumInputs == -2) && (configNumOutputs == -1)))
                {
				    foundMatch = true;
					// unit accepts any number of channels on input and output IFF they are the same number on both scopes
                }
				else if (((configNumInputs == -1) && (configNumOutputs == -1)) && (auNumInputs == auNumOutputs))
                {
				    foundMatch = true;
					// unit has specified a particular number of channels on both scopes
                }
				else
                    continue;
            }
            else
            {
					// the -1 case on either scope is saying that the unit doesn't care about the
					// number of channels on that scope
                bool inputMatch = (auNumInputs == configNumInputs) || (configNumInputs == -1);
                bool outputMatch = (auNumOutputs == configNumOutputs) || (configNumOutputs == -1);
                if (inputMatch && outputMatch)
                    foundMatch = true;
            }
        }
        if (!foundMatch)
            return kAudioUnitErr_FormatNotSupported;
    }
    else
    {
			// there is no specifically published channel info
			// so for those kinds of effects, the assumption is that the channels (whatever their number)
			// should match on both scopes
        if ((auNumOutputs != auNumInputs) || (auNumOutputs == 0))
		{
		    return kAudioUnitErr_FormatNotSupported;
		}
    }

    MaintainKernels();
    return noErr;
}

ComponentResult		AUEffectBase::Reset(		AudioUnitScope 		inScope,
								 				AudioUnitElement 	inElement)
{
	for (KernelList::iterator it = mKernelList.begin(); it != mKernelList.end(); ++it) {
		AUKernelBase *kernel = *it;
		if (kernel != NULL)
			kernel->Reset();
	}

	return noErr;
}

ComponentResult		AUEffectBase::GetPropertyInfo (AudioUnitPropertyID	inID,
												AudioUnitScope					inScope,
												AudioUnitElement				inElement,
												UInt32 &						outDataSize,
												Boolean &						outWritable)
{
	if (inScope == kAudioUnitScope_Global) {
		switch (inID) {
			case kAudioUnitProperty_BypassEffect:
				outWritable = true;
				outDataSize = sizeof (UInt32);
				return noErr;
			case kAudioUnitProperty_InPlaceProcessing:
				outWritable = true;
				outDataSize = sizeof (UInt32);
				return noErr;
		}
	}
	return AUBase::GetPropertyInfo (inID, inScope, inElement, outDataSize, outWritable);
}


ComponentResult		AUEffectBase::GetProperty (AudioUnitPropertyID 		inID,
									  AudioUnitScope 					inScope,
									  AudioUnitElement			 		inElement,
									  void *							outData)
{
	if (inScope == kAudioUnitScope_Global) {
		switch (inID) {
			case kAudioUnitProperty_BypassEffect:
				*((UInt32*)outData) = (IsBypassEffect() ? 1 : 0);
				return noErr;
			case kAudioUnitProperty_InPlaceProcessing:
				*((UInt32*)outData) = (mProcessesInPlace ? 1 : 0);
				return noErr;
		}
	}
	return AUBase::GetProperty (inID, inScope, inElement, outData);
}


ComponentResult		AUEffectBase::SetProperty(		AudioUnitPropertyID inID,
									   AudioUnitScope 		inScope,
									   AudioUnitElement 	inElement,
									   const void *			inData,
									   UInt32 				inDataSize)
{
	if (inScope == kAudioUnitScope_Global) {
		switch (inID) {
			case kAudioUnitProperty_BypassEffect:
			{
				if (inDataSize < sizeof(UInt32))
					return kAudioUnitErr_InvalidPropertyValue;

				bool tempNewSetting = *((UInt32*)inData) != 0;
					// we're changing the state of bypass
				if (tempNewSetting != IsBypassEffect())
				{
					if (!tempNewSetting && IsBypassEffect() && IsInitialized()) // turning bypass off and we're initialized
						Reset(0, 0);
					SetBypassEffect (tempNewSetting);
				}
				return noErr;
			}
			case kAudioUnitProperty_InPlaceProcessing:
				mProcessesInPlace = (*((UInt32*)inData) != 0);
				return noErr;
		}
	}
	return AUBase::SetProperty (inID, inScope, inElement, inData, inDataSize);
}


void	AUEffectBase::MaintainKernels()
{
	UInt32 nChannels = GetNumberOfChannels();

	if (mKernelList.size() < nChannels) {
		mKernelList.reserve(nChannels);
		for (UInt32 i = mKernelList.size(); i < nChannels; ++i)
			mKernelList.push_back(NewKernel());
	} else
		while (mKernelList.size() > nChannels) {
			AUKernelBase *kernel = mKernelList.back();
			delete kernel;
			mKernelList.pop_back();
		}

	for(unsigned int i = 0; i < nChannels; i++ )
	{
		if(mKernelList[i]) {
			mKernelList[i]->SetLastKernel(i == nChannels-1 );
			mKernelList[i]->SetChannelNum (i);
		}
	}
}

bool		AUEffectBase::StreamFormatWritable(	AudioUnitScope					scope,
												AudioUnitElement				element)
{
	return IsInitialized() ? false : true;
}

ComponentResult		AUEffectBase::ChangeStreamFormat(	AudioUnitScope				inScope,
														AudioUnitElement			inElement,
														const CAStreamBasicDescription & inPrevFormat,
														const CAStreamBasicDescription & inNewFormat)
{
	ComponentResult result = AUBase::ChangeStreamFormat(inScope, inElement, inPrevFormat, inNewFormat);
	if (result == noErr)
	{
		// for the moment this only dependency we know about
		// where a parameter's range may change is with the sample rate
		// and effects are only publishing parameters in the global scope!
		if (GetParamHasSampleRateDependency() && fnotequal(inPrevFormat.mSampleRate, inNewFormat.mSampleRate))
			PropertyChanged(kAudioUnitProperty_ParameterList, kAudioUnitScope_Global, 0);
	}

	return result;
}


// ____________________________________________________________________________
//
//	This method is called (potentially repeatedly) by ProcessForScheduledParams()
//	in order to perform the actual DSP required for this portion of the entire buffer
//	being processed.  The entire buffer can be divided up into smaller "slices"
//	according to the timestamps on the scheduled parameters...
//
ComponentResult	AUEffectBase::ProcessScheduledSlice(	void				*inUserData,
														UInt32				inStartFrameInBuffer,
														UInt32				inSliceFramesToProcess,
														UInt32				inTotalBufferFrames )
{
	ScheduledProcessParams	&sliceParams = *((ScheduledProcessParams*)inUserData);

	AudioUnitRenderActionFlags 	&actionFlags = *sliceParams.actionFlags;
	AudioBufferList 			&inputBufferList = *sliceParams.inputBufferList;
	AudioBufferList 			&outputBufferList = *sliceParams.outputBufferList;

		// fix the size of the buffer we're operating on before we render this slice of time
	for(unsigned int i = 0; i < inputBufferList.mNumberBuffers; i++ ) {
		inputBufferList.mBuffers[i].mDataByteSize =
			(inputBufferList.mBuffers[i].mNumberChannels * inSliceFramesToProcess * sizeof(AudioSampleType));
	}

	for(unsigned int i = 0; i < outputBufferList.mNumberBuffers; i++ ) {
		outputBufferList.mBuffers[i].mDataByteSize =
			(outputBufferList.mBuffers[i].mNumberChannels * inSliceFramesToProcess * sizeof(AudioSampleType));
	}
		// process the buffer
	ComponentResult result = ProcessBufferLists(actionFlags, inputBufferList, outputBufferList, inSliceFramesToProcess );

		// we just partially processed the buffers, so increment the data pointers to the next part of the buffer to process
	for(unsigned int i = 0; i < inputBufferList.mNumberBuffers; i++ ) {
		inputBufferList.mBuffers[i].mData =
			(AudioSampleType *)inputBufferList.mBuffers[i].mData + inputBufferList.mBuffers[i].mNumberChannels * inSliceFramesToProcess;
	}

	for(unsigned int i = 0; i < outputBufferList.mNumberBuffers; i++ ) {
		outputBufferList.mBuffers[i].mData =
			(AudioSampleType *)outputBufferList.mBuffers[i].mData + outputBufferList.mBuffers[i].mNumberChannels * inSliceFramesToProcess;
	}

	return result;
}

// ____________________________________________________________________________
//

ComponentResult 	AUEffectBase::Render(	AudioUnitRenderActionFlags &ioActionFlags,
											const AudioTimeStamp &		inTimeStamp,
											UInt32						nFrames)
{
	if (!HasInput(0))
		return kAudioUnitErr_NoConnection;

	ComponentResult result = noErr;
	AUOutputElement *theOutput = GetOutput(0);	// throws if error

	AUInputElement *theInput = GetInput(0);
	result = theInput->PullInput(ioActionFlags, inTimeStamp, 0 /* element */, nFrames);

	if (result == noErr)
	{
		if(ProcessesInPlace() )
		{
			theOutput->SetBufferList(theInput->GetBufferList() );
		}

		if (ShouldBypassEffect())
		{
			// leave silence bit alone

			if(!ProcessesInPlace() )
			{
				theInput->CopyBufferContentsTo (theOutput->GetBufferList());
			}
		}
		else
		{
			if(mParamList.size() == 0 )
			{
				// this will read/write silence bit
				result = ProcessBufferLists(ioActionFlags, theInput->GetBufferList(), theOutput->GetBufferList(), nFrames);
			}
			else
			{
				// deal with scheduled parameters...

				AudioBufferList &inputBufferList = theInput->GetBufferList();
				AudioBufferList &outputBufferList = theOutput->GetBufferList();

				ScheduledProcessParams processParams;
				processParams.actionFlags = &ioActionFlags;
				processParams.inputBufferList = &inputBufferList;
				processParams.outputBufferList = &outputBufferList;

				// divide up the buffer into slices according to scheduled params then
				// do the DSP for each slice (ProcessScheduledSlice() called for each slice)
				result = ProcessForScheduledParams(	mParamList,
													nFrames,
													&processParams );


				// fixup the buffer pointers to how they were before we started
				for(unsigned int i = 0; i < inputBufferList.mNumberBuffers; i++ ) {
					inputBufferList.mBuffers[i].mData =
						(AudioSampleType *)inputBufferList.mBuffers[i].mData - inputBufferList.mBuffers[i].mNumberChannels * nFrames;
					inputBufferList.mBuffers[i].mDataByteSize =
						(inputBufferList.mBuffers[i].mNumberChannels * nFrames * sizeof(AudioSampleType));
				}

				for(unsigned int i = 0; i < outputBufferList.mNumberBuffers; i++ ) {
					outputBufferList.mBuffers[i].mData =
						(AudioSampleType *)outputBufferList.mBuffers[i].mData - outputBufferList.mBuffers[i].mNumberChannels * nFrames;
					outputBufferList.mBuffers[i].mDataByteSize =
						(outputBufferList.mBuffers[i].mNumberChannels * nFrames * sizeof(AudioSampleType));
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

OSStatus	AUEffectBase::ProcessBufferLists(
									AudioUnitRenderActionFlags &	ioActionFlags,
									const AudioBufferList &			inBuffer,
									AudioBufferList &				outBuffer,
									UInt32							inFramesToProcess )
{
	bool ioSilence;

	bool silentInput = IsInputSilent (ioActionFlags, inFramesToProcess);
	ioActionFlags |= kAudioUnitRenderAction_OutputIsSilence;


	// call the kernels to handle either interleaved or deinterleaved
	if (inBuffer.mNumberBuffers == 1) {
		// interleaved (or mono)
		int channel = 0;

		for (KernelList::iterator it = mKernelList.begin(); it != mKernelList.end(); ++it, ++channel) {
			AUKernelBase *kernel = *it;

			if (kernel != NULL) {
				ioSilence = silentInput;

				// process each interleaved channel individually
				kernel->Process(
					(const AudioSampleType *)inBuffer.mBuffers[0].mData + channel,
					(AudioSampleType *)outBuffer.mBuffers[0].mData + channel,
					inFramesToProcess,
					inBuffer.mBuffers[0].mNumberChannels,
					ioSilence);

				if (!ioSilence)
					ioActionFlags &= ~kAudioUnitRenderAction_OutputIsSilence;
			}
		}
	} else {
		// deinterleaved
		const AudioBuffer *srcBuffer = inBuffer.mBuffers;
		AudioBuffer *destBuffer = outBuffer.mBuffers;

		for (KernelList::iterator it = mKernelList.begin(); it != mKernelList.end();
		++it, ++srcBuffer, ++destBuffer) {
			AUKernelBase *kernel = *it;

			if (kernel != NULL) {
				ioSilence = silentInput;

				kernel->Process(
					(const AudioSampleType *)srcBuffer->mData,
					(AudioSampleType *)destBuffer->mData,
					inFramesToProcess,
					1,
					ioSilence);

				if (!ioSilence)
					ioActionFlags &= ~kAudioUnitRenderAction_OutputIsSilence;
			}
		}
	}

	return noErr;
}

Float64		AUEffectBase::GetSampleRate()
{
	return GetOutput(0)->GetStreamFormat().mSampleRate;
}

UInt32		AUEffectBase::GetNumberOfChannels()
{
	return GetOutput(0)->GetStreamFormat().mChannelsPerFrame;
}

