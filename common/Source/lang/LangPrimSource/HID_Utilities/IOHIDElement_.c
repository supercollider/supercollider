//
//	File:		IOHIDElement_.c of HID Utilities
//
//	Created:	9/21/06
//
//	Contains:	Convieance functions for IOHIDElementGetProperty
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

//*****************************************************
#pragma mark - includes & imports
//-----------------------------------------------------

#include "IOHIDElement_.h"

//*****************************************************
#pragma mark - typedef's, struct's, enums, defines, etc.
//-----------------------------------------------------

//*****************************************************
#pragma mark - local (static) function prototypes
//-----------------------------------------------------

// static Boolean IOHIDElement_GetLongProperty( IOHIDElementRef inElementRef, CFStringRef inKey, long * outValue );
// static void IOHIDElement_SetLongProperty( IOHIDElementRef inElementRef, CFStringRef inKey, long inValue );

//*****************************************************
#pragma mark - exported globals
//-----------------------------------------------------

//*****************************************************
#pragma mark - local (static) globals
//-----------------------------------------------------

//*****************************************************
#pragma mark - exported function implementations
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
Boolean HIDIsValidElement( IOHIDElementRef inIOHIDElementRef )
{
	Boolean result = FALSE;	// assume failure (pessimist!)
	if ( inIOHIDElementRef ) {
		if ( CFGetTypeID( inIOHIDElementRef ) == IOHIDElementGetTypeID() ) {
			result = TRUE;
		}
	}
	return result;
}

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
double IOHIDElement_GetValue( IOHIDElementRef inElementRef, IOHIDValueScaleType inIOHIDValueScaleType )
{
	long result = 0;
	IOHIDValueRef tIOHIDValueRef;

	if ( kIOReturnSuccess == IOHIDDeviceGetValue( IOHIDElementGetDevice( inElementRef ), inElementRef, &tIOHIDValueRef ) ) {
		result = IOHIDValueGetScaledValue( tIOHIDValueRef, inIOHIDValueScaleType );
	}
	return result;
}   // IOHIDElement_GetValue

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

CFIndex IOHIDElement_GetCalibrationMin( IOHIDElementRef inElementRef )
{
	CFIndex result;

	if ( !IOHIDElement_GetLongProperty( inElementRef, CFSTR( kIOHIDElementCalibrationMinKey ), &result ) ) {
		if ( !IOHIDElement_GetLongProperty( inElementRef, CFSTR( kIOHIDElementMaxKey ), &result ) ) {
			result = 0x7FFFFFFF;
		}
		IOHIDElement_SetLongProperty( inElementRef, CFSTR( kIOHIDElementCalibrationMinKey ), result );
	}
	return result;
}   // IOHIDElement_GetCalibrationMin

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

void IOHIDElement_SetCalibrationMin( IOHIDElementRef inElementRef, CFIndex inValue )
{
	IOHIDElement_SetLongProperty( inElementRef, CFSTR( kIOHIDElementCalibrationMinKey ), inValue );
}   // IOHIDElement_SetCalibrationMin

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

CFIndex IOHIDElement_GetCalibrationMax( IOHIDElementRef inElementRef )
{
	CFIndex result;

	if ( !IOHIDElement_GetLongProperty( inElementRef, CFSTR( kIOHIDElementCalibrationMaxKey ), &result ) ) {
		if ( !IOHIDElement_GetLongProperty( inElementRef, CFSTR( kIOHIDElementMinKey ), &result ) ) {
			result = -0x7FFFFFFF;
		}
		IOHIDElement_SetLongProperty( inElementRef, CFSTR( kIOHIDElementCalibrationMaxKey ), result );
	}
	return result;
}   // IOHIDElement_GetCalibrationMax

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

void IOHIDElement_SetCalibrationMax( IOHIDElementRef inElementRef, CFIndex inValue )
{
	IOHIDElement_SetLongProperty( inElementRef, CFSTR( kIOHIDElementCalibrationMaxKey ), inValue );
}   // IOHIDElement_SetCalibrationMax

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

CFIndex IOHIDElement_GetCalibrationSaturationMin( IOHIDElementRef inElementRef )
{
	CFIndex result;

	if ( !IOHIDElement_GetLongProperty( inElementRef, CFSTR( kIOHIDElementCalibrationSaturationMinKey ), &result ) ) {
		if ( !IOHIDElement_GetLongProperty( inElementRef, CFSTR( kIOHIDElementMinKey ), &result ) ) {
			result = -0x7FFFFFFF;
		}
		IOHIDElement_SetLongProperty( inElementRef, CFSTR( kIOHIDElementCalibrationSaturationMinKey ), result );
	}
	return result;
}   // IOHIDElement_GetCalibrationSaturationMin

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

void IOHIDElement_SetCalibrationSaturationMin( IOHIDElementRef inElementRef, CFIndex inValue )
{
	IOHIDElement_SetLongProperty( inElementRef, CFSTR( kIOHIDElementCalibrationSaturationMinKey ), inValue );
}   // IOHIDElement_SetCalibrationSaturationMin

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

CFIndex IOHIDElement_GetCalibrationSaturationMax( IOHIDElementRef inElementRef )
{
	CFIndex result;

	if ( !IOHIDElement_GetLongProperty( inElementRef, CFSTR( kIOHIDElementCalibrationSaturationMaxKey ), &result ) ) {
		if ( !IOHIDElement_GetLongProperty( inElementRef, CFSTR( kIOHIDElementMinKey ), &result ) ) {
			result = -0x7FFFFFFF;
		}
		IOHIDElement_SetLongProperty( inElementRef, CFSTR( kIOHIDElementCalibrationSaturationMaxKey ), result );
	}
	return result;
}   // IOHIDElement_GetCalibrationSaturationMax

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

void IOHIDElement_SetCalibrationSaturationMax( IOHIDElementRef inElementRef, CFIndex inValue )
{
	IOHIDElement_SetLongProperty( inElementRef, CFSTR( kIOHIDElementCalibrationSaturationMaxKey ), inValue );
}   // IOHIDElement_SetCalibrationSaturationMax

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

CFIndex IOHIDElement_GetCalibrationDeadZoneMin( IOHIDElementRef inElementRef )
{
	CFIndex result;

	if ( !IOHIDElement_GetLongProperty( inElementRef, CFSTR( kIOHIDElementCalibrationDeadZoneMinKey ), &result ) ) {
		if ( !IOHIDElement_GetLongProperty( inElementRef, CFSTR( kIOHIDElementMinKey ), &result ) ) {
			result = -0x7FFFFFFF;
		}
		IOHIDElement_SetLongProperty( inElementRef, CFSTR( kIOHIDElementCalibrationDeadZoneMinKey ), result );
	}
	return result;
}   // IOHIDElement_GetCalibrationDeadZoneMin

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

void IOHIDElement_SetCalibrationDeadZoneMin( IOHIDElementRef inElementRef, CFIndex inValue )
{
	IOHIDElement_SetLongProperty( inElementRef, CFSTR( kIOHIDElementCalibrationDeadZoneMinKey ), inValue );
}   // IOHIDElement_SetCalibrationDeadZoneMin

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

CFIndex IOHIDElement_GetCalibrationDeadZoneMax( IOHIDElementRef inElementRef )
{
	CFIndex result;

	if ( !IOHIDElement_GetLongProperty( inElementRef, CFSTR( kIOHIDElementCalibrationDeadZoneMaxKey ), &result ) ) {
		if ( !IOHIDElement_GetLongProperty( inElementRef, CFSTR( kIOHIDElementMinKey ), &result ) ) {
			result = -0x7FFFFFFF;
		}
		IOHIDElement_SetLongProperty( inElementRef, CFSTR( kIOHIDElementCalibrationDeadZoneMaxKey ), result );
	}
	return result;
}   // IOHIDElement_GetCalibrationDeadZoneMax

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

void IOHIDElement_SetCalibrationDeadZoneMax( IOHIDElementRef inElementRef, CFIndex inValue )
{
	IOHIDElement_SetLongProperty( inElementRef, CFSTR( kIOHIDElementCalibrationDeadZoneMaxKey ), inValue );
}   // IOHIDElement_SetCalibrationDeadZoneMax

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

double_t IOHIDElement_GetCalibrationGranularity( IOHIDElementRef inElementRef )
{
	CFIndex result;

	if ( !IOHIDElement_GetLongProperty( inElementRef, CFSTR( kIOHIDElementCalibrationGranularityKey ), &result ) ) {
		if ( !IOHIDElement_GetLongProperty( inElementRef, CFSTR( kIOHIDElementMinKey ), &result ) ) {
			result = -0x7FFFFFFF;
		}
		IOHIDElement_SetLongProperty( inElementRef, CFSTR( kIOHIDElementCalibrationGranularityKey ), result );
	}
	return result;
}   // IOHIDElement_GetCalibrationGranularity

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

void IOHIDElement_SetCalibrationGranularity( IOHIDElementRef inElementRef, double_t inValue )
{
	IOHIDElement_SetLongProperty( inElementRef, CFSTR( kIOHIDElementCalibrationGranularityKey ), inValue );
}   // IOHIDElement_SetCalibrationGranularity

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
void IOHIDElement_SetupCalibration( IOHIDElementRef inIOHIDElementRef )
{
	// these are the min/max values returned by IOHIDValueGetScaledValue( v, kIOHIDValueScaleTypeCalibrated );
	IOHIDElement_SetCalibrationMin( inIOHIDElementRef, IOHIDElementGetLogicalMin( inIOHIDElementRef ) );
	IOHIDElement_SetCalibrationMax( inIOHIDElementRef, IOHIDElementGetLogicalMax( inIOHIDElementRef ) );

	// this is the granularity of the values returned by IOHIDValueGetScaledValue( v, kIOHIDValueScaleTypeCalibrated );
	// for example if set to 0.1 the values returned will be multiples of 0.1 ( 0.1, 0.2, 0.3, etc. )
	IOHIDElement_SetCalibrationGranularity( inIOHIDElementRef, 0. );
	
	// these define the dead zone (like in the middel of joystick axis)
	IOHIDElement_SetCalibrationDeadZoneMin( inIOHIDElementRef, 0 );
	IOHIDElement_SetCalibrationDeadZoneMax( inIOHIDElementRef, 0 );
#if 1
	// get the current value of this element
	double value = IOHIDElement_GetValue( inIOHIDElementRef, kIOHIDValueScaleTypePhysical );
	// use it as our min/mas saturation
	IOHIDElement_SetCalibrationSaturationMin( inIOHIDElementRef, value );
	IOHIDElement_SetCalibrationSaturationMax( inIOHIDElementRef, value );
#else
	// calculate the middle physical value we would expect from this element
	CFIndex valueMin = IOHIDElementGetPhysicalMin( inIOHIDElementRef );
	CFIndex valueMax = IOHIDElementGetPhysicalMax( inIOHIDElementRef );
	CFIndex valueMid = ( valueMin + valueMax ) / 2;
	
	// use it as our min/mas saturation
	// this value determines the min/max values that have been recieved from the device element
	IOHIDElement_SetCalibrationSaturationMin( inIOHIDElementRef, valueMid );
	IOHIDElement_SetCalibrationSaturationMax( inIOHIDElementRef, valueMid );
	
	// get the current value of this element
	double value = IOHIDElement_GetValue( inIOHIDElementRef, kIOHIDValueScaleTypePhysical );
	
	// and use it to adjust the current saturation values if it's outside their range
	if ( value < IOHIDElement_GetCalibrationSaturationMin( inIOHIDElementRef ) ) {
		IOHIDElement_SetCalibrationSaturationMin( inIOHIDElementRef, value );
	}
	
	if ( value > IOHIDElement_GetCalibrationSaturationMax( inIOHIDElementRef ) ) {
		IOHIDElement_SetCalibrationSaturationMax( inIOHIDElementRef, value );
	}
#endif
}	// IOHIDElement_SetupCalibration
//*****************************************************
#pragma mark - local (static) function implementations
//-----------------------------------------------------

//*************************************************************************
//
// IOHIDElement_GetLongProperty( inElementRef, inKey, outValue )
//
// Purpose:	convieance function to return a long property of an element
//
// Inputs:	inElementRef	- the element
//			inKey			- CFString for the key
//			outValue		- address where to store the value
// Returns:	Boolean			- TRUE if successful
//			outValue		- the long property's value
//

Boolean IOHIDElement_GetLongProperty( IOHIDElementRef inElementRef, CFStringRef inKey, long * outValue )
{
	Boolean result = FALSE;

	CFTypeRef tCFTypeRef = IOHIDElementGetProperty( inElementRef, inKey );

	if ( tCFTypeRef ) {
		// if this is a number
		if ( CFNumberGetTypeID() == CFGetTypeID( tCFTypeRef ) ) {
			// get it's value
			result =  CFNumberGetValue( ( CFNumberRef ) tCFTypeRef, kCFNumberSInt32Type, outValue );
		}
	}
	return result;
} /* IOHIDElement_GetLongProperty */

//*************************************************************************
//
// IOHIDElement_SetLongProperty( inElementRef, inKey, inValue )
//
// Purpose:	convieance function to set a long property of an element
//
// Inputs:	inElementRef	- the element
//			inKey			- CFString for the key
//			inValue			- the value to set it to
//
// Returns:	nothing
//

void IOHIDElement_SetLongProperty( IOHIDElementRef inElementRef, CFStringRef inKey, long inValue )
{
	CFNumberRef tCFNumberRef = CFNumberCreate( kCFAllocatorDefault, kCFNumberSInt32Type, &inValue );

	if ( tCFNumberRef ) {
		IOHIDElementSetProperty( inElementRef, inKey, tCFNumberRef );
		CFRelease( tCFNumberRef );
	}
}

//*****************************************************
