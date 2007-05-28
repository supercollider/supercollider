GeneralHID{
	classvar <>all, <>maps, <>verbose=false; 
	classvar <scheme, <schemes;

	*initClass { 
		schemes	= IdentityDictionary.new;
	}

	*new { arg key; ^scheme.perform( key )}

	*current {
		^scheme;
	}

	*get { arg id;
		^schemes[ id.asSymbol ];
	}

	*set { arg aScheme;
		scheme = aScheme;
	}

	*use { arg aScheme, func;
		var recent	= scheme;
		scheme		= aScheme;
		^func.protect({ scheme = recent });
	}

	*useID { arg id, func;
		^this.use( schemes[ id.asSymbol ], func );
	}

	*doesNotUnderstand { arg selector ... args;
		^scheme.perform( selector, args );
	}

	/*	*deviceList {
		^scheme.deviceList;
		}*/

	*add { arg aScheme;
		schemes.put( aScheme.id, aScheme );
		if( scheme.isNil, {				// first registration = first default kit
			scheme = aScheme;
		}, {
			scheme = schemes[ scheme.id ];	// in case we are updating an existing scheme
		});
	}

	*fromID { arg id;
		var newScheme = schemes[ id.asSymbol ];
		if( newScheme.notNil, {
			scheme = newScheme;
		}, {
			("GeneralHID.fromID : The HID scheme '" ++ id ++ "' is not installed\n" ++
			 "The current scheme is still '" ++ if( scheme.notNil, { scheme.id }) ++ "'!").warn;
		});
		^scheme;
	}
	
}

GeneralHIDDevice{
	classvar <specs, all;
	var <slots, <spec;
	var <device;

	*initClass {
		all = [];
		specs = IdentityDictionary.new;
	}
	*all {
		^all.copy
	}
	*closeAll {
		all.copy.do({ | dev | dev.close });
	}
	*register { | name, spec |
		specs[name] = spec;
	}
	*new { |newDevice|
		^all.detect({ | dev | dev.device == newDevice }) ?? { super.new.init(newDevice) }
	}
	init{ |newDevice|
		device = newDevice;
		slots = device.getSlots;
	}
	close{
		device.close;
	}
	isOpen{
		^device.isOpen;
	}
	info{
		^device.info;
	}
	debug_{ |onoff|
		slots.do{ |sl|
			sl.do{ |slt| slt.debug_( onoff ) } };
	}
	caps{
		slots.do{ |sl|
			"".postln;
			sl.do{ |slt| "\t".post; slt.asString.postln; } };
	}
}

GeneralHIDSlot{
	var <type, <id, <device, <devSlot;
	classvar <typeMap;
	var <action;

	*initClass{
		// typeMap is modeled after Linux input system.
		// to get the Mac work in a similar way, some tricks will be needed
		typeMap = IdentityDictionary.new.addAll([
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
			0x0017 -> "Force Feedback Status"
		]);
	}

	*new{ |type,id,device,devSlot|
		^super.newCopyArgs( type, id, device, devSlot ).init;
	}

	init{
		this.action_( {} );
	}

	value {
		^devSlot.value;
	}

	value_ { |v|
		devSlot.value_( v );
	}

	debug_{ |onoff|
		if ( onoff, {
			this.action_({ |slot| [ slot.type, slot.code, slot.value ].postln; });
		},{
			this.action_({});
		});
	}
	
	action_{ |actionFunc|
		action = actionFunc;
		devSlot.action = actionFunc;
	}

	printOn { | stream |
		stream
		<< this.class.name << $(
		<< this.class.typeMap[type] << ", "
		<< "type: " << type << ", "
		<< "id: " << id << ", "
		<< "value: " << this.value << $)
	}
}