//
// File:       HID_Name_Lookup.c
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
// Copyright © 2002-2009 Apple Inc. All Rights Reserved.
//
//*****************************************************
#pragma mark - includes & imports
//*****************************************************
#include "HID_Utilities_External.h"
//*****************************************************
#pragma mark - typedefs, enums, defines, etc.
//*****************************************************
#define FAKE_MISSING_NAMES		0	// for debugging; returns the vendor, product & cookie ( or usage info ) as numbers.
#define VERBOSE_ELEMENT_NAMES	0	// set TRUE to include vender & product names in element names ( useful for debugging )

#define kNameKeyCFStringRef CFSTR( "Name" )
//*****************************************************
#pragma mark - local ( static ) function prototypes
//*****************************************************

#if 0	// currently unused
static SInt32 hu_SaveToXMLFile( CFPropertyListRef inCFPRef, CFURLRef inCFURLRef );
static SInt32 hu_XMLSave( CFPropertyListRef inCFPropertyListRef, CFStringRef inResourceName, CFStringRef inResourceExtension );
#endif
static CFPropertyListRef hu_LoadFromXMLFile( CFURLRef inCFURLRef );
static CFPropertyListRef hu_XMLLoad( CFStringRef inResourceName, CFStringRef inResourceExtension );

static Boolean hu_XMLSearchForElementNameByCookie( long inVendorID, long inProductID, IOHIDElementCookie inCookie, char * outCStr );
static Boolean hu_XMLSearchForElementNameByUsage( long inVendorID, long inProductID, long inUsagePage, long inUsage, char * outCStr );

static Boolean hu_XMLSearchForVendorNameByVendorID( long inVendorID, char * outCStr );
static Boolean hu_XMLSearchForProductNameByVendorProductID( long inVendorID, long inProductID, char * outCStr );

#if 0	// currently unused
static Boolean hu_AddVendorProductToCFDict( CFMutableDictionaryRef inCFMutableDictionaryRef, long inVendorID, CFStringRef inVendorCFStringRef, long inProductID, CFStringRef inProductCFStringRef );
static Boolean hu_AddDeviceElementToUsageXML( IOHIDDeviceRef inIOHIDDeviceRef, IOHIDElementRef inIOHIDElementRef );
#endif
//*****************************************************
#pragma mark - exported globals
//*****************************************************
#pragma mark - local ( static ) globals
//*****************************************************
static CFPropertyListRef gCookieCFPropertyListRef = NULL;
static CFPropertyListRef gUsageCFPropertyListRef = NULL;

//*****************************************************
#pragma mark - exported function implementations
//*****************************************************

/*************************************************************************
 *
 * HIDGetVendorNameFromVendorID( inVendorID, inProductID, inCookie, outCStrName )
 *
 * Purpose: Uses an devices vendor ID to generate a name for it.
 *
 * Notes:	Now uses XML files to store dictionary of names
 *
 * Inputs: inVendorID - the elements vendor ID
 *			outCStrName - address where result will be returned
 * Returns: Boolean		- if successful
 */
Boolean HIDGetVendorNameFromVendorID( long inVendorID, char * outCStrName )
{
	Boolean result = FALSE;
	* outCStrName = 0; // clear name

	if ( hu_XMLSearchForVendorNameByVendorID( inVendorID, outCStrName ) )
		return TRUE;

#if FAKE_MISSING_NAMES
	sprintf( outCStrName, "#{ V: %ld}#", inVendorID );
	result = TRUE;
#endif FAKE_MISSING_NAMES
	return result;
}	// HIDGetVendorNameFromVendorID

/*************************************************************************
 *
 * HIDGetProductNameFromVendorProductID( inVendorID, inProductID, outCStrName )
 *
 * Purpose: Uses an elements vendor, product & usage info to generate a name for it.
 *
 * Notes:	Now uses XML files to store dictionary of names
 *
 * Inputs: inVendorID - the elements vendor ID
 *			inProductID - the elements product ID
 *			inUsagePage	- the elements usage page
 *			inUsage		- the elements usage
 *			outCStrName - address where result will be returned
 * Returns: Boolean		- if successful
 */
Boolean HIDGetProductNameFromVendorProductID( long inVendorID, long inProductID, char * outCStrName )
{
	Boolean result = FALSE;
	* outCStrName = 0; // clear name

	if ( hu_XMLSearchForProductNameByVendorProductID( inVendorID, inProductID, outCStrName ) )
		return TRUE;

#if FAKE_MISSING_NAMES
	sprintf( outCStrName, "#{ V: %ld, P: %ld, U: %ld: %ld}#", inVendorID, inProductID, inUsagePage, inUsage );
	result = TRUE;
#endif FAKE_MISSING_NAMES
	return result;
}	// HIDGetProductNameFromVendorProductID

/*************************************************************************
 *
 * HIDGetElementNameFromVendorProductCookie( inVendorID, inProductID, inCookie, outCStrName )
 *
 * Purpose: Uses an elements vendor, product & cookie to generate a name for it.
 *
 * Notes:	Now uses XML files to store dictionary of names
 *
 * Inputs: inVendorID - the elements vendor ID
 *			inProductID - the elements product ID
 *			inCookie		- the elements cookie
 *			outCStrName - address where result will be returned
 * Returns: Boolean		- if successful
 */
Boolean HIDGetElementNameFromVendorProductCookie (int inVendorID, int inProductID, IOHIDElementCookie inCookie, char * outCStrName)
{
	Boolean result = FALSE;
	*outCStrName = 0; // clear name

	// Look in the XML file first
	if ( hu_XMLSearchForElementNameByCookie( inVendorID, inProductID, inCookie, outCStrName ) )
		return TRUE;

#if FAKE_MISSING_NAMES
	sprintf( outCStrName, "#{ V: %ld, P: %ld, C: %ld}#", inVendorID, inProductID, inCookie );
#else
	result = FALSE;
#endif FAKE_MISSING_NAMES
	return result;
}	// HIDGetElementNameFromVendorProductCookie

/*************************************************************************
 *
 * HIDGetElementNameFromVendorProductUsage( inVendorID, inProductID, inUsagePage, inUsage, outCStrName )
 *
 * Purpose: Uses an elements vendor, product & usage info to generate a name for it.
 *
 * Notes:	Now uses XML files to store dictionary of names
 *
 * Inputs: inVendorID - the elements vendor ID
 *			inProductID - the elements product ID
 *			inUsagePage	- the elements usage page
 *			inUsage		- the elements usage
 *			outCStrName - address where result will be returned
 * Returns: Boolean		- if successful
 */
Boolean HIDGetElementNameFromVendorProductUsage( long inVendorID, long inProductID, long inUsagePage, long inUsage, char * outCStrName )
{
	Boolean result = FALSE;
	* outCStrName = 0; // clear name

	if ( hu_XMLSearchForElementNameByUsage( inVendorID, inProductID, inUsagePage, inUsage, outCStrName ) )
		return TRUE;

#if FAKE_MISSING_NAMES
	sprintf( outCStrName, "#{ V: %ld, P: %ld, U: %ld: %ld}#", inVendorID, inProductID, inUsagePage, inUsage );
	result = TRUE;
#endif FAKE_MISSING_NAMES
	return result;
}	// HIDGetElementNameFromVendorProductUsage

#if 0	// currently unused
/*************************************************************************
 *
 * HIDAddDeviceToXML( inDevice )
 *
 * Purpose: Adds a devices info to the HID_device_usage_strings.plist( XML ) file
 *
 * Inputs: inDevice		- the device
 * Returns: Boolean		- if successful
 */
static Boolean HIDAddDeviceToXML( IOHIDDeviceRef inIOHIDDeviceRef )
{
	Boolean result = FALSE;

	if ( HIDIsValidDevice( inIOHIDDeviceRef ) ) {
		CFStringRef vendorCFStringRef = IOHIDDevice_GetManufacturer( inIOHIDDeviceRef );
		CFStringRef productCFStringRef = IOHIDDevice_GetProduct( inIOHIDDeviceRef );
		if ( vendorCFStringRef && productCFStringRef ) {

#if 0 // don't update the cookie xml file
			gCookieCFPropertyListRef = hu_XMLLoad( CFSTR( "HID_cookie_strings" ), CFSTR( "plist" ) );
			if ( gCookieCFPropertyListRef ) {
				CFMutableDictionaryRef tCFMutableDictionaryRef = CFDictionaryCreateMutableCopy( kCFAllocatorDefault, 0, gCookieCFPropertyListRef );
				if ( tCFMutableDictionaryRef ) {
					if ( hu_AddVendorProductToCFDict( tCFMutableDictionaryRef, vendorID, vendorCFStringRef, productID, productCFStringRef ) ) {
						hu_XMLSave( tCFMutableDictionaryRef, CFSTR( "HID_cookie_strings" ), CFSTR( "plist" ) );
						result = TRUE;
					}
					CFRelease( tCFMutableDictionaryRef );
				}
			}
#endif
			if ( gUsageCFPropertyListRef )
				CFRelease( gUsageCFPropertyListRef );

			gUsageCFPropertyListRef = hu_XMLLoad( CFSTR( "HID_device_usage_strings" ), CFSTR( "plist" ) );
			if ( gUsageCFPropertyListRef ) {
				CFMutableDictionaryRef tCFMutableDictionaryRef = CFDictionaryCreateMutableCopy( kCFAllocatorDefault, 0, gUsageCFPropertyListRef );
				if ( tCFMutableDictionaryRef ) {
					long vendorID = IOHIDDevice_GetVendorID( inIOHIDDeviceRef );
					long productID = IOHIDDevice_GetProductID( inIOHIDDeviceRef );
					if ( hu_AddVendorProductToCFDict( tCFMutableDictionaryRef, vendorID, vendorCFStringRef, productID, productCFStringRef ) ) {
						hu_XMLSave( tCFMutableDictionaryRef, CFSTR( "HID_device_usage_strings" ), CFSTR( "plist" ) );
						result = TRUE;
					}
					CFRelease( tCFMutableDictionaryRef );
				}
			}
		}
	}
	return result;
}	// HIDAddDeviceToXML

/*************************************************************************
 *
 * HIDAddDeviceElementToXML( inDevice, inElement )
 *
 * Purpose: Adds a devices info to the HID_device_usage_strings.plist( XML ) file
 *
 * Inputs: inDevice		- the device
 *			inElement	- the element
 *
 * Returns: Boolean		- if successful
 */
Boolean HIDAddDeviceElementToXML( IOHIDDeviceRef inIOHIDDeviceRef, IOHIDElementRef inIOHIDElementRef )
{
	Boolean result = FALSE;

	if ( HIDIsValidElement( inIOHIDElementRef ) ) {
		if ( HIDAddDeviceToXML( inIOHIDDeviceRef ) )
			result = TRUE;

		if ( hu_AddDeviceElementToUsageXML( inIOHIDDeviceRef, inIOHIDElementRef ) ) {
			result = TRUE;
		}
	}
	return result;
}	// HIDAddDeviceElementToXML
#endif
/*************************************************************************
 *
 * HIDGetTypeName( inIOHIDElementType, outCStrName )
 *
 * Purpose: return a C string for a given element type( see IOHIDKeys.h )
 * Notes:	returns "Unknown Type" for invalid types
 *
 * Inputs: inIOHIDElementType	- type element type
 *			outCStrName			- address where to store element type string
 *
 * Returns: outCStrName			- the element type string
 */

void HIDGetTypeName( IOHIDElementType inIOHIDElementType, char * outCStrName )
{
	switch( inIOHIDElementType ) {
		case kIOHIDElementTypeInput_Misc: {
			sprintf( outCStrName, "Miscellaneous Input" );
			break;
		}
		case kIOHIDElementTypeInput_Button: {
			sprintf( outCStrName, "Button Input" );
			break;
		}
		case kIOHIDElementTypeInput_Axis: {
			sprintf( outCStrName, "Axis Input" );
			break;
		}
		case kIOHIDElementTypeInput_ScanCodes: {
			sprintf( outCStrName, "Scan Code Input" );
			break;
		}
		case kIOHIDElementTypeOutput:	{
			sprintf( outCStrName, "Output" );
			break;
		}
		case kIOHIDElementTypeFeature: {
			sprintf( outCStrName, "Feature" );
			break;
		}
		case kIOHIDElementTypeCollection: {
			sprintf( outCStrName, "Collection" );
			break;
		}
		default: {
			sprintf( outCStrName, "Unknown Type" );
			break;
		}
	}
}	// HIDGetTypeName


void HIDGetUsageName (const long valueUsagePage, const long valueUsage, char * cstrName)
{
    switch (valueUsagePage)
    {
        case kHIDPage_Undefined:
			switch (valueUsage)
        {
            default: sprintf (cstrName, "Undefined Page, Usage 0x%lx", valueUsage); break;
        }
            break;
        case kHIDPage_GenericDesktop:
            switch (valueUsage)
        {
            case kHIDUsage_GD_Pointer: sprintf (cstrName, "Pointer"); break;
            case kHIDUsage_GD_Mouse: sprintf (cstrName, "Mouse"); break;
            case kHIDUsage_GD_Joystick: sprintf (cstrName, "Joystick"); break;
            case kHIDUsage_GD_GamePad: sprintf (cstrName, "GamePad"); break;
            case kHIDUsage_GD_Keyboard: sprintf (cstrName, "Keyboard"); break;
            case kHIDUsage_GD_Keypad: sprintf (cstrName, "Keypad"); break;
            case kHIDUsage_GD_MultiAxisController: sprintf (cstrName, "Multi-Axis Controller"); break;

            case kHIDUsage_GD_X: sprintf (cstrName, "X-Axis"); break;
            case kHIDUsage_GD_Y: sprintf (cstrName, "Y-Axis"); break;
            case kHIDUsage_GD_Z: sprintf (cstrName, "Z-Axis"); break;
            case kHIDUsage_GD_Rx: sprintf (cstrName, "X-Rotation"); break;
            case kHIDUsage_GD_Ry: sprintf (cstrName, "Y-Rotation"); break;
            case kHIDUsage_GD_Rz: sprintf (cstrName, "Z-Rotation"); break;
            case kHIDUsage_GD_Slider: sprintf (cstrName, "Slider"); break;
            case kHIDUsage_GD_Dial: sprintf (cstrName, "Dial"); break;
            case kHIDUsage_GD_Wheel: sprintf (cstrName, "Wheel"); break;
            case kHIDUsage_GD_Hatswitch: sprintf (cstrName, "Hatswitch"); break;
            case kHIDUsage_GD_CountedBuffer: sprintf (cstrName, "Counted Buffer"); break;
            case kHIDUsage_GD_ByteCount: sprintf (cstrName, "Byte Count"); break;
            case kHIDUsage_GD_MotionWakeup: sprintf (cstrName, "Motion Wakeup"); break;
            case kHIDUsage_GD_Start: sprintf (cstrName, "Start"); break;
            case kHIDUsage_GD_Select: sprintf (cstrName, "Select"); break;

            case kHIDUsage_GD_Vx: sprintf (cstrName, "X-Velocity"); break;
            case kHIDUsage_GD_Vy: sprintf (cstrName, "Y-Velocity"); break;
            case kHIDUsage_GD_Vz: sprintf (cstrName, "Z-Velocity"); break;
            case kHIDUsage_GD_Vbrx: sprintf (cstrName, "X-Rotation Velocity"); break;
            case kHIDUsage_GD_Vbry: sprintf (cstrName, "Y-Rotation Velocity"); break;
            case kHIDUsage_GD_Vbrz: sprintf (cstrName, "Z-Rotation Velocity"); break;
            case kHIDUsage_GD_Vno: sprintf (cstrName, "Vno"); break;

            case kHIDUsage_GD_SystemControl: sprintf (cstrName, "System Control"); break;
            case kHIDUsage_GD_SystemPowerDown: sprintf (cstrName, "System Power Down"); break;
            case kHIDUsage_GD_SystemSleep: sprintf (cstrName, "System Sleep"); break;
            case kHIDUsage_GD_SystemWakeUp: sprintf (cstrName, "System Wake Up"); break;
            case kHIDUsage_GD_SystemContextMenu: sprintf (cstrName, "System Context Menu"); break;
            case kHIDUsage_GD_SystemMainMenu: sprintf (cstrName, "System Main Menu"); break;
            case kHIDUsage_GD_SystemAppMenu: sprintf (cstrName, "System App Menu"); break;
            case kHIDUsage_GD_SystemMenuHelp: sprintf (cstrName, "System Menu Help"); break;
            case kHIDUsage_GD_SystemMenuExit: sprintf (cstrName, "System Menu Exit"); break;
            case kHIDUsage_GD_SystemMenu: sprintf (cstrName, "System Menu"); break;
            case kHIDUsage_GD_SystemMenuRight: sprintf (cstrName, "System Menu Right"); break;
            case kHIDUsage_GD_SystemMenuLeft: sprintf (cstrName, "System Menu Left"); break;
            case kHIDUsage_GD_SystemMenuUp: sprintf (cstrName, "System Menu Up"); break;
            case kHIDUsage_GD_SystemMenuDown: sprintf (cstrName, "System Menu Down"); break;

            case kHIDUsage_GD_DPadUp: sprintf (cstrName, "DPad Up"); break;
            case kHIDUsage_GD_DPadDown: sprintf (cstrName, "DPad Down"); break;
            case kHIDUsage_GD_DPadRight: sprintf (cstrName, "DPad Right"); break;
            case kHIDUsage_GD_DPadLeft: sprintf (cstrName, "DPad Left"); break;

            case kHIDUsage_GD_Reserved: sprintf (cstrName, "Reserved"); break;

            default: sprintf (cstrName, "Generic Desktop Usage 0x%lx", valueUsage); break;
        }
            break;
        case kHIDPage_Simulation:
            switch (valueUsage)
        {
            default: sprintf (cstrName, "Simulation Usage 0x%lx", valueUsage); break;
        }
            break;
        case kHIDPage_VR:
            switch (valueUsage)
        {
            default: sprintf (cstrName, "VR Usage 0x%lx", valueUsage); break;
        }
            break;
        case kHIDPage_Sport:
            switch (valueUsage)
        {
            default: sprintf (cstrName, "Sport Usage 0x%lx", valueUsage); break;
        }
            break;
        case kHIDPage_Game:
            switch (valueUsage)
        {
            default: sprintf (cstrName, "Game Usage 0x%lx", valueUsage); break;
        }
            break;
        case kHIDPage_KeyboardOrKeypad:
            switch (valueUsage)
        {
            default: sprintf (cstrName, "Keyboard Usage 0x%lx", valueUsage); break;
        }
            break;
        case kHIDPage_LEDs:
            switch (valueUsage)
        {
				// some LED usages
            case kHIDUsage_LED_IndicatorRed: sprintf (cstrName, "Red LED"); break;
            case kHIDUsage_LED_IndicatorGreen: sprintf (cstrName, "Green LED"); break;
            case kHIDUsage_LED_IndicatorAmber: sprintf (cstrName, "Amber LED"); break;
            case kHIDUsage_LED_GenericIndicator: sprintf (cstrName, "Generic LED"); break;
            case kHIDUsage_LED_SystemSuspend: sprintf (cstrName, "System Suspend LED"); break;
            case kHIDUsage_LED_ExternalPowerConnected: sprintf (cstrName, "External Power LED"); break;
            default: sprintf (cstrName, "LED Usage 0x%lx", valueUsage); break;
        }
            break;
        case kHIDPage_Button:
            switch (valueUsage)
        {
            default: sprintf (cstrName, "Button #%ld", valueUsage); break;
        }
            break;
        case kHIDPage_Ordinal:
            switch (valueUsage)
        {
            default: sprintf (cstrName, "Ordinal Instance %lx", valueUsage); break;
        }
            break;
        case kHIDPage_Telephony:
            switch (valueUsage)
        {
            default: sprintf (cstrName, "Telephony Usage 0x%lx", valueUsage); break;
        }
            break;
        case kHIDPage_Consumer:
            switch (valueUsage)
        {
            default: sprintf (cstrName, "Consumer Usage 0x%lx", valueUsage); break;
        }
            break;
        case kHIDPage_Digitizer:
            switch (valueUsage)
        {
            default: sprintf (cstrName, "Digitizer Usage 0x%lx", valueUsage); break;
        }
            break;
        case kHIDPage_PID:
			if (((valueUsage >= 0x02) && (valueUsage <= 0x1F)) || ((valueUsage >= 0x29) && (valueUsage <= 0x2F)) ||
                ((valueUsage >= 0x35) && (valueUsage <= 0x3F)) || ((valueUsage >= 0x44) && (valueUsage <= 0x4F)) ||
                (valueUsage == 0x8A) || (valueUsage == 0x93)  || ((valueUsage >= 0x9D) && (valueUsage <= 0x9E)) ||
                ((valueUsage >= 0xA1) && (valueUsage <= 0xA3)) || ((valueUsage >= 0xAD) && (valueUsage <= 0xFFFF)))
                sprintf (cstrName, "PID Reserved");
			else
				switch (valueUsage)
            {
                case 0x00: sprintf (cstrName, "PID Undefined Usage"); break;
                case kHIDUsage_PID_PhysicalInterfaceDevice: sprintf (cstrName, "Physical Interface Device"); break;
                case kHIDUsage_PID_Normal: sprintf (cstrName, "Normal Force"); break;

                case kHIDUsage_PID_SetEffectReport: sprintf (cstrName, "Set Effect Report"); break;
                case kHIDUsage_PID_EffectBlockIndex: sprintf (cstrName, "Effect Block Index"); break;
                case kHIDUsage_PID_ParamBlockOffset: sprintf (cstrName, "Parameter Block Offset"); break;
                case kHIDUsage_PID_ROM_Flag: sprintf (cstrName, "ROM Flag"); break;

                case kHIDUsage_PID_EffectType: sprintf (cstrName, "Effect Type"); break;
                case kHIDUsage_PID_ET_ConstantForce: sprintf (cstrName, "Effect Type Constant Force"); break;
                case kHIDUsage_PID_ET_Ramp: sprintf (cstrName, "Effect Type Ramp"); break;
                case kHIDUsage_PID_ET_CustomForceData: sprintf (cstrName, "Effect Type Custom Force Data"); break;
                case kHIDUsage_PID_ET_Square: sprintf (cstrName, "Effect Type Square"); break;
                case kHIDUsage_PID_ET_Sine: sprintf (cstrName, "Effect Type Sine"); break;
                case kHIDUsage_PID_ET_Triangle: sprintf (cstrName, "Effect Type Triangle"); break;
                case kHIDUsage_PID_ET_SawtoothUp: sprintf (cstrName, "Effect Type Sawtooth Up"); break;
                case kHIDUsage_PID_ET_SawtoothDown: sprintf (cstrName, "Effect Type Sawtooth Down"); break;
                case kHIDUsage_PID_ET_Spring: sprintf (cstrName, "Effect Type Spring"); break;
                case kHIDUsage_PID_ET_Damper: sprintf (cstrName, "Effect Type Damper"); break;
                case kHIDUsage_PID_ET_Inertia: sprintf (cstrName, "Effect Type Inertia"); break;
                case kHIDUsage_PID_ET_Friction: sprintf (cstrName, "Effect Type Friction"); break;
                case kHIDUsage_PID_Duration: sprintf (cstrName, "Effect Duration"); break;
                case kHIDUsage_PID_SamplePeriod: sprintf (cstrName, "Effect Sample Period"); break;
                case kHIDUsage_PID_Gain: sprintf (cstrName, "Effect Gain"); break;
                case kHIDUsage_PID_TriggerButton: sprintf (cstrName, "Effect Trigger Button"); break;
                case kHIDUsage_PID_TriggerRepeatInterval: sprintf (cstrName, "Effect Trigger Repeat Interval"); break;

                case kHIDUsage_PID_AxesEnable: sprintf (cstrName, "Axis Enable"); break;
                case kHIDUsage_PID_DirectionEnable: sprintf (cstrName, "Direction Enable"); break;

                case kHIDUsage_PID_Direction: sprintf (cstrName, "Direction"); break;

                case kHIDUsage_PID_TypeSpecificBlockOffset: sprintf (cstrName, "Type Specific Block Offset"); break;

                case kHIDUsage_PID_BlockType: sprintf (cstrName, "Block Type"); break;

                case kHIDUsage_PID_SetEnvelopeReport: sprintf (cstrName, "Set Envelope Report"); break;
                case kHIDUsage_PID_AttackLevel: sprintf (cstrName, "Envelope Attack Level"); break;
                case kHIDUsage_PID_AttackTime: sprintf (cstrName, "Envelope Attack Time"); break;
                case kHIDUsage_PID_FadeLevel: sprintf (cstrName, "Envelope Fade Level"); break;
                case kHIDUsage_PID_FadeTime: sprintf (cstrName, "Envelope Fade Time"); break;

                case kHIDUsage_PID_SetConditionReport: sprintf (cstrName, "Set Condition Report"); break;
                case kHIDUsage_PID_CP_Offset: sprintf (cstrName, "Condition CP Offset"); break;
                case kHIDUsage_PID_PositiveCoefficient: sprintf (cstrName, "Condition Positive Coefficient"); break;
                case kHIDUsage_PID_NegativeCoefficient: sprintf (cstrName, "Condition Negative Coefficient"); break;
                case kHIDUsage_PID_PositiveSaturation: sprintf (cstrName, "Condition Positive Saturation"); break;
                case kHIDUsage_PID_NegativeSaturation: sprintf (cstrName, "Condition Negative Saturation"); break;
                case kHIDUsage_PID_DeadBand: sprintf (cstrName, "Condition Dead Band"); break;

                case kHIDUsage_PID_DownloadForceSample: sprintf (cstrName, "Download Force Sample"); break;
                case kHIDUsage_PID_IsochCustomForceEnable: sprintf (cstrName, "Isoch Custom Force Enable"); break;

                case kHIDUsage_PID_CustomForceDataReport: sprintf (cstrName, "Custom Force Data Report"); break;
                case kHIDUsage_PID_CustomForceData: sprintf (cstrName, "Custom Force Data"); break;

                case kHIDUsage_PID_CustomForceVendorDefinedData: sprintf (cstrName, "Custom Force Vendor Defined Data"); break;
                case kHIDUsage_PID_SetCustomForceReport: sprintf (cstrName, "Set Custom Force Report"); break;
                case kHIDUsage_PID_CustomForceDataOffset: sprintf (cstrName, "Custom Force Data Offset"); break;
                case kHIDUsage_PID_SampleCount: sprintf (cstrName, "Custom Force Sample Count"); break;

                case kHIDUsage_PID_SetPeriodicReport: sprintf (cstrName, "Set Periodic Report"); break;
                case kHIDUsage_PID_Offset: sprintf (cstrName, "Periodic Offset"); break;
                case kHIDUsage_PID_Magnitude: sprintf (cstrName, "Periodic Magnitude"); break;
                case kHIDUsage_PID_Phase: sprintf (cstrName, "Periodic Phase"); break;
                case kHIDUsage_PID_Period: sprintf (cstrName, "Periodic Period"); break;

                case kHIDUsage_PID_SetConstantForceReport: sprintf (cstrName, "Set Constant Force Report"); break;

                case kHIDUsage_PID_SetRampForceReport: sprintf (cstrName, "Set Ramp Force Report"); break;
                case kHIDUsage_PID_RampStart: sprintf (cstrName, "Ramp Start"); break;
                case kHIDUsage_PID_RampEnd: sprintf (cstrName, "Ramp End"); break;

                case kHIDUsage_PID_EffectOperationReport: sprintf (cstrName, "Effect Operation Report"); break;

                case kHIDUsage_PID_EffectOperation: sprintf (cstrName, "Effect Operation"); break;
                case kHIDUsage_PID_OpEffectStart: sprintf (cstrName, "Op Effect Start"); break;
                case kHIDUsage_PID_OpEffectStartSolo: sprintf (cstrName, "Op Effect Start Solo"); break;
                case kHIDUsage_PID_OpEffectStop: sprintf (cstrName, "Op Effect Stop"); break;
                case kHIDUsage_PID_LoopCount: sprintf (cstrName, "Op Effect Loop Count"); break;

                case kHIDUsage_PID_DeviceGainReport: sprintf (cstrName, "Device Gain Report"); break;
                case kHIDUsage_PID_DeviceGain: sprintf (cstrName, "Device Gain"); break;

                case kHIDUsage_PID_PoolReport: sprintf (cstrName, "PID Pool Report"); break;
                case kHIDUsage_PID_RAM_PoolSize: sprintf (cstrName, "RAM Pool Size"); break;
                case kHIDUsage_PID_ROM_PoolSize: sprintf (cstrName, "ROM Pool Size"); break;
                case kHIDUsage_PID_ROM_EffectBlockCount: sprintf (cstrName, "ROM Effect Block Count"); break;
                case kHIDUsage_PID_SimultaneousEffectsMax: sprintf (cstrName, "Simultaneous Effects Max"); break;
                case kHIDUsage_PID_PoolAlignment: sprintf (cstrName, "Pool Alignment"); break;

                case kHIDUsage_PID_PoolMoveReport: sprintf (cstrName, "PID Pool Move Report"); break;
                case kHIDUsage_PID_MoveSource: sprintf (cstrName, "Move Source"); break;
                case kHIDUsage_PID_MoveDestination: sprintf (cstrName, "Move Destination"); break;
                case kHIDUsage_PID_MoveLength: sprintf (cstrName, "Move Length"); break;

                case kHIDUsage_PID_BlockLoadReport: sprintf (cstrName, "PID Block Load Report"); break;

                case kHIDUsage_PID_BlockLoadStatus: sprintf (cstrName, "Block Load Status"); break;
                case kHIDUsage_PID_BlockLoadSuccess: sprintf (cstrName, "Block Load Success"); break;
                case kHIDUsage_PID_BlockLoadFull: sprintf (cstrName, "Block Load Full"); break;
                case kHIDUsage_PID_BlockLoadError: sprintf (cstrName, "Block Load Error"); break;
                case kHIDUsage_PID_BlockHandle: sprintf (cstrName, "Block Handle"); break;

                case kHIDUsage_PID_BlockFreeReport: sprintf (cstrName, "PID Block Free Report"); break;

                case kHIDUsage_PID_TypeSpecificBlockHandle: sprintf (cstrName, "Type Specific Block Handle"); break;

                case kHIDUsage_PID_StateReport: sprintf (cstrName, "PID State Report"); break;
                case kHIDUsage_PID_EffectPlaying: sprintf (cstrName, "Effect Playing"); break;

                case kHIDUsage_PID_DeviceControlReport: sprintf (cstrName, "PID Device Control Report"); break;

                case kHIDUsage_PID_DeviceControl: sprintf (cstrName, "PID Device Control"); break;
                case kHIDUsage_PID_DC_EnableActuators: sprintf (cstrName, "Device Control Enable Actuators"); break;
                case kHIDUsage_PID_DC_DisableActuators: sprintf (cstrName, "Device Control Disable Actuators"); break;
                case kHIDUsage_PID_DC_StopAllEffects: sprintf (cstrName, "Device Control Stop All Effects"); break;
                case kHIDUsage_PID_DC_DeviceReset: sprintf (cstrName, "Device Control Reset"); break;
                case kHIDUsage_PID_DC_DevicePause: sprintf (cstrName, "Device Control Pause"); break;
                case kHIDUsage_PID_DC_DeviceContinue: sprintf (cstrName, "Device Control Continue"); break;
                case kHIDUsage_PID_DevicePaused: sprintf (cstrName, "Device Paused"); break;
                case kHIDUsage_PID_ActuatorsEnabled: sprintf (cstrName, "Actuators Enabled"); break;
                case kHIDUsage_PID_SafetySwitch: sprintf (cstrName, "Safety Switch"); break;
                case kHIDUsage_PID_ActuatorOverrideSwitch: sprintf (cstrName, "Actuator Override Switch"); break;
                case kHIDUsage_PID_ActuatorPower: sprintf (cstrName, "Actuator Power"); break;
                case kHIDUsage_PID_StartDelay: sprintf (cstrName, "Start Delay"); break;

                case kHIDUsage_PID_ParameterBlockSize: sprintf (cstrName, "Parameter Block Size"); break;
                case kHIDUsage_PID_DeviceManagedPool: sprintf (cstrName, "Device Managed Pool"); break;
                case kHIDUsage_PID_SharedParameterBlocks: sprintf (cstrName, "Shared Parameter Blocks"); break;

                case kHIDUsage_PID_CreateNewEffectReport: sprintf (cstrName, "Create New Effect Report"); break;
                case kHIDUsage_PID_RAM_PoolAvailable: sprintf (cstrName, "RAM Pool Available"); break;
                default: sprintf (cstrName, "PID Usage 0x%lx", valueUsage); break;
            }
            break;
        case kHIDPage_Unicode:
            switch (valueUsage)
        {
            default: sprintf (cstrName, "Unicode Usage 0x%lx", valueUsage); break;
        }
            break;
        case kHIDPage_PowerDevice:
			if (((valueUsage >= 0x06) && (valueUsage <= 0x0F)) || ((valueUsage >= 0x26) && (valueUsage <= 0x2F)) ||
                ((valueUsage >= 0x39) && (valueUsage <= 0x3F)) || ((valueUsage >= 0x48) && (valueUsage <= 0x4F)) ||
                ((valueUsage >= 0x58) && (valueUsage <= 0x5F)) || (valueUsage == 0x6A) ||
                ((valueUsage >= 0x74) && (valueUsage <= 0xFC)))
                sprintf (cstrName, "Power Device Reserved");
			else
				switch (valueUsage)
            {
                case kHIDUsage_PD_Undefined: sprintf (cstrName, "Power Device Undefined Usage"); break;
                case kHIDUsage_PD_iName: sprintf (cstrName, "Power Device Name Index"); break;
                case kHIDUsage_PD_PresentStatus: sprintf (cstrName, "Power Device Present Status"); break;
                case kHIDUsage_PD_ChangedStatus: sprintf (cstrName, "Power Device Changed Status"); break;
                case kHIDUsage_PD_UPS: sprintf (cstrName, "Uninterruptible Power Supply"); break;
                case kHIDUsage_PD_PowerSupply: sprintf (cstrName, "Power Supply"); break;

                case kHIDUsage_PD_BatterySystem: sprintf (cstrName, "Battery System Power Module"); break;
                case kHIDUsage_PD_BatterySystemID: sprintf (cstrName, "Battery System ID"); break;
                case kHIDUsage_PD_Battery: sprintf (cstrName, "Battery"); break;
                case kHIDUsage_PD_BatteryID: sprintf (cstrName, "Battery ID"); break;
                case kHIDUsage_PD_Charger: sprintf (cstrName, "Charger"); break;
                case kHIDUsage_PD_ChargerID: sprintf (cstrName, "Charger ID"); break;
                case kHIDUsage_PD_PowerConverter: sprintf (cstrName, "Power Converter Power Module"); break;
                case kHIDUsage_PD_PowerConverterID: sprintf (cstrName, "Power Converter ID"); break;
                case kHIDUsage_PD_OutletSystem: sprintf (cstrName, "Outlet System power module"); break;
                case kHIDUsage_PD_OutletSystemID: sprintf (cstrName, "Outlet System ID"); break;
                case kHIDUsage_PD_Input: sprintf (cstrName, "Power Device Input"); break;
                case kHIDUsage_PD_InputID: sprintf (cstrName, "Power Device Input ID"); break;
                case kHIDUsage_PD_Output: sprintf (cstrName, "Power Device Output"); break;
                case kHIDUsage_PD_OutputID: sprintf (cstrName, "Power Device Output ID"); break;
                case kHIDUsage_PD_Flow: sprintf (cstrName, "Power Device Flow"); break;
                case kHIDUsage_PD_FlowID: sprintf (cstrName, "Power Device Flow ID"); break;
                case kHIDUsage_PD_Outlet: sprintf (cstrName, "Power Device Outlet"); break;
                case kHIDUsage_PD_OutletID: sprintf (cstrName, "Power Device Outlet ID"); break;
                case kHIDUsage_PD_Gang: sprintf (cstrName, "Power Device Gang"); break;
                case kHIDUsage_PD_GangID: sprintf (cstrName, "Power Device Gang ID"); break;
                case kHIDUsage_PD_PowerSummary: sprintf (cstrName, "Power Device Power Summary"); break;
                case kHIDUsage_PD_PowerSummaryID: sprintf (cstrName, "Power Device Power Summary ID"); break;

                case kHIDUsage_PD_Voltage: sprintf (cstrName, "Power Device Voltage"); break;
                case kHIDUsage_PD_Current: sprintf (cstrName, "Power Device Current"); break;
                case kHIDUsage_PD_Frequency: sprintf (cstrName, "Power Device Frequency"); break;
                case kHIDUsage_PD_ApparentPower: sprintf (cstrName, "Power Device Apparent Power"); break;
                case kHIDUsage_PD_ActivePower: sprintf (cstrName, "Power Device RMS Power"); break;
                case kHIDUsage_PD_PercentLoad: sprintf (cstrName, "Power Device Percent Load"); break;
                case kHIDUsage_PD_Temperature: sprintf (cstrName, "Power Device Temperature"); break;
                case kHIDUsage_PD_Humidity: sprintf (cstrName, "Power Device Humidity"); break;
                case kHIDUsage_PD_BadCount: sprintf (cstrName, "Power Device Bad Condition Count"); break;

                case kHIDUsage_PD_ConfigVoltage: sprintf (cstrName, "Power Device Nominal Voltage"); break;
                case kHIDUsage_PD_ConfigCurrent: sprintf (cstrName, "Power Device Nominal Current"); break;
                case kHIDUsage_PD_ConfigFrequency: sprintf (cstrName, "Power Device Nominal Frequency"); break;
                case kHIDUsage_PD_ConfigApparentPower: sprintf (cstrName, "Power Device Nominal Apparent Power"); break;
                case kHIDUsage_PD_ConfigActivePower: sprintf (cstrName, "Power Device Nominal RMS Power"); break;
                case kHIDUsage_PD_ConfigPercentLoad: sprintf (cstrName, "Power Device Nominal Percent Load"); break;
                case kHIDUsage_PD_ConfigTemperature: sprintf (cstrName, "Power Device Nominal Temperature"); break;

                case kHIDUsage_PD_ConfigHumidity: sprintf (cstrName, "Power Device Nominal Humidity"); break;
                case kHIDUsage_PD_SwitchOnControl: sprintf (cstrName, "Power Device Switch On Control"); break;
                case kHIDUsage_PD_SwitchOffControl: sprintf (cstrName, "Power Device Switch Off Control"); break;
                case kHIDUsage_PD_ToggleControl: sprintf (cstrName, "Power Device Toogle Sequence Control"); break;
                case kHIDUsage_PD_LowVoltageTransfer: sprintf (cstrName, "Power Device Min Transfer Voltage"); break;
                case kHIDUsage_PD_HighVoltageTransfer: sprintf (cstrName, "Power Device Max Transfer Voltage"); break;
                case kHIDUsage_PD_DelayBeforeReboot: sprintf (cstrName, "Power Device Delay Before Reboot"); break;
                case kHIDUsage_PD_DelayBeforeStartup: sprintf (cstrName, "Power Device Delay Before Startup"); break;
                case kHIDUsage_PD_DelayBeforeShutdown: sprintf (cstrName, "Power Device Delay Before Shutdown"); break;
                case kHIDUsage_PD_Test: sprintf (cstrName, "Power Device Test Request/Result"); break;
                case kHIDUsage_PD_ModuleReset: sprintf (cstrName, "Power Device Reset Request/Result"); break;
                case kHIDUsage_PD_AudibleAlarmControl: sprintf (cstrName, "Power Device Audible Alarm Control"); break;

                case kHIDUsage_PD_Present: sprintf (cstrName, "Power Device Present"); break;
                case kHIDUsage_PD_Good: sprintf (cstrName, "Power Device Good"); break;
                case kHIDUsage_PD_InternalFailure: sprintf (cstrName, "Power Device Internal Failure"); break;
                case kHIDUsage_PD_VoltageOutOfRange: sprintf (cstrName, "Power Device Voltage Out Of Range"); break;
                case kHIDUsage_PD_FrequencyOutOfRange: sprintf (cstrName, "Power Device Frequency Out Of Range"); break;
                case kHIDUsage_PD_Overload: sprintf (cstrName, "Power Device Overload"); break;
                case kHIDUsage_PD_OverCharged: sprintf (cstrName, "Power Device Over Charged"); break;
                case kHIDUsage_PD_OverTemperature: sprintf (cstrName, "Power Device Over Temperature"); break;
                case kHIDUsage_PD_ShutdownRequested: sprintf (cstrName, "Power Device Shutdown Requested"); break;

                case kHIDUsage_PD_ShutdownImminent: sprintf (cstrName, "Power Device Shutdown Imminent"); break;
                case kHIDUsage_PD_SwitchOnOff: sprintf (cstrName, "Power Device On/Off Switch Status"); break;
                case kHIDUsage_PD_Switchable: sprintf (cstrName, "Power Device Switchable"); break;
                case kHIDUsage_PD_Used: sprintf (cstrName, "Power Device Used"); break;
                case kHIDUsage_PD_Boost: sprintf (cstrName, "Power Device Boosted"); break;
                case kHIDUsage_PD_Buck: sprintf (cstrName, "Power Device Bucked"); break;
                case kHIDUsage_PD_Initialized: sprintf (cstrName, "Power Device Initialized"); break;
                case kHIDUsage_PD_Tested: sprintf (cstrName, "Power Device Tested"); break;
                case kHIDUsage_PD_AwaitingPower: sprintf (cstrName, "Power Device Awaiting Power"); break;
                case kHIDUsage_PD_CommunicationLost: sprintf (cstrName, "Power Device Communication Lost"); break;

                case kHIDUsage_PD_iManufacturer: sprintf (cstrName, "Power Device Manufacturer String Index"); break;
                case kHIDUsage_PD_iProduct: sprintf (cstrName, "Power Device Product String Index"); break;
                case kHIDUsage_PD_iserialNumber: sprintf (cstrName, "Power Device Serial Number String Index"); break;
                default: sprintf (cstrName, "Power Device Usage 0x%lx", valueUsage); break;
            }
            break;
        case kHIDPage_BatterySystem:
			if (((valueUsage >= 0x0A) && (valueUsage <= 0x0F)) || ((valueUsage >= 0x1E) && (valueUsage <= 0x27)) ||
                ((valueUsage >= 0x30) && (valueUsage <= 0x3F)) || ((valueUsage >= 0x4C) && (valueUsage <= 0x5F)) ||
                ((valueUsage >= 0x6C) && (valueUsage <= 0x7F)) || ((valueUsage >= 0x90) && (valueUsage <= 0xBF)) ||
                ((valueUsage >= 0xC3) && (valueUsage <= 0xCF)) || ((valueUsage >= 0xDD) && (valueUsage <= 0xEF)) ||
                ((valueUsage >= 0xF2) && (valueUsage <= 0xFF)))
                sprintf (cstrName, "Power Device Reserved");
			else
				switch (valueUsage)
            {
                case kHIDUsage_BS_Undefined: sprintf (cstrName, "Battery System Undefined"); break;
                case kHIDUsage_BS_SMBBatteryMode: sprintf (cstrName, "SMB Mode"); break;
                case kHIDUsage_BS_SMBBatteryStatus: sprintf (cstrName, "SMB Status"); break;
                case kHIDUsage_BS_SMBAlarmWarning: sprintf (cstrName, "SMB Alarm Warning"); break;
                case kHIDUsage_BS_SMBChargerMode: sprintf (cstrName, "SMB Charger Mode"); break;
                case kHIDUsage_BS_SMBChargerStatus: sprintf (cstrName, "SMB Charger Status"); break;
                case kHIDUsage_BS_SMBChargerSpecInfo: sprintf (cstrName, "SMB Charger Extended Status"); break;
                case kHIDUsage_BS_SMBSelectorState: sprintf (cstrName, "SMB Selector State"); break;
                case kHIDUsage_BS_SMBSelectorPresets: sprintf (cstrName, "SMB Selector Presets"); break;
                case kHIDUsage_BS_SMBSelectorInfo: sprintf (cstrName, "SMB Selector Info"); break;
                case kHIDUsage_BS_OptionalMfgFunction1: sprintf (cstrName, "Battery System Optional SMB Mfg Function 1"); break;
                case kHIDUsage_BS_OptionalMfgFunction2: sprintf (cstrName, "Battery System Optional SMB Mfg Function 2"); break;
                case kHIDUsage_BS_OptionalMfgFunction3: sprintf (cstrName, "Battery System Optional SMB Mfg Function 3"); break;
                case kHIDUsage_BS_OptionalMfgFunction4: sprintf (cstrName, "Battery System Optional SMB Mfg Function 4"); break;
                case kHIDUsage_BS_OptionalMfgFunction5: sprintf (cstrName, "Battery System Optional SMB Mfg Function 5"); break;
                case kHIDUsage_BS_ConnectionToSMBus: sprintf (cstrName, "Battery System Connection To System Management Bus"); break;
                case kHIDUsage_BS_OutputConnection: sprintf (cstrName, "Battery System Output Connection Status"); break;
                case kHIDUsage_BS_ChargerConnection: sprintf (cstrName, "Battery System Charger Connection"); break;
                case kHIDUsage_BS_BatteryInsertion: sprintf (cstrName, "Battery System Battery Insertion"); break;
                case kHIDUsage_BS_Usenext: sprintf (cstrName, "Battery System Use Next"); break;
                case kHIDUsage_BS_OKToUse: sprintf (cstrName, "Battery System OK To Use"); break;
                case kHIDUsage_BS_BatterySupported: sprintf (cstrName, "Battery System Battery Supported"); break;
                case kHIDUsage_BS_SelectorRevision: sprintf (cstrName, "Battery System Selector Revision"); break;
                case kHIDUsage_BS_ChargingIndicator: sprintf (cstrName, "Battery System Charging Indicator"); break;
                case kHIDUsage_BS_ManufacturerAccess: sprintf (cstrName, "Battery System Manufacturer Access"); break;
                case kHIDUsage_BS_RemainingCapacityLimit: sprintf (cstrName, "Battery System Remaining Capacity Limit"); break;
                case kHIDUsage_BS_RemainingTimeLimit: sprintf (cstrName, "Battery System Remaining Time Limit"); break;
                case kHIDUsage_BS_AtRate: sprintf (cstrName, "Battery System At Rate..."); break;
                case kHIDUsage_BS_CapacityMode: sprintf (cstrName, "Battery System Capacity Mode"); break;
                case kHIDUsage_BS_BroadcastToCharger: sprintf (cstrName, "Battery System Broadcast To Charger"); break;
                case kHIDUsage_BS_PrimaryBattery: sprintf (cstrName, "Battery System Primary Battery"); break;
                case kHIDUsage_BS_ChargeController: sprintf (cstrName, "Battery System Charge Controller"); break;
                case kHIDUsage_BS_TerminateCharge: sprintf (cstrName, "Battery System Terminate Charge"); break;
                case kHIDUsage_BS_TerminateDischarge: sprintf (cstrName, "Battery System Terminate Discharge"); break;
                case kHIDUsage_BS_BelowRemainingCapacityLimit: sprintf (cstrName, "Battery System Below Remaining Capacity Limit"); break;
                case kHIDUsage_BS_RemainingTimeLimitExpired: sprintf (cstrName, "Battery System Remaining Time Limit Expired"); break;
                case kHIDUsage_BS_Charging: sprintf (cstrName, "Battery System Charging"); break;
                case kHIDUsage_BS_Discharging: sprintf (cstrName, "Battery System Discharging"); break;
                case kHIDUsage_BS_FullyCharged: sprintf (cstrName, "Battery System Fully Charged"); break;
                case kHIDUsage_BS_FullyDischarged: sprintf (cstrName, "Battery System Fully Discharged"); break;
                case kHIDUsage_BS_ConditioningFlag: sprintf (cstrName, "Battery System Conditioning Flag"); break;
                case kHIDUsage_BS_AtRateOK: sprintf (cstrName, "Battery System At Rate OK"); break;
                case kHIDUsage_BS_SMBErrorCode: sprintf (cstrName, "Battery System SMB Error Code"); break;
                case kHIDUsage_BS_NeedReplacement: sprintf (cstrName, "Battery System Need Replacement"); break;
                case kHIDUsage_BS_AtRateTimeToFull: sprintf (cstrName, "Battery System At Rate Time To Full"); break;
                case kHIDUsage_BS_AtRateTimeToEmpty: sprintf (cstrName, "Battery System At Rate Time To Empty"); break;
                case kHIDUsage_BS_AverageCurrent: sprintf (cstrName, "Battery System Average Current"); break;
                case kHIDUsage_BS_Maxerror: sprintf (cstrName, "Battery System Max Error"); break;
                case kHIDUsage_BS_RelativeStateOfCharge: sprintf (cstrName, "Battery System Relative State Of Charge"); break;
                case kHIDUsage_BS_AbsoluteStateOfCharge: sprintf (cstrName, "Battery System Absolute State Of Charge"); break;
                case kHIDUsage_BS_RemainingCapacity: sprintf (cstrName, "Battery System Remaining Capacity"); break;
                case kHIDUsage_BS_FullChargeCapacity: sprintf (cstrName, "Battery System Full Charge Capacity"); break;
                case kHIDUsage_BS_RunTimeToEmpty: sprintf (cstrName, "Battery System Run Time To Empty"); break;
                case kHIDUsage_BS_AverageTimeToEmpty: sprintf (cstrName, "Battery System Average Time To Empty"); break;
                case kHIDUsage_BS_AverageTimeToFull: sprintf (cstrName, "Battery System Average Time To Full"); break;
                case kHIDUsage_BS_CycleCount: sprintf (cstrName, "Battery System Cycle Count"); break;
                case kHIDUsage_BS_BattPackModelLevel: sprintf (cstrName, "Battery System Batt Pack Model Level"); break;
                case kHIDUsage_BS_InternalChargeController: sprintf (cstrName, "Battery System Internal Charge Controller"); break;
                case kHIDUsage_BS_PrimaryBatterySupport: sprintf (cstrName, "Battery System Primary Battery Support"); break;
                case kHIDUsage_BS_DesignCapacity: sprintf (cstrName, "Battery System Design Capacity"); break;
                case kHIDUsage_BS_SpecificationInfo: sprintf (cstrName, "Battery System Specification Info"); break;
                case kHIDUsage_BS_ManufacturerDate: sprintf (cstrName, "Battery System Manufacturer Date"); break;
                case kHIDUsage_BS_SerialNumber: sprintf (cstrName, "Battery System Serial Number"); break;
                case kHIDUsage_BS_iManufacturerName: sprintf (cstrName, "Battery System Manufacturer Name Index"); break;
                case kHIDUsage_BS_iDevicename: sprintf (cstrName, "Battery System Device Name Index"); break;
                case kHIDUsage_BS_iDeviceChemistry: sprintf (cstrName, "Battery System Device Chemistry Index"); break;
                case kHIDUsage_BS_ManufacturerData: sprintf (cstrName, "Battery System Manufacturer Data"); break;
                case kHIDUsage_BS_Rechargable: sprintf (cstrName, "Battery System Rechargable"); break;
                case kHIDUsage_BS_WarningCapacityLimit: sprintf (cstrName, "Battery System Warning Capacity Limit"); break;
                case kHIDUsage_BS_CapacityGranularity1: sprintf (cstrName, "Battery System Capacity Granularity 1"); break;
                case kHIDUsage_BS_CapacityGranularity2: sprintf (cstrName, "Battery System Capacity Granularity 2"); break;
                case kHIDUsage_BS_iOEMInformation: sprintf (cstrName, "Battery System OEM Information Index"); break;
                case kHIDUsage_BS_InhibitCharge: sprintf (cstrName, "Battery System Inhibit Charge"); break;
                case kHIDUsage_BS_EnablePolling: sprintf (cstrName, "Battery System Enable Polling"); break;
                case kHIDUsage_BS_ResetToZero: sprintf (cstrName, "Battery System Reset To Zero"); break;
                case kHIDUsage_BS_ACPresent: sprintf (cstrName, "Battery System AC Present"); break;
                case kHIDUsage_BS_BatteryPresent: sprintf (cstrName, "Battery System Battery Present"); break;
                case kHIDUsage_BS_PowerFail: sprintf (cstrName, "Battery System Power Fail"); break;
                case kHIDUsage_BS_AlarmInhibited: sprintf (cstrName, "Battery System Alarm Inhibited"); break;
                case kHIDUsage_BS_ThermistorUnderRange: sprintf (cstrName, "Battery System Thermistor Under Range"); break;
                case kHIDUsage_BS_ThermistorHot: sprintf (cstrName, "Battery System Thermistor Hot"); break;
                case kHIDUsage_BS_ThermistorCold: sprintf (cstrName, "Battery System Thermistor Cold"); break;
                case kHIDUsage_BS_ThermistorOverRange: sprintf (cstrName, "Battery System Thermistor Over Range"); break;
                case kHIDUsage_BS_VoltageOutOfRange: sprintf (cstrName, "Battery System Voltage Out Of Range"); break;
                case kHIDUsage_BS_CurrentOutOfRange: sprintf (cstrName, "Battery System Current Out Of Range"); break;
                case kHIDUsage_BS_CurrentNotRegulated: sprintf (cstrName, "Battery System Current Not Regulated"); break;
                case kHIDUsage_BS_VoltageNotRegulated: sprintf (cstrName, "Battery System Voltage Not Regulated"); break;
                case kHIDUsage_BS_MasterMode: sprintf (cstrName, "Battery System Master Mode"); break;
                case kHIDUsage_BS_ChargerSelectorSupport: sprintf (cstrName, "Battery System Charger Support Selector"); break;
                case kHIDUsage_BS_ChargerSpec: sprintf (cstrName, "attery System Charger Specification"); break;
                case kHIDUsage_BS_Level2: sprintf (cstrName, "Battery System Charger Level 2"); break;
                case kHIDUsage_BS_Level3: sprintf (cstrName, "Battery System Charger Level 3"); break;
                default: sprintf (cstrName, "Battery System Usage 0x%lx", valueUsage); break;
            }
            break;
        case kHIDPage_AlphanumericDisplay:
            switch (valueUsage)
        {
            default: sprintf (cstrName, "Alphanumeric Display Usage 0x%lx", valueUsage); break;
        }
            break;
		case kHIDPage_BarCodeScanner:
            switch (valueUsage)
        {
            default: sprintf (cstrName, "Bar Code Scanner Usage 0x%lx", valueUsage); break;
        }
            break;
        case kHIDPage_Scale:
            switch (valueUsage)
        {
            default: sprintf (cstrName, "Scale Usage 0x%lx", valueUsage); break;
        }
            break;
        case kHIDPage_CameraControl:
            switch (valueUsage)
        {
            default: sprintf (cstrName, "Camera Control Usage 0x%lx", valueUsage); break;
        }
            break;
        case kHIDPage_Arcade:
            switch (valueUsage)
        {
            default: sprintf (cstrName, "Arcade Usage 0x%lx", valueUsage); break;
        }
            break;
        default:
			if (valueUsagePage > kHIDPage_VendorDefinedStart)
				sprintf (cstrName, "Vendor Defined Usage 0x%lx", valueUsage);
			else
				sprintf (cstrName, "Page: 0x%lx, Usage: 0x%lx", valueUsagePage, valueUsage);
            break;
    }
}

//*************************************************************************
//
// HIDCopyUsageName( inUsagePage, inUsage )
//
// Purpose:	return a CFStringRef string for a given usage page & usage( see IOUSBHIDParser.h )
//
// Notes:	returns usage page and usage values in CFString form for unknown values
//
// Inputs:	inUsagePage	- the usage page
//			inUsage		- the usage
//
// Returns:	CFStringRef	- the resultant string
//

CFStringRef HIDCopyUsageName( long inUsagePage, long inUsage )
{
	static CFPropertyListRef tCFPropertyListRef = NULL;
	CFStringRef result = NULL;

	if ( !tCFPropertyListRef ) {
		tCFPropertyListRef = hu_XMLLoad( CFSTR( "HID_usage_strings" ), CFSTR( "plist" ) );
	}

	if ( tCFPropertyListRef ) {
		if ( CFDictionaryGetTypeID( ) == CFGetTypeID( tCFPropertyListRef ) ) {
			CFStringRef pageKeyCFStringRef = CFStringCreateWithFormat( kCFAllocatorDefault, NULL, CFSTR( "0x%4.4lX" ), inUsagePage );

			if ( pageKeyCFStringRef ) {
				CFDictionaryRef pageCFDictionaryRef;

				if ( CFDictionaryGetValueIfPresent( tCFPropertyListRef, pageKeyCFStringRef, ( const void** ) &pageCFDictionaryRef ) ) {
					CFStringRef pageCFStringRef;

					if ( CFDictionaryGetValueIfPresent( pageCFDictionaryRef, kNameKeyCFStringRef, ( const void** ) &pageCFStringRef ) ) {
						CFStringRef usageKeyCFStringRef = CFStringCreateWithFormat( kCFAllocatorDefault, NULL, CFSTR( "0x%4.4lX" ), inUsage );

						if ( usageKeyCFStringRef ) {
							CFStringRef usageCFStringRef;

							if ( CFDictionaryGetValueIfPresent( pageCFDictionaryRef, usageKeyCFStringRef, ( const void** ) &usageCFStringRef ) ) {
								result = CFStringCreateWithFormat( kCFAllocatorDefault, NULL, CFSTR( "%@ %@" ), pageCFStringRef, usageCFStringRef );
							}
#if FAKE_MISSING_NAMES
							else {
								result = CFStringCreateWithFormat( kCFAllocatorDefault, NULL, CFSTR( "%@ #%@" ), pageCFStringRef, usageKeyCFStringRef );
							}
#endif
							CFRelease( usageKeyCFStringRef );
						}
					} else {
						// no name data for this page
					}
				} else {
					// no data for this page
				}
				CFRelease( pageKeyCFStringRef );
			}
		}
		// CFRelease( tCFPropertyListRef );	// Leak this!
		// tCFPropertyListRef = NULL;
	}
	return result;
}   // HIDCopyUsageName



//*****************************************************
#pragma mark - local ( static ) function implementations
//*****************************************************
#if 0	// currently unused
/*************************************************************************
 *
 * hu_SaveToXMLFile( inCFPRef, inCFURLRef )
 *
 * Purpose: save a property list into an XML file
 *
 * Inputs: inCFPRef		- the data
 *			inCFURLRef	- URL for the file
 *
 * Returns: SInt32		- error code ( if any )
 */
static SInt32 hu_SaveToXMLFile( CFPropertyListRef inCFPRef, CFURLRef inCFURLRef )
{
	CFDataRef xmlCFDataRef;
	SInt32 error = coreFoundationUnknownErr;

	// Convert the property list into XML data.
	xmlCFDataRef = CFPropertyListCreateXMLData( kCFAllocatorDefault, inCFPRef );
	if ( xmlCFDataRef ) {
		// Write the XML data to the file.
		( void ) CFURLWriteDataAndPropertiesToResource( inCFURLRef, xmlCFDataRef, NULL, &error );

		// Release the XML data
		CFRelease( xmlCFDataRef );
	}
	return error;
}	// hu_SaveToXMLFile
#endif
/*************************************************************************
 *
 * hu_LoadFromXMLFile( inCFURLRef )
 *
 * Purpose: load a property list from an XML file
 *
 * Inputs: inCFURLRef			- URL for the file
 *
 * Returns: CFPropertyListRef - the data
 */
static CFPropertyListRef hu_LoadFromXMLFile( CFURLRef inCFURLRef )
{
	CFDataRef xmlCFDataRef;
	CFPropertyListRef myCFPropertyListRef = NULL;

	// Read the XML file.
	SInt32 error;
	if ( CFURLCreateDataAndPropertiesFromResource( kCFAllocatorDefault, inCFURLRef, & xmlCFDataRef, NULL, NULL, & error ) ) {
		CFStringRef errorString;
		// Reconstitute the dictionary using the XML data.
		myCFPropertyListRef = CFPropertyListCreateFromXMLData( kCFAllocatorDefault, xmlCFDataRef, kCFPropertyListImmutable, & errorString );
		// Release the XML data
		CFRelease( xmlCFDataRef );
	}
	return myCFPropertyListRef;
}	// hu_LoadFromXMLFile

#if 0	// currently unused
/*************************************************************************
 *
 * hu_XMLSave( inCFPropertyListRef, inResourceName, inResourceExtension )
 *
 * Purpose: save a CFPropertyListRef into a resource( XML ) file
 *
 * Inputs: inCFPropertyListRef - the data
 *			inResourceName		- name of the resource file
 *			inResourceExtension - extension of the resource file
 *
 * Returns: SInt32				- error code ( if any )
 */
static SInt32 hu_XMLSave( CFPropertyListRef inCFPropertyListRef, CFStringRef inResourceName, CFStringRef inResourceExtension )
{
	CFURLRef resFileCFURLRef;
	SInt32 error = - 1;

	resFileCFURLRef = CFBundleCopyResourceURL( CFBundleGetMainBundle( ), inResourceName, inResourceExtension, NULL );
	if ( resFileCFURLRef ) {
		error = hu_SaveToXMLFile( inCFPropertyListRef, resFileCFURLRef );
		CFRelease( resFileCFURLRef );
	}
	return error;
}	// hu_XMLSave
#endif
/*************************************************************************
 *
 * hu_XMLLoad( inResourceName, inResourceExtension )
 *
 * Purpose: Load a resource( XML ) file into a CFPropertyListRef
 *
 * Inputs: inResourceName		- name of the resource file
 *			inResourceExtension - extension of the resource file
 *
 * Returns: CFPropertyListRef - the data
 */
static CFPropertyListRef hu_XMLLoad( CFStringRef inResourceName, CFStringRef inResourceExtension )
{
	CFURLRef resFileCFURLRef;
	CFPropertyListRef tCFPropertyListRef = NULL;

	resFileCFURLRef = CFBundleCopyResourceURL( CFBundleGetMainBundle( ), inResourceName, inResourceExtension, NULL );
	if ( resFileCFURLRef ) {
		tCFPropertyListRef = hu_LoadFromXMLFile( resFileCFURLRef );
		CFRelease( resFileCFURLRef );
	}
	return tCFPropertyListRef;
}	// hu_XMLLoad

/*************************************************************************
 *
 * hu_XMLSearchForVendorNameByVendorID( inVendorID, outCStr )
 *
 * Purpose: Find a vendor string in the <HID_device_usage_strings.plist> resource ( XML ) file
 *
 * Inputs: inVendorID - the elements vendor ID
 *			inProductID - the elements product ID
 *			outCStr		- address where result will be returned
 *
 * Returns: Boolean		- if successful
 */
static Boolean hu_XMLSearchForVendorNameByVendorID( long inVendorID, char * outCStr )
{
	Boolean results = FALSE;

	if ( ! gUsageCFPropertyListRef )
		gUsageCFPropertyListRef = hu_XMLLoad( CFSTR( "HID_device_usage_strings" ), CFSTR( "plist" ) );

	if ( gUsageCFPropertyListRef ) {
		if ( CFDictionaryGetTypeID( ) == CFGetTypeID( gUsageCFPropertyListRef ) ) {
			CFDictionaryRef vendorCFDictionaryRef;
			CFStringRef	vendorKeyCFStringRef = CFStringCreateWithFormat( kCFAllocatorDefault, NULL, CFSTR( "%ld" ), inVendorID );
			if ( vendorKeyCFStringRef ) {
				if ( CFDictionaryGetValueIfPresent( gUsageCFPropertyListRef, vendorKeyCFStringRef, ( const void ** ) & vendorCFDictionaryRef ) ) {
					CFStringRef	vendorCFStringRef = NULL;
					if ( CFDictionaryGetValueIfPresent( vendorCFDictionaryRef, kNameKeyCFStringRef, ( const void ** ) & vendorCFStringRef ) && vendorCFStringRef ) {
						// CFShow( vendorCFStringRef );
						results = CFStringGetCString( vendorCFStringRef, outCStr, CFStringGetLength( vendorCFStringRef ) * sizeof( UniChar ) + 1, kCFStringEncodingUTF8 );
					}
				}
				CFRelease( vendorKeyCFStringRef );
			}
		}
		// ++ CFRelease( gUsageCFPropertyListRef );	// Leak this !
	}
	return results;
}	// hu_XMLSearchForVendorNameByVendorID

/*************************************************************************
 *
 * hu_XMLSearchForProductNameByVendorProductID( inVendorID, inProductID, outCStr )
 *
 * Purpose: Find an product string in the <HID_device_usage_strings.plist> resource ( XML ) file
 *
 * Inputs: inVendorID - the elements vendor ID
 *			inProductID - the elements product ID
 *			outCStr		- address where result will be returned
 *
 * Returns: Boolean		- if successful
 */
static Boolean hu_XMLSearchForProductNameByVendorProductID( long inVendorID, long inProductID, char * outCStr )
{
	Boolean results = FALSE;

	if ( ! gUsageCFPropertyListRef )
		gUsageCFPropertyListRef = hu_XMLLoad( CFSTR( "HID_device_usage_strings" ), CFSTR( "plist" ) );

	if ( gUsageCFPropertyListRef ) {
		if ( CFDictionaryGetTypeID( ) == CFGetTypeID( gUsageCFPropertyListRef ) ) {
			// first we make our vendor ID key
			CFStringRef	vendorKeyCFStringRef = CFStringCreateWithFormat( kCFAllocatorDefault, NULL, CFSTR( "%ld" ), inVendorID );
			if ( vendorKeyCFStringRef ) {
				// and use it to look up our vendor dictionary
				CFDictionaryRef vendorCFDictionaryRef;
				if ( CFDictionaryGetValueIfPresent( gUsageCFPropertyListRef, vendorKeyCFStringRef, ( const void ** ) & vendorCFDictionaryRef ) ) {
					// pull our vendor name our of that dictionary
					CFStringRef	vendorCFStringRef = NULL;
					if ( CFDictionaryGetValueIfPresent( vendorCFDictionaryRef, kNameKeyCFStringRef, ( const void ** ) & vendorCFStringRef ) ) {
#if FAKE_MISSING_NAMES
						CFRetain( vendorCFStringRef );	// so we can CFRelease it later
					} else {
						vendorCFStringRef = CFStringCreateWithFormat( kCFAllocatorDefault, NULL, CFSTR( "V: %@" ), vendorKeyCFStringRef );
#endif
					}

					// now we make our product ID key
					CFStringRef	productKeyCFStringRef = CFStringCreateWithFormat( kCFAllocatorDefault, NULL, CFSTR( "%ld" ), inProductID );
					if ( productKeyCFStringRef ) {
						// and use that key to look up our product dictionary in the vendor dictionary
						CFDictionaryRef productCFDictionaryRef;
						if ( CFDictionaryGetValueIfPresent( vendorCFDictionaryRef, productKeyCFStringRef, ( const void ** ) & productCFDictionaryRef ) ) {
							// pull our product name our of the product dictionary
							CFStringRef	productCFStringRef = NULL;
							if ( CFDictionaryGetValueIfPresent( productCFDictionaryRef, kNameKeyCFStringRef, ( const void ** ) & productCFStringRef ) ) {
#if FAKE_MISSING_NAMES
								CFRetain( productCFStringRef );	// so we can CFRelease it later
							} else {
								productCFStringRef = CFStringCreateWithFormat( kCFAllocatorDefault, NULL, CFSTR( "P: %@" ), kNameKeyCFStringRef );
#endif
							}
							CFStringRef fullCFStringRef = CFStringCreateWithFormat( kCFAllocatorDefault, NULL, CFSTR( "%@ %@" ), vendorCFStringRef, productCFStringRef );
							if ( fullCFStringRef ) {
								// CFShow( fullCFStringRef );
								results = CFStringGetCString( fullCFStringRef, outCStr, CFStringGetLength( fullCFStringRef ) * sizeof( UniChar ) + 1, kCFStringEncodingUTF8 );
								CFRelease( fullCFStringRef );
							}
#if FAKE_MISSING_NAMES
							if ( productCFStringRef ) {
								CFRelease( productCFStringRef );
							}
#endif
						}
						CFRelease( productKeyCFStringRef );
					}

#if FAKE_MISSING_NAMES
					if ( vendorCFStringRef ) {
						CFRelease( vendorCFStringRef );
					}
#endif
				}
				CFRelease( vendorKeyCFStringRef );
			}
		}
		// ++ CFRelease( gUsageCFPropertyListRef );	// Leak this !
	}
	return results;
}	// hu_XMLSearchForProductNameByVendorProductID

/*************************************************************************
 *
 * hu_XMLSearchForElementNameByCookie( inVendorID, inProductID, inCookie, outCStr )
 *
 * Purpose: Find an element string in the <HID_cookie_strings.plist> resource( XML ) file
 *
 * Inputs: inVendorID - the elements vendor ID
 *			inProductID - the elements product ID
 *			inCookie		- the elements cookie
 *			outCStr		- address where result will be returned
 *
 * Returns: Boolean		- if successful
 */
static Boolean hu_XMLSearchForElementNameByCookie( long inVendorID, long inProductID, IOHIDElementCookie inCookie, char * outCStr )
{
	Boolean results = FALSE;

	if ( ! gCookieCFPropertyListRef )
		gCookieCFPropertyListRef = hu_XMLLoad( CFSTR( "HID_cookie_strings" ), CFSTR( "plist" ) );

	if ( gCookieCFPropertyListRef ) {
		if ( CFDictionaryGetTypeID( ) == CFGetTypeID( gCookieCFPropertyListRef ) ) {
			CFStringRef	vendorKeyCFStringRef = CFStringCreateWithFormat( kCFAllocatorDefault, NULL, CFSTR( "%ld" ), inVendorID );
			if ( vendorKeyCFStringRef ) {
				CFDictionaryRef vendorCFDictionaryRef;
				if ( CFDictionaryGetValueIfPresent( gCookieCFPropertyListRef, vendorKeyCFStringRef, ( const void ** ) & vendorCFDictionaryRef ) ) {
					CFDictionaryRef productCFDictionaryRef;
					CFStringRef	productKeyCFStringRef;
					CFStringRef	vendorCFStringRef;

					if ( CFDictionaryGetValueIfPresent( vendorCFDictionaryRef, kNameKeyCFStringRef, ( const void ** ) & vendorCFStringRef ) ) {
						// CFShow( vendorCFStringRef );
					}
					productKeyCFStringRef = CFStringCreateWithFormat( kCFAllocatorDefault, NULL, CFSTR( "%ld" ), inProductID );

					if ( CFDictionaryGetValueIfPresent( vendorCFDictionaryRef, productKeyCFStringRef, ( const void ** ) & productCFDictionaryRef ) ) {
						CFStringRef fullCFStringRef = NULL;
						CFStringRef	cookieKeyCFStringRef;
						CFStringRef	productCFStringRef;
						CFStringRef	cookieCFStringRef;

						if ( CFDictionaryGetValueIfPresent( productCFDictionaryRef, kNameKeyCFStringRef, ( const void ** ) & productCFStringRef ) ) {
							// CFShow( productCFStringRef );
						}
						cookieKeyCFStringRef = CFStringCreateWithFormat( kCFAllocatorDefault, NULL, CFSTR( "%ld" ), inCookie );

						if ( CFDictionaryGetValueIfPresent( productCFDictionaryRef, cookieKeyCFStringRef, ( const void ** ) & cookieCFStringRef ) ) {
#if VERBOSE_ELEMENT_NAMES
							fullCFStringRef = CFStringCreateWithFormat( kCFAllocatorDefault, NULL, CFSTR( "%@ %@ %@" ), vendorCFStringRef, productCFStringRef, cookieCFStringRef );
#else
							fullCFStringRef = CFStringCreateWithFormat( kCFAllocatorDefault, NULL, CFSTR( "%@" ), cookieCFStringRef );
#endif VERBOSE_ELEMENT_NAMES
							// CFShow( cookieCFStringRef );
						}
#if FAKE_MISSING_NAMES
						else {
							fullCFStringRef = CFStringCreateWithFormat( kCFAllocatorDefault, NULL, CFSTR( "%@ %@ # %@" ), vendorCFStringRef, productCFStringRef, cookieKeyCFStringRef );
						}
#endif FAKE_MISSING_NAMES
						if ( fullCFStringRef ) {
							// CFShow( fullCFStringRef );
							results = CFStringGetCString( fullCFStringRef, outCStr, CFStringGetLength( fullCFStringRef ) * sizeof( UniChar ) + 1, kCFStringEncodingUTF8 );
							CFRelease( fullCFStringRef );
						}
						CFRelease( cookieKeyCFStringRef );
					}
					CFRelease( productKeyCFStringRef );
				}
				CFRelease( vendorKeyCFStringRef );
			}
		}
		// ++ CFRelease( gCookieCFPropertyListRef );	// Leak this !
	}
	return results;
}	// hu_XMLSearchForElementNameByCookie

/*************************************************************************
 *
 * hu_XMLSearchForElementNameByUsage( inVendorID, inProductID, inUsagePage, inUsage, outCStr )
 *
 * Purpose: Find an element string in the <HID_device_usage_strings.plist> resource( XML ) file
 *
 * Inputs: inVendorID - the elements vendor ID
 *			inProductID - the elements product ID
 *			inUsagePage	- the elements usage page
 *			inUsage		- the elements usage
 *			outCStr		- address where result will be returned
 *
 * Returns: Boolean		- if successful
 */
static Boolean hu_XMLSearchForElementNameByUsage( long inVendorID, long inProductID, long inUsagePage, long inUsage, char * outCStr )
{
	Boolean results = FALSE;

	if ( !gUsageCFPropertyListRef ) {
		gUsageCFPropertyListRef = hu_XMLLoad( CFSTR( "HID_device_usage_strings" ), CFSTR( "plist" ) );
	}

	if ( gUsageCFPropertyListRef ) {
		if ( CFDictionaryGetTypeID( ) == CFGetTypeID( gUsageCFPropertyListRef ) ) {
			CFStringRef vendorKeyCFStringRef = CFStringCreateWithFormat( kCFAllocatorDefault, NULL, CFSTR( "%ld" ), inVendorID );

			if ( vendorKeyCFStringRef ) {
				CFDictionaryRef vendorCFDictionaryRef;

				if ( CFDictionaryGetValueIfPresent( gUsageCFPropertyListRef, vendorKeyCFStringRef, ( const void ** ) &vendorCFDictionaryRef ) ) {
					CFStringRef vendorCFStringRef = NULL;

					if ( CFDictionaryGetValueIfPresent( vendorCFDictionaryRef, kNameKeyCFStringRef, ( const void ** ) &vendorCFStringRef ) ) {
						vendorCFStringRef = CFStringCreateCopy( kCFAllocatorDefault, vendorCFStringRef );
					} else {
						vendorCFStringRef = CFStringCreateWithFormat( kCFAllocatorDefault, NULL, CFSTR( "v: %ld" ), inVendorID );
						// CFShow( vendorCFStringRef );
					}
					CFStringRef productKeyCFStringRef = CFStringCreateWithFormat( kCFAllocatorDefault, NULL, CFSTR( "%ld" ), inProductID );

					CFDictionaryRef productCFDictionaryRef;
					if ( CFDictionaryGetValueIfPresent( vendorCFDictionaryRef, productKeyCFStringRef, ( const void ** ) &productCFDictionaryRef ) ) {
						CFStringRef fullCFStringRef = NULL;

						CFStringRef productCFStringRef;
						if ( CFDictionaryGetValueIfPresent( productCFDictionaryRef, kNameKeyCFStringRef, ( const void ** ) &productCFStringRef ) ) {
							// CFShow( productCFStringRef );
						}
						CFStringRef usageKeyCFStringRef = CFStringCreateWithFormat( kCFAllocatorDefault, NULL, CFSTR( "%ld:%ld" ), inUsagePage, inUsage );
						CFStringRef usageCFStringRef;
						if ( CFDictionaryGetValueIfPresent( productCFDictionaryRef, usageKeyCFStringRef, ( const void ** ) &usageCFStringRef ) ) {
#if VERBOSE_ELEMENT_NAMES
							fullCFStringRef = CFStringCreateWithFormat( kCFAllocatorDefault, NULL, CFSTR( "%@ %@ %@" ), vendorCFStringRef, productCFStringRef, usageCFStringRef );
#else
							fullCFStringRef = CFStringCreateWithFormat( kCFAllocatorDefault, NULL, CFSTR( "%@" ), usageCFStringRef );
#endif VERBOSE_ELEMENT_NAMES
							// CFShow( usageCFStringRef );
						}
#if FAKE_MISSING_NAMES
						else {
							fullCFStringRef = CFStringCreateWithFormat( kCFAllocatorDefault, NULL, CFSTR( "%@ %@ # %@" ), vendorCFStringRef, productCFStringRef, usageKeyCFStringRef );
						}
#endif FAKE_MISSING_NAMES

						if ( fullCFStringRef ) {
							// CFShow( fullCFStringRef );
							results = CFStringGetCString( fullCFStringRef, outCStr, CFStringGetLength( fullCFStringRef ) * sizeof( UniChar ) + 1, kCFStringEncodingUTF8 );
							CFRelease( fullCFStringRef );
						}
						CFRelease( usageKeyCFStringRef );
					}
					if ( vendorCFStringRef ) {
						CFRelease( vendorCFStringRef );
					}
					CFRelease( productKeyCFStringRef );
				}
				CFRelease( vendorKeyCFStringRef );
			}
		}
		// ++ CFRelease( gUsageCFPropertyListRef );	// Leak this !
	}
	return results;
}	// hu_XMLSearchForElementNameByUsage

#if 0	// currently unused
/*************************************************************************
 *
 * hu_AddVendorProductToCFDict( inCFMutableDictionaryRef, inVendorID, inVendorCFStringRef, inProductID, inProductCFStringRef )
 *
 * Purpose: add a vendor & product to a dictionary
 *
 * Inputs: inCFMutableDictionaryRef - the dictionary
 *			inVendorID				- the elements vendor ID
 *			inProductID				- the elements product ID
 *			inProductCFStringRef	- the string to be added
 *
 * Returns: Boolean		- if successful
 */
static Boolean hu_AddVendorProductToCFDict( CFMutableDictionaryRef inCFMutableDictionaryRef, long inVendorID, CFStringRef inVendorCFStringRef, long inProductID, CFStringRef inProductCFStringRef )
{
	Boolean results = FALSE;

	if ( inCFMutableDictionaryRef && ( CFDictionaryGetTypeID( ) == CFGetTypeID( inCFMutableDictionaryRef ) ) ) {
		CFMutableDictionaryRef vendorCFMutableDictionaryRef;
		CFStringRef	vendorKeyCFStringRef;

		CFMutableDictionaryRef productCFMutableDictionaryRef;
		CFStringRef	productKeyCFStringRef;

		// if the vendor dictionary doesn't exist
		vendorKeyCFStringRef = CFStringCreateWithFormat( kCFAllocatorDefault, NULL, CFSTR( "%ld" ), inVendorID );
		if ( CFDictionaryGetValueIfPresent( inCFMutableDictionaryRef, vendorKeyCFStringRef, ( const void ** ) & vendorCFMutableDictionaryRef ) ) {
			// copy it.
			vendorCFMutableDictionaryRef = CFDictionaryCreateMutableCopy( kCFAllocatorDefault, 0, vendorCFMutableDictionaryRef );
		} else {	// ...otherwise...
			// create it.
			vendorCFMutableDictionaryRef = CFDictionaryCreateMutable( kCFAllocatorDefault, 0, & kCFTypeDictionaryKeyCallBacks, & kCFTypeDictionaryValueCallBacks );
			results = TRUE;
		}

		// if the vendor name key doesn't exist
		if ( ! CFDictionaryContainsKey( vendorCFMutableDictionaryRef, kNameKeyCFStringRef ) ) {
			// create it.
			CFDictionaryAddValue( vendorCFMutableDictionaryRef, kNameKeyCFStringRef, inVendorCFStringRef );
			results = TRUE;
		}

		// if the product key exists in the vendor dictionary
		productKeyCFStringRef = CFStringCreateWithFormat( kCFAllocatorDefault, NULL, CFSTR( "%ld" ), inProductID );
		if ( CFDictionaryGetValueIfPresent( vendorCFMutableDictionaryRef, productKeyCFStringRef, ( const void ** ) & productCFMutableDictionaryRef ) ) {
			// copy it.
			productCFMutableDictionaryRef = CFDictionaryCreateMutableCopy( kCFAllocatorDefault, 0, productCFMutableDictionaryRef );
		} else {	// ...otherwise...
			// create it.
			productCFMutableDictionaryRef = CFDictionaryCreateMutable( kCFAllocatorDefault, 0, & kCFTypeDictionaryKeyCallBacks, & kCFTypeDictionaryValueCallBacks );
			results = TRUE;
		}

		// if the product name key doesn't exist
		if ( ! CFDictionaryContainsKey( productCFMutableDictionaryRef, kNameKeyCFStringRef ) ) {
			// create it.
			CFDictionaryAddValue( productCFMutableDictionaryRef, kNameKeyCFStringRef, inProductCFStringRef );
			results = TRUE;
		}

		if ( vendorCFMutableDictionaryRef ) {
			if ( productCFMutableDictionaryRef ) {
				if ( results )
					CFDictionarySetValue( vendorCFMutableDictionaryRef, productKeyCFStringRef, productCFMutableDictionaryRef );
				CFRelease( productCFMutableDictionaryRef );
			}
			if ( results )
				CFDictionarySetValue( inCFMutableDictionaryRef, vendorKeyCFStringRef, vendorCFMutableDictionaryRef );
			CFRelease( vendorCFMutableDictionaryRef );
		}
		if ( productKeyCFStringRef )
			CFRelease( productKeyCFStringRef );
		if ( vendorKeyCFStringRef )
			CFRelease( vendorKeyCFStringRef );
	}
	return results;
}	// hu_AddVendorProductToCFDict

/*************************************************************************
 *
 * hu_AddDeviceElementToUsageXML( inDevice, inElement )
 *
 * Purpose: add a device and it's elements to our usage( XML ) file
 *
 * Inputs: inDevice		- the device
 *			inElement	- the element
 *
 * Returns: Boolean		- if successful
 */
static Boolean hu_AddDeviceElementToUsageXML( IOHIDDeviceRef inIOHIDDeviceRef, IOHIDElementRef inIOHIDElementRef )
{
	Boolean results = FALSE;

	if ( gUsageCFPropertyListRef ) {
		CFRelease( gUsageCFPropertyListRef );
	}

	gUsageCFPropertyListRef = hu_XMLLoad( CFSTR( "HID_device_usage_strings" ), CFSTR( "plist" ) );

	if ( gUsageCFPropertyListRef ) {
		CFMutableDictionaryRef tCFMutableDictionaryRef = CFDictionaryCreateMutableCopy( kCFAllocatorDefault, 0, gUsageCFPropertyListRef );

		if ( tCFMutableDictionaryRef ) {
			CFMutableDictionaryRef vendorCFMutableDictionaryRef;

			CFMutableDictionaryRef productCFMutableDictionaryRef;
			CFStringRef productKeyCFStringRef;

			CFStringRef usageKeyCFStringRef;

			// if the vendor dictionary exists...
			long vendorID = IOHIDDevice_GetVendorID( inIOHIDDeviceRef );
			CFStringRef vendorKeyCFStringRef = CFStringCreateWithFormat( kCFAllocatorDefault, NULL, CFSTR( "%ld" ), vendorID );
			if ( vendorKeyCFStringRef ) {
				if ( CFDictionaryGetValueIfPresent( tCFMutableDictionaryRef, vendorKeyCFStringRef,
												   ( const void ** ) &vendorCFMutableDictionaryRef ) ) {
					// ...copy it...
					vendorCFMutableDictionaryRef = CFDictionaryCreateMutableCopy( kCFAllocatorDefault, 0, vendorCFMutableDictionaryRef );
				} else {        // ...otherwise...
					// ...create it.
					vendorCFMutableDictionaryRef = CFDictionaryCreateMutable( kCFAllocatorDefault,
																			 0,
																			 &kCFTypeDictionaryKeyCallBacks,
																			 &kCFTypeDictionaryValueCallBacks );
					results = TRUE;
				}

				// if the vendor name key doesn't exist...
				if ( !CFDictionaryContainsKey( vendorCFMutableDictionaryRef, kNameKeyCFStringRef ) ) {
					CFStringRef manCFStringRef = IOHIDDevice_GetManufacturer( inIOHIDDeviceRef );
					// ...create it.
					CFDictionaryAddValue( vendorCFMutableDictionaryRef, kNameKeyCFStringRef, manCFStringRef );
					results = TRUE;
				}

				// if the product key exists in the vendor dictionary...
				long productID = IOHIDDevice_GetProductID( inIOHIDDeviceRef );
				productKeyCFStringRef = CFStringCreateWithFormat( kCFAllocatorDefault, NULL, CFSTR( "%ld" ), productID );

				if ( CFDictionaryGetValueIfPresent( vendorCFMutableDictionaryRef, productKeyCFStringRef,
												   ( const void ** ) &productCFMutableDictionaryRef ) ) {
					// ...copy it...
					productCFMutableDictionaryRef = CFDictionaryCreateMutableCopy( kCFAllocatorDefault, 0, productCFMutableDictionaryRef );
				} else {        // ...otherwise...
					// ...create it.
					productCFMutableDictionaryRef = CFDictionaryCreateMutable( kCFAllocatorDefault,
																			  0,
																			  &kCFTypeDictionaryKeyCallBacks,
																			  &kCFTypeDictionaryValueCallBacks );
					results = TRUE;
				}

				// if the product name key doesn't exist...
				if ( !CFDictionaryContainsKey( productCFMutableDictionaryRef, kNameKeyCFStringRef ) ) {
					CFStringRef productCFStringRef = IOHIDDevice_GetProduct( inIOHIDDeviceRef );
					// ...create it.
					CFDictionaryAddValue( productCFMutableDictionaryRef, kNameKeyCFStringRef, productCFStringRef );
					results = TRUE;
				}

				// if the usage key doesn't exist in the product dictionary...
				uint32_t usagePage = IOHIDElementGetUsagePage( inIOHIDElementRef );
				uint32_t usage = IOHIDElementGetUsagePage( inIOHIDElementRef );
				usageKeyCFStringRef = CFStringCreateWithFormat( kCFAllocatorDefault, NULL, CFSTR( "%ld:%ld" ), usagePage, usage );

				if ( usageKeyCFStringRef ) {
					if ( !CFDictionaryContainsKey( productCFMutableDictionaryRef, usageKeyCFStringRef ) ) {
						// find it's generic name
						CFStringRef usageCFStringRef = HIDCopyUsageName( usagePage, usage );

						if ( usageCFStringRef ) {
							// and add that.
							CFDictionaryAddValue( productCFMutableDictionaryRef, usageKeyCFStringRef, usageCFStringRef );
							results = TRUE;
							CFRelease( usageCFStringRef );
						}
					}
					CFRelease( usageKeyCFStringRef );
				}

				if ( vendorCFMutableDictionaryRef ) {
					if ( productCFMutableDictionaryRef ) {
						if ( results ) {
							CFDictionarySetValue( vendorCFMutableDictionaryRef, productKeyCFStringRef, productCFMutableDictionaryRef );
						}
						CFRelease( productCFMutableDictionaryRef );
					}

					if ( results ) {
						CFDictionarySetValue( tCFMutableDictionaryRef, vendorKeyCFStringRef, vendorCFMutableDictionaryRef );
					}
					CFRelease( vendorCFMutableDictionaryRef );
				}
				CFRelease( vendorKeyCFStringRef );
			}

			if ( productKeyCFStringRef ) {
				CFRelease( productKeyCFStringRef );
			}

			if ( results ) {
				hu_XMLSave( tCFMutableDictionaryRef, CFSTR( "HID_device_usage_strings" ), CFSTR( "plist" ) );
			}
			CFRelease( tCFMutableDictionaryRef );
		}
	}
	return results;
}	// hu_AddDeviceElementToUsageXML
#endif
