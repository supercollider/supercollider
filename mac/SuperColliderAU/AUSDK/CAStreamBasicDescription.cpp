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
	CAStreamBasicDescription.cpp

=============================================================================*/

#include "CAStreamBasicDescription.h"
#include "CAMath.h"

#if !defined(__COREAUDIO_USE_FLAT_INCLUDES__)
	#include <CoreFoundation/CFByteOrder.h>
#else
	#include <CFByteOrder.h>
#endif

#if TARGET_OS_WIN32
	#include <stddef.h>
#endif

#pragma mark	This file needs to compile on earlier versions of the OS, so please keep that in mind when editing it

const AudioStreamBasicDescription	CAStreamBasicDescription::sEmpty = { 0.0, 0, 0, 0, 0, 0, 0, 0, 0 };

CAStreamBasicDescription::CAStreamBasicDescription(double inSampleRate,		UInt32 inFormatID,
									UInt32 inBytesPerPacket,	UInt32 inFramesPerPacket,
									UInt32 inBytesPerFrame,		UInt32 inChannelsPerFrame,
									UInt32 inBitsPerChannel,	UInt32 inFormatFlags)
{
	mSampleRate = inSampleRate;
	mFormatID = inFormatID;
	mBytesPerPacket = inBytesPerPacket;
	mFramesPerPacket = inFramesPerPacket;
	mBytesPerFrame = inBytesPerFrame;
	mChannelsPerFrame = inChannelsPerFrame;
	mBitsPerChannel = inBitsPerChannel;
	mFormatFlags = inFormatFlags;
	mReserved = 0;
}

void CAStreamBasicDescription::PrintFormat2(FILE *f, const char *indent, const char *name) const
{
	fprintf(f, "%s%s ", indent, name);
	char formatID[5];
	*(UInt32 *)formatID = CFSwapInt32HostToBig(mFormatID);
	formatID[4] = '\0';
	fprintf(f, "%2d ch, %6.0f Hz, '%-4.4s' (0x%08X) ",
				(int)NumberChannels(), mSampleRate, formatID,
				(int)mFormatFlags);
	if (mFormatID == kAudioFormatLinearPCM) {
		bool isInt = !(mFormatFlags & kLinearPCMFormatFlagIsFloat);
		int wordSize = SampleWordSize();
		const char *endian = (wordSize > 1) ?
			((mFormatFlags & kLinearPCMFormatFlagIsBigEndian) ? " big-endian" : " little-endian" ) : "";
		const char *sign = isInt ?
			((mFormatFlags & kLinearPCMFormatFlagIsSignedInteger) ? " signed" : " unsigned") : "";
		const char *floatInt = isInt ? "integer" : "float";
		char packed[32];
		if (wordSize > 0 && PackednessIsSignificant()) {
			if (mFormatFlags & kLinearPCMFormatFlagIsPacked)
				sprintf(packed, "packed in %d bytes", wordSize);
			else
				sprintf(packed, "unpacked in %d bytes", wordSize);
		} else
			packed[0] = '\0';
		const char *align = (wordSize > 0 && AlignmentIsSignificant()) ?
			((mFormatFlags & kLinearPCMFormatFlagIsAlignedHigh) ? " high-aligned" : " low-aligned") : "";
		const char *deinter = (mFormatFlags & kAudioFormatFlagIsNonInterleaved) ? ", deinterleaved" : "";
		const char *commaSpace = (packed[0]!='\0') || (align[0]!='\0') ? ", " : "";

		fprintf(f, "%d-bit%s%s %s%s%s%s%s",
			(int)mBitsPerChannel, endian, sign, floatInt,
			commaSpace, packed, align, deinter);
	} else if (mFormatID == 'alac') {	//	kAudioFormatAppleLossless
		int sourceBits = 0;
		switch (mFormatFlags)
		{
			case 1:	//	kAppleLosslessFormatFlag_16BitSourceData
				sourceBits = 16;
				break;
    		case 2:	//	kAppleLosslessFormatFlag_20BitSourceData
    			sourceBits = 20;
    			break;
    		case 3:	//	kAppleLosslessFormatFlag_24BitSourceData
    			sourceBits = 24;
    			break;
    		case 4:	//	kAppleLosslessFormatFlag_32BitSourceData
    			sourceBits = 32;
    			break;
		}
		if (sourceBits)
			fprintf(f, "from %d-bit source, ", sourceBits);
		else
			fprintf(f, "from UNKNOWN source bit depth, ");

		fprintf(f, "%d frames/packet", (int)mFramesPerPacket);
	}
	else
		fprintf(f, "%d bits/channel, %d bytes/packet, %d frames/packet, %d bytes/frame",
			(int)mBitsPerChannel, (int)mBytesPerPacket, (int)mFramesPerPacket, (int)mBytesPerFrame);
}

void	CAStreamBasicDescription::NormalizeLinearPCMFormat(AudioStreamBasicDescription& ioDescription)
{
	//  the only thing that changes is to make mixable linear PCM into the canonical linear PCM format
	if((ioDescription.mFormatID == kAudioFormatLinearPCM) && ((ioDescription.mFormatFlags & kIsNonMixableFlag) == 0))
	{
		//  the canonical linear PCM format
		ioDescription.mFormatFlags = kAudioFormatFlagsCanonical;
		ioDescription.mBytesPerPacket = sizeof(AudioSampleType) * ioDescription.mChannelsPerFrame;
		ioDescription.mFramesPerPacket = 1;
		ioDescription.mBytesPerFrame = sizeof(AudioSampleType) * ioDescription.mChannelsPerFrame;
		ioDescription.mBitsPerChannel = 8 * sizeof(AudioSampleType);
	}
}

void	CAStreamBasicDescription::ResetFormat(AudioStreamBasicDescription& ioDescription)
{
	ioDescription.mSampleRate = 0;
	ioDescription.mFormatID = 0;
	ioDescription.mBytesPerPacket = 0;
	ioDescription.mFramesPerPacket = 0;
	ioDescription.mBytesPerFrame = 0;
	ioDescription.mChannelsPerFrame = 0;
	ioDescription.mBitsPerChannel = 0;
	ioDescription.mFormatFlags = 0;
}

void	CAStreamBasicDescription::FillOutFormat(AudioStreamBasicDescription& ioDescription, const AudioStreamBasicDescription& inTemplateDescription)
{
	if(fiszero(ioDescription.mSampleRate))
	{
		ioDescription.mSampleRate = inTemplateDescription.mSampleRate;
	}
	if(ioDescription.mFormatID == 0)
	{
		ioDescription.mFormatID = inTemplateDescription.mFormatID;
	}
	if(ioDescription.mFormatFlags == 0)
	{
		ioDescription.mFormatFlags = inTemplateDescription.mFormatFlags;
	}
	if(ioDescription.mBytesPerPacket == 0)
	{
		ioDescription.mBytesPerPacket = inTemplateDescription.mBytesPerPacket;
	}
	if(ioDescription.mFramesPerPacket == 0)
	{
		ioDescription.mFramesPerPacket = inTemplateDescription.mFramesPerPacket;
	}
	if(ioDescription.mBytesPerFrame == 0)
	{
		ioDescription.mBytesPerFrame = inTemplateDescription.mBytesPerFrame;
	}
	if(ioDescription.mChannelsPerFrame == 0)
	{
		ioDescription.mChannelsPerFrame = inTemplateDescription.mChannelsPerFrame;
	}
	if(ioDescription.mBitsPerChannel == 0)
	{
		ioDescription.mBitsPerChannel = inTemplateDescription.mBitsPerChannel;
	}
}

void	CAStreamBasicDescription::GetSimpleName(const AudioStreamBasicDescription& inDescription, char* outName, bool inAbbreviate)
{
	switch(inDescription.mFormatID)
	{
		case kAudioFormatLinearPCM:
			{
				const char* theEndianString = NULL;
				if((inDescription.mFormatFlags & kAudioFormatFlagIsBigEndian) != 0)
				{
					#if	TARGET_RT_LITTLE_ENDIAN
						theEndianString = "Big Endian";
					#endif
				}
				else
				{
					#if	TARGET_RT_BIG_ENDIAN
						theEndianString = "Little Endian";
					#endif
				}

				const char* theKindString = NULL;
				if((inDescription.mFormatFlags & kAudioFormatFlagIsFloat) != 0)
				{
					theKindString = (inAbbreviate ? "Float" : "Floating Point");
				}
				else if((inDescription.mFormatFlags & kAudioFormatFlagIsSignedInteger) != 0)
				{
					theKindString = (inAbbreviate ? "SInt" : "Signed Integer");
				}
				else
				{
					theKindString = (inAbbreviate ? "UInt" : "Unsigned Integer");
				}

				const char* thePackingString = NULL;
				if((inDescription.mFormatFlags & kAudioFormatFlagIsPacked) == 0)
				{
					if((inDescription.mFormatFlags & kAudioFormatFlagIsAlignedHigh) != 0)
					{
						thePackingString = "High";
					}
					else
					{
						thePackingString = "Low";
					}
				}

				const char* theMixabilityString = NULL;
				if((inDescription.mFormatFlags & kIsNonMixableFlag) == 0)
				{
					theMixabilityString = "Mixable";
				}
				else
				{
					theMixabilityString = "Unmixable";
				}

				if(inAbbreviate)
				{
					if(theEndianString != NULL)
					{
						if(thePackingString != NULL)
						{
							sprintf(outName, "%s %d Ch %s %s %s%d/%s%d", theMixabilityString, (int)inDescription.mChannelsPerFrame, theEndianString, thePackingString, theKindString, (int)inDescription.mBitsPerChannel, theKindString, (int)(inDescription.mBytesPerFrame / inDescription.mChannelsPerFrame) * 8);
						}
						else
						{
							sprintf(outName, "%s %d Ch %s %s%d", theMixabilityString, (int)inDescription.mChannelsPerFrame, theEndianString, theKindString, (int)inDescription.mBitsPerChannel);
						}
					}
					else
					{
						if(thePackingString != NULL)
						{
							sprintf(outName, "%s %d Ch %s %s%d/%s%d", theMixabilityString, (int)inDescription.mChannelsPerFrame, thePackingString, theKindString, (int)inDescription.mBitsPerChannel, theKindString, (int)((inDescription.mBytesPerFrame / inDescription.mChannelsPerFrame) * 8));
						}
						else
						{
							sprintf(outName, "%s %d Ch %s%d", theMixabilityString, (int)inDescription.mChannelsPerFrame, theKindString, (int)inDescription.mBitsPerChannel);
						}
					}
				}
				else
				{
					if(theEndianString != NULL)
					{
						if(thePackingString != NULL)
						{
							sprintf(outName, "%s %d Channel %d Bit %s %s Aligned %s in %d Bits", theMixabilityString, (int)inDescription.mChannelsPerFrame, (int)inDescription.mBitsPerChannel, theEndianString, theKindString, thePackingString, (int)(inDescription.mBytesPerFrame / inDescription.mChannelsPerFrame) * 8);
						}
						else
						{
							sprintf(outName, "%s %d Channel %d Bit %s %s", theMixabilityString, (int)inDescription.mChannelsPerFrame, (int)inDescription.mBitsPerChannel, theEndianString, theKindString);
						}
					}
					else
					{
						if(thePackingString != NULL)
						{
							sprintf(outName, "%s %d Channel %d Bit %s Aligned %s in %d Bits", theMixabilityString, (int)inDescription.mChannelsPerFrame, (int)inDescription.mBitsPerChannel, theKindString, thePackingString, (int)(inDescription.mBytesPerFrame / inDescription.mChannelsPerFrame) * 8);
						}
						else
						{
							sprintf(outName, "%s %d Channel %d Bit %s", theMixabilityString, (int)inDescription.mChannelsPerFrame, (int)inDescription.mBitsPerChannel, theKindString);
						}
					}
				}
			}
			break;

		case kAudioFormatAC3:
			strcpy(outName, "AC-3");
			break;

		case kAudioFormat60958AC3:
			strcpy(outName, "AC-3 for SPDIF");
			break;

		default:
			CACopy4CCToCString(outName, inDescription.mFormatID);
			break;
	};
}

#if CoreAudio_Debug
#include "CALogMacros.h"

void	CAStreamBasicDescription::PrintToLog(const AudioStreamBasicDescription& inDesc)
{
	PrintFloat		("  Sample Rate:        ", inDesc.mSampleRate);
	Print4CharCode	("  Format ID:          ", inDesc.mFormatID);
	PrintHex		("  Format Flags:       ", inDesc.mFormatFlags);
	PrintInt		("  Bytes per Packet:   ", inDesc.mBytesPerPacket);
	PrintInt		("  Frames per Packet:  ", inDesc.mFramesPerPacket);
	PrintInt		("  Bytes per Frame:    ", inDesc.mBytesPerFrame);
	PrintInt		("  Channels per Frame: ", inDesc.mChannelsPerFrame);
	PrintInt		("  Bits per Channel:   ", inDesc.mBitsPerChannel);
}
#endif

bool	operator<(const AudioStreamBasicDescription& x, const AudioStreamBasicDescription& y)
{
	bool theAnswer = false;
	bool isDone = false;

	//	note that if either side is 0, that field is skipped

	//	format ID is the first order sort
	if((!isDone) && ((x.mFormatID != 0) && (y.mFormatID != 0)))
	{
		if(x.mFormatID != y.mFormatID)
		{
			//	formats are sorted numerically except that linear
			//	PCM is always first
			if(x.mFormatID == kAudioFormatLinearPCM)
			{
				theAnswer = true;
			}
			else if(y.mFormatID == kAudioFormatLinearPCM)
			{
				theAnswer = false;
			}
			else
			{
				theAnswer = x.mFormatID < y.mFormatID;
			}
			isDone = true;
		}
	}


	//  mixable is always better than non-mixable for linear PCM and should be the second order sort item
	if((!isDone) && ((x.mFormatID == kAudioFormatLinearPCM) && (y.mFormatID == kAudioFormatLinearPCM)))
	{
		if(((x.mFormatFlags & kIsNonMixableFlag) == 0) && ((y.mFormatFlags & kIsNonMixableFlag) != 0))
		{
			theAnswer = true;
			isDone = true;
		}
		else if(((x.mFormatFlags & kIsNonMixableFlag) != 0) && ((y.mFormatFlags & kIsNonMixableFlag) == 0))
		{
			theAnswer = false;
			isDone = true;
		}
	}

	//	floating point vs integer for linear PCM only
	if((!isDone) && ((x.mFormatID == kAudioFormatLinearPCM) && (y.mFormatID == kAudioFormatLinearPCM)))
	{
		if((x.mFormatFlags & kAudioFormatFlagIsFloat) != (y.mFormatFlags & kAudioFormatFlagIsFloat))
		{
			//	floating point is better than integer
			theAnswer = y.mFormatFlags & kAudioFormatFlagIsFloat;
			isDone = true;
		}
	}

	//	bit depth
	if((!isDone) && ((x.mBitsPerChannel != 0) && (y.mBitsPerChannel != 0)))
	{
		if(x.mBitsPerChannel != y.mBitsPerChannel)
		{
			//	deeper bit depths are higher quality
			theAnswer = x.mBitsPerChannel < y.mBitsPerChannel;
			isDone = true;
		}
	}

	//	sample rate
	if((!isDone) && fnonzero(x.mSampleRate) && fnonzero(y.mSampleRate))
	{
		if(fnotequal(x.mSampleRate, y.mSampleRate))
		{
			//	higher sample rates are higher quality
			theAnswer = x.mSampleRate < y.mSampleRate;
			isDone = true;
		}
	}

	//	number of channels
	if((!isDone) && ((x.mChannelsPerFrame != 0) && (y.mChannelsPerFrame != 0)))
	{
		if(x.mChannelsPerFrame != y.mChannelsPerFrame)
		{
			//	more channels is higher quality
			theAnswer = x.mChannelsPerFrame < y.mChannelsPerFrame;
			isDone = true;
		}
	}

	return theAnswer;
}

static bool MatchFormatFlags(const AudioStreamBasicDescription& x, const AudioStreamBasicDescription& y)
{
	UInt32 xFlags = x.mFormatFlags;
	UInt32 yFlags = y.mFormatFlags;

	// match wildcards
	if (x.mFormatID == 0 || y.mFormatID == 0 || xFlags == 0 || yFlags == 0)
		return true;

	if (x.mFormatID == kAudioFormatLinearPCM)
	{
		// knock off the all clear flag
		xFlags = xFlags & ~kAudioFormatFlagsAreAllClear;
		yFlags = yFlags & ~kAudioFormatFlagsAreAllClear;

		// if both kAudioFormatFlagIsPacked bits are set, then we don't care about the kAudioFormatFlagIsAlignedHigh bit.
		if (xFlags & yFlags & kAudioFormatFlagIsPacked) {
			xFlags = xFlags & ~kAudioFormatFlagIsAlignedHigh;
			yFlags = yFlags & ~kAudioFormatFlagIsAlignedHigh;
		}

		// if both kAudioFormatFlagIsFloat bits are set, then we don't care about the kAudioFormatFlagIsSignedInteger bit.
		if (xFlags & yFlags & kAudioFormatFlagIsFloat) {
			xFlags = xFlags & ~kAudioFormatFlagIsSignedInteger;
			yFlags = yFlags & ~kAudioFormatFlagIsSignedInteger;
		}

		//	if the bit depth is 8 bits or less and the format is packed, we don't care about endianness
		if((x.mBitsPerChannel <= 8) && ((xFlags & kAudioFormatFlagIsPacked) == kAudioFormatFlagIsPacked))
		{
			xFlags = xFlags & ~kAudioFormatFlagIsBigEndian;
		}
		if((y.mBitsPerChannel <= 8) && ((yFlags & kAudioFormatFlagIsPacked) == kAudioFormatFlagIsPacked))
		{
			yFlags = yFlags & ~kAudioFormatFlagIsBigEndian;
		}

		//	if the number of channels is 0 or 1, we don't care about non-interleavedness
		if (x.mChannelsPerFrame <= 1 && y.mChannelsPerFrame <= 1) {
			xFlags &= ~kLinearPCMFormatFlagIsNonInterleaved;
			yFlags &= ~kLinearPCMFormatFlagIsNonInterleaved;
		}
	}
	return xFlags == yFlags;
}

bool	operator==(const AudioStreamBasicDescription& x, const AudioStreamBasicDescription& y)
{
	//	the semantics for equality are:
	//		1) Values must match exactly
	//		2) wildcard's are ignored in the comparison

#define MATCH(name) ((x.name) == 0 || (y.name) == 0 || (x.name) == (y.name))

	return
			//	check the sample rate
		(fiszero(x.mSampleRate) || fiszero(y.mSampleRate) || fequal(x.mSampleRate, y.mSampleRate))

			//	check the format ids
		&& MATCH(mFormatID)

			//	check the format flags
		&& MatchFormatFlags(x, y)

			//	check the bytes per packet
		&& MATCH(mBytesPerPacket)

			//	check the frames per packet
		&& MATCH(mFramesPerPacket)

			//	check the bytes per frame
		&& MATCH(mBytesPerFrame)

			//	check the channels per frame
		&& MATCH(mChannelsPerFrame)

			//	check the channels per frame
		&& MATCH(mBitsPerChannel) ;
}

bool	CAStreamBasicDescription::IsEqual(const AudioStreamBasicDescription &other, bool interpretingWildcards) const
{
	if (interpretingWildcards)
		return *this == other;
	return memcmp(this, &other, offsetof(AudioStreamBasicDescription, mReserved)) == 0;
}

bool SanityCheck(const AudioStreamBasicDescription& x)
{
	return (x.mSampleRate >= 0.);
}
