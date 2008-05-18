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

GeneralHIDInfo{
	var <name, <bustype, <vendor, <product, <version, <physical, <unique;

	*new{arg  name, bustype, vendor, product, version, physical, unique=0;
		^super.newCopyArgs( name, bustype, vendor, product, version, physical, unique ).init;
	}
	
	init{
	}

	printOn { | stream |
		super.printOn(stream);
		stream << $( << name << ", ";
		[
			bustype,
			vendor,
			product,
			version
		].collect({ | x | "0x" ++ x.asHexString(4) }).printItemsOn(stream);
		stream << ", " << physical << ", " << unique;
		stream.put($));
	}
}

GeneralHIDDevice{
	classvar <specs, all;
	var <slots, <spec;
	var <device, <info;

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
		info = device.getInfo;
	}
	close{
		device.close;
	}
	isOpen{
		^device.isOpen;
	}
	/*	info{
		^device.info;
		}*/
	debug_{ |onoff|
		device.class.debug_( onoff );
		slots.do{ |sl|
			sl.do{ |slt| slt.debug_( onoff ) } };
	}
	caps{
		slots.do{ |sl|
			"".postln;
			sl.do{ |slt| "\t".post; slt.asString.postln; } };
	}
	grab{
		device.grab;
	}
	ungrab{
		device.ungrab;
	}
}

GeneralHIDSlot{
	var <type, <id, <device, <devSlot;
	classvar <typeMap;
	var <action;
	var <bus, busAction;

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
			0x0017 -> "Force Feedback Status",
			0x0FFF -> "Linear"
		]);
	}

	*new{ |type,id,device,devSlot|
		^super.newCopyArgs( type, id, device, devSlot ).init;
	}

	init{
		busAction = {};
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
		devSlot.action = { |v| action.value(v); busAction.value(v); };
	}

	createBus{ |s|
		bus = Bus.control( s, 1 );
		busAction = { |v| bus.set( v.value ); };
		devSlot.action = { |v| action.value(v); busAction.value(v); };
	}

	freeBus{ 
		busAction = {};
		bus.free;
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