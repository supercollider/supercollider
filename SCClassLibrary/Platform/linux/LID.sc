LIDInfo {
	var <name, <bustype, <vendorID, <productID, <version, <physical, <unique;
	var <>path;

	printOn { | stream |
		super.printOn(stream);
		stream << $( << name << ", " << path << ", ";
		[
			vendorID,
			productID,
			version,
			bustype
		].collect({ | x | "0x" ++ x.asHexString(4) }).printItemsOn(stream);
		stream << ", " << physical << ", " << unique;
		stream.put($));
	}

	postInfo {
		"\tName: \t%\n".postf( name );
		"\tVendor and product ID: \t%, %\n".postf( vendorID, productID );
		"\tPath: \t%\n".postf( path );
		"\tPhysical: \t%\n".postf( physical );
		"\tVersion and bustype: \t%, %\n".postf( version, bustype );
		"\tUnique: \t%\n".postf( unique );
		// "\tUsage name and page: \t%, \t%\n".postf( this.usageName, this.pageName );
		// "\tVendor name: \t%\n".postf( vendor );
		// "\tProduct name: \t%\n".postf( product );
	}

	open{
		^LID.new( path );
	}

	findArgs {
		^[vendorID, productID, path, version, physical, unique]
	}
}

LIDAbsInfo {
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

LID {
	var dataPtr, <path, <info, <caps, spec, <slots, <isGrabbed=false, <>action;
	var <>closeAction;
	var debugAction;
	classvar openDevices, eventTypes, <specs, <>deviceRoot = "/dev/input", <available;
	classvar eventLoopIsRunning = false;
	classvar globalDebugAction;
	classvar <action, prAction;

	*running{
		^eventLoopIsRunning;
	}

	*initClass {
		// all = []; // becomes openDevices
		specs = IdentityDictionary.new;

		available = IdentityDictionary.new;
		openDevices = [];
		// availableUsages = IdentityDictionary.new;

		eventTypes = [
			// maps event type (index) to max code value
			0x0001,		// EV_SYN
			0x02ff,		// EV_KEY
			0x000f,		// EV_REL
			0x003f,		// EV_ABS
			0x0007,		// EV_MSC
			0x000f,     // EV_SW (switch) added by nescivi

			nil, nil, nil,
			nil, nil, nil, nil,
			nil, nil, nil, nil,

			0x000f,		// EV_LED
			0x0007,		// EV_SND

			nil,

			0x0001,		// EV_REP
			0x007f,		// EV_FF
			0x0000,		// EV_PWR
			0x0001,		// EV_FF_STATUS

			nil, nil, nil, nil,
			nil, nil, nil
		];
	}

	*initializeLID{
		"Starting LID eventloop".postln;
		this.prStartEventLoop;
		eventLoopIsRunning = true;
		ShutDown.add {
			this.closeAll;
			this.prStopEventLoop;
		};
	}

	*findAvailable{ |name|
		var devicePaths, d, open;
		if ( eventLoopIsRunning.not ){ this.initializeLID; };
		name = name ? "event";
		devicePaths = (deviceRoot++"/"++name++"*").pathMatch;
		// deviceList = Array.fill( devices.size, 0 );

		available = IdentityDictionary.new;

		devicePaths.do{ |it,i|
			open = false;
			if ( openDevices.detect({ | dev | dev.path == it }).notNil, {open = true});
			d = try { LID( it ) };
			if ( d != nil,
				{
					d.info.path_( it );
					available.put( i, d.info ); // why did I need the slots already here?
					if ( open.not, {d.close} );
				},{
					// just print that device is not openable, and don't add it to the available list
					("LID: could not open device with path"+ it + "\n" ).warn;
				}
			);
		};
		"LID: found % devices\n".postf( available.size );
		^available
	}

	*postAvailable {
		this.available.sortedKeysValuesDo { |k, v| "%: ".postf( k ); v.postInfo; };
	}

	*register { | name, spec |
		specs[name] = spec;
	}

	*mouseDeviceSpec {
		^(
			// key
			b1: #[0x0001, 0x0110],	// left button
			b2: #[0x0001, 0x0111],	// middle button
			b3: #[0x0001, 0x0112],	// right button
			// rel
			x: #[0x0002, 0x0000],	// x axis
			y: #[0x0002, 0x0001],	// y axis
			s: #[0x0002, 0x0008]	// scroll wheel
		)
	}
	*keyboardDeviceSpec {
		^(
			esc: [1, 1],
			one:  [1, 2], two: [1, 3], three: [1, 4], four: [1, 5],
			five: [1, 6], six: [1, 7], seven: [1, 8], eight: [1, 9],
			nine: [1, 10], zero: [1, 11], minus: [1, 12], equal: [1, 13],
			backspace: [1, 14],
			tab: [1, 15], q: [1, 16], w: [1, 17], e: [1, 18],
			r: [1, 19], t: [1, 20], y: [1, 21], u: [1, 22], i: [1, 23],
			o: [1, 24], p: [1, 25], leftbrace: [1, 26], rightbrace: [1, 27],
			enter: [1, 28],
			leftctrl: [1, 29],
			a: [1, 30], s: [1, 31], d: [1, 32], f: [1, 33], g: [1, 34],
			h: [1, 35], j: [1, 36], k: [1, 37], l: [1, 38], semicolon: [1, 39],
			apostrophe: [1, 40],
			grave: [1, 41],
			leftshift: [1, 42],
			backslash: [1, 43],
			z: [1, 44], x: [1, 45], c: [1, 46], v: [1, 47], b: [1, 48],
			n: [1, 49], m: [1, 50], comma: [1, 51], dot: [1, 52],
			slash: [1, 53], rightshift: [1, 54],
			kpasterisk: [1, 55],
			leftalt: [1, 56], space: [1, 57], capslock: [1, 58],
			f1: [1, 59], f2: [1, 60], f3: [1, 61], f4: [1, 62],
			f5: [1, 63], f6: [1, 64], f7: [1, 65], f8: [1, 66],
			f9: [1, 67], f10: [1, 68], numlock: [1, 69], scrolllock: [1, 70],
			kp7: [1, 71], kp8: [1, 72], kp9: [1, 73], kpminus: [1, 74],
			kp4: [1, 75], kp5: [1, 76], kp6: [1, 77], kpplus: [1, 78],
			kp1: [1, 79], kp2: [1, 80], kp3: [1, 81],
			kp0: [1, 82], kpdot: [1, 83],
			zenkakuhankaku: [1, 85],
			the102nd: [1, 86],
			f11: [1, 87],
			f12: [1, 88],
			ro: [1, 89],
			katakana: [1, 90],
			hiragana: [1, 91],
			henkan: [1, 92],
			katakanahiragana: [1, 93],
			muhenkan: [1, 94],
			kpjpcomma: [1, 95],
			kpenter: [1, 96],
			rightctrl: [1, 97],
			kpslash: [1, 98],
			sysrq: [1, 99],
			rightalt: [1, 100],
			linefeed: [1, 101],
			home: [1, 102],
			up: [1, 103],
			pageup: [1, 104],
			left: [1, 105],
			right: [1, 106],
			end: [1, 107],
			down: [1, 108],
			pagedown: [1, 109],
			insert: [1, 110],
			delete: [1, 111],
			macro: [1, 112],
			mute: [1, 113],
			volumedown: [1, 114],
			volumeup: [1, 115],
			power: [1, 116],
			kpequal: [1, 117],
			kpplusminus: [1, 118],
			pause: [1, 119],
			kpcomma: [1, 121],
			hanguel: [1, 122],
			hanja: [1, 123],
			yen: [1, 124],
			leftmeta: [1, 125],
			rightmeta: [1, 126],
			compose: [1, 127],
			stop: [1, 128],
			again: [1, 129],
			props: [1, 130],
			undo: [1, 131],
			front: [1, 132],
			copy: [1, 133],
			open: [1, 134],
			paste: [1, 135],
			find: [1, 136],
			cut: [1, 137],
			help: [1, 138],
			menu: [1, 139],
			calc: [1, 140],
			setup: [1, 141],
			sleep: [1, 142],
			wakeup: [1, 143],
			file: [1, 144],
			sendfile: [1, 145],
			deletefile: [1, 146],
			xfer: [1, 147],
			prog1: [1, 148],
			prog2: [1, 149],
			www: [1, 150],
			msdos: [1, 151],
			coffee: [1, 152],
			direction: [1, 153],
			cyclewindows: [1, 154],
			mail: [1, 155],
			bookmarks: [1, 156],
			computer: [1, 157],
			back: [1, 158],
			forward: [1, 159],
			closecd: [1, 160],
			ejectcd: [1, 161],
			ejectclosecd: [1, 162],
			nextsong: [1, 163],
			playpause: [1, 164],
			previoussong: [1, 165],
			stopcd: [1, 166],
			record: [1, 167],
			rewind: [1, 168],
			phone: [1, 169],
			iso: [1, 170],
			config: [1, 171],
			homepage: [1, 172],
			refresh: [1, 173],
			exit: [1, 174],
			move: [1, 175],
			edit: [1, 176],
			scrollup: [1, 177],
			scrolldown: [1, 178],
			kpleftparen: [1, 179],
			kprightparen: [1, 180],
			new: [1, 181],
			redo: [1, 182],
			f13: [1, 183],
			f14: [1, 184],
			f15: [1, 185],
			f16: [1, 186],
			f17: [1, 187],
			f18: [1, 188],
			f19: [1, 189],
			f20: [1, 190],
			f21: [1, 191],
			f22: [1, 192],
			f23: [1, 193],
			f24: [1, 194],
			playcd: [1, 200],
			pausecd: [1, 201],
			prog3: [1, 202],
			prog4: [1, 203],
			suspend: [1, 205],
			close: [1, 206],
			play: [1, 207],
			fastforward: [1, 208],
			bassboost: [1, 209],
			print: [1, 210],
			hp: [1, 211],
			camera: [1, 212],
			sound: [1, 213],
			question: [1, 214],
			email: [1, 215],
			chat: [1, 216],
			search: [1, 217],
			connect: [1, 218],
			finance: [1, 219],
			sport: [1, 220],
			shop: [1, 221],
			alterase: [1, 222],
			cancel: [1, 223],
			brightnessdown: [1, 224],
			brightnessup: [1, 225],
			media: [1, 226],
			switchvideomode: [1, 227],
			kbdillumtoggle: [1, 228],
			kbdillumdown: [1, 229],
			kbdillumup: [1, 230],
			send: [1, 231],
			reply: [1, 232],
			forwardmail: [1, 233],
			save: [1, 234],
			documents: [1, 235]
		)
	}

	*openDevices{
		^openDevices.copy;
	}

	*closeAll {
		openDevices.copy.do{ |dev| dev.close };
		this.prStopEventLoop;
		eventLoopIsRunning = false;
	}

	*openAt{ |index|
		^available.at( index ).open;
	}

	*findBy{ |vendorID, productID, path, version, physical, unique|
		if ( [vendorID, productID, path, version, physical, unique].every( _.isNil ) ) {
			^nil;
		};
		^LID.available.select{ |info|
			vendorID.isNil or: { info.vendorID == vendorID } and:
			{ productID.isNil or: { info.productID == productID } } and:
			{ path.isNil or: { info.path == path } } and:
			{ version.isNil or: { info.version == version } } and:
			{ physical.isNil or: { info.physical == physical.asSymbol } } and:
			{ unique.isNil or: { info.unique == unique.asSymbol } }
		};
	}

	*open{ |vendorID, productID, path, version, physical, unique|
		var devInfo, device;
		devInfo = this.findBy( vendorID, productID, path, version, physical, unique );
		if ( devInfo.isNil ){
			("LID: could not find device" + vendorID + "," + productID + "," + path + "\n").error;
			^nil;
		};
		devInfo = devInfo.asArray.first;
		device = LID.new( devInfo.path );
		// merge usageDict?
		^device;
	}

	*openPath { |path|
		// "LID: Opening device %\n".postf( path );
		^LID.new( path );
	}

	/*
	*mergeUsageDict { |dev|
		dev.usages.keysValuesDo { |key, val|
			if ( availableUsages.at( key ).isNil ) {
				availableUsages.put( key, IdentityDictionary.new );
			};
			availableUsages.at( key ).put( dev.id, val );
		};
	}

	*removeUsageDict { |dev| // when device is closed
		availableUsages.do { |val|
			val.removeAt( dev.id );
		};
	}
	*/

	*new { | path |
		path = PathName(path);
		if (path.isRelativePath) {
			path = (deviceRoot ++ "/" ++ path.fullPath).standardizePath
		}{
			path = path.fullPath;
		};
		^openDevices.detect({ | dev | dev.path == path }) ?? { super.new.prInit(path) }
	}

	postInfo{
		this.info.postInfo;
	}

	vendor{
		^this.info.vendorID;
	}

	product{
		^this.info.productID;
	}

	postSlots{
		slots.sortedKeysValuesDo{ |k,v|
			v.sortedKeysValuesDo{ |ks,vs|
				"%,%: %\n".format( k, ks, vs.key ).post;
				vs.postInfo;
			};
		};
	}

	isOpen {
		^dataPtr.notNil
	}

	close {
		if (this.isOpen) {
			closeAction.value;
			this.prClose;
			openDevices.remove(this);
		};
	}

	dumpCaps {
		caps.keys.do { | evtType |
			Post << "0x" << evtType.asHexString << ":\n";
			caps[evtType].do { | evtCode |
				Post << $\t << "0x" << evtCode.asHexString << "\n";
			}
		}
	}

	debug_{ |onoff|
		if ( onoff ){
			debugAction =  { | evtType, evtCode, value |
				[this.info.name, evtType, evtCode, value].postln;
			};
		}{
			debugAction = nil;
		}
	}

	debug{
		^debugAction.notNil;
	}

	slot { | evtType, evtCode |
		^slots.atFail(evtType, {
			Error("event type not supported").throw
		}).atFail(evtCode, {
			Error("event code not supported").throw
		})
	}
	at { | controlName |
		^this.slot(*this.spec.atFail(controlName, {
			Error("invalid control name").throw
		}))
	}

	getAbsInfo { | evtCode |
		^this.prGetAbsInfo(evtCode, LIDAbsInfo.new)
	}
	getKeyState { | evtCode |
		^this.prGetKeyState(evtCode)
	}
	getLEDState { | evtCode |
		^0
	}
	setLEDState { |evtCode, evtValue |
		^this.prSetLedState( evtCode, evtValue )
	}
	setMSCState { |evtCode, evtValue |
		^this.prSetMscState( evtCode, evtValue )
	}

	grab { | flag = true |
		// useful when using mouse or keyboard. be sure to have an
		// 'exit point', or your desktop will be rendered useless ...
		if (isGrabbed != flag) {
			this.prGrab(flag);
			isGrabbed = flag;
		};
	}
	ungrab {
		this.grab(false)
	}

	*debug_{ |onoff = true|
		if ( onoff ){
			globalDebugAction = { | device, evtType, evtCode, value |
				[device.info.name, evtType, evtCode, value].postln;
			};
		}{
			globalDebugAction = nil;
		}
	}

	*debug{
		^globalDebugAction.notNil;
	}

	// action interface:
	*addRecvFunc { |function|
		if ( prAction.isNil ) {
			prAction = FunctionList.new;
		};
		prAction = prAction.addFunc( function );
	}

	*removeRecvFunc { |function|
		prAction.removeFunc( function );
	}


	*action_ { |function|
		if ( action.notNil ) {
			this.removeRecvFunc( action );
		};
		action = function;
		this.addRecvFunc( function );
	}

	spec{ |forceLookup = false|
		if ( spec.notNil and: forceLookup.not ){ ^spec };
		spec = specs.atFail(info.name, { IdentityDictionary.new });
		spec.keysValuesDo{ |k,v|
			var slot = slots[ v[0] ][ v[1] ];
			if ( slot.notNil ){ slot.key = k };
		};
		^spec;
	}

	// PRIVATE
	*prStartEventLoop {
		_LID_Start
		^this.primitiveFailed
	}
	*prStopEventLoop {
		_LID_Stop
		^this.primitiveFailed
	}
	prInit { | argPath |
		this.prOpen(argPath);
		openDevices = openDevices.add(this);
		closeAction = {};
		path = argPath;
		info = this.prGetInfo(LIDInfo.new);
		info.path_( path );
		("LID: Opened device: %\n".postf( this.info ) );
		caps = IdentityDictionary.new;
		slots = IdentityDictionary.new;
		eventTypes.do { | evtTypeMax, evtType |
			// nescivi: below was evtType.notNil, but since that is the index, that makes no sense... however evtTypeMax can be nil, and should be skipped if it is... so I'm changing it.
			if (evtTypeMax.notNil and: { this.prEventTypeSupported(evtType) }) {
				caps[evtType] = List.new;
				slots[evtType] = IdentityDictionary.new;
				for (0, evtTypeMax, { | evtCode |
					if (this.prEventCodeSupported(evtType, evtCode)) {
						caps[evtType].add(evtCode);
						slots[evtType][evtCode] = LIDSlot.new(
							this, evtType, evtCode
						);
					};
				});
				caps[evtType].sort;
			}
		};
	}
	prOpen { | path |
		_LID_Open
		^this.primitiveFailed
	}
	prClose {
		_LID_Close
		^this.primitiveFailed
	}
	prEventTypeSupported { | evtType |
		_LID_EventTypeSupported
		^this.primitiveFailed
	}
	prEventCodeSupported { | evtType, evtCode |
		_LID_EventCodeSupported
		^this.primitiveFailed
	}
	prGetInfo { | info |
		_LID_GetInfo
		^this.primitiveFailed
	}
	prGetKeyState { | evtCode |
		_LID_GetKeyState
		^this.primitiveFailed
	}
	prGetAbsInfo { | evtCode, absInfo |
		_LID_GetAbsInfo
		^this.primitiveFailed
	}
	prGrab { | flag |
		_LID_Grab
		^this.primitiveFailed
	}
	prHandleEvent { | evtType, evtCode, evtValue |
		if ( debugAction.notNil ){
			debugAction.value( evtType, evtCode, evtValue );
		};
		if ( globalDebugAction.notNil ){
			globalDebugAction.value( this, evtType, evtCode, evtValue );
		};
		if ( slots.notNil ){
			// not either or for the device action. Do slot actions in any case:
			slots[evtType][evtCode].rawValue_(evtValue);
			// event callback
			if (action.notNil) {
				action.value(evtType, evtCode, evtValue, slots[evtType][evtCode].value);
			};
		};
		if ( prAction.notNil ){
			prAction.value( this, evtType, evtCode, evtValue );
		}
	}

	// this prevents a high cpu cycle when device was detached; added by marije
	prReadError{
		this.close;
		("WARNING: Device was removed: " + this.path + this.info).postln;
	}

	prSetLedState { |evtCode, evtValue|	// added by Marije Baalman
		// set LED value
		_LID_SetLedState
		^this.primitiveFailed
	}
	prSetMscState { |evtCode, evtValue|
		// set MSC value
		_LID_SetMscState
		^this.primitiveFailed
	}
}

LIDSlot {
	var <device, <type, <code, <spec, <>action;
	var rawValue = 0;
	classvar slotTypeMap, <slotTypeStrings;
	var <bus, busAction;
	var debugAction;
	var <>key;


	*initClass {
		slotTypeMap = IdentityDictionary.new.addAll([
			0x0001 -> LIDKeySlot,
			0x0002 -> LIDRelSlot,
			0x0003 -> LIDAbsSlot,
			0x0004 -> LIDMscSlot,
			0x0011 -> LIDLedSlot,
		]);
		slotTypeStrings = IdentityDictionary.new.addAll([
			0x0000 -> "Syn",
			0x0001 -> "Button",
			0x0002 -> "Relative",
			0x0003 -> "Absolute",
			0x0004 -> "Miscellaneous",
			0x0011 -> "LED",
			0x0012 -> "Sound",
			0x0014 -> "Rep",
			0x0015 -> "Force Feedback",
			0x0016 -> "Power",
			0x0017 -> "Force Feedback Status",
			0x0FFF -> "Linear"
		]);

	}

	postInfo {
		"\tType: \t%, %\n".postf( type, slotTypeStrings.at( type ) );
		"\tCode: \t%\n".postf( code );
		"\tKey: \t%\n".postf( key );
		"\tSpec: \t%\n".postf( spec );
		"\tValue: \t%\n".postf( this.value );
	}

	*new { | device, evtType, evtCode |
		^(slotTypeMap[evtType] ? this).newCopyArgs(device, evtType, evtCode).init.initSpec
	}

	init{
		busAction = {};
		debugAction = {};
		action = {};
	}

	initSpec {
		spec = ControlSpec(0, 1, \lin, 1, 0);
	}
	rawValue {
		^rawValue
	}
	value {
		^spec.unmap(rawValue)
	}
	rawValue_ { | inValue |
		rawValue = inValue;
		action.value(this);
		busAction.value( this );
		debugAction.value( this );
	}
	next {
		^this.value
	}

	debug_{ |onoff|
		if ( onoff, {
			debugAction = { |slot| [ slot.type, slot.code, slot.value, slot.key ].postln; };
		}, {
			debugAction = {};
		});
	}

	debug{
		^debugAction.notNil;
	}

	createBus { |server|
		server = server ? Server.default;
		if ( bus.isNil, {
			bus = Bus.control( server, 1 );
		}, {
			if ( bus.index.isNil, {
				bus = Bus.control( server, 1 );
			});
		});
		busAction = { |v| bus.set( v.value ); };
	}

	freeBus {
		busAction = {};
		bus.free;
		bus = nil;
	}

	// JITLib support
	kr {
		this.createBus;
		^In.kr( bus );
	}
}

LIDKeySlot : LIDSlot {
	initSpec {
		super.initSpec;
		rawValue = device.getKeyState(code);
	}
}

LIDRelSlot : LIDSlot {
	var delta = 0, <>deltaAction;

	initSpec { }
	value { ^rawValue }
	rawValue_ { | inDelta |
		delta = inDelta;
		rawValue = rawValue + delta;
		action.value(this);
		busAction.value( this );
		debugAction.value( this );
		deltaAction.value(this);
	}

	delta { ^delta }

	debug_{ |onoff|
		if ( onoff, {
			debugAction = { |slot| [ slot.type, slot.code, slot.value, slot.delta, slot.key ].postln; };
		}, {
			debugAction = {};
		});
	}


}

LIDLedSlot : LIDSlot {

	initSpec { }
	value { ^rawValue }
	value_{ |inValue| this.rawValue_( spec.map( inValue ) ); }
	rawValue_ { | inValue |
		rawValue = inValue;
		device.setLEDState( code, inValue );
		action.value(this);
		busAction.value( this );
		debugAction.value( this );
	}
}

LIDMscSlot : LIDSlot {

	initSpec { }
	value { ^rawValue }
	value_{ |inValue| this.rawValue_( spec.map( inValue ) ); }
	rawValue_ { | inValue |
		rawValue = inValue;
		device.setMSCState( code, rawValue );
		action.value(this);
		busAction.value( this );
		debugAction.value( this );
	}
}

LIDAbsSlot : LIDSlot {
	var <info;

	initSpec {
		info = device.getAbsInfo(code);
		spec = ControlSpec(info.min, info.max, \lin, 1);
		spec.default = spec.map(0.5).asInteger;
		rawValue = info.value;
	}
}

// EOF
