// a listener on a bus

BusPlug : AbstractFunction {
	
	var <server, <bus; 		
	var <monitorGroup;
	var <busArg = \; // cache for "/s_new" bus arg
	var <monitorVol = 1, <>onClear;
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
			}, [\ir, \ir]).writeDefFile;
		};
		2.do { arg i; i = i + 1;
			SynthDef.writeOnce("system_link_control_" ++ i, { arg out=0, in=16;
				var env;
				env = EnvGate.new;
				Out.kr(out, In.kr(in, i) * env) 
			}, [\ir, \ir]);
		}
	}
	
	
	clear { 
		this.free;
		this.stop;
		this.freeBus;
		this.doOnClear; 
	}
	doOnClear {
		onClear.do { arg item; item.value(this) };
		onClear = nil;
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
			bus.setAll(0); // clean up
			bus.free;
		});
		bus = nil;
		this.makeBusArg;
	}
		
	makeBusArg { 	
			var index, numChannels;
			busArg = if(bus.isNil or: { bus.rate === 'audio' }) // audio buses can't be 
					{ \ } {						// used for control mapping
						index = this.index;
						numChannels = this.numChannels;
						if(numChannels == 1) 
							{ asSymbol("\c" ++ index) } 
							{
							Array.fill(numChannels, { arg i; asSymbol("\c" ++ (index + i)) })
							}
					}          
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

	ar { arg numChannels, offset=0;
		if(this.isNeutral) { 
			this.defineBus(\audio, numChannels) 
		};
		this.prepareOutput;
		^InBus.ar(bus, numChannels ? this.class.defaultNumAudio, offset)
	}
	
	kr { arg numChannels, offset=0;
		if(this.isNeutral) { 
			this.defineBus(\control, numChannels) 
		};
		this.prepareOutput;
		^InBus.kr(bus, numChannels ? this.class.defaultNumControl, offset)
	}
	
	//////////// embedding bus in event streams, myself if within a normal stream
	
	embedInStream { arg inval;
					if(inval.notNil) { 
						if(this.isPlaying.not) {
							if(this.isNeutral) { this.defineBus(\control, 1) }; 
							this.wakeUp 
						};  
						busArg.yield;
					} { this.yield  } // if in event stream yield bus arg
					^inval
	}
	asStream  {
			^Routine.new { arg inval;
				loop { inval = this.embedInStream(inval) }
			}
	}
	
	
	///// lazy  math support  /////////
	
	value { arg something; 
		var n;
		n = something.numChannels;
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
	}
	
	// error catch
	
	writeInputSpec {
		"use .ar or .kr to use within a synth.".error; this.halt;
	}
	
	
	///// monitoring //////////////
	
	//play { arg group=0, atTime, bus, multi=false;
	play { arg busIndex=0, nChan, group, multi=false, vol;  
		var bundle, divider, n, localServer, ok;
		
		localServer = this.localServer; // multi client support
		if(localServer.serverRunning.not, { "server not running".inform; ^nil });
		group = (group ? localServer).asGroup;
		if(multi.not and: { monitorGroup.isPlaying })
		{ // maybe should warn if not same server
			if(monitorGroup.group !== group) { monitorGroup.moveToTail(group) };
			monitorGroup.set(\gate, 1); // if releasing, keep up.
			this.wakeUp;
			^monitorGroup 
		};
			
			ok = this.initBus(\audio, nChan);
			if(ok.not) { ("can't monitor a" + bus.rate + "proxy").error };
			this.wakeUp;

			n = bus.numChannels;
			nChan = nChan ? n;
			nChan = nChan.min(n);
			
			monitorVol = vol ? monitorVol;
			bundle = MixedBundle.new;
			if(monitorGroup.isPlaying.not) { 
				monitorGroup = Group.newToBundle(bundle, group, \addToTail);
				bundle.add("/n_set", monitorGroup.nodeID, \fadeTime, this.fadeTime);
				NodeWatcher.register(monitorGroup);
			};
			
			divider = if(nChan.even, 2, 1);
			(nChan div: divider).do { arg i;
			bundle.add([9, "system_link_audio_"++divider, 
					localServer.nextNodeID, 1, monitorGroup.nodeID,
					\out, busIndex + (i*divider), 
					\in, bus.index + (i*divider),
					\vol, monitorVol
					]);
			};
			bundle.send(localServer, localServer.latency);
		^monitorGroup
	}
	
	fadeTime  { ^0.02 }
	
	vol_ { arg invol;
		monitorVol = invol ? monitorVol;
		monitorGroup.set(\vol, monitorVol);
	}
	
	vol { ^monitorVol }
	
	toggle {
		if(monitorGroup.isPlaying) { this.stop } { this.play };
	}

	stop { arg fadeTime=0.1;
		if(monitorGroup.isPlaying) {
			monitorGroup.release(fadeTime);
			SystemClock.sched(fadeTime, { 
				monitorGroup.free;
				monitorGroup = nil;
			}); // revisit
		}
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
	localServer { ^server }
	
	printOn { arg stream;
		stream 	<< this.class.name << "." << bus.rate << "(" 
				<< server << ", " << bus.numChannels <<")";
	}
	
	
}




//////////////////////////////////////////////////////////////
// a bus plug that holds synths or client side players ///////


NodeProxy : BusPlug {


	var <group, <objects, <nodeMap;	
	var <loaded=false, <>awake=true, <>paused=false, <>clock; 	
	classvar <>buildProxyControl;
	
	
	*new { arg server, rate, numChannels, inputs;
		var res;
		res = super.new(server).init;
		res.initBus(rate, numChannels);
		inputs.do { arg o; res.add(o) };
		^res
	}
	
	init {
		nodeMap = ProxyNodeMap.new.proxy_(this); 
		objects = Order.new;
		loaded = false;
	}
	
	clear {
		this.free(0, true); // free group and objects
		this.stop(0);		// stop any monitor
		this.freeBus;	 // free the bus from the server allocator 
		this.init;	// reset the environment
		this.doOnClear; // if any dependants are to be freed, do it.
	}
	
	end { arg fadeTime;
		var dt;
		dt = fadeTime ? this.fadeTime;
		Routine.new { this.free(dt, true); (dt + server.latency).wait; this.stop(0);  } .play;
	}
	
	pause {
		objects.do { |item| item.pause(clock) };
		paused = true;
	}
	
	resume {
		paused = false;
		objects.do { |item| item.resume(clock) };
	}
	
	fadeTime_ { arg t;
		this.set(\fadeTime, t);
	}
	fadeTime {
		^nodeMap.at(\fadeTime).value ? 0.02;
	}
	
	asGroup { ^group.asGroup }
	asTarget { ^group.asGroup }
	
		
		
	//////////// set the source to anything that returns a valid ugen input ////////////
	
	add { arg obj, channelOffset=0, extraArgs;
		this.put(objects.indices.last ? 0 + 1, obj, channelOffset, extraArgs)
	}
	
	source_ { arg obj;
		if(obj.isSequenceableCollection) {
			obj.do { arg item, i; this.put(i, item) }
		} {
			this.put(nil, obj, 0)
		}
	}
	
		
	at { arg index;  ^objects.at(index) }
	
	
	put { arg index=0, obj, channelOffset = 0, extraArgs; 			var container, bundle, z;
			
			if(obj.isNil) { this.removeAt(index); ^this };			
			container = obj.makeProxyControl(channelOffset, this);
			container.build(this, index ? 0); // bus allocation happens here
			
			if(container.readyForPlay) {
				bundle = MixedBundle.new;
				this.addObject(bundle, container, index);
			} { 
				"failed to add to node proxy: numChannels and rate must match".inform;
				^this 
			};
			
			if(server.serverRunning) {
				container.loadToBundle(bundle);
				loaded = true;
				if(awake) { // revisit for tasks!
					this.prepareToBundle(nil, bundle);
					this.sendObjectToBundle(bundle, container, extraArgs, index);
				};
				bundle.schedSend(server, clock);
			} {
				loaded = false;
			};

	}
	
	addObject { arg bundle, obj, index;
			this.removeToBundleAt(bundle, index);
			objects = objects.put(index ? 0, obj);
	}
	
	removeLast { this.removeAt(objects.indices.last) }
	removeAll { this.removeAt(nil) }
	removeAt { arg index; 
				var bundle;
				bundle = MixedBundle.new; 
				this.removeToBundleAt(bundle, index);
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
		if(index.isNil) { "couldn't allocate bus".error; this.halt };
		nodeMap.set(\out, index, \i_out, index);
	}
	
	bus_ { arg inBus;
		if(server != inBus.server) { "can't change the server".inform; ^this };
		super.bus_(inBus);
		this.linkNodeMap;
		this.rebuild;
	}
		
	group_ { arg agroup;
		var bundle;
		if(agroup.server !== server, { "cannot move to another server".error; ^this });
		NodeWatcher.register(agroup.isPlaying_(true)); // assume it is playing
		if(this.isPlaying)
		{ 	bundle = MixedBundle.new;
			this.stopAllToBundle(bundle); 
			group = agroup;
			this.sendAllToBundle(bundle); 
			bundle.schedSend(server, clock);
		} { group = agroup };
	}
	
	// applying the settings to nodes //
	
	sendNodeMap { 
		var bundle;
		bundle = MixedBundle.new;
		nodeMap.addToBundle(bundle, group);
		bundle.schedSend(server, clock);
	}
	
	nodeMap_ { arg map; // keep fadeTime?
		nodeMap = map;
		this.linkNodeMap;
		if(this.isPlaying) { this.sendNodeMap };
	}
	
	fadeToMap { arg map, interpolKeys;
		var fadeTime, values;
		fadeTime = this.fadeTime;
		if(interpolKeys.notNil) { 
			values = interpolKeys.asCollection.collect { arg item; map.settings[item].value };
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
			this.stopAllToBundle(bundle, true);
			bundle.schedSend(server, clock);
			bundle = MixedBundle.new;
			this.loadToBundle(bundle);
			this.sendAllToBundle(bundle);
			bundle.schedSend(server, clock);
		} {
			loaded = false;
		};
	
	}
	
	// derive names and default args from synthDefs
	supplementNodeMap { arg keys;
		objects.do { |obj|
				var names, values;
				names = obj.controlNames;
				values = obj.controlValues;
				names.do { arg name,i;
					name = name.asSymbol; 
					if (nodeMap.at(name).isNil and: { keys.isNil or: { keys.includes(name) } })
						{ nodeMap.set(name, values.at(i)) }
				};
		};
	}		
	generateUniqueName {
			^asString(this.identityHash.abs)
	}
	
	
	///////////////////// tasks ////////////////////////
	
	// sends all objects, frees the last ones first.
	
	spawner { arg beats=1, argFunc;
		var dt;
		dt = beats.asStream;
		argFunc = if(argFunc.notNil) { argFunc.asArgStream } { #[] };
		//awake = true;
		this[-1] = Task.new { 
					inf.do { arg i;
						var t, args;
						if((t = dt.next).isNil, { nil.alwaysYield });
						args = argFunc.next(i, beats);
						this.send(args ++ [\i, i, \beats, thisThread.beats]); 
						t.wait; 
						}
					};
		//awake = false;
	}
	
	// spawns synth at index, assumes fixed time envelope
	
	gspawner { arg beats=1, argFunc, index=0;
		var dt;
		dt = beats.asStream;
		argFunc = if(argFunc.notNil) { argFunc.asArgStream } { #[] };
		index = index.loop.asStream;
		//awake = true;
		this[-1] = Task.new { 
					inf.do { arg i;	
						var t;
						if((t = dt.next).isNil, { nil.alwaysYield });
						this.spawn(
							argFunc.value(i, beats) ++ [\i, i, \beats, thisThread.beats], 
							index.next
						); 
						t.wait; 
						}
					};
		//awake = false;
	}
	//freeSpawn { awake = true; this.removeAt(-1) }
	
	
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
				[\in, item.index, \out, this.index]
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
		nodeMap.updateBundle;
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
				nodeMap.setToBundle(bundle, -1);
				nodeMap.mapToBundle(bundle, -1);
				bundle.schedSend(server);
			}
	}
	
	
	send { arg extraArgs, index, freeLast=true;
			var bundle, obj;
			if(index.isNil) { 
				bundle = this.getBundle;
				if(freeLast, { this.stopAllToBundle(bundle) });
				this.sendAllToBundle(bundle, extraArgs);
				bundle.schedSend(server);
			
			} {
				obj = objects.at(index);
				if(obj.notNil) {
					bundle = this.getBundle;
					if(freeLast, { obj.stopToBundle(bundle) });
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
	
	wakeUp { 	if(this.isPlaying.not) { this.deepWakeUp } }
	
	deepWakeUp {
				var bundle;
				bundle = MixedBundle.new;
				this.wakeUpToBundle(bundle);
				bundle.schedSend(server, clock)
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
				var synthID, target;
				synthID = object.playToBundle(bundle, extraArgs.value, this);
				if(synthID.notNil) {
					if(index.notNil) { // if nil, all are sent anyway
						// find next running synth
						target = objects.detect({ arg obj; obj.nodeID.notNil }, index + 1);
						if(target.notNil) { bundle.add(["/n_before", synthID, target.nodeID]) };
					};
				nodeMap.addToBundle(bundle, synthID)
				};
	}
	
	
	stopAllToBundle { arg bundle, freeTasks=false;
				var dt;
				dt = this.fadeTime;
				objects.do { arg item;
					item.stopToBundle(bundle, dt, freeTasks);
				};
	}
	
	stopToBundleAt { arg bundle, index;
				var obj;
				obj = objects.at(index);
				if(obj.notNil) { obj.stopToBundle(bundle) };
	}
	
	removeToBundleAt { arg bundle, index;
		var obj, dt;
		dt = this.fadeTime;
		if(index.isNil) { 
			objects.makeEmpty { arg obj; obj.stopToBundle(bundle, dt, true) }
		} {
			obj = objects.removeAt(index);
			if(obj.notNil) { obj.stopToBundle(bundle, dt, true) }
		}
	}
	
	loadToBundle { arg bundle;
		if(bus.notNil) { bus.realloc }; // if server was rebooted 
		objects.do { arg item, i;
			item.build(this, i);
			item.loadToBundle(bundle);
		};
		loaded = true;
	}

	
	wakeUpToBundle { arg bundle, checkedAlready;
		if(checkedAlready.isNil) { checkedAlready = IdentitySet.new };
		if(checkedAlready.includes(this).not) {
			checkedAlready.add(this);
			this.wakeUpParentsToBundle(bundle, checkedAlready);
			if(loaded.not) { this.loadToBundle(bundle) };
			if(this.isPlaying.not) { 
				this.prepareToBundle(nil, bundle);
				if(awake) { this.sendAllToBundle(bundle) }; // revisit for tasks!
			};
		};
		
	}
	
	wakeUpParentsToBundle { arg bundle, checkedAlready;
			objects.do { arg item; item.wakeUpParentsToBundle(bundle, checkedAlready) };
			nodeMap.wakeUpParentsToBundle(bundle, checkedAlready);
	}
	
	// used in 'garbage collector'
	
	getFamily { arg set, alreadyAsked;
		var parents;
		parents = IdentitySet.new;
		alreadyAsked = alreadyAsked ?? { IdentitySet.new };
		alreadyAsked.add(this);
		objects.do { arg obj; parents.addAll(obj.parents) };
		parents.addAll(nodeMap.parents);
		parents.do { arg proxy; 
			if(alreadyAsked.includes(proxy).not) { proxy.getFamily(parents, alreadyAsked) }
		};
		set.add(this);
		set.addAll(parents);
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
		if(child.notNil and: { child !== this }) { child.addParent(this) };
		^child.isPlaying;
	}
	
	
	////////////behave like my group////////////
	
	isPlaying { ^group.isPlaying }
	
	free { arg fadeTime, freeGroup = true;
		var bundle;
		if(this.isPlaying, {	
			bundle = MixedBundle.new;
			if(fadeTime.notNil) { bundle.add(["/n_set", group.nodeID, \fadeTime, fadeTime]) };
			this.stopAllToBundle(bundle, true);
			if(freeGroup) { 
				bundle.addSchedFunction({ group.free }, fadeTime ? this.fadeTime) 
			};
			bundle.send(server);
		})
 	}
	
	release { arg fadeTime; this.free(fadeTime, false) }
	
	
	set { arg ... args;
		nodeMap.set(*args);
		if(this.isPlaying) { 
			server.sendBundle(server.latency, [15, group.nodeID] ++ args); 
		};
	}
	
	setn { arg ... args;
		nodeMap.setn(*args);
		if(this.isPlaying) { group.setn(*args) };
	}
	
	// map to a control proxy
	map { arg key, proxy ... args;
		args = [key,proxy]++args;
		nodeMap.map(*args);
		if(this.isPlaying) { 
			nodeMap.updateBundle;
			server.sendBundle(server.latency, [14, group.nodeID] ++ nodeMap.mapArgs);
		}
	}
	
	// map to current environment
	mapEnvir { arg keys;
		nodeMap.mapEnvir(keys);
		if(this.isPlaying) { 
			nodeMap.sendToNode(group);
		}
	}
	
		
	unset { arg ... keys;
		if(keys.isEmpty, { keys = nodeMap.settingKeys });
		nodeMap.unset(*keys);
		if(this.isPlaying) { this.sendAll(nil, true) };
	}
	
	unmap { arg ... keys;
		if(keys.isEmpty) { keys = nodeMap.mappingKeys };
		nodeMap.unmap(*keys);
		if(this.isPlaying) {
			keys.do { arg key; group.map(key,-1) };
			nodeMap.sendToNode(group) 
		};
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
		var oldLevel;
		oldLevel = nodeMap.at(key).value; // revisit: maybe simply set group?
		this.set(key, level); 
		SystemClock.sched(dur, {
			if(oldLevel.notNil) { this.set(key, oldLevel) } { this.unset(key) }; 
			nil;
		});
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
			this.supplementNodeMap;
			missing = keys.select { arg key; nodeMap.settings[key].isNil };
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




// the server needs to be a BroadcastServer.
// this class takes care for a constant groupID.

// to test with patterns

SharedNodeProxy : NodeProxy {
	var <constantGroupID;
	
	*new { arg server, groupID;
		^super.newCopyArgs(server).initGroupID(groupID).init	}
	
	shared { ^true }
	
	initGroupID { arg groupID;  
		constantGroupID = groupID ?? { server.nextSharedNodeID }; 
		awake = true;
	}
	defaultGroupID { 
				postln("started new shared group" + constantGroupID);
				^constantGroupID;
	}
	
	localServer { ^server.localServer }
	
	generateUniqueName {
		^asString(constantGroupID)
	}
	
	addObject { arg bundle, obj, index, freeAll;
		if(obj.distributable) {
			super.addObject(bundle, obj, index, freeAll);
		} { "this type of input is not distributable in a shared node proxy".error; ^this.halt };
	}

	// map to a control proxy
	map { arg key, proxy ... args;
		args = [key,proxy]++args;
		// check if any not shared proxy is passed in
		(args.size div: 2).do { arg i; 
			if(args[2*i+1].shared.not, { "shouldn't map a local to a shared proxy".error; ^this }) 
		};
		nodeMap.map(*args);
		if(this.isPlaying) { nodeMap.sendToNode(group) }
	}
	
	mapEnvir {}
	
	// use shared node proxy only with functions that can release the synth.
	// this is checked and throws an error in addObj
	stopAllToBundle { arg bundle;
				bundle.add(["/n_set", constantGroupID, \gate, 0]);
	}
	stopToBundleAt { arg bundle, index;
				bundle.add(["/n_set", constantGroupID, \gate, 0]); // revisit: try indexed free later
	}
	group_ {}
	bus_ {}
	
	

	///////////////////
	
	prepareToBundle { arg argGroup, bundle; // ignore ingroup
		if(this.isPlaying.not) {
				group = Group.basicNew(
					this.localServer,   // NodeWatcher should know when local group stopped
					this.defaultGroupID // but not care about any remote groups
				);
				group.isPlaying = true;
				NodeWatcher.register(group);
				// this happens in sendAll and sendEach, should implement in sendObj
				// bundle.add(group.newMsg(argGroup ? server, \addToHead));
		};
		bundle.add(["/g_new", this.defaultGroupID]); // duplicate sending is no problem
	}
	


}


