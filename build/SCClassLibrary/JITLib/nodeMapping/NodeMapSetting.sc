NodeMapSetting {
	var <>key, <>value, <>busNumChannels;
	
	*new { arg key, value, busNumChannels;
		^super.newCopyArgs(key, value, busNumChannels)
	}
	
	map {Êarg index, numChannels=1;
		value = index;
		busNumChannels = numChannels;
	}
	set { arg val;
		value = val;
		busNumChannels = nil;
	}
		
	updateNodeMap { arg nodeMap;
		if(busNumChannels.notNil) {
			if(busNumChannels > 1) {
				nodeMap.mapnArgs = nodeMap.mapnArgs.addAll([key, this.index, busNumChannels]);
			}{
				nodeMap.mapArgs = nodeMap.mapArgs.addAll([key, this.index]);
			}
		} {
			if(value.notNil) {
				if(value.isSequenceableCollection) {
					nodeMap.setnArgs = nodeMap.setnArgs.addAll([key, value.size]++value)
				} {
					nodeMap.setArgs = nodeMap.setArgs.addAll([key, value])
				}
			};
		}
	}
	
	copy {
		^this.class.new(key, value, busNumChannels)
	}
	
	isEmpty { ^value.isNil }
	index { ^value }
	isMapped { ^busNumChannels.notNil }
	
	storeArgs { ^[value, busNumChannels] }

	printOn { arg stream;
		stream << this.storeArgs
	}
}


ProxyNodeMapSetting : NodeMapSetting {
	var <>rate; // rate is the synthDef "rate" arg, bus is a proxy
	
	index { ^value.index }
	isEmpty {
		^value.isNil and: { rate.isNil }
	}
	storeArgs { ^[value, busNumChannels, rate] }


}

