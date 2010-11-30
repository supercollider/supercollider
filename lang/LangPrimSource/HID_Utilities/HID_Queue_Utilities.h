//
// File:       HID_Queue_Utilities.h
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
// Copyright ( C ) 2001-2008 Apple Inc. All Rights Reserved.
//

#ifndef _HID_Queue_Utilities_h_
#define _HID_Queue_Utilities_h_

#include "HID_Utilities.h"

// ==================================

#ifdef __cplusplus
extern "C" {
#endif

// ==================================

enum
{
    kDeviceQueueSize = 50	// this is wired kernel memory so should be set to as small as possible
							// but should account for the maximum possible events in the queue
							// USB updates will likely occur at 100 Hz so one must account for this rate of
							// if states change quickly (updates are only posted on state changes)
};

// ==================================

// Create and open an interface to device, required prior to extracting values or building queues
// int HIDCreateOpenDeviceInterface (void * hidDevice, pRecDevice pDevice);

// queues specific element, performing any device queue set up required
int  HIDQueueElement (pRecDevice pDevice, pRecElement pElement);

// adds all elements to queue, performing any device queue set up required
int  HIDQueueDevice (pRecDevice pDevice);

// removes element for queue, if last element in queue will release queue and device
int  HIDDequeueElement (pRecDevice pDevice, pRecElement pElement);

// completely removes all elements from queue and releases queue and device
int  HIDDequeueDevice (pRecDevice pDevice);

// releases all device queues for quit or rebuild (must be called)
int  HIDReleaseAllDeviceQueues (void);

// releases interface to device, should be done prior to exiting application (called from ReleaseHIDDeviceList)
int  HIDCloseReleaseInterface (pRecDevice pDevice);

// returns true if an event is avialable for the element and fills out *pHIDEvent structure, returns false otherwise
// pHIDEvent is a poiner to a IOHIDEventStruct, using void here for compatibility, users can cast a required
unsigned char HIDGetEvent (pRecDevice pDevice, void * pHIDEvent);

// returns current value for element, creating device interface as required, polling element
// Note: this DOES NOT release the inteface so applications must properly release devices via ReleaseHIDDeviceList
int HIDGetElementValue (pRecDevice pDevice, pRecElement pElement);

#ifdef __cplusplus
}
#endif

#endif // _HID_Queue_Utilities_h_

