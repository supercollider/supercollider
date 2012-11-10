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

	addToEvent { arg event;
		var mapPrefix;
		if(isMapped) {
			mapPrefix = if(mappedRate === \audio) { "a" } { "c" };
			event.put(key, [{ |i| mapPrefix ++ (i + this.index) }.dup(busNumChannels)])
		} {
			if(isMultiChannel) {
				event.put(key, [value])
			} {
				event.put(key, value)
			}
		}
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
		^this.class.prNew(key, value, busNumChannels, isMultiChannel, isMapped, mappedRate)
	}
	
	asControlName {
		^ControlName(key, nil, mappedRate, value)
	}

	isEmpty { ^value.isNil }
	index { ^value }
	isNeutral { ^false }
	updateBusNumChannels {}

	storeArgs { ^[key, value, busNumChannels] }

	printOn { arg stream;
		stream << this.storeArgs
	}

	*prNew { arg key, value, busNumChannels, isMultiChannel=false, isMapped=false, mappedRate;
		^super.newCopyArgs(key, value, busNumChannels, isMultiChannel, isMapped, mappedRate)
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
		isMapped = true;
		// stays neutral if both mappedRate, busNumChannels are nil
		if(proxy.isNeutral) { proxy.initBus(mappedRate, busNumChannels) }; // could add a test
		busNumChannels = proxy.numChannels;
		busNumChannels !? { isMultiChannel = busNumChannels > 1 };
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
