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
	AUScopeElement.cpp

=============================================================================*/

#include "AUScopeElement.h"
#include "AUBase.h"
#if !defined(__COREAUDIO_USE_FLAT_INCLUDES__)
	#include <CoreServices/CoreServices.h>
#else
	#include <Endian.h>
#endif

//_____________________________________________________________________________
//
//	By default, parameterIDs may be arbitrarily spaced, and an STL map
//  will be used for access.  Calling UseIndexedParameters() will
//	instead use an STL vector for faster indexed access.
//	This assumes the paramIDs are numbered 0.....inNumberOfParameters-1
//	Call this before defining/adding any parameters with SetParameter()
//
void	AUElement::UseIndexedParameters(int inNumberOfParameters)
{
	mIndexedParameters.resize (inNumberOfParameters);
	mUseIndexedParameters = true;
}

//_____________________________________________________________________________
//
//	Helper method.
//	returns the ParameterMapEvent object associated with the paramID
//
inline ParameterMapEvent&	AUElement::GetParamEvent(AudioUnitParameterID paramID)
{
	ParameterMapEvent *event;

	if(mUseIndexedParameters)
	{
		if(paramID >= mIndexedParameters.size() )
			COMPONENT_THROW(kAudioUnitErr_InvalidParameter);

		event = &mIndexedParameters[paramID];
	}
	else
	{
		ParameterMap::iterator i = mParameters.find(paramID);
		if (i == mParameters.end())
			COMPONENT_THROW(kAudioUnitErr_InvalidParameter);

		event = &(*i).second;
	}

	return *event;
}

//_____________________________________________________________________________
//
//	caller assumes that this is actually an immediate parameter
//
AudioUnitParameterValue		AUElement::GetParameter(AudioUnitParameterID paramID)
{
	ParameterMapEvent &event = GetParamEvent(paramID);

	return event.GetValue();
}


//_____________________________________________________________________________
//
void			AUElement::GetRampSliceStartEnd(	AudioUnitParameterID		paramID,
													AudioUnitParameterValue &	outStartValue,
													AudioUnitParameterValue &	outEndValue,
													AudioUnitParameterValue &	outValuePerFrameDelta )

{
	ParameterMapEvent &event = GetParamEvent(paramID);

	// works even if the value is constant (immediate parameter value)
	event.GetRampSliceStartEnd(outStartValue, outEndValue, outValuePerFrameDelta );
}

//_____________________________________________________________________________
//
void			AUElement::SetParameter(AudioUnitParameterID paramID, AudioUnitParameterValue inValue)
{
	if(mUseIndexedParameters)
	{
		ParameterMapEvent &event = GetParamEvent(paramID);
		event.SetValue(inValue);
	}
	else
	{
		ParameterMap::iterator i = mParameters.find(paramID);

		if (i == mParameters.end())
		{
			// create new entry in map for the paramID (only happens first time)
			ParameterMapEvent event(inValue);
			mParameters[paramID] = event;
		}
		else
		{
			// paramID already exists in map so simply change its value
			ParameterMapEvent &event = (*i).second;
			event.SetValue(inValue);
		}
	}
}

//_____________________________________________________________________________
//
void			AUElement::SetScheduledEvent(	AudioUnitParameterID 			paramID,
												const AudioUnitParameterEvent 	&inEvent,
												UInt32 							inSliceOffsetInBuffer,
												UInt32							inSliceDurationFrames )
{
	if(mUseIndexedParameters)
	{
		ParameterMapEvent &event = GetParamEvent(paramID);
		event.SetScheduledEvent(inEvent, inSliceOffsetInBuffer, inSliceDurationFrames );
	}
	else
	{
		ParameterMap::iterator i = mParameters.find(paramID);

		if (i == mParameters.end())
		{
			// create new entry in map for the paramID (only happens first time)
			ParameterMapEvent event(inEvent, inSliceOffsetInBuffer, inSliceDurationFrames );
			mParameters[paramID] = event;
		}
		else
		{
			// paramID already exists in map so simply change its value
			ParameterMapEvent &event = (*i).second;

			event.SetScheduledEvent(inEvent, inSliceOffsetInBuffer, inSliceDurationFrames );
		}
	}
}



//_____________________________________________________________________________
//
void			AUElement::GetParameterList(AudioUnitParameterID *outList)
{
	if(mUseIndexedParameters)
	{
		UInt32 nparams = mIndexedParameters.size();
		for (UInt32 i = 0; i < nparams; i++ )
			*outList++ = (AudioUnitParameterID)i;
	}
	else
	{
		for (ParameterMap::iterator i = mParameters.begin(); i != mParameters.end(); ++i)
			*outList++ = (*i).first;
	}
}

//_____________________________________________________________________________
//
void			AUElement::SaveState(CFMutableDataRef data)
{
	if(mUseIndexedParameters)
	{
		UInt32 nparams = mIndexedParameters.size();
		UInt32 theData = CFSwapInt32HostToBig(nparams);
		CFDataAppendBytes(data, (UInt8 *)&theData, sizeof(nparams));

		for (UInt32 i = 0; i < nparams; i++)
		{
			struct {
				UInt32				paramID;
				//CFSwappedFloat32	value; crashes gcc3 PFE
				UInt32				value;	// really a big-endian float
			} entry;

			entry.paramID = CFSwapInt32HostToBig(i);

			AudioUnitParameterValue v = mIndexedParameters[i].GetValue();
			entry.value = CFSwapInt32HostToBig(*(UInt32 *)&v );

			CFDataAppendBytes(data, (UInt8 *)&entry, sizeof(entry));
		}
	}
	else
	{
		UInt32 nparams = CFSwapInt32HostToBig(mParameters.size());
		CFDataAppendBytes(data, (UInt8 *)&nparams, sizeof(nparams));

		for (ParameterMap::iterator i = mParameters.begin(); i != mParameters.end(); ++i) {
			struct {
				UInt32				paramID;
				//CFSwappedFloat32	value; crashes gcc3 PFE
				UInt32				value;	// really a big-endian float
			} entry;

			entry.paramID = CFSwapInt32HostToBig((*i).first);

			AudioUnitParameterValue v = (*i).second.GetValue();
			entry.value = CFSwapInt32HostToBig(*(UInt32 *)&v );

			CFDataAppendBytes(data, (UInt8 *)&entry, sizeof(entry));
		}
	}
}

//_____________________________________________________________________________
//
const UInt8 *	AUElement::RestoreState(const UInt8 *state)
{
	union FloatInt32 { UInt32 i; AudioUnitParameterValue f; };
	const UInt8 *p = state;
	UInt32 nparams = CFSwapInt32BigToHost(*(UInt32 *)p);
	p += sizeof(UInt32);

	for (UInt32 i = 0; i < nparams; ++i) {
		struct {
			AudioUnitParameterID		paramID;
			AudioUnitParameterValue		value;
		} entry;

		entry.paramID = CFSwapInt32BigToHost(*(UInt32 *)p);
		p += sizeof(UInt32);
		FloatInt32 temp;
		temp.i = CFSwapInt32BigToHost(*(UInt32 *)p);
		entry.value = temp.f;
		p += sizeof(AudioUnitParameterValue);

		SetParameter(entry.paramID, entry.value);
	}
	return p;
}

//_____________________________________________________________________________
//
void	AUElement::SetName (CFStringRef inName)
{
	if (mElementName) CFRelease (mElementName);
	mElementName = inName;
	if (mElementName) CFRetain (mElementName);
}


//_____________________________________________________________________________
//
AUIOElement::AUIOElement(AUBase *audioUnit) :
	AUElement(audioUnit)
{
	mStreamFormat.SetCanonical(2,	// stereo
		audioUnit->AudioUnitAPIVersion() == 1);
		// interleaved if API version 1, deinterleaved if version 2
	mStreamFormat.mSampleRate = kAUDefaultSampleRate;
}

//_____________________________________________________________________________
//
OSStatus		AUIOElement::SetStreamFormat(const CAStreamBasicDescription &desc)
{
	mStreamFormat = desc;
	return noErr;
}

//_____________________________________________________________________________
// inFramesToAllocate == 0 implies the AudioUnit's max-frames-per-slice will be used
void			AUIOElement::AllocateBuffer(UInt32 inFramesToAllocate)
{
	if (GetAudioUnit()->IsInitialized())
	{
		UInt32 framesToAllocate = inFramesToAllocate > 0 ? inFramesToAllocate : GetAudioUnit()->GetMaxFramesPerSlice();

		mIOBuffer.Allocate(mStreamFormat, NeedsBufferSpace() ? framesToAllocate : 0);
	}
}

//_____________________________________________________________________________
//
void			AUIOElement::DeallocateBuffer()
{
	mIOBuffer.Deallocate();
}

//_____________________________________________________________________________
//
//		AudioChannelLayout support

// outLayoutTagsPtr WILL be NULL if called to find out how many
// layouts that Audio Unit will report
// return 0 (ie. NO channel layouts) if the AU doesn't require channel layout knowledge
UInt32		AUIOElement::GetChannelLayoutTags (AudioChannelLayoutTag		*outLayoutTagsPtr)
{
	return 0;
}

// As the AudioChannelLayout can be a variable length structure
// (though in most cases it won't be!!!)
// the AU should return the address of the current ACM in use (in outMapPtr) AND if it is writable
// The size of the ACM is returned by the method
// If the AU doesn't require an AudioChannelLayout, then just return 0.
UInt32		AUIOElement::GetAudioChannelLayout (AudioChannelLayout		*outMapPtr,
											Boolean				&outWritable)
{
	return 0;
}

// the incoming channel map will be at least as big as a basic AudioChannelLayout
// but its contents will determine its actual size
// Subclass should overide if channel map is writable
OSStatus	AUIOElement::SetAudioChannelLayout (const AudioChannelLayout &inData)
{
	return kAudioUnitErr_InvalidProperty;
}

// Some units support optional usage of channel maps - typically converter units
// that can do channel remapping between different maps. In that optional case
// the user should be able to remove a channel map if that is possible.
// Typically this is NOT the case (e.g., the 3DMixer even in the stereo case
// needs to know if it is rendering to speakers or headphones)
OSStatus	AUIOElement::RemoveAudioChannelLayout ()
{
	return kAudioUnitErr_InvalidPropertyValue;
}


//_____________________________________________________________________________
//
AUScope::~AUScope()
{
	for (ElementVector::iterator it = mElements.begin(); it != mElements.end(); ++it)
		delete *it;
}

//_____________________________________________________________________________
//
void	AUScope::SetNumberOfElements(UInt32 numElements)
{
	if (mDelegate)
		return mDelegate->SetNumberOfElements(numElements);

	if (numElements > mElements.size()) {
		mElements.reserve(numElements);
		while (numElements > mElements.size()) {
			AUElement *elem = GetCreator()->CreateElement(GetScope(), mElements.size());
			mElements.push_back(elem);
		}
	} else
		while (numElements < mElements.size()) {
			AUElement *elem = mElements.back();
			mElements.pop_back();
			delete elem;
		}
}

//_____________________________________________________________________________
//
bool	AUScope::HasElementWithName () const
{
	for (UInt32 i = 0; i < GetNumberOfElements(); ++i) {
		AUElement *	el = const_cast<AUScope*>(this)->GetElement (i);
		if (el && el->HasName()) {
			return true;
		}
	}
	return false;
}

//_____________________________________________________________________________
//

void	AUScope::AddElementNamesToDict (CFMutableDictionaryRef & inNameDict)
{
	if (HasElementWithName())
	{
		static char string[32];
		CFMutableDictionaryRef elementDict = CFDictionaryCreateMutable	(NULL, 0,
								&kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
		CFStringRef str;
		for (UInt32 i = 0; i < GetNumberOfElements(); ++i) {
			AUElement *	el = GetElement (i);
			if (el && el->HasName()) {
				sprintf (string, "%d", int(i));
				str = CFStringCreateWithCString (NULL, string, kCFStringEncodingASCII);
				CFDictionarySetValue (elementDict, str, el->GetName());
				CFRelease (str);
			}
		}

		sprintf (string, "%d", int(mScope));
		str = CFStringCreateWithCString (NULL, string, kCFStringEncodingASCII);
		CFDictionarySetValue (inNameDict, str, elementDict);
		CFRelease (str);
		CFRelease (elementDict);
	}
}

//_____________________________________________________________________________
//
bool	AUScope::RestoreElementNames (CFDictionaryRef& inNameDict)
{
	static char string[32];

	//first we have to see if we have enough elements and if not create them
	bool didAddElements = false;
	unsigned int maxElNum = 0;

	int dictSize = CFDictionaryGetCount(inNameDict);
	CFStringRef * keys = (CFStringRef*)malloc (dictSize * sizeof (CFStringRef));
	CFDictionaryGetKeysAndValues (inNameDict, reinterpret_cast<const void**>(keys), NULL);
	for (int i = 0; i < dictSize; i++)
	{
		unsigned int intKey;
		CFStringGetCString (keys[i], string, 32, kCFStringEncodingASCII);
		sscanf (string, "%d", &intKey);
		if (UInt32(intKey) > maxElNum)
			maxElNum = intKey;
	}

	if (maxElNum >= GetNumberOfElements()) {
		SetNumberOfElements (maxElNum+1);
		didAddElements = true;
	}

		// OK, now we have the number of elements that we need - lets restate their names
	for (int i = 0; i < dictSize; i++)
	{
		CFStringRef elName = reinterpret_cast<CFStringRef>(CFDictionaryGetValue (inNameDict,  keys[i]));
		int intKey;
		CFStringGetCString (keys[i], string, 32, kCFStringEncodingASCII);
		sscanf (string, "%d", &intKey);
		GetElement (intKey)->SetName (elName);
	}
	free (keys);

	return didAddElements;
}


