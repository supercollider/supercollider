NodeMap : IdentityDictionary {
	var <>upToDate = false;
	var setArgs;

	clear {
		super.clear;
		upToDate = false;
		this.changed(\clear);
	}

	updateArgs {
		setArgs = this.asKeyValuePairs.asOSCArgArray;
		upToDate = true;
	}

	get { |key|
		^this.at(key)
	}

	set { |...args|
		this.putPairs(args);
		upToDate = false;
		this.changed(\set, args);
	}

	unset { |... keys|
		keys.do { |x| this.put(x, nil) };
		this.changed(\unset, keys);
	}

	unmap { |... keys|
		keys.do { |x| this.put(x, nil) };
		this.changed(\unmap, keys);
	}

	asOSCArgArray {
		if(upToDate.not) { this.updateArgs };
		^setArgs
	}

	send { |server, nodeID, latency|
		var bundle = List.new;
		this.addToBundle(bundle, nodeID);
		server.listSendBundle(latency, bundle);
	}

	sendToNode { |node, latency|
		node = node.asTarget;
		this.send(node.server, node.nodeID, latency)
	}

	addToBundle { |bundle, target|
		var msg = this.setMsg(target);
		msg !? { bundle.add(this.setMsg(target)) }
	}

	addToEvent { |event|
		this.keysValuesDo { |key, val|
			event.put(key, val.asControlInput)
		}
	}

	setMsg { |target|
		if(this.isEmpty) { ^nil };
		if(upToDate.not) { this.updateArgs };
		^["/n_set", target.asControlInput] ++ setArgs
	}

	unmapArgsToBundle { |bundle, target, keys|
		var args;
		if(this.isEmpty) { ^this };
		keys = keys ?? { this.keys };
		keys.do { |key|
			var val = this.at(key);
			if(val.isNumber.not and: { val.isSequenceableCollection.not }) {
				args = args ++ [key, -1, val.numChannels]
			}
		};
		if(args.notNil) { bundle.add([48, target.asNodeID] ++ args) };
	}
}

ProxyNodeMap : NodeMap {
	var rates, <>proxy;

	clear {
		this.keys.do { |key|
			this.set(key, nil)
		};
		upToDate = false;
		this.changed(\clear);
	}

	put { |key, item|
		if(proxy.notNil) {
			if(this.at(key).hasProxyRelatives) {
				this.at(key).removeChild(proxy);
			};
			if(item.hasProxyRelatives) {
				item.addChild(proxy);
			}
		};
		super.put(key, item)
	}

	wakeUpParentsToBundle { | bundle, checkedAlready |
		this.pairsDo({ |key, item| item.wakeUpToBundle(bundle, checkedAlready) });
	}

	// map works only for proxies so far

	map { |...args|
		this.putPairs(args);
		upToDate = false;
		this.changed(\map, args);
	}

	mapEnvir { |... keys|
		if(keys.isEmpty) { keys = this.keys };
		this.map(currentEnvironment.getPairs(keys));
	}

	parents {
		var res = Array.new;
		this.do { |item|
			if(item.hasProxyRelatives) {
				res = res.add(item.asProxyParent)
			}
		};
		^res
	}

	controlNames {
		var res = Array.new;
		this.keysValuesDo { |key, value|
			var rate = if(value.respondsTo(\rate) and:{
				value.rate == \audio
			}) { \audio } { \control };
			res = res.add(ControlName(key, nil, rate, value))
		};
		^res
	}

	settingKeys {
		var res;
		this.keysValuesDo { |key, val|
			if(val.nodeMapMapsToControl.not) { res = res.add(key) }
		}
		^res
	}

	mappingKeys {
		var res;
		this.keysValuesDo { |key, val|
			if(val.nodeMapMapsToControl) { res = res.add(key) }
		}
		^res
	}

	changed { |...args|
		proxy.changed(*args)
	}



	hasRates { ^rates.notNil and: { rates.notEmpty } }
	setRates { |args|
		//this.deprecated(thisMethod); // maybe deprecate later
		rates = rates ?? { IdentityDictionary.new };
		rates.putPairs(args);
	}
	ratesFor { arg keys;
		^rates !? { rates.atAll(keys) }
	}
}


// represents a subset of the output bus of a proxy
// that can be mapped to a control

NodeProxyPartMap {
	var <proxy, <indices;

	*new { |proxy, indices=0|
		^this.newCopyArgs(proxy, indices)
	}

	asControlInput {
		var b = proxy.asControlInput;
		var busMaps = if(b.isString) { [b] } { b };
		^indices.asArray.collect { |i| busMaps.wrapAt(i) }
	}

	wakeUpToBundle { |bundle, checkedAlready|
		proxy.wakeUpToBundle(bundle, checkedAlready)
	}

	hasProxyRelatives {
		^true
	}

	asProxyParent {
		^proxy
	}

	removeChild { |obj|
		proxy.removeChild(obj)
	}

	addChild { |obj|
		proxy.addChild(obj)
	}

}


+ BusPlug {
	asOSCArgEmbeddedArray { |array|
		^this.asControlInput.asOSCArgEmbeddedArray(array)
	}
	asProxyParent {
		^this
	}
}

+ NodeProxy {
	partMap { |indices|
		^NodeProxyPartMap(this, indices)
	}
}

+ Object {
	wakeUpToBundle {}
	hasProxyRelatives { ^false }
}
