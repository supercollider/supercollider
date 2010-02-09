/*
	File:		HID_Name_Lookup.c

	Contains:	Implementation of the HID device name lookup functions for the HID utilites.

	DRI: George Warner

	Copyright:	Copyright © 2002 Apple Computer, Inc., All Rights Reserved

	Disclaimer:	IMPORTANT:  This Apple software is supplied to you by Apple Computer, Inc.
				("Apple") in consideration of your agreement to the following terms, and your
				use, installation, modification or redistribution of this Apple software
				constitutes acceptance of these terms.  If you do not agree with these terms,
				please do not use, install, modify or redistribute this Apple software.

				In consideration of your agreement to abide by the following terms, and subject
				to these terms, Apple grants you a personal, non-exclusive license, under Apple’s
				copyrights in this original Apple software (the "Apple Software"), to use,
				reproduce, modify and redistribute the Apple Software, with or without
				modifications, in source and/or binary forms; provided that if you redistribute
				the Apple Software in its entirety and without modifications, you must retain
				this notice and the following text and disclaimers in all such redistributions of
				the Apple Software.  Neither the name, trademarks, service marks or logos of
				Apple Computer, Inc. may be used to endorse or promote products derived from the
				Apple Software without specific prior written permission from Apple.  Except as
				expressly stated in this notice, no other rights or licenses, express or implied,
				are granted by Apple herein, including but not limited to any patent rights that
				may be infringed by your derivative works or by other works in which the Apple
				Software may be incorporated.

				The Apple Software is provided by Apple on an "AS IS" basis.  APPLE MAKES NO
				WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE IMPLIED
				WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A PARTICULAR
				PURPOSE, REGARDING THE APPLE SOFTWARE OR ITS USE AND OPERATION ALONE OR IN
				COMBINATION WITH YOUR PRODUCTS.

				IN NO EVENT SHALL APPLE BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL OR
				CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
				GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
				ARISING IN ANY WAY OUT OF THE USE, REPRODUCTION, MODIFICATION AND/OR DISTRIBUTION
				OF THE APPLE SOFTWARE, HOWEVER CAUSED AND WHETHER UNDER THEORY OF CONTRACT, TORT
				(INCLUDING NEGLIGENCE), STRICT LIABILITY OR OTHERWISE, EVEN IF APPLE HAS BEEN
				ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#ifdef SC_DARWIN
#include "HID_Utilities_Internal.h"
#include "HID_Name_Lookup.h"

#define FAKE_IT	1	// set true for debugging; returns the vendor, product & cookie (or usage info) as numbers.

// ---------------------------------
// Load the element strings from the given resource (XML) file into a CFPropertyListRef
static CFPropertyListRef xml_load(const CFStringRef pResourceName,const CFStringRef pResourceExtension)
{
	CFPropertyListRef tCFPropertyListRef = NULL;
	CFURLRef resFileCFURLRef = CFBundleCopyResourceURL(CFBundleGetMainBundle(), pResourceName, pResourceExtension, NULL);

	if (NULL != resFileCFURLRef)
	{
		CFDataRef resCFDataRef;

		if (CFURLCreateDataAndPropertiesFromResource(kCFAllocatorDefault, resFileCFURLRef, &resCFDataRef, nil, nil, nil))
		{
			if (NULL != resCFDataRef)
			{
				CFStringRef errorString;

				tCFPropertyListRef = CFPropertyListCreateFromXMLData(kCFAllocatorDefault, resCFDataRef, kCFPropertyListImmutable, &errorString);
				if (NULL == tCFPropertyListRef)
					CFShow(errorString);
				CFRelease(resCFDataRef);
			}
		}
		CFRelease(resFileCFURLRef);
	}
	return tCFPropertyListRef;
}

// ---------------------------------
// Find an element string in the <HID_cookie_strings.plist> resource (XML) file
static Boolean xml_search_cookie(const long pVendorID, const long pProductID, const long pCookie, char* pCstr)
{
	static CFPropertyListRef tCFPropertyListRef = NULL;
	Boolean results = false;

	if (NULL == tCFPropertyListRef)
		tCFPropertyListRef = xml_load(CFSTR("HID_cookie_strings"), CFSTR("plist"));
	if (NULL != tCFPropertyListRef)
	{
		if (CFDictionaryGetTypeID() == CFGetTypeID(tCFPropertyListRef))
		{
			CFDictionaryRef vendorCFDictionaryRef;
			CFStringRef	vendorKeyCFStringRef;
			vendorKeyCFStringRef = CFStringCreateWithFormat(kCFAllocatorDefault, NULL, CFSTR("%ld"), pVendorID);

			if (CFDictionaryGetValueIfPresent(tCFPropertyListRef, vendorKeyCFStringRef, (const void**) &vendorCFDictionaryRef))
			{
				CFDictionaryRef productCFDictionaryRef;
				CFStringRef	productKeyCFStringRef;
				CFStringRef	vendorCFStringRef;

				if (CFDictionaryGetValueIfPresent(vendorCFDictionaryRef, CFSTR("Name"), (const void**) &vendorCFStringRef))
				{
					//CFShow(vendorCFStringRef);
				}
				productKeyCFStringRef = CFStringCreateWithFormat(kCFAllocatorDefault, NULL, CFSTR("%ld"), pProductID);

				if (CFDictionaryGetValueIfPresent(vendorCFDictionaryRef, productKeyCFStringRef, (const void**) &productCFDictionaryRef))
				{
					CFStringRef fullCFStringRef = NULL;
					CFStringRef	cookieKeyCFStringRef;
					CFStringRef	productCFStringRef;
					CFStringRef	cookieCFStringRef;

					if (CFDictionaryGetValueIfPresent(productCFDictionaryRef, CFSTR("Name"), (const void**) &productCFStringRef))
					{
						//CFShow(productCFStringRef);
					}
					cookieKeyCFStringRef = CFStringCreateWithFormat(kCFAllocatorDefault, NULL, CFSTR("%ld"), pCookie);

					if (CFDictionaryGetValueIfPresent(productCFDictionaryRef, cookieKeyCFStringRef, (const void**) &cookieCFStringRef))
					{
						fullCFStringRef = CFStringCreateWithFormat(kCFAllocatorDefault, NULL, CFSTR("%@ %@ %@"),
												 vendorCFStringRef, productCFStringRef, cookieCFStringRef);
						// CFShow(cookieCFStringRef);
					}
#if FAKE_IT
					else
					{
						fullCFStringRef = CFStringCreateWithFormat(kCFAllocatorDefault, NULL, CFSTR("%@ %@ #%@"),
												 vendorCFStringRef, productCFStringRef, cookieKeyCFStringRef);
					}
#endif
					if (fullCFStringRef)
					{
						// CFShow(fullCFStringRef);
						results = CFStringGetCString(
								   fullCFStringRef, pCstr, CFStringGetLength(fullCFStringRef) * sizeof(UniChar) + 1, kCFStringEncodingMacRoman);
						CFRelease(fullCFStringRef);
					}
					CFRelease(cookieKeyCFStringRef);
				}
				CFRelease(productKeyCFStringRef);
			}
			CFRelease(vendorKeyCFStringRef);
		}
		//++CFRelease(tCFPropertyListRef);	// Leak this!
	}
	return results;
}

// ---------------------------------
// Find an element string in the <HID_device_usage_strings.plist> resource (XML) file
static Boolean xml_search_usage(const long pVendorID, const long pProductID, const long pUsagePage, const long pUsage, char* pCstr)
{
	static CFPropertyListRef tCFPropertyListRef = NULL;
	Boolean results = false;

	if (NULL == tCFPropertyListRef)
		tCFPropertyListRef = xml_load(CFSTR("HID_device_usage_strings"), CFSTR("plist"));
	if (NULL != tCFPropertyListRef)
	{
		if (CFDictionaryGetTypeID() == CFGetTypeID(tCFPropertyListRef))
		{
			CFDictionaryRef vendorCFDictionaryRef;
			CFStringRef	vendorKeyCFStringRef;
			vendorKeyCFStringRef = CFStringCreateWithFormat(kCFAllocatorDefault, NULL, CFSTR("%ld"), pVendorID);

			if (CFDictionaryGetValueIfPresent(tCFPropertyListRef, vendorKeyCFStringRef, (const void**) &vendorCFDictionaryRef))
			{
				CFDictionaryRef productCFDictionaryRef;
				CFStringRef	productKeyCFStringRef;
				CFStringRef	vendorCFStringRef;

				if (!CFDictionaryGetValueIfPresent(vendorCFDictionaryRef, CFSTR("Name"), (const void**) &vendorCFStringRef))
				{
					vendorCFStringRef = CFStringCreateWithFormat(kCFAllocatorDefault, NULL, CFSTR("v: %ld"), pVendorID);
					//CFShow(vendorCFStringRef);
				}
				productKeyCFStringRef = CFStringCreateWithFormat(kCFAllocatorDefault, NULL, CFSTR("%ld"), pProductID);

				if (CFDictionaryGetValueIfPresent(vendorCFDictionaryRef, productKeyCFStringRef, (const void**) &productCFDictionaryRef))
				{
					CFStringRef fullCFStringRef = NULL;
					CFStringRef	usageKeyCFStringRef;
					CFStringRef	productCFStringRef;
					CFStringRef	usageCFStringRef;

					if (CFDictionaryGetValueIfPresent(productCFDictionaryRef, CFSTR("Name"), (const void**) &productCFStringRef))
					{
						//CFShow(productCFStringRef);
					}
					usageKeyCFStringRef = CFStringCreateWithFormat(kCFAllocatorDefault, NULL, CFSTR("%ld:%ld"), pUsagePage, pUsage);

					if (CFDictionaryGetValueIfPresent(productCFDictionaryRef, usageKeyCFStringRef, (const void**) &usageCFStringRef))
					{
						fullCFStringRef = CFStringCreateWithFormat(kCFAllocatorDefault, NULL, CFSTR("%@ %@ %@"),
												 vendorCFStringRef, productCFStringRef, usageCFStringRef);
						// CFShow(usageCFStringRef);
					}
#if FAKE_IT
					else
					{
						fullCFStringRef = CFStringCreateWithFormat(kCFAllocatorDefault, NULL, CFSTR("%@ %@ #%@"),
												 vendorCFStringRef, productCFStringRef, usageKeyCFStringRef);
					}
#endif
					if (fullCFStringRef)
					{
						// CFShow(fullCFStringRef);
						results = CFStringGetCString(
								   fullCFStringRef, pCstr, CFStringGetLength(fullCFStringRef) * sizeof(UniChar) + 1, kCFStringEncodingMacRoman);
						CFRelease(fullCFStringRef);
					}
					CFRelease(usageKeyCFStringRef);
				}
				CFRelease(productKeyCFStringRef);
			}
			CFRelease(vendorKeyCFStringRef);
		}
		//++CFRelease(tCFPropertyListRef);	// Leak this!
	}
	return results;
}

// ---------------------------------
// set name from vendor id/product id look up
Boolean HIDGetElementNameFromVendorProductCookie (const long pVendorID, const long pProductID, const long pCookie, char * pName)
{
	Boolean result = false;
	*pName = 0; // clear name

	if (xml_search_cookie(pVendorID, pProductID, pCookie, pName))
		return true;

	switch (pVendorID) {
		case kMacally:
			switch (pProductID) {
				case kiShock:
					result = true;
					switch (pCookie) {
						case 3: sprintf(pName, "D-Pad Up"); break;
						case 4: sprintf(pName, "D-Pad Down"); break;
						case 5: sprintf(pName, "D-Pad Left"); break;
						case 6: sprintf(pName, "D-Pad Right"); break;
						case 7: sprintf(pName, "Up Button"); break;
						case 8: sprintf(pName, "Right Button"); break;
						case 9: sprintf(pName, "Down Button"); break;
						case 10: sprintf(pName, "Left Button"); break;
						case 11: sprintf(pName, "C Button"); break;
						case 12: sprintf(pName, "B Button [Select]"); break;
						case 13: sprintf(pName, "A Button [Start]"); break;
						case 14: sprintf(pName, "F Button"); break;
						case 15: sprintf(pName, "R1 Trigger"); break;
						case 16: sprintf(pName, "R2 Trigger"); break;
						case 17: sprintf(pName, "L1 Trigger"); break;
						case 18: sprintf(pName, "L2 Trigger"); break;
						case 19: sprintf(pName, "Left Stick Button"); break;
						case 20: sprintf(pName, "Right Stick Button"); break;
						case 21: sprintf(pName, "D Button"); break;
						case 22: sprintf(pName, "E Button"); break;
						case 23: sprintf(pName, "Left Stick X-Axis"); break;
						case 24: sprintf(pName, "Left Stick Y-Axis"); break;
						case 25: sprintf(pName, "Right Stick X-Axis"); break;
						case 26: sprintf(pName, "Right Stick Y-Axis"); break;
						default:
#if FAKE_IT
							sprintf(pName, "#{V:Macally, P:iShock, C:%ld}#", pCookie);
#else
							result = false;
#endif FAKE_IT
							break;
					}
					break;
				default:
#if FAKE_IT
					sprintf(pName, "#{V:Macally, P:%ld, C:%ld}#", pProductID, pCookie); break;
#else
					result = false;
#endif FAKE_IT
					break;
			}
			break;
		case kMacsense:
			switch (pProductID) {
				case kFunPadF107:
					result = true;
					switch (pCookie) {
						case  3: sprintf(pName, "Button 1"); break;
						case  4: sprintf(pName, "Button 2"); break;
						case  5: sprintf(pName, "Button 3"); break;
						case  6: sprintf(pName, "Button 4"); break;
						case  7: sprintf(pName, "L1 Trigger"); break;
						case  8: sprintf(pName, "R1 Trigger"); break;
						case  9: sprintf(pName, "L2 Trigger"); break;
						case 10: sprintf(pName, "R2 Trigger"); break;
						case 11: sprintf(pName, "Right Stick X-Axis"); break;
						case 12: sprintf(pName, "Right Stick Y-Axis"); break;
						case 13: sprintf(pName, "Left Stick X-Axis"); break;
						case 14: sprintf(pName, "Left Stick Y-Axis"); break;
						case 15: sprintf(pName, "Hat Switch"); break;
						default:
#if FAKE_IT
							sprintf(pName, "#{V:Macsense, P:FunPad F-107, C:%ld}#", pCookie);
#else
							result = false;
#endif FAKE_IT
							break;
					}
				default:
#if FAKE_IT
					sprintf(pName, "#{V:Macsense, P:%ld, C:%ld}#", pProductID, pCookie);
#else
					result = false;
#endif FAKE_IT
					break;
			}
			break;
		default:
#if FAKE_IT
			sprintf(pName, "#{V:%ld, P:%ld, C:%ld}#", pVendorID, pProductID, pCookie);
#else
			result = false;
#endif FAKE_IT
			break;
	}
	return result;
}


// ---------------------------------
// set name from vendor id/product id & usage look up
Boolean HIDGetElementNameFromVendorProductUsage (const long pVendorID, const long pProductID, const long pUsagePage, const long pUsage, char * pName)
{
	Boolean result = false;
	*pName = 0; // clear name

	if (xml_search_usage(pVendorID, pProductID, pUsagePage, pUsage, pName))
		return true;

#if FAKE_IT
	sprintf(pName, "#{V:%ld, P:%ld, U:%ld:%ld}#", pVendorID, pProductID, pUsagePage, pUsage);
	result = true;
#endif

	return result;
}
#endif //SC_DARWIN