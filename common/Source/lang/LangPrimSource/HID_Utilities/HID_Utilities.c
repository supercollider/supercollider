/*
	File:		HID_Utilities.c

	Contains:	Implementation of HID Utilities

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
#include "HID_Utilities_External.h"

#include <IOKit/IOCFPlugIn.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/IOMessage.h>
#include <IOKit/hid/IOHIDUsageTables.h>

//================================================================================================
#define USE_NOTIFICATIONS 1
//================================================================================================
// local (static) functions
//================================================================================================
static void hid_GetCollectionElements (CFMutableDictionaryRef deviceProperties, pRecElement *ppCurrentCollection);
static pRecDevice hid_DisposeDevice (pRecDevice ppDevice);
//================================================================================================
//   globals
//================================================================================================
#if USE_NOTIFICATIONS
static IONotificationPortRef	gNotifyPort;
static io_iterator_t		gAddedIter;
static CFRunLoopRef		gRunLoop;
#endif USE_NOTIFICATIONS

// for element retrieval
static pRecDevice gCurrentGetDevice = NULL;
static Boolean gAddAsChild = false;
static int gDepth = false;

static pRecDevice gpDeviceList = NULL;
static UInt32 gNumDevices = 0;

#pragma mark private functions
// ==================================
// private functions

static UInt32 hid_CountCurrentDevices (void);

// extracts actual specific element information from each element CF dictionary entry
static void hid_GetElementInfo (CFTypeRef refElement, pRecElement pElement)
{
	long number;
	CFTypeRef refType;
	// type, usagePage, usage already stored
	refType = CFDictionaryGetValue (refElement, CFSTR(kIOHIDElementCookieKey));
	if (refType && CFNumberGetValue (refType, kCFNumberLongType, &number))
		pElement->cookie = (IOHIDElementCookie) number;
	else
		pElement->cookie = (IOHIDElementCookie) 0;

	refType = CFDictionaryGetValue (refElement, CFSTR(kIOHIDElementMinKey));
	if (refType && CFNumberGetValue (refType, kCFNumberLongType, &number))
		pElement->min = number;
	else
		pElement->min = 0;

	pElement->calMax = pElement->min;
	pElement->userMin = kDefaultUserMin;

	refType = CFDictionaryGetValue (refElement, CFSTR(kIOHIDElementMaxKey));
	if (refType && CFNumberGetValue (refType, kCFNumberLongType, &number))
		pElement->max = number;
	else
		pElement->max = 0;

	pElement->calMin = pElement->max;
	pElement->userMax = kDefaultUserMax;

	refType = CFDictionaryGetValue (refElement, CFSTR(kIOHIDElementScaledMinKey));
	if (refType && CFNumberGetValue (refType, kCFNumberLongType, &number))
		pElement->scaledMin = number;
	else
		pElement->scaledMin = 0;

	refType = CFDictionaryGetValue (refElement, CFSTR(kIOHIDElementScaledMaxKey));
	if (refType && CFNumberGetValue (refType, kCFNumberLongType, &number))
		pElement->scaledMax = number;
	else
		pElement->scaledMax = 0;

	refType = CFDictionaryGetValue (refElement, CFSTR(kIOHIDElementSizeKey));
	if (refType && CFNumberGetValue (refType, kCFNumberLongType, &number))
		pElement->size = number;
	else
		pElement->size = 0;

	refType = CFDictionaryGetValue (refElement, CFSTR(kIOHIDElementIsRelativeKey));
	if (refType)
		pElement->relative = CFBooleanGetValue (refType);
	else
		pElement->relative = 0;

	refType = CFDictionaryGetValue (refElement, CFSTR(kIOHIDElementIsWrappingKey));
	if (refType)
		pElement->wrapping = CFBooleanGetValue (refType);
	else
		pElement->wrapping = false;

	refType = CFDictionaryGetValue (refElement, CFSTR(kIOHIDElementIsNonLinearKey));
	if (refType)
		pElement->nonLinear = CFBooleanGetValue (refType);
	else
		pElement->wrapping = false;

#ifdef kIOHIDElementHasPreferredStateKey
	refType = CFDictionaryGetValue (refElement, CFSTR(kIOHIDElementHasPreferredStateKey));
#else // Mac OS X 10.0 has spelling error
	refType = CFDictionaryGetValue (refElement, CFSTR(kIOHIDElementHasPreferedStateKey));
#endif
	if (refType)
		pElement->preferredState = CFBooleanGetValue (refType);
	else
		pElement->preferredState = false;

	refType = CFDictionaryGetValue (refElement, CFSTR(kIOHIDElementHasNullStateKey));
	if (refType)
		pElement->nullState = CFBooleanGetValue (refType);
	else
		pElement->nullState = false;

	refType = CFDictionaryGetValue (refElement, CFSTR(kIOHIDElementUnitKey));
	if (refType && CFNumberGetValue (refType, kCFNumberLongType, &number))
		pElement->units = number;
	else
		pElement->units = 0;

	refType = CFDictionaryGetValue (refElement, CFSTR(kIOHIDElementUnitExponentKey));
	if (refType && CFNumberGetValue (refType, kCFNumberLongType, &number))
		pElement->unitExp = number;
	else
		pElement->unitExp = 0;

	refType = CFDictionaryGetValue (refElement, CFSTR(kIOHIDElementNameKey));
	if (refType)
		if (!CFStringGetCString (refType, pElement->name, 256, CFStringGetSystemEncoding ()))
			HIDReportError ("CFStringGetCString error retrieving pElement->name.");
	if (!*pElement->name)
	{
		// set name from vendor id, product id & usage info look up
		if (!HIDGetElementNameFromVendorProductUsage (gCurrentGetDevice->vendorID, gCurrentGetDevice->productID, pElement->usagePage, pElement->usage, pElement->name))
		{
			// set name from vendor id/product id look up
			HIDGetElementNameFromVendorProductCookie (gCurrentGetDevice->vendorID, gCurrentGetDevice->productID, (long) pElement->cookie, pElement->name);
			if (!*pElement->name) { // if no name
				HIDGetUsageName (pElement->usagePage, pElement->usage, pElement->name);
				if (!*pElement->name) // if not usage
					sprintf (pElement->name, "Element");
			}
		}
	}
}

// ---------------------------------
// examines CF dictionary vlaue in device element hierarchy to determine if it is element of interest or a collection of more elements
// if element of interest allocate storage, add to list and retrieve element specific info
// if collection then pass on to deconstruction collection into additional individual elements

static void hid_AddElement (CFTypeRef refElement, pRecElement * ppElementCurrent)
{
	pRecDevice pDevice = gCurrentGetDevice;
    pRecElement pElement = NULL;
    long elementType, usagePage, usage;
    CFTypeRef refElementType = CFDictionaryGetValue (refElement, CFSTR(kIOHIDElementTypeKey));
    CFTypeRef refUsagePage = CFDictionaryGetValue (refElement, CFSTR(kIOHIDElementUsagePageKey));
    CFTypeRef refUsage = CFDictionaryGetValue (refElement, CFSTR(kIOHIDElementUsageKey));

    if (refElementType)
		CFNumberGetValue (refElementType, kCFNumberLongType, &elementType);
	if (refUsagePage)
		CFNumberGetValue (refUsagePage, kCFNumberLongType, &usagePage);
	if (refUsage)
		CFNumberGetValue (refUsage, kCFNumberLongType, &usage);

	if (NULL == pDevice)
		return;

    if (elementType)
    {
        // look at types of interest
        if (elementType != kIOHIDElementTypeCollection)
		{
            if (usagePage && usage) // if valid usage and page
			{
				switch (usagePage) // only interested in kHIDPage_GenericDesktop and  kHIDPage_Button
				{
					case kHIDPage_GenericDesktop:
					{
						switch (usage) // look at usage to determine function
						{
							case kHIDUsage_GD_X:
							case kHIDUsage_GD_Y:
							case kHIDUsage_GD_Z:
							case kHIDUsage_GD_Rx:
							case kHIDUsage_GD_Ry:
							case kHIDUsage_GD_Rz:
								pElement = (pRecElement) malloc (sizeof (recElement));
								if (pElement) pDevice->axis++;
									break;
							case kHIDUsage_GD_Slider:
								pElement = (pRecElement) malloc (sizeof (recElement));
								if (pElement) pDevice->sliders++;
									break;
							case kHIDUsage_GD_Dial:
								pElement = (pRecElement) malloc (sizeof (recElement));
								if (pElement) pDevice->dials++;
									break;
							case kHIDUsage_GD_Wheel:
								pElement = (pRecElement) malloc (sizeof (recElement));
								if (pElement) pDevice->wheels++;
									break;
							case kHIDUsage_GD_Hatswitch:
								pElement = (pRecElement) malloc (sizeof (recElement));
								if (pElement) pDevice->hats++;
									break;
							default:
								pElement = (pRecElement) malloc (sizeof (recElement));
								break;
						}
					}
						break;
					case kHIDPage_Button:
						pElement = (pRecElement) malloc (sizeof (recElement));
						if (pElement) pDevice->buttons++;
							break;
					default:
						// just add a generic element
						pElement = (pRecElement) malloc (sizeof (recElement));
						break;
				}
			}
#if 0
            else
                HIDReportError ("CFNumberGetValue error when getting value for refUsage or refUsagePage.");
#endif 0
        }
        else // collection
			pElement = (pRecElement) malloc (sizeof (recElement));
    }
    else
        HIDReportError ("CFNumberGetValue error when getting value for refElementType.");

    if (pElement) // add to list
    {
		// this code builds a binary tree based on the collection hierarchy of inherent in the device element layout
		// it preserves the structure of the lements as collections have children and elements are siblings to each other

		// clear record
		bzero(pElement,sizeof(recElement));

		// get element info
        pElement->type = elementType;
        pElement->usagePage = usagePage;
        pElement->usage = usage;
        pElement->depth = 0;		// assume root object
        hid_GetElementInfo (refElement, pElement);

		// count elements
		pDevice->totalElements++;

		switch (pElement->type)
		{
			case kIOHIDElementTypeInput_Misc:
			case kIOHIDElementTypeInput_Button:
			case kIOHIDElementTypeInput_Axis:
			case kIOHIDElementTypeInput_ScanCodes:
				pDevice->inputs++;
				break;
			case kIOHIDElementTypeOutput:
				pDevice->outputs++;
				break;
			case kIOHIDElementTypeFeature:
				pDevice->features++;
				break;
			case kIOHIDElementTypeCollection:
				pDevice->collections++;
				break;
			default:
				HIDReportErrorNum ("Unknown element type : ", pElement->type);
		}

        if (NULL == *ppElementCurrent) // if at list head
		{
            pDevice->pListElements = pElement; // add current element
			*ppElementCurrent = pElement; // set current element to element we just added
		}
		else // have exsiting structure
		{
			if (gAddAsChild) // if the previous element was a collection, let's add this as a child of the previous
			{
				// this iteration should not be needed but there maybe some untested degenerate case which this code will ensure works
				while ((*ppElementCurrent)->pChild) // step down tree until free child node found
					*ppElementCurrent = (*ppElementCurrent)->pChild;
				(*ppElementCurrent)->pChild = pElement; // insert there
				pElement->depth = (*ppElementCurrent)->depth + 1;
			}
			else // add as sibling
			{
				// this iteration should not be needed but there maybe some untested degenerate case which this code will ensure works
				while ((*ppElementCurrent)->pSibling) // step down tree until free sibling node found
					*ppElementCurrent = (*ppElementCurrent)->pSibling;
				(*ppElementCurrent)->pSibling = pElement; // insert there
				pElement->depth = (*ppElementCurrent)->depth;
			}
			pElement->pPrevious = *ppElementCurrent; // point to previous
			*ppElementCurrent = pElement; // set current to our collection
		}

		if (elementType == kIOHIDElementTypeCollection) // if this element is a collection of other elements
		{
			gAddAsChild = true; // add next set as children to this element
			gDepth++;
			hid_GetCollectionElements ((CFMutableDictionaryRef) refElement, &pElement); // recursively process the collection
			gDepth--;
		}
		gAddAsChild = false; // add next as this elements sibling (when return from a collection or with non-collections)
    }
#if 0
    else
        HIDReportError ("hid_AddElement - no element added.");
#endif
}

// ---------------------------------
// collects information from each array member in device element list (each array memeber = element)

static void hid_GetElementsCFArrayHandler (const void * value, void * parameter)
{
    if (CFGetTypeID (value) == CFDictionaryGetTypeID ())
        hid_AddElement ((CFTypeRef) value, (pRecElement *) parameter);
}

// ---------------------------------
// handles retrieval of element information from arrays of elements in device IO registry information

static void hid_GetElements (CFTypeRef refElementCurrent, pRecElement *ppCurrentElement)
{
    CFTypeID type = CFGetTypeID (refElementCurrent);
    if (type == CFArrayGetTypeID()) // if element is an array
    {
        CFRange range = {0, CFArrayGetCount (refElementCurrent)};
        // CountElementsCFArrayHandler called for each array member
        CFArrayApplyFunction (refElementCurrent, range, hid_GetElementsCFArrayHandler, ppCurrentElement);
    }
}

// ---------------------------------
// handles extracting element information from element collection CF types
// used from top level element decoding and hierarchy deconstruction to flatten device element list

static void hid_GetCollectionElements (CFMutableDictionaryRef deviceProperties, pRecElement *ppCurrentCollection)
{
    CFTypeRef refElementTop = CFDictionaryGetValue (deviceProperties, CFSTR(kIOHIDElementKey));
    if (refElementTop)
        hid_GetElements (refElementTop, ppCurrentCollection);
    else
        HIDReportError ("hid_GetCollectionElements: CFDictionaryGetValue error when creating CFTypeRef for kIOHIDElementKey.");
}

// ---------------------------------
// use top level element usage page and usage to discern device usage page and usage setting appropriate values in device record

static void hid_TopLevelElementHandler (const void * value, void * parameter)
{
    CFTypeRef refCF = 0;
    if ((NULL == value) || (NULL == parameter))
        return;	// (kIOReturnBadArgument)
    if (CFGetTypeID (value) != CFDictionaryGetTypeID ())
        return;	// (kIOReturnBadArgument)
    refCF = CFDictionaryGetValue (value, CFSTR(kIOHIDElementUsagePageKey));
    if (!CFNumberGetValue (refCF, kCFNumberLongType, &((pRecDevice) parameter)->usagePage))
        HIDReportError ("CFNumberGetValue error retrieving pDevice->usagePage.");
    refCF = CFDictionaryGetValue (value, CFSTR(kIOHIDElementUsageKey));
    if (!CFNumberGetValue (refCF, kCFNumberLongType, &((pRecDevice) parameter)->usage))
        HIDReportError ("CFNumberGetValue error retrieving pDevice->usage.");
}

// ---------------------------------
// extracts device info from CF dictionary records in IO registry

static void hid_GetDeviceInfo (io_object_t hidDevice, CFMutableDictionaryRef hidProperties, pRecDevice pDevice)
{
	CFMutableDictionaryRef usbProperties = 0;
	io_registry_entry_t parent1, parent2;

    // Mac OS X currently is not mirroring all USB properties to HID page so need to look at USB device page also
    // get dictionary for usb properties: step up two levels and get CF dictionary for USB properties
    if ((KERN_SUCCESS == IORegistryEntryGetParentEntry (hidDevice, kIOServicePlane, &parent1)) &&
        (KERN_SUCCESS == IORegistryEntryGetParentEntry (parent1, kIOServicePlane, &parent2)) &&
        (KERN_SUCCESS == IORegistryEntryCreateCFProperties (parent2, &usbProperties, kCFAllocatorDefault, kNilOptions)))
    {
        if (usbProperties)
        {
            CFTypeRef refCF = 0;
            // get device info
            // try hid dictionary first, if fail then go to usb dictionary

            // get transport
            refCF = CFDictionaryGetValue (hidProperties, CFSTR(kIOHIDTransportKey));
            if (refCF)
            {
				if (!CFStringGetCString (refCF, pDevice->transport, 256, CFStringGetSystemEncoding ()))
                    HIDReportError ("CFStringGetCString error retrieving pDevice->transport.");
            }

            // get vendorID
            refCF = CFDictionaryGetValue (hidProperties, CFSTR(kIOHIDVendorIDKey));
            if (!refCF)
                refCF = CFDictionaryGetValue (usbProperties, CFSTR("idVendor"));
            if (refCF)
            {
                if (!CFNumberGetValue (refCF, kCFNumberLongType, &pDevice->vendorID))
                    HIDReportError ("CFNumberGetValue error retrieving pDevice->vendorID.");
            }

            // get product ID
            refCF = CFDictionaryGetValue (hidProperties, CFSTR(kIOHIDProductIDKey));
            if (!refCF)
                refCF = CFDictionaryGetValue (usbProperties, CFSTR("idProduct"));
            if (refCF)
            {
                if (!CFNumberGetValue (refCF, kCFNumberLongType, &pDevice->productID))
                    HIDReportError ("CFNumberGetValue error retrieving pDevice->productID.");
            }

            // get product version
            refCF = CFDictionaryGetValue (hidProperties, CFSTR(kIOHIDVersionNumberKey));
            if (refCF)
            {
                if (!CFNumberGetValue (refCF, kCFNumberLongType, &pDevice->version))
                    HIDReportError ("CFNumberGetValue error retrieving pDevice->version.");
            }

            // get manufacturer name
            refCF = CFDictionaryGetValue (hidProperties, CFSTR(kIOHIDManufacturerKey));
            if (!refCF)
                refCF = CFDictionaryGetValue (usbProperties, CFSTR("USB Vendor Name"));
            if (refCF)
            {
                if (!CFStringGetCString (refCF, pDevice->manufacturer, 256, CFStringGetSystemEncoding ()))
                    HIDReportError ("CFStringGetCString error retrieving pDevice->manufacturer.");
            }

            // get product name
            refCF = CFDictionaryGetValue (hidProperties, CFSTR(kIOHIDProductKey));
            if (!refCF)
                refCF = CFDictionaryGetValue (usbProperties, CFSTR("USB Product Name"));
            if (refCF)
            {
                if (!CFStringGetCString (refCF, pDevice->product, 256, CFStringGetSystemEncoding ()))
                    HIDReportError ("CFStringGetCString error retrieving pDevice->product.");
            }

            // get serial
            refCF = CFDictionaryGetValue (hidProperties, CFSTR(kIOHIDSerialNumberKey));
            if (refCF)
            {
                if (!CFStringGetCString (refCF, pDevice->serial, 256, CFStringGetSystemEncoding ()))
                    HIDReportError ("CFStringGetCString error retrieving pDevice->serial.");
            }

            // get location ID
            refCF = CFDictionaryGetValue (hidProperties, CFSTR(kIOHIDLocationIDKey));
            if (!refCF)
                refCF = CFDictionaryGetValue (usbProperties, CFSTR("locationID"));
            if (refCF)
            {
                if (!CFNumberGetValue (refCF, kCFNumberLongType, &pDevice->locID))
                    HIDReportError ("CFNumberGetValue error retrieving pDevice->locID.");
            }

            // get usage page and usage
            refCF = CFDictionaryGetValue (hidProperties, CFSTR(kIOHIDPrimaryUsagePageKey));
            if (refCF)
            {
                if (!CFNumberGetValue (refCF, kCFNumberLongType, &pDevice->usagePage))
                    HIDReportError ("CFNumberGetValue error retrieving pDevice->usagePage.");
                refCF = CFDictionaryGetValue (hidProperties, CFSTR(kIOHIDPrimaryUsageKey));
                if (refCF)
                    if (!CFNumberGetValue (refCF, kCFNumberLongType, &pDevice->usage))
                        HIDReportError ("CFNumberGetValue error retrieving pDevice->usage.");
            }
            if (NULL == refCF) // get top level element HID usage page or usage
            {
                // use top level element instead
                CFTypeRef refCFTopElement = 0;
                refCFTopElement = CFDictionaryGetValue (hidProperties, CFSTR(kIOHIDElementKey));
                {
                    // refCFTopElement points to an array of element dictionaries
                    CFRange range = {0, CFArrayGetCount (refCFTopElement)};
                    CFArrayApplyFunction (refCFTopElement, range, hid_TopLevelElementHandler, NULL);
                }
            }
        }
        else
            HIDReportError ("IORegistryEntryCreateCFProperties failed to create usbProperties.");

        CFRelease (usbProperties);
        if (kIOReturnSuccess != IOObjectRelease (parent2))
            HIDReportError ("IOObjectRelease error with parent2.");
        if (kIOReturnSuccess != IOObjectRelease (parent1))
            HIDReportError ("IOObjectRelease error with parent1.");
    }
}

// ---------------------------------
// adds device to linked list of devices passed in (handles NULL lists properly)
// (returns where you just stored it)
static pRecDevice* hid_AddDevice (pRecDevice *ppListDeviceHead, pRecDevice pNewDevice)
{
	pRecDevice* result = NULL;

    if (NULL == *ppListDeviceHead)
        result = ppListDeviceHead;
    else
    {
        pRecDevice pDevicePrevious = NULL, pDevice = *ppListDeviceHead;
        while (pDevice)
        {
            pDevicePrevious = pDevice;
            pDevice = pDevicePrevious->pNext;
        }
        result = &pDevicePrevious->pNext;
    }
    pNewDevice->pNext = NULL;

	*result = pNewDevice;

	return result;
}

// ---------------------------------
// given a IO device object build a flat device record including device info and elements

static pRecDevice hid_BuildDevice (io_object_t hidDevice)
{
    pRecDevice pDevice = (pRecDevice) malloc (sizeof (recDevice));

    if (NULL != pDevice)
    {
		// get dictionary for HID properties
        CFMutableDictionaryRef hidProperties = 0;
        kern_return_t result = IORegistryEntryCreateCFProperties (hidDevice, &hidProperties, kCFAllocatorDefault, kNilOptions);

		// clear record
		bzero(pDevice, sizeof(recDevice));

        if ((result == KERN_SUCCESS) && (NULL != hidProperties))
        {
			pRecElement pCurrentElement = NULL;
			// create device interface
			result = HIDCreateOpenDeviceInterface (hidDevice, pDevice);
			if (kIOReturnSuccess != result)
				HIDReportErrorNum ("HIDCreateOpenDeviceInterface failed.", result);
            hid_GetDeviceInfo (hidDevice, hidProperties, pDevice); // hidDevice used to find parents in registry tree
																   // set current device for use in getting elements
			gCurrentGetDevice = pDevice;
			// Add all elements
            hid_GetCollectionElements (hidProperties, &pCurrentElement);
			gCurrentGetDevice = NULL;
            CFRelease (hidProperties);
        }
        else
            HIDReportErrorNum ("IORegistryEntryCreateCFProperties error when creating deviceProperties.", result);
    }
    else
        HIDReportError ("malloc error when allocating pRecDevice.");
    return pDevice;
}

#if USE_NOTIFICATIONS
//================================================================================================
//
//	hid_DeviceNotification
//
//	This routine will get called whenever any kIOGeneralInterest notification happens.  We are
//	interested in the kIOMessageServiceIsTerminated message so that's what we look for.  Other
//	messages are defined in IOMessage.h.
//
//================================================================================================
//
static void hid_DeviceNotification( void *refCon,
									io_service_t service,
									natural_t messageType,
									void *messageArgument )
{
    pRecDevice pDevice = (pRecDevice) refCon;

    if (messageType == kIOMessageServiceIsTerminated)
    {
        printf("Device 0x%08x \"%s\"removed.\n", service, pDevice->product);

        // Free the data we're no longer using now that the device is going away
		hid_DisposeDevice (pDevice);
    }
}
#else

static void hid_RemovalCallbackFunction(void * target, IOReturn result, void * refcon, void * sender)
{
	hid_DisposeDevice ((pRecDevice) target);
}

#endif USE_NOTIFICATIONS

//================================================================================================
//
//	hid_AddDevices
//
//	This routine is the callback for our IOServiceAddMatchingNotification.  When we get called
//	we will look at all the devices that were added and we will:
//
//	1.  Create some private data to relate to each device.
//	2.  Submit an IOServiceAddInterestNotification of type kIOGeneralInterest for this device,
//	    using the refCon field to store a pointer to our data.  When we get called with
//	    this interest notification, we can grab the refCon and access our private data.
//
//================================================================================================
//
// ---------------------------------
// given a IO device iterator, iterate it and add all its devices

static void hid_AddDevices (void *refCon, io_iterator_t iterator)
{
	// NOTE: refcon passed in is used to point to the device list head
    pRecDevice* pListDeviceHead = (pRecDevice*) refCon;
    IOReturn result = kIOReturnSuccess;
    io_object_t ioHIDDeviceObject = 0;

    while (ioHIDDeviceObject = IOIteratorNext (iterator))
    {
		pRecDevice* pNewDeviceAt = NULL;
		pRecDevice pNewDevice = hid_BuildDevice (ioHIDDeviceObject);
		if (pNewDevice)
		{
#if 0	// set true for verbose output
			printf("\nhid_AddDevices: pNewDevice = {t: \"%s\", v: %ld, p: %ld, v: %ld, m: \"%s\", " \
		  "p: \"%s\", l: %ld, u: %4.4lX:%4.4lX, #e: %ld, #f: %ld, #i: %ld, #o: %ld, " \
		  "#c: %ld, #a: %ld, #b: %ld, #h: %ld, #s: %ld, #d: %ld, #w: %ld}.",
		  pNewDevice->transport,
		  pNewDevice->vendorID,
		  pNewDevice->productID,
		  pNewDevice->version,
		  pNewDevice->manufacturer,
		  pNewDevice->product,
		  pNewDevice->locID,
		  pNewDevice->usagePage,
		  pNewDevice->usage,
		  pNewDevice->totalElements,
		  pNewDevice->features,
		  pNewDevice->inputs,
		  pNewDevice->outputs,
		  pNewDevice->collections,
		  pNewDevice->axis,
		  pNewDevice->buttons,
		  pNewDevice->hats,
		  pNewDevice->sliders,
		  pNewDevice->dials,
		  pNewDevice->wheels
		  );
			fflush(stdout);
#elif	0	// otherwise output brief description
			printf("\nhid_AddDevices: pNewDevice = {m: \"%s\" p: \"%s\", vid: %ld, pid: %ld, loc: %8.8lX, usage: %4.4lX:%4.4lX}.",
		  pNewDevice->manufacturer,
		  pNewDevice->product,
		  pNewDevice->vendorID,
		  pNewDevice->productID,
		  pNewDevice->locID,
		  pNewDevice->usagePage,
		  pNewDevice->usage
		  );
			fflush(stdout);
#endif
			pNewDeviceAt = hid_AddDevice (pListDeviceHead, pNewDevice);
		}

#if USE_NOTIFICATIONS
        // Register for an interest notification of this device being removed. Use a reference to our
        // private data as the refCon which will be passed to the notification callback.
        result = IOServiceAddInterestNotification( gNotifyPort,					// notifyPort
												   ioHIDDeviceObject,			// service
												   kIOGeneralInterest,			// interestType
												   hid_DeviceNotification,		// callback
												   pNewDevice,					// refCon
												   (io_object_t*) &pNewDevice->notification);	// notification
		if (KERN_SUCCESS != result)
			HIDReportErrorNum ("hid_AddDevices: IOServiceAddInterestNotification error: x0%8.8lX.", result);
#else
		result = (*(IOHIDDeviceInterface**)pNewDevice->interface)->setRemovalCallback (pNewDevice->interface, hid_RemovalCallbackFunction,pNewDeviceAt,0);
#endif USE_NOTIFICATIONS

		// release the device object, it is no longer needed
		result = IOObjectRelease (ioHIDDeviceObject);
		if (KERN_SUCCESS != result)
			HIDReportErrorNum ("hid_AddDevices: IOObjectRelease error with ioHIDDeviceObject.", result);
    }
}

// ---------------------------------
// disposes of the element list associated with a device and the memory associated with the list
// uses depthwise recursion to dispose both collections and elements.

static void hid_DisposeDeviceElements (pRecElement pElement)
{
	if (pElement)
	{
		if (pElement->pChild)
			hid_DisposeDeviceElements (pElement->pChild);
		if (pElement->pSibling)
			hid_DisposeDeviceElements (pElement->pSibling);
		free (pElement);
	}
}

// ---------------------------------
// disposes of a single device, closing and releaseing interface, freeing memory fro device and elements, setting device pointer to NULL
// all your device no longer belong to us... (i.e., you do not 'own' the device anymore)

static pRecDevice hid_DisposeDevice (pRecDevice pDevice)
{
    kern_return_t result = KERN_SUCCESS;
    pRecDevice pDeviceNext = NULL;

    if (HIDIsValidDevice(pDevice))
    {
        // save next device prior to disposing of this device
        pDeviceNext = pDevice->pNext;

		result = HIDDequeueDevice (pDevice);
#if 0
		if (kIOReturnSuccess != result)
			HIDReportErrorNum ("hid_DisposeDevice: HIDDequeueDevice error: 0x%8.8X.", result);
#endif 1

        hid_DisposeDeviceElements (pDevice->pListElements);
		pDevice->pListElements = NULL;

		result = HIDCloseReleaseInterface (pDevice); // function sanity checks interface value (now application does not own device)
		if (kIOReturnSuccess != result)
			HIDReportErrorNum ("hid_DisposeDevice: HIDCloseReleaseInterface error: 0x%8.8X.", result);

#if USE_NOTIFICATIONS
        if (pDevice->interface)
        {
			// replace (*pDevice->interface)->Release(pDevice->interface);
			result = IODestroyPlugInInterface (pDevice->interface);
			if (kIOReturnSuccess != result)
				HIDReportErrorNum ("hid_DisposeDevice: IODestroyPlugInInterface error: 0x%8.8X.", result);
        }

        if (pDevice->notification)
		{
			result = IOObjectRelease((io_object_t) pDevice->notification);
			if (kIOReturnSuccess != result)
				HIDReportErrorNum ("hid_DisposeDevice: IOObjectRelease error: 0x%8.8X.", result);
		}
#endif USE_NOTIFICATIONS

		// remove this device from the device list
		if (gpDeviceList == pDevice)	// head of list?
			gpDeviceList = pDeviceNext;
		else
		{
			pRecDevice pDeviceTemp = pDeviceNext = gpDeviceList;	// we're going to return this if we don't find ourselfs in the list
			while (pDeviceTemp)
			{
				if (pDeviceTemp->pNext == pDevice) // found us!
				{
					// take us out of linked list
					pDeviceTemp->pNext = pDeviceNext = pDevice->pNext;
					break;
				}
				pDeviceTemp = pDeviceTemp->pNext;
			}
		}
        free (pDevice);
    }

	// update device count
	gNumDevices = hid_CountCurrentDevices ();

    return pDeviceNext;
}

// ---------------------------------
// count number of devices in global device list (gpDeviceList)
static UInt32 hid_CountCurrentDevices (void)
{
    pRecDevice pDevice = gpDeviceList;
    UInt32 devices = 0;
    while (pDevice)
    {
        devices++;
        pDevice = pDevice->pNext;
    }
    return devices;
}

// ---------------------------------
// matches type masks passed in to actual element types (which are not set up to be used as a mask
static Boolean hid_MatchElementTypeMask (IOHIDElementType type, HIDElementTypeMask typeMask)
{
	if (typeMask & kHIDElementTypeInput)
		if ((type == kIOHIDElementTypeInput_Misc) || (type == kIOHIDElementTypeInput_Button) || (type == kIOHIDElementTypeInput_Axis) || (type == kIOHIDElementTypeInput_ScanCodes))
			return true;
	if (typeMask & kHIDElementTypeOutput)
		if (type == kIOHIDElementTypeOutput)
			return true;
	if (typeMask & kHIDElementTypeFeature)
		if (type == kIOHIDElementTypeFeature)
			return true;
	if (typeMask & kHIDElementTypeCollection)
		if (type == kIOHIDElementTypeCollection)
			return true;
	return false;
}

// ---------------------------------

static pRecElement hid_GetDeviceElement (pRecElement pElement, HIDElementTypeMask typeMask)
{
	// we are asking for this element
    if (NULL != pElement)
	{
		if (hid_MatchElementTypeMask (pElement->type, typeMask)) // if the type match what we are looking for
			return pElement; // return the element
		else
			return HIDGetNextDeviceElement (pElement, typeMask); // else get the next one
	}
	return NULL;
}

#define FAKE_IT	0	// set true for debugging; returns the usage & usage page as numbers

// ---------------------------------
// Load the usage strings from the <HID_usage_strings.plist> resource (XML) file into a CFPropertyListRef
static CFPropertyListRef xml_load_usage_strings(void)
{
	CFPropertyListRef tCFPropertyListRef = NULL;
	CFURLRef resFileCFURLRef = CFBundleCopyResourceURL(CFBundleGetMainBundle(), CFSTR("HID_usage_strings"), CFSTR("plist"), NULL);

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
// Find a usage string in the <HID_usage_strings.plist> resource (XML) file

static Boolean xml_GetUsageName(const long valueUsagePage, const long valueUsage, char* pCstr)
{
	static CFPropertyListRef tCFPropertyListRef = NULL;
	Boolean results = false;

	if (NULL == tCFPropertyListRef)
		tCFPropertyListRef = xml_load_usage_strings();
	if (NULL != tCFPropertyListRef)
	{
		if (CFDictionaryGetTypeID() == CFGetTypeID(tCFPropertyListRef))
		{
			CFDictionaryRef pageCFDictionaryRef;
			CFStringRef	pageKeyCFStringRef;
			pageKeyCFStringRef = CFStringCreateWithFormat(kCFAllocatorDefault, NULL, CFSTR("0x%4.4lX"), valueUsagePage);

			if (CFDictionaryGetValueIfPresent(tCFPropertyListRef, pageKeyCFStringRef, (const void**) &pageCFDictionaryRef))
			{
				CFStringRef	pageCFStringRef;

				if (CFDictionaryGetValueIfPresent(pageCFDictionaryRef, CFSTR("Name"), (const void**) &pageCFStringRef))
				{
					//CFShow(pageCFStringRef);
				}

				{
					CFStringRef fullCFStringRef = NULL;
					CFStringRef	usageKeyCFStringRef;
					CFStringRef	usageCFStringRef;

					usageKeyCFStringRef = CFStringCreateWithFormat(kCFAllocatorDefault, NULL, CFSTR("0x%4.4lX"), valueUsage);

					if (CFDictionaryGetValueIfPresent(pageCFDictionaryRef, usageKeyCFStringRef, (const void**) &usageCFStringRef))
					{
						fullCFStringRef = CFStringCreateWithFormat(kCFAllocatorDefault, NULL, CFSTR("%@ %@"),
												 pageCFStringRef, usageCFStringRef);
						// CFShow(usageCFStringRef);
					}
#if FAKE_IT
					else
					{
						fullCFStringRef = CFStringCreateWithFormat(kCFAllocatorDefault, NULL, CFSTR("%@ #%@"),
												 pageCFStringRef, usageKeyCFStringRef);
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
			}
			CFRelease(pageKeyCFStringRef);
		}
		//++CFRelease(tCFPropertyListRef);	// Leak this!
	}
	return results;
}

#pragma mark public functions
// =================================
// public functions

// ---------------------------------
// Create and open an interface to device, required prior to extracting values or building queues
// Note: appliction now owns the device and must close and release it prior to exiting

unsigned long HIDCreateOpenDeviceInterface (UInt32 hidDevice, pRecDevice pDevice)
{
    IOReturn result = kIOReturnSuccess;
    HRESULT plugInResult = S_OK;
    SInt32 score = 0;
    IOCFPlugInInterface ** ppPlugInInterface = NULL;

	if (NULL == pDevice->interface)
	{
		result = IOCreatePlugInInterfaceForService (hidDevice, kIOHIDDeviceUserClientTypeID,
											  kIOCFPlugInInterfaceID, &ppPlugInInterface, &score);
		if (kIOReturnSuccess == result)
		{
			// Call a method of the intermediate plug-in to create the device interface
			plugInResult = (*ppPlugInInterface)->QueryInterface (ppPlugInInterface,
														CFUUIDGetUUIDBytes (kIOHIDDeviceInterfaceID), (void *) &(pDevice->interface));
			if (S_OK != plugInResult)
				HIDReportErrorNum ("Couldn’t query HID class device interface from plugInInterface", plugInResult);
			IODestroyPlugInInterface (ppPlugInInterface); // replace (*ppPlugInInterface)->Release (ppPlugInInterface)
		}
		else
			HIDReportErrorNum ("Failed to create **plugInInterface via IOCreatePlugInInterfaceForService.", result);
	}
	if (NULL != pDevice->interface)
	{
		result = (*(IOHIDDeviceInterface**)pDevice->interface)->open (pDevice->interface, 0);
		if (kIOReturnSuccess != result)
			HIDReportErrorNum ("Failed to open pDevice->interface via open.", result);
	}
    return result;
}

// ---------------------------------
// builds list of device with elements (allocates memory and captures devices)
// list is allcoated internally within HID Utilites and can be accessed via accessor functions
// structures within list are considered flat and user accessable, but not user modifiable
// can be called again to rebuild list to account for new devices (will do the right thing in case of disposing existing list)
// returns true if succesful

Boolean HIDBuildDeviceList (UInt32 usagePage, UInt32 usage)
{
    IOReturn result = kIOReturnSuccess;
    mach_port_t masterPort = 0;

    if (NULL != gpDeviceList)
        HIDReleaseDeviceList ();

    result = IOMasterPort (bootstrap_port, &masterPort);
    if (kIOReturnSuccess != result)
        HIDReportErrorNum ("IOMasterPort error with bootstrap_port.", result);
    else
    {
		CFMutableDictionaryRef hidMatchDictionary = NULL;

		// Set up matching dictionary to search the I/O Registry for HID devices we are interested in. Dictionary reference is NULL if error.
		{
			CFNumberRef refUsage = NULL, refUsagePage = NULL;

			// Set up a matching dictionary to search I/O Registry by class name for all HID class devices.
			hidMatchDictionary = IOServiceMatching (kIOHIDDeviceKey);
			if (NULL != hidMatchDictionary)
			{
				if (usagePage)
				{
					// Add key for device type (joystick, in this case) to refine the matching dictionary.
					refUsagePage = CFNumberCreate (kCFAllocatorDefault, kCFNumberLongType, &usagePage);
					CFDictionarySetValue (hidMatchDictionary, CFSTR (kIOHIDPrimaryUsagePageKey), refUsagePage);
					CFRelease (refUsagePage);
					if (usage)
					{
						refUsage = CFNumberCreate (kCFAllocatorDefault, kCFNumberLongType, &usage);
						CFDictionarySetValue (hidMatchDictionary, CFSTR (kIOHIDPrimaryUsageKey), refUsage);
						CFRelease (refUsage);
					}
				}
				CFRetain(hidMatchDictionary);
			}
			else
				HIDReportError ("Failed to get HID CFMutableDictionaryRef via IOServiceMatching.");
		}

#if USE_NOTIFICATIONS
		// Create a notification port and add its run loop event source to our run loop
		// This is how async notifications get set up.
		{
			CFRunLoopSourceRef		runLoopSource;

			gNotifyPort = IONotificationPortCreate(masterPort);
			runLoopSource = IONotificationPortGetRunLoopSource(gNotifyPort);

			gRunLoop = CFRunLoopGetCurrent();
			CFRunLoopAddSource(gRunLoop, runLoopSource, kCFRunLoopDefaultMode);

			// Now set up a notification to be called when a device is first matched by I/O Kit.
			result = IOServiceAddMatchingNotification(gNotifyPort,			// notifyPort
											 kIOFirstMatchNotification,		// notificationType
											 hidMatchDictionary,			// matching
											 hid_AddDevices,				// callback
											 &gpDeviceList,					// refCon
											 &gAddedIter					// notification
											 );

			// call it now to add all existing devices
			hid_AddDevices(&gpDeviceList,gAddedIter);
		}
#else
		{
			io_iterator_t hidObjectIterator = NULL;

			// Now search I/O Registry for matching devices.
			result = IOServiceGetMatchingServices (masterPort, hidMatchDictionary, &hidObjectIterator);
			if (kIOReturnSuccess != result)
				HIDReportErrorNum ("Failed to create IO object iterator, error:", result);
			else if (NULL == hidObjectIterator) // likely no HID devices which matched selection criteria are connected
				HIDReportError ("Warning: Could not find any matching devices, thus iterator creation failed.");

			if (NULL != hidObjectIterator)
			{
				hid_AddDevices(&gpDeviceList,hidObjectIterator);

				result = IOObjectRelease (hidObjectIterator); // release the iterator
				if (kIOReturnSuccess != result)
					HIDReportErrorNum ("IOObjectRelease error with hidObjectIterator.", result);

				gNumDevices = hid_CountCurrentDevices ();
				return true;
			}
		}
#endif USE_NOTIFICATIONS
		// IOServiceGetMatchingServices consumes a reference to the dictionary, so we don't need to release the dictionary ref.
		hidMatchDictionary = NULL;
    }
	return false;
}

// ---------------------------------
// release list built by above function
// MUST be called prior to application exit to properly release devices
// if not called (or app crashes) devices can be recovered by pluging into different location in USB chain

void HIDReleaseDeviceList (void)
{
    while (NULL != gpDeviceList)
		gpDeviceList = hid_DisposeDevice (gpDeviceList); // dispose current device return next device will set gpDeviceList to NULL
    gNumDevices = 0;
}

// ---------------------------------
// does a device list exist

Boolean HIDHaveDeviceList (void)
{
    if (NULL != gpDeviceList)
		return true;
    return false;
}

// ---------------------------------
// how many HID devices have been found
// returns 0 if no device list exist

UInt32 HIDCountDevices (void)
{
	gNumDevices = hid_CountCurrentDevices ();

    return gNumDevices;
}

// ---------------------------------
// how many elements does a specific device have
// returns 0 if device is invlaid or NULL

UInt32 HIDCountDeviceElements (pRecDevice pDevice, HIDElementTypeMask typeMask)
{
	long count = 0;
    if (HIDIsValidDevice(pDevice))
	{
		if (typeMask & kHIDElementTypeInput)
			count += pDevice->inputs;
		if (typeMask & kHIDElementTypeOutput)
			count += pDevice->outputs;
		if (typeMask & kHIDElementTypeFeature)
			count += pDevice->features;
		if (typeMask & kHIDElementTypeCollection)
			count += pDevice->collections;
	}
	return count;
}

// ---------------------------------
// get the first device in the device list
// returns NULL if no list exists

pRecDevice HIDGetFirstDevice (void)
{
    return gpDeviceList;
}

// ---------------------------------
// get next device in list given current device as parameter
// returns NULL if end of list

pRecDevice HIDGetNextDevice (pRecDevice pDevice)
{
    if (NULL != pDevice)
        return pDevice->pNext;
    else
        return NULL;
}

// ---------------------------------
// get the first element of device passed in as parameter
// returns NULL if no list exists or device does not exists or is NULL
pRecElement HIDGetFirstDeviceElement (pRecDevice pDevice, HIDElementTypeMask typeMask)
{
    if (HIDIsValidDevice(pDevice))
	{
        if (hid_MatchElementTypeMask (pDevice->pListElements->type, typeMask)) // ensure first type matches
			return pDevice->pListElements;
		else
			return HIDGetNextDeviceElement (pDevice->pListElements, typeMask);
	}
    else
        return NULL;
}

// ---------------------------------
// get next element of given device in list given current element as parameter
// will walk down each collection then to next element or collection (depthwise traverse)
// returns NULL if end of list
// uses mask of HIDElementTypeMask to restrict element found
// use kHIDElementTypeIO to get previous HIDGetNextDeviceElement functionality
pRecElement HIDGetNextDeviceElement (pRecElement pElement, HIDElementTypeMask typeMask)
{
	// should only have elements passed in (though someone could mix calls and pass us a collection)
	// collection means return the next child or sibling (in that order)
	// element means returnt he next sibling (as elements can't have children
    if (NULL != pElement)
	{
		if (pElement->pChild)
		{
			if (pElement->type != kIOHIDElementTypeCollection)
				HIDReportError ("Malformed element list: found child of element.");
			else
				return hid_GetDeviceElement (pElement->pChild, typeMask); // return the child of this element
		}
		else if (pElement->pSibling)
		{
			return hid_GetDeviceElement (pElement->pSibling, typeMask); //return the sibling of this element
		}
		else // at end back up correctly
		{
			pRecElement pPreviousElement = NULL;
			// malformed device ending in collection
			if (pElement->type == kIOHIDElementTypeCollection)
				HIDReportError ("Malformed device: found collection at end of element chain.");
			// walk back up tree to element prior to first collection ecountered and take next element
			while (NULL != pElement->pPrevious)
			{
				pPreviousElement = pElement;
				pElement = pElement->pPrevious; // look at previous element
									// if we have a collection and the previous element is the branch element (should have both a colection and next element attached to it)
		 // if we found a collection, which we are not at the sibling level that actually does have siblings
				if (((pElement->type == kIOHIDElementTypeCollection) && (pPreviousElement != pElement->pSibling) && pElement->pSibling) ||
		// or if we are at the top
		(NULL == pElement->pPrevious)) // at top of tree
					break;
			}
			if (NULL == pElement->pPrevious)
				return NULL; // got to top of list with only a collection as the first element
				 // now we must have been down the child route so go down the sibling route
			pElement = pElement->pSibling; // element of interest
			return hid_GetDeviceElement (pElement, typeMask); // otherwise return this element
		}
	}
	return NULL;
}

// ---------------------------------
// get previous element of given device in list given current element as parameter
// this wlaks directly up the tree to the top element and does not search at each level
// returns NULL if beginning of list
// uses mask of HIDElementTypeMask to restrict element found
// use kHIDElementTypeIO to get non-collection elements
pRecElement HIDGetPreviousDeviceElement (pRecElement pElement, HIDElementTypeMask typeMask)
{
	pRecElement pPreviousElement = pElement->pPrevious;
	// walk back up tree to element prior
	while (pPreviousElement && !hid_MatchElementTypeMask (pPreviousElement->type, typeMask))
	{
		pElement = pPreviousElement; // look at previous element
		pPreviousElement = pElement->pPrevious;
	}
	return pPreviousElement; // return this element
}

// ---------------------------------
// returns C string type name given a type enumeration passed in as parameter (see IOHIDKeys.h)
// returns "Unknown Type" for invalid types

void HIDGetTypeName (IOHIDElementType theType, char * cstrName)
{
    switch (theType)
    {
        case kIOHIDElementTypeInput_Misc:
            sprintf(cstrName, "Miscellaneous Input");
            break;
        case kIOHIDElementTypeInput_Button:
            sprintf(cstrName, "Button Input");
            break;
        case kIOHIDElementTypeInput_Axis:
            sprintf(cstrName, "Axis Input");
            break;
        case kIOHIDElementTypeInput_ScanCodes:
            sprintf(cstrName, "Scan Code Input");
            break;
        case kIOHIDElementTypeOutput:
            sprintf(cstrName, "Output");
            break;
        case kIOHIDElementTypeFeature:
            sprintf(cstrName, "Feature");
            break;
        case kIOHIDElementTypeCollection:
            sprintf(cstrName, "Collection");
            break;
        default:
            sprintf(cstrName, "Unknown Type");
            break;
    }
}

// ---------------------------------
// returns C string usage given usage page and usage passed in as parameters (see IOUSBHIDParser.h)
// returns usage page and usage values in string form for unknown values

void HIDGetUsageName (const long valueUsagePage, const long valueUsage, char * cstrName)
{
	if (xml_GetUsageName(valueUsagePage, valueUsage, cstrName))
		return;

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

// ---------------------------------
// returns calibrated value given raw value passed in
// calibrated value is equal to min and max values returned by HIDGetElementValue since device list built scaled to element reported min and max values

SInt32 HIDCalibrateValue (SInt32 value, pRecElement pElement)
{
	if (NULL != pElement)
	{
		float deviceScale = pElement->max - pElement->min;
		float readScale = pElement->calMax - pElement->calMin;
		if (readScale == 0)
			return value; // no scaling as
		else
			return ((value - pElement->calMin) * deviceScale / readScale) + pElement->min;
	}
	else
		return 0; // bad element passed in
}

// ---------------------------------
// returns scaled value given raw value passed in
// scaled value is equal to current value assumed to be in the range of element reported min and max values scaled to user min and max scaled values

SInt32 HIDScaleValue (SInt32 value, pRecElement pElement)
{
    float deviceScale = pElement->userMax - pElement->userMin;
    float readScale = pElement->max - pElement->min;
    if (readScale == 0)
		return value;
	else
		return (value - pElement->min) * deviceScale / readScale + pElement->userMin;
}

// ---------------------------------
// convert an element type to a mask
HIDElementTypeMask HIDConvertElementTypeToMask (const long type)
{
    HIDElementTypeMask result = kHIDElementTypeAll;

    switch (type)
    {
        case kIOHIDElementTypeInput_Misc:
        case kIOHIDElementTypeInput_Button:
        case kIOHIDElementTypeInput_Axis:
        case kIOHIDElementTypeInput_ScanCodes:
            result = kHIDElementTypeInput;
            break;
        case kIOHIDElementTypeOutput:
            result = kHIDElementTypeOutput;
            break;
        case kIOHIDElementTypeFeature:
            result = kHIDElementTypeFeature;
            break;
        case kIOHIDElementTypeCollection:
            result = kHIDElementTypeCollection;
            break;
        default:
            result = kHIDElementTypeAll;
            break;
    }
    return result;
}

Boolean HIDFindDevice(const pRecDevice pSearchDevice, pRecDevice *ppFoundDevice)
{
    pRecDevice pDevice, pBestDevice = NULL;
    long bestScore = 0;

    // iterate over all devices
    pDevice = HIDGetFirstDevice();
    while (pDevice)
    {
        long deviceScore = 1;

        if (pSearchDevice->vendorID && (pSearchDevice->vendorID == pDevice->vendorID))
        {
            deviceScore += 10;
            if (pSearchDevice->productID && (pSearchDevice->productID == pDevice->productID))
                deviceScore += 8;
        }

        if ((pSearchDevice->usagePage && (pSearchDevice->usagePage == pDevice->usagePage)) &&
            (pSearchDevice->usage && (pSearchDevice->usage == pDevice->usage)))
            deviceScore += 9;

        if (pSearchDevice->locID && (pSearchDevice->locID == pDevice->locID))
            deviceScore += 5;

		if (deviceScore > bestScore)
		{
			pBestDevice = pDevice;
			bestScore = deviceScore;
#if 0	// set true to output scoring informaton
			printf("\n-HIDFindDevice(%ld:%ld)-I-Debug, better score: %ld.",pSearchElement->usagePage, pSearchElement->usage, score);
			HIDPrintElement(pBestElement);
#endif
        }
        pDevice = HIDGetNextDevice(pDevice);
    }

    if (NULL != pBestDevice)
    {
        *ppFoundDevice = pBestDevice;
#if 0	// set true to output scoring informaton
		printf("\n-HIDFindDevice(%ld:%ld)-I-Debug, best score: %ld.",pSearchElement->usagePage, pSearchElement->usage, bestScore);
		HIDPrintElement(pBestElement);
		printf("\n");
#endif
        return true;
    }
    else
        return false;
}

// ---------------------------------
// find the device and element for this action
// Device: serial, vendorID, productID, location, usagePage, usage
// Element: cookie, usagePage, usage,

Boolean HIDFindActionDeviceAndElement(const pRecDevice pSearchDevice, const pRecElement pSearchElement,
									  pRecDevice *ppFoundDevice, pRecElement *ppFoundElement)
{
    pRecDevice pDevice, pBestDevice = NULL;
    pRecElement pElement, pBestElement = NULL;
	HIDElementTypeMask hidMask = HIDConvertElementTypeToMask (pSearchElement->type);
    long bestScore = 0;

    // iterate over all devices
    pDevice = HIDGetFirstDevice();
    while (pDevice)
    {
        long deviceScore = 1;

        if (pSearchDevice->vendorID && (pSearchDevice->vendorID == pDevice->vendorID))
        {
            deviceScore += 10;
            if (pSearchDevice->productID && (pSearchDevice->productID == pDevice->productID))
                deviceScore += 8;
        }
        if ((pSearchDevice->usagePage && (pSearchDevice->usagePage == pDevice->usagePage)) &&
            (pSearchDevice->usage && (pSearchDevice->usage == pDevice->usage)))
            deviceScore += 9;

        if (pSearchDevice->locID && (pSearchDevice->locID == pDevice->locID))
            deviceScore += 5;

        // iterate over all elements of this device
        pElement = HIDGetFirstDeviceElement(pDevice, hidMask);
        while (pElement)
        {
            long score = deviceScore;

            if ((pSearchElement->usagePage && (pSearchElement->usagePage == pElement->usagePage)) &&
                (pSearchElement->usage && (pSearchElement->usage == pElement->usage)))
			{
                score += 5;

				if (pSearchElement->cookie && (pSearchElement->cookie == pElement->cookie))
					score += 4;
			}
			else
				score = 0;
#if 0		// set true to output scoring informaton
			if (kHIDPage_KeyboardOrKeypad != pElement->usagePage)	// skip keyboards here
			{
				printf("\n-HIDFindActionDeviceAndElement(%ld:%ld)-I-Debug, score: %ld.",pSearchElement->usagePage, pSearchElement->usage, score);
				HIDPrintElement(pElement);
			}
#endif
            if (score > bestScore)
            {
                pBestDevice = pDevice;
                pBestElement = pElement;
                bestScore = score;
#if 0		// set true to output scoring informaton
				printf("\n-HIDFindActionDeviceAndElement(%ld:%ld)-I-Debug, better score: %ld.",pSearchElement->usagePage, pSearchElement->usage, score);
				HIDPrintElement(pBestElement);
#endif
            }
            pElement = HIDGetNextDeviceElement(pElement, hidMask);
        }
        pDevice = HIDGetNextDevice(pDevice);
    }

    if ((NULL != pBestDevice) || (NULL != pBestElement))
    {
        *ppFoundDevice = pBestDevice;
        *ppFoundElement = pBestElement;
#if 0		// set true to output scoring informaton
		printf("\n-HIDFindActionDeviceAndElement(%ld:%ld)-I-Debug, best score: %ld.",pSearchElement->usagePage, pSearchElement->usage, bestScore);
		HIDPrintElement(pBestElement);
		printf("\n");
#endif
        return true;
    }
    else
        return false;
}

// ---------------------------------
// find the device and element for this action
// Device: serial, vendorID, productID, location, usagePage, usage
// Element: cookie, usagePage, usage,

Boolean HIDFindSubElement(const pRecElement pStartElement, const pRecElement pSearchElement, pRecElement *ppFoundElement)
{
    pRecElement pElement, pBestElement = NULL;
	HIDElementTypeMask hidMask = HIDConvertElementTypeToMask (pSearchElement->type);
    long bestScore = 0;

    if ((NULL == pStartElement) || (NULL == pSearchElement) || (NULL == ppFoundElement))
        return false;

	// iterate over all children of this element
	pElement = pStartElement->pChild;

	while (pElement)
	{
		long score = 0;
#if 0		// set true to output searching informaton
		printf("\n-HIDFindSubElement, search = {t:%.2lX, u:%.4lX:%.4lX}, match = {t:%.2lX, u:%.4lX:%.4lX, s:\"%s\"}",
		 pSearchElement->type, pSearchElement->usagePage, pSearchElement->usage,
		 pElement->type, pElement->usagePage, pElement->usage, pElement->name);
		fflush(stdout);
#endif
		if ((pSearchElement->usagePage && (pSearchElement->usagePage == pElement->usagePage)) && (pSearchElement->usage && (pSearchElement->usage == pElement->usage)))
		{
			score += 4;
			if (pSearchElement->cookie && (pSearchElement->cookie == pElement->cookie))
				score += 5;
		}
#if 0		// set true to output searching informaton
		if (kHIDPage_KeyboardOrKeypad != pElement->usagePage)	// skip keyboards here
		{
			printf("\n-HIDFindSubElement(%ld:%ld)-I-Debug, score: %ld.",pSearchElement->usagePage, pSearchElement->usage, score);
			HIDPrintElement(pElement);
		}
#endif

		if (score > bestScore)
		{
			pBestElement = pElement;
			bestScore = score;
#if 0		// set true to output searching informaton
			printf("\n-HIDFindSubElement(%ld:%ld)-I-Debug, better score: %ld.",pSearchElement->usagePage, pSearchElement->usage, score);
			HIDPrintElement(pBestElement);
#endif
		}
		pElement = HIDGetNextDeviceElement(pElement, hidMask);
	}
#if 0		// set true to output searching informaton
	if (pBestElement)
	{
		printf("\n-HIDFindSubElement(%ld:%ld)-I-Debug, best score: %ld.",pSearchElement->usagePage, pSearchElement->usage, bestScore);
		HIDPrintElement(pBestElement);
		printf("\n");
	}
#endif

	*ppFoundElement = pBestElement;
	return (NULL != pBestElement);
}

// print out all of an elements information
int HIDPrintElement(const pRecElement pElement)
{
	int results;
	int count;

	printf("\n");

	if (gDepth != pElement->depth)
		printf("%d",gDepth);
	for (count = 0;count < pElement->depth;count++)
		printf("  | ");

#if 0	// this is verbose
	results =
		printf("-HIDPrintElement = {name: \"%s\", t: 0x%.2lX, u:%ld:%ld, c: %ld, min/max: %ld/%ld, scaled: %ld/%ld, size: %ld, rel: %s, wrap: %s, nonLinear: %s, preferred: %s, nullState: %s, units: %ld, exp: %ld, cal: %ld/%ld, user: %ld/%ld, depth: %ld}.",
		 pElement->name,							// name of element (c string)
		 pElement->type,							// the type defined by IOHIDElementType in IOHIDKeys.h
		 pElement->usagePage,					// usage page from IOUSBHIDParser.h which defines general usage
		 pElement->usage,						// usage within above page from IOUSBHIDParser.h which defines specific usage
		 (long) pElement->cookie,				// unique value (within device of specific vendorID and productID) which identifies element, will NOT change
		 pElement->min,							// reported min value possible
		 pElement->max,							// reported max value possible
		 pElement->scaledMin,					// reported scaled min value possible
		 pElement->scaledMax,					// reported scaled max value possible
		 pElement->size,							// size in bits of data return from element
		 pElement->relative ? "YES" : "NO",		// are reports relative to last report (deltas)
		 pElement->wrapping ? "YES" : "NO",		// does element wrap around (one value higher than max is min)
		 pElement->nonLinear ? "YES" : "NO",		// are the values reported non-linear relative to element movement
		 pElement->preferredState ? "YES" : "NO",// does element have a preferred state (such as a button)
		 pElement->nullState ? "YES" : "NO",		// does element have null state
		 pElement->units,						// units value is reported in (not used very often)
		 pElement->unitExp,						// exponent for units (also not used very often)
		 pElement->calMin, 						// min returned value (for calibrate call)
		 pElement->calMax, 						// max returned value
		 pElement->userMin, 						// user set min to scale to (for scale call)
		 pElement->userMax, 						// user set max
		 pElement->depth
		 );
#else	// this is brief
	results =
		printf("-HIDPrintElement = {t: 0x%lX, u:%ld:%ld, c: %ld, name: \"%s\", d: %ld}.",
		 pElement->type,				// the type defined by IOHIDElementType in IOHIDKeys.h
		 pElement->usagePage,			// usage page from IOUSBHIDParser.h which defines general usage
		 pElement->usage,				// usage within above page from IOUSBHIDParser.h which defines specific usage
		 (long) pElement->cookie,		// unique value (within device of specific vendorID and productID) which identifies element, will NOT change
		 pElement->name,				// name of element (c string)
		 pElement->depth
		 );
#endif
	fflush(stdout);
	return results;
}

// return true if this is a valid device pointer
Boolean HIDIsValidDevice(const pRecDevice pSearchDevice)
{
	pRecDevice pDevice = gpDeviceList;

	while (pDevice)
	{
		if (pDevice == pSearchDevice)
			return true;
		pDevice = pDevice->pNext;
	}
	return false;
}

// return true if this is a valid element pointer for this device
Boolean HIDIsValidElement(const pRecDevice pSearchDevice, const pRecElement pSearchElement)
{
	if (HIDIsValidDevice(pSearchDevice))
	{
		pRecElement pRecElementTemp = HIDGetFirstDeviceElement(pSearchDevice,kHIDElementTypeAll);
		while (pRecElementTemp)
		{
			if (pRecElementTemp == pSearchElement)
				return true;
			pRecElementTemp = HIDGetNextDeviceElement(pRecElementTemp,kHIDElementTypeAll);
		}
	}
	return false;
}
#endif //SC_DARWIN