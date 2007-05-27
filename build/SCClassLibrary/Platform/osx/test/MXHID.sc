/* Wrapper for HIDDeviceService for General HID support */

MXHID {
	classvar extraClasses;
	var <device;

	*initClass {
		Class.initClassTree( Event );
		extraClasses = Event.new;
		Class.initClassTree( GeneralHID );
		GeneralHID.add( this );
	}

	*id { ^\osx_hid }

	*put { arg key, object;
		extraClasses.put( key, object );
	}

	*doesNotUnderstand { arg selector ... args;
		^extraClasses.perform( selector, *args );
	}

	/// ----------------- functions ---------------

	*buildDeviceList{
		^HIDDeviceService.buildDeviceList(nil,nil); 
		//deviceList = HIDDeviceServer.deviceList;		
	}

	*deviceList{
		^HIDDeviceService.deviceList;
	}

	*postDevices { 
		HIDDeviceService.devices.do({arg dev;
			//			"".postln;
			[dev.manufacturer, dev.product, dev.vendorID, dev.productID, dev.locID].postcs;
		});
	}

	*postDevicesAndProperties { 
		HIDDeviceService.devices.do({arg dev;
			"".postln;
			[dev.manufacturer, dev.product, dev.vendorID, dev.productID, dev.locID].postcs;
			dev.elements.do({arg ele;
				[ele.type, ele.usage, ele.cookie, ele.min, ele.max].postln;
			});
		});
	}


	*startEventLoop{ |rate=0.005|
		HIDDeviceService.runEventLoop(rate);
	}

	*stopEventLoop{
		HIDDeviceService.stopEventLoop;
	}
	
	*eventLoopIsRunning{
		^HIDDeviceService.eventLoopIsRunning;
	}

	*open { arg dev;
		^super.new.init( dev );
	}

	init{ |dev|
		if ( dev.isKindOf( HIDDevice ),
				{ 
					device = dev;
					dev.queueDevice;
				},{
					"not a valid device".warn;
				});
	}

	close{
		if ( device.notNil,
			{
				device.dequeueDevice;
			});
	}

	isOpen{
		if ( device.notNil,
			{
				^device.isOpen;
			},{
				^false;
			}
		);
	}

	/*
	*initHID { |rate=0.01|
		all = all ? (); maps = maps ? ();
		// make sure event loop is stopped before restarting it later on.
		HIDDeviceService.stopEventLoop;
		
		all.do{ |dev|
			if (verbose)	{ dev.postln; };
			if ( dev.isKindOf( HIDDevice ),
				{ dev.dequeueDevice; },
				{ dev.device.dequeueDevice; })
		};

		HIDDeviceService.buildDeviceList(nil,nil); 
		
	}
	
	*initHID2 { |rate=0.01|	
	
		all.do{ |dev|
			if (verbose)	{ dev.postln; };
			if ( dev.isKindOf( HIDDevice ),
				{ dev.queueDevice; },
				{ dev.device.queueDevice; })
		};
	
		HIDDeviceService.action_({arg productID, vendorID, locID, cookie, val;
			if (verbose)	{ [productID, vendorID, locID, cookie, val].postln; };
			try { 
				maps.at(locID).at(cookie).value(val); 
			} { 
				// fall thru to next action here...
				("fall thru"+[productID, vendorID, locID, cookie, val]).postln;
			}
		});
		
		{ HIDDeviceService.runEventLoop(rate) }.defer;
		
	}
	*/
	

}