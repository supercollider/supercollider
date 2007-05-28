/* Wrapper for LID for General HID support */

GLID{
	classvar extraClasses;
	classvar <debug = false;
	var <device;

	*initClass{
		if ( \LID.asClass.notNil, {
			Class.initClassTree( Event );
			extraClasses = Event.new;
			Class.initClassTree( GeneralHID );
			GeneralHID.add( this );
		});
	}

	*id { ^\linux_hid }

	*put { arg key, object;
		extraClasses.put( key, object );
	}

	*doesNotUnderstand { arg selector ... args;
		^extraClasses.perform( selector, *args );
	}

	// ------------ functions ------------

	*buildDeviceList{
		^LID.buildDeviceList;
	}

	*deviceList{
		^LID.deviceList;
	}

	*postDevices { 
		LID.deviceList.do({arg dev;
			//			"".postln;
			if ( dev[1].isKindOf( LIDInfo ), {
				[ dev[1].vendor, dev[1].asString, dev[0]].postcs;
			},{
				dev.postcs;
			});
		});
	}

	*postDevicesAndProperties { 
		LID.deviceList.do({arg dev;
			"".postln;
			if ( dev[1].isKindOf( LIDInfo ), {
				
				[ dev[1].vendor, dev[1].asString, dev[0]].postcs;
				//[dev[0], dev[1].name, dev[1].product, dev[1].vendor, dev[1].version].postcs;
				dev[2].keysValuesDo{ |key,slotgroup,i|
					("\t"++key+LIDSlot.slotTypeStrings[key]).postln;
					slotgroup.do{ |slot|
						"\t\t".post;
						if ( slot.isKindOf( LIDAbsSlot ),{
							[ slot.type, LIDSlot.slotTypeStrings[slot.type], slot.code, slot.info.asString ].postcs;
						},{
							[ slot.type, LIDSlot.slotTypeStrings[slot.type], slot.code ].postcs;
						});
					};
				};
			},{
				dev.postcs;
			});
		});	
	}

	*startEventLoop{ |rate|
		// event loop starts at startup with LID
	}

	*stopEventLoop{
		// event loop is stopped at shutdown with LID
	}

	*eventLoopIsRunning{
		^LID.eventLoopIsRunning;
	}

	*debug_{ |onoff|
		debug = onoff;
	}

	*open { arg dev;
		^super.new.init( dev );
	}

	init{ |dev|
		var mydev;
		mydev = dev[0];
		//mydev.postln;
		if ( mydev[1].isKindOf( LIDInfo ),
				{ 
					device = LID.new( mydev[0] );
					^GeneralHIDDevice.new( this );
				},{
					"not a valid device or could not open device".warn;
					^nil;
				});
	}

	getSlots{
		var mySlots = IdentityDictionary.new;
		var devSlots = device.slots;
		devSlots.keysValuesDo{ |key,value,i|
			//		(""++i+"key"+key+"value"+value).postcs;
			mySlots[key] = IdentityDictionary.new;
			value.keysValuesDo{ |key2,value2,i2|
				mySlots[key][key2] = GeneralHIDSlot.new( key, key2, device, value2 );
			};
		};
		^mySlots;
	}

	close{
		if ( device.notNil,
			{
				device.close;
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

	info{
		if ( device.notNil,
			{
				^device.info;
			},{
				^nil;
			}
		);
	}

}