/*
 *  SC_HID.cpp
 *  SC3lang
 *
 *  Created by Jan Truetzschler v. Falkenstein on Tue Sep 30 2003.
 *  Copyright (c) 2003 jan.t. All rights reserved.
 *  modifications by Marije Baalman in May 2007
 *	part of ...
	SuperCollider real time audio synthesis system
    Copyright (c) 2002 James McCartney. All rights reserved.
	http://www.audiosynth.com

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA

 */
#ifdef SC_DARWIN

#include <Carbon/Carbon.h>

#include "HID_Utilities_External.h"

/*
#include <IOKit/IOKitLib.h>
#include <IOKit/IOCFPlugIn.h>
#include <IOKit/hid/IOHIDLib.h>
#include <IOKit/hid/IOHIDKeys.h>
*/
#include <IOKit/hid/IOHIDUsageTables.h>

#include <mach/mach.h>
#include <mach/mach_error.h>
#include "SCBase.h"
#include "VMGlobals.h"
#include "PyrSymbolTable.h"
#include "PyrInterpreter.h"
#include "PyrKernel.h"

#include "PyrObjectProto.h"
#include "PyrPrimitiveProto.h"
#include "PyrKernelProto.h"
#include "SC_InlineUnaryOp.h"
#include "SC_InlineBinaryOp.h"
#include "PyrSched.h"
#include "GC.h"

PyrSymbol * s_readError;
PyrSymbol * s_hidAction;
PyrSymbol * s_hid;
extern bool compiledOK;
int gNumberOfHIDDevices = 0;
EventLoopTimerRef gTimer = NULL; // timer for element data updates

void releaseHIDDevices ();
void releaseHIDDevices ()
{
	if (gTimer)
    {
		RemoveEventLoopTimer(gTimer);
		gTimer = NULL;
	}
	HIDReleaseAllDeviceQueues();
	HIDReleaseDeviceList();
	gNumberOfHIDDevices = 0;
}

int prHIDGetElementListSize(VMGlobals *g, int numArgsPushed);
int prHIDGetElementListSize(VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a = g->sp - 1; //class
	PyrSlot *b = g->sp; //locID device
	int locID;
	int err = slotIntVal(b, &locID);
	if (err) return err;
    IOHIDDeviceRef  pCurrentHIDDevice = HIDGetFirstDevice ();
	while (pCurrentHIDDevice && (IOHIDDevice_GetLocationID(pCurrentHIDDevice) !=locID))
        pCurrentHIDDevice = HIDGetNextDevice (pCurrentHIDDevice);
	if(!pCurrentHIDDevice) return errFailed;
	UInt32 numElements = HIDCountDeviceElements (pCurrentHIDDevice, kHIDElementTypeAll );
	SetInt(a, numElements);
	return errNone;
}

int prHIDBuildElementList(VMGlobals *g, int numArgsPushed);
int prHIDBuildElementList(VMGlobals *g, int numArgsPushed)
{

	PyrSlot *a = g->sp - 2; //class
	PyrSlot *b = g->sp - 1; //locID device
	PyrSlot *c = g->sp; //array

	int locID;
	int err = slotIntVal(b, &locID);
	if (err) return err;
	//look for the right device:
    IOHIDDeviceRef  pCurrentHIDDevice = HIDGetFirstDevice ();
	while (pCurrentHIDDevice && (IOHIDDevice_GetLocationID(pCurrentHIDDevice) !=locID))
        pCurrentHIDDevice = HIDGetNextDevice (pCurrentHIDDevice);
	if(!pCurrentHIDDevice) return errFailed;

	IOHIDElementRef	devElement =  HIDGetFirstDeviceElement (pCurrentHIDDevice, kHIDElementTypeAll );
	UInt32 numElements = HIDCountDeviceElements (pCurrentHIDDevice, kHIDElementTypeAll );

//		PyrObject* devAllElementsArray = newPyrArray(g->gc, numElements * sizeof(PyrObject), 0 , true);
		PyrObject *devAllElementsArray = slotRawObject(c);
//		post("numElements: %d\n", numElements);
		numElements = sc_clip(numElements, 0, devAllElementsArray->size);
		for(uint i=0; i<numElements; i++){
			if(devElement){
				char cstrElementName [256];
				PyrObject* devElementArray = newPyrArray(g->gc, 8 * sizeof(PyrObject), 0 , true);
				// type name (1)
				HIDGetTypeName(IOHIDElementGetType(devElement), cstrElementName);
				PyrString *devstring = newPyrString(g->gc, cstrElementName, 0, true);
				SetObject(devElementArray->slots+devElementArray->size++, devstring);
				//g->gc->GCWrite(devElementArray, (PyrObject*) devstring);
				//usage (2)
				CFStringRef stringref = HIDCopyUsageName(IOHIDElementGetUsagePage(devElement), IOHIDElementGetUsage(devElement));
				CFStringGetCString(stringref, cstrElementName, 256, kCFStringEncodingASCII);
				CFRelease(stringref);
				PyrString *usestring = newPyrString(g->gc, cstrElementName, 0, true);
				SetObject(devElementArray->slots+devElementArray->size++, usestring);
				//g->gc->GCWrite(devElementArray, (PyrObject*) usestring);
				//cookie (3)
				SetInt(devElementArray->slots+devElementArray->size++, (long) IOHIDElementGetCookie(devElement));
				// min (4)
				SetInt(devElementArray->slots+devElementArray->size++, (long) IOHIDElementGetLogicalMin(devElement));
				// max (5)
				SetInt(devElementArray->slots+devElementArray->size++, (long) IOHIDElementGetLogicalMax(devElement));

				// IO type as int: (6)
				SetInt(devElementArray->slots+devElementArray->size++, (int) IOHIDElementGetType(devElement));
				// Usage page as int: (7)
				SetInt(devElementArray->slots+devElementArray->size++, (long) IOHIDElementGetUsagePage(devElement));
				// Usage type as int: (8)
				SetInt(devElementArray->slots+devElementArray->size++, (long) IOHIDElementGetUsage(devElement));

				SetObject(devAllElementsArray->slots+i, devElementArray);
				//g->gc->GCWrite(devAllElementsArray, (PyrObject*) devElementArray);
			}
			devElement =  HIDGetNextDeviceElement (devElement, kHIDElementTypeAll);
		}
	SetObject(a, devAllElementsArray);
	return errNone;

}


int prHIDBuildDeviceList(VMGlobals *g, int numArgsPushed);
int prHIDBuildDeviceList(VMGlobals *g, int numArgsPushed)
{
	//build a device list
	PyrSlot *a = g->sp - 2;
	PyrSlot *b = g->sp - 1; //usagePage
	PyrSlot *c = g->sp;		//usage

	int usagePage, usage, err;
	if(IsNil(b))
		usagePage = 0;
	else
	{
		err = slotIntVal(b, &usagePage);
		if (err) return err;
	}
	if(IsNil(c))
		usage = 0;
	else
	{
		err = slotIntVal(c, &usage);
		if (err) return err;
	}

	//pass in usage & usagepage
	//kHIDUsage_GD_Joystick kHIDUsage_GD_GamePad
	//UInt32 usagePage = kHIDPage_GenericDesktop;
	//UInt32 usage = NULL;

	Boolean result = HIDBuildDeviceList (usagePage, usage);
	// returns false if no device found (ignored in this case) - returns always false ?

	if(result) post("no HID devices found\n");

	int numdevs = HIDCountDevices();
	gNumberOfHIDDevices = numdevs;
	if(!numdevs){
		SetNil(a);
		return errNone;
	}
	//post("number of devices: %d", numdevs);

    IOHIDDeviceRef  pCurrentHIDDevice = HIDGetFirstDevice ();
	PyrObject* allDevsArray = newPyrArray(g->gc, numdevs * sizeof(PyrObject), 0 , true);
	for(int i=0; i<numdevs; i++){
		char tmp[256];
		CFStringRef stringref;
		//device:
		PyrObject* devNameArray = newPyrArray(g->gc, 8 * sizeof(PyrObject), 0 , true);
		//manufacturer:
		stringref = IOHIDDevice_GetManufacturer(pCurrentHIDDevice);
		CFStringGetCString(stringref, tmp, 256, kCFStringEncodingASCII);
		CFRelease(stringref);
		PyrString *devstring = newPyrString(g->gc, tmp, 0, true);
		SetObject(devNameArray->slots+devNameArray->size++, devstring);
		g->gc->GCWrite(devNameArray, (PyrObject*) devstring);
		//product name:
		stringref = IOHIDDevice_GetProduct(pCurrentHIDDevice);
		CFStringGetCString(stringref, tmp, 256, kCFStringEncodingASCII);
		CFRelease(stringref);
		devstring = newPyrString(g->gc, tmp, 0, true);
		SetObject(devNameArray->slots+devNameArray->size++, devstring);
		g->gc->GCWrite(devNameArray, (PyrObject*) devstring);
		//usage
		stringref = HIDCopyUsageName(IOHIDDevice_GetUsagePage(pCurrentHIDDevice), IOHIDDevice_GetUsage(pCurrentHIDDevice));
		CFStringGetCString(stringref, tmp, 256, kCFStringEncodingASCII);
		CFRelease(stringref);
		devstring = newPyrString(g->gc, tmp, 0, true);
		SetObject(devNameArray->slots+devNameArray->size++, devstring);
		g->gc->GCWrite(devNameArray, (PyrObject*) devstring);
		//vendor id
		SetInt(devNameArray->slots+devNameArray->size++, IOHIDDevice_GetVendorID(pCurrentHIDDevice));
		//product id
		SetInt(devNameArray->slots+devNameArray->size++, IOHIDDevice_GetProductID(pCurrentHIDDevice));
		//locID
		SetInt(devNameArray->slots+devNameArray->size++, IOHIDDevice_GetLocationID(pCurrentHIDDevice));

		//version
		SetInt(devNameArray->slots+devNameArray->size++, IOHIDDevice_GetVersionNumber(pCurrentHIDDevice));

		//serial
		stringref = IOHIDDevice_GetSerialNumber(pCurrentHIDDevice);
		CFStringGetCString(stringref, tmp, 256, kCFStringEncodingASCII);
		CFRelease(stringref);
		devstring = newPyrString(g->gc, tmp, 0, true);
		SetObject(devNameArray->slots+devNameArray->size++, devstring);
		g->gc->GCWrite(devNameArray, (PyrObject*) devstring);

		SetObject(allDevsArray->slots+allDevsArray->size++, devNameArray);
		g->gc->GCWrite(allDevsArray, (PyrObject*) devNameArray);
		pCurrentHIDDevice = HIDGetNextDevice (pCurrentHIDDevice);

	}

	//UInt32 outnum =  HIDCountDeviceElements (pCurrentHIDDevice, kHIDElementTypeOutput);
	//post("number of outputs: %d \n", outnum);
	SetObject(a, allDevsArray);

	return errNone;
}


int prHIDGetValue(VMGlobals *g, int numArgsPushed);
int prHIDGetValue(VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a = g->sp - 2; //class
	PyrSlot *b = g->sp - 1; //locID device
	PyrSlot *c = g->sp; //element cookie
	int locID, cookieNum;
	int err = slotIntVal(b, &locID);
	if (err) return err;
	err = slotIntVal(c, &cookieNum);
	if (err) return err;
	IOHIDElementCookie cookie = (IOHIDElementCookie) cookieNum;
	//look for the right device:
    IOHIDDeviceRef  pCurrentHIDDevice = HIDGetFirstDevice ();
	while (pCurrentHIDDevice && (IOHIDDevice_GetLocationID(pCurrentHIDDevice) !=locID))
        pCurrentHIDDevice = HIDGetNextDevice (pCurrentHIDDevice);
	if(!pCurrentHIDDevice) return errFailed;
	//look for the right element:
	IOHIDElementRef pCurrentHIDElement =  HIDGetFirstDeviceElement (pCurrentHIDDevice, kHIDElementTypeAll);
	// use gElementCookie to find current element
    while (pCurrentHIDElement && (IOHIDElementGetCookie(pCurrentHIDElement) != cookie))
        pCurrentHIDElement = HIDGetNextDeviceElement (pCurrentHIDElement, kHIDElementTypeAll);

	if (pCurrentHIDElement)
    {
		SInt32 value;
		 // if it's not a button and it's not a hatswitch then calibrate
		if(( IOHIDElementGetType(pCurrentHIDElement) != kIOHIDElementTypeInput_Button ) &&
			( IOHIDElementGetUsagePage(pCurrentHIDElement) == 0x01 && IOHIDElementGetUsage(pCurrentHIDElement) != kHIDUsage_GD_Hatswitch))
		{
			value = IOHIDElement_GetValue(pCurrentHIDElement, kIOHIDValueScaleTypeCalibrated);
		}
		else
		{
			value = IOHIDElement_GetValue(pCurrentHIDElement, kIOHIDValueScaleTypePhysical);
		}
		SetInt(a, value);
	}
	else SetNil(a);
	return errNone;

}


int prHIDSetValue(VMGlobals *g, int numArgsPushed);
int prHIDSetValue(VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a = g->sp - 3; //class
	PyrSlot *b = g->sp - 2; //locID
	PyrSlot *c = g->sp - 1; //element device
	PyrSlot *d = g->sp; //value cookie
	int locID, cookieNum, value;
	int err = slotIntVal(b, &locID);
	if (err) return err;
	err = slotIntVal(c, &cookieNum);
	if (err) return err;
	IOHIDElementCookie cookie = (IOHIDElementCookie) cookieNum;
	err = slotIntVal(d, &value);
	if (err) return err;
	//look for the right device:
    IOHIDDeviceRef  pCurrentHIDDevice = HIDGetFirstDevice ();
	while (pCurrentHIDDevice && (IOHIDDevice_GetLocationID(pCurrentHIDDevice) !=locID))
        pCurrentHIDDevice = HIDGetNextDevice (pCurrentHIDDevice);
	if(!pCurrentHIDDevice) return errFailed;
	//look for the right element:
	IOHIDElementRef pCurrentHIDElement =  HIDGetFirstDeviceElement (pCurrentHIDDevice, kHIDElementTypeAll);
	// use gElementCookie to find current element
    while (pCurrentHIDElement && (IOHIDElementGetCookie(pCurrentHIDElement) != cookie))
        pCurrentHIDElement = HIDGetNextDeviceElement (pCurrentHIDElement, kHIDElementTypeAll);
//struct IOHIDEventStruct
//{
//    IOHIDElementType	type;
//    IOHIDElementCookie	elementCookie;
//    SInt32		value;
//    AbsoluteTime	timestamp;
//    UInt32		longValueSize;
//    void *		longValue;
//};

	if (pCurrentHIDElement)
    {
		IOHIDValueRef valueref = IOHIDValueCreateWithIntegerValue(0, pCurrentHIDElement, 0, value);
		IOHIDDeviceSetValue(pCurrentHIDDevice, pCurrentHIDElement, valueref);
		CFRelease(valueref);

		SetInt(a, value);
	}
	else SetNil(a);
	return errNone;

}

void PushQueueEvents_RawValue ();
void PushQueueEvents_RawValue (){

	IOHIDValueRef value_ref = 0;
	IOHIDDeviceRef  pCurrentHIDDevice = HIDGetFirstDevice ();
	int numdevs = gNumberOfHIDDevices;
	unsigned char result;
	for(int i=0; i< numdevs; i++){
		result = HIDGetEvent(pCurrentHIDDevice, &value_ref);
		if(result && compiledOK) {
			SInt32 value = IOHIDValueGetIntegerValue(value_ref);
			int vendorID = IOHIDDevice_GetVendorID(pCurrentHIDDevice);;
			int productID = IOHIDDevice_GetProductID(pCurrentHIDDevice);
			int locID = IOHIDDevice_GetLocationID(pCurrentHIDDevice);
			IOHIDElementCookie cookie = IOHIDElementGetCookie(IOHIDValueGetElement(value_ref));
			CFRelease(value_ref);
			VMGlobals *g = gMainVMGlobals;
			pthread_mutex_lock (&gLangMutex);
			g->canCallOS = false; // cannot call the OS
			++g->sp; SetObject(g->sp, s_hid->u.classobj); // Set the class HIDService
			//set arguments:
			++g->sp;SetInt(g->sp, vendorID);
			++g->sp;SetInt(g->sp, productID);
			++g->sp;SetInt(g->sp, locID);
			++g->sp;SetInt(g->sp, (int) cookie);
			++g->sp;SetInt(g->sp, value);
			runInterpreter(g, s_hidAction, 6);
			g->canCallOS = false; // cannot call the OS
			pthread_mutex_unlock (&gLangMutex);
		}

	/* FIXME: this does not seem to be working
		if ( !HIDIsValidDevice(pCurrentHIDDevice) )
		{ // readError
		 post("HID: read Error\n");
			int locID = pCurrentHIDDevice->locID;
			VMGlobals *g = gMainVMGlobals;
			pthread_mutex_lock (&gLangMutex);
			g->canCallOS = false; // cannot call the OS
			++g->sp; SetObject(g->sp, s_hid->u.classobj); // Set the class HIDService
			++g->sp;SetInt(g->sp, locID);
			runInterpreter(g, s_readError, 2);
			g->canCallOS = false; // cannot call the OS
			pthread_mutex_unlock (&gLangMutex);
		}
		*/
	pCurrentHIDDevice = HIDGetNextDevice(pCurrentHIDDevice);
	}
}

void PushQueueEvents_CalibratedValue ();
void PushQueueEvents_CalibratedValue (){

	IOHIDValueRef value_ref = 0;
	IOHIDDeviceRef  pCurrentHIDDevice = HIDGetFirstDevice ();

	int numdevs = gNumberOfHIDDevices;
	unsigned char result;
	for(int i=0; i< numdevs; i++){

		result = HIDGetEvent(pCurrentHIDDevice, &value_ref);
		if(result && compiledOK) {
			SInt32 value = IOHIDValueGetScaledValue(value_ref, kIOHIDValueScaleTypeCalibrated);
			int vendorID = IOHIDDevice_GetVendorID(pCurrentHIDDevice);;
			int productID = IOHIDDevice_GetProductID(pCurrentHIDDevice);
			int locID = IOHIDDevice_GetLocationID(pCurrentHIDDevice);
			IOHIDElementCookie cookie = IOHIDElementGetCookie(IOHIDValueGetElement(value_ref));
			CFRelease(value_ref);
			IOHIDElementRef pCurrentHIDElement =  HIDGetFirstDeviceElement (pCurrentHIDDevice, kHIDElementTypeAll);
	// use gElementCookie to find current element
			while (pCurrentHIDElement && ( (IOHIDElementGetCookie(pCurrentHIDElement)) != cookie))
			pCurrentHIDElement = HIDGetNextDeviceElement (pCurrentHIDElement, kHIDElementTypeAll);

			if (pCurrentHIDElement)
			{
			//find element to calibrate
			VMGlobals *g = gMainVMGlobals;
			pthread_mutex_lock (&gLangMutex);
			g->canCallOS = false; // cannot call the OS
			++g->sp; SetObject(g->sp, s_hid->u.classobj); // Set the class HIDService
			//set arguments:
			++g->sp;SetInt(g->sp, vendorID);
			++g->sp;SetInt(g->sp, productID);
			++g->sp;SetInt(g->sp, locID);
			++g->sp;SetInt(g->sp, (int) cookie);
			++g->sp;SetInt(g->sp, value);
			runInterpreter(g, s_hidAction, 6);
			g->canCallOS = false; // cannot call the OS
			pthread_mutex_unlock (&gLangMutex);
			}
		}
	/* FIXME: this does not seem to be working!
		if ( !HIDIsValidDevice(pCurrentHIDDevice) )
		{ // readError
		 post("HID: read Error\n");
			int locID = pCurrentHIDDevice->locID;
			VMGlobals *g = gMainVMGlobals;
			pthread_mutex_lock (&gLangMutex);
			g->canCallOS = false; // cannot call the OS
			++g->sp; SetObject(g->sp, s_hid->u.classobj); // Set the class HIDService
			++g->sp;SetInt(g->sp, locID);
			runInterpreter(g, s_readError, 2);
			g->canCallOS = false; // cannot call the OS
			pthread_mutex_unlock (&gLangMutex);
		}*/
	pCurrentHIDDevice = HIDGetNextDevice(pCurrentHIDDevice);
	}
}

static pascal void IdleTimer (EventLoopTimerRef inTimer, void* userData);
static pascal void IdleTimer (EventLoopTimerRef inTimer, void* userData)
{
	#pragma unused (inTimer, userData)
	PushQueueEvents_CalibratedValue ();
}
int prHIDReleaseDeviceList(VMGlobals *g, int numArgsPushed);
int prHIDReleaseDeviceList(VMGlobals *g, int numArgsPushed)
{
	releaseHIDDevices();
	return errNone;
}
static EventLoopTimerUPP GetTimerUPP (void);
static EventLoopTimerUPP GetTimerUPP (void)
{
	static EventLoopTimerUPP	sTimerUPP = NULL;

	if (sTimerUPP == NULL)
		sTimerUPP = NewEventLoopTimerUPP (IdleTimer);

	return sTimerUPP;
}
/*
typedef void (*IOHIDCallbackFunction)
              (void * target, IOReturn result, void * refcon, void * sender);

*/
/*
void callback  (void * target, IOReturn result, void * refcon, void * sender);
void callback  (void * target, IOReturn result, void * refcon, void * sender)
{
}
*/
int prHIDRunEventLoop(VMGlobals *g, int numArgsPushed);
int prHIDRunEventLoop(VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a = g->sp - 1; //class
	PyrSlot *b = g->sp; //num
	double eventtime;
	int err = slotDoubleVal(b, &eventtime);
	if (err) return err;
	if(gTimer)
	{
        RemoveEventLoopTimer(gTimer);
		gTimer = NULL;
	}
	InstallEventLoopTimer (GetCurrentEventLoop(), 0, (EventTimerInterval) eventtime, GetTimerUPP (), 0, &gTimer);
	//HIDSetQueueCallback(pCurrentHIDDevice, callback);
	return errNone;
}

int prHIDQueueDevice(VMGlobals *g, int numArgsPushed);
int prHIDQueueDevice(VMGlobals *g, int numArgsPushed)
{

	PyrSlot *a = g->sp - 1; //class
	PyrSlot *b = g->sp; //locID device
	int locID;
	int err = slotIntVal(b, &locID);
	if (err) return err;
	//look for the right device:
    IOHIDDeviceRef  pCurrentHIDDevice = HIDGetFirstDevice ();
	while (pCurrentHIDDevice && (IOHIDDevice_GetLocationID(pCurrentHIDDevice) !=locID))
        pCurrentHIDDevice = HIDGetNextDevice (pCurrentHIDDevice);
	if(!pCurrentHIDDevice) return errFailed;
	HIDQueueDevice(pCurrentHIDDevice);
	return errNone;
}

int prHIDQueueElement(VMGlobals *g, int numArgsPushed);
int prHIDQueueElement(VMGlobals *g, int numArgsPushed)
{

	PyrSlot *a = g->sp - 2; //class
	PyrSlot *b = g->sp - 1; //locID device
	PyrSlot *c = g->sp; //element cookie
	int locID, cookieNum;
	int err = slotIntVal(b, &locID);
	if (err) return err;
	err = slotIntVal(c, &cookieNum);
	if (err) return err;
	IOHIDElementCookie cookie = (IOHIDElementCookie) cookieNum;
	//look for the right device:
    IOHIDDeviceRef  pCurrentHIDDevice = HIDGetFirstDevice ();
	while (pCurrentHIDDevice && (IOHIDDevice_GetLocationID(pCurrentHIDDevice) !=locID))
        pCurrentHIDDevice = HIDGetNextDevice (pCurrentHIDDevice);
	if(!pCurrentHIDDevice) return errFailed;
	//look for the right element:
	IOHIDElementRef pCurrentHIDElement =  HIDGetFirstDeviceElement (pCurrentHIDDevice, kHIDElementTypeAll);
	// use gElementCookie to find current element
    while (pCurrentHIDElement && (IOHIDElementGetCookie(pCurrentHIDElement) != cookie))
        pCurrentHIDElement = HIDGetNextDeviceElement (pCurrentHIDElement, kHIDElementTypeAll);
	if(!pCurrentHIDElement) return errFailed;
	HIDQueueElement(pCurrentHIDDevice, pCurrentHIDElement);
	return errNone;
}

int prHIDDequeueElement(VMGlobals *g, int numArgsPushed);
int prHIDDequeueElement(VMGlobals *g, int numArgsPushed)
{

	PyrSlot *a = g->sp - 2; //class
	PyrSlot *b = g->sp - 1; //locID device
	PyrSlot *c = g->sp; //element cookie
	int locID, cookieNum;
	int err = slotIntVal(b, &locID);
	if (err) return err;
	err = slotIntVal(c, &cookieNum);
	if (err) return err;
	IOHIDElementCookie cookie = (IOHIDElementCookie) cookieNum;
	//look for the right device:
    IOHIDDeviceRef  pCurrentHIDDevice = HIDGetFirstDevice ();
	while (pCurrentHIDDevice && (IOHIDDevice_GetLocationID(pCurrentHIDDevice) !=locID))
        pCurrentHIDDevice = HIDGetNextDevice (pCurrentHIDDevice);
	if(!pCurrentHIDDevice) return errFailed;
	//look for the right element:
	IOHIDElementRef pCurrentHIDElement =  HIDGetFirstDeviceElement (pCurrentHIDDevice, kHIDElementTypeAll);
    while (pCurrentHIDElement && (IOHIDElementGetCookie(pCurrentHIDElement) != cookie))
        pCurrentHIDElement = HIDGetNextDeviceElement (pCurrentHIDElement, kHIDElementTypeAll);
	if(!pCurrentHIDElement) return errFailed;
	HIDDequeueElement(pCurrentHIDDevice, pCurrentHIDElement);
	return errNone;
}

int prHIDDequeueDevice(VMGlobals *g, int numArgsPushed);
int prHIDDequeueDevice(VMGlobals *g, int numArgsPushed)
{

	PyrSlot *a = g->sp - 1; //class
	PyrSlot *b = g->sp; //locID device
	int locID;
	int err = slotIntVal(b, &locID);
	if (err) return err;
	//look for the right device:
    IOHIDDeviceRef  pCurrentHIDDevice = HIDGetFirstDevice ();
	while (pCurrentHIDDevice && (IOHIDDevice_GetLocationID(pCurrentHIDDevice) !=locID))
        pCurrentHIDDevice = HIDGetNextDevice (pCurrentHIDDevice);
	if(!pCurrentHIDDevice) return errFailed;
	HIDDequeueDevice(pCurrentHIDDevice);
	return errNone;
}

int prHIDStopEventLoop(VMGlobals *g, int numArgsPushed);
int prHIDStopEventLoop(VMGlobals *g, int numArgsPushed)
{
	if (gTimer)
	{
        RemoveEventLoopTimer(gTimer);
		gTimer = NULL;
	}
	return errNone;
}


void initHIDPrimitives()
{
	int base, index;

	releaseHIDDevices();

	s_hid = getsym("HIDDeviceService");
	s_hidAction = getsym("prHidAction");
	s_readError = getsym("prReadError");

	base = nextPrimitiveIndex();
	index = 0;
	definePrimitive(base, index++, "_HIDBuildDeviceList", prHIDBuildDeviceList, 3, 0);

	definePrimitive(base, index++, "_HIDGetElementListSize", prHIDGetElementListSize, 2, 0);
	definePrimitive(base, index++, "_HIDBuildElementList", prHIDBuildElementList, 3, 0);

	definePrimitive(base, index++, "_HIDGetValue", prHIDGetValue, 3, 0);
	definePrimitive(base, index++, "_HIDSetValue", prHIDSetValue, 4, 0);

	definePrimitive(base, index++, "_HIDReleaseDeviceList", prHIDReleaseDeviceList, 1, 0);
	definePrimitive(base, index++, "_HIDRunEventLoop", prHIDRunEventLoop, 2, 0);
	definePrimitive(base, index++, "_HIDStopEventLoop", prHIDStopEventLoop, 1, 0);
	definePrimitive(base, index++, "_HIDQueueDevice", prHIDQueueDevice, 2, 0);
	definePrimitive(base, index++, "_HIDDequeueDevice", prHIDDequeueDevice, 2, 0);
	definePrimitive(base, index++, "_HIDDequeueElement", prHIDDequeueElement, 3, 0);
	definePrimitive(base, index++, "_HIDQueueElement", prHIDQueueElement, 3, 0);

}
#else // !SC_DARWIN
void initHIDPrimitives()
{
	//other platforms?
}
#endif // SC_DARWIN