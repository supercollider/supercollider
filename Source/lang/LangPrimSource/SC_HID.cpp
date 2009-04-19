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
    pRecDevice  pCurrentHIDDevice = HIDGetFirstDevice ();
	while (pCurrentHIDDevice && (pCurrentHIDDevice->locID !=locID))
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
    pRecDevice  pCurrentHIDDevice = HIDGetFirstDevice ();
	while (pCurrentHIDDevice && (pCurrentHIDDevice->locID !=locID))
        pCurrentHIDDevice = HIDGetNextDevice (pCurrentHIDDevice);
	if(!pCurrentHIDDevice) return errFailed;
	
	pRecElement	devElement =  HIDGetFirstDeviceElement (pCurrentHIDDevice, kHIDElementTypeAll );
	UInt32 numElements = HIDCountDeviceElements (pCurrentHIDDevice, kHIDElementTypeAll );

//		PyrObject* devAllElementsArray = newPyrArray(g->gc, numElements * sizeof(PyrObject), 0 , true);
		PyrObject *devAllElementsArray = c->uo;
//		post("numElements: %d\n", numElements);
		numElements = sc_clip(numElements, 0, devAllElementsArray->size);
		for(uint i=0; i<numElements; i++){
			if(devElement){
				char cstrElementName [256];
				PyrObject* devElementArray = newPyrArray(g->gc, 8 * sizeof(PyrObject), 0 , true);
				// type name (1)
				HIDGetTypeName((IOHIDElementType) devElement->type, cstrElementName);
				PyrString *devstring = newPyrString(g->gc, cstrElementName, 0, true);
				SetObject(devElementArray->slots+devElementArray->size++, devstring);
				//g->gc->GCWrite(devElementArray, (PyrObject*) devstring);
				//usage (2)
				HIDGetUsageName (devElement->usagePage, devElement->usage, cstrElementName);
				PyrString *usestring = newPyrString(g->gc, cstrElementName, 0, true);			
				SetObject(devElementArray->slots+devElementArray->size++, usestring);
				//g->gc->GCWrite(devElementArray, (PyrObject*) usestring);
				//cookie (3)
				SetInt(devElementArray->slots+devElementArray->size++, (long) devElement->cookie);
				// min (4)
				SetInt(devElementArray->slots+devElementArray->size++, (long) devElement->min);
				// max (5)
				SetInt(devElementArray->slots+devElementArray->size++, (long) devElement->max);
				
				// IO type as int: (6)
				SetInt(devElementArray->slots+devElementArray->size++, (int) devElement->type);
				// Usage page as int: (7)
				SetInt(devElementArray->slots+devElementArray->size++, (long) devElement->usagePage);
				// Usage type as int: (8)
				SetInt(devElementArray->slots+devElementArray->size++, (long) devElement->usage);
				
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
	char cstrDeviceName [256];
		
    pRecDevice  pCurrentHIDDevice = HIDGetFirstDevice ();
	PyrObject* allDevsArray = newPyrArray(g->gc, numdevs * sizeof(PyrObject), 0 , true);
	for(int i=0; i<numdevs; i++){
		//device:
		PyrObject* devNameArray = newPyrArray(g->gc, 8 * sizeof(PyrObject), 0 , true);
		//manufacturer:
		PyrString *devstring = newPyrString(g->gc, pCurrentHIDDevice->manufacturer, 0, true);
		SetObject(devNameArray->slots+devNameArray->size++, devstring);
		g->gc->GCWrite(devNameArray, (PyrObject*) devstring);
		//product name:
		devstring = newPyrString(g->gc, pCurrentHIDDevice->product, 0, true);
		SetObject(devNameArray->slots+devNameArray->size++, devstring);
		g->gc->GCWrite(devNameArray, (PyrObject*) devstring);
		//usage
		HIDGetUsageName (pCurrentHIDDevice->usagePage, pCurrentHIDDevice->usage, cstrDeviceName);
		devstring = newPyrString(g->gc, cstrDeviceName, 0, true);
		SetObject(devNameArray->slots+devNameArray->size++, devstring);
		g->gc->GCWrite(devNameArray, (PyrObject*) devstring);
		//vendor id
		SetInt(devNameArray->slots+devNameArray->size++, pCurrentHIDDevice->vendorID);
		//product id
		SetInt(devNameArray->slots+devNameArray->size++, pCurrentHIDDevice->productID);
		//locID
		SetInt(devNameArray->slots+devNameArray->size++, pCurrentHIDDevice->locID);
		
		//version
		SetInt(devNameArray->slots+devNameArray->size++, pCurrentHIDDevice->version);
		
		//serial
		devstring = newPyrString(g->gc, pCurrentHIDDevice->serial, 0, true);
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
    pRecDevice  pCurrentHIDDevice = HIDGetFirstDevice ();
	while (pCurrentHIDDevice && (pCurrentHIDDevice->locID !=locID))
        pCurrentHIDDevice = HIDGetNextDevice (pCurrentHIDDevice);
	if(!pCurrentHIDDevice) return errFailed;
	//look for the right element:
	pRecElement pCurrentHIDElement =  HIDGetFirstDeviceElement (pCurrentHIDDevice, kHIDElementTypeAll);
	// use gElementCookie to find current element
    while (pCurrentHIDElement && (pCurrentHIDElement->cookie != cookie))
        pCurrentHIDElement = HIDGetNextDeviceElement (pCurrentHIDElement, kHIDElementTypeAll);
		
	if (pCurrentHIDElement)
    {
		SInt32 value = HIDGetElementValue (pCurrentHIDDevice, pCurrentHIDElement);
		 // if it's not a button and it's not a hatswitch then calibrate
		if(( pCurrentHIDElement->type != kIOHIDElementTypeInput_Button ) &&
			( pCurrentHIDElement->usagePage == 0x01 && pCurrentHIDElement->usage != kHIDUsage_GD_Hatswitch)) 
			value = HIDCalibrateValue ( value, pCurrentHIDElement );
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
    pRecDevice  pCurrentHIDDevice = HIDGetFirstDevice ();
	while (pCurrentHIDDevice && (pCurrentHIDDevice->locID !=locID))
        pCurrentHIDDevice = HIDGetNextDevice (pCurrentHIDDevice);
	if(!pCurrentHIDDevice) return errFailed;
	//look for the right element:
	pRecElement pCurrentHIDElement =  HIDGetFirstDeviceElement (pCurrentHIDDevice, kHIDElementTypeAll);
	// use gElementCookie to find current element
    while (pCurrentHIDElement && (pCurrentHIDElement->cookie != cookie))
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
		IOHIDEventStruct event = 
		{
			kIOHIDElementTypeOutput, 
			pCurrentHIDElement->cookie,
			value,
			{0},
			sizeof(int),
			NULL
		};
		SInt32 value = HIDSetElementValue (pCurrentHIDDevice, pCurrentHIDElement, &event);
		 // if it's not a button and it's not a hatswitch then calibrate
	//	if(( pCurrentHIDElement->type != kIOHIDElementTypeInput_Button ) &&
	//		( pCurrentHIDElement->usagePage == 0x01 && pCurrentHIDElement->usage != kHIDUsage_GD_Hatswitch)) 
	//		value = HIDCalibrateValue ( value, pCurrentHIDElement );
		SetInt(a, value);
	}
	else SetNil(a);
	return errNone;	
	
}

void PushQueueEvents_RawValue ();
void PushQueueEvents_RawValue (){

	IOHIDEventStruct event;
	pRecDevice  pCurrentHIDDevice = HIDGetFirstDevice ();
	int numdevs = gNumberOfHIDDevices;
	unsigned char result;
	for(int i=0; i< numdevs; i++){
		result = HIDGetEvent(pCurrentHIDDevice, (void*) &event);
		if(result && compiledOK) {
			SInt32 value = event.value;
			int vendorID = pCurrentHIDDevice->vendorID;
			int productID = pCurrentHIDDevice->productID;			
			int locID = pCurrentHIDDevice->locID;
			IOHIDElementCookie cookie = (IOHIDElementCookie) event.elementCookie;
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

	IOHIDEventStruct event;
	pRecDevice  pCurrentHIDDevice = HIDGetFirstDevice ();
	
	int numdevs = gNumberOfHIDDevices;
	unsigned char result;
	for(int i=0; i< numdevs; i++){

		result = HIDGetEvent(pCurrentHIDDevice, (void*) &event);
		if(result && compiledOK) {
			SInt32 value = event.value;
			int vendorID = pCurrentHIDDevice->vendorID;
			int productID = pCurrentHIDDevice->productID;			
			int locID = pCurrentHIDDevice->locID;
			IOHIDElementCookie cookie = (IOHIDElementCookie) event.elementCookie;
			pRecElement pCurrentHIDElement =  HIDGetFirstDeviceElement (pCurrentHIDDevice, kHIDElementTypeAll);
	// use gElementCookie to find current element
			while (pCurrentHIDElement && ( (pCurrentHIDElement->cookie) != cookie))
			pCurrentHIDElement = HIDGetNextDeviceElement (pCurrentHIDElement, kHIDElementTypeAll);
		
			if (pCurrentHIDElement)
			{
			value = HIDCalibrateValue(value, pCurrentHIDElement);
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
    pRecDevice  pCurrentHIDDevice = HIDGetFirstDevice ();
	while (pCurrentHIDDevice && (pCurrentHIDDevice->locID !=locID))
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
    pRecDevice  pCurrentHIDDevice = HIDGetFirstDevice ();
	while (pCurrentHIDDevice && (pCurrentHIDDevice->locID !=locID))
        pCurrentHIDDevice = HIDGetNextDevice (pCurrentHIDDevice);
	if(!pCurrentHIDDevice) return errFailed;
	//look for the right element:
	pRecElement pCurrentHIDElement =  HIDGetFirstDeviceElement (pCurrentHIDDevice, kHIDElementTypeAll);
	// use gElementCookie to find current element
    while (pCurrentHIDElement && (pCurrentHIDElement->cookie != cookie))
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
    pRecDevice  pCurrentHIDDevice = HIDGetFirstDevice ();
	while (pCurrentHIDDevice && (pCurrentHIDDevice->locID !=locID))
        pCurrentHIDDevice = HIDGetNextDevice (pCurrentHIDDevice);
	if(!pCurrentHIDDevice) return errFailed;
	//look for the right element:
	pRecElement pCurrentHIDElement =  HIDGetFirstDeviceElement (pCurrentHIDDevice, kHIDElementTypeAll);
    while (pCurrentHIDElement && (pCurrentHIDElement->cookie != cookie))
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
    pRecDevice  pCurrentHIDDevice = HIDGetFirstDevice ();
	while (pCurrentHIDDevice && (pCurrentHIDDevice->locID !=locID))
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