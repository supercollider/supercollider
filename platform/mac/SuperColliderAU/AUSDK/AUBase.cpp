/*	Copyright � 2007 Apple Inc. All Rights Reserved.

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
	AUBase.cpp

=============================================================================*/

#include "AUBase.h"
#include "AUDispatch.h"
#include "AUInputElement.h"
#include "AUOutputElement.h"
#include <algorithm>
#include "CAAudioChannelLayout.h"
#include "CAHostTimeBase.h"
#include "CAVectorUnit.h"

#if TARGET_OS_MAC && (TARGET_CPU_X86 || TARGET_CPU_X86_64)
	// our compiler does ALL floating point with SSE
	#define GETCSR()    ({ int _result; asm volatile ("stmxcsr %0" : "=m" (*&_result) ); /*return*/ _result; })
	#define SETCSR( a )    { int _temp = a; asm volatile( "ldmxcsr %0" : : "m" (*&_temp ) ); }

	#define DISABLE_DENORMALS int _savemxcsr = GETCSR(); SETCSR(_savemxcsr | 0x8040);
	#define RESTORE_DENORMALS SETCSR(_savemxcsr);
#else
	#define DISABLE_DENORMALS
	#define RESTORE_DENORMALS
#endif

#define kAudioUnitRenderAction_PostRenderError (1 << 8)


static bool sAUBaseCFStringsInitialized = false;
// this is used for the presets
static CFStringRef kUntitledString = NULL;
//these are the current keys for the class info document
static CFStringRef kVersionString = NULL;
static CFStringRef kTypeString = NULL;
static CFStringRef kSubtypeString = NULL;
static CFStringRef kManufacturerString = NULL;
static CFStringRef kDataString = NULL;
static CFStringRef kNameString = NULL;
static CFStringRef kRenderQualityString = NULL;
static CFStringRef kCPULoadString = NULL;
static CFStringRef kVSTDataString = NULL;
static CFStringRef kElementNameString = NULL;
static CFStringRef kPartString = NULL;

SInt32 AUBase::sVectorUnitType = kVecUninitialized;

//_____________________________________________________________________________
//
AUBase::AUBase(	AudioUnit						inInstance,
				UInt32							numInputElements,
				UInt32							numOutputElements,
				UInt32							numGroupElements,
				UInt32							numPartElements) :
	ComponentBase(inInstance),
	mElementsCreated(false),
	mInitialized(false),
	mInitNumInputEls(numInputElements), mInitNumOutputEls(numOutputElements),
	mInitNumGroupEls(numGroupElements), mInitNumPartEls(numPartElements),
	mRenderThreadID (NULL),
	mWantsRenderThreadID (false),
	mLastRenderedSampleTime(kNoLastRenderedSampleTime),
	mLastRenderError(0),
	mContextName(NULL),
	mBuffersAllocated(false),
	mLogString (NULL),
	mDebugDispatcher (NULL)

{

	if(!sAUBaseCFStringsInitialized)
	{
		kUntitledString = CFSTR("Untitled");
		kVersionString = CFSTR(kAUPresetVersionKey);
		kTypeString = CFSTR(kAUPresetTypeKey);
		kSubtypeString = CFSTR(kAUPresetSubtypeKey);
		kManufacturerString = CFSTR(kAUPresetManufacturerKey);
		kDataString = CFSTR(kAUPresetDataKey);
		kNameString = CFSTR(kAUPresetNameKey);
		kRenderQualityString = CFSTR(kAUPresetRenderQualityKey);
		kCPULoadString = CFSTR(kAUPresetCPULoadKey);
		kVSTDataString = CFSTR(kAUPresetVSTDataKey);
		kElementNameString = CFSTR(kAUPresetElementNameKey);
		kPartString = CFSTR(kAUPresetPartKey);
		sAUBaseCFStringsInitialized = true;
	}

	if (sVectorUnitType == kVecUninitialized) {
		sVectorUnitType = CAVectorUnit::GetVectorUnitType() ;
	}

	mAudioUnitAPIVersion = 2;

	SetMaxFramesPerSlice(kAUDefaultMaxFramesPerSlice);

	GlobalScope().Initialize(this, kAudioUnitScope_Global, 1);

	if (mAudioUnitAPIVersion > 1)
		mParamList.reserve (24);

	memset (&mHostCallbackInfo, 0, sizeof (mHostCallbackInfo));

	mCurrentPreset.presetNumber = -1;
	mCurrentPreset.presetName = kUntitledString;
	CFRetain (mCurrentPreset.presetName);
}

//_____________________________________________________________________________
//
AUBase::~AUBase()
{
	if (mCurrentPreset.presetName) CFRelease (mCurrentPreset.presetName);
	if (mContextName) CFRelease (mContextName);
	if (mLogString) delete [] mLogString;
}

//_____________________________________________________________________________
//
void	AUBase::CreateElements()
{
	if (!mElementsCreated) {
		Inputs().Initialize(this, kAudioUnitScope_Input, mInitNumInputEls);
		Outputs().Initialize(this, kAudioUnitScope_Output, mInitNumOutputEls);
		Groups().Initialize(this, kAudioUnitScope_Group, mInitNumGroupEls);
		Parts().Initialize(this, kAudioUnitScope_Part, mInitNumPartEls);
		mElementsCreated = true;
	}
}

//_____________________________________________________________________________
//
void	AUBase::SetMaxFramesPerSlice(UInt32 nFrames)
{
	mMaxFramesPerSlice = nFrames;
	if (mBuffersAllocated)
		ReallocateBuffers();
	PropertyChanged(kAudioUnitProperty_MaximumFramesPerSlice, kAudioUnitScope_Global, 0);
}

//_____________________________________________________________________________
//
OSStatus			AUBase::CanSetMaxFrames() const
{
	return IsInitialized() ? kAudioUnitErr_Initialized : OSStatus(noErr);
}

//_____________________________________________________________________________
//
void	AUBase::ReallocateBuffers()
{
	CreateElements();

	int i;
	int nOutputs = Outputs().GetNumberOfElements();
	for (i = 0; i < nOutputs; ++i) {
		AUOutputElement *output = GetOutput(i);
		output->AllocateBuffer();	// does no work if already allocated
	}
	int nInputs = Inputs().GetNumberOfElements();
	for (i = 0; i < nInputs; ++i) {
		AUInputElement *input = GetInput(i);
		input->AllocateBuffer();	// does no work if already allocated
	}
	mBuffersAllocated = true;
}

//_____________________________________________________________________________
//
ComponentResult		AUBase::DoInitialize()
{
	ComponentResult result = noErr;

	if (!mInitialized) {
		result = Initialize();
		if (result == noErr) {
			mInitialized = true;		// signal that it's okay to initialize buffers
			ReallocateBuffers();	// calls CreateElements()
		}
	}

	return result;
}

//_____________________________________________________________________________
//
ComponentResult		AUBase::Initialize()
{
	return noErr;
}

//_____________________________________________________________________________
//
void				AUBase::PreDestructor()
{
	DoCleanup();
}

//_____________________________________________________________________________
//
void				AUBase::DoCleanup()
{
	if (mInitialized)
		Cleanup();
	mInitialized = false;
}

//_____________________________________________________________________________
//
void				AUBase::Cleanup()
{
}

//_____________________________________________________________________________
//
ComponentResult		AUBase::Reset(					AudioUnitScope 					inScope,
													AudioUnitElement 				inElement)
{
	mLastRenderedSampleTime = -1;
	return noErr;
}

//_____________________________________________________________________________
//
ComponentResult		AUBase::DispatchGetPropertyInfo(AudioUnitPropertyID				inID,
													AudioUnitScope					inScope,
													AudioUnitElement				inElement,
													UInt32 &						outDataSize,
													Boolean &						outWritable)
{
	ComponentResult result = noErr;
	bool validateElement = true;

	switch (inID) {
	case kAudioUnitProperty_MakeConnection:
		require(inScope == kAudioUnitScope_Input || inScope == kAudioUnitScope_Global, InvalidScope);
		outDataSize = sizeof(AudioUnitConnection);
		outWritable = true;
		break;


	case kAudioUnitProperty_SetRenderCallback:
		require(AudioUnitAPIVersion() > 1, InvalidProperty);
		require(inScope == kAudioUnitScope_Input || inScope == kAudioUnitScope_Global, InvalidScope);
		outDataSize = sizeof(AURenderCallbackStruct);
		outWritable = true;
		break;


	case kAudioUnitProperty_FastDispatch:
		require(inScope == kAudioUnitScope_Global, InvalidScope);
		outDataSize = sizeof(void *);
		outWritable = false;
		validateElement = false;
		break;

	case kAudioUnitProperty_StreamFormat:
		outDataSize = sizeof(CAStreamBasicDescription);
		outWritable = IsStreamFormatWritable(inScope, inElement);
		break;

	case kAudioUnitProperty_SampleRate:
		outDataSize = sizeof(Float64);
		outWritable = IsStreamFormatWritable(inScope, inElement);
		break;

	case kAudioUnitProperty_ClassInfo:
		require(inScope == kAudioUnitScope_Global, InvalidScope);
		outDataSize = sizeof(CFPropertyListRef);
		outWritable = true;
		break;

	case kAudioUnitProperty_FactoryPresets:
		require(inScope == kAudioUnitScope_Global, InvalidScope);
		result = GetPresets(NULL);
		if (!result) {
			outDataSize = sizeof(CFArrayRef);
			outWritable = false;
		}
		break;

	case kAudioUnitProperty_PresentPreset:
#ifndef __LP64__
	case kAudioUnitProperty_CurrentPreset:
#endif
		require(inScope == kAudioUnitScope_Global, InvalidScope);
		outDataSize = sizeof(AUPreset);
		outWritable = true;
		break;

	case kAudioUnitProperty_ContextName:
		require(inScope == kAudioUnitScope_Global, InvalidScope);
		outDataSize = sizeof(CFStringRef);
		outWritable = true;
		break;

	case kAudioUnitProperty_ElementName:
		outDataSize = sizeof (CFStringRef);
		outWritable = true;
		break;

	case kAudioUnitProperty_GetUIComponentList:
		require(inScope == kAudioUnitScope_Global, InvalidScope);
		outDataSize = GetNumCustomUIComponents();
		if (outDataSize == 0)
			goto InvalidProperty;
		outDataSize *= sizeof (ComponentDescription);

		outWritable = false;
		break;

	case kAudioUnitProperty_ParameterList:
		{
			UInt32 nparams = 0;
			result = GetParameterList(inScope, NULL, nparams);

			outDataSize = sizeof(AudioUnitParameterID) * nparams;
			outWritable = false;
			validateElement = false;
		}
		break;

	case kAudioUnitProperty_ParameterInfo:
		outDataSize = sizeof(AudioUnitParameterInfo);
		outWritable = false;
		validateElement = false;
		break;

	case kAudioUnitProperty_ParameterValueStrings:
		result = GetParameterValueStrings(inScope, inElement, NULL);
		if (result == noErr) {
			outDataSize = sizeof(CFArrayRef);
			outWritable = false;
			validateElement = false;
		}
		break;

	case kAudioUnitProperty_ElementCount:
		outDataSize = sizeof(UInt32);
		outWritable = BusCountWritable(inScope);
		validateElement = false;
		break;

	case kAudioUnitProperty_Latency:
		require(inScope == kAudioUnitScope_Global, InvalidScope);
		outDataSize = sizeof(Float64);
		outWritable = false;
		break;

	case kAudioUnitProperty_TailTime:
		require(inScope == kAudioUnitScope_Global, InvalidScope);
		if (SupportsTail()) {
			outDataSize = sizeof(Float64);
			outWritable = false;
		} else
			goto InvalidProperty;
		break;

	case kAudioUnitProperty_MaximumFramesPerSlice:
		require(inScope == kAudioUnitScope_Global, InvalidScope);
		outDataSize = sizeof(UInt32);
		outWritable = true;
		break;

	case kAudioUnitProperty_LastRenderError:
		require(inScope == kAudioUnitScope_Global, InvalidScope);
		outDataSize = sizeof(OSStatus);
		outWritable = false;
		break;

#if 0
	case kAudioUnitProperty_SetExternalBuffer:
		require(inScope == kAudioUnitScope_Global, InvalidScope);
		outDataSize = sizeof(AudioUnitExternalBuffer);
		outWritable = true;
		break;
#endif

	case kAudioUnitProperty_SupportedNumChannels:
	{
		require(inScope == kAudioUnitScope_Global, InvalidScope);
		UInt32 num = SupportedNumChannels (NULL);
		if (num) {
			outDataSize = sizeof (AUChannelInfo) * num;
			result = noErr;
		} else
			goto InvalidProperty;
		outWritable = false;
		break;
	}

	case kAudioUnitProperty_SupportedChannelLayoutTags:
	{
		UInt32 numLayouts = GetChannelLayoutTags(inScope, inElement, NULL);
		if (numLayouts) {
			outDataSize = numLayouts * sizeof(AudioChannelLayoutTag);
			result = noErr;
		} else
			goto InvalidProperty;
		outWritable = false;
		validateElement = false; //already done it
		break;
	}

	case kAudioUnitProperty_AudioChannelLayout:
	{
		outWritable = false;
		outDataSize = GetAudioChannelLayout(inScope, inElement, NULL, outWritable);
		if (outDataSize) {
			result = noErr;
		} else {
			if (GetChannelLayoutTags(inScope, inElement, NULL) == 0)
				goto InvalidProperty;
			else
				result = kAudioUnitErr_InvalidPropertyValue;
		}
		validateElement = false; //already done it
		break;
	}

	case kAudioUnitProperty_HostCallbacks:
		require(inScope == kAudioUnitScope_Global, InvalidScope);
		outDataSize = sizeof (HostCallbackInfo);
		outWritable = true;
		break;

	case kAudioUnitProperty_IconLocation:
		require(inScope == kAudioUnitScope_Global, InvalidScope);
		outWritable = false;
		if (!HasIcon())
			goto InvalidProperty;
		outDataSize = sizeof(CFURLRef);
		break;

	default:
		result = GetPropertyInfo(inID, inScope, inElement, outDataSize, outWritable);
		validateElement = false;
		break;
	}

	if (result == noErr && validateElement) {
		require(GetElement(inScope, inElement) != NULL, InvalidElement);
	}

	return result;
InvalidProperty:
	return kAudioUnitErr_InvalidProperty;
InvalidScope:
	return kAudioUnitErr_InvalidScope;
InvalidElement:
	return kAudioUnitErr_InvalidElement;
}

//_____________________________________________________________________________
//
ComponentResult		AUBase::DispatchGetProperty(	AudioUnitPropertyID 			inID,
													AudioUnitScope 					inScope,
													AudioUnitElement			 	inElement,
													void *							outData)
{
	// NOTE: We're currently only called from AUBase::ComponentEntryDispatch, which
	// calls DispatchGetPropertyInfo first, which performs validation of the scope/element,
	// and ensures that the outData buffer is non-null and large enough.
	ComponentResult result = noErr;

	switch (inID) {
	case kAudioUnitProperty_FastDispatch:
		switch (inElement) {
		case kAudioUnitGetParameterSelect:
			*(AudioUnitGetParameterProc *)outData = (AudioUnitGetParameterProc)AudioUnitBaseGetParameter;
			break;
		case kAudioUnitSetParameterSelect:
			*(AudioUnitSetParameterProc *)outData = (AudioUnitSetParameterProc)AudioUnitBaseSetParameter;
			break;
		case kAudioUnitRenderSelect:
			if (AudioUnitAPIVersion() > 1)
				*(AudioUnitRenderProc *)outData = (AudioUnitRenderProc)AudioUnitBaseRender;
			else result = kAudioUnitErr_InvalidElement;
			break;
		default:
			result = GetProperty(inID, inScope, inElement, outData);
			break;
		}
		break;

	case kAudioUnitProperty_StreamFormat:
		*(CAStreamBasicDescription *)outData = GetStreamFormat(inScope, inElement);
		break;

	case kAudioUnitProperty_SampleRate:
		*(Float64 *)outData = GetStreamFormat(inScope, inElement).mSampleRate;
		break;

	case kAudioUnitProperty_ParameterList:
		{
			UInt32 nparams = 0;
			result = GetParameterList(inScope, (AudioUnitParameterID *)outData, nparams);
		}
		break;

	case kAudioUnitProperty_ParameterInfo:
		result = GetParameterInfo(inScope, inElement, *(AudioUnitParameterInfo *)outData);
		break;

	case kAudioUnitProperty_ClassInfo:
		{
			*(CFPropertyListRef *)outData = NULL;
			result = SaveState((CFPropertyListRef *)outData);
		}
		break;

	case kAudioUnitProperty_ParameterValueStrings:
		result = GetParameterValueStrings(inScope, inElement, (CFArrayRef *)outData);
		break;

	case kAudioUnitProperty_FactoryPresets:
		{
			*(CFArrayRef *)outData = NULL;
			result = GetPresets ((CFArrayRef *)outData);
		}
		break;

	case kAudioUnitProperty_PresentPreset:
#ifndef __LP64__
	case kAudioUnitProperty_CurrentPreset:
#endif
		{
			*(AUPreset *)outData = mCurrentPreset;

				// retain current string (as client owns a reference to it and will release it)
			if (inID == kAudioUnitProperty_PresentPreset && mCurrentPreset.presetName)
				CFRetain (mCurrentPreset.presetName);

			result = noErr;
		}
		break;

	case kAudioUnitProperty_ContextName:
		*(CFStringRef *)outData = mContextName;
		if (mContextName) {
			CFRetain(mContextName);
			// retain CFString (if exists) since client will be responsible for its release
			result = noErr;
		} else {
			result = kAudioUnitErr_InvalidPropertyValue;
		}
		break;

	case kAudioUnitProperty_ElementName:
		{
			AUElement * element = GetElement(inScope, inElement);
			if (element->HasName()) {
				*(CFStringRef *)outData = element->GetName();
				CFRetain (element->GetName());
				result = noErr;
			} else
				result = kAudioUnitErr_InvalidPropertyValue;
		}
		break;

	case kAudioUnitProperty_ElementCount:
		*(UInt32 *)outData = GetScope(inScope).GetNumberOfElements();
		break;

	case kAudioUnitProperty_Latency:
		*(Float64 *)outData = GetLatency();
		break;

	case kAudioUnitProperty_TailTime:
		if (SupportsTail())
			*(Float64 *)outData = GetTailTime();
		else
			result = kAudioUnitErr_InvalidProperty;
		break;

	case kAudioUnitProperty_MaximumFramesPerSlice:
		*(UInt32 *)outData = mMaxFramesPerSlice;
		break;

	case kAudioUnitProperty_LastRenderError:
		*(OSStatus *)outData = mLastRenderError;
		mLastRenderError = 0;
		break;

	case kAudioUnitProperty_SupportedNumChannels:
		{
			const AUChannelInfo* infoPtr;
			UInt32 num = SupportedNumChannels (&infoPtr);
			memcpy (outData, infoPtr, num * sizeof (AUChannelInfo));
		}
		break;

	case kAudioUnitProperty_SupportedChannelLayoutTags:
		{
			AudioChannelLayoutTag* ptr = outData ? static_cast<AudioChannelLayoutTag*>(outData) : NULL;
			UInt32 numLayouts = GetChannelLayoutTags (inScope, inElement, ptr);
			if (numLayouts == 0)
				result = kAudioUnitErr_InvalidProperty;
		}
		break;

	case kAudioUnitProperty_AudioChannelLayout:
	{
		AudioChannelLayout* ptr = outData ? static_cast<AudioChannelLayout*>(outData) : NULL;
		Boolean writable;
		UInt32 dataSize = GetAudioChannelLayout(inScope, inElement, ptr, writable);
		if (!dataSize) {
			result = kAudioUnitErr_InvalidProperty;
		}
		break;
	}

	case kAudioUnitProperty_HostCallbacks:
		*(HostCallbackInfo *)outData = mHostCallbackInfo;
		break;

	case kAudioUnitProperty_GetUIComponentList:
		GetUIComponentDescs ((ComponentDescription*)outData);
		break;

	case kAudioUnitProperty_IconLocation:
		{
			CFURLRef iconLocation = CopyIconLocation();
			if (iconLocation) {
				*(CFURLRef*)outData = iconLocation;
			} else
				result = kAudioUnitErr_InvalidProperty;
		}
		break;

	default:
		result = GetProperty(inID, inScope, inElement, outData);
		break;
	}
	return result;
}


//_____________________________________________________________________________
//
ComponentResult		AUBase::DispatchSetProperty(	AudioUnitPropertyID 			inID,
													AudioUnitScope 					inScope,
													AudioUnitElement 				inElement,
													const void *					inData,
													UInt32 							inDataSize)
{
	ComponentResult result = noErr;

	switch (inID) {
	case kAudioUnitProperty_MakeConnection:
		require(inDataSize >= sizeof(AudioUnitConnection), InvalidPropertyValue);
		{
			AudioUnitConnection &connection = *(AudioUnitConnection *)inData;
//printf("Making connection AU %08lX output %ld => AU %08lX input %ld\n",
//connection.sourceAudioUnit, connection.sourceOutputNumber, GetComponentInstance(), connection.destInputNumber);
			result = SetConnection(connection);
		}
		break;


	case kAudioUnitProperty_SetRenderCallback:
		{
			require(inDataSize >= sizeof(AURenderCallbackStruct), InvalidPropertyValue);
			require(AudioUnitAPIVersion() > 1, InvalidProperty);
			AURenderCallbackStruct &callback = *(AURenderCallbackStruct*)inData;
			result = SetInputCallback(kAudioUnitProperty_SetRenderCallback, inElement,
						reinterpret_cast<ProcPtr>(callback.inputProc), callback.inputProcRefCon);
		}
		break;

	case kAudioUnitProperty_ElementCount:
		require(inDataSize == sizeof(UInt32), InvalidPropertyValue);
		require(BusCountWritable(inScope), NotWritable);
		result = SetBusCount(inScope, *(UInt32*)inData);
		if (result == noErr) {
			PropertyChanged(inID, inScope, inElement);
		}
		break;

	case kAudioUnitProperty_MaximumFramesPerSlice:
		require(inDataSize == sizeof(UInt32), InvalidPropertyValue);
		result = CanSetMaxFrames();
		if (result) return result;
		SetMaxFramesPerSlice(*(UInt32 *)inData);
		break;

	case kAudioUnitProperty_StreamFormat:
		{
			if (inDataSize < 36) goto InvalidPropertyValue;
			require(GetElement(inScope, inElement) != NULL, InvalidElement);

			CAStreamBasicDescription newDesc;
				// now we're going to be ultra conservative! because of discrepancies between
				// sizes of this struct based on aligment padding inconsistencies
			memset (&newDesc, 0, sizeof(newDesc));
			memcpy (&newDesc, inData, 36);

			require(ValidFormat(inScope, inElement, newDesc), InvalidFormat);

			const CAStreamBasicDescription curDesc = GetStreamFormat(inScope, inElement);

			if ( !(curDesc == newDesc) ) {
				require(IsStreamFormatWritable(inScope, inElement), NotWritable);
				result = ChangeStreamFormat(inScope, inElement, curDesc, newDesc);
			}
		}
		break;

	case kAudioUnitProperty_SampleRate:
		{
			require(inDataSize == sizeof(Float64), InvalidPropertyValue);
			require(GetElement(inScope, inElement) != NULL, InvalidElement);

			const CAStreamBasicDescription curDesc = GetStreamFormat(inScope, inElement);
			CAStreamBasicDescription newDesc = curDesc;
			newDesc.mSampleRate = *(Float64 *)inData;

			require(ValidFormat(inScope, inElement, newDesc), InvalidFormat);

			if ( !(curDesc == newDesc) ) {
				require(IsStreamFormatWritable(inScope, inElement), NotWritable);
				result = ChangeStreamFormat(inScope, inElement, curDesc, newDesc);
			}
		}
		break;

	case kAudioUnitProperty_AudioChannelLayout:
		{
			const AudioChannelLayout *layout = static_cast<const AudioChannelLayout *>(inData);
			require(inDataSize >= offsetof(AudioChannelLayout, mChannelDescriptions) + layout->mNumberChannelDescriptions * sizeof(AudioChannelDescription), InvalidPropertyValue);
			result = SetAudioChannelLayout(inScope, inElement, layout);
			if (result == noErr)
				PropertyChanged(inID, inScope, inElement);
			break;
		}

	case kAudioUnitProperty_SetExternalBuffer:
		require(inDataSize >= sizeof(AudioUnitExternalBuffer), InvalidPropertyValue);
		require(IsInitialized(), Uninitialized);
		{
			AudioUnitExternalBuffer &buf = *(AudioUnitExternalBuffer*)inData;
			if (intptr_t(buf.buffer) & 0x0F) result = paramErr;
			else if (inScope == kAudioUnitScope_Input) {
				AUInputElement *input = GetInput(inElement);
				input->UseExternalBuffer(buf);
			} else {
				AUOutputElement *output = GetOutput(inElement);
				output->UseExternalBuffer(buf);
			}
		}
		break;

	case kAudioUnitProperty_ClassInfo:
		require(inDataSize == sizeof(CFPropertyListRef *), InvalidPropertyValue);
		require(inScope == kAudioUnitScope_Global, InvalidScope);
		result = RestoreState(*(CFPropertyListRef *)inData);
		break;

	case kAudioUnitProperty_PresentPreset:
#ifndef __LP64__
	case kAudioUnitProperty_CurrentPreset:
#endif
		{
			require(inDataSize == sizeof(AUPreset), InvalidPropertyValue);
			require(inScope == kAudioUnitScope_Global, InvalidScope);
			AUPreset & newPreset = *(AUPreset *)inData;

			if (newPreset.presetNumber >= 0)
			{
				result = NewFactoryPresetSet(newPreset);
				// NewFactoryPresetSet SHOULD call SetAFactoryPreset if the preset is valid
				// from its own list of preset number->name
				if (!result)
					PropertyChanged(inID, inScope, inElement);
			}
			else if (newPreset.presetName)
			{
				CFRelease (mCurrentPreset.presetName);
				mCurrentPreset = newPreset;
				CFRetain (mCurrentPreset.presetName);
				PropertyChanged(inID, inScope, inElement);
			}
			else
				result = kAudioUnitErr_InvalidPropertyValue;
		}
		break;

	case kAudioUnitProperty_ContextName:
		{
			require(inDataSize == sizeof(CFStringRef), InvalidPropertyValue);
			require(inScope == kAudioUnitScope_Global, InvalidScope);
			CFStringRef inStr = *(CFStringRef *)inData;
			if (mContextName) CFRelease(mContextName);
			if (inStr) CFRetain(inStr);
			mContextName = inStr;
			PropertyChanged(inID, inScope, inElement);
		}
		break;

	case kAudioUnitProperty_ElementName:
		{
			require(GetElement(inScope, inElement) != NULL, InvalidElement);
			require(inDataSize == sizeof(CFStringRef), InvalidPropertyValue);
			AUElement * element = GetScope(inScope).GetElement (inElement);
			element->SetName (*(CFStringRef *)inData);
			PropertyChanged(inID, inScope, inElement);
		}
		break;

	case kAudioUnitProperty_HostCallbacks:
	{
		require(inScope == kAudioUnitScope_Global, InvalidScope);
		UInt32 availSize = (inDataSize < sizeof (mHostCallbackInfo) ? inDataSize : sizeof (mHostCallbackInfo));
		bool hasChanged = !memcmp (&mHostCallbackInfo, inData, availSize);
		memset (&mHostCallbackInfo, 0, sizeof (mHostCallbackInfo));
		memcpy (&mHostCallbackInfo, inData, availSize);
		if (hasChanged)
			PropertyChanged(inID, inScope, inElement);
		break;
	}

	default:
		result = SetProperty(inID, inScope, inElement, inData, inDataSize);
		if (result == noErr)
			PropertyChanged(inID, inScope, inElement);

		break;
	}
	return result;
NotWritable:
	return kAudioUnitErr_PropertyNotWritable;
InvalidFormat:
	return kAudioUnitErr_FormatNotSupported;
Uninitialized:
	return kAudioUnitErr_Uninitialized;
InvalidScope:
	return kAudioUnitErr_InvalidScope;
InvalidProperty:
	return kAudioUnitErr_InvalidProperty;
InvalidPropertyValue:
	return kAudioUnitErr_InvalidPropertyValue;
InvalidElement:
	return kAudioUnitErr_InvalidElement;
}

//_____________________________________________________________________________
//
ComponentResult		AUBase::DispatchRemovePropertyValue (AudioUnitPropertyID		inID,
													AudioUnitScope 					inScope,
													AudioUnitElement 				inElement)
{
	ComponentResult result = noErr;
	switch (inID)
	{
	case kAudioUnitProperty_AudioChannelLayout:
	{
		result = RemoveAudioChannelLayout(inScope, inElement);
		if (result == noErr)
			PropertyChanged(inID, inScope, inElement);
		break;
	}

	case kAudioUnitProperty_ContextName:
		if (mContextName) CFRelease(mContextName);
		mContextName = NULL;
		result = noErr;
		break;

	case kAudioUnitProperty_HostCallbacks:
	{
		require(inScope == kAudioUnitScope_Global, InvalidScope);
		bool hasValue = false;
		void* ptr = &mHostCallbackInfo;
		for (unsigned int i = 0; i <  sizeof (HostCallbackInfo); ++i) {
			if (static_cast<char*>(ptr)[i]) {
				hasValue = true;
				break;
			}
		}
		if (hasValue) {
			memset (&mHostCallbackInfo, 0, sizeof (HostCallbackInfo));
			PropertyChanged(inID, inScope, inElement);
		}
		break;
	}

	default:
		result = RemovePropertyValue (inID, inScope, inElement);
		break;
	}

	return result;
InvalidScope:
	return kAudioUnitErr_InvalidScope;
}

//_____________________________________________________________________________
//
ComponentResult		AUBase::GetPropertyInfo(		AudioUnitPropertyID				inID,
													AudioUnitScope					inScope,
													AudioUnitElement				inElement,
													UInt32 &						outDataSize,
													Boolean &						outWritable)
{
	return kAudioUnitErr_InvalidProperty;
}


//_____________________________________________________________________________
//
ComponentResult		AUBase::GetProperty(			AudioUnitPropertyID 			inID,
													AudioUnitScope 					inScope,
													AudioUnitElement			 	inElement,
													void *							outData)
{
	return kAudioUnitErr_InvalidProperty;
}


//_____________________________________________________________________________
//
ComponentResult		AUBase::SetProperty(			AudioUnitPropertyID 			inID,
													AudioUnitScope 					inScope,
													AudioUnitElement 				inElement,
													const void *					inData,
													UInt32 							inDataSize)
{
	return kAudioUnitErr_InvalidProperty;
}

//_____________________________________________________________________________
//
ComponentResult		AUBase::RemovePropertyValue (	AudioUnitPropertyID		 		inID,
													AudioUnitScope 					inScope,
													AudioUnitElement 				inElement)
{
	return kAudioUnitErr_InvalidPropertyValue;
}

//_____________________________________________________________________________
//
ComponentResult		AUBase::AddPropertyListener(	AudioUnitPropertyID				inID,
													AudioUnitPropertyListenerProc	inProc,
													void *							inProcRefCon)
{
	PropertyListener pl;

	pl.propertyID = inID;
	pl.listenerProc = inProc;
	pl.listenerRefCon = inProcRefCon;

	if (mPropertyListeners.empty())
		mPropertyListeners.reserve(32);
	mPropertyListeners.push_back(pl);

	return noErr;
}

//_____________________________________________________________________________
//
ComponentResult		AUBase::RemovePropertyListener(		AudioUnitPropertyID				inID,
														AudioUnitPropertyListenerProc	inProc,
														void *							inProcRefCon,
														bool							refConSpecified)
{
	// iterate in reverse so that it's safe to erase in the middle of the vector
	for (int i = mPropertyListeners.size(); --i >=0; ) {
		PropertyListeners::iterator it = mPropertyListeners.begin() + i;
		if ((*it).propertyID == inID && (*it).listenerProc == inProc && (!refConSpecified || (*it).listenerRefCon == inProcRefCon))
			mPropertyListeners.erase(it);
	}
	return noErr;
}

//_____________________________________________________________________________
//
void				AUBase::PropertyChanged(			AudioUnitPropertyID				inID,
														AudioUnitScope					inScope,
														AudioUnitElement				inElement)
{
	for (PropertyListeners::iterator it = mPropertyListeners.begin(); it != mPropertyListeners.end(); ++it)
		if ((*it).propertyID == inID)
			((*it).listenerProc)((*it).listenerRefCon, mComponentInstance, inID, inScope, inElement);
}

//_____________________________________________________________________________
//
ComponentResult		AUBase::SetRenderNotification(	ProcPtr					 		inProc,
													void *							inRefCon)
{
	if (inProc == NULL) {
			return paramErr;
	} else {
		mRenderCallbacks.deferred_add(RenderCallback(inProc, inRefCon));
			// this will do nothing if it's already in the list
	}
	return noErr;
}

//_____________________________________________________________________________
//
ComponentResult		AUBase::RemoveRenderNotification(	ProcPtr						inProc,
														void *						inRefCon)
{
	mRenderCallbacks.deferred_remove(RenderCallback(inProc, inRefCon));
	return noErr;	// error?
}

//_____________________________________________________________________________
//
ComponentResult 	AUBase::GetParameter(			AudioUnitParameterID			inID,
													AudioUnitScope 					inScope,
													AudioUnitElement 				inElement,
													AudioUnitParameterValue &		outValue)
{
	if (inScope == kAudioUnitScope_Group) {
		return GetGroupParameter (inID, inElement, outValue);
	}

	AUElement *elem = SafeGetElement(inScope, inElement);
	outValue = elem->GetParameter(inID);
	return noErr;
}


//_____________________________________________________________________________
//
ComponentResult 	AUBase::SetParameter(			AudioUnitParameterID			inID,
													AudioUnitScope 					inScope,
													AudioUnitElement 				inElement,
													AudioUnitParameterValue			inValue,
													UInt32							inBufferOffsetInFrames)
{
	if (inScope == kAudioUnitScope_Group) {
		return SetGroupParameter (inID, inElement, inValue, inBufferOffsetInFrames);
	}

	AUElement *elem = SafeGetElement(inScope, inElement);
	elem->SetParameter(inID, inValue);
	return noErr;
}

//_____________________________________________________________________________
//
ComponentResult 	AUBase::SetGroupParameter(		AudioUnitParameterID			inID,
													AudioUnitElement 				inElement,
													AudioUnitParameterValue			inValue,
													UInt32							inBufferOffsetInFrames)
{
	return kAudioUnitErr_InvalidScope;
}

//_____________________________________________________________________________
//
ComponentResult 	AUBase::GetGroupParameter(		AudioUnitParameterID			inID,
													AudioUnitElement 				inElement,
													AudioUnitParameterValue &		outValue)
{
	return kAudioUnitErr_InvalidScope;
}

//_____________________________________________________________________________
//
ComponentResult 	AUBase::ScheduleParameter (	const AudioUnitParameterEvent 		*inParameterEvent,
													UInt32							inNumEvents)
{
	for (UInt32 i = 0; i < inNumEvents; ++i)
	{
		if (inParameterEvent[i].eventType == kParameterEvent_Immediate)
		{
			SetParameter (inParameterEvent[i].parameter,
							inParameterEvent[i].scope,
							inParameterEvent[i].element,
							inParameterEvent[i].eventValues.immediate.value,
							inParameterEvent[i].eventValues.immediate.bufferOffset);
		}
		mParamList.push_back (inParameterEvent[i]);
	}

	return noErr;
}

// ____________________________________________________________________________
//
static bool SortParameterEventList(const AudioUnitParameterEvent &ev1, const AudioUnitParameterEvent &ev2 )
{
	int offset1 = ev1.eventType == kParameterEvent_Immediate ?  ev1.eventValues.immediate.bufferOffset : ev1.eventValues.ramp.startBufferOffset;
	int offset2 = ev2.eventType == kParameterEvent_Immediate ?  ev2.eventValues.immediate.bufferOffset : ev2.eventValues.ramp.startBufferOffset;

	if(offset1 < offset2) return true;
	return false;
}


// ____________________________________________________________________________
//
ComponentResult 	AUBase::ProcessForScheduledParams(	ParameterEventList		&inParamList,
														UInt32					inFramesToProcess,
														void					*inUserData )
{
	ComponentResult result = noErr;

	int totalFramesToProcess = inFramesToProcess;

	int framesRemaining = totalFramesToProcess;

	unsigned int currentStartFrame = 0;	// start of the whole buffer



	// sort the ParameterEventList by startBufferOffset
	std::sort(inParamList.begin(), inParamList.end(), SortParameterEventList);

	ParameterEventList::iterator iter = inParamList.begin();


	while(framesRemaining > 0 )
	{
		// first of all, go through the ramped automation events and find out where the next
		// division of our whole buffer will be

		int currentEndFrame = totalFramesToProcess;	// start out assuming we'll process all the way to
													// the end of the buffer

		iter = inParamList.begin();

		// find the next break point
		while(iter != inParamList.end() )
		{
			AudioUnitParameterEvent &event = *iter;

			int offset = event.eventType == kParameterEvent_Immediate ?  event.eventValues.immediate.bufferOffset : event.eventValues.ramp.startBufferOffset;

			if(offset > (int)currentStartFrame && offset < currentEndFrame )
			{
				currentEndFrame = offset;
				break;
			}

			// consider ramp end to be a possible choice (there may be gaps in the supplied ramp events)
			if(event.eventType == kParameterEvent_Ramped )
			{
				offset = event.eventValues.ramp.startBufferOffset + event.eventValues.ramp.durationInFrames;

				if(offset > (int)currentStartFrame && offset < currentEndFrame )
				{
					currentEndFrame = offset;
				}
			}

			iter++;
		}

		int framesThisTime = currentEndFrame - currentStartFrame;

		// next, setup the parameter maps to be current for the ramp parameters active during
		// this time segment...

		for(ParameterEventList::iterator iter2 = inParamList.begin(); iter2 != inParamList.end(); iter2++ )
		{
			AudioUnitParameterEvent &event = *iter2;

			bool eventFallsInSlice;


			if(event.eventType == kParameterEvent_Ramped)
				eventFallsInSlice = event.eventValues.ramp.startBufferOffset < currentEndFrame
					&& event.eventValues.ramp.startBufferOffset + event.eventValues.ramp.durationInFrames > currentStartFrame;
			else /* kParameterEvent_Immediate */
				// actually, for the same parameter, there may be future immediate events which override this one,
				// but it's OK since the event list is sorted in time order, we're guaranteed to end up with the current one
				eventFallsInSlice = event.eventValues.immediate.bufferOffset <= currentStartFrame;

			if(eventFallsInSlice)
			{
				AUElement *element = GetElement(event.scope, event.element );

				if(element) element->SetScheduledEvent(	event.parameter,
														event,
														currentStartFrame,
														currentEndFrame - currentStartFrame );
			}
		}



		// Finally, actually do the processing for this slice.....

		result = ProcessScheduledSlice(	inUserData,
										currentStartFrame,
										framesThisTime,
										inFramesToProcess );

		if(result != noErr) break;

		framesRemaining -= framesThisTime;
		currentStartFrame = currentEndFrame;	// now start from where we left off last time
	}

	return result;
}

//_____________________________________________________________________________
//
void				AUBase::SetWantsRenderThreadID (bool inFlag)
{
	if (inFlag == mWantsRenderThreadID)
		return;

	mWantsRenderThreadID = inFlag;
	if (!mWantsRenderThreadID)
		mRenderThreadID = NULL;
}

//_____________________________________________________________________________
//

//_____________________________________________________________________________
//
ComponentResult		AUBase::DoRender(		AudioUnitRenderActionFlags &	ioActionFlags,
											const AudioTimeStamp &			inTimeStamp,
											UInt32							inBusNumber,
											UInt32							inFramesToProcess,
											AudioBufferList &				ioData)
{
	ComponentResult theError;
	RenderCallbackList::iterator rcit;

	DISABLE_DENORMALS

	try {
		if (!IsInitialized())
			COMPONENT_THROW(kAudioUnitErr_Uninitialized);

		check(mAudioUnitAPIVersion >= 2);

		if (inFramesToProcess > mMaxFramesPerSlice)
			COMPONENT_THROW(kAudioUnitErr_TooManyFramesToProcess);

		AUOutputElement *output = GetOutput(inBusNumber);	// will throw if non-existant
		if (output->GetStreamFormat().NumberChannelStreams() != ioData.mNumberBuffers) {
			debug_string("DoRender: output stream format does not match ioData.mNumberBuffers");
			COMPONENT_THROW(paramErr);
		}

		if (WantsRenderThreadID())
		{
			#if TARGET_OS_MAC
				mRenderThreadID = pthread_self();
			#elif TARGET_OS_WIN32
				mRenderThreadID = GetCurrentThreadId();
			#endif
		}

		mRenderCallbacks.update();
		AudioUnitRenderActionFlags flags = ioActionFlags | kAudioUnitRenderAction_PreRender;
		for (rcit = mRenderCallbacks.begin(); rcit != mRenderCallbacks.end(); ++rcit) {
			RenderCallback &rc = *rcit;
			(*(AURenderCallback)rc.mRenderNotify)(rc.mRenderNotifyRefCon,
							&flags,
							&inTimeStamp, inBusNumber, inFramesToProcess, &ioData);
		}

		theError = DoRenderBus(ioActionFlags, inTimeStamp, inBusNumber, output, inFramesToProcess, ioData);

		flags = ioActionFlags | kAudioUnitRenderAction_PostRender;

		if (SetRenderError (theError)) {
			flags |= kAudioUnitRenderAction_PostRenderError;	//commentd to compile under 10.4
		}

		for (rcit = mRenderCallbacks.begin(); rcit != mRenderCallbacks.end(); ++rcit) {
			RenderCallback &rc = *rcit;
			(*(AURenderCallback)rc.mRenderNotify)(rc.mRenderNotifyRefCon,
							&flags,
							&inTimeStamp, inBusNumber, inFramesToProcess, &ioData);
		}

		// The vector's being emptied
		// because these events should only apply to this Render cycle, so anything
		// left over is from a preceding cycle and should be dumped.  New scheduled
		// parameters must be scheduled from the next pre-render callback.
		if (!mParamList.empty())
			mParamList.clear();

	}
	catch (OSStatus err) {
		DebugMessageN1 ("  from %s", GetLoggingString());
		theError = SetRenderError (err);
	}

	RESTORE_DENORMALS

	return theError;
}

//_____________________________________________________________________________
//
ComponentResult		AUBase::SetInputCallback(		UInt32							inPropertyID,
													AudioUnitElement 				inElement,
													ProcPtr							inProc,
													void *							inRefCon)
{
	AUInputElement *input = GetInput(inElement);	// may throw

	input->SetInputCallback(inProc, inRefCon);
	PropertyChanged(inPropertyID, kAudioUnitScope_Input, inElement);

	return noErr;
}

//_____________________________________________________________________________
//
ComponentResult		AUBase::SetConnection(			const AudioUnitConnection &		inConnection)
{
	OSStatus err;
	AUInputElement *input = GetInput(inConnection.destInputNumber);	// may throw

	if (inConnection.sourceAudioUnit) {
		// connecting, not disconnecting
		CAStreamBasicDescription sourceDesc;
		UInt32 size = sizeof(CAStreamBasicDescription);
		require_noerr(err = AudioUnitGetProperty(
										inConnection.sourceAudioUnit,
										kAudioUnitProperty_StreamFormat,
										kAudioUnitScope_Output,
										inConnection.sourceOutputNumber,
										&sourceDesc,
										&size), errexit);
		require_noerr(err = DispatchSetProperty (kAudioUnitProperty_StreamFormat,
								kAudioUnitScope_Input, inConnection.destInputNumber,
								&sourceDesc, sizeof(CAStreamBasicDescription)), errexit);
	}
	input->SetConnection(inConnection);

	PropertyChanged(kAudioUnitProperty_MakeConnection, kAudioUnitScope_Input, inConnection.destInputNumber);
	return noErr;

errexit:
	return err;
}

//_____________________________________________________________________________
//
UInt32				AUBase::SupportedNumChannels (	const AUChannelInfo** 			outInfo)
{
	return 0;
}

//_____________________________________________________________________________
//
bool				AUBase::ValidFormat(			AudioUnitScope					inScope,
													AudioUnitElement				inElement,
													const CAStreamBasicDescription &		inNewFormat)
{
	return FormatIsCanonical(inNewFormat);
}

//_____________________________________________________________________________
//
bool				AUBase::IsStreamFormatWritable(	AudioUnitScope					scope,
													AudioUnitElement				element)
{
	switch (scope) {
	case kAudioUnitScope_Input:
		{
			AUInputElement *input = GetInput(element);
			if (input->HasConnection()) return false;	// can't write format when input comes from connection
		}
		// ... fall ...
	case kAudioUnitScope_Output:
		return StreamFormatWritable(scope, element);

//#warning "aliasing of global scope format should be pushed to subclasses"
	case kAudioUnitScope_Global:
		return StreamFormatWritable(kAudioUnitScope_Output, 0);
	}
	return false;
}

//_____________________________________________________________________________
//
const CAStreamBasicDescription &
					AUBase::GetStreamFormat(		AudioUnitScope					inScope,
													AudioUnitElement				inElement)
{
//#warning "aliasing of global scope format should be pushed to subclasses"
	AUIOElement *element;

	switch (inScope) {
	case kAudioUnitScope_Input:
		element = Inputs().GetIOElement(inElement);
		break;
	case kAudioUnitScope_Output:
		element = Outputs().GetIOElement(inElement);
		break;
	case kAudioUnitScope_Global:	// global stream description is an alias for that of output 0
		element = Outputs().GetIOElement(0);
		break;
	default:
		COMPONENT_THROW(kAudioUnitErr_InvalidScope);
	}
	return element->GetStreamFormat();
}

ComponentResult		AUBase::SetBusCount(	AudioUnitScope					inScope,
											UInt32 							inCount)
{
	GetScope(inScope).SetNumberOfElements(inCount);
	return noErr;
}

//_____________________________________________________________________________
//
ComponentResult		AUBase::ChangeStreamFormat(		AudioUnitScope					inScope,
													AudioUnitElement				inElement,
													const CAStreamBasicDescription & inPrevFormat,
													const CAStreamBasicDescription &	inNewFormat)
{
//#warning "aliasing of global scope format should be pushed to subclasses"
	AUIOElement *element;

	switch (inScope) {
	case kAudioUnitScope_Input:
		element = Inputs().GetIOElement(inElement);
		break;
	case kAudioUnitScope_Output:
		element = Outputs().GetIOElement(inElement);
		break;
	case kAudioUnitScope_Global:
		element = Outputs().GetIOElement(0);
		break;
	default:
		COMPONENT_THROW(kAudioUnitErr_InvalidScope);
	}
	element->SetStreamFormat(inNewFormat);
	PropertyChanged(kAudioUnitProperty_StreamFormat, inScope, inElement);
	return noErr;
}

UInt32		AUBase::GetChannelLayoutTags(	AudioUnitScope				inScope,
											AudioUnitElement 			inElement,
											AudioChannelLayoutTag *		outLayoutTags)
{
	return GetIOElement(inScope, inElement)->GetChannelLayoutTags(outLayoutTags);
}

UInt32		AUBase::GetAudioChannelLayout(	AudioUnitScope				scope,
											AudioUnitElement 			element,
											AudioChannelLayout *		outLayoutPtr,
											Boolean &					outWritable)
{
	AUIOElement * el = GetIOElement(scope, element);
	return el->GetAudioChannelLayout(outLayoutPtr, outWritable);
}

OSStatus	AUBase::RemoveAudioChannelLayout(			AudioUnitScope				inScope,
														AudioUnitElement			inElement)
{
	OSStatus result = noErr;
	AUIOElement * el = GetIOElement(inScope, inElement);
	Boolean writable;
	if (el->GetAudioChannelLayout(NULL, writable)) {
		result = el->RemoveAudioChannelLayout();
	}
	return result;
}

OSStatus 	AUBase::SetAudioChannelLayout(				AudioUnitScope 				inScope,
														AudioUnitElement 			inElement,
														const AudioChannelLayout *	inLayout)
{
	AUIOElement* ioEl = GetIOElement (inScope, inElement);

	// the num channels of the layout HAS TO MATCH the current channels of the Element's stream format
	UInt32 currentChannels = ioEl->GetStreamFormat().NumberChannels();
	UInt32 numChannelsInLayout = CAAudioChannelLayout::NumberChannels(*inLayout);
	if (currentChannels != numChannelsInLayout)
		return kAudioUnitErr_InvalidPropertyValue;

	UInt32 numLayouts = GetChannelLayoutTags (inScope, inElement, NULL);
	if (numLayouts == 0)
		return kAudioUnitErr_InvalidProperty;
	AudioChannelLayoutTag *tags = (AudioChannelLayoutTag *)malloc (numLayouts * sizeof (AudioChannelLayoutTag));
	GetChannelLayoutTags (inScope, inElement, tags);
	bool foundTag = false;
	for (unsigned int i = 0; i < numLayouts; ++i) {
		if (tags[i] == inLayout->mChannelLayoutTag || tags[i] == kAudioChannelLayoutTag_UseChannelDescriptions) {
			foundTag = true;
			break;
		}
	}
	free(tags);

	if (foundTag == false)
		return kAudioUnitErr_InvalidPropertyValue;

	return ioEl->SetAudioChannelLayout(*inLayout);
}

static void AddNumToDictionary (CFMutableDictionaryRef dict, CFStringRef key, SInt32 value)
{
	CFNumberRef num = CFNumberCreate (NULL, kCFNumberSInt32Type, &value);
	CFDictionarySetValue (dict, key, num);
	CFRelease (num);
}

#define kCurrentSavedStateVersion 0

ComponentResult		AUBase::SaveState(		CFPropertyListRef * outData)
{
	ComponentDescription desc;
	OSStatus result = GetComponentInfo((Component)GetComponentInstance(), &desc, NULL, NULL, NULL);
	if (result) return result;

	CFMutableDictionaryRef dict = CFDictionaryCreateMutable	(NULL, 0,
								&kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);

// first step -> save the version to the data ref
	SInt32 value = kCurrentSavedStateVersion;
	AddNumToDictionary (dict, kVersionString, value);

// second step -> save the component type, subtype, manu to the data ref
	value = desc.componentType;
	AddNumToDictionary (dict, kTypeString, value);

	value = desc.componentSubType;
	AddNumToDictionary (dict, kSubtypeString, value);

	value = desc.componentManufacturer;
	AddNumToDictionary (dict, kManufacturerString, value);

// fourth step -> save the state of all parameters on all scopes and elements
	CFMutableDataRef data = CFDataCreateMutable(NULL, 0);
	for (AudioUnitScope iscope = 0; iscope < 4; ++iscope) {
		if (iscope == kAudioUnitScope_Group)
			continue;
		AUScope &scope = GetScope(iscope);
		AudioUnitElement nElems = scope.GetNumberOfElements();
		for (AudioUnitElement ielem = 0; ielem < nElems; ++ielem) {
			AUElement *element = scope.GetElement(ielem);
			UInt32 nparams = element->GetNumberOfParameters();
			if (nparams > 0) {
				struct {
					UInt32	scope;
					UInt32	element;
				} hdr;

				hdr.scope = CFSwapInt32HostToBig(iscope);
				hdr.element = CFSwapInt32HostToBig(ielem);
				CFDataAppendBytes(data, (UInt8 *)&hdr, sizeof(hdr));

				element->SaveState(data);
			}
		}
	}

// save all this in the data section of the dictionary
	CFDictionarySetValue(dict, kDataString, data);
	CFRelease (data);

//OK - now we're going to do some properties
//save the preset name...
	CFDictionarySetValue (dict, kNameString, mCurrentPreset.presetName);

// Does the unit support the RenderQuality property - if so, save it...
	value = 0;
	result = DispatchGetProperty (kAudioUnitProperty_RenderQuality,
								kAudioUnitScope_Global,
								0,
								&value);

	if (result == noErr) {
		AddNumToDictionary (dict, kRenderQualityString, value);
	}

// Does the unit support the CPULoad Quality property - if so, save it...
	Float32 cpuLoad;
	result = DispatchGetProperty (kAudioUnitProperty_CPULoad,
								kAudioUnitScope_Global,
								0,
								&cpuLoad);

	if (result == noErr) {
		CFNumberRef num = CFNumberCreate (NULL, kCFNumberFloatType, &cpuLoad);
		CFDictionarySetValue (dict, kCPULoadString, num);
		CFRelease (num);
	}

// Do we have any element names for any of our scopes?
	// first check to see if we have any names...
	bool foundName = false;
	for (AudioUnitScope i = 0; i < kNumScopes; ++i) {
		foundName = GetScope (i).HasElementWithName();
		if (foundName)
			break;
	}
		// OK - we found a name away we go...
	if (foundName) {
		CFMutableDictionaryRef nameDict = CFDictionaryCreateMutable	(NULL, 0,
								&kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
		for (AudioUnitScope i = 0; i < kNumScopes; ++i) {
			GetScope (i).AddElementNamesToDict (nameDict);
		}

		CFDictionarySetValue (dict, kElementNameString, nameDict);
		CFRelease (nameDict);
	}

// we're done!!!
	*outData = dict;

	return noErr;
}

//_____________________________________________________________________________
//
ComponentResult		AUBase::RestoreState(	CFPropertyListRef	plist)
{
	if (CFGetTypeID(plist) != CFDictionaryGetTypeID()) return kAudioUnitErr_InvalidPropertyValue;

	ComponentDescription desc;
	OSStatus result = GetComponentInfo((Component)GetComponentInstance(), &desc, NULL, NULL, NULL);
	if (result) return result;

	CFDictionaryRef dict = static_cast<CFDictionaryRef>(plist);

// zeroeth step - make sure the Part key is NOT present, as this method is used
// to restore the GLOBAL state of the dictionary
	if (CFDictionaryContainsKey (dict, kPartString))
		return kAudioUnitErr_InvalidPropertyValue;

// first step -> check the saved version in the data ref
// at this point we're only dealing with version==0
	CFNumberRef cfnum = reinterpret_cast<CFNumberRef>(CFDictionaryGetValue (dict, kVersionString));
	if (cfnum == NULL) return kAudioUnitErr_InvalidPropertyValue;
	SInt32 value;
	CFNumberGetValue (cfnum, kCFNumberSInt32Type, &value);
	if (value != kCurrentSavedStateVersion) return kAudioUnitErr_InvalidPropertyValue;

// second step -> check that this data belongs to this kind of audio unit
// by checking the component subtype and manuID
// We're not checking the type, since there may be different versions (effect, format-converter, offline)
// of essentially the same AU
	cfnum = reinterpret_cast<CFNumberRef>(CFDictionaryGetValue (dict, kSubtypeString));
	if (cfnum == NULL) return kAudioUnitErr_InvalidPropertyValue;
	CFNumberGetValue (cfnum, kCFNumberSInt32Type, &value);
	if (UInt32(value) != desc.componentSubType) return kAudioUnitErr_InvalidPropertyValue;

	cfnum = reinterpret_cast<CFNumberRef>(CFDictionaryGetValue (dict, kManufacturerString));
	if (cfnum == NULL) return kAudioUnitErr_InvalidPropertyValue;
	CFNumberGetValue (cfnum, kCFNumberSInt32Type, &value);
	if (UInt32(value) != desc.componentManufacturer) return kAudioUnitErr_InvalidPropertyValue;

// fourth step -> restore the state of all of the parameters for each scope and element
	CFDataRef data = reinterpret_cast<CFDataRef>(CFDictionaryGetValue (dict, kDataString));
	if (data != NULL)
	{
		const UInt8 *p, *pend;

		p = CFDataGetBytePtr(data);
		pend = p + CFDataGetLength(data);

		// we have a zero length data, which may just mean there were no parameters to save!
		//	if (p >= pend) return noErr;

		while (p < pend) {
			struct {
				UInt32	scope;
				UInt32	element;
			} hdr;

			hdr.scope = CFSwapInt32BigToHost(*(UInt32 *)p);		p += sizeof(UInt32);
			hdr.element = CFSwapInt32BigToHost(*(UInt32 *)p);	p += sizeof(UInt32);

			AUScope &scope = GetScope(hdr.scope);
			AUElement *element = scope.GetElement(hdr.element);
				// $$$ order of operations issue: what if the element does not yet exist?
				// then we just break out of the loop
			if (!element)
				break;
			p = element->RestoreState(p);
		}
	}
#if 0
// we're not going to do anything here - we'll assume that the subclass can handle this case
// if translating from either VST or MAS formats
// If an AU want this method to validate that the preset is valid and contains either/or a vstdata
// or msdata key, then this can be done here as below for the vstdata example.
	else // no data key
	{
		if (!CFDictionaryContainsKey (dict, kVSTDataString))
			return kAudioUnitErr_InvalidPropertyValue;
	}
#endif

//OK - now we're going to do some properties
//restore the preset name...
	CFStringRef name = reinterpret_cast<CFStringRef>(CFDictionaryGetValue (dict, kNameString));
	if (mCurrentPreset.presetName) CFRelease (mCurrentPreset.presetName);
	if (name)
	{
		mCurrentPreset.presetName = name;
		mCurrentPreset.presetNumber = -1;
	}
	else { // no name entry make the default one
		mCurrentPreset.presetName = kUntitledString;
		mCurrentPreset.presetNumber = -1;
	}

	CFRetain (mCurrentPreset.presetName);
#ifndef __LP64__
	PropertyChanged(kAudioUnitProperty_CurrentPreset, kAudioUnitScope_Global, 0);
#endif
	PropertyChanged(kAudioUnitProperty_PresentPreset, kAudioUnitScope_Global, 0);

// Does the dict contain render quality information?
	if (CFDictionaryGetValueIfPresent (dict, kRenderQualityString, reinterpret_cast<const void**>(&cfnum)))
	{
		CFNumberGetValue (cfnum, kCFNumberSInt32Type, &value);
		DispatchSetProperty (kAudioUnitProperty_RenderQuality,
								kAudioUnitScope_Global,
								0,
								&value,
								sizeof(value));
	}

// Does the unit support the CPULoad Quality property - if so, save it...
	if (CFDictionaryGetValueIfPresent (dict, kCPULoadString, reinterpret_cast<const void**>(&cfnum)))
	{
		Float32 floatValue;
		CFNumberGetValue (cfnum, kCFNumberFloatType, &floatValue);
		DispatchSetProperty (kAudioUnitProperty_CPULoad,
								kAudioUnitScope_Global,
								0,
								&floatValue,
								sizeof(floatValue));
	}

// Do we have any element names for any of our scopes?
	CFDictionaryRef nameDict;
	if (CFDictionaryGetValueIfPresent (dict, kElementNameString, reinterpret_cast<const void**>(&nameDict)))
	{
		char string[64];
		for (int i = 0; i < kNumScopes; ++i)
		{
			sprintf (string, "%d", i);
			CFStringRef key = CFStringCreateWithCString (NULL, string, kCFStringEncodingASCII);
			CFDictionaryRef elementDict;
			if (CFDictionaryGetValueIfPresent (nameDict, key, reinterpret_cast<const void**>(&elementDict)))
			{
				bool didAddElements = GetScope (i).RestoreElementNames (elementDict);
				if (didAddElements)
					PropertyChanged (kAudioUnitProperty_ElementCount, i, 0);
			}
			CFRelease (key);
		}
	}

	return noErr;
}

ComponentResult		AUBase::GetPresets (			CFArrayRef * 					outData) const
{
	return kAudioUnitErr_InvalidProperty;
}

OSStatus			AUBase::NewFactoryPresetSet (const AUPreset & inNewFactoryPreset)
{
	return kAudioUnitErr_InvalidProperty;
}

		// set the default preset for the unit -> the number of the preset MUST be >= 0
		// and the name should be valid, or the preset WON'T take
bool				AUBase::SetAFactoryPresetAsCurrent (const AUPreset & inPreset)
{
	if (inPreset.presetNumber < 0 || inPreset.presetName == NULL) return false;
	CFRelease (mCurrentPreset.presetName);
	mCurrentPreset = inPreset;
	CFRetain (mCurrentPreset.presetName);
	return true;
}

int			AUBase::GetNumCustomUIComponents ()
{
	return 0;
}

void		AUBase::GetUIComponentDescs (ComponentDescription* inDescArray) {}

bool		AUBase::HasIcon ()
{
	CFURLRef url = CopyIconLocation();
	if (url) {
		CFRelease (url);
		return true;
	}
	return false;
}

CFURLRef	AUBase::CopyIconLocation ()
{
	return NULL;
}

//_____________________________________________________________________________
//
ComponentResult		AUBase::GetParameterList(		AudioUnitScope					inScope,
													AudioUnitParameterID *			outParameterList,
													UInt32 &						outNumParameters)
{
	AUScope &scope = GetScope(inScope);
	AUElement *elementWithMostParameters = NULL;
	UInt32 maxNumParams = 0;

	int nElems = scope.GetNumberOfElements();
	for (int ielem = 0; ielem < nElems; ++ielem) {
		AUElement *element = scope.GetElement(ielem);
		UInt32 nParams = element->GetNumberOfParameters();
		if (nParams > maxNumParams) {
			maxNumParams = nParams;
			elementWithMostParameters = element;
		}
	}

	if (outParameterList != NULL && elementWithMostParameters != NULL)
		elementWithMostParameters->GetParameterList(outParameterList);

	outNumParameters = maxNumParams;
	return noErr;
}

//_____________________________________________________________________________
//
ComponentResult		AUBase::GetParameterInfo(		AudioUnitScope			inScope,
													AudioUnitParameterID	inParameterID,
													AudioUnitParameterInfo	&outParameterInfo )
{
	return kAudioUnitErr_InvalidParameter;
}

//_____________________________________________________________________________
//
ComponentResult		AUBase::GetParameterValueStrings(AudioUnitScope					inScope,
													AudioUnitParameterID			inParameterID,
													CFArrayRef *					outStrings)
{
	return kAudioUnitErr_InvalidProperty;	// this AU doesn't know how
}

//_____________________________________________________________________________
//
void				AUBase::SetNumberOfElements(	AudioUnitScope					inScope,
													UInt32							numElements)
{
	if (inScope == kAudioUnitScope_Global && numElements != 1)
		COMPONENT_THROW(kAudioUnitErr_InvalidScope);

	GetScope(inScope).SetNumberOfElements(numElements);
}

//_____________________________________________________________________________
//
AUElement *			AUBase::CreateElement(			AudioUnitScope					scope,
													AudioUnitElement				element)
{
	switch (scope) {
	case kAudioUnitScope_Global:
		return new AUGlobalElement(this);
	case kAudioUnitScope_Input:
		return new AUInputElement(this);
	case kAudioUnitScope_Output:
		return new AUOutputElement(this);
	case kAudioUnitScope_Group:
		return new AUGroupElement(this);
	case kAudioUnitScope_Part:
		return new AUPartElement(this);
	}
	COMPONENT_THROW(kAudioUnitErr_InvalidScope);

	return NULL;	// get rid of compiler warning
}

//_____________________________________________________________________________
//
bool	AUBase::FormatIsCanonical(		const CAStreamBasicDescription &f)
{
	return (f.mFormatID == kAudioFormatLinearPCM
		&&	f.mFramesPerPacket == 1
		&&	f.mBytesPerPacket == f.mBytesPerFrame
//		&&	f.mChannelsPerFrame >= 0	-- this is always true since it's unsigned
		// so far, it's a valid PCM format
		&&	(f.mFormatFlags & kLinearPCMFormatFlagIsFloat) != 0
		&&	((f.mFormatFlags & kAudioFormatFlagIsNonInterleaved) == 0) == (mAudioUnitAPIVersion == 1)
#if TARGET_RT_BIG_ENDIAN
		&&	(f.mFormatFlags & kLinearPCMFormatFlagIsBigEndian) != 0
#else
		&&	(f.mFormatFlags & kLinearPCMFormatFlagIsBigEndian) == 0
#endif
		&&	f.mBitsPerChannel == 32
		&&	f.mBytesPerFrame == f.NumberInterleavedChannels() * sizeof(AudioSampleType)
		);
}

//_____________________________________________________________________________
//
void	AUBase::MakeCanonicalFormat(	CAStreamBasicDescription &		f,
										int								nChannels)
{
	f.SetCanonical(nChannels, mAudioUnitAPIVersion < 2);	// interleaved for v1, non for v2
	f.mSampleRate = 0.0;
}

const Float64 AUBase::kNoLastRenderedSampleTime = -1.;

//_____________________________________________________________________________
//
char *StringForOSType (OSType t, char *writeLocation)
{
	char *p = writeLocation;
	unsigned char str[4], *q = str;
	*(UInt32 *)str = CFSwapInt32HostToBig(t);

	bool hasNonPrint = false;
	for (int i = 0; i < 4; ++i) {
		if (!(isprint(*q) && *q != '\\')) {
			hasNonPrint = true;
			break;
		}
	}

	if (hasNonPrint)
		p += sprintf (p, "0x");

	for (int i = 0; i < 4; ++i) {
		if (hasNonPrint) {
			p += sprintf(p, "%02X", *q++);
		} else {
			*p++ = *q++;
		}
	}
	*p = '\0';
	return writeLocation;
}

char*	AUBase::GetLoggingString () const
{
	if (mLogString) return mLogString;

	ComponentDescription desc;
	OSStatus err = GetComponentInfo((Component)GetComponentInstance(), &desc, NULL, NULL, NULL);
	if (err) return "";

	const_cast<AUBase*>(this)->mLogString = new char[128];
	char str[24];
	char str1[24];
	char str2[24];
	sprintf (const_cast<AUBase*>(this)->mLogString, "AU (%p): \'%s - %s - %s\'",
		GetComponentInstance(),
		StringForOSType(desc.componentType, str),
		StringForOSType(desc.componentSubType, str1),
		StringForOSType(desc.componentManufacturer, str2));

	return mLogString;
}

