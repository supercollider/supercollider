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
	CAVectorUnit.cpp

=============================================================================*/

#include "CAVectorUnit.h"

#if !TARGET_OS_WIN32
	#include <sys/sysctl.h>
#elif HAS_IPP
	#include "ippdefs.h"
	#include "ippcore.h"
#endif

int CAVectorUnit::sVectorUnitType = kVecUninitialized;

SInt32	CAVectorUnit::CheckVectorUnit()
{
	int result = kVecNone;

#if !TARGET_OS_WIN32
#if DEBUG
	if (getenv("CA_NoVector")) {
		fprintf(stderr, "CA_NoVector set; Vector unit optimized routines will be bypassed\n");
		return result;
	}
	else
#endif
	{
	#if (TARGET_CPU_PPC || TARGET_CPU_PPC64)
		int sels[2] = { CTL_HW, HW_VECTORUNIT };
		int vType = 0; //0 == scalar only
		size_t length = sizeof(vType);
		int error = sysctl(sels, 2, &vType, &length, NULL, 0);
		if (!error && vType > 0)
			result = kVecAltivec;
	#elif (TARGET_CPU_X86 || TARGET_CPU_X86_64)
		int answer = 0;
		size_t length = sizeof(answer);
		int error = sysctlbyname("hw.optional.sse3", &answer, &length, NULL, 0);
		if (!error && answer)
			result = kVecSSE3;
		else {
			answer = 0;
			length = sizeof(answer);
			error = sysctlbyname("hw.optional.sse2", &answer, &length, NULL, 0);
			if (!error && answer)
				result = kVecSSE2;
		}
	#endif
	}
#elif HAS_IPP
	int error = ippStaticInit();
	if(ippStsNoErr == error)
	{
		switch(ippGetCpuType())
		{
			case ippCpuP4HT2:
			case ippCpuDS:
				result = kVecSSE3;
				break;
			case ippCpuP4:
			case ippCpuP4HT:
			case ippCpuCentrino:
				result = kVecSSE2;
				break;
			default:
				break;
		}
	}
#endif
	sVectorUnitType = result;
	return result;
}

