// a listener on a bus

BusPlug : AbstractFunction {
	
	var <server, <bus; 		
	var <monitor;
	var <busArg = ""; // cache for "/s_new" bus arg
	classvar <>defaultNumAudio=2, <>defaultNumControl=1;
	
	
	*new { arg server;
		^super.newCopyArgs(server ? Server.default);
	}
	
	*for { arg bus;
		bus = bus.asBus;
		^this.new(bus.server).bus_(bus)
	}
	
	*audio { arg server, numChannels;
		^this.new(server).defineBus(\audio, numChannels);
	}
	
	*control { arg server, numChannels;
		^this.new(server).defineBus(\control, numChannels);
	}
	
	*initClass {
		2.do { arg i; i = i + 1;
			SynthDef("system_link_audio_" ++ i, { arg out=0, in=16, vol=1;
				var env;
				env = EnvGate.new * vol;
				Out.ar(out, InFeedback.ar(in, i) * env) 
			}, [\kr, \ir]).writeDefFile;
		};
		2.do { arg i; i = i + 1;
			SynthDef("system_link_control_" ++ i, { arg out=0, in=16;
				var env;
				env = EnvGate.new;
				Out.kr(out, In.kr(in, i) * env) 
			}, [\kr, \ir]).writeDefFile;
		}
	}
	
	
	clear { 
		this.free;
		this.stop;
		this.freeBus;
	}
		
	
	////////  bus definitions  //////////////////////////////////////////
	
	
	
	bus_ { arg b; 
		this.freeBus; 
		bus = b; 
		this.makeBusArg;
	}
	
	// returns boolean
	initBus { arg rate, numChannels;
				if(rate.isNil or: { rate === 'scalar' }) { ^true }; // this is no problem
				if(this.isNeutral) {
					this.defineBus(rate, numChannels);
					^true
				} {
					numChannels = numChannels ? this.numChannels;
					^(bus.rate === rate) and: { numChannels <= bus.numChannels }
				};
	}
	
	defineBus { arg rate=\audio, numChannels;
		if(numChannels.isNil, {
			numChannels = if(rate === \audio) { 
								this.class.defaultNumAudio 
							} {
								this.class.defaultNumControl							}
		});
		this.freeBus;
		bus = Bus.alloc(rate, server, numChannels);
		this.makeBusArg;
	}
	
	freeBus {
		if(bus.notNil, {
			if(bus.rate === \control) { bus.setAll(0) }; // clean up
			bus.free;
		});
		bus = nil;
		this.makeBusArg;
	}
		
	makeBusArg { 	
			var index, numChannels;
			busArg = if(bus.isNil or: { bus.rate === 'audio' }) // audio buses can't be 
					{ "" } {						// used for control mapping
						index = this.index;
						numChannels = this.numChannels;
						if(numChannels == 1) 
							{ ("\c" ++ index) } 
							{
							Array.fill(numChannels, { arg i; "\c" ++ (index + i) })
							}
					};      
	}
	wakeUpToBundle {}
	wakeUp {}
	asBus { ^if(this.isNeutral) { nil } { bus } }
	
		
	
	////////////  playing and access  /////////////////////////////////////////////
	
	
	rate {  ^if(bus.isNil) { \scalar } { bus.rate } }
	numChannels {  ^if(bus.isNil) { nil } { bus.numChannels } }
	index { ^if(bus.isNil) { nil } { bus.index } }
	
	
	isNeutral {
		^bus.isNil or: { bus.index.isNil and: { bus.numChannels.isNil } }
	}
	
	prepareOutput { } // see subclass
	clock { ^nil  }

	ar { arg numChannels, offset=0;
		if(this.isNeutral) { 
			this.defineBus(\audio, numChannels) 
		};
		this.prepareOutput;
		^InBus.ar(bus, numChannels ? bus.numChannels, offset)
	}
	
	kr { arg numChannels, offset=0;
		if(this.isNeutral) { 
			this.defineBus(\control, numChannels) 
		};
		this.prepareOutput;
		^InBus.kr(bus, numChannels ? bus.numChannels, offset)
	}
	
	
	
	// embed bus in event streams
	
	embedInStream { arg inval;
					if(inval.notNil) {
						if(this.isPlaying.not) {
							if(this.isNeutral) { this.defineBus(\control, 1) }; 
							this.wakeUp 
						};// if in event stream yield bus arg, else a normal stream
						busArg.yield;
					} { this.yield  }
					^inval
	}
	asStream  {
			^Routine { arg inval;
				loop { inval = this.embedInStream(inval) }
			}
	}
		
	
	/////  math support  /////////
	
	value { arg something; 
		var n;
		something !? {Ên = something.numChannels };
		^if(something.rate == 'audio') { this.ar(n) } { this.kr(n) }  
	}
	
	composeUnaryOp { arg aSelector;
		^UnaryOpPlug.new(aSelector, this)
	}
	composeBinaryOp { arg aSelector, something;
		^BinaryOpPlug.new(aSelector, this, something)
	}
	reverseComposeBinaryOp { arg aSelector, something;
		^BinaryOpPlug.new(aSelector, something, this)
	}
	composeNAryOp { arg aSelector, anArgList;
		^thisMethod.notYetImplemented
		//^NAryOpPlug.new(aSelector, [this]++anArgList) // nary op ugens are not yet implemented
	}
	
	
	
	///// monitoring //////////////
	
	
	play { arg out, numChannels, group, multi=false, vol, fadeTime;  
		var ok, homeServer, bundle;
		
		homeServer = this.homeServer; // multi client support: monitor only locally
		if(homeServer.serverRunning.not, { "server not running".inform; ^nil });
		bundle = MixedBundle.new;
		this.initBus(\audio, numChannels);
		if(this.rate !== 'audio') { Error("can't monitor a control rate proxy").throw };
		if(this.isPlaying.not) { this.wakeUpToBundle(bundle); };
		if(monitor.isNil) { monitor = Monitor.new };
		group = (group ? homeServer).asGroup;
		monitor.playToBundle(bundle, bus.index, bus.numChannels, out, numChannels, 
				group, multi, vol, fadeTime);
		bundle.schedSend(homeServer, this.clock, this.quant)
		^monitor.group
	}
	
	fadeTime { ^0.02 }
	quant { ^nil }
	vol { ^if(monitor.isNil) { 1.0 }{ monitor.vol } }
	vol_ { arg val; if(this.rate === 'audio') {
						if(monitor.isNil) { monitor = Monitor.new }; monitor.vol = val 
				}
	}
	monitorIndex { ^if(monitor.isNil) { nil }{ monitor.out } }
	monitorGroup { ^if(monitor.isNil) { nil } { monitor.group } }
	
	stop { arg fadeTime=0.1;
		monitor.stop(fadeTime)
	}
	
	scope { arg bufsize = 4096, zoom; if(this.isNeutral.not) { ^bus.scope(bufsize, zoom) } }
	
	record { arg path, headerFormat="aiff", sampleFormat="int16", numChannels;
		var rec;
		if(server.serverRunning.not) { "server not running".inform; ^nil };
		rec = RecNodeProxy.newFrom(this, numChannels);
		rec.open(path, headerFormat, sampleFormat);
		rec.record;
		^rec
	}
	
	// shared node proxy support
	
	shared { ^false } 
	homeServer { ^server }
	
	printOn { arg stream;
		stream 	<< this.class.name << "." << bus.rate << "(" 
				<< server << ", " << bus.numChannels <<")";
	}
	
	
}




//////////////////////////////////////////////////////////////
// a bus plug that holds synths or client side players ///////


NodeProxy : BusPlug {


	var <group, <objects, <nodeMap;	
	var <loaded=false, <>awake=true, <>paused=false;
	var <>clock, <>quant;
	classvar <>buildProxyControl;
	
	
	*new { arg server, rate, numChannels, inputs;
		var res;
		res = super.new(server).init;
		res.initBus(rate, numChannels);
		inputs.do { arg o; res.add(o) };
		^res
	}
	
	init {
		nodeMap = ProxyNodeMap.new; 
		objects = Order.new;
		loaded = false;
	}
	
	clear { arg fadeTime=0;
		this.free(fadeTime, true); 	// free group and objects
		this.removeAll; 			// take out all objects
		this.stop(fadeTime);		// stop any monitor
		this.freeBus;	 // free the bus from the server allocator 
		this.init;	// reset the environment
	}
	
	end { arg fadeTime;
		var dt;
		dt = fadeTime ? this.fadeTime;
		Routine { this.free(dt, true); (dt + server.latency).wait; this.stop(0);  } .play;
	}
	
	pause {
		if(this.isPlaying) { objects.do { |item| item.pause(clock, quant) } };
		paused = true;
	}
	
	resume {
		paused = false;
		if(this.isPlaying) { objects.do { |item| item.resume(clock, quant) } };
	}
	
	fadeTime_ { arg t;
		this.set(\fadeTime, t);
	}
	fadeTime {
		^nodeMap.at(\fadeTime).value ? 0.02;
	}
	prFadeTime { ^nodeMap.at(\fadeTime).value }
	
	asGroup { ^group.asGroup }
	asTarget { ^group.asGroup }
	
		
		
	//////////// set the source to anything that returns a valid ugen input ////////////
	
	add { arg obj, channelOffset=0, extraArgs;
		this.put(objects.indices.last ? -1 + 1, obj, channelOffset, extraArgs)
	}
	
	source_ { arg obj;
		this.put(nil, obj, 0)
	}
	
	at { arg index;  ^objects.at(index) }
	
	source { ^if(objects.size == 1) { objects.at(0).source } { objects.array.collect(_.source) } }
	
	
	put { arg index, obj, channelOffset = 0, extraArgs; 			var container, bundle, orderIndex;
			
			if(obj.isNil) { this.removeAt(index); ^this };
			
			orderIndex = index ? 0;
			if(obj.isSequenceableCollection) {
				channelOffset = channelOffset.asArray;
				if(index.isSequenceableCollection) {
					obj.do { |item, i| this.put(index.wrapAt(i), item, channelOffset.wrapAt(i)) } 
				}{
					obj.do { |item, i| this.put(i + orderIndex, item, channelOffset.wrapAt(i)) }				}
				^this 
			};
			if(index.isSequenceableCollection) { 
				obj = obj.asArray;
				channelOffset = channelOffset.asArray;
				index.do { |index, i| this.put(index, obj.wrapAt(i),channelOffset.wrapAt(i)) } 
				^this	
			};
			
			container = obj.makeProxyControl(channelOffset, this);
			container.build(this, orderIndex); // bus allocation happens here
			
			if(this.shouldAddObject(container, index)) {
				bundle = MixedBundle.new;
				if(index.isNil) 
					{ this.removeAllToBundle(bundle) }
					{ this.removeToBundle(bundle, index) };
				objects = objects.put(orderIndex, container);
			} { 
				Error("failed to add object to node proxy.").throw;
				^this 
			};
			
			if(server.serverRunning) {
				container.loadToBundle(bundle, server);
				loaded = true;
				if(awake) {
					this.prepareToBundle(nil, bundle);
					container.wakeUpParentsToBundle(bundle);
					this.sendObjectToBundle(bundle, container, extraArgs, index);
				};
				bundle.schedSend(server, clock, quant);
			} {
				loaded = false;
			};

	}
	
	putSeries { arg first, second, last, value;
		last = last ?? {objects.size - 1};
		this.put((first, second..last), value) 
	}
	
	shouldAddObject { arg obj; ^obj.readyForPlay } // shared node proxy overrides this
	
	removeLast { this.removeAt(objects.indices.last) }
	removeAll { this.removeAt(nil) }
	removeAt { arg index; 
				var bundle;
				bundle = MixedBundle.new; 
				if(index.isNil) 
					{ this.removeAllToBundle(bundle) }
					{ this.removeToBundle(bundle, index) };
				bundle.schedSend(server); 
	}
	
	lag { arg ... args;
		nodeMap.setRates(args);
		this.rebuild;
	}
	
	setRates { arg ... args;
		nodeMap.setRates(args);
		this.rebuild;
	}
			
	defineBus { arg rate=\audio, numChannels;
		super.defineBus(rate, numChannels);
		this.linkNodeMap;
	}
	
	linkNodeMap {
		var index;
		index = this.index;
		if(index.notNil) { nodeMap.set(\out, index, \i_out, index) };
		nodeMap.proxy = this;
	}
	
	bus_ { arg inBus;
		if(server != inBus.server) { Error("can't change the server").throw };
		super.bus_(inBus);
		this.linkNodeMap;
		this.rebuild;
	}
		
	group_ { arg agroup;
		var bundle;
		if(agroup.server !== server, { Error("cannot move to another server").throw });
		NodeWatcher.register(agroup.isPlaying_(true)); // assume it is playing
		if(this.isPlaying)
		{ 	bundle = MixedBundle.new;
			this.stopAllToBundle(bundle); 
			group = agroup;
			this.sendAllToBundle(bundle); 
			bundle.schedSend(server, clock, 0.0);
		} { group = agroup };
	}
	
	// applying the settings to nodes //
	
	nodeMap_ { arg map;
		this.setNodeMap(map, false)
	}
	
	setNodeMap { arg map, xfade=true;
		var bundle, old, fadeTime;
		map.set(\fadeTime, this.fadeTime); // keep old fadeTime
		bundle = MixedBundle.new;
		old = nodeMap;
		nodeMap = map;
		this.linkNodeMap;
		if(this.isPlaying) {
			if(xfade) { this.sendEach(nil,true) }
			{
			old.unsetArgsToBundle(bundle, group); // unmap old
			nodeMap.addToBundle(bundle, group); // map new
			bundle.schedSend(server, clock, quant);
			}
		};
	}
	
	fadeToMap { arg map, interpolKeys;
		var fadeTime, values;
		fadeTime = this.fadeTime; // todo: old args
		if(interpolKeys.notNil) { 
			values = interpolKeys.asCollection.collect { arg item; map[item].value };
			this.lineAt(interpolKeys, values) 
		};
		nodeMap = map.copy;
		nodeMap.set(\fadeTime, fadeTime);
		this.linkNodeMap;
		if(this.isPlaying) { this.sendEach(nil,true) };
	}
	
	rebuild {
		var bundle;
		if(this.isPlaying) {
			bundle = MixedBundle.new;
			this.stopAllToBundle(bundle);
			bundle.schedSend(server, clock, quant);
			bundle = MixedBundle.new;
			loaded = false;
			this.loadToBundle(bundle);
			this.sendAllToBundle(bundle);
			bundle.schedSend(server, clock, quant);
		} {
			loaded = false;
		};
	
	}
	
	controlNames {
		var all; // Set doesn't work, because equality undefined for ControlName
		all = Array.new;
		objects.do { |el|
			var cn;
			cn = el.controlNames.reject { |item| 
				all.any { |cn| cn.name === item.name }
			};
			all = all.addAll(cn) 
		};
		^all
	}
	
	// derive names and default args from synthDefs
	supplementNodeMap { arg keys, replaceOldKeys=false;
		var controlNames, excluded;
		controlNames = this.controlNames;
		excluded = #[\out, \i_out, \gate];
		controlNames.do { |el|
					var key;
					key = el.name; 
					if (
						excluded.includes(key).not 
						and: { replaceOldKeys or: { nodeMap.at(key).isNil } }
						and: { keys.isNil or: { keys.includes(key) } }
					) { nodeMap.set(key, el.defaultValue) }
		}

	}
	
	/*
	supplementNodeMapFromServer { arg keys;
			controlNames = this.controlNames;
	
	}
	*/
	
	generateUniqueName {
			^server.clientID.asString ++ this.identityHash.abs
	}
	
	
	
	/////////// filtering within one proxy /////////////////
	
	filter { arg i, func; this.put(i, \filter -> func)	}
	
	/////////// shortcuts for efficient bus input //////////////
	
	readFromBus { arg busses;
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
	
	read { arg proxies;
		proxies = proxies.asCollection;
		proxies.do { arg item; item.wakeUp };
		this.readFromBus(proxies)
	}
	
	
	/////// send and spawn //////////
	
	
	getBundle {
		var bundle;
		bundle = 	MixedBundle.new; 
		this.prepareToBundle(nil, bundle);
		^bundle
	}
		
	spawn { arg extraArgs, index=0;
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
	
	
	send { arg extraArgs, index, freeLast=true;
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
	
	sendAll { arg extraArgs, freeLast=true;
		this.send(extraArgs, nil, freeLast);
	}
	
	sendEach { arg extraArgs, freeLast=true;
			var bundle;
			bundle = this.getBundle;
			if(freeLast, { this.stopAllToBundle(bundle) });
			this.sendEachToBundle(bundle, extraArgs);
			bundle.schedSend(server);
	
	}
	
	wakeUp { 	if(this.isPlaying.not) { this.deepWakeUp } } // do not touch internal state if playing
	
	deepWakeUp {
				var bundle;
				bundle = MixedBundle.new;
				this.wakeUpToBundle(bundle);
				bundle.schedSend(server, clock, quant)
	}
		
		
	/////// append to bundle commands
	
	
	defaultGroupID { ^server.nextNodeID } //shared proxy support
	
	prepareToBundle { arg argGroup, bundle;
		if(this.isPlaying.not) {
				group = Group.basicNew(server, this.defaultGroupID);
				NodeWatcher.register(group);
				group.isPlaying = server.serverRunning;
				bundle.add(group.newMsg(argGroup ? server.asGroup, \addToHead));
		}
	}

	//apply the node map settings to the entire group
	sendAllToBundle { arg bundle, extraArgs;
				objects.do { arg item;
					item.playToBundle(bundle, extraArgs.value, this);
				};
				if(objects.notEmpty) { nodeMap.addToBundle(bundle, group) };
	}
	
	//apply the node map settings to each synth separately
	sendEachToBundle { arg bundle, extraArgs;
				objects.do { arg item;
					this.sendObjectToBundle(bundle, item, extraArgs.value)
				};
	}
	
	//send single object
	sendObjectToBundle { arg bundle, object, extraArgs, index;
				var synthID, target, nodes;
				synthID = object.playToBundle(bundle, extraArgs.value, this);
				if(synthID.notNil) {
					if(index.notNil) { // if nil, all are sent anyway
					// make list of nodeIDs following the index
					nodes = Array(4);
					objects.doFrom({ arg obj, i; 
						var id; id = obj.nodeID;
						if(id.notNil) { nodes = nodes ++ id ++ synthID };
					}, index + 1);
					if(nodes.size > 0) { bundle.add(["/n_before"] ++ nodes.reverse) };
				};
				nodeMap.addToBundle(bundle, synthID)
				};
	}
	
	removeToBundle { arg bundle, index;
		var obj, dt, playing;
		playing = this.isPlaying;
		obj = objects.removeAt(index);
		if(obj.notNil) { 
				dt = this.fadeTime;
				if(playing) { obj.stopToBundle(bundle, dt) };
				obj.freeToBundle(bundle, dt);
		};
		
	}
	
	removeAllToBundle { arg bundle;
		var dt, playing;
		dt = this.fadeTime;
		playing = this.isPlaying;
		objects.do { arg obj; 
				if(playing) { obj.stopToBundle(bundle, dt) }; 
				obj.freeToBundle(bundle, dt);
		};
		objects.makeEmpty;
	}
	
	stopAllToBundle { arg bundle;
		var obj, dt;
		dt = this.fadeTime;
		if(this.isPlaying) { objects.do { arg obj; obj.stopToBundle(bundle, dt) } };
	}
	
	reallocBus { // this is called only when the server was not booted on creation.
		if(bus.notNil) { 
			if(loaded.not) {
				bus.realloc; 
				this.linkNodeMap
			} {
			 "should not reallocate bus when already loaded.".warn
			} 
		};
	}
	
	loadToBundle { arg bundle;
		this.reallocBus; // if server was rebooted 
		objects.do { arg item, i;
			item.build(this, i);
			item.loadToBundle(bundle, server);
		};
		loaded = true;
	}

	
	wakeUpToBundle { arg bundle, checkedAlready;
		if(checkedAlready.isNil) { checkedAlready = IdentitySet.new };
		if(checkedAlready.includes(this).not) {
			checkedAlready.add(this);
			this.wakeUpParentsToBundle(bundle, checkedAlready);
			if(loaded.not) { this.loadToBundle(bundle) };
			if(awake and: { this.isPlaying.not }) { 
				this.prepareToBundle(nil, bundle);
				this.sendAllToBundle(bundle)
			};
		};
		
	}
	
	wakeUpParentsToBundle { arg bundle, checkedAlready;
			objects.do{ arg item; item.wakeUpParentsToBundle(bundle, checkedAlready) };
			nodeMap.wakeUpParentsToBundle(bundle, checkedAlready);
	}
		
	// used in 'garbage collector'
	
	getFamily { arg set, alreadyAsked;
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

		
	
	////// private /////
	
	
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

 	
 	
	////////////behave like my group////////////
	
	isPlaying { ^group.isPlaying }
	
	free { arg fadeTime, freeGroup = true;
		var bundle;
		if(this.isPlaying) {	
			bundle = MixedBundle.new;
			if(fadeTime.notNil) { bundle.add([15, group.nodeID, "fadeTime", fadeTime]) };
			this.stopAllToBundle(bundle);
			if(freeGroup) { 
				bundle.addSchedFunction({ group.free }, 
					(fadeTime ? this.fadeTime) + server.latency); 
			};
			bundle.send(server);
		}
 	}
	
	release { arg fadeTime; this.free(fadeTime, false) }
	
	
	set { arg ... args; // pairs of keys or indices and value
		nodeMap.set(*args);
		if(this.isPlaying) { 
			server.sendBundle(server.latency, [15, group.nodeID] ++ args); 
		};
	}
	
	setn { arg ... args;
		nodeMap.set(*args);
		if(this.isPlaying) { 
			server.sendBundle(server.latency, group.setnMsg(*args)); 
		};
	}
	
	setGroup { arg args, useLatency=false;
		if(this.isPlaying) { 
			server.sendBundle(if(useLatency) { server.latency }, [15, group.nodeID] ++ args); 
		};
	}
	
	// map to a control proxy
		
	map { arg ... args; // key, proxy ... args; 
		var bundle;
		if(this.isPlaying) {
			bundle = List.new;
			nodeMap.unmapArgsToBundle(bundle, group.nodeID, args[0,2..args.size-2]);
			nodeMap.map(*args).updateBundle;
			nodeMap.addToBundle(bundle, group.nodeID);
			server.listSendBundle(server.latency, bundle);
		} { nodeMap.map(*args) }
	}
	
	mapn { arg ... args; // key, proxy ... args; proxy is multichannel
		var bundle;
		if(this.isPlaying) {
			bundle = List.new;
			nodeMap.unmapArgsToBundle(bundle, group.nodeID, args[0,2..args.size-2]);
			nodeMap.mapn(*args).updateBundle;
			nodeMap.addToBundle(bundle, group.nodeID);
			server.listSendBundle(server.latency, bundle);
		} { nodeMap.mapn(*args) }
	}
	
	
	// map to current environment
	mapEnvir { arg ... keys;
		nodeMap.mapEnvir(*keys);
		if(this.isPlaying) { 
			nodeMap.sendToNode(group);
		}
	}
	
		
	unset { arg ... keys;
		var bundle, all;
		all = keys.isEmpty;
		if(all) { 
			keys = nodeMap.settings.keys; 			
			keys.remove('out');
			keys.remove('i_out');  
		};
		if(this.isPlaying) {
			bundle = List.new;
			nodeMap.unsetArgsToBundle(bundle, group.nodeID, keys);
			server.listSendBundle(server.latency, bundle);
		};
		nodeMap.unset(*keys);
	}
	
	unmap { arg ... keys;
		var bundle;
		if(keys.isEmpty) { keys = nodeMap.mappingKeys };
		if(this.isPlaying) {
			bundle = List.new;
			nodeMap.unmapArgsToBundle(bundle, group.nodeID, keys);
			server.listSendBundle(server.latency, bundle);
		};
		nodeMap.unmap(*keys);
	}
	
	
	// xfades
	
	xset { arg ... args;
		this.xFadePerform(\set, args);
	}
	xmap { arg ... args;
		this.xFadePerform(\map, args);
	}
	xsetn { arg ... args;
		this.xFadePerform(\setn, args);
	}
	xmapn { arg ... args;
		this.xFadePerform(\mapn, args);
	}
	xunset { arg ... args;
		this.xFadePerform(\unset, args);
	}

	xFadePerform { arg selector, args;
		var bundle;
		nodeMap.performList(selector, args);
		if(this.isPlaying) 
		{ this.sendEach(nil, true) } 
		{ "not playing".inform }
	}
	
	
	
	//////////////   behave like my bus //////////////////
	
	gate { arg level=1.0, dur=0;
		if(bus.notNil && this.isPlaying, { bus.gate(level, dur, group) }) 
		{ "not playing".inform };
	}
	
	line { arg level=1.0, dur=1.0;
		if(bus.notNil && this.isPlaying, { group.freeAll; bus.line(level, dur, group) })
		{ "not playing".inform };
	}
	
	xline { arg level=1.0, dur=1.0;
		if(bus.notNil && this.isPlaying, { group.freeAll; bus.xline(level, dur, group) })
		{ "not playing".inform };
	}
	
	env { arg env;
		if(bus.notNil && this.isPlaying, { group.freeAll; bus.env(env) })
		{ "not playing".inform };
	}
		
	gateAt { arg key, level=1.0, dur=1.0;
		this.group.set(key, level); 
		SystemClock.sched(dur, { this.group.set(key, nodeMap[key].value); nil });
	}
	
	lineAt { arg key, level=1.0, dur;
		this.performAtControl(\line, key, level, dur);
	}
	
	xlineAt { arg key, level=1.0, dur;
		this.performAtControl(\xline, key, level, dur);
	}
	
	// private
	performAtControl { arg action, keys, levels=1.0, durs;
		var ctlBus, bundle, id, setArgs, setBundle, ctlIndex, missing;
		if(this.isPlaying) {
			durs = durs ? this.fadeTime;
			id = group.nodeID;
			keys = keys.asArray; levels = levels.asArray; durs = durs.asArray;

			ctlBus = Bus.alloc('control', server, keys.size);
			ctlIndex = ctlBus.index;
			bundle = ["/n_map", id];
			keys.do { arg key, i; bundle = bundle.addAll([key, ctlIndex + i]) };
			missing = keys.select { arg key; nodeMap[key].isNil };
			if(missing.notEmpty) { 
				this.supplementNodeMap(missing); 
				nodeMap.addToBundle(bundle, group) 
			};
			server.sendBundle(server.latency, bundle);
				
			ctlBus.perform(action, levels, durs);
			ctlBus.free;
				
			setArgs = [keys, levels].flop.flat;
				// set the node map
			nodeMap.set(*setArgs);
				// finally set it to that vealue
			server.sendBundle(server.latency + durs.maxItem, 
					 ["/n_map", id] ++ [keys, -1].flop.flat,
					 ["/n_set", id] ++ setArgs
				);
		} { "not playing".inform };
	}
	
	
}



Ndef : NodeProxy {
	classvar <>defaultServer;
	var key;
	*new { arg key, object, server;
		var res;
		server = server ? defaultServer ? Server.default;
		res = this.at(server, key);
		if(res.isNil, {
			res = super.new(server).toLib(key);
		});
		if(object.notNil, { res.source = object });
		^res;
	}
	*clear {
		Library.at(this).do { arg item; item.do { arg item; item.clear } };
	}
	
	toLib { arg key;
		Library.put(this.class, server, key, this);
	}
	*at { arg server, key;
		^Library.at(this, server, key)
	}
}




SharedNodeProxy : NodeProxy { // todo: should pass in a bus index/numChannels.
	var <constantGroupID;
	
	*new { arg broadcastServer, groupID; // keep fixed group id (< 999)
		^super.newCopyArgs(broadcastServer).initGroupID(groupID).init	}
	
	shared { ^true }
	
	initGroupID { arg groupID;  
		constantGroupID = groupID;
		awake = true;
	}
	
	reallocBus {} // for now: just don't. server shouldn't be rebooted.
		
	homeServer { ^server.homeServer }
	
	generateUniqueName {
		^asString(constantGroupID)
	}
	
	shouldAddObject { arg obj, index;
			^if(index.notNil and: { index > 0 }) {
				"only one object per proxy in shared node proxy possible".inform;
				^false
			} {
				if(obj.distributable.not) { 
					"this type of input is not distributable in a shared node proxy".inform;
					false
				} {
					obj.readyForPlay 
				}
			}
	}

	// map to a control proxy
	map { arg key, proxy ... args;
		args = [key,proxy]++args;
		// check if any not shared proxy is passed in
		(args.size div: 2).do { arg i; 
			if(args[2*i+1].shared.not, { Error("shouldn't map a local to a shared proxy").throw }) 
		};
		nodeMap.map(*args);
		if(this.isPlaying) { nodeMap.sendToNode(group) }
	}
	
	mapEnvir {}
	
	// use shared node proxy only with functions that can release the synth.
	// this is checked and throws an error in addObj
	stopAllToBundle { arg bundle;
			bundle.add([ 15, constantGroupID, "gate", 0, "fadeTime", this.fadeTime ])
	}
	
	removeToBundle { arg bundle, index;
		this.removeAllToBundle(bundle);
	}
	
	removeAllToBundle { arg bundle;
		var dt, playing;
		dt = this.fadeTime;
		playing = this.isPlaying;
		if(playing) { this.stopAllToBundle(bundle) };
		objects.do { arg obj; obj.freeToBundle(bundle, dt) };
		objects.makeEmpty;
	}
	
	clear { this.free; }
	
	reallyClear { super.clear; }
	
	group_ {}
	bus_ {}
	
	

	///////////////////
	
	prepareToBundle { arg argGroup, bundle; // ignore ingroup
		if(this.isPlaying.not) {
				group = Group.basicNew(
					this.homeServer,   // NodeWatcher should know when local group stopped
					this.constantGroupID // but not care about any remote groups
				);
				group.isPlaying = true;
				NodeWatcher.register(group);
		};
		bundle.add([21, constantGroupID, 0, 1]); // duplicate sending is no problem
	}

}


