NodeMapSetting {
	var <>key, <>value, <>bus;
	
	*new { arg key, value, bus;
		^super.newCopyArgs(key, value, bus)
	}
		
	updateNodeMap { arg nodeMap;
		if(bus.notNil, {
			this.updateBusToNodeMap(nodeMap);
		}, {
			if(value.isSequenceableCollection, {
				nodeMap.setnArgs = nodeMap.setnArgs.addAll([key, value.size]++value)
			}, {
				nodeMap.setArgs = nodeMap.setArgs.addAll([key, value])
			})
		})
	}
	
	updateBusToNodeMap { arg nodeMap;
		nodeMap.mapArgs = nodeMap.mapArgs.addAll([key, bus]);
	}
	
	copy {
		^this.class.new(key, value, bus)
	}
	
	isEmpty { ^value.isNil && bus.isNil }
	
	storeArgs { ^[value, bus] }

	printOn { arg stream;
		stream << this.storeArgs
	}
}


ProxyNodeMapSetting : NodeMapSetting {
	var <>channelOffset=0, <>lag;
	
	
	updateBusToNodeMap { arg nodeMap;
		nodeMap.mapArgs = nodeMap.mapArgs.addAll([key, bus.index +  channelOffset]);
	}
	isEmpty {
		^lag.isNil and: { super.isEmpty }
	}
	storeArgs { ^[value, bus, channelOffset, lag] }


}