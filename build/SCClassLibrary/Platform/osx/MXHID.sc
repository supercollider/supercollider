/* Wrapper for HIDDeviceService for General HID support */

MXHID {
	classvar extraClasses;
	classvar <usageMap;
	classvar all;
	classvar <deviceList;
	classvar <>debug = false;
	var <device, <slots;

	*initClass {
		Class.initClassTree( Event );
		extraClasses = Event.new;
		Class.initClassTree( GeneralHID );
		GeneralHID.add( this );
		this.initUsageMap;
		all = IdentityDictionary.new;
	}
	
	*all{
		^all.copy;
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
		HIDDeviceService.buildDeviceList(nil,nil); 
		//deviceList = HIDDeviceServer.deviceList;
		deviceList = HIDDeviceService.devices.collect{ |dev,i|
			[ dev, this.getInfo( dev ) ]
		}
		^deviceList;
	}

	*postDevices { 
		deviceList.do{ |dev,i|
			[ i, dev[0], dev[1].asString ].postcs;
		}
		/*		HIDDeviceService.devices.do({arg dev;
			//			"".postln;
			this.class.getInfo( dev ).postcs;
			//[dev.manufacturer, dev.info.asString, dev.locID].postcs;
			});*/
	}

	*postDevicesAndProperties { 
		deviceList.do{ |dev,i|
			[ i, dev[0], dev[1].asString ].postcs;
			dev[0].elements.do({arg ele;
				"\t".post;
				[ele.type, ele.usage, ele.cookie, ele.min, ele.max, ele.ioType, ele.usagePage, ele.usageType].postln;
			});
		};
		/*		HIDDeviceService.devices.do({arg dev, i;
			(""++i++": ").postln;
			this.class.getInfo( dev ).postcs;
			//	[dev.manufacturer, dev.info.asString, dev.locID].postcs;
			dev.elements.do({arg ele;
				"\t".post;
				[ele.type, ele.usage, ele.cookie, ele.min, ele.max, ele.ioType, ele.usagePage, ele.usageType].postln;
			});
			});*/
	}

	*getInfo{ |dev|
		var info;
		info = GeneralHIDInfo.new(
			dev.info.name,
			dev.info.bustype,
			dev.info.vendor,
			dev.info.product,
			dev.info.version,
			dev.locID
		);
		^info;
	}

	*startEventLoop{ |rate|
		if ( rate.isNil or: (rate.size == 0),
			{
			HIDDeviceService.runEventLoop;
			},{
			HIDDeviceService.runEventLoop(rate[0]);
			});
		this.initAction;
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
	
	*initAction{
		HIDDeviceService.action_({arg productID, vendorID, locID, cookie, val;
		//	[productID, vendorID, locID, cookie, val].postln;
		//	if (debug) {("debug"+[productID, vendorID, locID, cookie, val]).postln;};
			try { 
				all.at( locID ).slots.at( cookie ).value_(val);
			} { 
				// fall thru to next action here...
				if (debug) {("fall thru"+[productID, vendorID, locID, cookie, val]).postln;}
			}
		});
	}

	init{ |dev|
		dev = dev[0];
		if ( dev.isKindOf( HIDDevice ),
				{ 
					device = dev;
					device.queueDevice;
					slots = IdentityDictionary.new;
					all = all.put( device.locID, this );
					^GeneralHIDDevice.new( this );
				},{
					"not a valid device".warn;
				});
	}

	getInfo{
		var info;
		info = GeneralHIDInfo.new(
			device.info.name,
			device.info.bustype,
			device.info.vendor,
			device.info.product,
			device.info.version,
			device.locID
		);
		^info;
	}
	
	getSlots{
		var mySlots = IdentityDictionary.new;
		var devElements = device.elements;
		GeneralHIDSlot.typeMap.keysValuesDo{ |key|
			mySlots.put( key, IdentityDictionary.new );
			};
		devElements.do{ |ele,i|
			var newSlot, key;
			if ( this.class.usageMap[ele.usagePage].notNil,{
				key = this.class.usageMap[ele.usagePage].matchAt(ele.usageType);
				});
			//("key"+key+ele.usagePage+ele.usageType).postln;
			if ( key.notNil, {
				newSlot =  MXHIDSlot.new( device, key, ele.usageType, ele.cookie );
				slots.put( ele.cookie, newSlot);
				//slots.postln;
				mySlots[key][ele.usageType] =  GeneralHIDSlot.new( key, ele.usageType, device, newSlot );
			});
		};
		mySlots = mySlots.select(_.notEmpty);	// remove empty dicts
		^mySlots;
	}

	action_{ |act|
		device.action = act;
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
				^device.isQueued;
			},{
				^false;
			}
		);
	}
	
	info{
		if ( device.notNil,
			{
				^device.info;
			},{
				^nil
			});
	}

	grab{ // not implemented on osx
	}

	ungrab{ // not implemented on osx
	}
	
	*initUsageMap{
	
	// the usageMap maps the usagePage and usageType to the different slot types we are using
	// this map is not yet complete, due to time limitations. Please write to the sc-user list
	// if you use a device not on one of the defined pages, or when stuff is missing
		usageMap = IdentityDictionary.new;
		usageMap.put( 0x01, Dictionary.new ); // Generic Desktop
		usageMap.at( 0x01 ).put( { |v| ( v >= 0x30 and: ( v <= 0x39 ) ); }, 0x0003 );
		usageMap.at( 0x01 ).put( 0x3B, 0x0002 );
		usageMap.at( 0x01 ).put( { |v| ( v == 0x3D or: ( v == 0x3E ) ); }, 0x0001 );
		usageMap.at( 0x01 ).put( { |v| ( v >= 0x40 and: ( v <= 0x46 ) ); }, 0x0003 ); // maybe 0x0002 ??
		usageMap.at( 0x01 ).put( 0x48, 0x0002 );
		usageMap.at( 0x01 ).put( { |v| ( v >= 0x81 and: ( v <= 0x93 ) ); }, 0x0001 );
		usageMap.at( 0x01 ).put( { |v| ( v >= 0xA0 and: ( v <= 0xA8 ) ); }, 0x0001 );
		usageMap.at( 0x01 ).put( { |v| ( v >= 0xB0 and: ( v <= 0xB7 ) ); }, 0x0001 );
		
		usageMap.put( 0x02, Dictionary.new ); // Simulations Controls
		// feel free to fill in
		usageMap.put( 0x03, Dictionary.new ); // VR Controls
		// feel free to fill in
		usageMap.put( 0x04, Dictionary.new ); // Sport Controls
		// feel free to fill in
		
		usageMap.put( 0x05, Dictionary.new ); // Game Controls
		usageMap.at( 0x05 ).put( { |v| ( v >= 0x20 and: ( v <= 0x29 ) ); }, 0x0003 );
		usageMap.at( 0x05 ).put( { |v| ( v >= 0x2A and: ( v <= 0x31 ) ); }, 0x0001 );
		usageMap.at( 0x05 ).put( { |v| ( v >= 0x33 and: ( v <= 0x35 ) ); }, 0x0001 ); // ?? could be wrong, as they are select type
		
		usageMap.put( 0x06, Dictionary.new ); // Generic Device Controls
		// feel free to fill in
		
		usageMap.put( 0x07, Dictionary.new ); // Keyboard / Keypad
		usageMap.at( 0x07 ).put( { |v| ( v >= 0x01 and: ( v <= 0xA4 ) ); }, 0x0001 );
		usageMap.at( 0x07 ).put( { |v| ( v >= 0xB0 and: ( v <= 0xDD ) ); }, 0x0001 );
		usageMap.at( 0x07 ).put( { |v| ( v >= 0xE0 and: ( v <= 0xE7 ) ); }, 0x0001 );
		
		usageMap.put( 0x08, Dictionary.new ); // LED 
		usageMap.at( 0x08 ).put( { |v| ( v >= 0x01 and: ( v <= 0x39 ) ); }, 0x0011 );
		usageMap.at( 0x08 ).put( { |v| ( v >= 0x4B and: ( v <= 0x4D ) ); }, 0x0011 );
		
		usageMap.put( 0x09, Dictionary.new ); // Button
		usageMap.at( 0x09 ).put( { |v| ( v >= 0x01 and: ( v <= 0xFFFF ) ); }, 0x0001 );
		
		usageMap.put( 0x0A, Dictionary.new ); // Ordinal Page
		// feel free to fill in
		
		usageMap.put( 0x0B, Dictionary.new ); // Telephony Page
		// feel free to fill in
		
		usageMap.put( 0x0C, Dictionary.new ); // Consumer
		// feel free to fill in

		usageMap.put( 0x0D, Dictionary.new ); // Digitizer controls
		usageMap.at( 0x0D ).put( { |v| ( v >= 0x30 and: ( v <= 0x31 ) ); }, 0x0003 );
		usageMap.at( 0x0D ).put( { |v| ( v >= 0x32 and: ( v <= 0x36 ) ); }, 0x0001 );
		usageMap.at( 0x0D ).put( 0x38, 0x0003 );
		usageMap.at( 0x0D ).put( 0x3B, 0x0003 );
		usageMap.at( 0x0D ).put( 0x3C, 0x0001 );
		usageMap.at( 0x0D ).put( { |v| ( v >= 0x3D and: ( v <= 0x41 ) ); }, 0x0003 );
		usageMap.at( 0x0D ).put( { |v| ( v >= 0x42 and: ( v <= 0x46 ) ); }, 0x0001 );
				
		usageMap.put( 0x0F, Dictionary.new ); // Physical Interface Device (Force Feedback)
		// feel free to fill in
		
		usageMap.put( 0x10, Dictionary.new );	// Unicode
		// feel free to fill in
		
		usageMap.put( 0x14, Dictionary.new ); // Alphanumeric
		// feel free to fill in
		
		usageMap.put( 0x40, Dictionary.new ); // Medical instrument
		// feel free to fill in
		
	}
}


MXHIDSlot {
	var <device, <type, <code, <cookie, value=0,  <>action;
	var <spec;
	classvar slotTypeMap;
	
	*initClass {
		slotTypeMap = IdentityDictionary.new.addAll([
			0x0001 -> MXHIDKeySlot,
			0x0002 -> MXHIDRelSlot,
			0x0003 -> MXHIDAbsSlot,
			0x0011 -> MXHIDLedSlot
		]);
	}

	*new { | device, evtType, evtCode, evtCookie |
		^(slotTypeMap[evtType] ? this).newCopyArgs(device, evtType, evtCode, evtCookie).initSpec
	}
	initSpec {
		spec = ControlSpec(0, 1, \lin, 1, 0);
	}
	rawValue {
		^value
	}
	value {
		^spec.unmap(value)
	}
	value_ { | rawValue |
		value = rawValue;
		action.value(this);
		device.action.value( type, code, rawValue, this.value );
	}
	next {
		^this.value
	}
}

MXHIDKeySlot : MXHIDSlot {
	initSpec {
		super.initSpec;
		//FIXME: 
		value = device.valueByCookie(cookie);
	}
}

MXHIDRelSlot : MXHIDSlot {
	var delta, <>deltaAction;

	initSpec { }
	value { ^value }
	value_ { | dta |
		delta = dta;
		value = value + delta;
		action.value(this);
		deltaAction.value(this);
		device.action.value( type, code, dta, value );
	}

	delta { ^delta }
}

MXHIDLedSlot : MXHIDSlot {

	initSpec { }
	value { ^value }
	value_ { | v |
		value = v;
		// FIXME: device.setLEDState( code, value );
		device.setValueByCookie(cookie);
		action.value(this);
		device.action.value( type, code, value, value );
	}
}

MXHIDAbsSlot : MXHIDSlot {
	var <info;

	initSpec {
		info = device.elements.detect( { |ele| ele.cookie == cookie } );
		spec = ControlSpec(info.min, info.max, \lin, 1);
		spec.default = spec.map(0.5).asInteger;
		//value = info.value;
	}
}

MXHIDAbsInfo {
	var <value = 0, <min = 0, <max = 0, <fuzz = 0, <flat = 0;

	printOn { | stream |
		stream
		<< this.class.name << $(
		<< "value: " << value << ", "
		<< "min: " << min << ", "
		<< "max: " << max << ", "
		<< "fuzz: " << fuzz << ", "
		<< "flat: " << flat << $)
	}
}