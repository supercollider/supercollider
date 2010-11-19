//
//	File:		IOHIDElement_.h of HID Utilities
//
//	Created:	9/21/06
//
//	Contains:	Definition of the interfaces to <IOHIDElement_.c>
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
#ifndef __IOHIDElement___
#define __IOHIDElement___

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
// HIDIsValidElement( inIOHIDElementRef )
//
// Purpose:	validate this element
//
// Inputs:	inIOHIDElementRef	- the element
//
// Returns:	Boolean			- TRUE if this is a valid element ref
//
extern Boolean HIDIsValidElement( IOHIDElementRef inIOHIDElementRef );
	
//*************************************************************************
//
// IOHIDElement_GetValue( inElementRef, inIOHIDValueScaleType )
//
// Purpose:	returns the current value for an element( polling )
//
// Notes:	will return 0 on error conditions which should be accounted for by application
//
// Inputs:	inElementRef	- the element
//			inIOHIDValueScaleType	- scale type ( calibrated or physical )
//
// Returns:	double		- current value for element
//
extern double IOHIDElement_GetValue( IOHIDElementRef inElementRef, IOHIDValueScaleType inIOHIDValueScaleType );

//*************************************************************************
//
// IOHIDElement_GetCalibrationMin( inElementRef )
//
// Purpose:	get the minimum bounds for a calibrated value for this element
//
// Inputs:  inElementRef - the IOHIDElementRef for this element
//
// Returns:	CFIndex - the minimum Calibration value for this element
//

extern CFIndex  IOHIDElement_GetCalibrationMin( IOHIDElementRef inElementRef );

//*************************************************************************
//
// IOHIDElement_SetCalibrationMin( inElementRef, inValue )
//
// Purpose:	set the minimum bounds for a calibrated value for this element
//
// Inputs:  inElementRef	- the IOHIDElementRef for this element
//			inValue			- the minimum bounds for a calibrated value for this element
//
// Returns:	nothing
//

extern void IOHIDElement_SetCalibrationMin( IOHIDElementRef inElementRef, CFIndex inValue );

//*************************************************************************
//
// IOHIDElement_GetCalibrationMax( inElementRef )
//
// Purpose:	get the maximum bounds for a calibrated value for this element
//
// Inputs:  inElementRef - the IOHIDElementRef for this element
//
// Returns:	CFIndex - the maximum Calibration value for this element
//

extern CFIndex  IOHIDElement_GetCalibrationMax( IOHIDElementRef inElementRef );

//*************************************************************************
//
// IOHIDElement_SetCalibrationMax( inElementRef, inValue )
//
// Purpose:	set the maximum bounds for a calibrated value for this element
//
// Inputs:  inElementRef	- the IOHIDElementRef for this element
//			inValue			- the maximum Calibration value for this element
//
// Returns:	nothing
//

extern void IOHIDElement_SetCalibrationMax( IOHIDElementRef inElementRef, CFIndex inValue );

//*************************************************************************
//
// IOHIDElement_GetCalibrationSaturationMin( inElementRef )
//
// Purpose:	get the mininum tolerance to be used when calibrating a logical element value
//
// Inputs:  inElementRef - the IOHIDElementRef for this element
//
// Returns:	CFIndex - the maximum Calibration value for this element
//

extern CFIndex  IOHIDElement_GetCalibrationSaturationMin( IOHIDElementRef inElementRef );

//*************************************************************************
//
// IOHIDElement_SetCalibrationSaturationMin( inElementRef, inValue )
//
// Purpose:	set the mininum tolerance to be used when calibrating a logical element value
//
// Inputs:  inElementRef	- the IOHIDElementRef for this element
//			inValue			- the maximum Calibration value for this element
//
// Returns:	nothing
//

extern void IOHIDElement_SetCalibrationSaturationMin( IOHIDElementRef inElementRef, CFIndex inValue );

//*************************************************************************
//
// IOHIDElement_GetCalibrationSaturationMax( inElementRef )
//
// Purpose:	get the maximum tolerance to be used when calibrating a logical element value
//
// Inputs:  inElementRef - the IOHIDElementRef for this element
//
// Returns:	CFIndex - the maximum Calibration value for this element
//

extern CFIndex  IOHIDElement_GetCalibrationSaturationMax( IOHIDElementRef inElementRef );

//*************************************************************************
//
// IOHIDElement_SetCalibrationSaturationMax( inElementRef, inValue )
//
// Purpose:	set the maximum tolerance to be used when calibrating a logical element value
//
// Inputs:  inElementRef	- the IOHIDElementRef for this element
//			inValue			- the maximum Calibration value for this element
//
// Returns:	nothing
//

extern void IOHIDElement_SetCalibrationSaturationMax( IOHIDElementRef inElementRef, CFIndex inValue );

//*************************************************************************
//
// IOHIDElement_GetCalibrationDeadZoneMin( inElementRef )
//
// Purpose:	get the minimum bounds near the midpoint of a logical value in which the value is ignored
//
// Inputs:  inElementRef - the IOHIDElementRef for this element
//
// Returns:	CFIndex - the maximum Calibration value for this element
//

extern CFIndex  IOHIDElement_GetCalibrationDeadZoneMin( IOHIDElementRef inElementRef );

//*************************************************************************
//
// IOHIDElement_SetCalibrationDeadZoneMin( inElementRef, inValue )
//
// Purpose:	set the minimum bounds near the midpoint of a logical value in which the value is ignored
//
// Inputs:  inElementRef	- the IOHIDElementRef for this element
//			inValue			- the maximum Calibration value for this element
//
// Returns:	nothing
//

extern void IOHIDElement_SetCalibrationDeadZoneMin( IOHIDElementRef inElementRef, CFIndex inValue );

//*************************************************************************
//
// IOHIDElement_GetCalibrationDeadZoneMax( inElementRef )
//
// Purpose:	get the maximum bounds near the midpoint of a logical value in which the value is ignored
//
// Inputs:  inElementRef - the IOHIDElementRef for this element
//
// Returns:	CFIndex - the maximum Calibration value for this element
//

extern CFIndex  IOHIDElement_GetCalibrationDeadZoneMax( IOHIDElementRef inElementRef );

//*************************************************************************
//
// IOHIDElement_SetCalibrationDeadZoneMax( inElementRef, inValue )
//
// Purpose:	set the maximum bounds near the midpoint of a logical value in which the value is ignored
//
// Inputs:  inElementRef	- the IOHIDElementRef for this element
//			inValue			- the maximum Calibration value for this element
//
// Returns:	nothing
//

extern void IOHIDElement_SetCalibrationDeadZoneMax( IOHIDElementRef inElementRef, CFIndex inValue );

//*************************************************************************
//
// IOHIDElement_GetCalibrationGranularity( inElementRef )
//
// Purpose:	get the level of detail returned for a calibrated element value
//
// Inputs:  inElementRef - the IOHIDElementRef for this element
//
// Returns:	double_t - the maximum Calibration value for this element
//

extern double_t  IOHIDElement_GetCalibrationGranularity( IOHIDElementRef inElementRef );

//*************************************************************************
//
// IOHIDElement_SetCalibrationGranularity( inElementRef, inValue )
//
// Purpose:	set the level of detail returned for a calibrated element value
//
// Inputs:  inElementRef	- the IOHIDElementRef for this element
//			inValue			- the the level of detail for this element
//
// Returns:	nothing
//

extern void IOHIDElement_SetCalibrationGranularity( IOHIDElementRef inElementRef, double_t inValue );

//*************************************************************************
//
// IOHIDElement_SetupCalibration( inElementRef )
//
// Purpose:	set default values for the element calibration parameters
//
// Inputs:  inElementRef	- the IOHIDElementRef for this element
//
// Returns:	nothing
//

extern void IOHIDElement_SetupCalibration( IOHIDElementRef inIOHIDElementRef );
	
extern Boolean IOHIDElement_GetLongProperty( IOHIDElementRef inElementRef, CFStringRef inKey, long * outValue );
extern void IOHIDElement_SetLongProperty( IOHIDElementRef inElementRef, CFStringRef inKey, long inValue );

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

#endif  // __IOHIDElement___ //
