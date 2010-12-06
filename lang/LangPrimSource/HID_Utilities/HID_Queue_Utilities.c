//
// File:       HID_Queue_Utilities.c
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

#include "HID_Utilities_External.h"

// ==================================
// private functions

// creates a queue for a device, creates and opens device interface if required
static IOReturn HIDCreateQueue( IOHIDDeviceRef inIOHIDDeviceRef )
{
	IOReturn result = kIOReturnSuccess;
	
	if ( inIOHIDDeviceRef ) {
		assert( IOHIDDeviceGetTypeID() == CFGetTypeID( inIOHIDDeviceRef ) );
		
		// do we already have a queue?
		IOHIDQueueRef tIOHIDQueueRef = IOHIDDevice_GetQueue( inIOHIDDeviceRef );
		
		if ( tIOHIDQueueRef ) { // (yes)
			assert( IOHIDQueueGetTypeID() == CFGetTypeID( tIOHIDQueueRef ) );
		} else {
			tIOHIDQueueRef = IOHIDQueueCreate( kCFAllocatorDefault, inIOHIDDeviceRef, kDeviceQueueSize, kIOHIDOptionsTypeNone );
			
			if ( tIOHIDQueueRef ) { // did that work
				HIDReportErrorNum( "Failed to create queue via create", result );
			} else {
				result = kIOReturnSuccess;
			}
		}
	} else {
		HIDReportErrorNum( "HID device ref does not exist for queue creation", result );
	}
	return result;
} /* HIDCreateQueue */

// ---------------------------------
// returns true if queue is empty false otherwise
// error if no device, empty if no queue
static unsigned char HIDIsDeviceQueueEmpty( IOHIDDeviceRef inIOHIDDeviceRef )
{
	if ( inIOHIDDeviceRef ) { // need device and queue
		assert( IOHIDDeviceGetTypeID() == CFGetTypeID( inIOHIDDeviceRef ) );
		IOHIDQueueRef tIOHIDQueueRef = IOHIDDevice_GetQueue( inIOHIDDeviceRef );
		if ( tIOHIDQueueRef ) {
			IOHIDElementRef tIOHIDElementRef = HIDGetFirstDeviceElement( inIOHIDDeviceRef, kHIDElementTypeIO );
			while ( tIOHIDElementRef ) {
				if ( IOHIDQueueContainsElement( tIOHIDQueueRef, tIOHIDElementRef ) ) {
					return false;
				}
				tIOHIDElementRef = HIDGetNextDeviceElement( tIOHIDElementRef, kHIDElementTypeIO );
			}
		} else {
			HIDReportError( "NULL device passed to HIDIsDeviceQueueEmpty." );
		}
	} else {
		HIDReportError( "NULL device passed to HIDIsDeviceQueueEmpty." );
	}
	return true;
} /* HIDIsDeviceQueueEmpty */

// ---------------------------------

// disposes and releases queue, sets queue to NULL,.
// Note: will have no effect if device or queue do not exist
static IOReturn HIDDisposeReleaseQueue( IOHIDDeviceRef inIOHIDDeviceRef )
{
	IOReturn result = kIOReturnSuccess;
	
	if ( inIOHIDDeviceRef ) {
		IOHIDQueueRef tIOHIDQueueRef = IOHIDDevice_GetQueue( inIOHIDDeviceRef );
		if ( tIOHIDQueueRef ) {
			// stop queue
			IOHIDQueueStop( tIOHIDQueueRef );
			
			// release the queue
			CFRelease( tIOHIDQueueRef );
		}
	} else {
		HIDReportError( "NULL device passed to HIDDisposeReleaseQueue." );
	}
	return result;
} /* HIDDisposeReleaseQueue */

// ==================================
// public functions
// ----------------------------------

// queues specific element, performing any device queue set up required
// queue is started and ready to return events on exit from this function
int  HIDQueueElement( IOHIDDeviceRef inIOHIDDeviceRef, IOHIDElementRef inIOHIDElementRef )
{
	IOReturn result = kIOReturnSuccess;
	
	if ( inIOHIDDeviceRef ) {
		assert( IOHIDDeviceGetTypeID() == CFGetTypeID( inIOHIDDeviceRef ) );
		if ( inIOHIDElementRef ) {
			assert( IOHIDElementGetTypeID() == CFGetTypeID( inIOHIDElementRef ) );
			IOHIDQueueRef tIOHIDQueueRef = IOHIDDevice_GetQueue( inIOHIDDeviceRef );
			if ( !tIOHIDQueueRef ) {         // if no queue create queue
				result = HIDCreateQueue( inIOHIDDeviceRef );
				if ( kIOReturnSuccess == result ) {
					tIOHIDQueueRef = IOHIDDevice_GetQueue( inIOHIDDeviceRef );
				}
			}
			if ( tIOHIDQueueRef ) {
				
				// stop queue
				IOHIDQueueStop( tIOHIDQueueRef );
				
				// queue element
				if ( !IOHIDQueueContainsElement( tIOHIDQueueRef, inIOHIDElementRef ) ) {
					IOHIDQueueAddElement( tIOHIDQueueRef, inIOHIDElementRef );
				}
				
				// restart queue
				IOHIDQueueStart( tIOHIDQueueRef );
			} else {
				HIDReportError( "No queue for device passed to HIDQueueElement." );
				if ( kIOReturnSuccess == result ) {
					result = kIOReturnError;
				}
			}
		} else {
			HIDReportError( "NULL element passed to HIDQueueElement." );
			result = kIOReturnBadArgument;
		}
	} else {
		HIDReportError( "NULL device passed to HIDQueueElement." );
		result = kIOReturnBadArgument;
	}
	return result;
} /* HIDQueueElement */
// ---------------------------------

// adds all elements to queue, performing any device queue set up required
// queue is started and ready to return events on exit from this function
int  HIDQueueDevice( IOHIDDeviceRef inIOHIDDeviceRef )
{
	IOReturn result = kIOReturnSuccess;
	
	// error checking
	if ( !inIOHIDDeviceRef ) {
		HIDReportError( "Device does not exist, cannot queue device." );
		return kIOReturnBadArgument;
	}
	
	if ( !inIOHIDDeviceRef ) { // must have interface
		HIDReportError( "Device does not have hid device ref, cannot queue device." );
		return kIOReturnError;
	}
	
	IOHIDQueueRef tIOHIDQueueRef = IOHIDDevice_GetQueue( inIOHIDDeviceRef );
	if ( !tIOHIDQueueRef ) {         // if no queue create queue
		result = HIDCreateQueue( inIOHIDDeviceRef );
		if ( kIOReturnSuccess == result ) {
			tIOHIDQueueRef = IOHIDDevice_GetQueue( inIOHIDDeviceRef );
		}
	}
	
	if ( ( kIOReturnSuccess != result ) || ( !tIOHIDQueueRef ) ) {
		HIDReportErrorNum( "Could not queue device due to problem creating queue.", result );
		
		if ( kIOReturnSuccess != result ) {
			return result;
		} else {
			return kIOReturnError;
		}
	}
	
	// stop queue
	IOHIDQueueStop( tIOHIDQueueRef );
	
	// queue element
	IOHIDElementRef tIOHIDElementRef = HIDGetFirstDeviceElement( inIOHIDDeviceRef, kHIDElementTypeIO );
	while ( tIOHIDElementRef ) {
		if ( !IOHIDQueueContainsElement( tIOHIDQueueRef, tIOHIDElementRef ) ) {
			IOHIDQueueAddElement( tIOHIDQueueRef, tIOHIDElementRef );
		}
		tIOHIDElementRef = HIDGetNextDeviceElement( tIOHIDElementRef, kHIDElementTypeIO );
	}
	
	// restart queue
	IOHIDQueueStart( tIOHIDQueueRef );
	
	return result;
} /* HIDQueueDevice */

// ---------------------------------
// removes element for queue, if last element in queue will release queue and closes device interface
int  HIDDequeueElement( IOHIDDeviceRef inIOHIDDeviceRef, IOHIDElementRef inIOHIDElementRef )
{
	IOReturn result = kIOReturnSuccess;
	
	if ( inIOHIDDeviceRef ) {
		assert( IOHIDDeviceGetTypeID() == CFGetTypeID( inIOHIDDeviceRef ) );
		if ( inIOHIDElementRef ) {
			assert( IOHIDElementGetTypeID() == CFGetTypeID( inIOHIDElementRef ) );
			IOHIDQueueRef tIOHIDQueueRef = IOHIDDevice_GetQueue( inIOHIDDeviceRef );
			if ( tIOHIDQueueRef ) {
				// stop queue
				IOHIDQueueStop( tIOHIDQueueRef );
				
				// de-queue element
				if ( IOHIDQueueContainsElement( tIOHIDQueueRef, inIOHIDElementRef ) ) {
					IOHIDQueueRemoveElement( tIOHIDQueueRef, inIOHIDElementRef );
				}
				
				// release device queue and close interface if queue empty
				if ( HIDIsDeviceQueueEmpty( inIOHIDDeviceRef ) ) {
					result = HIDDisposeReleaseQueue( inIOHIDDeviceRef );
					
					if ( kIOReturnSuccess != result ) {
						HIDReportErrorNum( "Failed to dispose and release queue.", result );
					}
				} else { // not empty so restart queue
					IOHIDQueueStart( tIOHIDQueueRef );
				}
			} else {
				HIDReportError( "No queue for device passed to HIDDequeueElement." );
				if ( kIOReturnSuccess == result ) {
					result = kIOReturnError;
				}
			}
		} else {
			HIDReportError( "NULL element passed to HIDDequeueElement." );
			result = kIOReturnBadArgument;
		}
	} else {
		HIDReportError( "NULL device passed to HIDDequeueElement." );
		result = kIOReturnBadArgument;
	}
	return result;
} /* HIDDequeueElement */

// ---------------------------------
// completely removes all elements from queue and releases queue and closes device interface
// does not release device interfaces, application must call ReleaseHIDDeviceList on exit
int  HIDDequeueDevice( IOHIDDeviceRef inIOHIDDeviceRef )
{
	
	IOReturn result = kIOReturnSuccess;
	
	// error checking
	if ( !inIOHIDDeviceRef ) {
		HIDReportError( "Device does not exist, cannot queue device." );
		return kIOReturnBadArgument;
	}
	
	if ( !inIOHIDDeviceRef ) { // must have interface
		HIDReportError( "Device does not have hid device ref, cannot queue device." );
		return kIOReturnError;
	}
	
	IOHIDQueueRef tIOHIDQueueRef = IOHIDDevice_GetQueue( inIOHIDDeviceRef );
	
	if ( tIOHIDQueueRef ) {
		// iterate through elements and if queued, remove
		IOHIDElementRef tIOHIDElementRef = HIDGetFirstDeviceElement( inIOHIDDeviceRef, kHIDElementTypeIO );
		while ( tIOHIDElementRef ) {
			// de-queue element
			if ( IOHIDQueueContainsElement( tIOHIDQueueRef, tIOHIDElementRef ) ) {
				IOHIDQueueRemoveElement( tIOHIDQueueRef, tIOHIDElementRef );
			}
			tIOHIDElementRef = HIDGetNextDeviceElement( tIOHIDElementRef, kHIDElementTypeIO );
		}
		// ensure queue is disposed and released
		result = HIDDisposeReleaseQueue( inIOHIDDeviceRef );
		
		if ( kIOReturnSuccess != result ) {
			HIDReportErrorNum( "Failed to dispose and release queue.", result );
		}
	} else {
		HIDReportError( "No queue for device passed to HIDDequeueElement." );
		if ( kIOReturnSuccess == result ) {
			result = kIOReturnError;
		}
	}
	return result;
} /* HIDDequeueDevice */
// ---------------------------------

// releases all device queues for quit or rebuild (must be called)
// does not release device interfaces, application must call ReleaseHIDDeviceList on exit
IOReturn HIDReleaseAllDeviceQueues( void )
{
	IOReturn result = kIOReturnSuccess;
	IOHIDDeviceRef tIOHIDDeviceRef = HIDGetFirstDevice();
	while ( tIOHIDDeviceRef ) {
		result = HIDDequeueDevice( tIOHIDDeviceRef );
		
		if ( kIOReturnSuccess != result ) {
			HIDReportErrorNum( "Could not dequeue device.", result );
		}
		tIOHIDDeviceRef = HIDGetNextDevice( tIOHIDDeviceRef );
	}
	return result;
} /* HIDReleaseAllDeviceQueues */

// ---------------------------------
// Get the next event in the queue for a device
// elements or entire device should be queued prior to calling this with HIDQueueElement or HIDQueueDevice
// returns true if an event is avialable for the element and fills out *pHIDEvent structure, returns false otherwise
// Note: kIOReturnUnderrun returned from getNextEvent indicates an empty queue not an error condition
// Note: application should pass in a pointer to a IOHIDEventStruct cast to a void (for CFM compatibility)
unsigned char HIDGetEvent( IOHIDDeviceRef inIOHIDDeviceRef, IOHIDValueRef * pIOHIDValueRef )
{
	if ( inIOHIDDeviceRef ) {
		IOHIDQueueRef tIOHIDQueueRef = IOHIDDevice_GetQueue( inIOHIDDeviceRef );
		if ( tIOHIDQueueRef ) {
			if ( pIOHIDValueRef ) {
				*pIOHIDValueRef = IOHIDQueueCopyNextValueWithTimeout( tIOHIDQueueRef, 0.0 );
				
				if ( *pIOHIDValueRef ) {
					return true;
				}
			}
		} else {
			HIDReportError( "Could not get HID event, hid queue reference does not exist." );
		}
	} else {
		HIDReportError( "Could not get HID event, device does not exist." );
	}
	return false;     // did not get event
} /* HIDGetEvent */
