/*
	File:		HID_Queue_Utilities.c

	Contains:	Implementation of the HID queue functions for the HID utilites.

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

//#include <IOKit/hid/IOHIDDevice.h>
#ifdef SC_DARWIN
#include "HID_Utilities_Internal.h"
#include "HID_Utilities_External.h"

// ==================================
// compiler directives
// ==================================
#define USE_ASYNC_EVENTS TRUE
#define REPORT_ERRORS FALSE
// ==================================
#if REPORT_ERRORS
#define HIDREPORTERRORNUM(s,n) HIDReportErrorNum(s,n)
#define HIDREPORTERROR(s)		HIDReportError(s)
#else
#define HIDREPORTERRORNUM(s,n)	do {} while (false)
#define HIDREPORTERROR(s)		do {} while (false)
#endif
// ==================================
// private functions
// ==================================

// creates a queue for a device

static IOReturn hid_CreateQueue (pRecDevice pDevice)
{
    IOReturn result = kIOReturnError;	// assume failure (pessimist!)

	if (HIDIsValidDevice(pDevice))
	{
		if (NULL == pDevice->queue) // do we already have a queue
		{
			if (NULL != pDevice->interface)
			{
				pDevice->queue = (void *) (*(IOHIDDeviceInterface**) pDevice->interface)->allocQueue (pDevice->interface); // alloc queue
				if (pDevice->queue)
				{
					result = (*(IOHIDQueueInterface**) pDevice->queue)->create (pDevice->queue, 0, kDeviceQueueSize); // create actual queue
					if (kIOReturnSuccess != result)
						HIDREPORTERRORNUM ("hid_CreateQueue - Failed to create queue via create", result);
				}
				else
				{
					HIDREPORTERROR ("hid_CreateQueue - Failed to alloc IOHIDQueueInterface ** via allocQueue");
					result = kIOReturnError; // synthesis error
				}
			}
			else
				HIDREPORTERRORNUM ("hid_CreateQueue - Device inteface does not exist for queue creation", result);
		}
	}
	else
		HIDREPORTERRORNUM ("hid_CreateQueue - Invalid Device", result);
    return result;
}

// ---------------------------------
// returns true if queue is empty false otherwise
// error if no device, empty if no queue

static unsigned char hid_IsDeviceQueueEmpty (pRecDevice pDevice)
{
	if (HIDIsValidDevice(pDevice))	// need valid device
	{
		if (pDevice->queue) // and queue
		{
			pRecElement pElement = HIDGetFirstDeviceElement (pDevice, kHIDElementTypeIO);
			while (pElement)
			{
				if ((*(IOHIDQueueInterface**) pDevice->queue)->hasElement (pDevice->queue, pElement->cookie))
					return false;
				pElement = HIDGetNextDeviceElement (pElement, kHIDElementTypeIO);
			}
		}
		else
			HIDREPORTERROR ("hid_IsDeviceQueueEmpty - no queue.");
	}
	else
		HIDREPORTERROR ("hid_IsDeviceQueueEmpty - Invalid device.");
	return true;
}

// ---------------------------------
// disposes and releases queue, sets queue to NULL,.
// Note: will have no effect if device or queue do not exist

static IOReturn hid_DisposeReleaseQueue (pRecDevice pDevice)
{
    IOReturn result = kIOReturnError;	// assume failure (pessimist!)

	if (HIDIsValidDevice(pDevice))	// need valid device
	{
		if (pDevice->queue) // and queue
		{
			// stop queue
			result = (*(IOHIDQueueInterface**) pDevice->queue)->stop (pDevice->queue);
			if (kIOReturnSuccess != result)
				HIDREPORTERRORNUM ("hid_DisposeReleaseQueue - Failed to stop queue.", result);
			// dispose of queue
			result = (*(IOHIDQueueInterface**) pDevice->queue)->dispose (pDevice->queue);
			if (kIOReturnSuccess != result)
				HIDREPORTERRORNUM ("hid_DisposeReleaseQueue - Failed to dipose queue.", result);
			// release the queue
			result = (*(IOHIDQueueInterface**) pDevice->queue)->Release (pDevice->queue);
			if (kIOReturnSuccess != result)
				HIDREPORTERRORNUM ("hid_DisposeReleaseQueue - Failed to release queue.", result);

			pDevice->queue = NULL;
		}
		else
			HIDREPORTERROR ("hid_DisposeReleaseQueue - no queue.");
	}
	else
		HIDREPORTERROR ("hid_DisposeReleaseQueue - Invalid device.");
    return result;
}

// ==================================
// public functions

// ---------------------------------
// queues specific element, performing any device queue set up required
// queue is started and ready to return events on exit from this function

unsigned long  HIDQueueElement (pRecDevice pDevice, pRecElement pElement)
{
    IOReturn result = kIOReturnError;	// assume failure (pessimist!)

	if (HIDIsValidElement(pDevice,pElement))
	{
		if (NULL == pDevice->interface) // must have interface
		{
			HIDREPORTERROR ("HIDQueueElement - Device does not have interface.");
			return kIOReturnError;
		}
		if (NULL == pDevice->queue) // if no queue create queue
			result = hid_CreateQueue (pDevice);
		if ((kIOReturnSuccess != result) || (NULL == pDevice->queue))
		{
			HIDREPORTERRORNUM ("HIDQueueElement - problem creating queue.", result);
			if (kIOReturnSuccess != result)
				return result;
			else
				return kIOReturnError;
		}

		// stop queue
		result = (*(IOHIDQueueInterface**) pDevice->queue)->stop (pDevice->queue);
		if (kIOReturnSuccess != result)
			HIDREPORTERROR ("HIDQueueElement - Failed to stop queue.");

		// queue element
		if (!(*(IOHIDQueueInterface**) pDevice->queue)->hasElement (pDevice->queue, pElement->cookie))
		{
			result = (*(IOHIDQueueInterface**) pDevice->queue)->addElement (pDevice->queue, pElement->cookie, 0);
			if (kIOReturnSuccess != result)
				HIDREPORTERROR ("HIDQueueElement - Failed to add Element to queue.");
		}

		// restart queue
		result = (*(IOHIDQueueInterface**) pDevice->queue)->start (pDevice->queue);
		if (kIOReturnSuccess != result)
			HIDREPORTERROR ("HIDQueueElement - Failed to start queue.");
	}
	else
	{
		HIDREPORTERROR ("HIDQueueElement - Invalid Device and/or element.");
        return kIOReturnBadArgument;
	}

   return result;
}

// ---------------------------------
// adds all elements to queue, performing any device queue set up required
// queue is started and ready to return events on exit from this function

unsigned long  HIDQueueDevice (pRecDevice pDevice)
{
    IOReturn result = kIOReturnError;	// assume failure (pessimist!)
    pRecElement pElement;

	if (HIDIsValidDevice(pDevice))
	{
		// error checking
		if (NULL == pDevice)
		{
			HIDREPORTERROR ("HIDQueueDevice - Device does not exist.");
			return kIOReturnBadArgument;
		}
		if (NULL == pDevice->interface) // must have interface
		{
			HIDREPORTERROR ("HIDQueueDevice - Device does not have interface.");
			return kIOReturnError;
		}
		if (NULL == pDevice->queue) // if no queue create queue
			result = hid_CreateQueue (pDevice);
		if ((kIOReturnSuccess != result) || (NULL == pDevice->queue))
		{
			HIDREPORTERRORNUM ("HIDQueueDevice - problem creating queue.", result);
			if (kIOReturnSuccess != result)
				return result;
			else
				return kIOReturnError;
		}

		// stop queue
		result = (*(IOHIDQueueInterface**) pDevice->queue)->stop (pDevice->queue);
		if (kIOReturnSuccess != result)
			HIDREPORTERRORNUM ("HIDQueueDevice - Failed to stop queue.", result);

		// queue element
  //• pElement = HIDGetFirstDeviceElement (pDevice, kHIDElementTypeIO);
		pElement = HIDGetFirstDeviceElement (pDevice, kHIDElementTypeInput | kHIDElementTypeFeature);

		while (pElement)
		{
			if (!(*(IOHIDQueueInterface**) pDevice->queue)->hasElement (pDevice->queue, pElement->cookie))
			{
				result = (*(IOHIDQueueInterface**) pDevice->queue)->addElement (pDevice->queue, pElement->cookie, 0);
				if (kIOReturnSuccess != result)
					HIDREPORTERRORNUM ("HIDQueueDevice - Failed to add element to queue.", result);
			}
			//• pElement = HIDGetNextDeviceElement (pElement, kHIDElementTypeIO);
			pElement = HIDGetNextDeviceElement (pElement, kHIDElementTypeInput | kHIDElementTypeFeature);
		}

		// start queue
		result = (*(IOHIDQueueInterface**) pDevice->queue)->start (pDevice->queue);
		if (kIOReturnSuccess != result)
			HIDREPORTERRORNUM ("HIDQueueDevice - Failed to start queue.", result);

	}
	else
		HIDREPORTERROR ("HIDQueueDevice - Invalid device.");

    return result;
}

// ---------------------------------
// removes element for queue, if last element in queue will release queue and closes device interface

unsigned long  HIDDequeueElement (pRecDevice pDevice, pRecElement pElement)
{
    IOReturn result = kIOReturnError;	// assume failure (pessimist!)

	if (HIDIsValidElement(pDevice,pElement))
	{
		if (!pDevice || !pElement)
			result = kIOReturnBadArgument;
		else
		{
			if ((pDevice->interface) && (pDevice->queue))
			{
				// stop queue
				result = (*(IOHIDQueueInterface**) pDevice->queue)->stop (pDevice->queue);
				if (kIOReturnSuccess != result)
					HIDREPORTERRORNUM ("HIDDequeueElement - Failed to stop queue.", result);

				if ((*(IOHIDQueueInterface**) pDevice->queue)->hasElement (pDevice->queue, pElement->cookie)) // if has element then remove
				{
					result = (*(IOHIDQueueInterface**) pDevice->queue)->removeElement (pDevice->queue, pElement->cookie);
					if (kIOReturnSuccess != result)
						HIDREPORTERRORNUM ("HIDDequeueElement - Failed to add element to queue", result);
				}

				if (hid_IsDeviceQueueEmpty (pDevice)) // release device queue and close interface if queue empty
				{
					result = hid_DisposeReleaseQueue (pDevice);
					if (kIOReturnSuccess != result)
						HIDREPORTERRORNUM ("HIDDequeueElement - Failed to dispose and release queue.", result);
				}
				else // not empty so restart queue
				{
					result = (*(IOHIDQueueInterface**) pDevice->queue)->start (pDevice->queue);
					if (kIOReturnSuccess != result)
						HIDREPORTERRORNUM ("HIDDequeueElement - Failed to start queue.", result);
				}
			}
			else
			{
				HIDREPORTERROR ("HIDDequeueElement - No device inteface or queue.");
				return kIOReturnError;
			}
		}
	}
	else
		HIDREPORTERROR ("HIDDequeueElement - Invalid device.");

    return result;
}

// ---------------------------------
// completely removes all elements from queue and releases queue and closes device interface
// does not release device interfaces, application must call HIDReleaseDeviceList on exit

unsigned long  HIDDequeueDevice (pRecDevice pDevice)
{
    IOReturn result = kIOReturnSuccess;

    if (HIDIsValidDevice(pDevice))
	{
		if ((pDevice->interface) && (pDevice->queue))
		{
			// iterate through elements and if queued, remove
			pRecElement pElement = HIDGetFirstDeviceElement (pDevice, kHIDElementTypeIO);
			while (pElement)
			{
				if ((*(IOHIDQueueInterface**) pDevice->queue)->hasElement (pDevice->queue, pElement->cookie))
				{
					result = (*(IOHIDQueueInterface**) pDevice->queue)->removeElement (pDevice->queue, pElement->cookie);
					if (kIOReturnSuccess != result)
						HIDREPORTERRORNUM ("HIDDequeueDevice - Failed to remove element from queue.", result);
				}
				pElement = HIDGetNextDeviceElement (pElement, kHIDElementTypeIO);
			}
		}
		// ensure queue is disposed and released
		// interface will be closed and released on call to HIDReleaseDeviceList
		result = hid_DisposeReleaseQueue (pDevice);
		if (kIOReturnSuccess != result)
			HIDREPORTERRORNUM ("removeElement - Failed to dispose and release queue.", result);
#if USE_ASYNC_EVENTS
		else if (NULL != pDevice->queueRunLoopSource)
		{
			if (CFRunLoopContainsSource(CFRunLoopGetCurrent(), pDevice->queueRunLoopSource, kCFRunLoopDefaultMode))
				CFRunLoopRemoveSource(CFRunLoopGetCurrent(), pDevice->queueRunLoopSource, kCFRunLoopDefaultMode);
			CFRelease(pDevice->queueRunLoopSource);
			pDevice->queueRunLoopSource = NULL;
		}
#endif USE_ASYNC_EVENTS
	}
	else
	{
		HIDREPORTERROR ("HIDDequeueDevice - Invalid device.");
		result = kIOReturnBadArgument;
	}
    return result;
}

// ---------------------------------
// releases all device queues for quit or rebuild (must be called)
// does not release device interfaces, application must call HIDReleaseDeviceList on exit

unsigned long HIDReleaseAllDeviceQueues (void)
{
    IOReturn result = kIOReturnBadArgument;
    pRecDevice pDevice = HIDGetFirstDevice ();

    while (pDevice)
    {
        result = HIDDequeueDevice (pDevice);
        if (kIOReturnSuccess != result)
            HIDREPORTERRORNUM ("HIDReleaseAllDeviceQueues - Could not dequeue device.", result);
        pDevice = HIDGetNextDevice (pDevice);
    }
    return result;
}

// ---------------------------------
// Closes and releases interface to device, should be done prior to exting application
// Note: will have no affect if device or interface do not exist
// application will "own" the device if interface is not closed
// (device may have to be plug and re-plugged in different location to get it working again without a restart)

unsigned long HIDCloseReleaseInterface (pRecDevice pDevice)
{
	IOReturn result = kIOReturnSuccess;

	if (HIDIsValidDevice(pDevice) && (NULL != pDevice->interface))
	{
		// close the interface
		result = (*(IOHIDDeviceInterface**) pDevice->interface)->close (pDevice->interface);
		if (kIOReturnNotOpen == result)
		{
			//  do nothing as device was not opened, thus can't be closed
		}
		else if (kIOReturnSuccess != result)
			HIDREPORTERRORNUM ("HIDCloseReleaseInterface - Failed to close IOHIDDeviceInterface.", result);
		//release the interface
		result = (*(IOHIDDeviceInterface**) pDevice->interface)->Release (pDevice->interface);
		if (kIOReturnSuccess != result)
			HIDREPORTERRORNUM ("HIDCloseReleaseInterface - Failed to release interface.", result);
		pDevice->interface = NULL;
	}
	return result;
}

// ---------------------------------
// Get the next event in the queue for a device
// elements or entire device should be queued prior to calling this with HIDQueueElement or HIDQueueDevice
// returns true if an event is avialable for the element and fills out *pHIDEvent structure, returns false otherwise
// Note: kIOReturnUnderrun returned from getNextEvent indicates an empty queue not an error condition
// Note: application should pass in a pointer to a IOHIDEventStruct cast to a void (for CFM compatibility)

unsigned char HIDGetEvent (pRecDevice pDevice, void * pHIDEvent)
{
    IOReturn result = kIOReturnBadArgument;
    AbsoluteTime zeroTime = {0,0};

    if (HIDIsValidDevice(pDevice))
	{
		if (pDevice->queue)
		{
			result = (*(IOHIDQueueInterface**) pDevice->queue)->getNextEvent (pDevice->queue, (IOHIDEventStruct *)pHIDEvent, zeroTime, 0);
			if (kIOReturnUnderrun == result)
				return false;  // no events in queue not an error per say
			else if (kIOReturnSuccess != result) // actual error versus just an empty queue
				HIDREPORTERRORNUM ("HIDGetEvent - Could not get HID event via getNextEvent.", result);
			else
				return true;
		}
		else
			HIDREPORTERROR ("HIDGetEvent - queue does not exist.");
	}
	else
		HIDREPORTERROR ("HIDGetEvent - invalid device.");

    return false; // did not get event
}

// ---------------------------------
// returns current value for element, polling element
// will return 0 on error conditions which should be accounted for by application

long HIDGetElementValue (pRecDevice pDevice, pRecElement pElement)
{
    IOReturn result = kIOReturnBadArgument;
    IOHIDEventStruct hidEvent;

	hidEvent.value = 0;
    hidEvent.longValueSize = 0;
	hidEvent.longValue = nil;

	if (HIDIsValidElement(pDevice, pElement))
	{
		if (NULL != pDevice->interface)
		{
			// ++ NOTE: If the element type is feature then use queryElementValue instead of getElementValue
			if (kIOHIDElementTypeFeature == pElement->type)
			{
				result = (*(IOHIDDeviceInterface**) pDevice->interface)->queryElementValue (pDevice->interface, pElement->cookie, &hidEvent,0,NULL,NULL,NULL);
				if (kIOReturnUnsupported == result)	// unless it's unsuported.
					goto try_getElementValue;
				else if (kIOReturnSuccess != result)
					HIDREPORTERRORNUM ("HIDGetElementValue - Could not get HID element value via queryElementValue.", result);
			}
			else if (pElement->type <= kIOHIDElementTypeInput_ScanCodes)
			{
try_getElementValue:
				result = (*(IOHIDDeviceInterface**) pDevice->interface)->getElementValue (pDevice->interface, pElement->cookie, &hidEvent);
				if (kIOReturnSuccess != result)
					HIDREPORTERRORNUM ("HIDGetElementValue - Could not get HID element value via getElementValue.", result);
			}
			// on 10.0.x this returns the incorrect result for negative ranges, so fix it!!!
   // this is not required on Mac OS X 10.1+
			if ((pElement->min < 0) && (hidEvent.value > pElement->max)) // assume range problem
				hidEvent.value = hidEvent.value + pElement->min - pElement->max - 1;
		}
		else
			HIDREPORTERROR ("HIDGetElementValue - no interface for device.");
	}
	else
		HIDREPORTERROR ("HIDGetElementValue - invalid device and/or element.");

    // record min and max for auto scale and auto ...
    if (hidEvent.value < pElement->calMin)
        pElement->calMin = hidEvent.value;
    if (hidEvent.value > pElement->calMax)
        pElement->calMax = hidEvent.value;

    // auto user scale
    return hidEvent.value;
}

// ---------------------------------
// Set an elements value
// NOTE: This should only be used when a single element of a report needs to be sent.
// If multiple elements are to be send then transactions should be used.

long HIDSetElementValue (pRecDevice pDevice, pRecElement pElement,void* pIOHIDEvent)
{
	IOHIDEventStruct* pMyIOHIDEvent = (IOHIDEventStruct*) pIOHIDEvent;
    IOReturn result = kIOReturnError;	// assume failure (pessimist!)

	if (HIDIsValidElement(pDevice, pElement))
	{
		if (NULL != pDevice->interface)
		{
			result = (*(IOHIDDeviceInterface**) pDevice->interface)->setElementValue (pDevice->interface, pElement->cookie, pMyIOHIDEvent, -1, nil, nil, nil);
			if (kIOReturnSuccess != result)
				HIDREPORTERRORNUM ("HIDSetElementValue - Could not set HID element value via setElementValue.", result);
		}
		else
			HIDREPORTERROR ("HIDSetElementValue - no interface for device.");
	}
	else
		HIDREPORTERROR ("HIDSetElementValue - invalid device and/or element.");

    return result;
}

// ---------------------------------
// Set a callback to be called when a queue goes from empty to non-empty
long HIDSetQueueCallback (pRecDevice pDevice, IOHIDCallbackFunction callback)
{
    IOReturn result = kIOReturnError;	// assume failure (pessimist!)

	if (HIDIsValidDevice(pDevice))
	{
#if USE_ASYNC_EVENTS
		// if we don't have a queue runloop source…
		if (NULL == pDevice->queueRunLoopSource)
		{
			// then create one
			result = (*(IOHIDQueueInterface**) pDevice->queue)->createAsyncEventSource((void *) pDevice->queue,
				(CFRunLoopSourceRef*) &pDevice->queueRunLoopSource);
			if (kIOReturnSuccess != result)
				HIDREPORTERRORNUM ("HIDSetQueueCallback - Failed to createAsyncEventSource error: ", result);
		}
		// if we have one now…
		if (NULL != pDevice->queueRunLoopSource)
		{
			// and it's not already attached to our runloop…
			if (!CFRunLoopContainsSource(CFRunLoopGetCurrent(), pDevice->queueRunLoopSource, kCFRunLoopDefaultMode))
				// then attach it now.
				CFRunLoopAddSource(CFRunLoopGetCurrent(), pDevice->queueRunLoopSource, kCFRunLoopDefaultMode);
		}

		// now install our callback
		result = (*(IOHIDQueueInterface**) pDevice->queue)->setEventCallout(pDevice->queue, callback, pDevice, pDevice);
		if (kIOReturnSuccess != result)
			HIDREPORTERRORNUM ("HIDSetQueueCallback - Could not set HID queue callback via setEventCallout.", result);
#endif USE_ASYNC_EVENTS
	}
	else
		HIDREPORTERROR ("HIDSetQueueCallback - invalid device and/or element.");
    return result;
}

#if 1
// ---------------------------------
// Get a report from a device
long HIDGetReport (pRecDevice pDevice,const IOHIDReportType reportType, const UInt32 reportID, void* reportBuffer, UInt32* reportBufferSize)
{
    IOReturn result = kIOReturnError;	// assume failure (pessimist!)

    if (HIDIsValidDevice(pDevice))
    {
		if (NULL != pDevice->interface)
		{
			result = (*(IOHIDDeviceInterface**) pDevice->interface)->getReport (pDevice->interface, reportType, reportID, reportBuffer, reportBufferSize, -1, nil, nil, nil);
			if (kIOReturnSuccess != result)
				HIDREPORTERRORNUM ("HIDGetReport - Could not getReport, error: ", result);
		}
		else
			HIDREPORTERROR ("HIDGetReport - no interface for device.");
    }
    else
        HIDREPORTERROR ("HIDGetReport - invalid device.");

    return result;
}

// ---------------------------------
// Send a report to a device
long HIDSetReport (pRecDevice pDevice,const IOHIDReportType reportType, const UInt32 reportID, void* reportBuffer, const UInt32 reportBufferSize)
{
    IOReturn result = kIOReturnError;	// assume failure (pessimist!)

    if (HIDIsValidDevice(pDevice))
    {
		if (NULL != pDevice->interface)
		{
			result = (*(IOHIDDeviceInterface**) pDevice->interface)->setReport (pDevice->interface, reportType, reportID, reportBuffer, reportBufferSize, -1, nil, nil, nil);
			if (kIOReturnSuccess != result)
				HIDREPORTERRORNUM ("HIDGetReport - Could not setReport; error: ", result);
		}
		else
			HIDREPORTERROR ("HIDGetReport - no interface for device.");
    }
    else
        HIDREPORTERROR ("HIDGetReport - invalid device.");

    return result;
}
#endif
#endif //SC_DARWIN