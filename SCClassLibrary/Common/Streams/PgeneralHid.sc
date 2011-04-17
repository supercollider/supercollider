//human device interface pattern. pulls values from devices like gamepads etc.
// based on GeneralHID

PhidSlot : Pattern {
	var <>slot,<>type,<>device;
	var <>repeats;
	var <>pSlot;

	// the Device list must have been built.
	*new { arg slot, type, device, repeats=inf;
		^super.new.init(slot, type, device, repeats)
	}

	init{|sl,tp,dev,rep|
		device = dev;
		slot = sl;
		type = tp;
		repeats = rep;
		if ( device.isKindOf( GeneralHIDDevice ).not,
			{
				try { device = GeneralHID.open( device ); }{ "device argument is not a GeneralHIDDevice".error; ^nil }
			});
	}

	storeArgs { ^[slot, type, device, repeats] }

	embedInStream { arg inval;

		// slot and type streams
		var slotStr = slot.asStream;
		var typeStr = type.asStream;
		var slotVal, typeVal, slottypes;

		repeats.value.do({

			slotVal = slotStr.next(inval);
			typeVal = typeStr.next(inval);

			slottypes = [slotVal, typeVal].flop;

			inval = slottypes.collect{ |it|
				var ret;
				if ( device.slots[ it[1] ].isNil,
					{ "slot type not found".warn;
						ret = inval;
					},{
					if ( device.slots[ it[1] ][ it[0] ].isNil,
						{ "slot not found".warn;
							ret = inval;
						},{
							ret = device.slots[ it[1] ][ it[0] ].value;
						});
					});
				ret;
			}.unbubble.yield;

		});

		^inval;
	}
}


PhidKey : Pattern {
	var <>key,<>device;
	var <>repeats;
	var <>pSlot;

	// the Device list must have been built.
	*new { arg key, device, repeats=inf;
		^super.new.init(key, device, repeats)
	}

	init{|ky,dev,rep|
		key = ky;
		device = dev;
		repeats = rep;
		if ( device.isKindOf( GeneralHIDDevice ).not,
			{
				try { device = GeneralHID.open( device ); }{ "device argument is not a GeneralHIDDevice".error; ^nil }
			});
	}

	storeArgs { ^[key, device, repeats] }

	embedInStream { arg inval;
		// key stream
		var keyStr = key.asStream;
		var keyVal;

		repeats.value.do({

			keyVal = keyStr.next(inval);
			keyVal = keyVal.asArray;

			inval = keyVal.collect{ |it|
				var ret;
				if ( device.at(it).isNil,
					{ "slot not found".warn;
						ret = inval;
					},{
						ret = device.at( it ).value;
					});
				ret;
			}.unbubble.yield;

		});

		^inval;
	}
}
