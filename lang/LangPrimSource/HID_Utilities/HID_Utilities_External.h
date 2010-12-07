//
// File:       HID_Utilities_External.h
//
// Abstract:   External interface for HID Utilities, can be used with either library or source
//             Check notes below for usage.  Some type casting is required so library is framework and carbon free
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
// Copyright © 2001-2009 Apple Inc. All Rights Reserved.
//

#ifndef _HID_Utilities_External_h_
#define _HID_Utilities_External_h_

// ==================================

#ifdef __cplusplus
extern "C" {
#endif

// ==================================

//includes

#include <stdio.h>
#include "IOHIDLib_.h"

// ==================================

#ifndef _IOKIT_HID_IOHIDKEYS_H_
/*!
 @typedef IOHIDElementCookie
 @abstract Abstract data type used as a unique identifier for an element.
 */
#ifdef __LP64__
typedef uint32_t IOHIDElementCookie;
#else
typedef void * IOHIDElementCookie;
#endif
#endif

// Device and Element Interfaces

enum HIDElementTypeMask
{
	kHIDElementTypeInput				= 1 << 1,
	kHIDElementTypeOutput            	= 1 << 2,
	kHIDElementTypeFeature           	= 1 << 3,
	kHIDElementTypeCollection        	= 1 << 4,
	kHIDElementTypeIO					= kHIDElementTypeInput | kHIDElementTypeOutput | kHIDElementTypeFeature,
	kHIDElementTypeAll					= kHIDElementTypeIO | kHIDElementTypeCollection
};
typedef enum HIDElementTypeMask HIDElementTypeMask;

// ==================================

//*****************************************************
#pragma mark - exported globals
//-----------------------------------------------------

extern IOHIDManagerRef gIOHIDManagerRef;
extern CFMutableArrayRef gDeviceCFArrayRef;
extern CFArrayRef gElementCFArrayRef;

//*************************************************************************
//
// HIDBuildMultiDeviceList( inUsagePages, inUsages, inNumDeviceTypes )
//
// Purpose:	builds list of devices with elements (allocates memory and captures devices) in which 
//			the devices could be of different types/usages list is allocated internally within HID
//			Utilites and can be accessed via accessor functions structures within list are considered
//			flat and user accessable, but not user modifiable can be called again to rebuild list to 
//			account for new devices (will do the right thing in case of disposing existing list) 
//			usagePage, usage are each a numDeviceTypes sized array of matching usage and usage pages
//			returns true if succesful
//
// Inputs:	inUsagePages		- inNumDeviceTypes sized array of matching usage pages
//			inUsages			- inNumDeviceTypes sized array of matching usages
//			inNumDeviceTypes	- number of usage pages & usages
//
// Returns:	Boolean				- if successful
//
extern Boolean HIDBuildMultiDeviceList( const UInt32 *inUsagePages, const UInt32 *inUsages, int inNumDeviceTypes );

// same as above but this uses a single usagePage and usage
extern Boolean HIDBuildDeviceList (UInt32 usagePage, UInt32 usage);

// updates the current device list for any new/removed devices
// if this is called before HIDBuildDeviceList the it functions like HIDBuildMultiDeviceList
// usagePage, usage are each a numDeviceTypes sized array of matching usage and usage pages
// returns true if successful which means if any device were added or removed (the device config changed)
extern Boolean HIDUpdateDeviceList( const UInt32 *inUsagePages, const UInt32 *inUsages, int inNumDeviceTypes );


// release list built by above function
// MUST be called prior to application exit to properly release devices
// if not called (or app crashes) devices can be recovered by pluging into different location in USB chain
extern void HIDReleaseDeviceList (void);

//*************************************************************************
//
// HIDRebuildDevices(  )
//
// Purpose:	rebuilds the (internal) list of devices
//
// Inputs:	none
//
// Returns:	none
//

extern void HIDRebuildDevices( void );

// does a device list exist
extern unsigned char HIDHaveDeviceList (void);

// how many HID devices have been found
// returns 0 if no device list exist
extern UInt32 HIDCountDevices (void);

// how many elements does a specific device have
// returns 0 if device is invalid or NULL
// uses mask of HIDElementTypeMask to restrict element found
// use kHIDElementTypeIO to get non-collection elements
extern UInt32 HIDCountDeviceElements ( IOHIDDeviceRef inIOHIDDeviceRef, HIDElementTypeMask typeMask);

// get the first device in the device list
// returns NULL if no list exists
extern IOHIDDeviceRef HIDGetFirstDevice (void);

// get next device in list given current device as parameter
// returns NULL if end of list
extern IOHIDDeviceRef HIDGetNextDevice (IOHIDDeviceRef inIOHIDDeviceRef);

// get the first element of device passed in as parameter
// returns NULL if no list exists or device does not exists or is NULL
// uses mask of HIDElementTypeMask to restrict element found
// use kHIDElementTypeIO to get previous HIDGetFirstDeviceElement functionality
extern IOHIDElementRef HIDGetFirstDeviceElement (IOHIDDeviceRef inIOHIDDeviceRef, HIDElementTypeMask typeMask);

// get next element of given device in list given current element as parameter
// will walk down each collection then to next element or collection (depthwise traverse)
// returns NULL if end of list
// uses mask of HIDElementTypeMask to restrict element found
// use kHIDElementTypeIO to get previous HIDGetNextDeviceElement functionality
extern IOHIDElementRef HIDGetNextDeviceElement ( IOHIDElementRef inIOHidElementRef, HIDElementTypeMask typeMask);

// get previous element of given device in list given current element as parameter
// this walks directly up the tree to the top element and does not search at each level
// returns NULL if beginning of list
// uses mask of HIDElementTypeMask to restrict element found
// use kHIDElementTypeIO to get non-collection elements
extern IOHIDElementRef HIDGetPreviousDeviceElement (IOHIDElementRef inIOHidElementRef, HIDElementTypeMask typeMask);

// returns C string type name given a type enumeration passed in as parameter( see IOHIDKeys.h )
// returns empty string for invalid types
extern void HIDGetTypeName( IOHIDElementType inIOHIDElementType, char * outCStrName );
	
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

extern CFStringRef HIDCopyUsageName( long inUsagePage, long inUsage );

// ==================================

// Element Event Queue and Value Interfaces

enum
{
	kDefaultUserMin = 0,					// default user min and max used for scaling
	kDefaultUserMax = 255
};

enum
{
	kDeviceQueueSize = 50	// this is wired kernel memory so should be set to as small as possible
	// but should account for the maximum possible events in the queue
	// USB updates will likely occur at 100 Hz so one must account for this rate of
	// if states change quickly (updates are only posted on state changes)
};

// ==================================

// queues specific element, performing any device queue set up required
extern int  HIDQueueElement (IOHIDDeviceRef inIOHIDDeviceRef, IOHIDElementRef inIOHidElementRef);

// adds all elements to queue, performing any device queue set up required
extern int  HIDQueueDevice (IOHIDDeviceRef inIOHIDDeviceRef);

// removes element for queue, if last element in queue will release queue and device
extern int  HIDDequeueElement (IOHIDDeviceRef inIOHIDDeviceRef, IOHIDElementRef inIOHidElementRef);

// completely removes all elements from queue and releases queue and device
extern int  HIDDequeueDevice (IOHIDDeviceRef inIOHIDDeviceRef);

// releases all device queues for quit or rebuild (must be called)
extern int  HIDReleaseAllDeviceQueues (void);

// returns true if an event is avialable for the element and fills out *pHIDEvent structure, returns false otherwise
// pHIDEvent is a poiner to a IOHIDEventStruct, using void here for compatibility, users can cast a required
	extern unsigned char HIDGetEvent( IOHIDDeviceRef inIOHIDDeviceRef, IOHIDValueRef * pIOHIDValueRef );

// ==================================

// Conguration and Save Interfaces

enum
{
	kPercentMove = 10 // precent of overall range a element must move to register
};

struct recSaveHID
{
	int actionCookie;
	// device
	// need to add serial number when I have a test case
	struct {
		int vendorID;
		int productID;
		int locID;
		int usage;
		int usagePage;
	} device;
	// elements
	struct {
		int usagePage;
		int usage;
		int minReport;
		int maxReport;
		IOHIDElementCookie cookie; // always 32 bits
	} element;
};
typedef struct recSaveHID recSaveHID;
typedef recSaveHID * pRecSaveHID;

// get vendor name from vendor ID
extern Boolean HIDGetVendorNameFromVendorID( long inVendorID, char * outCStrName );

// get product name from vendor/product ID
extern Boolean HIDGetProductNameFromVendorProductID( long inVendorID, long inProductID, char * outCStrName );
	
// get element name from vendor id/product id look up ( using element cookie )
extern Boolean HIDGetElementNameFromVendorProductCookie (int inVendorID, int inProductID, IOHIDElementCookie inCookie, char * outCStrName);

// get element name from vendor id/product id look up ( using element usage page & usage )
extern Boolean HIDGetElementNameFromVendorProductUsage( long inVendorID, long inProductID, long inUsagePage, long inUsage, char * inCStrName );

// utility routines to dump device or element info
extern void HIDDumpDeviceInfo( IOHIDDeviceRef inIOHIDDeviceRef );
extern void HIDDumpElementInfo( IOHIDElementRef inIOHIDElementRef );
extern void HIDDumpElementCalibrationInfo( IOHIDElementRef inIOHIDElementRef );

// polls single device's elements for a change greater than kPercentMove.  Times out after given time
// returns 1 and pointer to element if found
// returns 0 and NULL for both parameters if not found
extern unsigned char HIDConfigureSingleDeviceAction (IOHIDDeviceRef inIOHIDDeviceRef, IOHIDElementRef *outIOHIDElementRef, float timeout);

//*************************************************************************
//
// HIDConfigureAction( outDeviceRef, outElementRef, inTimeout )
//
// Purpose:	polls all devices and elements for a change greater than kPercentMove.
//			Times out after given time returns 1 and pointer to device and element
//			if found; returns 0 and NULL for both parameters if not found
//
// Inputs:	outDeviceRef	- address where to store the device
//			outElementRef	- address where to store the element
//			inTimeout		- the timeout
// Returns:	Boolean			- TRUE if successful
//			outDeviceRef	- the device
//			outElementRef	- the element
//

extern Boolean HIDConfigureAction( IOHIDDeviceRef* outDeviceRef, IOHIDElementRef *outElementRef, float inTimeout );

//*************************************************************************
//
// HIDSaveElementPref( inKeyCFStringRef, inAppCFStringRef, inDeviceRef, inElementRef )
//
// Purpose:	Save the device & element values into the specified key in the specified applications preferences
//
// Inputs:	inKeyCFStringRef	- the preference key
//			inAppCFStringRef	- the application identifier
//			inDeviceRef			- the device
//			inElementRef			- the element
// Returns:	Boolean				- if successful
//

extern Boolean HIDSaveElementPref( const CFStringRef inKeyCFStringRef,
								  CFStringRef inAppCFStringRef,
								  IOHIDDeviceRef inDeviceRef,
								  IOHIDElementRef inElementRef );

//*************************************************************************
//
// HIDRestoreElementPref( inKeyCFStringRef, inAppCFStringRef, outDeviceRef, outElementRef )
//
// Purpose:	Find the specified preference in the specified application
//
// Inputs:	inKeyCFStringRef	- the preference key
//			inAppCFStringRef	- the application identifier
//			outDeviceRef		- address where to restore the device
//			outElementRef		- address where to restore the element
// Returns:	Boolean				- if successful
//			outDeviceRef		- the device
//			outElementRef		- the element
//

extern Boolean HIDRestoreElementPref( CFStringRef inKeyCFStringRef,
									 CFStringRef inAppCFStringRef,
									 IOHIDDeviceRef*outDeviceRef,
									 IOHIDElementRef *outElementRef );

//*************************************************************************
//
// HIDFindDeviceAndElement( inSearchInfo, outFoundDevice, outFoundElement )
//
// Purpose:	find the closest matching device and element for this action
//
// Notes:	matches device: serial, vendorID, productID, location, inUsagePage, usage
//			matches element: cookie, inUsagePage, usage,
//
// Inputs:	inSearchInfo	- the device & element info we searching for
//			outFoundDevice	- the address of the best matching device
//			outFoundElement	- the address of the best matching element
//
// Returns:	Boolean			- TRUE if we find a match
//			outFoundDevice	- the best matching device
//			outFoundElement	- the best matching element
//

extern Boolean HIDFindDeviceAndElement( const recSaveHID* inSearchInfo,
									   IOHIDDeviceRef* outFoundDevice,
									   IOHIDElementRef *outFoundElement );

// -- These are routines to use if the applcationwants HID Utilities to do the file handling --
// Note: the FILE * is a MachO posix FILE and will not likely work directly with MW MSL FILE * type.

// take input records, save required info
// assume file is open and at correct position.
void HIDSaveElementConfig (FILE * fileRef, IOHIDDeviceRef inIOHIDDeviceRef, IOHIDElementRef inIOHidElementRef, int actionCookie);

// takes a file, reads one record (assume file position is correct and file is open)
// search for matching device
// return tIOHIDDeviceRef, tIOHIDElementRef and cookie for action
int HIDRestoreElementConfig (FILE * fileRef, IOHIDDeviceRef *outIOHIDDeviceRef, IOHIDElementRef *outIOHIDElementRef);

// -- These are routines to use if the client wants to use their own file handling -- 

// Set up a config record for saving
// takes an input records, returns record user can save as they want 
// Note: the save rec must be pre-allocated by the calling app and will be filled out
void HIDSetElementConfig (pRecSaveHID pConfigRec, IOHIDDeviceRef inIOHIDDeviceRef, IOHIDElementRef inIOHidElementRef, int actionCookie);

// Get matching element from config record
// takes a pre-allocated and filled out config record
// search for matching device
// return tIOHIDDeviceRef, tIOHIDElementRef and cookie for action
int HIDGetElementConfig (pRecSaveHID pConfigRec, IOHIDDeviceRef *outIOHIDDeviceRef, IOHIDElementRef *outIOHIDElementRef);

// ==================================

// Error reporter, can be set to report however the application desires
extern void HIDReportError (char * strError);

// Error with numeric code reporter, can be set to report however the application desires
extern void HIDReportErrorNum (char * strError, int numError);


#ifdef __cplusplus
}
#endif

#endif // _HID_Utilities_External_h_
