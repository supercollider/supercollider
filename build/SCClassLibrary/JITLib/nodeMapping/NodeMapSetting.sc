NodeMapSetting {
	var <>key, <>value, <>bus;
	
	*new { arg key, value, bus;
		^super.newCopyArgs(key, value, bus)
	}
	//assumes order of bundles: [[set, setn, map]
	addToBundle { arg bundle;
		if(bus.notNil, {
			this.addBusToBundle(bundle)
		}, {
			if(value.isSequenceableCollection, {
				bundle[1] = bundle[1].addAll([key, value.size]++value)
			}, {
				bundle[0] = bundle[0].addAll([key, value])
			})
		})
	}
	
	addBusToBundle { arg bundle, nodeID;
		bundle[2] = bundle[2].addAll([key, bus])
	}
	
	copy {
		^this.class.new(key, value, bus)
	}
	
	isEmpty { ^value.isNil && bus.isNil }
}


ProxyNodeMapSetting : NodeMapSetting {
	var <>channelOffset=0, <>lag;
	
	addBusToBundle { arg bundle;
		if(bus.index.notNil, {
			bundle[2] = bundle[2].addAll([ key, bus.index + channelOffset])
		})
	}
	isEmpty {
		^lag.isNil and: { super.isEmpty }
	}

}