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
	CAHostTimeBase.h

=============================================================================*/
#if !defined(__CAHostTimeBase_h__)
#define __CAHostTimeBase_h__

//=============================================================================
//	Includes
//=============================================================================

#if !defined(__COREAUDIO_USE_FLAT_INCLUDES__)
	#include <CoreAudio/CoreAudioTypes.h>
#else
	#include <CoreAudioTypes.h>
#endif

#if TARGET_OS_MAC
	#include <mach/mach_time.h>
#elif TARGET_OS_WIN32
	#include <windows.h>
#else
	#error	Unsupported operating system
#endif

#include "CADebugMacros.h"

//=============================================================================
//	CAHostTimeBase
//
//	This class provides platform independent access to the host's time base.
//=============================================================================

#if CoreAudio_Debug
//	#define Log_Host_Time_Base_Parameters	1
//	#define Track_Host_TimeBase				1
#endif

class	CAHostTimeBase
{

public:
	static UInt64	ConvertToNanos(UInt64 inHostTime);
	static UInt64	ConvertFromNanos(UInt64 inNanos);

	static UInt64	GetTheCurrentTime();
#if TARGET_OS_MAC
	static UInt64	GetCurrentTime() { return GetTheCurrentTime(); }
#endif
	static UInt64	GetCurrentTimeInNanos();

	static Float64	GetFrequency() { if(!sIsInited) { Initialize(); } return sFrequency; }
	static UInt32	GetMinimumDelta() { if(!sIsInited) { Initialize(); } return sMinDelta; }

	static UInt64	AbsoluteHostDeltaToNanos(UInt64 inStartTime, UInt64 inEndTime);
	static SInt64	HostDeltaToNanos(UInt64 inStartTime, UInt64 inEndTime);

private:
	static void		Initialize();

	static bool		sIsInited;

	static Float64	sFrequency;
	static UInt32	sMinDelta;
	static UInt32	sToNanosNumerator;
	static UInt32	sToNanosDenominator;
	static UInt32	sFromNanosNumerator;
	static UInt32	sFromNanosDenominator;
	static bool		sUseMicroseconds;
#if Track_Host_TimeBase
	static UInt64	sLastTime;
#endif
};

inline UInt64	CAHostTimeBase::GetTheCurrentTime()
{
	UInt64 theTime = 0;

	#if TARGET_OS_MAC
		theTime = mach_absolute_time();
	#elif TARGET_OS_WIN32
		LARGE_INTEGER theValue;
		QueryPerformanceCounter(&theValue);
		theTime = *((UInt64*)&theValue);
	#endif

	#if	Track_Host_TimeBase
		if(sLastTime != 0)
		{
			if(theTime <= sLastTime)
			{
				DebugMessageN2("CAHostTimeBase::GetTheCurrentTime: the current time is earlier than the last time, now: %qd, then: %qd", theTime, sLastTime);
			}
			sLastTime = theTime;
		}
		else
		{
			sLastTime = theTime;
		}
	#endif

	return theTime;
}

inline UInt64	CAHostTimeBase::ConvertToNanos(UInt64 inHostTime)
{
	if(!sIsInited)
	{
		Initialize();
	}

	Float64 theNumerator = static_cast<Float64>(sToNanosNumerator);
	Float64 theDenominator = static_cast<Float64>(sToNanosDenominator);
	Float64 theHostTime = static_cast<Float64>(inHostTime);

	Float64 thePartialAnswer = theHostTime / theDenominator;
	Float64 theFloatAnswer = thePartialAnswer * theNumerator;
	UInt64 theAnswer = static_cast<UInt64>(theFloatAnswer);

	//Assert(!((theNumerator > theDenominator) && (theAnswer < inHostTime)), "CAHostTimeBase::ConvertToNanos: The conversion wrapped");
	//Assert(!((theDenominator > theNumerator) && (theAnswer > inHostTime)), "CAHostTimeBase::ConvertToNanos: The conversion wrapped");

	return theAnswer;
}

inline UInt64	CAHostTimeBase::ConvertFromNanos(UInt64 inNanos)
{
	if(!sIsInited)
	{
		Initialize();
	}

	Float64 theNumerator = static_cast<Float64>(sToNanosNumerator);
	Float64 theDenominator = static_cast<Float64>(sToNanosDenominator);
	Float64 theNanos = static_cast<Float64>(inNanos);

	Float64 thePartialAnswer = theNanos / theNumerator;
	Float64 theFloatAnswer = thePartialAnswer * theDenominator;
	UInt64 theAnswer = static_cast<UInt64>(theFloatAnswer);

	//Assert(!((theDenominator > theNumerator) && (theAnswer < inNanos)), "CAHostTimeBase::ConvertToNanos: The conversion wrapped");
	//Assert(!((theNumerator > theDenominator) && (theAnswer > inNanos)), "CAHostTimeBase::ConvertToNanos: The conversion wrapped");

	return theAnswer;
}


inline UInt64	CAHostTimeBase::GetCurrentTimeInNanos()
{
	return ConvertToNanos(GetTheCurrentTime());
}

inline UInt64	CAHostTimeBase::AbsoluteHostDeltaToNanos(UInt64 inStartTime, UInt64 inEndTime)
{
	UInt64 theAnswer;

	if(inStartTime <= inEndTime)
	{
		theAnswer = inEndTime - inStartTime;
	}
	else
	{
		theAnswer = inStartTime - inEndTime;
	}

	return ConvertToNanos(theAnswer);
}

inline SInt64	CAHostTimeBase::HostDeltaToNanos(UInt64 inStartTime, UInt64 inEndTime)
{
	SInt64 theAnswer;
	SInt64 theSign = 1;

	if(inStartTime <= inEndTime)
	{
		theAnswer = inEndTime - inStartTime;
	}
	else
	{
		theAnswer = inStartTime - inEndTime;
		theSign = -1;
	}

	return theSign * ConvertToNanos(theAnswer);
}

#endif
