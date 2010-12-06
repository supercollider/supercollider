//
//	File:		IOHIDDevice_.h of HID Utilities
//
//	Created:	9/21/06
//
//	Contains:	Definition of the interfaces to <IOHIDDevice_.c>
//
//	Copyright © 2007-2009 Apple Inc., All Rights Reserved
//
//	Disclaimer:	IMPORTANT:  This Apple software is supplied to you by
//				Apple Inc. ("Apple") in consideration of your agreement to the
//				following terms, and your use, installation, modification or
//				redistribution of this Apple software constitutes acceptance of these
//				terms.  If you do not agree with these terms, please do not use,
//				install, modify or redistribute this Apple software.
//
//				In consideration of your agreement to abide by the following terms, and
//				subject to these terms, Apple grants you a personal, non-exclusive
//				license, under Apple's copyrights in this original Apple software (the
//				"Apple Software"), to use, reproduce, modify and redistribute the Apple
//				Software, with or without modifications, in source and/or binary forms;
//				provided that if you redistribute the Apple Software in its entirety and
//				without modifications, you must retain this notice and the following
//				text and disclaimers in all such redistributions of the Apple Software.
//				Neither the name, trademarks, service marks or logos of Apple Inc.
//				may be used to endorse or promote products derived from the Apple
//				Software without specific prior written permission from Apple.  Except
//				as expressly stated in this notice, no other rights or licenses, express
//				or implied, are granted by Apple herein, including but not limited to
//				any patent rights that may be infringed by your derivative works or by
//				other works in which the Apple Software may be incorporated.
//
//				The Apple Software is provided by Apple on an "AS IS" basis.  APPLE
//				MAKES NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION
//				THE IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS
//				FOR A PARTICULAR PURPOSE, REGARDING THE APPLE SOFTWARE OR ITS USE AND
//				OPERATION ALONE OR IN COMBINATION WITH YOUR PRODUCTS.
//
//				IN NO EVENT SHALL APPLE BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL
//				OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
//				SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
//				INTERRUPTION) ARISING IN ANY WAY OUT OF THE USE, REPRODUCTION,
//				MODIFICATION AND/OR DISTRIBUTION OF THE APPLE SOFTWARE, HOWEVER CAUSED
//				AND WHETHER UNDER THEORY OF CONTRACT, TORT (INCLUDING NEGLIGENCE),
//				STRICT LIABILITY OR OTHERWISE, EVEN IF APPLE HAS BEEN ADVISED OF THE
//				POSSIBILITY OF SUCH DAMAGE.
//
#ifndef __IOHIDDevice__
#define __IOHIDDevice__

//*****************************************************
#pragma mark - includes & imports

#include <AvailabilityMacros.h>

#include <Carbon/Carbon.h>

#include "IOHIDLib_.h"

//*****************************************************
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
#pragma options align=mac68k
#elif PRAGMA_STRUCT_PACKPUSH
#pragma pack(push, 2)
#elif PRAGMA_STRUCT_PACK
#pragma pack(2)
#endif

//*****************************************************
#pragma mark - typedef's, struct's, enums, defines, etc.
//-----------------------------------------------------

//*****************************************************
#pragma mark - exported globals
//-----------------------------------------------------

//*****************************************************
#pragma mark - exported function prototypes
//-----------------------------------------------------
	
//*************************************************************************
//
// HIDIsValidDevice( inIOHIDDeviceRef )
//
// Purpose:	validate this device
//
// Inputs:  inIOHIDDeviceRef - the IDHIDDeviceRef for this device
//
// Returns:	Boolean			- TRUE if we find the device in our( internal ) device list
//

extern Boolean HIDIsValidDevice( IOHIDDeviceRef inIOHIDDeviceRef );
	
//*************************************************************************
//
// IOHIDDevice_GetTransport( inIOHIDDeviceRef )
//
// Purpose:	get the Transport CFString for this device
//
// Inputs:  inIOHIDDeviceRef - the IDHIDDeviceRef for this device
//
// Returns:	CFStringRef - the Transport CFString for this device
//

extern CFStringRef IOHIDDevice_GetTransport( IOHIDDeviceRef inIOHIDDeviceRef );

//*************************************************************************
//
// IOHIDDevice_GetVendorID( inIOHIDDeviceRef )
//
// Purpose:	get the vendor ID for this device
//
// Inputs:  inIOHIDDeviceRef - the IDHIDDeviceRef for this device
//
// Returns:	long - the vendor ID for this device
//

extern long IOHIDDevice_GetVendorID( IOHIDDeviceRef inIOHIDDeviceRef );

//*************************************************************************
//
// IOHIDDevice_GetVendorIDSource( inIOHIDDeviceRef )
//
// Purpose:	get the VendorIDSource for this device
//
// Inputs:  inIOHIDDeviceRef - the IDHIDDeviceRef for this device
//
// Returns:	long - the VendorIDSource for this device
//

extern long IOHIDDevice_GetVendorIDSource( IOHIDDeviceRef inIOHIDDeviceRef );

//*************************************************************************
//
// IOHIDDevice_GetProductID( inIOHIDDeviceRef )
//
// Purpose:	get the product ID for this device
//
// Inputs:  inIOHIDDeviceRef - the IDHIDDeviceRef for this device
//
// Returns:	long - the product ID for this device
//

extern long IOHIDDevice_GetProductID( IOHIDDeviceRef inIOHIDDeviceRef );

//*************************************************************************
//
// IOHIDDevice_GetVersionNumber( inIOHIDDeviceRef )
//
// Purpose:	get the VersionNumber CFString for this device
//
// Inputs:  inIOHIDDeviceRef - the IDHIDDeviceRef for this device
//
// Returns:	long - the VersionNumber for this device
//

extern long IOHIDDevice_GetVersionNumber( IOHIDDeviceRef inIOHIDDeviceRef );

//*************************************************************************
//
// IOHIDDevice_GetManufacturer( inIOHIDDeviceRef )
//
// Purpose:	get the Manufacturer CFString for this device
//
// Inputs:  inIOHIDDeviceRef - the IDHIDDeviceRef for this device
//
// Returns:	CFStringRef - the Manufacturer CFString for this device
//

extern CFStringRef IOHIDDevice_GetManufacturer( IOHIDDeviceRef inIOHIDDeviceRef );

//*************************************************************************
//
// IOHIDDevice_GetProduct( inIOHIDDeviceRef )
//
// Purpose:	get the Product CFString for this device
//
// Inputs:  inIOHIDDeviceRef - the IDHIDDeviceRef for this device
//
// Returns:	CFStringRef - the Product CFString for this device
//

extern CFStringRef IOHIDDevice_GetProduct( IOHIDDeviceRef inIOHIDDeviceRef );

//*************************************************************************
//
// IOHIDDevice_GetSerialNumber( inIOHIDDeviceRef )
//
// Purpose:	get the SerialNumber CFString for this device
//
// Inputs:  inIOHIDDeviceRef - the IDHIDDeviceRef for this device
//
// Returns:	CFStringRef - the SerialNumber CFString for this device
//

extern CFStringRef IOHIDDevice_GetSerialNumber( IOHIDDeviceRef inIOHIDDeviceRef );

//*************************************************************************
//
// IOHIDDevice_GetCountryCode( inIOHIDDeviceRef )
//
// Purpose:	get the CountryCode CFString for this device
//
// Inputs:  inIOHIDDeviceRef - the IDHIDDeviceRef for this device
//
// Returns:	long - the CountryCode for this device
//

extern long IOHIDDevice_GetCountryCode( IOHIDDeviceRef inIOHIDDeviceRef );

//*************************************************************************
//
// IOHIDDevice_GetLocationID( inIOHIDDeviceRef )
//
// Purpose:	get the location ID for this device
//
// Inputs:  inIOHIDDeviceRef - the IDHIDDeviceRef for this device
//
// Returns:	long - the location ID for this device
//

extern long IOHIDDevice_GetLocationID( IOHIDDeviceRef inIOHIDDeviceRef );

//*************************************************************************
//
// IOHIDDevice_GetUsage( inIOHIDDeviceRef )
//
// Purpose:	get the usage for this device
//
// Inputs:  inIOHIDDeviceRef - the IDHIDDeviceRef for this device
//
// Returns:	uint32_t - the usage for this device
//

extern uint32_t IOHIDDevice_GetUsage( IOHIDDeviceRef inIOHIDDeviceRef );

//*************************************************************************
//
// IOHIDDevice_GetUsagePage( inIOHIDDeviceRef )
//
// Purpose:	get the usage page for this device
//
// Inputs:  inIOHIDDeviceRef - the IDHIDDeviceRef for this device
//
// Returns:	uint32_t - the usage page for this device
//

extern uint32_t IOHIDDevice_GetUsagePage( IOHIDDeviceRef inIOHIDDeviceRef );

//*************************************************************************
//
// IOHIDDevice_GetUsagePairs( inIOHIDDeviceRef )
//
// Purpose:	get the UsagePairs CFString for this device
//
// Inputs:  inIOHIDDeviceRef - the IDHIDDeviceRef for this device
//
// Returns:	CFArrayRef - the UsagePairs for this device
//

extern CFArrayRef IOHIDDevice_GetUsagePairs( IOHIDDeviceRef inIOHIDDeviceRef );

//*************************************************************************
//
// IOHIDDevice_GetPrimaryUsage( inIOHIDDeviceRef )
//
// Purpose:	get the PrimaryUsage CFString for this device
//
// Inputs:  inIOHIDDeviceRef - the IDHIDDeviceRef for this device
//
// Returns:	CFStringRef - the PrimaryUsage CFString for this device
//

extern uint32_t IOHIDDevice_GetPrimaryUsage( IOHIDDeviceRef inIOHIDDeviceRef );

//*************************************************************************
//
// IOHIDDevice_GetPrimaryUsagePage( inIOHIDDeviceRef )
//
// Purpose:	get the PrimaryUsagePage CFString for this device
//
// Inputs:  inIOHIDDeviceRef - the IDHIDDeviceRef for this device
//
// Returns:	CFStringRef - the PrimaryUsagePage CFString for this device
//

extern uint32_t IOHIDDevice_GetPrimaryUsagePage( IOHIDDeviceRef inIOHIDDeviceRef );

//*************************************************************************
//
// IOHIDDevice_GetMaxInputReportSize( inIOHIDDeviceRef )
//
// Purpose:	get the MaxInputReportSize for this device
//
// Inputs:  inIOHIDDeviceRef - the IDHIDDeviceRef for this device
//
// Returns:	long - the MaxInputReportSize for this device
//

extern long IOHIDDevice_GetMaxInputReportSize( IOHIDDeviceRef inIOHIDDeviceRef );

//*************************************************************************
//
// IOHIDDevice_GetMaxOutputReportSize( inIOHIDDeviceRef )
//
// Purpose:	get the MaxOutputReportSize for this device
//
// Inputs:  inIOHIDDeviceRef - the IDHIDDeviceRef for this device
//
// Returns:	long - the MaxOutputReportSize for this device
//

extern long IOHIDDevice_GetMaxOutputReportSize( IOHIDDeviceRef inIOHIDDeviceRef );

//*************************************************************************
//
// IOHIDDevice_GetMaxFeatureReportSize( inIOHIDDeviceRef )
//
// Purpose:	get the MaxFeatureReportSize for this device
//
// Inputs:  inIOHIDDeviceRef - the IDHIDDeviceRef for this device
//
// Returns:	long - the MaxFeatureReportSize for this device
//

extern long IOHIDDevice_GetMaxFeatureReportSize( IOHIDDeviceRef inIOHIDDeviceRef );

//*************************************************************************
//
// IOHIDDevice_GetReportInterval( inIOHIDDeviceRef )
//
// Purpose:	get the ReportInterval for this device
//
// Inputs:  inIOHIDDeviceRef - the IDHIDDeviceRef for this device
//
// Returns:	long - the ReportInterval for this device
//

extern long IOHIDDevice_GetReportInterval( IOHIDDeviceRef inIOHIDDeviceRef );

//*************************************************************************
//
// IOHIDDevice_GetQueue( inIOHIDDeviceRef )
//
// Purpose:	get the Queue for this device
//
// Inputs:  inIOHIDDeviceRef - the IDHIDDeviceRef for this device
//
// Returns:	IOHIDQueueRef - the Queue for this device
//

extern IOHIDQueueRef IOHIDDevice_GetQueue( IOHIDDeviceRef inIOHIDDeviceRef );

//*************************************************************************
//
// IOHIDDevice_SetQueue( inIOHIDDeviceRef, inQueueRef )
//
// Purpose:	Set the Queue for this device
//
// Inputs:  inIOHIDDeviceRef - the IDHIDDeviceRef for this device
//			inQueueRef - the Queue
//
// Returns:	nothing
//

extern void IOHIDDevice_SetQueue( IOHIDDeviceRef inIOHIDDeviceRef, IOHIDQueueRef inQueueRef );

//*************************************************************************
//
// IOHIDDevice_GetTransaction( inIOHIDDeviceRef )
//
// Purpose:	get the Transaction for this device
//
// Inputs:  inIOHIDDeviceRef - the IDHIDDeviceRef for this device
//
// Returns:	IOHIDTransactionRef - the Transaction for this device
//

extern IOHIDTransactionRef IOHIDDevice_GetTransaction( IOHIDDeviceRef inIOHIDDeviceRef );

//*************************************************************************
//
// IOHIDDevice_SetTransaction( inIOHIDDeviceRef, inTransactionRef )
//
// Purpose:	Set the Transaction for this device
//
// Inputs:  inIOHIDDeviceRef - the IDHIDDeviceRef for this device
//			inTransactionRef - the Transaction
//
// Returns:	nothing
//

extern void IOHIDDevice_SetTransaction( IOHIDDeviceRef inIOHIDDeviceRef, IOHIDTransactionRef inTransactionRef );

//*****************************************************
#if PRAGMA_STRUCT_ALIGN
#pragma options align=reset
#elif PRAGMA_STRUCT_PACKPUSH
#pragma pack(pop)
#elif PRAGMA_STRUCT_PACK
#pragma pack()
#endif

#ifdef PRAGMA_IMPORT_OFF
#pragma import off
#elif PRAGMA_IMPORT
#pragma import reset
#endif

#ifdef __cplusplus
}
#endif

#endif  // __IOHIDDevice__ //
