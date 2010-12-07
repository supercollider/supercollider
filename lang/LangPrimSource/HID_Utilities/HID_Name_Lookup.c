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
