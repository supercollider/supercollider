NodeMapSetting {
	var <>key, <>value, <>bus;
	
	*new { arg key, value, bus;
		^super.newCopyArgs(key, value, bus)
	}
	
	addToBundle { arg bundle, nodeID;
		if(bus.notNil, {
			this.addBusToBundle(bundle, nodeID)
		}, {
			if(value.isSequenceableCollection, {
				bundle.add([16, nodeID, key, value.size]++value)
			}, {
				bundle.add([15, nodeID, key, value])
			})
		})
	}
	
	addBusToBundle { arg bundle, nodeID;
		bundle.add([14, nodeID, key, bus])
	}
	
	copy {
		^this.class.new(key, value, bus)
	}
	
	isEmpty { ^value.isNil && bus.isNil }
}


ProxyNodeMapSetting : NodeMapSetting {
	var <>channelOffset=0, <>lag;
	
	addBusToBundle { arg bundle, nodeID;
		if(bus.index.notNil, {
			bundle.add([14, nodeID, key, bus.index + channelOffset])
		})
	}
	isEmpty {
		^lag.isNil and: { super.isEmpty }
	}

}