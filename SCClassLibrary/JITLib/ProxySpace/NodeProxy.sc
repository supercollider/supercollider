NodeProxy : BusPlug {

	var <group, <objects, <nodeMap;
	var <loaded=false, <>awake=true, <paused=false;
	var <>clock, <>quant;
	classvar <>buildProxyControl;


	*new { | server, rate, numChannels, inputs |
		var res = super.new(server).init;
		res.initBus(rate, numChannels);
		inputs.do { |o| res.add(o) };
		^res
	}

	init {
		nodeMap = ProxyNodeMap.new;
		objects = Order.new;
		loaded = false;
	}

	clear { | fadeTime = 0 |
		this.free(fadeTime, true); 	// free group and objects
		this.removeAll; 			// take out all objects
		this.stop(fadeTime, true);		// stop any monitor
		monitor = nil;
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
		^nodeMap.at(\fadeTime).value ? 0.02;
	}

	prFadeTime { ^nodeMap.at(\fadeTime).value }

	asGroup { ^group.asGroup }
	asTarget { ^group.asGroup }
	asNodeID { ^group.asNodeID }

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

	put { | index, obj, channelOffset = 0, extraArgs, now = true |		var container, bundle, orderIndex;
		if(obj.isNil) { this.removeAt(index); ^this };
		if(index.isSequenceableCollection) {
			^this.putAll(obj.asArray, index, channelOffset)
		};

		orderIndex = index ? 0;
		container = obj.makeProxyControl(channelOffset, this);
		container.build(this, orderIndex); // bus allocation happens here

		if(this.shouldAddObject(container, index)) {
			bundle = MixedBundle.new;
			if(index.isNil)
				{ this.removeAllToBundle(bundle) }
				{ this.removeToBundle(bundle, index) };
			objects = objects.put(orderIndex, container);
			this.changed(\source, [obj, index, channelOffset, extraArgs, now]);
		} {
			format("failed to add % to node proxy: %", obj, this).inform;
			^this
		};

		if(server.serverRunning) {
			now = awake && now;
			if(now) {
				this.prepareToBundle(nil, bundle);
			};
			container.loadToBundle(bundle, server);
			loaded = true;
			if(now) {
				container.wakeUpParentsToBundle(bundle);
				this.sendObjectToBundle(bundle, container, extraArgs, index);
			};
			nodeMap.wakeUpParentsToBundle(bundle);
			bundle.schedSend(server, clock ? TempoClock.default, quant);
		} {
			loaded = false;
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
		bundle.schedSend(server);
	}

	rebuild {
		var bundle;
		if(this.isPlaying) {
			bundle = MixedBundle.new;
			this.stopAllToBundle(bundle);
			bundle.schedSend(server, clock ? TempoClock.default, quant);
			bundle = MixedBundle.new;
			loaded = false;
			this.loadToBundle(bundle);
			this.sendAllToBundle(bundle);
			bundle.schedSend(server, clock ? TempoClock.default, quant);
		} {
			loaded = false;
		};

	}

	lag { | ... args |
		nodeMap.setRates(args);
		this.rebuild;
	}

	setRates { | ... args |
		nodeMap.setRates(args);
		this.rebuild;
	}

	server_ { | inServer |
		if(this.isNeutral.not) {
			this.end;
			loaded = false;
		};
		server = inServer;
	}

	bus_ { | inBus |
		var oldBus = bus;
		if(server != inBus.server) { Error("can't change the server").throw };
		super.bus_(inBus);
		this.linkNodeMap;
		if(oldBus.notNil) { this.rebuild };
	}

	group_ { | inGroup |
		var bundle;
		if(inGroup.server !== server, { Error("cannot move to another server").throw });
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
		proxies = proxies.asCollection;
		proxies.do { arg item; item.wakeUp };
		this.readFromBus(proxies)
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
			server.sendBundle(server.latency, [15, group.nodeID] ++ args);
		}
	}

	setn { | ... args |
		nodeMap.set(*args);
		if(this.isPlaying) {
			server.sendBundle(server.latency, group.setnMsg(*args));
		}
	}

	setGroup { | args, useLatency = false |
		if(this.isPlaying) {
			server.sendBundle(if(useLatency) { server.latency }, [15, group.nodeID] ++ args);
		};
	}

	map { | ... args | // key(s), proxy, key(s), proxy ...
		var bundle;
		if(this.isPlaying) {
			bundle = List.new;
			nodeMap.unmapArgsToBundle(bundle, group.nodeID, args[0,2..args.size-2]);
			nodeMap.map(*args).updateBundle;
			nodeMap.addToBundle(bundle, group.nodeID);
			server.listSendBundle(server.latency, bundle);
		} { nodeMap.map(*args) };
	}

	mapn { | ... args |
		"NodeProxy: mapn is deprecated, please use map instead".postln;
		^this.map(*args)
	}

	xset { | ... args |
		this.xFadePerform(\set, args)
	}
	xmap { | ... args |
		this.xFadePerform(\map, args)
	}
	xsetn { | ... args |
		this.xFadePerform(\setn, args)
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
		if(this.isPlaying)
		{
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
		var bundle, old, fadeTime;
		map.set(\fadeTime, this.fadeTime); // keep old fadeTime
		bundle = MixedBundle.new;
		old = nodeMap;
		nodeMap = map;
		old.clear;
		this.linkNodeMap;
		this.nodeMapChanged;
		if(this.isPlaying) {
			if(xfade) { this.sendEach(nil,true) }
			{
			this.unsetToBundle(bundle); // unmap old
			nodeMap.addToBundle(bundle, group); // map new
			bundle.schedSend(server, clock ? TempoClock.default, quant);
			}
		};
	}

	nodeMapChanged {
		var set, map;
		nodeMap.settings.do { |setting|
			if(setting.isMapped) {
				map = map.add(setting.key).add(setting.value)
			} {
				set = set.add(setting.key).add(setting.value)
			}
		};
		if(set.notNil) { this.changed(\set, set) };
		if(map.notNil) { this.changed(\map, map) };
	}


	// play proxy as source of receiver
	<-- { | proxy |
		var bundle = MixedBundle.new;
		this.source = proxy;

		if(proxy.monitorGroup.isPlaying) {
			proxy.stop(fadeTime: 0.5);
			if(this.monitorGroup.isPlaying.not) {
				this.playToBundle(bundle, fadeTime:0.1)
			}
		};
		bundle.add(proxy.moveBeforeMsg(this));
		bundle.send(server, server.latency);
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
		numChannels = ctl !? { ctl.defaultValue.asArray.size };		canBeMapped = proxy.initBus(rate, numChannels);
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
		obj = objects.at(index);
		if(obj.notNil) {
			i = this.index;
			bundle = this.getBundle;
			obj.spawnToBundle(bundle, extraArgs, this);
			nodeMap.addToBundle(bundle, -1);
			bundle.schedSend(server);
		}
	}


	send { | extraArgs, index, freeLast = true |
		var bundle, obj;
		if(objects.isEmpty) { ^this };
		if(index.isNil) {
			bundle = this.getBundle;
			if(freeLast) { this.stopAllToBundle(bundle) };
			this.sendAllToBundle(bundle, extraArgs);
			bundle.schedSend(server);

		} {
			obj = objects.at(index);
			if(obj.notNil) {
				bundle = this.getBundle;
				if(freeLast) { obj.stopToBundle(bundle) };

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
		bundle = this.getBundle;
		if(freeLast, { this.stopAllToBundle(bundle) });
		this.sendEachToBundle(bundle, extraArgs);
		bundle.schedSend(server);

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
		this.wakeUpToBundle(bundle);
		bundle.schedSend(server, clock ? TempoClock.default, quant)
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
		var pairs, result = [], myKeys, defaults, mapSettings;
		if (noInternalKeys) { except = except ++ this.internalKeys; };

		pairs = this.controlKeysValues(keys, except).clump(2);
		#myKeys, defaults = pairs.flop;

		mapSettings = nodeMap.settings;
		myKeys.collect { |key, i|
			var val, doAdd;
			val = mapSettings[key];
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
		var bundle;
		bundle = 	MixedBundle.new;
		this.prepareToBundle(nil, bundle);
		^bundle
	}

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
		objects.do { arg item;
			item.playToBundle(bundle, extraArgs.value, this);
		};
		if(objects.notEmpty) { nodeMap.addToBundle(bundle, group) };
	}

	// bundle: apply the node map settings to each synth separately
	sendEachToBundle { | bundle, extraArgs |
		objects.do { arg item;
			this.sendObjectToBundle(bundle, item, extraArgs.value)
		}
	}

	// bundle: send single object
	sendObjectToBundle { | bundle, object, extraArgs, index |
		var synthID, target, nodes;
		synthID = object.playToBundle(bundle, extraArgs.value, this);
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
		nodeMap.addToBundle(bundle, synthID)
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
				obj.freeToBundle(bundle, dt);
		}
	}
	// bundle: remove all objects
	removeAllToBundle { | bundle, fadeTime |
		var dt, playing;
		dt = fadeTime ? this.fadeTime;
		playing = this.isPlaying;
		objects.do { arg obj;
				if(playing) { obj.stopToBundle(bundle, dt) };
				obj.freeToBundle(bundle, dt);
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
			item.build(this, i);
			item.loadToBundle(bundle, server);
		};
		loaded = true;
	}

	unsetToBundle { | bundle, keys |
		var pairs = this.controlKeysValues(keys);
		if(this.isPlaying) {
			if(pairs.notEmpty) {
				bundle.add([15, group.nodeID] ++ pairs);
			};
		};
		nodeMap.unset(*pairs[0,2..]);
	}

	wakeUpToBundle { | bundle, checkedAlready |
		if(checkedAlready.isNil) { checkedAlready = IdentitySet.new };
		if(checkedAlready.includes(this).not) {
			checkedAlready.add(this);
			this.wakeUpParentsToBundle(bundle, checkedAlready);
			if(loaded.not) { this.loadToBundle(bundle) };
			if(awake and: { this.isPlaying.not }) {
				this.prepareToBundle(nil, bundle, \addToHead);
				this.sendAllToBundle(bundle);
			};
		};

	}

	wakeUpParentsToBundle { | bundle, checkedAlready |
			nodeMap.wakeUpParentsToBundle(bundle, checkedAlready);
			objects.do { arg item; item.wakeUpParentsToBundle(bundle, checkedAlready) };
	}






	// allocation

	defineBus { | rate = \audio, numChannels |
		super.defineBus(rate, numChannels);
		this.linkNodeMap;
	}

	reallocBusIfNeeded { // bus is reallocated only if the server was not booted on creation.
		if(busLoaded.not and: { bus.notNil }) {
			bus.realloc;
			this.linkNodeMap
		}
	}






	// network support

	shouldAddObject { | obj | ^obj.readyForPlay } // shared node proxy overrides this

	defaultGroupID { ^server.nextNodeID }






	// private implementation

	linkNodeMap {
		var index;
		index = this.index;
		if(index.notNil) { nodeMap.set(\out, index, \i_out, index) };
		nodeMap.proxy = this;
	}

	generateUniqueName {
			// if named, give us the name so we see it
			// in synthdef names of the server's nodes.
		var key = this.key ?? this.identityHash.abs;
		^server.clientID.asString ++ key ++ "_";
	}

	prepareOutput {
		var parentPlaying;
		parentPlaying = this.addToChild;
		if(parentPlaying) { this.deepWakeUp };
	}

	addToChild {
		var child;
		child = buildProxyControl;
		if(child.notNil) { child.addParent(this) };
		^child.isPlaying;
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
			server = Server.default;
		};

		dict = this.dictFor(server);
		res = dict.envir.at(key);
		if(res.isNil) {
			res = super.new(server).key_(key);
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
