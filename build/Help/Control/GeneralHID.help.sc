/// testing state!!!

GeneralHID is a cross platform wrapper for accessing HID devices.
Currently the MacOSX and Linux HID support has been wrapped.
Much of the code is inspired by the GUI wrapper.

Get the current scheme:
GeneralHID.scheme
GeneralHID.scheme.id

Build a device list:
GeneralHID.buildDeviceList;

Get the device list (not working currently):
d = GeneralHID.deviceList;
this works however:
d = GeneralHID.buildDeviceList;
d
Post the devices found:
GeneralHID.postDevices;

Post the devices and their capabilities:
GeneralHID.postDevicesAndProperties;

Start the event loop with rate (default is 0.005):
GeneralHID.startEventLoop( 0.005 );

Stop the event loop:
GeneralHID.stopEventLoop;

Check status of eventloop:
GeneralHID.eventLoopIsRunning;


Open a device:
a = GeneralHID.open( d[1] );

Get the device info:
a.info;

Check whether device is open:
a.isOpen;

A dictionary with all slots:
a.slots;

See if data is coming in:
a.debug_( true );

Turn it off again:
a.debug_( false );

(debugging can be turned on for each slot individually)

a.caps;

// close the device:
a.close;


a.device.slots
b = 
(
b = IdentityDictionary.new;
a.device.slots.keysValuesDo{ |key,value,i|
	(""++i+"key"+key+"value"+value).postcs;
	b[key] = IdentityDictionary.new;
	value.keysValuesDo{ |key2,value2,i2|
		b[key][key2] = GeneralHIDSlot.new( key, key2, a.device, value2 );
	};
};a.device.slotsa.device.slotsa.device.sa.device.slots
b = 
(
b = IdentityDictionary.new;lots
b = 
(
b = IdentityDictionary.new;
b = 
(
b = IdentityDictionary.new;
b = 
(
b = IdentityDictionary.new;
)

(
b.keysValuesDo{ |key,value,i|
	(""++i+"key"+key).postln;
	value.keysValuesDo{ |key2,value2,i2|
		value2.asString.postln;
	};
}
)