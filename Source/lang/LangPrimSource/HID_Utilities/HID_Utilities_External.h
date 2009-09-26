/*
	File:		HID_Utilities_External.h

	Contains:   Definition of the HID Utilities exported functions

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

#ifndef _HID_Utilities_External_h_
#define _HID_Utilities_External_h_

// ==================================
//includes

#if TARGET_RT_MAC_CFM
	typedef void (*IOHIDCallbackFunction)(void * target, unsigned long result, void* refcon, void * sender);
#else
	#include <IOKit/hid/IOHIDLib.h>
#endif TARGET_RT_MAC_CFM

#include <stdio.h>

#if 0
#include <IOKit/hid/IOHIDUsageTables.h>

//#include "PID.h"				// NOTE: These are now in <IOHIDUsageTables.h>
//#include "IOHIDPowerUsage.h"	// NOTE: These are now in <IOHIDUsageTables.h>
#endif

// ==================================

#ifdef __cplusplus
extern "C" {
#endif

// ==================================
// Device and Element Interfaces

typedef enum HIDElementTypeMask
{
	kHIDElementTypeInput				= 1 << 1,
	kHIDElementTypeOutput            	= 1 << 2,
	kHIDElementTypeFeature           	= 1 << 3,
	kHIDElementTypeCollection        	= 1 << 4,
	kHIDElementTypeIO					= kHIDElementTypeInput | kHIDElementTypeOutput | kHIDElementTypeFeature,
	kHIDElementTypeAll					= kHIDElementTypeIO | kHIDElementTypeCollection
}HIDElementTypeMask;

struct recElement
{
    unsigned long type;						// the type defined by IOHIDElementType in IOHIDKeys.h
    long usagePage;							// usage page from IOUSBHIDParser.h which defines general usage
    long usage;								// usage within above page from IOUSBHIDParser.h which defines specific usage
    void * cookie;				 			// unique value (within device of specific vendorID and productID) which identifies element, will NOT change
    long min;								// reported min value possible
    long max;								// reported max value possible
    long scaledMin;							// reported scaled min value possible
    long scaledMax;							// reported scaled max value possible
    long size;								// size in bits of data return from element
    unsigned char relative;					// are reports relative to last report (deltas)
    unsigned char wrapping;					// does element wrap around (one value higher than max is min)
    unsigned char nonLinear;				// are the values reported non-linear relative to element movement
    unsigned char preferredState;			// does element have a preferred state (such as a button)
    unsigned char nullState;				// does element have null state
    long units;								// units value is reported in (not used very often)
    long unitExp;							// exponent for units (also not used very often)
    char name[256];							// name of element (c string)

// runtime variables
    long calMin; 							// min returned value
    long calMax; 							// max returned value (calibrate call)
    long userMin; 							// user set value to scale to (scale call)
    long userMax;

	struct recElement * pPrevious;			// previous element (NULL at list head)
    struct recElement * pChild;				// next child (only of collections)
    struct recElement * pSibling;			// next sibling (for elements and collections)

	long depth;
};
typedef struct recElement recElement;
typedef recElement* pRecElement;

struct recDevice
{
    void * interface;						// interface to device, NULL = no interface
    void * queue;							// device queue, NULL = no queue
	void * queueRunLoopSource;				// device queue run loop source, NULL == no source
	void * transaction;						// output transaction interface, NULL == no interface
	void * notification;					// notifications
    char transport[256];					// device transport (c string)
    long vendorID;							// id for device vendor, unique across all devices
    long productID;							// id for particular product, unique across all of a vendors devices
    long version;							// version of product
    char manufacturer[256];					// name of manufacturer
    char product[256];						// name of product
    char serial[256];						// serial number of specific product, can be assumed unique across specific product or specific vendor (not used often)
    long locID;								// long representing location in USB (or other I/O) chain which device is pluged into, can identify specific device on machine
    long usage;								// usage page from IOUSBHID Parser.h which defines general usage
    long usagePage;							// usage within above page from IOUSBHID Parser.h which defines specific usage
    long totalElements;						// number of total elements (should be total of all elements on device including collections) (calculated, not reported by device)
	long features;							// number of elements of type kIOHIDElementTypeFeature
	long inputs;							// number of elements of type kIOHIDElementTypeInput_Misc or kIOHIDElementTypeInput_Button or kIOHIDElementTypeInput_Axis or kIOHIDElementTypeInput_ScanCodes
	long outputs;							// number of elements of type kIOHIDElementTypeOutput
	long collections;						// number of elements of type kIOHIDElementTypeCollection
    long axis;								// number of axis (calculated, not reported by device)
    long buttons;							// number of buttons (calculated, not reported by device)
    long hats;								// number of hat switches (calculated, not reported by device)
    long sliders;							// number of sliders (calculated, not reported by device)
    long dials;								// number of dials (calculated, not reported by device)
    long wheels;							// number of wheels (calculated, not reported by device)
    recElement* pListElements; 				// head of linked list of elements
    struct recDevice* pNext; 				// next device
};
typedef struct recDevice recDevice;
typedef recDevice* pRecDevice;

// ==================================
// HID Utilities interface
// ==================================
// Create and open an interface to device, required prior to extracting values or building queues
// Note: appliction now owns the device and must close and release it prior to exiting
extern unsigned long HIDCreateOpenDeviceInterface (UInt32 hidDevice, pRecDevice pDevice);

// builds list of device with elements (allocates memory and captures devices)
// list is allcoated internally within HID Utilites and can be accessed via accessor functions
// structures within list are considered flat and user accessable, butnot user modifiable
// can be called again to rebuild list to account for new devices (will do the right thing in case of disposing existing list)
extern Boolean HIDBuildDeviceList (UInt32 usagePage, UInt32 usage);

// release list built by above function
// MUST be called prior to application exit to properly release devices
// if not called (or app crashes) devices can be recovered by pluging into different location in USB chain
extern void HIDReleaseDeviceList (void);

// does a device list exist
extern Boolean HIDHaveDeviceList (void);

// how many HID devices have been found
// returns 0 if no device list exist
extern UInt32 HIDCountDevices (void);

// how many elements does a specific device have
// returns 0 if device is invalid or NULL
// uses mask of HIDElementTypeMask to restrict element found
// use kHIDElementTypeIO to get non-collection elements
extern UInt32 HIDCountDeviceElements (pRecDevice pDevice, HIDElementTypeMask typeMask);

// get the first device in the device list
// returns NULL if no list exists
extern pRecDevice HIDGetFirstDevice (void);

// get next device in list given current device as parameter
// returns NULL if end of list
extern pRecDevice HIDGetNextDevice (pRecDevice pDevice);

// get the first element of device passed in as parameter
// returns NULL if no list exists or device does not exists or is NULL
// uses mask of HIDElementTypeMask to restrict element found
// use kHIDElementTypeIO to get previous HIDGetFirstDeviceElement functionality
extern pRecElement HIDGetFirstDeviceElement (pRecDevice pDevice, HIDElementTypeMask typeMask);

// get next element of given device in list given current element as parameter
// will walk down each collection then to next element or collection (depthwise traverse)
// returns NULL if end of list
// uses mask of HIDElementTypeMask to restrict element found
// use kHIDElementTypeIO to get previous HIDGetNextDeviceElement functionality
extern pRecElement HIDGetNextDeviceElement (pRecElement pElement, HIDElementTypeMask typeMask);

// get previous element of given device in list given current element as parameter
// this wlaks directly up the tree to the top element and does not search at each level
// returns NULL if beginning of list
// uses mask of HIDElementTypeMask to restrict element found
// use kHIDElementTypeIO to get non-collection elements
extern pRecElement HIDGetPreviousDeviceElement (pRecElement pElement, HIDElementTypeMask typeMask);

// returns C string type name given a type enumeration passed in as parameter (see IOHIDKeys.h)
// returns empty string for invlid types
extern void HIDGetTypeName (IOHIDElementType theType, char * cstrName);

// set name from vendor id/product id look up (using cookies)
extern Boolean HIDGetElementNameFromVendorProductCookie (const long vendorID, const long productID, const long cookie, char * pName);

// set name from vendor id/product id look up (using usage page & usage)
extern Boolean HIDGetElementNameFromVendorProductUsage (const long vendorID, const long productID, const long pUsagePage, const long pUsage, char * pName);

// returns C string usage given usage page and usage passed in as parameters (see IOUSBHIDParser.h)
// returns usage page and usage values in string form for unknown values
extern void HIDGetUsageName (long valueUsagePage, long valueUsage, char * cstrName);

// returns calibrated value given raw value passed in
// calibrated value is equal to min and max values returned by HIDGetElementValue since device list built scaled to element reported min and max values
extern SInt32 HIDCalibrateValue (SInt32 value, pRecElement pElement);

// returns scaled value given raw value passed in
// scaled value is equal to current value assumed to be in the range of element reported min and max values scaled to user min and max scaled values
extern SInt32 HIDScaleValue (SInt32 value, pRecElement pElement);

// ---------------------------------
// convert an element type to a mask
extern HIDElementTypeMask HIDConvertElementTypeToMask (const long type);

// find this device
extern Boolean HIDFindDevice(const pRecDevice pSearchDevice, pRecDevice *ppFoundDevice);

// find the device and element for this action
// Device: serial, vendorID, productID, location, usagePage, usage
// Element: cookie, usagePage, usage,
extern Boolean HIDFindActionDeviceAndElement(const pRecDevice pSearchDevice, const pRecElement pSearchElement,
											 pRecDevice *ppFoundDevice, pRecElement *ppFoundElement);

// find the device and element for this action
// Device: serial, vendorID, productID, location, usagePage, usage
// Element: cookie, usagePage, usage,

extern Boolean HIDFindSubElement(const pRecElement pStartElement, const pRecElement pSearchElement, pRecElement *ppFoundElement);

// print out all of an elements information
extern int HIDPrintElement(const pRecElement pElement);

// return true if this is a valid device pointer
extern Boolean HIDIsValidDevice(const pRecDevice pSearchDevice);

// return true if this is a valid element pointer for this device
extern Boolean HIDIsValidElement(const pRecDevice pSearchDevice, const pRecElement pSearchElement);

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
extern unsigned long  HIDQueueElement (pRecDevice pDevice, pRecElement pElement);

// adds all elements to queue, performing any device queue set up required
extern unsigned long HIDQueueDevice (pRecDevice pDevice);

// removes element for queue, if last element in queue will release queue and device
extern unsigned long HIDDequeueElement (pRecDevice pDevice, pRecElement pElement);

// completely removes all elements from queue and releases queue and device
extern unsigned long HIDDequeueDevice (pRecDevice pDevice);

// releases all device queues for quit or rebuild (must be called)
extern unsigned long HIDReleaseAllDeviceQueues (void);

// releases interface to device, should be done prior to exiting application (called from HIDReleaseDeviceList)
extern unsigned long HIDCloseReleaseInterface (pRecDevice pDevice);

// returns true if an event is avialable for the element and fills out *pHIDEvent structure, returns false otherwise
// pHIDEvent is a poiner to a IOHIDEventStruct, using void here for compatibility, users can cast a required
extern unsigned char HIDGetEvent (pRecDevice pDevice, void * pHIDEvent);

// returns current value for element, creating device interface as required, polling element
extern long HIDGetElementValue (pRecDevice pDevice, pRecElement pElement);

// Set an elements value
// NOTE: This should only be used when a single element report needs to be sent.
// If multiple elements reports are to be send then transactions should be used.
// pIOHIDEvent is a poiner to a IOHIDEventStruct, using void here for compatibility, users can cast a required
extern long HIDSetElementValue (pRecDevice pDevice, pRecElement pElement,void* pIOHIDEvent);

// Set a callback to be called when a queue goes from empty to non-empty
extern long HIDSetQueueCallback (pRecDevice pDevice, IOHIDCallbackFunction callback);

#if 0
// Get a report from a device
extern long HIDGetReport (pRecDevice pDevice,const IOHIDReportType reportType, const unsigned long reportID, void* reportBuffer, unsigned long* reportBufferSize);

// Send a report to a device
extern long HIDSetReport (pRecDevice pDevice,const IOHIDReportType reportType, const unsigned long reportID, void* reportBuffer, const unsigned long reportBufferSize);
#endif

// ==================================
// HUD utilities interfaces

// returns calibrated value given raw value passed in
// calibrated value is equal to min and max values returned by HIDGetElementValue since device list built scaled to element reported min and max values
extern long HIDCalibrateValue (long value, pRecElement pElement);

// returns scaled value given raw value passed in
// scaled value is equal to current value assumed to be in the range of element reported min and max values scaled to user min and max scaled values
extern long HIDScaleValue (long value, pRecElement pElement);

// convert an element type to a mask
extern HIDElementTypeMask HIDConvertElementTypeToMask (const long type);

// find this device
extern Boolean HIDFindDevice(const pRecDevice pSearchDevice, pRecDevice *ppFoundDevice);

// find the device and element for this action
// Device: serial, vendorID, productID, location, usagePage, usage
// Element: cookie, usagePage, usage,
extern Boolean HIDFindActionDeviceAndElement(const pRecDevice pSearchDevice, const pRecElement pSearchElement,
										  pRecDevice *ppFoundDevice, pRecElement *ppFoundElement);
// find the device and element for this action
// Device: serial, vendorID, productID, location, usagePage, usage
// Element: cookie, usagePage, usage,

extern Boolean HIDFindSubElement(const pRecElement pStartElement, const pRecElement pSearchElement, pRecElement *ppFoundElement);

// print out all of an elements information
extern int HIDPrintElement(const pRecElement pElement);

// return true if this is a valid device pointer
extern Boolean HIDIsValidDevice(const pRecDevice pSearchDevice);

// return true if this is a valid element pointer for this device
extern Boolean HIDIsValidElement(const pRecDevice pSearchDevice, const pRecElement pSearchElement);

// ==================================
// Name Lookup Interfaces

// set name from vendor id/product id look up (using cookies)
extern Boolean HIDGetElementNameFromVendorProductCookie (const long vendorID, const long productID, const long cookie, char * pName);

// set name from vendor id/product id look up (using usage page & usage)
extern Boolean HIDGetElementNameFromVendorProductUsage (const long vendorID, const long productID, const long pUsagePage, const long pUsage, char * pName);

// ==================================
// Conguration and Save Interfaces

enum
{
    kPercentMove = 10 // precent of overall range a element must move to register
};
/* this is not in the current version... jan.t
typedef struct recSaveHID
{
    long actionCookie;
    // device
		// need to add serial number when I have a test case
    long vendorID;
    long productID;
    long locID;
    long usage;
    long usagePage;
    // elements
    long usagePageE;
    long usageE;
    void * cookie;
}recSaveHID,*pRecSaveHID;

// polls all devices and elements for a change greater than kPercentMove.  Times out after given time
// returns true and pointer to device and element if found
// returns false and NULL for both parameters if not found
extern unsigned char HIDConfigureAction (pRecDevice * ppDevice, pRecElement * ppElement, float timeout);

// take input records, save required info
// assume file is open and at correct position.
extern void HIDSaveElementConfig (FILE * fileRef, pRecDevice pDevice, pRecElement pElement, long actionCookie);

// take file, read one record (assume file position is correct and file is open)
// search for matching device
// return pDevice, pElement and cookie for action
extern long HIDRestoreElementConfig (FILE * fileRef, pRecDevice * ppDevice, pRecElement * ppElement);

// Save the device & element values into the specified key in the specified applications preferences
extern Boolean HIDSaveElementPref (CFStringRef keyCFStringRef, CFStringRef appCFStringRef, pRecDevice pDevice, pRecElement pElement);

// Find the specified preference in the specified application
// search for matching device and element
// return pDevice, pElement that matches

extern Boolean HIDRestoreElementPref (CFStringRef keyCFStringRef, CFStringRef appCFStringRef, pRecDevice * ppDevice, pRecElement * ppElement);
*/
// ==================================
// Output Transaction interface

// Create and open an transaction interface to device, required prior to extracting values or building Transactions
extern unsigned long HIDTransactionAddElement(pRecDevice pDevice, pRecElement pElement);

// removes an element from a Transaction
extern unsigned long HIDTransactionRemoveElement(pRecDevice pDevice, pRecElement pElement);

// return true if this transaction contains this element
extern Boolean HIDTransactionHasElement(pRecDevice pDevice, pRecElement pElement);

/* This changes the default value of an element, when the values of the */
/* elements are cleared, on clear or commit, they are reset to the */
/* default value */
/* This call can be made on elements that are not in the transaction, but */
/* has undefined behavior if made on elements not in the transaction */
/* which are later added to the transaction. */
/* In other words, an element should be added before its default is */
/* set, for well defined behavior. */
// pHIDEvent is a poiner to a IOHIDEventStruct, using void here for compatibility, users can cast a required
extern unsigned long HIDTransactionSetElementDefault(pRecDevice pDevice, pRecElement pElement,IOHIDEventStruct* pValueEvent);

/* Get the current setting of an element's default value */
// pHIDEvent is a poiner to a IOHIDEventStruct, using void here for compatibility, users can cast a required
extern unsigned long HIDTransactionGetElementDefault(pRecDevice pDevice, pRecElement pElement,IOHIDEventStruct* pValueEvent);

/* Add a change to the transaction, by setting an element value */
/* The change is not actually made until it is commited */
/* The element must be part of the transaction or this call will fail */
// pHIDEvent is a poiner to a IOHIDEventStruct, using void here for compatibility, users can cast a required
extern unsigned long HIDTransactionSetElementValue(pRecDevice pDevice, pRecElement pElement,IOHIDEventStruct* pValueEvent);

/* Get the current setting of an element value */
// pHIDEvent is a poiner to a IOHIDEventStruct, using void here for compatibility, users can cast a required
extern unsigned long HIDTransactionGetElementValue(pRecDevice pDevice, pRecElement pElement,IOHIDEventStruct* pValueEvent);

/* Commit the transaction, or clear all the changes and start over */
/* timoutMS is the timeout in milliseconds, a zero timeout will cause */
/*	this call to be non-blocking (returning queue empty) if there */
/*	is a NULL callback, and blocking forever until the queue is */
/*	non-empty if their is a valid callback */
/* callback, if non-NULL is a callback to be called when data is */
/*  inserted to the queue  */
/* callbackTarget and callbackRefcon are passed to the callback */
extern unsigned long HIDTransactionCommit(pRecDevice pDevice);

/* Clear all the changes and start over */
extern unsigned long HIDTransactionClear(pRecDevice pDevice);

// ==================================

#ifdef __cplusplus
}
#endif

#endif // _HID_Utilities_External_h_
