NodeMapSetting {
	var <>key, <>value, <>busNumChannels, <>isMultiChannel=false, <>isMapped=false, <>mappedRate;
	
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
	mapa { arg index;
		this.map(index);
		mappedRate = \audio;
	}
	mapan { arg index, numChannels;
		this.map(index, numChannels);
		mappedRate = \audio;
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
				if(mappedRate === \audio) {
					nodeMap.mapanArgs = nodeMap.mapanArgs.addAll([key,this.index,busNumChannels])
				} {
					nodeMap.mapnArgs = nodeMap.mapnArgs.addAll([key, this.index, busNumChannels])
				}
			}{
				if(mappedRate === \audio) {	
					nodeMap.mapaArgs = nodeMap.mapaArgs.addAll([key, this.index]);
				} {
					nodeMap.mapArgs = nodeMap.mapArgs.addAll([key, this.index]);
				}
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
	map { arg proxy;
		value = proxy;
		isMultiChannel = false;
		isMapped = true;
		busNumChannels = 1;
		mappedRate = proxy.rate; // here we determine the rate simply from the input proxy
	}
	mapn { arg proxy, numChannels;
		value = proxy;
		busNumChannels = numChannels;
		isMultiChannel = true;
		isMapped = true;
		mappedRate = proxy.rate; // here we determine the rate simply from the input proxy
	}
	isNeutral { ^value.isNeutral }
	storeArgs { ^[value, busNumChannels, rate] }
	updateBusNumChannels { 
		if(isMultiChannel and: { busNumChannels.isNil }) { 
			busNumChannels = value.numChannels; 
		};
	}

}

