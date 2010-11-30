//
// File:       HID_Name_Lookup.h
//
// Disclaimer: IMPORTANT:  This Apple software is supplied to you by Apple Inc. ("Apple")
//             in consideration of your agreement to the following terms, and your use,
//             installation, modification or redistribution of this Apple software
//             constitutes acceptance of these terms.  If you do not agree with these
//             terms, please do not use, install, modify or redistribute this Apple
//             software.
//
//             In consideration of your agreement to abide by the following terms, and
//             subject to these terms, Apple grants you a personal, non - exclusive
//             license, under Apple's copyrights in this original Apple software ( the
//             "Apple Software" ), to use, reproduce, modify and redistribute the Apple
//             Software, with or without modifications, in source and / or binary forms;
//             provided that if you redistribute the Apple Software in its entirety and
//             without modifications, you must retain this notice and the following text
//             and disclaimers in all such redistributions of the Apple Software. Neither
//             the name, trademarks, service marks or logos of Apple Inc. may be used to
//             endorse or promote products derived from the Apple Software without specific
//             prior written permission from Apple.  Except as expressly stated in this
//             notice, no other rights or licenses, express or implied, are granted by
//             Apple herein, including but not limited to any patent rights that may be
//             infringed by your derivative works or by other works in which the Apple
//             Software may be incorporated.
//
//             The Apple Software is provided by Apple on an "AS IS" basis.  APPLE MAKES NO
//             WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE IMPLIED
//             WARRANTIES OF NON - INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A
//             PARTICULAR PURPOSE, REGARDING THE APPLE SOFTWARE OR ITS USE AND OPERATION
//             ALONE OR IN COMBINATION WITH YOUR PRODUCTS.
//
//             IN NO EVENT SHALL APPLE BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL OR
//             CONSEQUENTIAL DAMAGES ( INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
//             SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
//             INTERRUPTION ) ARISING IN ANY WAY OUT OF THE USE, REPRODUCTION, MODIFICATION
//             AND / OR DISTRIBUTION OF THE APPLE SOFTWARE, HOWEVER CAUSED AND WHETHER
//             UNDER THEORY OF CONTRACT, TORT ( INCLUDING NEGLIGENCE ), STRICT LIABILITY OR
//             OTHERWISE, EVEN IF APPLE HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Copyright ( C ) 2002-2008 Apple Inc. All Rights Reserved.
//

#ifndef _HID_Name_Lookup_h_
#define _HID_Name_Lookup_h_

/*****************************************************/
#pragma mark - includes & imports

#include "HID_Utilities_External.h"

/*****************************************************/
#if PRAGMA_ONCE
#pragma once
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if PRAGMA_IMPORT
#pragma import on
#endif

#if PRAGMA_STRUCT_ALIGN
#pragma options align = mac68k
#elif PRAGMA_STRUCT_PACKPUSH
#pragma pack( push, 2 )
#elif PRAGMA_STRUCT_PACK
#pragma pack( 2 )
#endif

/*****************************************************/
#pragma mark - typedef's, struct's, enums, defines, etc.
// Notes: Now that as the device data has now been moved into external .XML files these
//	constansts should no longer be used. They will eventually be eliminated from this header.

#define kMicrosoft 				1118
#define kSideWinderFFB2 		27

#define kLogitech 				1133
#define kWingManStrikeForce3D 	49797

#define kMacally 				8738
#define kiShock 				16400
#define kiShockIIFFB 			16416

#define kMacsense 				1635
#define kFunPadF107				38916

/*****************************************************/
#pragma mark - exported globals

/*****************************************************/
#pragma mark - exported function prototypes

// get vendor name from vendor ID
extern Boolean HIDGetVendorNameFromVendorID( long inVendorID, char * outCStrName );

// get product name from vendor/product ID
extern Boolean HIDGetProductNameFromVendorProductID( long inVendorID, long inProductID, char * outCStrName );

// get element name from vendor id/product id look up ( using cookies )
extern Boolean HIDGetElementNameFromVendorProductCookie (int inVendorID, int inProductID, IOHIDElementCookie inCookie, char * outCStrName);

// get element name from vendor id/product id look up ( using usage page & usage )
extern Boolean HIDGetElementNameFromVendorProductUsage( long inVendorID, long inProductID, long inUsagePage, long inUsage, char * inCStrName );

// Add device to usage & cookie db's
extern Boolean HIDAddDeviceToXML( IOHIDDeviceRef inIOHIDDeviceRef );

// Add element to usage & cookie db's
extern Boolean HIDAddDeviceElementToXML( IOHIDDeviceRef inIOHIDDeviceRef, IOHIDElementRef inIOHIDElementRef );

// returns C string type name given a type enumeration passed in as parameter( see IOHIDKeys.h )
// returns empty string for invalid types
extern void HIDGetTypeName( IOHIDElementType inIOHIDElementType, char * outCStrName );

// return a CFStringRef string for a given usage page & usage( see IOUSBHIDParser.h )
extern CFStringRef HIDCopyUsageName( long inUsagePage, long inUsage );

/*****************************************************/
#if PRAGMA_STRUCT_ALIGN
#pragma options align = reset
#elif PRAGMA_STRUCT_PACKPUSH
#pragma pack( pop )
#elif PRAGMA_STRUCT_PACK
#pragma pack( )
#endif

#ifdef PRAGMA_IMPORT_OFF
#pragma import off
#elif PRAGMA_IMPORT
#pragma import reset
#endif

#ifdef __cplusplus
}
#endif
/*****************************************************/
#endif // _HID_Name_Lookup_h_
