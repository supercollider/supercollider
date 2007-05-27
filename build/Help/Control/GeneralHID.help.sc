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
LID.eventLoopIsRunning;

d.at(1)

// open a device:
a = GeneralHID.open( d[0] );
d[0]
// close the device:
a.close;

a.device.info
a.device.isOpen