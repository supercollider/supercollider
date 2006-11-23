HIDDevice {
	var <manufacturer, < product, <usage, < vendorID, < productID, < locID;
	var < elements;
	*new{arg manufacturer, product, usage, vendorID, productID, locID;
		^super.newCopyArgs(manufacturer, product, usage, vendorID, productID, locID).init;
	}
	init{
		elements = Array.new;
	}

	value{arg elementNum=0;
		^HIDDeviceService.value(locID,elements.at(elementNum).cookie)
	}
	setValue{|elementNum, val|
		HIDDeviceService.setValue(locID,elements.at(elementNum).cookie, val)
	}
	queueDevice{
		HIDDeviceService.queueDevice(locID);
	}
	
	dequeueDevice{
		HIDDeviceService.dequeueDevice(locID);
	}
	
	queueElement{arg elementNum=0;
		HIDDeviceService.queueElement(locID, elements.at(elementNum).cookie);
	}
	
	dequeueElement{arg elementNum=0;
		HIDDeviceService.dequeueElement(locID, elements.at(elementNum).cookie);
	}
	
	//private:
	prAddElement{arg type, usage, cookie, min, max;
		elements = elements.add(HIDDeviceElement(type, usage, cookie, min, max));
	}
}

HIDDeviceElement {
	var < type, <usage, < cookie, <min, <max;
	*new{arg type, usage, cookie, min, max;
		^super.newCopyArgs(type, usage, cookie, min, max);
	}
//	value_{|val|
//		HIDDeviceService.setValue(locID, cookie, val)
//	
//	}
}

HIDDeviceService{
	classvar < devices, <> action;
	classvar < initialized = false;
	classvar < deviceSpecs;

	*initClass {
		deviceSpecs = IdentityDictionary.new;
	}
	
	*keyToIndex { arg key, locID=0;
		var device, deviceSpec;
		device = devices.at(locID);
		^if(device.isNil, { ^nil }, {
			deviceSpec = deviceSpecs.at(device.product.asSymbol);
			if(deviceSpec.notNil, { deviceSpec.at(key) }, { nil });
		})
	}
	
	*value{arg locID, cookie;
		_HIDGetValue
	}	

	*setValue{arg locID, cookie, value;
		_HIDSetValue
	}
	
	*buildDeviceList{arg usagePage=1, usage=4;
		var devlist, elelist;
		devices = Array.new;
		devlist = this.prbuildDeviceList(usagePage, usage);
		devlist ?? {"HIDDeviceService: no devices found".warn; ^nil};
		devlist.do({arg dev;
			var newdev;
			newdev = HIDDevice(dev.at(0), dev.at(1), dev.at(2), dev.at(3), dev.at(4), dev.at(5));
			elelist = this.prbuildElementList(newdev.locID,
				Array.newClear(HIDDeviceService.prGetElementListSize(newdev.locID)));
			elelist.do({arg ele;
				if(ele.notNil){
					newdev.prAddElement(ele.at(0), ele.at(1), ele.at(2), ele.at(3), ele.at(4));
				};
			});
			devices = devices.add(newdev);
		});
		initialized = true;
	}
	
	*prbuildDeviceList{arg usagePage=1, usage;
		_HIDBuildDeviceList
		^this.primitiveFailed	
	}
	*prbuildElementList{arg locID;
		_HIDBuildElementList
		^this.primitiveFailed	
	}
	*prGetElementListSize{arg locID;
		_HIDGetElementListSize
		^this.primitiveFailed	
	}	
	*releaseDeviceList{
		//must be called before closing the program..
		if(initialized,{
			this.prreleaseDeviceList;
			devices = nil;
		});
	}	
	*prreleaseDeviceList{
		//must be called before closing the program..
		_HIDReleaseDeviceList
	}
	*runEventLoop{arg rate=0.002; 
		_HIDRunEventLoop
	}
	*stopEventLoop{
		_HIDStopEventLoop
	}

	*hidAction{arg vendorID, productID, locID, cookie, val;
		action.value(vendorID, productID, locID, cookie, val);
	}
	
	*queueDevice{arg locID;
		_HIDQueueDevice
		^this.primitiveFailed	
	}
	
	*dequeueDevice{arg locID;
		_HIDDequeueDevice
		^this.primitiveFailed	
	}
	
	*queueElement{arg locID, cookie;
		_HIDQueueElement
		^this.primitiveFailed	
	}
	
	*dequeueElement{arg locID, cookie;
		_HIDDequeueElement
		^this.primitiveFailed	
	}
}

//HIDDeviceService by jan trutzschler v. falkenstein [10/2003]
/*
these are values to pass in buildDeviceList(usagePage, usage);
this is an incomplete list.
for more look at IOHIDUsageTables.h

most common usagePages for hid:
enum
{
	kHIDPage_Undefined	= 0x00,
	kHIDPage_GenericDesktop	= 0x01,
	kHIDPage_Simulation	= 0x02,
	kHIDPage_VR	= 0x03,
	kHIDPage_Sport	= 0x04,
	kHIDPage_Game	= 0x05,
}
most common usage for hid:

/* GenericDesktop Page (0x01) */
enum
{
	kHIDUsage_GD_Pointer	= 0x01,	/* Physical Collection */
	kHIDUsage_GD_Mouse	= 0x02,	/* Application Collection */
	/* 0x03 Reserved */
	kHIDUsage_GD_Joystick	= 0x04,	/* Application Collection */
	kHIDUsage_GD_GamePad	= 0x05,	/* Application Collection */
	kHIDUsage_GD_Keyboard	= 0x06,	/* Application Collection */
	kHIDUsage_GD_Keypad	= 0x07,	/* Application Collection */
	kHIDUsage_GD_MultiAxisController	= 0x08,	/* Application Collection */
}

/* Simulation Page (0x02) */
/* This section provides detailed descriptions of the usages employed by simulation devices. */
enum
{
	kHIDUsage_Sim_FlightSimulationDevice	= 0x01,	/* Application Collection */
	kHIDUsage_Sim_AutomobileSimulationDevice	= 0x02,	/* Application Collection */
	kHIDUsage_Sim_TankSimulationDevice	= 0x03,	/* Application Collection */
	kHIDUsage_Sim_SpaceshipSimulationDevice	= 0x04,	/* Application Collection */
	kHIDUsage_Sim_SubmarineSimulationDevice	= 0x05,	/* Application Collection */
	kHIDUsage_Sim_SailingSimulationDevice	= 0x06,	/* Application Collection */
	kHIDUsage_Sim_MotorcycleSimulationDevice	= 0x07,	/* Application Collection */
	kHIDUsage_Sim_SportsSimulationDevice	= 0x08,	/* Application Collection */
	kHIDUsage_Sim_AirplaneSimulationDevice	= 0x09,	/* Application Collection */
	kHIDUsage_Sim_HelicopterSimulationDevice	= 0x0A,	/* Application Collection */
	kHIDUsage_Sim_MagicCarpetSimulationDevice	= 0x0B,	/* Application Collection */
	kHIDUsage_Sim_BicycleSimulationDevice	= 0x0C,	/* Application Collection */
	/* 0x0D - 0x1F Reserved */
	kHIDUsage_Sim_FlightControlStick	= 0x20,	/* Application Collection */
	kHIDUsage_Sim_FlightStick	= 0x21,	/* Application Collection */
	kHIDUsage_Sim_CyclicControl	= 0x22,	/* Physical Collection */
	kHIDUsage_Sim_CyclicTrim	= 0x23,	/* Physical Collection */
	kHIDUsage_Sim_FlightYoke	= 0x24,	/* Application Collection */
	kHIDUsage_Sim_TrackControl	= 0x25,	/* Physical Collection */
}

/* Game Page (0x05) */
enum
{
	kHIDUsage_Game_3DGameController	= 0x01,	/* Application Collection */
	kHIDUsage_Game_PinballDevice	= 0x02,	/* Application Collection */
	kHIDUsage_Game_GunDevice	= 0x03,	/* Application Collection */
}
 

*/