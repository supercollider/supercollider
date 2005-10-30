NodeMapSetting {
	var <>key, <>value, <>busNumChannels, <>isMultiChannel=false, <>isMapped=false;
	
	*new { arg key, value, busNumChannels;
		^super.newCopyArgs(key, value, busNumChannels)
	}
	
	map { arg index;
		value = index;
		isMultiChannel = false;
		isMapped = true;
		busNumChannels = 1;
	}
	mapn { arg index, numChannels;
		value = index;
		busNumChannels = numChannels;
		isMultiChannel = true;
		isMapped = true;
	}
	set { arg val;
		value = val;
		isMapped = false;
		isMultiChannel = val.isSequenceableCollection;
	}
	getValue { // unmapped, single channel only
		^if(this.isMapped.not and: { this.isMultiChannel.not }) { value } { nil }
	}
		
	updateNodeMap { arg nodeMap;
		if(isMapped) {
			if(this.isNeutral) { nodeMap.upToDate = false; ^this }; // ignore setting
			this.updateBusNumChannels;
			if(isMultiChannel) {
				nodeMap.mapnArgs = nodeMap.mapnArgs.addAll([key, this.index, busNumChannels]);
			}{
				nodeMap.mapArgs = nodeMap.mapArgs.addAll([key, this.index]);
			}
		} {
			if(value.notNil) {
				if(isMultiChannel) {
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
	isNeutral { ^false }
	updateBusNumChannels {}
	
	storeArgs { ^[value, busNumChannels] }

	printOn { arg stream;
		stream << this.storeArgs
	}
}


ProxyNodeMapSetting : NodeMapSetting {
	var <>rate; // rate is the synthDef "rate" arg, value is a proxy !
	
	index { ^value.index }
	isEmpty {
		^value.isNil and: { rate.isNil }
	}
	isNeutral { ^value.isNeutral }
	storeArgs { ^[value, busNumChannels, rate] }
	updateBusNumChannels { 
		if(isMultiChannel and: { busNumChannels.isNil }) { 
			busNumChannels = value.numChannels; 
		};
	}

}

