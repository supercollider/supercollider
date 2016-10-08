NodeProxy : BusPlug {

	var <group, <objects, <nodeMap, <children;
	var <loaded=false, <>awake=true, <paused=false;
	var <>clock, <>quant;
	classvar <>buildProxyControl, <>buildProxy;


	*new { | server, rate, numChannels, inputs |
		var res = super.new(server).init;
		if(rate.notNil or: { numChannels.notNil }) { res.initBus(rate, numChannels) };
		inputs.do { |o| res.add(o) };
		^res
	}


	init {
		nodeMap = ProxyNodeMap.new;
		objects = Order.new;
		loaded = false;
		reshaping = defaultReshaping;
		this.linkNodeMap;
	}

	clear { | fadeTime = 0 |
		this.free(fadeTime, true); 	// free group and objects
		this.removeAll; 			// take out all objects
		children = nil;             // for now: also remove children
		this.stop(fadeTime, true);		// stop any monitor
		monitor = nil;
		this.fadeTime = fadeTime; // set the fadeTime one last time for freeBus
		this.freeBus;	 // free the bus from the server allocator
		this.init;	// reset the environment
		this.changed(\clear, [fadeTime]);
	}

	end { | fadeTime, reset = false |
		var dt = fadeTime ? this.fadeTime;
		fork {
			this.free(dt, true);
			(dt + (server.latency ? 0)).wait;
			this.stop(0, reset);
		};
		this.changed(\end, [fadeTime, reset]);
	}

	isPlaying { ^group.isPlaying }

	free { | fadeTime, freeGroup = true |
		var bundle;
		var oldGroup = group;
		if(this.isPlaying) {
			bundle = MixedBundle.new;
			if(fadeTime.notNil) { bundle.add([15, group.nodeID, "fadeTime", fadeTime]) };
			this.stopAllToBundle(bundle, fadeTime);
			if(freeGroup) {
				oldGroup = group;
				group = nil;
				bundle.sched((fadeTime ? this.fadeTime) + (server.latency ? 0), { oldGroup.free });
			};
			bundle.send(server);
			this.changed(\free, [fadeTime, freeGroup]);
		}
	}

	release { | fadeTime |
		this.free(fadeTime, false)
	}

	pause {
		if(this.isPlaying) { objects.do { |item| item.pause(clock, quant) } };
		paused = true;
		this.changed(\pause);
	}

	resume {
		paused = false;
		if(this.isPlaying) { objects.do { |item| item.resume(clock, quant) } };
		this.changed(\resume);
	}

	fadeTime_ { | dur |
		if(dur.isNil) { this.unset(\fadeTime) } { this.set(\fadeTime, dur) };
	}

	fadeTime {
		^nodeMap.at(\fadeTime) ? 0.02;
	}

	asGroup { ^group.asGroup }
	asTarget { ^group.asGroup }
	asNodeID { ^group.asNodeID }
	nodeID { ^group.nodeID }

	parentGroup_ { | node |
		if(node.isPlaying.not) { "node not playing and registered: % \n".postf(node); ^this };
		parentGroup = node;
		if(group.isPlaying) { group.moveToHead(parentGroup) };
	}


	// setting the source

	source_ { | obj |
		this.put(nil, obj, 0)
	}

	prime { | obj |
		this.put(nil, obj, 0, nil, false);
	}

	sources_ { | list |
		this[0..] = list;
	}

	source { ^objects.at(0).source }
	sources { ^objects.array.collect(_.source) }

	add { | obj, channelOffset = 0, extraArgs, now = true |
		this.put(objects.pos, obj, channelOffset, extraArgs, now)
	}

	at { | index |
		^objects.at(index).source
	}

	put { | index, obj, channelOffset = 0, extraArgs, now = true |
		var container, bundle, oldBus = bus;

		if(obj.isNil) { this.removeAt(index); ^this };
		if(index.isSequenceableCollection) {
			^this.putAll(obj.asArray, index, channelOffset)
		};

		bundle = MixedBundle.new;
		container = obj.makeProxyControl(channelOffset, this);
		container.build(this, index ? 0); // bus allocation happens here


		if(this.shouldAddObject(container, index)) {
			// server sync happens here if necessary
			if(server.serverRunning) { container.loadToBundle(bundle, server) } { loaded = false; };
			this.prepareOtherObjects(bundle, index, oldBus.notNil and: { oldBus !== bus });
		} {
			format("failed to add % to node proxy: %", obj, this).inform;
			^this
		};

		this.putNewObject(bundle, index, container, extraArgs, now);
		this.changed(\source, [obj, index, channelOffset, extraArgs, now]);

	}

	prepareOtherObjects { |bundle, index, busChanged|
		var tempReshaping;
		if(index.isNil) {
			this.removeAllToBundle(bundle);
		} {
			this.removeToBundle(bundle, index);
		};
		if(busChanged) {
			tempReshaping = reshaping; reshaping = nil;
			this.rebuildDeepToBundle(bundle, true, nil, [this.fadeTime, quant, clock]);
			reshaping = tempReshaping;
		};
	}

	putNewObject { |bundle, index, container, extraArgs, now|
		objects = objects.put(index ? 0, container);
		if(server.serverRunning) {
			if(awake && now) {
				this.prepareToBundle(nil, bundle);
				container.wakeUpParentsToBundle(bundle);
				nodeMap.wakeUpParentsToBundle(bundle);
				this.sendObjectToBundle(bundle, container, extraArgs, index)
			};
			bundle.schedSend(server, clock ? TempoClock.default, quant);
		}
	}



	putAll { | list, index = (0), channelOffset = 0 |
		channelOffset = channelOffset.asArray;
		if(index.isSequenceableCollection) {
			max(list.size, index.size).do { |i|
				this.put(index.wrapAt(i), list.wrapAt(i), channelOffset.wrapAt(i))
			}
		}{
			list.do { |item, i| this.put(i + index, item, channelOffset.wrapAt(i)) }		}
	}

	putSeries { | first, second, last, value |
		last = last ?? { max(1, max(objects.size, value.size)) - 1 };
		this.putAll(value.asArray, (first, second..last))
	}

	filter { | i, func |
		this.put(i, \filter -> func)
	}

	removeFirst { | fadeTime | this.removeAt(objects.indices.first, fadeTime) }
	removeLast { | fadeTime | this.removeAt(objects.indices.last, fadeTime) }
	removeAll { | fadeTime | this.removeAt(nil, fadeTime) }
	removeAt { | index, fadeTime |
		var bundle = MixedBundle.new;
		if(index.isNil)
		{ this.removeAllToBundle(bundle, fadeTime) }
		{ this.removeToBundle(bundle, index, fadeTime) };
		this.changed(\source, [nil, index, fadeTime]);
		if(server.serverRunning) { bundle.schedSend(server) };
	}

	rebuild {
		var bundle = MixedBundle.new;
		this.rebuildDeepToBundle(bundle, false, nil, [this.fadeTime, quant, clock]);
		bundle.schedSend(server, clock ? TempoClock.default, quant);
	}

	mold { |numChannels, rate, argReshaping, fadeTime|
		var bundle, oldBus = bus, tempReshaping;
		fadeTime = fadeTime ?? { this.fadeTime };
		bundle = MixedBundle.new;
		tempReshaping = reshaping;
		if(numChannels.isNil and: { rate.isNil }) {
			// adjust to the source objects
			reshaping = argReshaping ? \expanding;
			this.rebuildDeepToBundle(bundle, false, nil, [fadeTime, quant, clock])
		} {
			// adjust to given shape
			reshaping = argReshaping ? \elastic;
			this.initBus(rate, numChannels);
			//  if necessary, rebuild without adjustment
			if(bus !== oldBus) {
				reshaping = nil;
				this.rebuildDeepToBundle(bundle, true, nil, [fadeTime, quant, clock])
			}
		};
		reshaping = tempReshaping;
		if(server.serverRunning) { bundle.schedSend(server, clock ? TempoClock.default, quant) };
	}

	lag { | ... args |
		nodeMap.setRates(args);
		this.rebuild;
	}

	setRates { | ... args |
		nodeMap.setRates(args);
		this.rebuild;
	}

	setBus { | argBus |
		super.setBus(argBus);
		this.linkNodeMap;
	}

	server_ { | inServer |
		this.deprecated(thisMethod);
		if(this.isNeutral.not) {
			this.end;
			loaded = false;
		};
		server = inServer;
	}

	group_ { | inGroup |
		var bundle;
		if(inGroup.server !== server) { Error("cannot move to another server").throw };
		NodeWatcher.register(inGroup.isPlaying_(true)); // assume it is playing
		if(this.isPlaying)
		{ 	bundle = MixedBundle.new;
			this.stopAllToBundle(bundle);
			group = inGroup;
			this.sendAllToBundle(bundle);
			bundle.schedSend(server, clock ? TempoClock.default, 0.0);
		} { group = inGroup };
	}

	read { | proxies |
		this.putAll(proxies)
	}

	readFromBus { | busses |
		var n, x;
		busses = busses.asCollection;
		n = this.numChannels;
		busses.do { arg item, i;
			x = min(item.numChannels ? n, n);
			this.put(i,
				SynthControl.new("system_link_" ++ this.rate ++ "_" ++ x),
				0,
				["in", item.index, "out", this.index]
			)
		};
	}


	// modifying context, setting controls

	set { | ... args | // pairs of keys or indices and value
		nodeMap.set(*args);
		if(this.isPlaying) {
			server.sendBundle(server.latency, [15, group.nodeID] ++ args.asOSCArgArray);
		}
	}

	setn { | ... args |
		this.set(*args)
	}

	setGroup { | args, useLatency = false |
		if(this.isPlaying) {
			server.sendBundle(if(useLatency) { server.latency },
				[15, group.nodeID] ++ args.asOSCArgArray
			);
		}
	}

	map { | ... args | // key(s), proxy, key(s), proxy ...
		this.set(*args)
	}

	mapn { | ... args |
		"NodeProxy: mapn is deprecated, please use map instead".postln;
		this.map(*args)
	}

	xset { | ... args |
		this.xFadePerform(\set, args)
	}
	xmap { | ... args |
		this.xFadePerform(\map, args)
	}
	xsetn { | ... args |
		this.xFadePerform(\set, args)
	}
	xmapn { | ... args |
		"NodeProxy: xmapn is decrepated, please use xmap instead".postln;
		this.xFadePerform(\map, args)
	}
	xunset { | ... args |
		this.xFadePerform(\unset, args)
	}

	xFadePerform { | selector, args |
		var bundle;
		if(this.isPlaying) {
			nodeMap.performList(selector, args);
			this.sendEach(nil, true)
		} {
			this.performList(selector, args)
		}
	}

	mapEnvir { | ... keys | // map to current environment
		nodeMap.mapEnvir(*keys);
		if(this.isPlaying) {
			nodeMap.sendToNode(group);
		};
	}

	unset { | ... keys |
		var bundle = List.new;
		this.unsetToBundle(bundle, keys);
		if(bundle.notEmpty) {
			server.listSendBundle(server.latency, bundle)
		};
	}

	unmap { | ... keys |
		var bundle;
		if(keys.isEmpty) { keys = nodeMap.mappingKeys; if(keys.isEmpty) { ^this } };
		if(this.isPlaying) {
			bundle = List.new;
			nodeMap.unmapArgsToBundle(bundle, group.nodeID, keys);
			if(bundle.notEmpty) { server.listSendBundle(server.latency, bundle) };
		};
		nodeMap.unmap(*keys);
	}

	nodeMap_ { | map |
		this.setNodeMap(map, false)
	}

	setNodeMap { | map, xfade = true |
		var bundle;
		if(map.isNil) { ^this.unmap };
		map.set(\fadeTime, this.fadeTime); // keep old fadeTime
		nodeMap.clear; nodeMap = map; this.linkNodeMap;
		if(this.isPlaying) {
			bundle = MixedBundle.new;
			if(xfade) { this.sendEach(nil,true) }
			{
				this.unsetToBundle(bundle); // unmap old
				nodeMap.addToBundle(bundle, group); // map new
				bundle.schedSend(server, clock ? TempoClock.default, quant);
			}
		};
	}

	// play proxy as source of receiver
	<-- { | proxy |
		var bundle = MixedBundle.new, fadeTime = this.fadeTime;
		this.source = proxy;

		if(proxy.monitor.isPlaying) {
			proxy.stop(fadeTime: fadeTime);
			if(monitor.isPlaying.not) {
				this.playToBundle(bundle, fadeTime: fadeTime)
			}
		};
		bundle.add(proxy.moveBeforeMsg(this));
		bundle.schedSend(server);
	}

	// map receiver to proxy input
	// second argument is an adverb
	<>> { | proxy, key = \in |
		proxy.perform('<<>', this, key);
		^proxy
	}

	// map proxy to receiver input
	// second argument is an adverb
	<<> { | proxy, key = \in |
		var ctl, rate, numChannels, canBeMapped;
		if(proxy.isNil) { ^this.unmap(key) };
		ctl = this.controlNames.detect { |x| x.name == key };
		rate = ctl.rate ?? {
			if(proxy.isNeutral) {
				if(this.isNeutral) { \audio } { this.rate }
			} {
				proxy.rate
			}
		};
		numChannels = ctl !? { ctl.defaultValue.asArray.size };
		canBeMapped = proxy.initBus(rate, numChannels); // warning: proxy should still have a fixed bus
		if(canBeMapped) {
			if(this.isNeutral) { this.defineBus(rate, numChannels) };
			this.xmap(key, proxy);
		} {
			"Could not link node proxies, no matching input found.".warn
		};
		^proxy // returns first argument for further chaining
	}


	// starting processes

	spawn { | extraArgs, index = 0 |
		var bundle, obj, i;
		if(server.serverRunning.not) { "server '%' not running\n".postf(server); ^this };
		obj = objects.at(index);
		if(obj.notNil) {
			i = this.index;
			bundle = this.getBundle;
			if(loaded.not) { this.loadToBundle(bundle) };
			obj.spawnToBundle(bundle, { nodeMap.asOSCArgArray ++ extraArgs.value.asOSCArgArray }, this);
			bundle.schedSend(server);
		}
	}


	send { | extraArgs, index, freeLast = true |
		var bundle, obj, fadeTime = this.fadeTime;
		if(objects.isEmpty) { ^this };
		if(server.serverRunning.not) { "server '%' not running\n".postf(server); ^this };
		if(index.isNil) {
			bundle = this.getBundle;
			if(freeLast) { this.stopAllToBundle(bundle, fadeTime) };
			if(loaded.not) { this.loadToBundle(bundle) };
			this.sendAllToBundle(bundle, extraArgs);
			bundle.schedSend(server);

		} {
			obj = objects.at(index);
			if(obj.notNil) {
				bundle = this.getBundle;
				if(freeLast) { obj.stopToBundle(bundle, fadeTime) };
				if(loaded.not) { this.loadToBundle(bundle) };
				this.sendObjectToBundle(bundle, obj, extraArgs, index);
				bundle.schedSend(server);
			}
		}
	}

	sendAll { | extraArgs, freeLast = true |
		this.send(extraArgs, nil, freeLast);
	}

	sendEach { | extraArgs, freeLast = true |
		var bundle;
		if(server.serverRunning.not) { "server '%' not running\n".postf(server); ^this };
		bundle = this.getBundle;
		if(freeLast, { this.stopAllToBundle(bundle) });
		if(loaded.not) { this.loadToBundle(bundle) };
		this.sendEachToBundle(bundle, extraArgs);
		bundle.schedSend(server)


	}

	quantize { | ... proxies |
		var quant = this.quant ? 1.0;
		([this]++proxies).do { |x|
			x.quant = quant;
			x.send;
		}
	}

	wakeUp { 	// do not touch internal state if already playing
		if(this.isPlaying.not) { this.deepWakeUp }
	}

	deepWakeUp {
		var bundle = MixedBundle.new;
		if(server.serverRunning.not) { "server '%' not running\n".postf(server); ^this };
		this.wakeUpToBundle(bundle);
		bundle.schedSend(server, clock ? TempoClock.default, quant)
	}

	// making copies

	copy {
		^this.class.new(server).copyState(this)
	}

	copyState { |proxy|

		super.copyState(proxy);

		proxy.objects.keysValuesDo { |key, val| objects[key] = val.copy };
		this.nodeMap = proxy.nodeMap.copy;

		loaded = false;
		awake = proxy.awake; paused = proxy.paused;
		clock = proxy.clock; quant = proxy.quant;

	}

	// gui support

	typeStr {
		if(this.rate === 'audio') { ^"ar" + this.numChannels };
		if(this.rate === 'control') { ^"kr" + this.numChannels };
		^"ir";
	}

	edit { | nSliders, parent, bounds |
		^NdefGui(this, nSliders ? this.getKeysValues.size.max(5), parent, bounds);
	}

	trace { |index = 0|
		var obj = objects[index];
		if(obj.nodeID.notNil) {
			server.sendMsg(\n_trace, obj.nodeID);
		} {
			"Cannot trace a% %".format(
				if(obj.class.name.asString.first.isVowel) { "n" } { "" },
				obj.class.name
			).warn;
		};
	}



	// interproxy structure and reading other busses

	orderNodes { | ... proxies |
		var msg = this.moveBeforeMsg(*proxies);
		msg !? {
			server.sendBundle(nil, msg)
		}
	}

	getFamily { | set, alreadyAsked |
		var parents;
		parents = IdentitySet.new;
		alreadyAsked = alreadyAsked ?? { IdentitySet.new };
		if(alreadyAsked.includes(this).not) {
			alreadyAsked.add(this);
			objects.do { arg obj; parents.addAll(obj.parents) };
			parents.addAll(nodeMap.parents);
			parents.do { arg proxy; proxy.getFamily(parents, alreadyAsked) };
			set.add(this);
			set.addAll(parents);
		};
		^set
	}

	getStructure { | alreadyAsked |
		var parents, substructure;
		parents = List.new;
		alreadyAsked = alreadyAsked ?? { IdentitySet.new };
		if(alreadyAsked.includes(this).not) {
			alreadyAsked.add(this);
			objects.do { arg obj; parents.addAll(obj.parents) };
			parents.addAll(nodeMap.parents);
			substructure = parents.collect { arg proxy; proxy.getStructure(alreadyAsked) };
			^[this, substructure.flatten(1)];
		};
		^nil
	}

	moveBeforeMsg { | ... proxies |
		var list;
		([this] ++ proxies).do { |el|
			if(el.isPlaying) {
				list = list.add(el.group);
				if(el.monitor.isPlaying) {
					list = list.add(el.monitor.group) // debatable. maybe check whether special
				}
			}
		};
		^list !? { Node.orderNodesMsg(list) }
	}




	// node map settings

	internalKeys {
		^#[\out, \i_out, \gate, \fadeTime];
	}

	// return names in the order they have in .objects
	controlNames { | except, addNodeMap = true |
		var all = Array.new; // Set doesn't work, because equality undefined for ControlName
		except = except ? this.internalKeys;
		objects.do { |el|
			el.controlNames.do { |item|
				if(except.includes(item.name).not and: {
					all.every { |cn| cn.name !== item.name }
				}) {
					all = all.add(item);
				}
			};
		};
		^if (addNodeMap.not or: nodeMap.isNil) { all } {
			this.addNodeMapControlNames(all, except)
		};
	}

	// if a name is set in nodemap, overwrite the values in objCtlNames;
	// if a key is set in the nodemap, but is not used in the objects yet, add at the end.
	addNodeMapControlNames { |objCtlNames, except = #[]|
		nodeMap.controlNames
		.reject { |ctlname| except.includes(ctlname.name) }
		.do { |mapCtl|
			var index = objCtlNames.detectIndex { |objCtl| objCtl.name == mapCtl.name };
			if (index.notNil) {
				objCtlNames.put(index, mapCtl)
			} {
				objCtlNames = objCtlNames.add(mapCtl)
			}
		};
		^objCtlNames
	}

	resetNodeMap {
		this.nodeMap = ProxyNodeMap.new;
		this.linkNodeMap;
	}

	cleanNodeMap {
		var nodeMapSettingKeys, nodeMapMappingKeys, keysToUnset, keysToUnmap, currentKeys;
		if (nodeMap.isNil) { ^this };

		nodeMapSettingKeys = difference(nodeMap.settingKeys ? [], this.internalKeys);
		nodeMapMappingKeys = difference(nodeMap.mappingKeys ? [], this.internalKeys);
		currentKeys = this.controlNames(addNodeMap: false).collect(_.name);
		keysToUnset = difference(nodeMapSettingKeys, currentKeys);
		keysToUnmap = difference(nodeMapMappingKeys, currentKeys);

		keysToUnset.do(this.unset(_));
		keysToUnmap.do(this.unmap(_));
	}

	controlKeys { | except, noInternalKeys = true |
		var list = Array.new;
		if (noInternalKeys) { except = except ++ this.internalKeys; };
		this.controlNames.do { |el, i|
			if(except.includes(el.name).not)
			{ list = list.add(el.name) }
		}
		^list
	}

	getKeysValues { | keys, except, withDefaults = true, noInternalKeys = true |
		var pairs, result = [], myKeys, defaults;
		if (noInternalKeys) { except = except ++ this.internalKeys; };

		pairs = this.controlKeysValues(keys, except).clump(2);
		#myKeys, defaults = pairs.flop;

		myKeys.collect { |key, i|
			var val, doAdd;
			val = nodeMap[key];
			doAdd = withDefaults or: val.notNil;
			if (doAdd) {
				result = result.add([ key, (val ? defaults[i]).value ]);
			};
		}
		^result
	}

	// controlPairs, gets default values
	controlKeysValues { | keys, except |
		var list, fullList;
		except = except ? this.internalKeys;
		fullList = this.controlNames(except);
		if(keys.isNil or: { keys.isEmpty }) {
			list = Array(fullList.size * 2);
			fullList.do { |cn| list.add(cn.name).add(cn.defaultValue) }
		} {
			list = Array(keys.size * 2);
			keys.do { |key|
				var val = fullList.detect { |cn| cn.name == key };
				val = if(val.isNil) { 0 } { val.defaultValue };
				list.add(key).add(val)
			}
		}
		^list;
	}

	// derive names and default args from synthDefs
	supplementNodeMap { | keys, replaceOldKeys=false |
		this.controlNames.do { |el|
			var key = el.name;
			if (
				( replaceOldKeys or: { nodeMap.at(key).isNil } )
				and:
				{ keys.isNil or: { keys.includes(key) } }
			) { nodeMap.set(key, el.defaultValue) }
		}
	}




	// bundling messages

	getBundle {
		var bundle = MixedBundle.new;
		this.prepareToBundle(nil, bundle);
		^bundle
	}

	// start group if necessary
	prepareToBundle { | argGroup, bundle, addAction = \addToTail |
		if(this.isPlaying.not) {
			group = Group.basicNew(server, this.defaultGroupID);
			NodeWatcher.register(group);
			group.isPlaying = server.serverRunning;
			if(argGroup.isNil and: { parentGroup.isPlaying }) { argGroup = parentGroup };
			bundle.addPrepare(group.newMsg(argGroup ?? { server.asGroup }, addAction));
		}
	}

	// bundle: apply the node map settings to the entire group
	sendAllToBundle { | bundle, extraArgs |
		var args = Thunk({ nodeMap.asOSCArgArray ++ extraArgs.value.asOSCArgArray });
		objects.do { arg item;
			item.playToBundle(bundle, args, this);
		}
	}

	// bundle: apply the node map settings to each synth separately
	sendEachToBundle { | bundle, extraArgs |
		objects.do { arg item;
			this.sendObjectToBundle(bundle, item, extraArgs)
		}
	}

	// bundle: send single object
	sendObjectToBundle { | bundle, object, extraArgs, index |
		var target, nodes;
		var synthID = object.playToBundle(bundle, { nodeMap.asOSCArgArray ++ extraArgs.value.asOSCArgArray }, this);
		if(synthID.notNil) {
			if(index.notNil and: { objects.size > 1 }) { // if nil, all are sent anyway
				// make list of nodeIDs following the index
				nodes = Array(4);
				objects.doRange({ arg obj;
					var id = obj.nodeID;
					if(id.notNil and: { id != synthID })
					{ nodes = nodes ++ id ++ synthID };
				}, index + 1);
				if(nodes.size > 0) { bundle.add(["/n_before"] ++ nodes.reverse) };
			};
		}
	}

	// bundle: remove single object
	removeToBundle { | bundle, index, fadeTime |
		var obj, dt, playing;
		playing = this.isPlaying;
		obj = objects.removeAt(index);

		if(obj.notNil) {
			dt = fadeTime ? this.fadeTime;
			if(playing) { obj.stopToBundle(bundle, dt) };
			obj.freeToBundle(bundle, this);
		}
	}

	// bundle: remove all objects
	removeAllToBundle { | bundle, fadeTime |
		var dt, playing;
		dt = fadeTime ? this.fadeTime;
		playing = this.isPlaying;
		objects.do { arg obj;
			if(playing) { obj.stopToBundle(bundle, dt) };
			obj.freeToBundle(bundle, this);
		};
		objects.makeEmpty;
	}

	// bundle: stop single process
	stopAllToBundle { | bundle, fadeTime |
		var obj, dt;
		dt = fadeTime ? this.fadeTime;
		if(this.isPlaying) {
			objects.do { |obj| obj.stopToBundle(bundle, dt) }
		}
	}

	loadToBundle { | bundle |
		this.reallocBusIfNeeded;
		objects.do { arg item, i;
			item.loadToBundle(bundle, server);
		};
		loaded = true;
	}

	unsetToBundle { | bundle, keys |
		var pairs = this.controlKeysValues(keys);
		if(this.isPlaying) {
			if(pairs.notEmpty) {
				bundle.add([15, group.nodeID] ++ pairs)
			}
		};
		nodeMap.unset(*pairs[0,2..])
	}

	wakeUpToBundle { | bundle, checkedAlready |
		if(checkedAlready.isNil) { checkedAlready = IdentitySet.new };
		if(checkedAlready.includes(this).not) {
			checkedAlready.add(this);
			this.wakeUpParentsToBundle(bundle, checkedAlready);
			if(loaded.not) { this.loadToBundle(bundle) };
			if(awake and: { this.isPlaying.not }) {
				this.prepareToBundle(nil, bundle);
				this.sendAllToBundle(bundle);
			};
		};

	}

	wakeUpParentsToBundle { | bundle, checkedAlready |
		nodeMap.wakeUpParentsToBundle(bundle, checkedAlready);
		objects.do { arg item; item.wakeUpParentsToBundle(bundle, checkedAlready) };
	}

	rebuildDeepToBundle { |bundle, busWasChangedExternally = true, checkedAlready, timeArgs|
		var oldBus = bus;
		if(checkedAlready.isNil or: { checkedAlready.includes(this).not }) {
			this.rebuildToBundle(bundle, timeArgs);
			if(busWasChangedExternally or: { bus !== oldBus }) {
				//"%: rebuilding children\n".postf(this);
				if(checkedAlready.isNil) { checkedAlready = IdentitySet.new };
				checkedAlready.add(this);
				children.do { |item|
					item.rebuildDeepToBundle(bundle, false, checkedAlready, timeArgs)
				};
				if(monitor.isPlaying) {
					//postf("in % restarting monitor\n", this);
					monitor.playNBusToBundle(bundle, bus: bus, fadeTime: timeArgs[0]);
				}
			}
		}
	}

	rebuildToBundle { |bundle, timeArgs|
		// we need to pass and temporarily set the timing information
		// so that freeBus has the correct fadeTime.
		var prevTimeArgs = [this.fadeTime, quant, clock];
		timeArgs !? { this.fadeTime = timeArgs[0]; quant = timeArgs[1]; clock = timeArgs[2] };
		loaded = false;
		nodeMap.upToDate = false; // if mapped to itself
		//"rebuilding proxy: % (% channels, % rate)\n".postf(this, this.numChannels, this.rate);
		if(this.isPlaying) {
			this.stopAllToBundle(bundle);
			objects.do { |item| item.freeToBundle(bundle, this) };
			objects.do { |item, i| item.build(this, i) };
			this.loadToBundle(bundle);
			this.sendAllToBundle(bundle);
		} {
			objects.do { |item| item.freeToBundle(bundle, this) };
			objects.do { |item, i| item.build(this, i) };
		};
		timeArgs !? { this.fadeTime = prevTimeArgs[0]; quant = prevTimeArgs[1]; clock = prevTimeArgs[2] };
		this.changed(\rebuild);
	}


	// allocation

	freeBus {
		var oldBus = bus, c;
		if(oldBus.isNil) { ^this };
		if(this.isPlaying) {
			c = (clock ? TempoClock.default);
			c.sched(server.latency ? 0.01 + quant.nextTimeOnGrid(c) + this.fadeTime, { oldBus.free(true); nil });
			CmdPeriod.doOnce { if(oldBus.index.notNil) { oldBus.free(true) } };
		} {
			oldBus.free(true)
		};
		busArg = bus = nil;
		busLoaded = false;
	}

	reallocBusIfNeeded { // bus is reallocated only if the server was not booted on creation.
		if(busLoaded.not and: { bus.notNil }) {
			bus.realloc;
			this.linkNodeMap
		}
	}

	serverQuit {
		busLoaded = false;
		loaded = false;
	}


	// network support

	shouldAddObject { | obj | ^obj.readyForPlay } // shared node proxy overrides this

	defaultGroupID { ^server.nextNodeID }




	// private implementation

	linkNodeMap {
		var index = this.index;
		if(index.notNil) { nodeMap.set(\out, index, \i_out, index) };
		nodeMap.proxy = this;
	}

	generateUniqueName {
		var uniqueName = this.identityHash;
		// if named, add the name so we see it
		// in synthdef names of the server's nodes.
		if(this.key.notNil, {
			uniqueName = this.key ++ uniqueName;
		});
		^server.clientID.asString ++ uniqueName ++ "_";
	}

	prepareOutput {
		var parentPlaying;
		parentPlaying = this.addToChild;
		if(parentPlaying) { this.deepWakeUp };
	}

	addToChild {
		var childProxyControl = buildProxyControl;
		if(childProxyControl.notNil) {
			childProxyControl.addParent(this);
		};
		^childProxyControl.isPlaying;
	}

	addChild { |proxy|
		children = children.add(proxy);
		//"%: added child: % in %\n".postf(this, proxy, children);
	}

	removeChild { |proxy|
		children.remove(proxy);
		//"%: removed child: % in %\n".postf(this, proxy, children);
	}

	// renames synthdef so one can use it in patterns
	nameDef { |name, index = 0|
		var func = objects[index].synthDef.func;
		name = name ?? {
			"New SynthDef name: ".post;
			(this.key ++ "_" ++ index).asSymbol.postcs;
		};
		^SynthDef(name, func);
	}
}



Ndef : NodeProxy {

	classvar <>defaultServer, <>all;
	var <>key;

	*initClass { all = () }

	*new { | key, object |
		// key may be simply a symbol, or an association of a symbol and a server name
		var res, server, dict;

		if(key.isKindOf(Association)) {
			server = Server.named.at(key.value);
			if(server.isNil) {
				Error("Ndef(%): no server found with this name.".format(key)).throw
			};
			key = key.key;
		} {
			server = defaultServer ? Server.default;
		};

		dict = this.dictFor(server);
		res = dict.envir.at(key);
		if(res.isNil) {
			res = super.new(server).key_(key);
			dict.initProxy(res);
			dict.envir.put(key, res)
		};

		object !? { res.source = object };
		^res;
	}

	*ar { | key, numChannels, offset = 0 |
		^this.new(key).ar(numChannels, offset)
	}

	*kr { | key, numChannels, offset = 0 |
		^this.new(key).kr(numChannels, offset)
	}

	*clear { | fadeTime |
		all.do(_.clear(fadeTime));
		all.clear;
	}

	*dictFor { | server |
		var dict = all.at(server.name);
		if(dict.isNil) {
			dict = ProxySpace.new(server); // use a proxyspace for ease of access.
			all.put(server.name, dict);
			dict.registerServer;
		};
		^dict
	}

	copy { |toKey|
		if(key == toKey) { Error("cannot copy to identical key").throw };
		^this.class.new(toKey).copyState(this)
	}

	proxyspace {
		^this.class.dictFor(this.server)
	}

	storeOn { | stream |
		this.printOn(stream);
	}
	printOn { | stream |
		var serverString = if (server == Server.default) { "" } {
			" ->" + server.name.asCompileString;
		};
		stream << this.class.name << "(" <<< this.key << serverString << ")"
	}

}
