// =====================================================================
// LID -- Linux Input Device
// =====================================================================
//
// This class provides a way to access devices in the linux input
// layer, which supports many input devices (mouse, keyboard,
// joystick, gamepad, tablet) and busses (serial, PS/2, USB).

// =====================================================================
// Opening a device
// =====================================================================
//
// Input devices are accessed through device nodes, typically
// /dev/input/event[0-9]. When using a userspace daemon like udev,
// meaningful names can be assigned to devices.

// raw device name
d = LID("/dev/input/event4");

// symbolic device name
d = LID("/dev/input/trackball");

// device name relative to LID.deviceRoot
d = LID("gamepad");

// build a table of the available devices:

LID.buildDeviceTable

// buildDeviceTable builds a table of the devices found in LID.deviceRoot+"/event",
// trying to open all that it finds, looking up its name and closing them again.
// the results is returned and can later be accessed by LID.deviceTable.
// you can query another name than "/event" by passing an argument.
// (the search will be: LID.deviceRoot++"/"++name++"*")

LID.buildDeviceTable( "mouse" );

// is likely to give the info that the devices could not be opened, as "mouse"
// uses another interface (you can of course access mice via the "event" interface)

// =====================================================================
// Querying device information
// =====================================================================

d.info;
d.info.name;
d.info.vendor.asHexString(4);
d.info.product.asHexString(4);

// =====================================================================
// Querying device capabilities
// =====================================================================
//
// Device capabilities are reported as event type and event code
// mappings. Event type and event code constants can be found in
// /usr/include/linux/input.h

d.caps;
d.dumpCaps;

// =====================================================================
// Event actions (raw events)
// =====================================================================
//
// The device's 'action' instance variable can be used for event
// notifications. it is passed the event type, code and current value.

(
d.action = { | evtType, evtCode, evtValue |
	[evtType.asHexString(4), evtCode.asHexString(4), evtValue].postln
}
)

d.action = nil;

// =====================================================================
// Event actions (raw slot events)
// =====================================================================
//
// When 'action' is nil, actions can be bound to specific events.

(
d.slot(0x0001, 0x0120).action = { | slot |
	[slot.type.asHexString(4), slot.code.asHexString(4), slot.rawValue].postln;
}
)

// =====================================================================
// Device specs
// =====================================================================
//
// Device specs are mappings between event codes and symbolic control
// names. New specs can be added to LID.specs via LID>>*register.

// Add a mouse device spec for a Logitech trackball
LID.register('Logitech Trackball', LID.mouseDeviceSpec);

// Add a custom device spec for a Logitech gamepad
(
LID.register('Logitech WingMan RumblePad', (
	// key
	rumble: #[0x0001, 0x0102],	// rumble (toggles ff)
	mode: #[0x0001, 0x0103],	// mode (switches h and l)
	a: #[0x0001, 0x0120],		// button a
	b: #[0x0001, 0x0121],		// button b
	c: #[0x0001, 0x0122],		// button c
	x: #[0x0001, 0x0123],		// button x
	y: #[0x0001, 0x0124],		// button y
	z: #[0x0001, 0x0125],		// button z
	l: #[0x0001, 0x0126],		// left front button
	r: #[0x0001, 0x0127],		// right front button
	s: #[0x0001, 0x0128],		// button s
	// abs
	lx: #[0x0003, 0x0000],		// left joystick x
	ly: #[0x0003, 0x0001],		// left joystick y
	rx: #[0x0003, 0x0005],		// right joystick x
	ry: #[0x0003, 0x0006],		// right joystick y
	hx: #[0x0003, 0x0010],		// hat x
	hy: #[0x0003, 0x0011],		// hat y
	slider: #[0x0003, 0x0002]	// slider
));
)

// =====================================================================
// Event actions (symbolic slot events)
// =====================================================================
//
// When a device spec was registered for a given device name, slot
// actions can be assigned by using the symbolic control name.

d[\a].action = { | slot | [\a, slot.value].postln };

// There is also a default keyboard device spec.

(
LID.keyboardDeviceSpec.keys.do { | key |
	d[key].action = { | slot | [key, slot.value].postln }
}
)

// =====================================================================
// LED's
// =====================================================================

// some devices have LEDs which can be turned on and off. These show up
// with d.caps as events of type 0x0011

d = LID("/dev/input/event0");
// LED's can be turned on:
d.setLEDState( 0x0, 1 )
// (LED 0x0 should be available on any keyboard)
// and off:
d.setLEDState( 0x0, 0 )
d.close;

// setLEDState( evtCode, evtValue ): value should be 1 or 0


// =====================================================================
// Grabbing devices
// =====================================================================
//
// Given proper permissions, devices can be grabbed to prevent use in
// other applications (including X). Be careful when grabbing mouse or
// keyboard!

d[\b].action = { d.ungrab };
d.grab;

d.isGrabbed;

// =====================================================================
// Closing devices
// =====================================================================

d.close;
LID.closeAll;

// =====================================================================
