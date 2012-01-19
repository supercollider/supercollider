GeneralHID{
	classvar <>all, <>verbose=false;
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

	*open { |dev| ^scheme.open(dev); }

		// maybe better define interface here instead,
		// rather than doesNotUnderstand.
	*doesNotUnderstand { arg selector ... args;
		//	["GeneralHID forwarding to scheme:", selector, args].postln;
		^scheme.performList( selector, args );
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
			if(thisProcess.platform.name != \windows){ // on win we know it's not yet supported
				("GeneralHID.fromID : The HID scheme '" ++ id ++ "' is not installed\n" ++
				"The current scheme is still '" ++ if( scheme.notNil, { scheme.id }) ++ "'!").warn;
			}
		});
		^scheme;
	}
		// find a device by its info properties
	*findBy { |vendorID, productID, locID, versionID|
		if ( locID.isKindOf( String ), { locID = locID.asSymbol } );
		^this.deviceList.detect { |pair|
			var dev, info; #dev, info = pair;
			(info.vendor == vendorID)
			and: { productID.isNil or: { info.product == productID } }
			and: { locID.isNil or: { info.physical == locID } }
			and: { versionID.isNil or: { info.version == versionID } }
		};
	}
}

GeneralHIDInfo{
	var <name, <bustype, <vendor, <product, <version, <physical, <unique;

	*new{arg  name="", bustype=0, vendor=0, product=0, version=0, physical=0, unique=0;
		^super.newCopyArgs( name, bustype, vendor, product, version, physical, unique ).init;
	}

	init{
	}

	findArgs{
		^[ vendor, product, physical, version ];
	}

	printOn { | stream |
		super.printOn(stream);
		stream << $( << name << ", ";
		// " VendorID: " << vendor << ", ProductID: " << product << ", locID: " << physical << ", version: " << version << $);
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
	classvar all;
	var <slots, <spec;
	var <device, <info;

	*initClass {
		all = [];
		//	specs = IdentityDictionary.new;
	}
	*all {
		^all.copy
	}
	*closeAll {
		all.copy.do({ | dev | dev.close });
	}
	/*	*register { | name, spec |
		specs[name] = spec;
		}*/
	*new { |newDevice|
		^all.detect({ | dev | dev.device == newDevice }) ?? { super.new.init(newDevice) }
	}
	init{ |newDevice|
		device = newDevice;
		slots = device.getSlots;
		info = device.getInfo;
		spec = GeneralHIDSpec.new( this );
	}
	close{
		device.close;
	}
	isOpen{
		^device.isOpen;
	}
	/*	info{
		^device.info;
		}
	*/
	debug_{ |onoff,allslots=true|
		device.class.debug_( onoff );
		// is this necessary? to also turn on/off all the slot debugging?
		if ( allslots ){
			slots.do{ |sl|
				sl.do{ |slt| slt.debug_( onoff ) }
			};
		};
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
	
	hidDeviceAction_{ |func|
		device.hidDeviceAction = func;	
	}

	hidDeviceAction{
		^device.hidDeviceAction;	
	}

	// spec support
	findSpec{
		^spec.find;
	}

	setSpec{ |name|
		spec.fromFile( name );
	}

	add{ |key, slot|
		spec.add( key, slot );
	}

	at{ |key|
		^spec.at( key );
	}

	value{ |key|
		^spec.value( key );
	}

	bus{ |key|
		^spec.bus( key );
	}

	value_{ |key,value|
		spec.value_( key, value );
	}

	action_{ |key,action|
		if ( (key.class == Function) and: action.isNil,{
			device.action = key;
		},{
			spec.action_( key, action );
		});
	}

	createBus{ |name,server|
		spec.createBus( name, server );
	}

	freeBus{ |name|
		spec.freeBus( name );
	}

	createAllBuses{ |server|
		spec.createAllBuses( server );
	}

	freeAllBuses{
		spec.freeAllBuses;
	}

	makeGui{
		^GeneralHIDDeviceGUI.new( this );
	}


}

GeneralHIDSlot{
	var <type, <id, <device, <devSlot;
	classvar <typeMap;
	var <action;
	var <bus, <busAction;
	var <debugAction;
	var <>key;

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
		debugAction = {};
		action = {};
		devSlot.action = { |v| this.debugAction.value(v); this.busAction.value(v); this.action.value(v) };
	}

	value {
		^devSlot.value;
	}

	rawValue {
		^devSlot.rawValue;
	}

	value_ { |v|
		devSlot.value_( v );
	}

	debug_{ |onoff|
		if ( onoff, {
			//	this.action_({ |slot| [ slot.type, slot.code, slot.value, key ].postln; });
			this.debugAction_({ |slot| [ slot.type, slot.code, slot.value, key ].postln; });
		},{
			//	this.action_({});
			this.debugAction_({});
		});
	}

	debugAction_{ |actionFunc|
		debugAction = actionFunc;
	}

	action_{ |actionFunc|
		action = actionFunc;
		//	devSlot.action = { |v| this.action.value(v); this.busAction.value(v); this.debugAction.value(v) };
	}

	createBus{ |s|
		s = s ? Server.default;
		if ( bus.isNil, {
			bus = Bus.control( s, 1 );
		},{
			if ( bus.index.isNil, {
				bus = Bus.control( s, 1 );
			});
		});
		/*		if ( s.serverRunning.not and: s.isLocal, {
			"Server seems not running, so bus will be invalid".warn;
			});*/
		busAction = { |v| bus.set( v.value ); };
		//	devSlot.action = { |v| action.value(v); busAction.value(v); };
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

// JITLib support
	kr{
		this.createBus;
		^In.kr( bus );
	}

}