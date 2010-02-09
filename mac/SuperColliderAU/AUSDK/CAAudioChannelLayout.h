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
	CAAudioChannelLayout.h

=============================================================================*/
#if !defined(__CAAudioChannelLayout_h__)
#define __CAAudioChannelLayout_h__

//=============================================================================
//	Includes
//=============================================================================

//	System Includes
#if !defined(__COREAUDIO_USE_FLAT_INCLUDES__)
	#include <CoreAudio/CoreAudioTypes.h>
	#include <CoreFoundation/CoreFoundation.h>
#else
	#include <CoreAudioTypes.h>
	#include <CoreFoundation.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if !HAL_Build
	#include "CAReferenceCounted.h"
#endif

//=============================================================================
//	CAAudioChannelLayout
//=============================================================================

bool	operator== (const AudioChannelLayout &x, const AudioChannelLayout &y);

extern "C" void 	CAShowAudioChannelLayout (FILE* file, const AudioChannelLayout *layout);

class CAAudioChannelLayout
{
//	static Construction/Destruction
public:
	static AudioChannelLayout*	Create(UInt32 inNumberChannelDescriptions);
	static void					Destroy(AudioChannelLayout* inChannelLayout);
	static UInt32				CalculateByteSize(UInt32 inNumberChannelDescriptions) {
									return offsetof(AudioChannelLayout, mChannelDescriptions) + inNumberChannelDescriptions * sizeof(AudioChannelDescription);
								}
	static void					SetAllToUnknown(AudioChannelLayout& outChannelLayout, UInt32 inNumberChannelDescriptions);
	static UInt32				NumberChannels(const AudioChannelLayout& inLayout);

#if !HAL_Build
// object methods
public:
								CAAudioChannelLayout ();

								CAAudioChannelLayout (UInt32 inNumberChannels, bool inChooseSurround);
									// if inChooseSurround is false, then symmetrical speaker arrangements
									// are chosen in place of surround layouts if there is a choice
									// This call chooses layouts based on the expected defaults in
									// AudioUnit usage
								CAAudioChannelLayout (AudioChannelLayoutTag inTag);
								CAAudioChannelLayout (const CAAudioChannelLayout &c);
								CAAudioChannelLayout (const AudioChannelLayout* inChannelLayout);
								~CAAudioChannelLayout();

	CAAudioChannelLayout&		operator= (const AudioChannelLayout* inChannelLayout);
	CAAudioChannelLayout&		operator= (const CAAudioChannelLayout& c);
	bool						operator== (const CAAudioChannelLayout &c) const;

	void						SetWithTag(AudioChannelLayoutTag inTag);

	bool						IsValid() const { return NumberChannels() > 0; }
	UInt32						Size() const { return mLayoutHolder ? mLayoutHolder->Size() : 0; }

	UInt32						NumberChannels() const { return mLayoutHolder->NumberChannels(); }

	AudioChannelLayoutTag		Tag() const { return Layout().mChannelLayoutTag; }
	const AudioChannelLayout&	Layout() const { return mLayoutHolder->Layout(); }
	operator const AudioChannelLayout *() const { return &Layout(); }

	void						Print () const { Print (stdout); }
	void						Print (FILE* file) const;

	OSStatus					Save (CFPropertyListRef *outData) const;
	OSStatus					Restore (CFPropertyListRef &inData);

private:
	class ACLRefCounter : public CAReferenceCounted {
	public:
				ACLRefCounter (UInt32 inDataSize)
				{
					if (inDataSize < offsetof(AudioChannelLayout, mChannelDescriptions))
						inDataSize = offsetof(AudioChannelLayout, mChannelDescriptions);

					mLayout = static_cast<AudioChannelLayout*>(malloc (inDataSize));
					memset (mLayout, 0, inDataSize);
					mByteSize = inDataSize;
				}

		const AudioChannelLayout & 	Layout() const { return *mLayout; }

		UInt32						Size () const { return mByteSize; }

		UInt32						NumberChannels() { return mLayout ? CAAudioChannelLayout::NumberChannels(Layout()) : 0; }

	private:
		AudioChannelLayout 	*mLayout;
		UInt32				mByteSize;

			// only the constructors can change the actual state of the layout
		friend CAAudioChannelLayout::CAAudioChannelLayout (UInt32 inNumberChannels, bool inChooseSurround);
		friend OSStatus CAAudioChannelLayout::Restore (CFPropertyListRef &inData);
		friend CAAudioChannelLayout& CAAudioChannelLayout::operator= (const AudioChannelLayout* inChannelLayout);
		friend void CAAudioChannelLayout::SetWithTag(AudioChannelLayoutTag inTag);

		AudioChannelLayout * 	GetLayout() { return mLayout; }
		~ACLRefCounter() { if (mLayout) { free(mLayout); mLayout = NULL; } }

	private:
		ACLRefCounter () : mLayout(NULL) { }
		ACLRefCounter(const ACLRefCounter& c) : mLayout(NULL) { }
		ACLRefCounter& operator=(const ACLRefCounter& c) { return *this; }
	};

	ACLRefCounter				*mLayoutHolder;
#endif	//	HAL_Build

};

#endif
